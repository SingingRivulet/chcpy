#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#include <stdio.h>
#include <stdlib.h>
#include "hmm.h"
namespace chcpy {
namespace gpu {

inline void CHECK() {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("glGetError returns %d\n", err);
    }
}
class GPUContext {
   private:
    EGLContext context;
    EGLDisplay dpy;

   public:
    inline GPUContext() {
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (dpy == EGL_NO_DISPLAY) {
            printf("eglGetDisplay returned EGL_NO_DISPLAY.\n");
            return;
        }

        EGLint majorVersion;
        EGLint minorVersion;
        EGLBoolean returnValue = eglInitialize(dpy, &majorVersion, &minorVersion);
        if (returnValue != EGL_TRUE) {
            printf("eglInitialize failed\n");
            return;
        }

        EGLConfig cfg;
        EGLint count;
        EGLint s_configAttribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_NONE};
        if (eglChooseConfig(dpy, s_configAttribs, &cfg, 1, &count) == EGL_FALSE) {
            printf("eglChooseConfig failed\n");
            return;
        }

        EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
        context = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, context_attribs);
        if (context == EGL_NO_CONTEXT) {
            printf("eglCreateContext failed\n");
            return;
        }
        returnValue = eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, context);
        if (returnValue != EGL_TRUE) {
            printf("eglMakeCurrent failed returned %d\n", returnValue);
            return;
        }
    }
    inline GLuint loadShader(GLenum shaderType, const char* pSource) {
        GLuint shader = glCreateShader(shaderType);
        if (shader) {
            glShaderSource(shader, 1, &pSource, NULL);
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled) {
                GLint infoLen = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                if (infoLen) {
                    char* buf = (char*)malloc(infoLen);
                    if (buf) {
                        glGetShaderInfoLog(shader, infoLen, NULL, buf);
                        fprintf(stderr, "Could not compile shader %d:\n%s\n",
                                shaderType, buf);
                        free(buf);
                    }
                    glDeleteShader(shader);
                    shader = 0;
                }
            }
        }
        return shader;
    }
    inline GLuint createComputeProgram(const char* pComputeSource) {
        GLuint computeShader = loadShader(GL_COMPUTE_SHADER, pComputeSource);
        if (!computeShader) {
            return 0;
        }

        GLuint program = glCreateProgram();
        if (program) {
            glAttachShader(program, computeShader);
            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            if (linkStatus != GL_TRUE) {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength) {
                    char* buf = (char*)malloc(bufLength);
                    if (buf) {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        fprintf(stderr, "Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
        return program;
    }
    inline void setupSSBufferObject(GLuint& ssbo, GLuint index, float* pIn, GLuint count) {
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

        glBufferData(GL_SHADER_STORAGE_BUFFER, count * sizeof(float), pIn, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);
    }
    inline ~GPUContext() {
        eglDestroyContext(dpy, context);
        eglTerminate(dpy);
    }
};

//gpu端
constexpr char hmm_shader[] = R"(

#version 320 es

uniform int M;
uniform int N;
uniform int seq_i;//seq.at(i)

layout(local_size_x = 1) in;

layout(binding = 0) readonly buffer  Input0{
    float data[];
} A_log;//状态转移矩阵A[vid][vid]，格式[N,N]

layout(binding = 1) readonly buffer  Input1{
    float data[];
} B_log;//发射矩阵，B[vid][kid]，格式[N,M]

layout(binding = 2) readonly buffer Input2 {
    float data[];
} dp_last;//上一次的值

layout(binding = 3) writeonly buffer Output0 {
    float data[];
} output;

void main(){
    const float infinity = 1. / 0.;//定义无限大常量
    uint idx = gl_GlobalInvocationID.x;//对应cpu版的j
    float dpi_val = -infinity;
    int ptr_val = 0;
    float base = B_log.data[idx*N + seq_i];
    for (int k = 0; k < N; ++k) {
        float score = base + dp_last.data[k] + A_log.data[k*N + idx];
        if (!isinf(score) && score > dpi_val) {
            dpi_val = score;
            ptr_val = k;
        }
    }
    output.data[idx*2] = dpi_val;
    output.data[idx*2+1] = float(ptr_val);
}

)";

class hmm_t {
   private:
    float* A_log;
    GLuint A_log_gpu;
    GLuint A_log_size;

    float* B_log;
    GLuint B_log_gpu;
    GLuint B_log_size;

    GLuint dp_last_gpu;
    GLuint dp_last_size;

    GLuint output_gpu;
    GLuint output_size;

    GLuint computeProgram;

   public:
    float* dp_last;
    chcpy::hmm::hmm_predict_t* cpu;
    GPUContext* context;
    inline hmm_t(GPUContext* context, chcpy::hmm::hmm_predict_t* cpu) {
        this->cpu = cpu;
        this->context = context;

        CHECK();
        computeProgram = context->createComputeProgram(hmm_shader);  //创建gpu端程序
        CHECK();

        //创建GPU端变量
        A_log_size = cpu->N * cpu->N;
        A_log = new float[A_log_size];

        B_log_size = cpu->N * cpu->M;
        B_log = new float[B_log_size];

        dp_last_size = cpu->N;
        dp_last = new float[dp_last_size];

        output_size = cpu->N * 2;
    }
    inline void start() {
        context->setupSSBufferObject(A_log_gpu, 0, A_log, A_log_size);
        context->setupSSBufferObject(B_log_gpu, 0, B_log, B_log_size);
        context->setupSSBufferObject(dp_last_gpu, 0, dp_last, dp_last_size);
        context->setupSSBufferObject(output_gpu, 0, NULL, output_size);
    }
    inline void update() {
        glBufferData(GL_SHADER_STORAGE_BUFFER, dp_last_size * sizeof(float), dp_last, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dp_last_gpu);
    }
    inline void run(GLint seq_i, const std::function<void(float*)>& callback) {
        glUseProgram(computeProgram);

        //创建uniform变量
        int gpu_M = glGetUniformLocation(computeProgram, "M");
        glUniform1i(gpu_M, cpu->M);
        int gpu_N = glGetUniformLocation(computeProgram, "N");
        glUniform1i(gpu_N, cpu->N);
        int gpu_seq_i = glGetUniformLocation(computeProgram, "seq_i");
        glUniform1i(gpu_seq_i, seq_i);

        glDispatchCompute(cpu->N, 1, 1);  //执行
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        CHECK();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, output_gpu);
        float* pOut = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, output_size * sizeof(float), GL_MAP_READ_BIT);
        callback(pOut);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    inline ~hmm_t() {
        glDeleteProgram(computeProgram);
        delete[] A_log;
        delete[] B_log;
        delete[] dp_last;
    }
};

template <typename T>
concept hmm_c = requires(T a) {
    a.A_log;
    a.B_log;
    a.dp_last;
    a.update();
    a.start();
    a.cpu;
    a.run(GLint(), [](float*) {});
};

}  // namespace gpu

