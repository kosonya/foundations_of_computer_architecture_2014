#ifndef _CACHE_CONFIGURATION_HPP_
#define _CACHE_CONFIGURATION_HPP_ 0

#include <iostream>

class Cache_Configuration {
	public:
		friend std::ostream& operator<<(std::ostream& os, const Cache_Configuration& cache_configuration);
		friend std::istream& operator>>(std::istream& is, Cache_Configuration& cache_configuration);
		unsigned int cache_size, block_size, associativity;

};

class Cache_Configurations {
	public:
		friend std::ostream& operator<<(std::ostream& os, const Cache_Configurations& cache_configurations);
		friend std::istream& operator>>(std::istream& is, Cache_Configurations& cache_configurations);
		Cache_Configuration i_cache, d_cache, l2_cache;
		uint64_t start_counting_after;
};

#endif
