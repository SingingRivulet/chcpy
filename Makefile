all:
	cd test && make
	cd tool && make
clean:
	-cd test && make clean
	-cd tool && make clean