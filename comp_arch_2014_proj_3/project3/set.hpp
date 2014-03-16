#ifndef _SET_HPP_
#define _SET_HPP_ 0

#include "cache_block.hpp"
#include "cache_configuration.hpp"
#include <vector>

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

#endif
