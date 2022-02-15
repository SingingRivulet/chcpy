#include "tone.h"
#include "filereader.h"
int main() {
    std::array<float, 12> count;
    auto tmap = chcpy::tone::createToneConfig("../data/tonemap.txt");
    int index = 0;
    printf("调性种类：%d\n", tmap.size());
    for (auto line : midiSearch::lineReader("test.txt")) {
        printf("第%d首\n", ++index);
        midiSearch::melody_t melody;
        midiSearch::str2melody(line->c_str(), melody);
        for (auto& it : count) {
            it = 0;
        }

        auto tones = chcpy::tone::getToneSection(tmap, melody);
        for (auto& it : tones) {
            printf("调性：%s 概率：%.3f 根音：%d 权重：%f\n",
                   it.tone->name.c_str(),
                   it.weight,
                   it.tone->baseTone,
                   it.aweight);
            for (auto note : it.melody) {
                printf("%d ", note);
            }
            printf("\n");
        }
        printf("\n");
    }
    return 0;
}