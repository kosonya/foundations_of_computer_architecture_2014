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


#ifndef _CACHE_HPP_
#define _CACHE_HPP_


#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

class Cache_block
{
	private:
		uint64_t last_used_cycle;
		uint32_t tag;	
		int valid_bit;
		int dirty_bit;  
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


class Instruction
{
	int type;
	uint32_t *address;
};

class Cache_Statistics
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
};

#endif
