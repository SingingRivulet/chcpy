#define CHCPY_DEBUG
#include "chordNext.h"
#include "bayes.h"
int main() {
    chcpy::chordNext::dict_t dict;
    dict.load("../data/chordtimedict.txt");
    chcpy::bayes::bayes_predict_t model;
    model.load("../data/bayesmodel");
    chcpy::activeBuffer buffer;

    buffer.pushChord("0-4-7");
    buffer.pushChord("0-4-7");
    buffer.pushChord("5-9-2");
    buffer.pushChord("5-9-2");

    std::vector<std::string> realtime;
    std::vector<chcpy::activeBuffer::chordtime> res;
    buffer.buildRealtimeBuffer(realtime, res);

    for (auto& it : res) {
        printf("%s=%d\n", std::get<0>(it).c_str(), std::get<1>(it));
    }

    auto max_id = chcpy::chordNext::predictNext(dict, model, res);

    auto it = dict.id_chord.find(max_id);
    if (it != dict.id_chord.end()) {
        printf("预测结果:id=%d\n", max_id);
        printf("和弦:%s\n", std::get<0>(it->second).c_str());
        printf("时长:%d\n", std::get<1>(it->second));
    }

    //getchar();
    return 0;
}