#include "filereader.h"
#include "hmm.h"
#include "seq2id.h"
int main() {
    chcpy::seq2id::dict_t seq_dic;
    chcpy::chord2id::dict_t chord_dic;
    chcpy::seq2id::load(seq_dic, "../data/melodydict.txt");
    chcpy::chord2id::load(chord_dic, "../data/chorddict.txt");
    chcpy::hmm::hmm_t model;
    printf("M:%d N:%d\n", seq_dic.index, chord_dic.index);
    chcpy::hmm::init(model, seq_dic.index, chord_dic.index);
    std::vector<int> key, val;
    for (auto data : midiSearch::lineReader("../data/2.list")) {
        QString line = data->c_str();
        auto kv = line.split("|");
        auto melody_str = kv.at(0).trimmed().split(" ");
        auto chord = kv.at(1).trimmed().split(" ");
        chcpy::seq2id::melody_t melody, melody_vec;
        key.clear();
        val.clear();
        for (auto& it : chord) {
            int id = chcpy::chord2id::get(chord_dic, it.toStdString());
            val.push_back(id);
        }
        for (auto& it : melody_str) {
            melody.push_back(it.toInt());
        }
        for (auto seq : chcpy::seq2id::melody2seq(melody)) {
            int id = chcpy::seq2id::getIdBySeq(seq_dic, *seq);
            key.push_back(id);
        }
        //开始训练
        int index = 0;
        int len = std::min(key.size(), val.size());
        chcpy::hmm::train_process(model, [&](std::pair<int, int>& data) {
            if (index < len) {
                data.first = key.at(index);
                data.second = val.at(index);
            } else {
                data.first = -1;
                data.second = -1;
            }
            ++index;
        });
    }
    chcpy::hmm::train_end(model);
    chcpy::hmm::save_text(model, "../data/model");
    return 0;
}
