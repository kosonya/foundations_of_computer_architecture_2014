#ifndef _CACHE_BLOACK_HPP_
#define _CACHE_BLOACK_HPP_

#include <cstdint>

class Cache_block
{
	private:
		uint64_t last_used_cycle;
		uint32_t tag;	
		bool valid_bit;
		bool dirty_bit;  
};

#endif
