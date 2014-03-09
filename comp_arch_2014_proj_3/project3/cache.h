/* Cache simulator header file */
//
// cache.h
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Cache data structures
//
// All code changes go to cache.c
//
//
#include <stdio.h>

#ifndef _cache_
#define _cache_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct 
{
	unsigned int tag;
	int valid;
}cache_entry;

typedef struct 
{
	int cache_size;
	int block_size;
	int associativity;
}cache;

typedef struct 
{
	unsigned int accesses;
	unsigned int misses;
	unsigned int reads;
	unsigned int read_misses;
	unsigned int writes;
	unsigned int write_misses;
	int clean_evictions;
	int dirty_writebacks;
	int forced_clean_evictions;
	int forced_dirty_evictions;
}cache_statistics;