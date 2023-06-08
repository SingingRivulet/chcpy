#include <fstream>
#include <iostream>
#include "chcpystring.h"
#include "melody2chord.h"
bool tone_nh[] = {true, false, true, false, true, true, false, true, false, true, false, true};
bool tone_1245_M[] = {true, false, true, false, false, true, false, true, false, false, false, false};
bool tone_1245_m[] = {false, false, true, false, true, false, false, false, false, true, false, true};

using melody_t = std::vector<int>;
inline void str2melody(const chcpy::string& str, melody_t& melody) {
    auto notes_arr = str.replace("[", "").replace("]", "").split(",");
    for (auto it : notes_arr) {
        int note = it.toInt();
        melody.push_back(note);
    }
}

bool isStructChord(bool isMajor, const melody_t& chord) {
    try {
        if (isMajor) {
            if (!tone_1245_m[chord.at(0) % 12]) {
                return false;
            }
        } else if (!tone_1245_m[chord.at(0) % 12]) {
            return false;
        }
        for (auto it : chord) {
            if (it != 0) {
                if (!tone_nh[it % 12]) {
                    return false;
                }
            }
        }
    } catch (...) {
        return false;
    }
    return true;
}
float getChordDist(const melody_t& A, const melody_t& B) {
    if (A.size() == 0 || B.size() == 0) {
        return 0;
    }
    float sumDelta = 0.;
    for (auto& it_A : A) {
        int minDelta = 999;
        for (auto& it_B : B) {
            auto delta = std::abs((it_A % 12) - (it_B % 12));
            if (delta < minDelta) {
                minDelta = delta;
            }
        }
        sumDelta += minDelta;
    }
    return sumDelta / A.size();
}

int main(int argc, char** argv) {
    bool processError = false;
    if (argc < 3) {
        printf("%s input tonal", argv[0]);
        return 0;
    }
    melody_t sc;
    float disSum = 0;
    int chordNum = 0;

    std::ifstream infile_data(argv[1]);
    if (!infile_data) {
        printf("fail to open data:%s\n", argv[1]);
    }
    chcpy::string line_data;

    std::ifstream infile_tonal(argv[2]);
    if (!infile_tonal) {
        printf("fail to open tonal:%s\n", argv[1]);
    }
    chcpy::string line_tonal;

    while (std::getline(infile_data, line_data) && std::getline(infile_tonal, line_tonal)) {
        if (line_data.size() > 1 && line_tonal.size() > 1) {
            auto arr_data = line_data.trimmed().split("|");
            auto arr_tonal = line_tonal.trimmed().split("|");
            if (arr_data.size() >= 2 && arr_tonal.size() >= 3) {
                auto tonal = arr_tonal[1].split(".");
                std::cout << tonal.at(0) << std::endl;
                melody_t ch;
                str2melody(arr_data.at(1), ch);
                bool isMajor = false;
                int tone = 0;
                //转换调性
                if (isStructChord(isMajor, ch)) {
                    sc = ch;
                } else {
                    disSum += getChordDist(sc, ch);
                }
                ++chordNum;
            }
        }
    }

    printf("%f\n", disSum / chordNum);
    return 0;
}