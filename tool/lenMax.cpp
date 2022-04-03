#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <set>
#include <vector>
#include "filereader.h"
#include "melody2chord.h"
int main(int argc, char* argv[]) {
    if (argc < 3) {
        return 1;
    }
    struct music_t : public midiSearch::music {
        int count;
    };
    std::vector<music_t> musics;
    for (auto music : midiSearch::musicReader_3colume(argv[1])) {
        music_t m(std::get<0>(music));
        m.count = 0;
        for (auto it : m.melody) {
            if (it != 0) {
                ++m.count;
            }
        }
        musics.push_back(std::move(m));
    }
    std::sort(musics.begin(),
              musics.end(),
              [](const music_t& A, const music_t& B) {
                  return A.melody.size() > B.melody.size();
              });
    mkdir(argv[2], 0755);
    char outPath_full[256];
    char outPath_melody[256];
    snprintf(outPath_full, sizeof(outPath_full), "%s/full.txt", argv[2]);
    snprintf(outPath_melody, sizeof(outPath_melody), "%s/melody.txt", argv[2]);
    printf("full:%s\n", outPath_full);
    printf("melody:%s\n", outPath_melody);
    auto fp_full = fopen(outPath_full, "w");
    auto fp_melody = fopen(outPath_melody, "w");
    for (auto& it : musics) {
        chcpy::stringlist melody_strlist, chords_strlist;
        for (auto& it : it.melody) {
            melody_strlist.push_back(chcpy::string::number(it));
        }
        auto melody_str = chcpy::join(melody_strlist, ",");
        if (fp_full) {
            fprintf(fp_full, "%s|[%s]|[", it.name.c_str(), melody_str.c_str());
        }
        if (fp_melody) {
            fprintf(fp_melody, "%s|[%s]|[[]]\n", it.name.c_str(), melody_str.c_str());
        }
        for (auto& ch : it.chord) {
            chcpy::stringlist chord_list;
            for (auto& it : ch) {
                chord_list.push_back(chcpy::string::number(it));
            }
            chords_strlist.push_back(
                chcpy::string("[") +
                chcpy::join(chord_list, ",") +
                chcpy::string("]"));
        }
        if (fp_full) {
            fprintf(fp_full, "%s]\n", chcpy::join(chords_strlist, ",").c_str());
        }
    }
    if (fp_full) {
        fclose(fp_full);
    }
    if (fp_melody) {
        fclose(fp_melody);
    }
    return 0;
}