#---------------------------------------------------------------
#
# Makefile
# Copyright (C) 2005 Carnegie Mellon University
#
# Description:
# Makefile for Project 2
#
# Revision history # 9-24-2005, Eric Chung


CFLAGS = -O3 
#DEBUG = -DDEBUG=
#ASSERT = -DASSERT=


#------------------------------------------------------------------------------------------------
# Periodically dump statistics every N number of references processed

DUMP_INTERVAL = -DDUMP_INTERVAL=10000000000


#------------------------------------------------------------------------------------------------
# Prefetcher flags

ENABLE_PF = -DENABLE_PF=          # Enable the Prefetcher
#DEBUG_PF = -DDEBUG_PF=


#------------------------------------------------------------------------------------------------
# Strided prefetcher, number of entries
# You need to use "HIST" as a preprocessor-defined parameter in your strided prefetcher

HIST = -DHIST=128


#------------------------------------------------------------------------------------------------
# Auxiliary cache settings

AUX_SIZE = -DAUX_SIZE=16          # Number of entries to use for an auxiliary cache size


#------------------------------------------------------------------------------------------------
# Type of auxiliary buffer (only 1 should be uncommented out)

#EN_FIFO_AUX = -DEN_FIFO_AUX       # If auxiliary prefetch buffer uses a FIFO replacement policy
EN_DM_AUX = -DEN_DM_AUX          # If auxiliary prefetch buffer uses a Direct-mapped structure
#EN_RAND_AUX = -DEN_RAND_AUX      # If auxiliary prefetch buffer uses a random policy


base_obj = CacheArray.o CacheController.o PrefetchBuffer.o 
base_src = CacheArray.cpp CacheController.cpp PrefetchBuffer.cpp 

comp_obj = CompetitionPrefetcher.o
comp_src = CompetitionPrefetcher.cpp

PF_FLAGS = $(ENABLE_PF) $(DEBUG_PF) $(AUX_SIZE) $(EN_FIFO_AUX) $(EN_DM_AUX) $(EN_RAND_AUX)


#------------------------------------------------------------------------------------------------
# Build targets


competition: $(base_obj) $(comp_obj)
	g++ $(DEBUG) $(ASSERT) $(CFLAGS) $(DUMP_INTERVAL) $(PF_FLAGS) -o pf_competition $(base_obj) $(comp_obj) Wiring.o


Wiring: Wiring.cpp
CacheArray: CacheArray.h
CacheController: CacheController.h
PrefetchBuffer: PrefetchBuffer.h
CompPrefetcher: CompetitionPrefetcher.h


$(base_obj): $(base_src) 
	g++ $(DEBUG) $(ASSERT) $(CFLAGS) $(PF_FLAGS) $(DUMP_INTERVAL) -c $(base_src)

$(comp_obj): $(comp_src)
	g++ $(DEBUG) $(ASSERT) $(CFLAGS) $(PF_FLAGS) $(DUMP_INTERVAL) -DCOMPETITION= -c $(comp_src) Wiring.cpp

clean:
	rm $(base_obj) $(comp_obj) Wiring.o pf_competition 
