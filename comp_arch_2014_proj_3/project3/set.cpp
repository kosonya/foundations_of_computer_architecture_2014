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

void Set::evict_cache_block()
{
	uint64_t oldest_block;
	 for(unsigned int i = 1; i < associativity; i++)
	 {
	 	if(cache_blocks[i].last_used_cycle < cache_blocks[i - 1].last_used_cycle)
	 	{
	 		oldest_block = cache_blocks[i].last_used_cycle;
	 	}
	 }
}


Set::~Set() {
	cache_blocks.clear();
}

std::ostream& operator<<(std::ostream& os, const Set& set) {
	os << "\t" << "Block size: " << set.block_size;
	os << "; Associativity: " << set.associativity;
	os << "; Available blocks: " << set.available_blocks;
	os << std::endl << "\tBlocks:" << std::endl;
	for(unsigned int block = 0; block < set.cache_blocks.size(); block++) {
		os << "\t\t" << set.cache_blocks[block] << std::endl;
	}
	return os;
}
