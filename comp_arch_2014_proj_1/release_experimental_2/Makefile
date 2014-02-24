#---------------------------------------------------------------
#
# Makefile
# Copyright (C) 2005 Carnegie Mellon University
#
# Description:
# Makefile for 18640 project 1 branch prediction
#
# 

CFLAGS = -g -Wall

GIVEN_OBJ = main.o
OBJECTS = predictor.o
SOURCES = predictor.c
HEADERS = predictor.h 
EXECFILE = predictor

$(EXECFILE): $(OBJECTS) 
	g++ $(OBJECTS) $(GIVEN_OBJ) -o $(EXECFILE)
$(OBJECTS): $(SOURCES) $(HEADERS)
	g++ $(CFLAGS) -c $(SOURCES)
	
clean:
	rm -rf predictor.o $(EXECFILE) 

