#pragma once
#include <math.h>
#include <memory.h>
#include <omp.h>
#include <functional>
#include <map>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include "hmm.h"

namespace chcpy::hmm {

struct hmmv3_train_t {
    int32_t M;                                        //key数量
    int32_t N;                                        //val数量
    std::vector<std::vector<float>> A1;               //状态转移矩阵A[vid][vid]，格式[N,N]
    std::vector<std::vector<std::vector<float>>> A2;  //状态转移矩阵A[vid][vid][vid]，格式[N,N,N]
    std::vector<std::vector<float>> B1;               //发射矩阵，B[vid][kid]，格式[N,M]
    std::vector<std::vector<std::vector<float>>> B2;  //发射矩阵，B[vid][vid][kid]，格式[N,N,M]
    std::vector<float> P;                             //初始概率P[vid]，格式[N]
};
template <typename T>
concept hmmv3_train_c = requires(T a) {
    a.A1;
    a.B1;
    a.A2;
    a.B2;
    a.P;
    a.M;
    a.N;
};

struct hmmv3_predict_t {
    int32_t M;                                            //key数量
    int32_t N;                                            //val数量
    std::vector<std::vector<float>> A1_log;               //状态转移矩阵A[vid][vid]，格式[N,N]
    std::vector<std::vector<std::vector<float>>> A2_log;  //状态转移矩阵A[vid][vid][vid]，格式[N,N,N]
    std::vector<std::vector<float>> B1_log;               //发射矩阵，B[vid][kid]，格式[N,M]
    std::vector<std::vector<std::vector<float>>> B2_log;  //发射矩阵，B[vid][vid][kid]，格式[N,N,M]
    std::vector<float> P_log;                             //初始概率P[vid]，格式[N]
};
template <typename T>
concept hmmv3_predict_c = requires(T a) {
    a.A1_log;
    a.B1_log;
    a.A2_log;
    a.B2_log;
    a.P_log;
    a.M;
    a.N;
};

template <hmmv3_train_c T>
inline void save_text(T& self, const std::string& path) {
    auto fp = fopen(path.c_str(), "w");
    if (fp) {
        fprintf(fp, "%d %d\n", self.M, self.N);
        //A1
        for (int i = 0; i < self.N; ++i) {
            for (int j = 0; j < self.N; ++j) {
                float val = self.A1.at(i).at(j);
                if (val != 0.0f) {
                    fprintf(fp, "A %d %d %f\n", i, j, val);
                }
            }
        }
        //A2
        for (int i = 0; i < self.N; ++i) {
            for (int j = 0; j < self.N; ++j) {
                for (int k = 0; k < self.N; ++k) {
                    float val = self.A2.at(i).at(j).at(k);
                    if (val != 0.0f) {
                        fprintf(fp, "a %d %d %d %f\n", i, j, k, val);
                    }
                }
            }
        }
        //B1
        for (int i = 0; i < self.N; ++i) {
            for (int j = 0; j < self.M; ++j) {
                float val = self.B1.at(i).at(j);
                if (val != 0.0f) {
                    fprintf(fp, "B %d %d %f\n", i, j, val);
                }
            }
        }
        //B2
        for (int i = 0; i < self.N; ++i) {
            for (int j = 0; j < self.N; ++j) {
                for (int k = 0; k < self.M; ++k) {
                    float val = self.B2.at(i).at(j).at(k);
                    if (val != 0.0f) {
                        fprintf(fp, "b %d %d %d %f\n", i, j, k, val);
                    }
                }
            }
        }
        //P
        for (int i = 0; i < self.N; ++i) {
            float val = self.P.at(i);
            if (val != 0.0f) {
                fprintf(fp, "P %d %f\n", i, val);
            }
        }
        fclose(fp);
    }
}

template <hmmv3_train_c h>
inline void init(  //初始化
    h& self,
    int M,
    int N) {
    self.M = M;
    self.N = N;
    self.P = std::vector<float>(N, 0);
    self.A1 = std::vector<std::vector<float>>(N, std::vector<float>(N, 0));
    self.A2 = std::vector<std::vector<std::vector<float>>>(N, std::vector<std::vector<float>>(N, std::vector<float>(N, 0)));
    self.B1 = std::vector<std::vector<float>>(N, std::vector<float>(M, 0));
    self.B2 = std::vector<std::vector<std::vector<float>>>(N, std::vector<std::vector<float>>(N, std::vector<float>(M, 0)));
}

template <hmmv3_predict_c h>
inline void init(  //初始化
    h& self,
    int M,
    int N) {
    self.M = M;
    self.N = N;
    self.P_log = std::vector<float>(N, log_safe(0));
    self.A1_log = std::vector<std::vector<float>>(N, std::vector<float>(N, log_safe(0)));
    self.A2_log = std::vector<std::vector<std::vector<float>>>(N, std::vector<std::vector<float>>(N, std::vector<float>(N, log_safe(0))));
    self.B1_log = std::vector<std::vector<float>>(N, std::vector<float>(M, log_safe(0)));
    self.B2_log = std::vector<std::vector<std::vector<float>>>(N, std::vector<std::vector<float>>(N, std::vector<float>(M, log_safe(0))));
}

template <hmmv3_train_c T>
inline void load_text(T& self, const std::string& path) {
    auto fp = fopen(path.c_str(), "r");
    if (fp) {
        char buf[128];
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), fp);
        std::istringstream head(buf);
        int M, N;
        std::string mat;
        int x, y, z;
        float val;
        head >> M;
        head >> N;
        init(self, M, N);
        while (!feof(fp)) {
            bzero(buf, sizeof(buf));
            fgets(buf, sizeof(buf), fp);
            std::istringstream line(buf);
            line >> mat;
            if (mat == "A") {
                line >> x;
                line >> y;
                line >> val;
                self.A1.at(x).at(y) = val;
            } else if (mat == "B") {
                line >> x;
                line >> y;
                line >> val;
                self.B1.at(x).at(y) = val;
            } else if (mat == "a") {
                line >> x;
                line >> y;
                line >> z;
                line >> val;
                self.A2.at(x).at(y).at(z) = val;
            } else if (mat == "b") {
                line >> x;
                line >> y;
                line >> z;
                line >> val;
                self.B2.at(x).at(y).at(z) = val;
            } else if (mat == "P") {
                line >> x;
                line >> val;
                self.P.at(x) = val;
            }
        }
        fclose(fp);
    }
}

