#pragma once
#include "chordNext.h"
#include "hmm_gpu.h"
#include "predict.h"
namespace chcpy::rtmtc {

struct rtmtc_t {
    melody2chord::musicSection_t buffer{};
    midiSearch::chord_t chord{};
    float act = 0.5;
    float sectionWeight = 1.0;
    int times = 1;
    bool updated = false;
};
template <typename T>
concept rtmtc_c = requires(T a) {
    a.act = 0;
    a.sectionWeight = 0;
    a.times = 0;
    a.updated = false;
    a.buffer;
    a.chord;
};

inline void pushSection(const melody2chord::chordMap& chord_map,
                        rtmtc_t& self,
                        gpu::hmm_t& model,
                        seq2id::dict_t& dict_melody,
                        chord2id::dict_t& dict_chord,
                        melody2chord::musicSection_t& section) {
    if (self.buffer.melody.empty()) {
        //第一次使用
        self.buffer = section;
    } else {
        //检测合并
        auto meg = melody2chord::merge(chord_map, self.buffer, section, self.act, self.times);
        float nowWeight = self.buffer.weight + section.weight + self.sectionWeight;
        if (nowWeight >= meg.weight) {
            //合并
            self.buffer = std::move(meg);
        } else {
            if (!self.buffer.melody.empty()) {
                //返回
                self.chord.clear();
                predict::genChord(chord_map,
                                  dict_melody,
                                  dict_chord,
                                  model,
                                  self.buffer,
                                  self.chord);
                self.updated = true;
            } else {
                self.updated = false;
            }
            self.buffer = section;
        }
    }
}

}  // namespace chcpy::rtmtc