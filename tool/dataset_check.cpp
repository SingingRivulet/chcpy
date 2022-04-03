#include <map>
#include <set>
#include <vector>
#include "filereader.h"
#include "melody2chord.h"
chcpy::string chord2relative(const midiSearch::melody_t& chord) {
    int last = -1;
    chcpy::string res;
    for (auto it : chord) {
        if (last > 0) {
            res.append(chcpy::string::number(it - last));
            res.append(" ");
        }
        last = it;
    }
    return res;
}
int main() {
    std::set<chcpy::string> inchords;
    for (auto music : midiSearch::musicReader_3colume("../data/3.txt")) {
        auto m = std::get<0>(music);
        auto& chords = m.chord;
        for (auto it : chords) {
            inchords.insert(chord2relative(it));
        }
    }
#define check(x)                                                  \
    {                                                             \
        midiSearch::melody_t m(x);                                \
        if (inchords.find(chord2relative(m)) == inchords.end()) { \
            for (auto it : m) {                                   \
                std::cout << it;                                  \
            }                                                     \
            std::cout << std::endl;                               \
        }                                                         \
    }
    check({57,61,64});
    return 0;
}