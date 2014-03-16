#include "cache.hpp"
#include "cache_configuration.hpp"
#include "set.hpp"
#include <vector>
#include <iostream>
#include <bitset>

Cache::Cache(Cache_Configuration config) {
	Set set(config);
	cache_size = config.cache_size;
	block_size = config.block_size;
	associativity = config.associativity;
	number_of_sets = cache_size / (block_size * associativity);
	for(unsigned i = 0; i < number_of_sets; i++) {
		sets.push_back(set);
	}
}

Cache::~Cache() {
	sets.clear();
}

std::ostream& operator<<(std::ostream& os, const Cache& cache) {
	os << "Cache size: " << cache.cache_size;
	os << "; Block size: " << cache.block_size;
	os << "; Associativity: " << cache.associativity;
	os << "; Number of sets: " << cache.number_of_sets << std::endl;
	for(unsigned int index = 0; index < cache.sets.size(); index++) {
		os << "Set " << std::hex << index << std::endl;
		os << cache.sets[index];
	}
	return os;
}
