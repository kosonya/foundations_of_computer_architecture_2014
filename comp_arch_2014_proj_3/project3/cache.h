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
	int number_of_blocks = (cache_size)/(block_size * associativity);
	Cache_block cache_block[number_of_blocks];
};

class Set
{
	unsigned int index;
	Cache cache;
	int lru_count[cache.associativity];
};

class Cache_block
{
	unsigned int tag;	
	int valid_bit;
	int dirty_bit;  
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
void place_cache_block(int index);
void evict_cache_block(int index);
bool find_cache_entry(instruction *inst);
void calculate_statistics(cache_statistics *stats);
void print_statistics(cache_statistics *statistics);


