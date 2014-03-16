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
#include <fstream>


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
	unsigned int clean_evictions;
	unsigned int dirty_writebacks;
	unsigned int forced_clean_evictions;
	unsigned int forced_dirty_evictions;
};

class Cache_Configuration {
	public:
		friend std::ostream& operator<<(std::ostream& os, const Cache_Configuration& cache_configuration);
		friend std::istream& operator>>(std::istream& is, Cache_Configuration& cache_configuration);
		unsigned int cache_size, block_size, associativity;

};

class Cache_Configurations {
	public:
		friend std::ostream& operator<<(std::ostream& os, const Cache_Configurations& cache_configurations);
		friend std::istream& operator>>(std::istream& is, Cache_Configurations& cache_configurations);
		Cache_Configuration i_cache, d_cache, l2_cache;
		uint64_t start_counting_after;
};

#endif
