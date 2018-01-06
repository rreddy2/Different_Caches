all: cache-sim

cache-sim:	prog2.cpp
	g++  prog2.cpp -g -o cache-sim

clean:
	rm -f *.o cache-sim
