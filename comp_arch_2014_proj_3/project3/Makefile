CPPCOMPILER = g++
CPPFLAGS = -Wall -std=gnu++0x -O3
EXECUTABLE = cache_simulator
OBJECTS = cache_block.o cache_configuration.o cache.o cache_statistics.o instruction.o main.o set.o

all: cache_simulator

clean:
	rm $(OBJECTS)

cache_simulator: $(OBJECTS)
	$(CPPCOMPILER) -o $(EXECUTABLE) $(OBJECTS)

cache_block.o: cache_block.cpp cache_block.hpp
	$(CPPCOMPILER) $(CPPFLAGS) -c -o cache_block.o cache_block.cpp

cache_configuration.o: cache_configuration.cpp cache_configuration.hpp
	$(CPPCOMPILER) $(CPPFLAGS) -c -o cache_configuration.o cache_configuration.cpp

cache.o: cache.cpp cache.hpp cache_configuration.hpp set.hpp cache_statistics.hpp
		$(CPPCOMPILER) $(CPPFLAGS) -c -o cache.o cache.cpp

cache_statistics.o: cache_statistics.cpp cache_statistics.hpp
	$(CPPCOMPILER) $(CPPFLAGS) -c -o cache_statistics.o cache_statistics.cpp

instruction.o: instruction.cpp instruction.hpp
	$(CPPCOMPILER) $(CPPFLAGS) -c -o instruction.o instruction.cpp

set.o: set.cpp set.hpp cache_configuration.hpp cache_block.hpp
	$(CPPCOMPILER) $(CPPFLAGS) -c -o set.o set.cpp

main.o: main.cpp cache.hpp cache_configuration.hpp instruction.hpp
	$(CPPCOMPILER) $(CPPFLAGS) -c -o main.o main.cpp
