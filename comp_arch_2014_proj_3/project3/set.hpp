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
		int evict_block(uint32_t tag); //returns -1 if block is not found; 0 if evicted successfuly
		bool is_hit(uint32_t tag);
		bool has_available_blocks();
		int update_cycle_counter(uint32_t tag, uint64_t cycle); //returns -1 if block is not found; 0 if executed successfuly
		int allocate_block(uint32_t tag, uint64_t cycle); //returns -1 if no space is found; 0 if executed successfuly
		uint32_t find_lru_block(); //returns the tag of least recently used block within this set. Should be only called if there's no available blocks
		int allocate_block_for_write(uint32_t tag, uint64_t cycle);
		void set_dirty_bit(uint32_t tag);
		bool get_dirty_bit(uint32_t tag);
	private:
		unsigned int available_blocks;
		unsigned int block_size;
		unsigned int associativity;
		std::vector<Cache_block> cache_blocks;
};

#endif
