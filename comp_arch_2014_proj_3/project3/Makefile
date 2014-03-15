all: cache

cache: cache.o
	g++ cache.o -o chache

cache.o: cache.hpp cache.cpp
	g++ cache.cpp -c -std=gnu++0x -o cache.o

clean:
	rm cache.o
