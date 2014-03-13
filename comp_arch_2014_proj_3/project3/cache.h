/* Cache simulator header file */
//
// cache.header
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Cache data structures
//
// All code changes go to cache.c
//
//


#ifndef _cache_
#define _cache_
#include <iostream>
#include <string>


class Cache
{
	public:
	int cache_type;
	int cache_size;
	int block_size;
	int associativity;
};

typedef struct
{
	unsigned int tag;
	int valid;
	int dirty_bit;
}cache_entry;

class cache_block
{
	int index;
    /* array of cache entries */
    cache_entry c_entry[block_size];
    int lru_count[block_size];
};

typedef struct 
{
	int type;
	char *address;
}instruction;

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

void init_cache();
void read_config_file();
bool find_cache_entry(instruction *inst);
void place_cache_block(int index);
void evict_cache_block(int index);
void calculate_statistics(cache_statistics *stats);
void print_statistics(cache_statistics *statistics);


