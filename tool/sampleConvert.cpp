#include <sys/stat.h>
#include <sys/types.h>
#include "filereader.h"
void convert(const char* out, const char* sample, bool shiftChord) {
    midiSearch::melody_t melody;
    midiSearch::chord_t chords;
    if (shiftChord) {
        chords.push_back(melody);
    }
    for (auto data : midiSearch::lineReader(sample)) {
        chcpy::string d(*data);
        auto arr = d.trimmed().split("|");
        if (arr.size() >= 2) {
            midiSearch::melody_t m, c;
            midiSearch::str2melody(arr.at(0), m);
            for (auto it : m) {
                melody.push_back(it);
            }
            if (arr.at(1) != "E") {
                midiSearch::str2melody(arr.at(1), c);
            }
            chords.push_back(c);
        }
    }
    mkdir(out, 0777);
    char outPath_full[256];
    char outPath_melody[256];
    snprintf(outPath_full, sizeof(outPath_full), "%s/full.txt", out);
    snprintf(outPath_melody, sizeof(outPath_melody), "%s/melody.txt", out);
    printf("full:%s\n", outPath_full);
    printf("melody:%s\n", outPath_melody);

    auto fp_full = fopen(outPath_full, "w");
    auto fp_melody = fopen(outPath_melody, "w");

    chcpy::stringlist melody_strlist, chords_strlist;
    for (auto& it : melody) {
        melody_strlist.push_back(chcpy::string::number(it));
    }
    auto melody_str = chcpy::join(melody_strlist, ",");
    if (fp_full) {
        fprintf(fp_full, "[%s]|[", melody_str.c_str());
    }
    if (fp_melody) {
        fprintf(fp_melody, "[%s]\n", melody_str.c_str());
    }
    for (auto& ch : chords) {
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

    if (fp_full) {
        fclose(fp_full);
    }
    if (fp_melody) {
        fclose(fp_melody);
    }
}
void convertMelody(const char* out, const char* sample) {
    midiSearch::melody_t melody;
    for (auto data : midiSearch::lineReader(sample)) {
        chcpy::string d(*data);
        auto arr = d.trimmed().split("|");
        if (arr.size() >= 1) {
            midiSearch::melody_t m;
            midiSearch::str2melody(arr.at(0), m);
            for (auto it : m) {
                melody.push_back(it);
            }
        }
    }
    chcpy::stringlist melody_strlist, chords_strlist;
    for (auto& it : melody) {
        melody_strlist.push_back(chcpy::string::number(it));
    }
    auto melody_str = chcpy::join(melody_strlist, ",");
    auto fp_melody = fopen(out, "w");
    if (fp_melody) {
        fprintf(fp_melody, "[%s]\n", melody_str.c_str());
        fclose(fp_melody);
    }
}
int main() {
    //convert("../outputs/sampleConvert/shift", "../outputs/sampleConvert/1.txt", true);
    //convert("../outputs/sampleConvert/noshift", "../outputs/sampleConvert/1.txt", false);
    convertMelody("../outputs/sampleConvert/melody.txt","../outputs/sampleConvert/2.txt");
    return 0;
}