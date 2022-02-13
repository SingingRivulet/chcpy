#include "MidiFile.h"
#include "Options.h"
#include "filereader.h"
void buildMidi(const char* dir, const char* sample) {
    int index = 0;
    char midi_name[256];
    int longChord = 0;
    int chordCount = 0;
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
int main() {
    buildMidi("../outputs/realtime/midi", "../outputs/realtime/output.txt");
    buildMidi("../outputs/chordGen/midi", "../outputs/chordGen/output.txt");
    buildMidi("../outputs/chordNext/midi", "../outputs/chordNext/output.txt");
    buildMidi("../outputs/ori/midi", "../test/test2.txt");
    return 0;
}