#ifndef _CACHE_HPP_
#define _CACHE_HPP_

#include "cache.hpp"
#include "cache_configuration.hpp"
#include "set.hpp"
#include <vector>
#include <cstdint>

class Cache
{
	public:
		Cache(Cache_Configuration config);
		~Cache();
		bool is_hit(uint32_t address);
		friend std::ostream& operator<<(std::ostream& os, const Cache& cache);
		uint32_t get_index(uint32_t address);
		uint32_t get_block_offset(uint32_t address);
		uint32_t get_tag(uint32_t address);
		bool has_available_blocks(uint32_t address);
		int update_cycle_counter(uint32_t address, uint64_t cycle); //returns -1 if block is not found; 0 if executed successfuly
		int allocate_block(uint32_t address, uint64_t cycle); //returns -1 if no space is found; 0 if executed successfuly


		uint32_t find_lru_block(uint32_t address);
		//returns the address (with block offset bits all equal to 0) of the least recently used block
		//within the set where the input address could be put


		
	private:

		unsigned int get_bit_width(unsigned int size);

		unsigned int number_of_sets;
		unsigned int cache_size;
		unsigned int block_size;
		unsigned int associativity;
		std::vector<Set> sets;
		unsigned int index_bit_width;
		unsigned int block_offset_bit_width;
		unsigned int tag_bit_width;
		
};



#endif
