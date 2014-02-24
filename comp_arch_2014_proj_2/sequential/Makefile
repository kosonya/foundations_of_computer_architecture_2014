###########################################################################3
#
# Makefile
# Copyright (C) 2005 Carnegie Mellon University
#
# 18-741 Fall 2005 Project 3 Makefile
#
#


GIVEN_OBJECTS = tomasulo_sim.o
OBJECTS = tomasulo.o
SOURCES = tomasulo.c 
HEADERS = tomasulo.h 
EXECFILE = tomasulo_sim 
 
$(EXECFILE): $(OBJECTS) $(GIVEN_OBJECTS)
	gcc $(OBJECTS) $(GIVEN_OBJECTS) -o $(EXECFILE)
$(OBJECTS): $(SOURCES) $(HEADERS)
	gcc -g -c $(SOURCES)
clean:
	rm -rf $(OBJECTS) $(EXECFILE) 
