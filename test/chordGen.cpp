#define CHCPY_DEBUF
#include "predict.h"
int main() {
    chcpy::seq2id::dict_t dict_seq;
    chcpy::chord2id::dict_t dict_chord;
    chcpy::melody2chord::chordMap chordmap;
    chcpy::hmm::hmm_predict_t model;

    chcpy::seq2id::load(dict_seq, "../data/melodydict.txt");
    chcpy::chord2id::load(dict_chord, "../data/chorddict.txt");
    chcpy::hmm::load_text(model, "../data/model");

    midiSearch::chord_t chord;
    for (auto line : midiSearch::lineReader("test.txt")) {
        midiSearch::melody_t melody;
        midiSearch::str2melody(line->c_str(), melody);
        chcpy::predict::gen(chordmap, dict_seq, dict_chord, model, melody, chord);
        printf("=========================================================\n");
    }
    return 0;
}