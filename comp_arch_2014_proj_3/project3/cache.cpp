#include "cache.hpp"
#include "cache_configuration.hpp"
#include <vector>





Cache::Cache(Cache_Configuration config) {
	cache_size = config.cache_size;
	block_size = config.block_size;
	associativity = config.associativity;
	number_of_blocks = cache_size / (block_size * associativity);
}









