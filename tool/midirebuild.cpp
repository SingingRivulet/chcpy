#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <functional>
#include "MidiFile.h"
#include "Options.h"
#include "filereader.h"
void buildMidi(const char* dir, const char* sample) {
    int index = 0;
    char midi_name[256];
    int longChord = 0;
    int chordCount = 0;
    mkdir(dir, 0777);
    for (auto data : midiSearch::musicReader_2colume(sample)) {
        snprintf(midi_name, sizeof(midi_name), "%s/%d.mid", dir, ++index);
        auto melody = std::get<0>(data);
        auto chords = std::get<1>(data);
        /*
        int min_melody = 999;
        for (auto it : melody) {
            if (it > 0 && it < min_melody) {
                min_melody = it;
            }
        }
        while (1) {
            for (auto& it : chords) {
                for (auto n : it) {
                    if (n > min_melody) {
                        goto doloop;
                    }
                }
            }
            goto endloop;
        doloop:
            for (auto& it : chords) {
                for (auto& n : it) {
                    n -= 12;
                }
            }
            continue;
        }
    endloop:
    */

        smf::MidiFile midifile;
        midifile.setTPQ(384);  //0音轨
        midifile.addTrack();
        midifile.addTempo(0, 0, 40);
        midifile.addTrack();
        midifile.addTrack();

        int tm = 0;
        int lastNote = 0;
        int lastNoteStart = 0;
        for (auto& key : melody) {
            if (key != lastNote) {
                if (lastNote > 0) {
                    midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
                    midifile.addNoteOff(1, tm * 24, 0, lastNote);
                }
                lastNote = key;
                lastNoteStart = tm;
            }
            ++tm;
        }
        if (lastNote != 0) {
            midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
            midifile.addNoteOff(1, tm * 24, 0, lastNote);
        }

        tm = 0;
        midiSearch::melody_t lastChord;
        int lastChordStart = 0;
        for (auto& chord : chords) {
            if (chord == lastChord) {
                ++longChord;
            }
            if (chord != lastChord || tm % 4 == 0) {
                if (!lastChord.empty()) {
                    for (auto it : lastChord) {
                        if (it > 0) {
                            midifile.addNoteOn(2, lastChordStart * 96, 0, it, 90);
                        }
                    }
                    for (auto it : lastChord) {
                        if (it > 0) {
                            midifile.addNoteOff(2, tm * 96, 0, it);
                        }
                    }
                }
                lastChord = chord;
                lastChordStart = tm;
            }
            ++chordCount;
            ++tm;
        }
        if (!lastChord.empty()) {
            for (auto it : lastChord) {
                if (it > 0) {
                    midifile.addNoteOn(2, lastChordStart * 96, 0, it, 90);
                }
            }
            for (auto it : lastChord) {
                if (it > 0) {
                    midifile.addNoteOff(2, tm * 96, 0, it);
                }
            }
        }
        midifile.write(midi_name);
    }
    printf("%s:longchord:%d/%d(%f)\n",
           sample,
           longChord, chordCount,
           (float)longChord / (float)chordCount);
}

