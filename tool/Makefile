CC = g++ -fcoroutines -fPIC -std=c++20 -ggdb -I../chcpy ../chcpy/cJSON.c
all:listBuilder hmm_train dict_builder chordtime_builder bayes_train midirebuild

midirebuild:midirebuild.cpp
	$(CC) midirebuild.cpp ../dep/midifile/lib/libmidifile.a \
	 -I ../dep/midifile/include -o midirebuild

listBuilder:listBuilder.cpp
	$(CC) listBuilder.cpp -o listBuilder

bayes_train:bayes_train.cpp
	$(CC) bayes_train.cpp -o bayes_train

hmm_train:hmm_train.cpp
	$(CC) hmm_train.cpp -o hmm_train

hmmv3_train:hmmv3_train.cpp
	$(CC) hmmv3_train.cpp -o hmmv3_train

dict_builder:dict_builder.cpp
	$(CC) dict_builder.cpp -o dict_builder

chordtime_builder:chordtime_builder.cpp
	$(CC) chordtime_builder.cpp -o chordtime_builder

dataset_check:dataset_check.cpp
	$(CC) dataset_check.cpp -o dataset_check

lenMax:lenMax.cpp
	$(CC) lenMax.cpp -o lenMax

sampleConvert:sampleConvert.cpp
	$(CC) sampleConvert.cpp -o sampleConvert

clean:
	-rm listBuilder hmm_train dict_builder hmmv3_train chordtime_builder bayes_train midirebuild