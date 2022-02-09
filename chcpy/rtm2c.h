#pragma once
#include "bayes.h"
#include "chordNext.h"
#include "hmm_gpu.h"
#include "predict.h"
namespace chcpy::rtmtc {

struct rtmtc_t {
    melody2chord::musicSection_t buffer{};
    midiSearch::chord_t chord{};
    std::vector<int> notes{};
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
    a.notes;
};

template <melody2chord::chord_map_c chord_map_t,
          rtmtc_c rtmtc_type,
          typename hmm_type,
          seq2id::dict_c dict_seq_t,
          chord2id::dict_c dict_chord_t>
inline bool pushSection(rtmtc_type& self,
                        const chord_map_t& chord_map,
                        hmm_type& model,
                        dict_seq_t& dict_melody,
                        dict_chord_t& dict_chord,
                        const melody2chord::musicSection_t& section) {
    self.updated = false;
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
            }
            self.buffer = section;
        }
    }
    return self.updated;
}

template <melody2chord::chord_map_c chord_map_t,
          rtmtc_c rtmtc_type,
          typename hmm_type,
          seq2id::dict_c dict_seq_t,
          chord2id::dict_c dict_chord_t>
inline bool pushNote(rtmtc_type& self,
                     const chord_map_t& chord_map,
                     hmm_type& model,
                     dict_seq_t& dict_melody,
                     dict_chord_t& dict_chord,
                     int note) {
    self.notes.push_back(note);
    if (self.notes.size() >= 4) {
        pushSection(
            self,
            chord_map,
            model,
            dict_melody,
            dict_chord,
            melody2chord::buildMusicSection(
                chord_map,
                self.notes,
                self.act,
                self.times));
        self.notes.clear();
        return true;
    }
    return false;
}

template <melody2chord::chord_map_c chord_map_t,
          rtmtc_c rtmtc_type,
          typename hmm_type,
          seq2id::dict_c dict_seq_t,
          chord2id::dict_c dict_chord_t>
inline void buildRealtimeBuffer(const chord_map_t& chord_map,
                                hmm_type& model,
                                dict_seq_t& dict_melody,
                                dict_chord_t& dict_chord,
                                activeBuffer& buf,
                                rtmtc_type& now,
                                midiSearch::chord_t& newChord,
                                std::vector<activeBuffer::chordtime> res) {
    std::vector<std::string> newChord_str;
    predict::genChord(chord_map,
                      dict_melody,
                      dict_chord,
                      model,
                      now.buffer,
                      newChord);
    //转换为字符串
    for (auto& chord : newChord) {
        std::vector<std::string> buffer;
        for (auto& it : chord) {
            buffer.push_back(it == 0 ? "0" : chcpy::string::number(it % 12));
        }
        auto str = join(buffer, "-");
        newChord_str.push_back(str);
    }
    buf.buildRealtimeBuffer(newChord_str, res);
}

template <int bayeslen = 5, chcpy::chordNext::dict_c dict_type>
inline midiSearch::melody_t genChord(dict_type& dict,
                                     bayes::bayes_predict_t<bayeslen>& model,
                                     const midiSearch::chord_t& newChord,
                                     const std::vector<activeBuffer::chordtime>& res) {
    midiSearch::melody_t outChord;
    auto last = newChord.rbegin();
    if (last == newChord.rend()) {
        //无法输出
        return outChord;
    }
    auto max_id = chcpy::chordNext::predictNext(dict, model, res);
    auto it = dict.id_chord.find(max_id);
    if (it != dict.id_chord.end()) {
        string chord = std::get<0>(it->second);
        //获取八度
        int oct = 0;
        if (last->size() > 0) {
            oct = std::max(last->at(0), 0) / 12;
        }
        //转换为数字
        auto arr = chord.split("-");
        for (auto& it : arr) {
            if (!it.empty()) {
                outChord.push_back(it.toInt() + oct * 12);
            }
        }
        return outChord;
    } else {
        return *last;
    }
}

}  // namespace chcpy::rtmtc