namespace hmm {

template <gpu::hmm_c h>
inline void predict(                 //维特比算法，获得最优切分路径
    const h& gpu,                    //hmm对象
    const melody_t& seq,             //seq须先用melody2seq预处理
    std::vector<int>& best_sequence  //输出
) {
#ifdef CHCPY_DEBUF
    clock_t startTime, endTime;
    startTime = clock();  //计时开始
#endif
    auto& self = *gpu.cpu;
    auto T = seq.size();
    constexpr auto log0 = -std::numeric_limits<float>::infinity();
    std::vector<std::vector<float>> dp(T, std::vector<float>(self.N, 0));
    std::vector<std::vector<int>> ptr(T, std::vector<int>(self.N, 0));

    for (size_t j = 0; j < self.N; ++j) {
        float startval = self.P_log.at(j) + self.B_log.at(j).at(seq.at(0));
        dp.at(0).at(j) = startval;
        gpu.dp_last[j] = startval;
    }
    gpu.start();  //初始化gpu
    for (size_t i = 1; i < T; ++i) {
        auto& dpi = dp.at(i);
        /*
        for (size_t j = 0; j < self.N; ++j) {
            dpi.at(j) = log0;
            float base = self.B_log.at(j).at(seq.at(i));
            for (size_t k = 0; k < self.N; ++k) {
                float score = base + dp.at(i - 1).at(k) + self.A_log.at(k).at(j);
                if (score != log0 && score > dpi.at(j)) {
                    dpi.at(j) = score;
                    ptr.at(i).at(j) = k;
                }
            }
        }
        */
        gpu.run(seq.at(i), [&](float* output) {
            for (size_t j = 0; j < self.N; ++j) {
                float dpi_val = output[j * 2];
                int ptr_val = output[j * 2 + 1];
                gpu.dp_last[j] = dpi_val;
                dpi.at(j) = dpi_val;
                ptr.at(i).at(j) = ptr_val;
            }
        });
        gpu.update();
    }
    best_sequence.resize(T);
    best_sequence.at(T - 1) = argmax(dp.at(T - 1).begin(), dp.at(T - 1).end());
    for (int i = T - 2; i >= 0; --i) {  //这里不能用size_t，否则将导致下溢出，造成死循环
        best_sequence.at(i) = ptr.at(i + 1).at(best_sequence.at(i + 1));
    }
#ifdef CHCPY_DEBUF
    endTime = clock();  //计时结束
    printf("\n用时%f秒\n", (float)(endTime - startTime) / CLOCKS_PER_SEC);
#endif
}

}  // namespace hmm
}  // namespace chcpy