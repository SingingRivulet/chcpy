#include <filesystem>
#include <set>
#include "chcpystring.h"
#include "filereader.h"
#include "melody2chord.h"
mgnr::melody2chord::chordMap chordmap;
inline std::string processChordStr(const chcpy::string& str) {
    //std::cout << str << std::endl;
    midiSearch::chord_t chords;
    midiSearch::str2chord(str, chords);
    for (auto& it : chords) {
        if (it.size() > 2) {
            mgnr::melody2chord::wmelody_t notes;
            std::vector<int> outChord;
            for (auto& n : it) {
                notes.push_back(std::make_tuple(n, 1.0));
            }
            auto c = mgnr::melody2chord::buildMusicSection(chordmap, notes);
            auto chord_it = chordmap.chord_map.find(c.chord_name);
            if (chord_it != chordmap.chord_map.end()) {
                std::set<int> pitchs;
                for (auto cn : chord_it->second) {
                    pitchs.insert((cn + c.chord_base) % 12);
                }
                int count = 0;
                for (auto& n : it) {
                    if (pitchs.find(n % 12) != pitchs.end()) {
                        outChord.push_back(n);
                        count++;
                        if (count >= 7) {
                            break;
                        }
                    }
                }
            }
            it = std::move(outChord);
        }
    }
    std::ostringstream oss;
    oss << "[";
    bool firstchord = true;
    for (auto& it : chords) {
        if (!firstchord) {
            oss << ",";
        }
        firstchord = false;
        oss << "[";
        bool first = true;
        for (auto cn : it) {
            if (!first) {
                oss << ",";
            }
            oss << cn;
            first = false;
        }
        oss << "]";
    }
    oss << "]";
    //std::cout << oss.str() << std::endl;
    return oss.str();
}

inline std::string processLine(const chcpy::string& str) {
    auto arr = str.split("|");
    if (arr.size() == 5) {
        arr.at(3) = std::move(processChordStr(arr.at(3)));
    } else if (arr.size() == 6) {
        arr.at(4) = std::move(processChordStr(arr.at(4)));
    }
    return chcpy::join(arr, "|");
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("%s input output", argv[0]);
        return 0;
    }
    std::filesystem::path path(argv[2]);
    auto indir = path.parent_path();
    std::cout << "path:" << path << std::endl;
    std::cout << "indir:" << indir << std::endl;
    std::filesystem::create_directories(indir);
    std::ofstream ofs(path);
    for (auto line : midiSearch::lineReader(argv[1])) {
        ofs << processLine(*line) << std::endl;
    }
    return 0;
}