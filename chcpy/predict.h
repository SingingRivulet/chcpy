#pragma once
#include "hmm.h"
#include "melody2chord.h"
#include "seq2id.h"
namespace chcpy::predict {

//流程：
//切分旋律为和弦
//将切分结果转换为14进制级数
//将14进制级数每四个一组切分，并转换为hmm所需id
//将id送入hmm模型预测
//将预测结果由14进制级数转换为12平均律
template <melody2chord::chord_map_c chord_map_t,
          seq2id::dict_c melody_dict_t,
          chord2id::dict_c chord_dict_t,
          hmm::hmm_c model_t>
inline void gen(
    chord_map_t& chord_map,          //和弦匹配表
    melody_dict_t& melody_dict,      //旋律字典
    chord_dict_t& chord_dict,        //和弦字典
    model_t& model,                  //hmm模型
    const seq2id::melody_t& melody,  //输入旋律
    midiSearch::chord_t& chord       //返回和弦
) {
    std::list<melody2chord::musicSection_t> segs = chcpy::melody2chord::getMusicSection(  //切分
        chord_map,
        melody,
        4, 0.5, 1.0);
    seq2id::melody_t buffer_melody, melody_seq;
    std::vector<int> chord_id;
    chord.clear();
    for (auto seg : segs) {
#ifdef CHCPY_DEBUF
        printf("%d_%s(w=%f) \n", seg.chord_base, seg.chord_name.c_str(), seg.weight);
#endif
        buffer_melody.clear();
        chord_id.clear();
        int minNote = 9999999;
        for (auto note : seg.melody) {  //转换为14进制级数
            if (note > 0 && note < minNote) {
                minNote = note;
            }
            int noteLevel = chcpy::melody2chord::getToneLevelDelta(note, seg.chord_base, 0);
            buffer_melody.push_back(noteLevel);
        }
        seq2id::melody2seq(melody_dict, buffer_melody, melody_seq);  //每四个一组
        hmm::predict(model, melody_seq, chord_id);                   //hmm预测
        for (auto id : chord_id) {
            //转换为和弦
            auto chord14 = chord2id::get(chord_dict, id);        //搜索
            auto note14s = QString(chord14.c_str()).split("-");  //分割
            int B = seg.chord_base;                              //片段根音
            int last = -1;
            std::vector<int> singleChord;
            int maxNote = -1;
            for (auto note14_str : note14s) {  //遍历
#ifdef CHCPY_DEBUF
                printf("%s ", note14_str.toStdString().c_str());
#endif
                int A = note14_str.toInt();                               //A的原始值（14进制）
                int tone = melody2chord::getToneFromLevelDelta(A, B, 0);  //实际音阶（12平均律）
                while (tone < last) {
                    tone += 12;
                }
                last = tone;
                if (maxNote < tone) {
                    maxNote = tone;
                }
                singleChord.push_back(tone);
            }
            int toneShift = (1 + (maxNote - minNote) / 12) * 12;
            for (auto& note : singleChord) {
                note -= toneShift;
            }
#ifdef CHCPY_DEBUF
            printf("(");
            for (auto note : singleChord) {
                printf("%d ", note);
            }
            printf(")\n");
#endif
            chord.push_back(singleChord);
        }
    }
}

}  // namespace chcpy::predict