CC = g++ -I/usr/include/x86_64-linux-gnu/qt5 -fcoroutines -fPIC -lQt5Core -std=c++20 -ggdb -I../chcpy ../chcpy/cJSON.c
all:listBuilder hmm_train dict_builder 

listBuilder:listBuilder.cpp
	$(CC) listBuilder.cpp -o listBuilder
hmm_train:hmm_train.cpp
	$(CC) hmm_train.cpp -o hmm_train
dict_builder:dict_builder.cpp
	$(CC) dict_builder.cpp -o dict_builder
clear:
	rm listBuilder hmm_train dict_builder