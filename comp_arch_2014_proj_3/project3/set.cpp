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
		if(it -> tag == tag && !(it -> is_available))
		{
			return true;
		}
	}
	return false;
}

int Set::allocate_block_for_write(uint32_t tag, uint64_t cycle)
{
	for(std::vector<Cache_block>::iterator it = cache_blocks.begin(); it != cache_blocks.end(); ++it)
	{
		if(it -> is_available)
		{
			it -> is_available = false;
			it -> tag = tag;
			it -> last_used_cycle = cycle;
			it -> dirty_bit = true;
			available_blocks--;
			return 0;
		}
	}
	return -1;	
}

void Set::set_dirty_bit(uint32_t tag)
{
	for(std::vector<Cache_block>::iterator it = cache_blocks.begin(); it!= cache_blocks.end(); ++it)
	{
		if((it -> tag) == tag)
		{
			it->dirty_bit = true;
		}
	}
}

bool Set::get_dirty_bit(uint32_t tag)
{
	for(std::vector<Cache_block>::iterator it = cache_blocks.begin(); it!= cache_blocks.end(); ++it)
	{
		if((it -> tag) == tag)
		{
			return (it -> dirty_bit);
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
			it -> valid_bit = true;
			available_blocks--;
			return 0;
		}
	}
	return -1;
}

uint32_t Set::find_lru_block()
{
	std::vector<Cache_block>::iterator it = cache_blocks.begin();
	uint32_t res = it -> tag;
	uint64_t lru = it -> last_used_cycle;
	for(++it; it != cache_blocks.end(); ++it)
	{
		if(it -> last_used_cycle < lru)
		{
			lru = it -> last_used_cycle;
			res = it -> tag;
		}
	}
	return res;

}

unsigned long Set::checksum(unsigned long seed) {
	for(std::vector<Cache_block>::iterator it = cache_blocks.begin(); it != cache_blocks.end(); ++it)
	{
		if( !(it -> is_available) )
		{
			seed = seed ^ (( (it -> tag) << 2) | ( (it -> valid_bit) << 1) | (it -> dirty_bit));
		}
	}
	return seed;
}

int Set::evict_block(uint32_t tag)
{
	for(std::vector<Cache_block>::iterator it = cache_blocks.begin(); it != cache_blocks.end(); ++it)
	{		
		if(it -> tag == tag)
		{
			if((it -> dirty_bit) == true)
			{
				//Write it back to memory
			}
			it -> is_available = true;
			available_blocks++;
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
