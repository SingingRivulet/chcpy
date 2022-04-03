CC = g++ -fcoroutines -fPIC -std=c++20 -fconcepts-diagnostics-depth=4 -ggdb -I../chcpy ../chcpy/cJSON.c
all:calcEditDist chordGen chordGen_gpu chordNext realtime tone

calcEditDist:calcEditDist.cpp
	$(CC) calcEditDist.cpp -o calcEditDist

chordNext:chordNext.cpp
	$(CC) chordNext.cpp -o chordNext

tone:tone.cpp
	$(CC) tone.cpp -fopenmp -o tone

realtime:realtime.cpp
	$(CC) realtime.cpp -lGLESv2 -lEGL -fopenmp -o realtime

chordGen:chordGen.cpp
	$(CC) chordGen.cpp -fopenmp -o chordGen

chordGen_gpu:chordGen_gpu.cpp
	$(CC) chordGen_gpu.cpp -lGLESv2 -lEGL -fopenmp -o chordGen_gpu

chordGenv3_gpu:chordGenv3_gpu.cpp
	$(CC) chordGenv3_gpu.cpp -lGLESv2 -lEGL -fopenmp -o chordGenv3_gpu

simpleHMM:simpleHMM.cpp
	$(CC) simpleHMM.cpp -lGLESv2 -lEGL -fopenmp -o simpleHMM

simpleHMM_train:simpleHMM_train.cpp
	$(CC) simpleHMM_train.cpp -lGLESv2 -lEGL -fopenmp -o simpleHMM_train

clean:
	-rm chordGen chordGen.out calcEditDist chordGenv3_gpu chordGen_gpu chordNext realtime tone