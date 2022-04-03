#include <sys/stat.h>
#include <sys/types.h>
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
int main() {
    /*
    buildMidi("../outputs/realtime/midi", "../outputs/realtime/output.txt");
    buildMidi("../outputs/chordGen/midi", "../outputs/chordGen/output.txt");
    buildMidi("../outputs/chordNext/midi", "../outputs/chordNext/output.txt");
    buildMidi("../outputs/ori/midi", "../test/test2.txt");
    buildMidi("../outputs/crf1/midi", "../outputs/crf1/output.txt");
    buildMidi("../outputs/crf2/midi", "../outputs/crf2/output.txt");
    buildMidi("../outputs/simpleHMM/midi", "../outputs/simpleHMM/output.txt");
    buildMidi("../outputs/rnn/midi", "../outputs/rnn/output.txt");
    for (int i = 0; i < 250; i+=10) {
        char buf_in[128];
        char buf_out[128];
        snprintf(buf_in, sizeof(buf_in), "../outputs/transformer/out4/test_ckpt%d.txt", i);
        snprintf(buf_out, sizeof(buf_out), "../outputs/transformer/transformer小规模+crf/test_ckpt%d.mid", i);
        buildMidi3(buf_out, buf_in);
    }
    */
    buildMidi_melody("../outputs/lenMax/midi/aige-midi", "../outputs/lenMax/aige/melody.txt", 10);
    buildMidi_melody("../outputs/lenMax/midi/nottingham-midi", "../outputs/lenMax/nottingham/melody.txt", 20);
    buildMidi_melody("../outputs/lenMax/midi/hook1-midi", "../outputs/lenMax/hook1/melody.txt", 50);
    buildMidi_melody("../outputs/lenMax/midi/hook2-midi", "../outputs/lenMax/hook2/melody.txt", 10);
    buildMidi_melody("../outputs/lenMax/midi/wikifonia-midi", "../outputs/lenMax/wikifonia/melody.txt", 51);
    return 0;
}
