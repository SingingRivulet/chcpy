#include "filereader.h"
#include "tone.h"
int main(int argc, char** argv) {
    if (argc < 4) {
        printf("%s tonemap.txt input output", argv[0]);
        return 0;
    }
    std::array<float, 12> count;
    auto tmap = chcpy::tone::createToneConfig(argv[1]);
    int index = 0;
    printf("调性种类：%d\n", tmap.size());
    auto out = fopen(argv[3], "w");
    if (out) {
        for (auto line : midiSearch::musicReader_3colume(argv[2])) {
            printf("第%d首\n", ++index);
            midiSearch::melody_t& melody = std::get<0>(line).melody;
            midiSearch::mu_melody_t mumelody;
            for (auto& it : count) {
                it = 0;
            }
            int id = 0;
            for (auto it : melody) {
                midiSearch::melody_t m;
                m.push_back(it);

                //提取和弦
                try {
                    auto& c = std::get<0>(line).chord.at(id / 4);
                    for (auto n : c) {
                        m.push_back(it);
                    }
                } catch (...) {
                }

                mumelody.push_back(m);
                ++id;
            }

            auto tones = chcpy::tone::getToneSection(tmap, mumelody);
            fprintf(out, "[");
            bool firstTone = true;
            for (auto& it : tones) {
                //printf("调性：%s 概率：%.3f 根音：%d 权重：%f\n",
                //       it.tone->name.c_str(),
                //       it.weight,
                //       it.tone->baseTone,
                //       it.aweight);
                //for (auto note : it.melody) {
                //    printf("%d ", note);
                //}
                //printf("\n");
                int count = it.melody.size() / 4;
                for (int i = 0; i < count; ++i) {
                    if (!firstTone) {
                        fprintf(out, ",");
                    }
                    if (it.tone == nullptr) {
                        fprintf(out, "c.major");
                    } else {
                        fprintf(out, "%s", it.tone->name.c_str());
                    }
                    firstTone = false;
                }
            }
            fprintf(out, "|%s\n", std::get<1>(line)->c_str());
            //printf("\n");
        }
        fclose(out);
    }
    return 0;
}