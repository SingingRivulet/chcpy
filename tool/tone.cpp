#include "tone.h"
#include "filereader.h"
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
        for (auto line : midiSearch::lineReader(argv[2])) {
            printf("第%d首\n", ++index);
            midiSearch::melody_t melody;
            midiSearch::mu_melody_t mumelody;
            midiSearch::str2melody(line->c_str(), melody);
            for (auto& it : count) {
                it = 0;
            }
            for (auto it : melody) {
                mumelody.push_back(midiSearch::melody_t({it}));
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
                    fprintf(out, "%s", it.tone->name.c_str());
                    firstTone = false;
                }
            }
            fprintf(out, "|%s\n", line->c_str());
            //printf("\n");
        }
        fclose(out);
    }
    return 0;
}