void buildMidi_melody(const char* dir, const char* sample, int num = -1) {
    int index = 0;
    char midi_name[256];
    int longChord = 0;
    int chordCount = 0;
    mkdir(dir, 0777);
    for (auto data : midiSearch::lineReader(sample)) {
        ++index;
        if (num > 0 && index > num) {
            break;
        }
        chcpy::string d(*data);
        auto arr = d.trimmed().split("|");
        if (arr.size() >= 2) {
            midiSearch::melody_t melody;
            midiSearch::str2melody(arr.at(1), melody);
            snprintf(midi_name, sizeof(midi_name), "%s/%d.mid", dir, index);

            smf::MidiFile midifile;
            midifile.setTPQ(384);  //0音轨
            midifile.addTrack();
            midifile.addTempo(0, 0, 40);
            midifile.addTrack();
            midifile.addTrack();

            int tm = 0;
            int lastNote = 0;
            int lastNoteStart = 0;
            for (auto& key : melody) {
                if (key != lastNote) {
                    if (lastNote > 0) {
                        midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
                        midifile.addNoteOff(1, tm * 24, 0, lastNote);
                    }
                    lastNote = key;
                    lastNoteStart = tm;
                }
                ++tm;
            }
            if (lastNote != 0) {
                midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
                midifile.addNoteOff(1, tm * 24, 0, lastNote);
            }
            midifile.write(midi_name);
        }
    }
    printf("%s:longchord:%d/%d(%f)\n",
           sample,
           longChord, chordCount,
           (float)longChord / (float)chordCount);
}
void buildMidi2(const char* out, const char* sample) {
    midiSearch::melody_t melody;
    midiSearch::chord_t chords;
    chords.push_back(melody);
    for (auto data : midiSearch::lineReader(sample)) {
        chcpy::string d(*data);
        auto arr = d.trimmed().split("|");
        if (arr.size() >= 2) {
            midiSearch::melody_t m, c;
            midiSearch::str2melody(arr.at(0), m);
            for (auto it : m) {
                melody.push_back(it);
            }
            if (arr.at(1) != "E") {
                midiSearch::str2melody(arr.at(1), c);
            }
            chords.push_back(c);
        }
    }
    int longChord = 0;
    int chordCount = 0;
    smf::MidiFile midifile;
    midifile.setTPQ(384);  //0音轨
    midifile.addTrack();
    midifile.addTempo(0, 0, 40);
    midifile.addTrack();
    midifile.addTrack();

    int tm = 0;
    int lastNote = 0;
    int lastNoteStart = 0;
    for (auto& key : melody) {
        if (key != lastNote) {
            if (lastNote > 0) {
                midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
                midifile.addNoteOff(1, tm * 24, 0, lastNote);
            }
            lastNote = key;
            lastNoteStart = tm;
        }
        ++tm;
    }
    if (lastNote != 0) {
        midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
        midifile.addNoteOff(1, tm * 24, 0, lastNote);
    }

    tm = 0;
    midiSearch::melody_t lastChord;
    int lastChordStart = 0;
    for (auto& chord : chords) {
        if (chord == lastChord) {
            ++longChord;
        }
        if (chord != lastChord || tm % 4 == 0) {
            if (!lastChord.empty()) {
                for (auto it : lastChord) {
                    if (it > 0) {
                        midifile.addNoteOn(2, lastChordStart * 96, 0, it, 90);
                    }
                }
                for (auto it : lastChord) {
                    if (it > 0) {
                        midifile.addNoteOff(2, tm * 96, 0, it);
                    }
                }
            }
            lastChord = chord;
            lastChordStart = tm;
        }
        ++chordCount;
        ++tm;
    }
    if (!lastChord.empty()) {
        for (auto it : lastChord) {
            if (it > 0) {
                midifile.addNoteOn(2, lastChordStart * 96, 0, it, 90);
            }
        }
        for (auto it : lastChord) {
            if (it > 0) {
                midifile.addNoteOff(2, tm * 96, 0, it);
            }
        }
    }
    midifile.write(out);
    printf("%s:longchord:%d/%d(%f)\n",
           sample,
           longChord, chordCount,
           (float)longChord / (float)chordCount);
}

void buildMidi3(const char* midi_name, const char* sample) {
    int longChord = 0;
    int chordCount = 0;
    for (auto data : midiSearch::musicReader_2colume(sample)) {
        auto melody = std::get<0>(data);
        auto chords = std::get<1>(data);

        smf::MidiFile midifile;
        midifile.setTPQ(384);  //0音轨
        midifile.addTrack();
        midifile.addTempo(0, 0, 30);
        midifile.addTrack();
        midifile.addTrack();

        int tm = 0;
        int lastNote = 0;
        int lastNoteStart = 0;
        for (auto& key : melody) {
            if (key != lastNote) {
                if (lastNote > 0) {
                    midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
                    midifile.addNoteOff(1, tm * 24, 0, lastNote);
                }
                lastNote = key;
                lastNoteStart = tm;
            }
            ++tm;
        }
        if (lastNote != 0) {
            midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
            midifile.addNoteOff(1, tm * 24, 0, lastNote);
        }

        tm = 0;
        midiSearch::melody_t lastChord;
        int lastChordStart = 0;
        for (auto& chord : chords) {
            if (chord == lastChord) {
                ++longChord;
            }
            if (chord != lastChord || tm % 4 == 0) {
                if (!lastChord.empty()) {
                    for (auto it : lastChord) {
                        if (it > 0) {
                            midifile.addNoteOn(2, lastChordStart * 96, 0, it, 90);
                        }
                    }
                    for (auto it : lastChord) {
                        if (it > 0) {
                            midifile.addNoteOff(2, tm * 96, 0, it);
                        }
                    }
                }
                lastChord = chord;
                lastChordStart = tm;
            }
            ++chordCount;
            ++tm;
        }
        if (!lastChord.empty()) {
            for (auto it : lastChord) {
                if (it > 0) {
                    midifile.addNoteOn(2, lastChordStart * 96, 0, it, 90);
                }
            }
            for (auto it : lastChord) {
                if (it > 0) {
                    midifile.addNoteOff(2, tm * 96, 0, it);
                }
            }
        }
        midifile.write(midi_name);
    }
    printf("%s:longchord:%d/%d(%f)\n",
           sample,
           longChord, chordCount,
           (float)longChord / (float)chordCount);
}

