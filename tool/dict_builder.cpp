#include "filereader.h"
#include "seq2id.h"
int main() {
    chcpy::seq2id::dict_t seq_dic;
    chcpy::chord2id::dict_t chord_dic;
    for (auto data : midiSearch::lineReader("../data/1.list")) {
        QString line = data->c_str();
        auto kv = line.split("|");
        auto melody_str = kv.at(0).trimmed().split(" ");
        auto chord = kv.at(1).trimmed().split(" ");
        chcpy::seq2id::melody_t melody, melody_vec;
        for (auto& it : chord) {
            chcpy::chord2id::add(chord_dic, it.toStdString());
        }
        for (auto& it : melody_str) {
            melody.push_back(it.toInt());
        }
        for (auto seq : chcpy::seq2id::melody2seq(melody)) {
            chcpy::seq2id::add(seq_dic, *seq);
        }
    }
    for (auto data : midiSearch::lineReader("../data/2.list")) {
        QString line = data->c_str();
        auto kv = line.split("|");
        auto melody_str = kv.at(0).trimmed().split(" ");
        auto chord = kv.at(1).trimmed().split(" ");
        chcpy::seq2id::melody_t melody, melody_vec;
        for (auto& it : chord) {
            chcpy::chord2id::add(chord_dic, it.toStdString());
        }
        for (auto& it : melody_str) {
            melody.push_back(it.toInt());
        }
        for (auto seq : chcpy::seq2id::melody2seq(melody)) {
            chcpy::seq2id::add(seq_dic, *seq);
        }
    }
    chcpy::seq2id::save(seq_dic, "../data/melodydict.txt");
    chcpy::chord2id::save(chord_dic, "../data/chorddict.txt");
    return 0;
}
