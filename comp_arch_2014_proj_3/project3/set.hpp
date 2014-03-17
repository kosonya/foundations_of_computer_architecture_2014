#ifndef _SET_HPP_
#define _SET_HPP_ 0

#include "cache_block.hpp"
#include "cache_configuration.hpp"
#include <vector>
#include <iostream>

class Set
{
	public:
		Set(Cache_Configuration config);
		~Set();
		friend std::ostream& operator<<(std::ostream& os, const Set& set);
		Cache_block evict_cache_block();
		bool is_hit(uint32_t tag);
		bool has_available_blocks();
		int update_cycle_counter(uint32_t tag, uint64_t cycle); //returns -1 if block is not found; 0 if executed successfuly
	//private:
		unsigned int available_blocks;
		unsigned int block_size;
		unsigned int associativity;
		std::vector<Cache_block> cache_blocks;
};

#endif
