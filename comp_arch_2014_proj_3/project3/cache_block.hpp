#ifndef _CACHE_BLOACK_HPP_
#define _CACHE_BLOACK_HPP_

#include <cstdint>
#include <iostream>

class Cache_block
{
	public:
		Cache_block();
		friend std::ostream& operator<<(std::ostream& os, const Cache_block& cache_block);
		uint64_t last_used_cycle;
		uint32_t tag;	
		bool valid_bit;
		bool dirty_bit;
		bool is_available;
};

#endif
