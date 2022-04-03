#include "filereader.h"
#include "hmm.h"
#include "hmm_gpu.h"
#include "seq2id.h"
chcpy::gpu::GPUContext context;  //gpu上下文
int main() {
    chcpy::seq2id::dict_t seq_dic;
    chcpy::chord2id::dict_t chord_dic;
    chcpy::seq2id::load(seq_dic, "../data/melodydict_simple.txt");
    chcpy::chord2id::load(chord_dic, "../data/chorddict_simple.txt");
    chcpy::hmm::hmm_predict_t model_cpu;
    printf("M:%d N:%d\n", seq_dic.index, chord_dic.index);
    chcpy::hmm::load_text(model_cpu, "../data/model_simple");
    chcpy::gpu::hmm_t model(&context, &model_cpu);  //复制到gpu端
    auto fp = fopen("../outputs/simpleHMM/output.txt", "w");
    chcpy::hmm::melody_t hmm_input;
    std::vector<int> hmm_output;
    clock_t startTime, endTime;
    startTime = clock();  //计时开始
    for (auto line : midiSearch::lineReader("test.txt")) {
        midiSearch::melody_t melody;
        midiSearch::str2melody(line->c_str(), melody);
        printf("音符数量：%d\n", melody.size());
        std::map<int, int> count;
        int note_index = 0;

        midiSearch::chord_t chords;
        chords.push_back(std::vector<int>());
        //填充空间
        std::list<int> buffer;
        for (int i = 0; i < 16; ++i) {
            buffer.push_back(chcpy::seq2id::getIdBySeq(seq_dic, std::vector<int>({0})));
        }
        for (auto note : melody) {
            ++note_index;
            count[note] += 1;
            if (note_index % 4 == 0) {
                int max_note_num = 0;
                int max_note = 0;
                for (auto it : count) {
                    if (it.second > max_note_num) {
                        max_note_num = it.second;
                        max_note = it.first;
                    }
                }
                count.clear();
                buffer.push_back(chcpy::seq2id::getIdBySeq(seq_dic, std::vector<int>({max_note})));
                buffer.pop_front();
                hmm_input.clear();
                hmm_output.clear();
                for (auto it : buffer) {
                    hmm_input.push_back(it);
                }
                chcpy::hmm::predict(model, hmm_input, hmm_output);
                auto out = *hmm_output.rbegin();
                auto& line = model_cpu.A_log[out];
                auto id = chcpy::hmm::argmax(line.begin(), line.end());
                auto outChord = chcpy::chord2id::get(chord_dic, id);
                auto chord_arr = chcpy::string(outChord).split("-");
                std::vector<int> chord_ids;
                for (auto it : chord_arr) {
                    chord_ids.push_back(it.toInt());
                }
                chords.push_back(chord_ids);
                printf("音符：%d\n", note_index);
            }
        }
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
    endTime = clock();  //计时结束
    printf("用时%f秒\n", (float)(endTime - startTime) / 100000);
    if (fp) {
        fclose(fp);
    }
    return 0;
}