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
#include <vector>


class Cache
{
	public:
		Cache(int cache_size, int associativity, int block_size);
	private:
		int number_of_blocks;
		int cache_size;
		int block_size;
		int associativity;
		std::vector<Set> sets;
};

class Set
{
	public:
		Set(int block_size, int associativity);
	private:
		int available_blocks;
		int block_size;
		int associativity;
		std::vector<Cache_block> cache_blocks;
};

class Cache_block
{
	private:
		unsigned long long int last_used_cycle;
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


