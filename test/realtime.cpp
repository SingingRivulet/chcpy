//#define CHCPY_DEBUG
#include "bayes.h"
#include "chordNext.h"
#include "hmm_gpu.h"
#include "predict.h"
#include "rtm2c.h"
chcpy::gpu::GPUContext context;  //gpu上下文
int main() {
    chcpy::chordNext::dict_t dict_time;
    dict_time.load("../data/chordtimedict.txt");

    chcpy::bayes::bayes_predict_t model_predictNext;
    model_predictNext.load("../data/bayesmodel");

    chcpy::activeBuffer activeBuffer;
    chcpy::seq2id::dict_t dict_seq;
    chcpy::chord2id::dict_t dict_chord;
    chcpy::melody2chord::chordMap chordmap;
    chcpy::rtmtc::rtmtc_t rtmtc;

    chcpy::hmm::hmm_predict_t model_cpu;  //cpu端

    chcpy::seq2id::load(dict_seq, "../data/melodydict.txt");
    chcpy::chord2id::load(dict_chord, "../data/chorddict.txt");
    chcpy::hmm::load_text(model_cpu, "../data/model");

    chcpy::gpu::hmm_t model_chordGen(&context, &model_cpu);  //复制到gpu端

    midiSearch::chord_t newChord;
    std::vector<chcpy::activeBuffer::chordtime> rtb;
    for (auto line : midiSearch::lineReader("test.txt")) {
        midiSearch::melody_t melody;
        midiSearch::str2melody(line->c_str(), melody);
        for (auto note : melody) {
            printf("音符：%d\n", note);
            if (chcpy::rtmtc::pushNote(rtmtc, chordmap, model_chordGen, dict_seq, dict_chord, note)) {
                newChord.clear();
                rtb.clear();
                //返回true说明应该生成和弦了
                if (rtmtc.updated) {
                    //音乐分段，应该更新历史和弦了
                    activeBuffer.pushChord(rtmtc.chord);
                }
                //构建向量，供预测
                chcpy::rtmtc::buildRealtimeBuffer(
                    chordmap, model_chordGen,
                    dict_seq, dict_chord,
                    activeBuffer, rtmtc,
                    newChord, rtb);

                printf("和弦：");
                auto chord = chcpy::rtmtc::genChord(dict_time, model_predictNext, newChord, rtb);
                for (auto& chord_note : chord) {
                    printf("%d ", chord_note);
                }
                printf("\n");
            }
        }
    }

    return 0;
}