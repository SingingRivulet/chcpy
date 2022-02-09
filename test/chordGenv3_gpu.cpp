#define CHCPY_DEBUG
#include "hmmv3_gpu.h"
#include "predict.h"
chcpy::gpu::GPUContext context;  //gpu上下文
int main() {
    chcpy::seq2id::dict_t dict_seq;
    chcpy::chord2id::dict_t dict_chord;
    chcpy::melody2chord::chordMap chordmap;
    chcpy::hmm::hmmv3_predict_t model_cpu;  //cpu端

    chcpy::seq2id::load(dict_seq, "../data/melodydict.txt");
    chcpy::chord2id::load(dict_chord, "../data/chorddict.txt");
    chcpy::hmm::load_text(model_cpu, "../data/modelv3");

    chcpy::gpu::hmmv3_t model(&context, &model_cpu);  //复制到gpu端
    midiSearch::chord_t chord;
    for (auto line : midiSearch::lineReader("test.txt")) {
        midiSearch::melody_t melody;
        midiSearch::str2melody(line->c_str(), melody);
        chcpy::predict::gen(chordmap, dict_seq, dict_chord, model, melody, chord);
        printf("=========================================================\n");
    }
    return 0;
}