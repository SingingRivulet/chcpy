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

struct hmm_v3_t {
    int32_t M;                                        //key数量
    int32_t N;                                        //val数量
    std::vector<std::vector<float>> A1;               //状态转移矩阵A[vid][vid]，格式[N,N]
    std::vector<std::vector<std::vector<float>>> A2;  //状态转移矩阵A[vid][vid][vid]，格式[N,N,N]
    std::vector<std::vector<float>> B1;               //发射矩阵，B[vid][kid]，格式[N,M]
    std::vector<std::vector<std::vector<float>>> B2;  //发射矩阵，B[vid][vid][kid]，格式[N,N,M]
    std::vector<float> P;                             //初始概率P[vid]，格式[N]
};
template <typename T>
concept hmm_v3_c = requires(T a) {
    a.A1;
    a.B1;
    a.A2;
    a.B2;
    a.P;
    a.M;
    a.N;
};

template <hmm_v3_c T>
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

template <hmm_v3_c h>
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

template <hmm_v3_c T>
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

template <hmm_v3_c h>
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

template <hmm_v3_c h>
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

}  // namespace chcpy::hmm