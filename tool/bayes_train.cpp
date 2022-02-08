#include "chordNext.h"
#include "filereader.h"
int main() {
    chcpy::chordNext::dict_t dict;
    chcpy::bayes::bayes_train_t model;
    dict.load("../data/chordtimedict.txt");
    printf("和弦数量：%d\n", dict.chord_id.size());

    for (auto data : midiSearch::lineReader("../data/chordtime.txt")) {
        chcpy::chordNext::trainLine(dict, model, *data);
    }

    model.normalize();
    model.save("../data/bayesmodel");
    return 0;
}