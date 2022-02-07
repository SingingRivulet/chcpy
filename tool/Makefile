CC = g++ -I/usr/include/x86_64-linux-gnu/qt5 -fcoroutines -fPIC -std=c++20 -ggdb -I../chcpy ../chcpy/cJSON.c
all:listBuilder hmm_train dict_builder chordtime_builder 

listBuilder:listBuilder.cpp
	$(CC) listBuilder.cpp -o listBuilder

hmm_train:hmm_train.cpp
	$(CC) hmm_train.cpp -o hmm_train

hmmv3_train:hmmv3_train.cpp
	$(CC) hmmv3_train.cpp -o hmmv3_train

dict_builder:dict_builder.cpp
	$(CC) dict_builder.cpp -o dict_builder

chordtime_builder:chordtime_builder.cpp
	$(CC) chordtime_builder.cpp -o chordtime_builder

clean:
	-rm listBuilder hmm_train dict_builder hmmv3_train chordtime_builder