template <hmmv3_predict_c T>
inline void load_text(T& self, const std::string& path) {
    auto fp = fopen(path.c_str(), "r");
    if (fp) {
        char buf[128];
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), fp);
        std::istringstream head(buf);
        int M, N;
        std::string mat;
        int x, y, z;
        float val;
        head >> M;
        head >> N;
        init(self, M, N);
        while (!feof(fp)) {
            bzero(buf, sizeof(buf));
            fgets(buf, sizeof(buf), fp);
            std::istringstream line(buf);
            line >> mat;
            if (mat == "A") {
                line >> x;
                line >> y;
                line >> val;
                self.A1.at(x).at(y) = log_safe(val);
            } else if (mat == "B") {
                line >> x;
                line >> y;
                line >> val;
                self.B1.at(x).at(y) = log_safe(val);
            } else if (mat == "a") {
                line >> x;
                line >> y;
                line >> z;
                line >> val;
                self.A2.at(x).at(y).at(z) = log_safe(val);
            } else if (mat == "b") {
                line >> x;
                line >> y;
                line >> z;
                line >> val;
                self.B2.at(x).at(y).at(z) = log_safe(val);
            } else if (mat == "P") {
                line >> x;
                line >> val;
                self.P.at(x) = log_safe(val);
            }
        }
        fclose(fp);
    }
}

