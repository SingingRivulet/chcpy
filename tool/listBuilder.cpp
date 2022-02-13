#include <map>
#include <set>
#include <vector>
#include "filereader.h"
#include "melody2chord.h"
std::string buildChordStr(const std::vector<int>& arr, int baseChord, int baseTone) {
    if (arr.empty()) {
        return "null";
    }
    chcpy::stringlist ch_arr;
    for (auto note : arr) {
        ch_arr.push_back(note <= 0 ? "0" : chcpy::string::number(chcpy::melody2chord::getToneLevelDelta(note + 120, baseChord, baseTone) % 14));
    }
    return chcpy::join(ch_arr, "-");
}
void process(const char* path, const char* out) {
    chcpy::melody2chord::chordMap chordmap;
    std::vector<int> buffer_melody;
    std::vector<std::string> buffer_chord;
    std::map<std::vector<int>, std::vector<std::string> > datas;
    for (auto music : midiSearch::musicReader_3colume(path)) {
        auto m = std::get<0>(music);
        printf("%s melody:%d chord:%d\n", m.name.c_str(), m.melody.size(), m.chord.size());
        std::vector<int>& notes = m.melody;
        auto res = chcpy::melody2chord::getMusicSection(chordmap, notes, 4, 0.5, 1.0);
        int index = 0;
        for (auto it : res) {
            //printf("%d_%s(w=%f) ", it.chord_base, it.chord_name.c_str(), it.weight);
            buffer_melody.clear();
            buffer_chord.clear();
            for (auto note : it.melody) {
                int noteLevel = chcpy::melody2chord::getToneLevelDelta(note, it.chord_base, 0);
                buffer_melody.push_back(noteLevel);
                if (index % 4 == 0) {
                    try {
                        auto chord = m.chord.at(index / 4);
                        buffer_chord.push_back(buildChordStr(chord, it.chord_base, 0));
                    } catch (...) {
                        printf("err:%d\n", index);
                        buffer_chord.push_back(buildChordStr({}, it.chord_base, 0));
                    }
                }
                ++index;
            }
            if (!buffer_melody.empty()) {
                datas[buffer_melody] = buffer_chord;
            }
            //printf("\n");
        }
        //printf("\n");
    }
    auto fp = fopen(out, "w");
    if (fp) {
        for (auto line : datas) {
            for (auto note : line.first) {
                fprintf(fp, "%d ", note);
            }
            fprintf(fp, "|");
            for (auto ch : line.second) {
                fprintf(fp, "%s ", ch.c_str());
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }
}
int main() {
    process("../data/1.txt", "../data/1.list");
    process("../data/2.txt", "../data/2.list");
    return 0;
}