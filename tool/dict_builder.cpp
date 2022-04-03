#include "filereader.h"
#include "seq2id.h"
int main() {
    chcpy::seq2id::dict_t seq_dic;
    chcpy::chord2id::dict_t chord_dic;
    chcpy::seq2id::dict_t seq_dic_simple;
    chcpy::chord2id::dict_t chord_dic_simple;
    for (auto data : midiSearch::musicReader_3colume("../data/3.txt")) {
        auto& m = std::get<0>(data);
        for (auto it : m.melody) {
            chcpy::seq2id::add(seq_dic_simple, std::vector<int>({it}));
        }
        for (auto& it : m.chord) {
            std::vector<chcpy::string> arr;
            for (auto n : it) {
                arr.push_back(chcpy::string::number(n));
            }
            chcpy::chord2id::add(chord_dic_simple, chcpy::join(arr, "-"));
        }
    }
    for (auto data : midiSearch::lineReader("../data/1.list")) {
        chcpy::string line = data->c_str();
        try {
            auto kv = line.split("|");
            auto melody_str = kv.at(0).trimmed().split(" ");
            auto chord = kv.at(1).trimmed().split(" ");
            chcpy::seq2id::melody_t melody, melody_vec;
            for (auto& it : chord) {
                chcpy::chord2id::add(chord_dic, it);
            }
            for (auto& it : melody_str) {
                melody.push_back(it.toInt());
            }
            for (auto seq : chcpy::seq2id::melody2seq(melody)) {
                chcpy::seq2id::add(seq_dic, *seq);
            }
        } catch (...) {
            printf("err:%s\n", data->c_str());
        }
    }
    for (auto data : midiSearch::lineReader("../data/2.list")) {
        try {
            chcpy::string line = data->c_str();
            auto kv = line.split("|");
            auto melody_str = kv.at(0).trimmed().split(" ");
            auto chord = kv.at(1).trimmed().split(" ");
            chcpy::seq2id::melody_t melody, melody_vec;
            for (auto& it : chord) {
                chcpy::chord2id::add(chord_dic, it);
            }
            for (auto& it : melody_str) {
                melody.push_back(it.toInt());
            }
            for (auto seq : chcpy::seq2id::melody2seq(melody)) {
                chcpy::seq2id::add(seq_dic, *seq);
            }
        } catch (...) {
            printf("err:%s\n", data->c_str());
        }
    }
    chcpy::seq2id::save(seq_dic, "../data/melodydict.txt");
    chcpy::chord2id::save(chord_dic, "../data/chorddict.txt");
    chcpy::seq2id::save(seq_dic_simple, "../data/melodydict_simple.txt");
    chcpy::chord2id::save(chord_dic_simple, "../data/chorddict_simple.txt");
    return 0;
}