#define E3 40
#define C4 48
void buildMidi_autooct(const char* out, const char* sample) {
    midiSearch::melody_t melody;
    midiSearch::chord_t chords;
    //chords.push_back(melody);
    for (auto data : midiSearch::lineReader(sample)) {
        chcpy::string d(*data);
        auto arr = d.trimmed().split("|");
        if (arr.size() >= 2) {
            midiSearch::melody_t m, ch, chord_list;
            midiSearch::str2melody(arr.at(0), m);
            for (auto it : m) {
                melody.push_back(it);
            }
            if (strstr(arr.at(1).c_str(), "E") == NULL) {
                midiSearch::str2melody(arr.at(1), ch);
                int num = 0;

                for (auto& it : ch) {
                    num++;
                    if (it < E3) {
                        it = it + 12;  // 比E3小的音太难听了
                    } else {
                        if (it < C4 && num > 1)
                            it = it + 12;
                    }
                    if (num > 4)  // 伴奏时不是音越多越好听，因为这里是只采用柱式和弦，5个音会过于庞杂会导致伴奏音感变差
                    {
                        break;
                    }
                    chord_list.push_back(it);
                }
            }
            //try {
            //    if (c.at(0) > 36) {
            //        c.at(0) -= 12;
            //    }
            //} catch (...) {
            //}
            chords.push_back(chord_list);
        }
    }
    int longChord = 0;
    int chordCount = 0;
    smf::MidiFile midifile;
    midifile.setTPQ(384);  //0音轨
    midifile.addTrack();
    midifile.addTempo(0, 0, 30);
    midifile.addTrack();
    midifile.addTrack();

    int tm = 0;
    int lastNote = 0;
    int lastNoteStart = 0;
    for (auto& key : melody) {
        if (key != lastNote) {
            if (lastNote > 0) {
                midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
                midifile.addNoteOff(1, tm * 24, 0, lastNote);
            }
            lastNote = key;
            lastNoteStart = tm;
        }
        ++tm;
    }
    if (lastNote != 0) {
        midifile.addNoteOn(1, lastNoteStart * 24, 0, lastNote, 90);
        midifile.addNoteOff(1, tm * 24, 0, lastNote);
    }

    tm = 0;
    midiSearch::melody_t lastChord;
    int lastChordStart = 0;
    for (auto& chord : chords) {
        if (chord == lastChord) {
            ++longChord;
        }
        if (chord != lastChord || tm % 4 == 0) {
            if (!lastChord.empty()) {
                for (auto it : lastChord) {
                    if (it > 0) {
                        midifile.addNoteOn(2, lastChordStart * 96, 0, it, 90);
                    }
                }
                for (auto it : lastChord) {
                    if (it > 0) {
                        midifile.addNoteOff(2, tm * 96, 0, it);
                    }
                }
            }
            lastChord = chord;
            lastChordStart = tm;
        }
        ++chordCount;
        ++tm;
    }
    if (!lastChord.empty()) {
        for (auto it : lastChord) {
            if (it > 0) {
                midifile.addNoteOn(2, lastChordStart * 96, 0, it, 90);
            }
        }
        for (auto it : lastChord) {
            if (it > 0) {
                midifile.addNoteOff(2, tm * 96, 0, it);
            }
        }
    }
    midifile.write(out);
    printf("%s:longchord:%d/%d(%f)\n",
           sample,
           longChord, chordCount,
           (float)longChord / (float)chordCount);
}

