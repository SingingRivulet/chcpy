CC = g++ -I/usr/include/x86_64-linux-gnu/qt5 -fcoroutines -fPIC -lQt5Core -std=c++20 -ggdb -I../chcpy ../chcpy/cJSON.c
all:calcEditDist chordGen

calcEditDist:calcEditDist.cpp
	$(CC) calcEditDist.cpp -o calcEditDist

chordGen:chordGen.cpp
	$(CC) chordGen.cpp -fopenmp -o chordGen

clean:
	rm chordGen chordGen.out calcEditDist