#define CHCPY_DEBUG
#include "chordNext.h"
#include "bayes.h"
#include "filereader.h"
#include "rtm2c.h"
int main() {
    chcpy::chordNext::dict_t dict;
    dict.load("../data/chordtimedict.txt");
    chcpy::bayes::bayes_predict_t model;
    model.load("../data/bayesmodel");
    chcpy::activeBuffer buffer;

    auto fp = fopen("../outputs/chordNext/output.txt", "w");
    midiSearch::melody_t lastChord;
    midiSearch::chord_t outChords;
    for (auto line : midiSearch::musicReader_2colume("test2.txt")) {
        auto& chord = std::get<1>(line);
        auto& melody = std::get<0>(line);
        outChords.clear();
        outChords.push_back({});
        for (auto note : chord) {
            lastChord = note;
            buffer.pushChord(note);
            int octave = lastChord.at(0) / 12;
            std::vector<std::string> realtime;
            std::vector<chcpy::activeBuffer::chordtime> res;
            buffer.buildRealtimeBuffer(realtime, res);

            for (auto& it : res) {
                printf("%s=%d\n", std::get<0>(it).c_str(), std::get<1>(it));
            }

            auto max_id = chcpy::chordNext::predictNext(dict, model, res);

            auto it = dict.id_chord.find(max_id);
            if (it != dict.id_chord.end()) {
                outChords.push_back(chcpy::rtmtc::str2chord(std::get<0>(it->second), octave));
            } else {
                outChords.push_back(lastChord);
            }
        }
        if (fp) {
            chcpy::stringlist melody_strlist, chords_strlist;
            for (auto& it : melody) {
                melody_strlist.push_back(chcpy::string::number(it));
            }
            fprintf(fp, "[%s]|[", chcpy::join(melody_strlist, ",").c_str());
            for (auto& ch : outChords) {
                chcpy::stringlist chord_list;
                for (auto& it : ch) {
                    chord_list.push_back(chcpy::string::number(it));
                }
                chords_strlist.push_back(
                    chcpy::string("[") +
                    chcpy::join(chord_list, ",") +
                    chcpy::string("]"));
            }
            fprintf(fp, "%s]\n", chcpy::join(chords_strlist, ",").c_str());
        }
    }

    if (fp) {
        fclose(fp);
    }

    //getchar();
    return 0;
}