template <hmmv3_train_c h>
inline void train_process(h& self, const std::function<void(std::pair<int, int>&)>& getData) {
    int prev1_tag = -1;
    int prev2_tag = -1;

    std::pair<int, int> line;
    while (1) {
        getData(line);
        int wordId = line.first;
        int tagId = line.second;
        if (wordId < 0 || tagId < 0 || wordId >= self.M || tagId >= self.N) {
            break;
        }
        if (prev2_tag < 0 || prev2_tag >= self.N) {
            self.P[tagId] += 1;
            self.B1[tagId][wordId] += 1;
            self.B2[tagId][tagId][wordId] += 1;
        } else if (prev1_tag < 0 || prev1_tag >= self.N) {
            self.A1[prev1_tag][tagId] += 1;
            self.B1[tagId][wordId] += 1;
        } else {
            self.A1[prev1_tag][tagId] += 1;
            self.A2[prev2_tag][prev1_tag][tagId] += 1;
            self.B1[tagId][wordId] += 1;
            self.B2[prev1_tag][tagId][wordId] += 1;
        }
        prev2_tag = prev1_tag;
        prev1_tag = tagId;
    }
}

template <hmmv3_train_c h>
inline void train_end(h& self) {
    normalize(self.P);
    for (auto& it : self.A1) {
        normalize(it);
    }
    for (auto& it1 : self.A2) {
        for (auto& it : it1) {
            normalize(it);
        }
    }
    for (auto& it : self.B1) {
        normalize(it);
    }
    for (auto& it1 : self.B2) {
        for (auto& it : it1) {
            normalize(it);
        }
    }
}

//此方法很慢，慎用
template <hmmv3_predict_c h>
[[depracted]] inline void predict(   //维特比算法，获得最优切分路径
    const h& self,                   //hmm对象
    const melody_t& seq,             //seq须先用melody2seq预处理
    std::vector<int>& best_sequence  //输出
) {
    auto T = seq.size();
    constexpr auto log0 = -std::numeric_limits<float>::infinity();

    std::vector<std::vector<std::vector<float>>> dp(T, std::vector<std::vector<float>>(self.N, std::vector<float>(self.N, log0)));
    std::vector<std::vector<std::vector<float>>> ptr(T, std::vector<std::vector<float>>(self.N, std::vector<float>(self.N, 0)));

#pragma omp parallel for
    for (int x = 0; x < self.N; x++) {
        for (int y = 0; y < self.N; y++) {
            float val = self.P_log[x] + self.B1_log[x][seq.at(0)] +
                        self.A1_log[x][y] + self.B2_log[x][y][seq.at(1)];
            dp[1][x][y] = val;
            ptr[1][x][y] = -1;
        }
    }

    for (int i = 2; i < T; i++) {
        int num_3 = seq.at(i);
        auto& dpi = dp.at(i);
        auto& dp_last = dp.at(i - 1);
#pragma omp parallel for
        for (int j = 0; j < self.N; j++) {
#pragma omp parallel for
            for (int k1 = 0; k1 < self.N; k1++) {
                float min_val = log0;
                int min_path = -1;
                for (int k2 = 0; k2 < self.N; k2++) {
                    float val = dp_last[k2][j] + self.A2_log[k2][j][k1];
                    if (val > min_val) {
                        min_val = val;
                        min_path = k2;
                    }
                }
                dpi[j][k1] = min_val + self.B2_log[j][k1][num_3];
                ptr[i][j][k1] = min_path;
            }
        }
    }

    best_sequence.resize(T);
    //argmax 2d
    float min_val = 100000.0;
    int min_path_i = -1;
    int min_path_j = -1;
    auto& dpi = dp.at(T - 1);
    for (int i = 0; i < self.N; i++) {
        for (int j = 0; j < self.N; j++) {
            if (dpi[i][j] < min_val) {
                min_val = dpi[i][j];
                min_path_i = i;
                min_path_j = j;
            }
        }
    }
    best_sequence.at(T - 1) = min_path_j;
    best_sequence.at(T - 2) = min_path_i;

    for (int t = T - 1; t > 1; t--) {
        int min_path_k = ptr[t][min_path_i][min_path_j];
        best_sequence.at(t - 2) = min_path_k;
        min_path_j = min_path_i;
        min_path_i = min_path_k;
    }
}

}  // namespace chcpy::hmm