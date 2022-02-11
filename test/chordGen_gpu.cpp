#define CHCPY_DEBUG
#include "hmm_gpu.h"
#include "predict.h"
chcpy::gpu::GPUContext context;  //gpu上下文
int main() {
    chcpy::seq2id::dict_t dict_seq;
    chcpy::chord2id::dict_t dict_chord;
    chcpy::melody2chord::chordMap chordmap;
    chcpy::hmm::hmm_predict_t model_cpu;  //cpu端

    chcpy::seq2id::load(dict_seq, "../data/melodydict.txt");
    chcpy::chord2id::load(dict_chord, "../data/chorddict.txt");
    chcpy::hmm::load_text(model_cpu, "../data/model");

    chcpy::gpu::hmm_t model(&context, &model_cpu);  //复制到gpu端
    midiSearch::chord_t chords;
    auto fp = fopen("../outputs/chordGen/output.txt", "w");
    for (auto line : midiSearch::lineReader("test.txt")) {
        midiSearch::melody_t melody;
        midiSearch::str2melody(line->c_str(), melody);
        chcpy::predict::gen(chordmap, dict_seq, dict_chord, model, melody, chords);
        printf("=========================================================\n");

        if (fp) {
            chcpy::stringlist melody_strlist, chords_strlist;
            for (auto& it : melody) {
                melody_strlist.push_back(chcpy::string::number(it));
            }
            fprintf(fp, "[%s]|[", chcpy::join(melody_strlist, ",").c_str());
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
            fprintf(fp, "%s]\n", chcpy::join(chords_strlist, ",").c_str());
        }
    }
    if (fp) {
        fclose(fp);
    }
    return 0;
}