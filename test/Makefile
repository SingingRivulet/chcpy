CC = g++ -fcoroutines -fPIC -std=c++20 -fconcepts-diagnostics-depth=4 -ggdb -I../chcpy ../chcpy/cJSON.c
all:calcEditDist chordGen chordGen_gpu chordNext realtime

calcEditDist:calcEditDist.cpp
	$(CC) calcEditDist.cpp -o calcEditDist

chordNext:chordNext.cpp
	$(CC) chordNext.cpp -o chordNext

realtime:realtime.cpp
	$(CC) realtime.cpp -lGLESv2 -lEGL -fopenmp -o realtime

chordGen:chordGen.cpp
	$(CC) chordGen.cpp -fopenmp -o chordGen

chordGen_gpu:chordGen_gpu.cpp
	$(CC) chordGen_gpu.cpp -lGLESv2 -lEGL -fopenmp -o chordGen_gpu

chordGenv3_gpu:chordGenv3_gpu.cpp
	$(CC) chordGenv3_gpu.cpp -lGLESv2 -lEGL -fopenmp -o chordGenv3_gpu

clean:
	-rm chordGen chordGen.out calcEditDist chordGenv3_gpu chordGen_gpu chordNext realtime