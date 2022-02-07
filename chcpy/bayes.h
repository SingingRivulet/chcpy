#pragma once
#include <memory.h>
#include <stdio.h>
#include <array>
#include <map>
#include <sstream>
#include <vector>
namespace chcpy::bayes {

template <int probnum = 5>
struct bayes_predict_t {
    std::array<std::map<int, std::map<int, float>>, probnum> prob;

    inline std::map<int, float> getProb(const std::array<int, probnum>& input) {
        bool first = true;

        //双缓冲
        std::map<int, float> res_1;
        std::map<int, float> res_2;
        auto res = &res_1;
        auto res_back = &res_2;

        for (size_t i = 0; i < probnum; ++i) {
            auto& prob_i = prob[i];
            auto prob_it = prob_i.find(input[i]);
            if (prob_it != prob_i.end()) {
                if (first) {
                    *res = prob_it->second;
                } else {
                    res_back->clear();
                    for (auto p : prob_it->second) {
                        //取交集
                        auto it = res->find(p.first);
                        if (it != res->end()) {
                            (*res_back)[p.first] = p.second * it->second;
                        }
                    }
                    auto tmp = res_back;
                    res_back = res;
                    res = tmp;
                }
                first = false;
            }
        }
        return *res;
    }
    inline void load(const char* path) {
        auto fp = fopen(path, "r");
        if (fp) {
            char buf[128];
            bzero(buf, sizeof(buf));
            fgets(buf, sizeof(buf), fp);
            while (!feof(fp)) {
                bzero(buf, sizeof(buf));
                fgets(buf, sizeof(buf), fp);
                std::istringstream line(buf);
                int i, j, k;
                float v;
                line >> i;
                line >> j;
                line >> k;
                line >> v;
                if (i >= 0 && i < probnum) {
                    prob.at(i)[j][k] = v;
                }
            }
            fclose(fp);
        }
    }
};

template <int probnum = 5>
struct bayes_train_t {
    std::array<std::map<int, std::map<int, float>>, probnum> count_c;
    std::array<std::map<int, float>, probnum> count_e;
    inline void add(const std::array<int, probnum>& input, int val) {
        for (size_t i = 0; i < probnum; ++i) {
            count_c[i][input[i]][val] += 1;
            count_e[i][input[i]] += 1;
        }
    }
    inline void normalize() {
        for (size_t i = 0; i < probnum; ++i) {
            auto& count_c_i = count_c[i];
            auto& count_e_i = count_e[i];
            float sum_e = 0;
            for (auto& it : count_e_i) {
                sum_e += it.second;
            }
            for (auto& it : count_e_i) {
                it.second /= sum_e;
            }
            for (auto& it_c : count_c_i) {
                float sum_c = 0;
                for (auto& it : it_c.second) {
                    sum_c += it.second;
                }
                auto it_e = count_e_i.find(it_c.first);
                if (it_e == count_e_i.end()) {
                    printf("找不到元素：i=%d e=%d\n", i, it_c.first);
                } else {
                    for (auto& it : it_c.second) {
                        //printf("归一化：i=%d e=%d v=%f c=%f sum=%f\n",
                        //       i, it_c.first, it_e->second, it.second, sum_c);
                        it.second /= (sum_c * it_e->second);
                    }
                }
            }
        }
    }
    inline void save(const char* path) {
        auto fp = fopen(path, "w");
        if (fp) {
            for (size_t i = 0; i < probnum; ++i) {
                auto& count_c_i = count_c[i];
                for (auto& it_i : count_c_i) {
                    for (auto& it_v : it_i.second) {
                        fprintf(fp, "%d %d %d %f\n", i, it_i.first, it_v.first, it_v.second);
                    }
                }
            }
            fclose(fp);
        }
    }
};

}  // namespace chcpy::bayes