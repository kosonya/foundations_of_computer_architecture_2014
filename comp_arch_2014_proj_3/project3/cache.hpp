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

class Cache_block
{
	private:
		uint64_t last_used_cycle;
		uint32_t tag;	
		bool valid_bit;
		bool dirty_bit;  
};

class Set
{
	public:
		Set(Cache_Configuration config);
	private:
		unsigned int available_blocks;
		unsigned int block_size;
		unsigned int associativity;
		std::vector<Cache_block> cache_blocks;
};

class Cache
{
	public:
		Cache(Cache_Configuration config);
	private:
		unsigned int number_of_blocks;
		unsigned int cache_size;
		unsigned int block_size;
		unsigned int associativity;
		std::vector<Set> sets;
};

enum Instruction_Type_t {PC = 0, LOAD = 1, STORE = 2};

class Instruction
{
	public:
		Instruction();
		friend std::ostream& operator<<(std::ostream& os, const Instruction& instruction);
		friend std::istream& operator>>(std::istream& is, Instruction& instruction);
		Instruction_Type_t type;
		uint32_t address;
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



#endif
