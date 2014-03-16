#ifndef _CACHE_HPP_
#define _CACHE_HPP_

#include "cache.hpp"
#include "cache_configuration.hpp"
#include "set.hpp"
#include <vector>


class Cache
{
	public:
		Cache(Cache_Configuration config);
		get_bits(int size);
		Is_hit(Instruction instruction);
	private:
		unsigned int number_of_blocks;
		unsigned int cache_size;
		unsigned int block_size;
		unsigned int associativity;
		std::vector<Set> sets;
};



#endif
