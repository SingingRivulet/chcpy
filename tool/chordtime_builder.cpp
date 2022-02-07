#include "filereader.h"
#include "seq2id.h"
std::string buildChordStr(const std::vector<int>& arr) {
    if (arr.empty()) {
        return "";
    }
    chcpy::stringlist ch_arr;
    for (auto note : arr) {
        ch_arr.push_back(note <= 0 ? "0" : chcpy::string::number(note % 12));
    }
    return chcpy::join(ch_arr, "-");
}
int chordIndex = 0;
std::map<std::string, int> chordmap;

template <typename T1>
void addChord(const T1& s) {
    auto it = chordmap.find(s);
    if (it == chordmap.end()) {
        ++chordIndex;
        chordmap[s] = chordIndex;
    }
}

template <typename T1, typename T2>
void processfile(T1 fn, T2 fp) {
    for (auto data : midiSearch::musicReader_3colume(fn)) {
        auto m = std::get<0>(data);
        std::string last;
        int count = 0;
        for (auto& it : m.chord) {
            auto str = buildChordStr(it);
            if (!str.empty()) {
                if (str != last) {
                    if (!last.empty()) {
                        auto chordstr = last + "=" + chcpy::string::number(count);
                        fprintf(fp, "%s ", chordstr.c_str());
                        addChord(chordstr);
                    }
                    count = 0;
                }
                ++count;
                last = str;
            }
        }
        if (count != 0 && !last.empty()) {
            auto chordstr = last + "=" + chcpy::string::number(count);
            fprintf(fp, "%s ", chordstr.c_str());
            addChord(chordstr);
        }
        fprintf(fp, "\n");
    }
}
int main() {
    {
        auto fp = fopen("../data/chordtime.txt", "w");
        if (fp) {
            processfile("../data/1.txt", fp);
            processfile("../data/2.txt", fp);
            fclose(fp);
        }
    }
    {
        auto fp = fopen("../data/chordtimedict.txt", "w");
        if (fp) {
            for (auto& it : chordmap) {
                fprintf(fp, "%s|%d\n", it.first.c_str(), it.second);
            }
            fclose(fp);
        }
    }
}