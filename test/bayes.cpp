#include "bayes.h"
int main() {
    chcpy::bayes::bayes_train_t model;
    model.add({1, 2, 3, 4, 5}, 9);
    model.add({1, 2, 3, 4, 6}, 10);
    model.add({1, 2, 3, 5, 6}, 9);
    model.normalize();
    model.save("1.out");
    return 0;
}