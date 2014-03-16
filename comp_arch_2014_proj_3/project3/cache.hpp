#ifndef _CACHE_HPP_
#define _CACHE_HPP_

#include "cache.hpp"
#include "cache_configuration.hpp"
#include "set.hpp"
#include <vector>
#include <iostream>

class Cache
{
	public:
		Cache(Cache_Configuration config);
		~Cache();
		friend std::ostream& operator<<(std::ostream& os, const Cache& cache);
	//private:
		unsigned int number_of_sets;
		unsigned int cache_size;
		unsigned int block_size;
		unsigned int associativity;
		std::vector<Set> sets;
};


#endif
