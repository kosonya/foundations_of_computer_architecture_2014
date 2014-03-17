#include "set.hpp"
#include "cache_configuration.hpp"
#include "cache_block.hpp"
#include <iostream>

Set::Set(Cache_Configuration config) {
	Cache_block cache_block;
	block_size = config.block_size;
	associativity = config.associativity;
	available_blocks = associativity;
	for(unsigned int i = 0; i < associativity; i++) {
		cache_blocks.push_back(cache_block);
	}
}

Set::~Set() {
	cache_blocks.clear();
}

Cache_block Set::evict_cache_block()
{
	//uint64_t oldest_block_cycle = cache_blocks[i - 1].last_used_cycle;
	uint64_t oldest_block_cycle = cache_blocks[0].last_used_cycle;
	Cache_block oldest_block;
/*	unsigned int i;
	 for(i = 1; i < associativity; i++)
	 {
	 	if(cache_blocks[i].last_used_cycle < oldest_block)
	 	{
	 		oldest_block_cycle = cache_blocks[i].last_used_cycle;
	 		oldest_block = cache_blocks[i];
	 	}
	 }*/
	 return oldest_block;
}

bool Set::has_available_blocks()
{
	return available_blocks > 0;
}

int Set::update_cycle_counter(uint32_t tag, uint64_t cycle)
{
	for(std::vector<Cache_block>::iterator it = cache_blocks.begin(); it != cache_blocks.end(); ++it)
	{
		if(it -> tag == tag && !(it -> is_available) )
		{
			it -> last_used_cycle = cycle;
			return 0;
		}
	}
	return -1;
}

bool Set::is_hit(uint32_t tag)
{
	if (available_blocks == associativity)
	{
		return false;
	}
	for(std::vector<Cache_block>::iterator it = cache_blocks.begin(); it != cache_blocks.end(); ++it)
	{
		if(it -> tag == tag && !(it -> is_available) )
		{
			return true;
		}
	}
	return false;
}

int Set::allocate_block(uint32_t tag, uint64_t cycle)
{
	for(std::vector<Cache_block>::iterator it = cache_blocks.begin(); it != cache_blocks.end(); ++it)
	{
		if(it -> is_available)
		{
			it -> is_available = false;
			it -> tag = tag;
			it -> last_used_cycle = cycle;
			available_blocks--;
			return 0;
		}
	}
	return -1;
}

std::ostream& operator<<(std::ostream& os, const Set& set) {
	os << "\t" << "Block size: " << std::dec << set.block_size;
	os << "; Associativity: " << std::dec << set.associativity;
	os << "; Available blocks: " << std::dec << set.available_blocks;
	os << std::endl << "\tBlocks:" << std::endl;
	for(unsigned int block = 0; block < set.cache_blocks.size(); block++) {
		os << "\t\t" << set.cache_blocks[block] << std::endl;
	}
	return os;
}