int getdir(const char* pathname, const std::function<void(const char*)>& callback) {
    DIR* path = NULL;
    path = opendir(pathname);

    if (path == NULL) {
        perror("failed");
        exit(1);
    }
    struct dirent* ptr;  //目录结构体---属性：目录类型 d_type,  目录名称d_name
    char buf[1024] = {0};
    while ((ptr = readdir(path)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        //如果是目录
        if (ptr->d_type == DT_DIR) {
            sprintf(buf, "%s/%s", pathname, ptr->d_name);
            printf("目录:%s\n", buf);
            getdir(buf, callback);
        }
        if (ptr->d_type == DT_REG) {
            sprintf(buf, "%s/%s", pathname, ptr->d_name);  //把pathname和文件名拼接后放进缓冲字符数组
            int len = strlen(buf);
            if (len > 5) {
                if (buf[len - 1] == 't' &&
                    buf[len - 2] == 'x' &&
                    buf[len - 3] == 't' &&
                    buf[len - 4] == '.') {
                    printf("文件:%s\n", buf);
                    callback(buf);
                }
            }
        }
    }
    return 0;
}

int main() {
    /*
    buildMidi("../outputs/chordGen/midi", "../outputs/chordGen/output.txt");
    buildMidi("../outputs/realtime/midi", "../outputs/realtime/output.txt");
    buildMidi("../outputs/chordNext/midi", "../outputs/chordNext/output.txt");
    buildMidi("../outputs/ori/midi", "../test/test2.txt");
    buildMidi("../outputs/crf1/midi", "../outputs/crf1/output.txt");
    buildMidi("../outputs/crf2/midi", "../outputs/crf2/output.txt");
    buildMidi("../outputs/simpleHMM/midi", "../outputs/simpleHMM/output.txt");
    buildMidi("../outputs/rnn/midi", "../outputs/rnn/output.txt");
    for (int i = 310; i <= 350; i+=10) {
        char buf_in[128];
        char buf_out[128];
        snprintf(buf_in, sizeof(buf_in), "/home/ubuntu/project/chcpy/outputs/tf300-350/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "/home/ubuntu/project/chcpy/outputs/tf300-350/test_ckpt%d.mid", i);
        buildMidi_autooct(buf_out, buf_in);
    }
    //for (int j = 2; j <= 5; ++j) {
    for (int i = 310; i <= 390; i += 10) {
        char buf_in[128];
        char buf_out[128];
        //snprintf(buf_in, sizeof(buf_in), "/home/ubuntu/project/chcpy/outputs/tfcrf310-390-2line/test_ckpt%d.txt", i);
        //snprintf(buf_out, sizeof(buf_out), "/home/ubuntu/project/chcpy/outputs/tfcrf310-390-2line/test_ckpt%d.mid", i);
        //buildMidi_autooct(buf_out, buf_in);
        snprintf(buf_in, sizeof(buf_in), "/home/ubuntu/project/chcpy/outputs/tf310-390-2line/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "/home/ubuntu/project/chcpy/outputs/tf310-390-2line/test_ckpt%d.mid", i);
        buildMidi_autooct(buf_out, buf_in);
    }
    //}
    buildMidi_melody("../outputs/lenMax/midi/aige-midi", "../outputs/lenMax/aige/melody.txt", 10);
    buildMidi_melody("../outputs/lenMax/midi/nottingham-midi", "../outputs/lenMax/nottingham/melody.txt", 20);
    buildMidi_melody("../outputs/lenMax/midi/hook1-midi", "../outputs/lenMax/hook1/melody.txt", 50);
    buildMidi_melody("../outputs/lenMax/midi/hook2-midi", "../outputs/lenMax/hook2/melody.txt", 10);
    buildMidi_melody("../outputs/lenMax/midi/wikifonia-midi", "../outputs/lenMax/wikifonia/melody.txt", 51);
    buildMidi_autooct("../outputs/对照/流式处理-HMM-已错位.mid", "../outputs/对照/流式处理-HMM-已错位.txt");
    buildMidi_autooct("../outputs/对照/预测输出-规则贝叶斯-已错位-带错误累积-加约束缩减么.mid", "../outputs/对照/预测输出-规则贝叶斯-已错位-带错误累积-加约束缩减么.txt");
    buildMidi_autooct("../outputs/对照/预测输出-CRF-已错位-带错误累积-必要的约束缩减.mid", "../outputs/对照/预测输出-CRF-已错位-带错误累积-必要的约束缩减.txt");
    buildMidi_autooct("../outputs/对照/流式处理-RNN-已错位.mid", "../outputs/对照/流式处理-RNN-已错位.txt");
    buildMidi_autooct("../outputs/对照/预测输出-规则贝叶斯-已错位-带错误累积.mid", "../outputs/对照/预测输出-规则贝叶斯-已错位-带错误累积.txt");
    buildMidi_autooct("../outputs/对照/预测输出-RNN-已错位-带错误累积.mid", "../outputs/对照/预测输出-RNN-已错位-带错误累积.txt");
    */
    //buildMidi_autooct("../outputs/tfcrf/2_20M版本大致最好轮-860轮.mid", "../outputs/tfcrf/2_20M版本大致最好轮-860轮.txt");
    //buildMidi_autooct("../outputs/tfcrf/2_40M版本大致最好轮-600轮.mid", "../outputs/tfcrf/2_40M版本大致最好轮-600轮.txt");
    //buildMidi_autooct("../outputs/20-40/20M_2line.mid", "../outputs/20-40/20M_2line.txt");
    //buildMidi_autooct("../outputs/20-40/40M_2line.mid", "../outputs/20-40/40M_2line.txt");

    /*
    for (int i = 1; i <= 29; i += 1) {
        char buf_in[128];
        char buf_out[128];
        snprintf(buf_in, sizeof(buf_in), "../outputs/midi-out/result_remove_structural_chord/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "../outputs/midi-out/result_remove_structural_chord/test_ckpt%d.mid", i);
        buildMidi_autooct(buf_out, buf_in);
    }
    for (int i = 0; i <= 29; i += 1) {
        char buf_in[128];
        char buf_out[128];
        snprintf(buf_in, sizeof(buf_in), "../outputs/midi-out/results_all_removed/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "../outputs/midi-out/results_all_removed/test_ckpt%d.mid", i);
        buildMidi_autooct(buf_out, buf_in);
    }

    for (int i = 1; i <= 30; i += 1) {
        char buf_in[128];
        char buf_out[128];
        snprintf(buf_in, sizeof(buf_in), "../outputs/midi-out/results_remove_cadence/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "../outputs/midi-out/results_remove_cadence/test_ckpt%d.mid", i);
        buildMidi_autooct(buf_out, buf_in);
    }

    for (int i = 1; i <= 50; i += 1) {
        char buf_in[128];
        char buf_out[128];
        snprintf(buf_in, sizeof(buf_in), "../outputs/midi-out/results_remove_is_weighted/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "../outputs/midi-out/results_remove_is_weighted/test_ckpt%d.mid", i);
        buildMidi_autooct(buf_out, buf_in);
    }
    for (int i = 1; i <= 50; i += 1) {
        char buf_in[128];
        char buf_out[128];
        snprintf(buf_in, sizeof(buf_in), "../outputs/midi-out/results_remove_weighted_features/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "../outputs/midi-out/results_remove_weighted_features/test_ckpt%d.mid", i);
        buildMidi_autooct(buf_out, buf_in);
    }
    for (int i = 0; i <= 30; i += 1) {
        char buf_in[128];
        char buf_out[128];
        snprintf(buf_in, sizeof(buf_in), "../outputs/results_decode/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "../outputs/results_decode/test_ckpt%d.mid", i);
        buildMidi_autooct(buf_out, buf_in);
    }
    */

    //buildMidi("../outputs/rnn-out/midi", "../outputs/rnn-out/output.txt");
    getdir("../outputs/midirebuild/", [](const char* pathname) {
        auto out = std::string(pathname) + ".mid";
        buildMidi_autooct(out.c_str(), pathname);
    });
    return 0;
}
