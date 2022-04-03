#include "filereader.h"
#include "hmm.h"
#include "seq2id.h"
int main() {
    chcpy::seq2id::dict_t seq_dic;
    chcpy::chord2id::dict_t chord_dic;
    chcpy::seq2id::load(seq_dic, "../data/melodydict_simple.txt");
    chcpy::chord2id::load(chord_dic, "../data/chorddict_simple.txt");
    chcpy::hmm::hmm_train_t model;
    printf("M:%d N:%d\n", seq_dic.index, chord_dic.index);
    chcpy::hmm::init(model, seq_dic.index, chord_dic.index);
    for (auto data : midiSearch::musicReader_3colume("../data/3.txt")) {
        auto& m = std::get<0>(data);
        //4:1
        int count = 0;
        std::vector<std::pair<int, int>> seq;
        for (auto chord : m.chord) {
            try {
                std::vector<chcpy::string> arr;
                for (auto n : chord) {
                    arr.push_back(chcpy::string::number(n));
                }
                auto chord_str = chcpy::join(arr, "-");
                std::map<int, int> note_count;
                for (int i = 0; i < 4; ++i) {
                    note_count[m.melody.at(i + count * 4)]++;
                }
                int max_note_num = 0;
                int max_note = 0;
                for (auto it : note_count) {
                    if (it.second > max_note_num) {
                        max_note_num = it.second;
                        max_note = it.first;
                    }
                }
                auto id_melody = chcpy::seq2id::getIdBySeq(seq_dic, std::vector<int>({max_note}));
                auto id_chord = chcpy::chord2id::get(chord_dic, chord_str);
                seq.push_back(std::make_pair(id_melody, id_chord));
                ++count;
            } catch (...) {
                break;
            }
        }
        int index = 0;
        int len = seq.size();
        chcpy::hmm::train_process(model, [&](std::pair<int, int>& data) {
            if (index < len) {
                auto tmp = seq.at(index);
                data.first = tmp.first;
                data.second = tmp.second;
            } else {
                data.first = -1;
                data.second = -1;
            }
            ++index;
        });
    }
    chcpy::hmm::train_end(model);
    chcpy::hmm::save_text(model, "../data/model_simple");
    return 0;
}
