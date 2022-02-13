#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <array>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include "cJSON.h"

namespace chcpy::tone {

template <typename T, int n>  //pearson相关系数
inline double pearson(const std::array<T, n>& inst1, const std::array<T, n>& inst2) {
    double pearson = n * std::inner_product(inst1.begin(), inst1.end(), inst2.begin(), 0.0) -
                     std::accumulate(inst1.begin(), inst1.end(), 0.0) *
                         std::accumulate(inst2.begin(), inst2.end(), 0.0);

    double temp1 = n * std::inner_product(inst1.begin(), inst1.end(), inst1.begin(), 0.0) -
                   pow(std::accumulate(inst1.begin(), inst1.end(), 0.0), 2.0);

    double temp2 = n * std::inner_product(inst2.begin(), inst2.end(), inst2.begin(), 0.0) -
                   pow(std::accumulate(inst2.begin(), inst2.end(), 0.0), 2.0);

    temp1 = sqrt(temp1);
    temp2 = sqrt(temp2);
    pearson = pearson / (temp1 * temp2);

    return pearson;
}

struct toneConfig {
    std::string name{};
    int baseTone = 0;
    std::array<float, 12> weights;
    inline double getPearson(const std::array<float, 12>& arr) const {
        return pearson<float, 12>(arr, weights);
    }
    inline bool set(const char* str) {
        bool haveBaseTone = false;
        bool haveWeights = false;
        bool haveName = false;
        auto obj = cJSON_Parse(str);
        if (obj) {
            if (obj->type == cJSON_Object) {  //如果是对象
                auto line = obj->child;       //开始遍历
                while (line) {
                    if (strcmp(line->string, "baseTone") == 0) {  //名称为baseTone
                        if (line->type == cJSON_Number) {         //类型为数字
                            baseTone = line->valueint;            //设置baseTone
                            haveBaseTone = true;
                        }
                    } else if (strcmp(line->string, "weights") == 0) {  //获取weights
                        if (line->type == cJSON_Array) {                //类型为数组
                            int index = 0;
                            auto prob = line->child;  //开始遍历
                            while (prob) {
                                if (prob->type == cJSON_Number) {  //类型为数字
                                    weights[index] = prob->valueint;
                                    haveWeights = true;
                                    ++index;
                                    if (index >= 12) {
                                        break;
                                    }
                                }
                                prob = prob->next;
                            }
                        }
                    } else if (strcmp(line->string, "name") == 0) {  //获取name
                        if (line->type == cJSON_String) {            //类型为str
                            name = line->valuestring;
                            haveName = true;
                        }
                    }
                    line = line->next;
                }
                cJSON_Delete(obj);
            }
        }
        return haveBaseTone && haveWeights && haveName;
    }
};

inline auto createToneConfig(const char* path) {
    std::vector<toneConfig> res;
    auto fp = fopen(path, "r");
    toneConfig item;

    char buf[1024];
    if (fp) {
        while (!feof(fp)) {
            bzero(buf, sizeof(buf));
            fgets(buf, sizeof(buf), fp);
            if (item.set(buf)) {
                res.push_back(item);
            }
        }
        fclose(fp);
    }
    return res;
}

}  // namespace chcpy::tone