#include "cache_configuration.hpp"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Cache_Configuration& cache_configuration) {
	os << "Cache size: " << std::dec << cache_configuration.cache_size;
	os << "; Block size: " << std::dec << cache_configuration.block_size;
	os << "; Associativity: " << cache_configuration.associativity;
	return os;
}

std::ostream& operator<<(std::ostream& os, const Cache_Configurations& cache_configurations){
	os << "I-cache: " << cache_configurations.i_cache << std::endl;
	os << "D-cache: " << cache_configurations.d_cache << std::endl;
	os << "L2-cache: " << cache_configurations.l2_cache;
	return os;
}

std::istream& operator>>(std::istream& is, Cache_Configuration& cache_configuration) {
	is >> cache_configuration.cache_size >> cache_configuration.block_size >> cache_configuration.associativity;
	return is;
}

std::istream& operator>>(std::istream& is, Cache_Configurations& cache_configurations){
	is >> cache_configurations.i_cache >> cache_configurations.d_cache >> cache_configurations.l2_cache;
	return is;
}

Cache_Configurations::Cache_Configurations() {
	i_cache.cache_size = 8192;
	i_cache.block_size = 32;
	i_cache.associativity = 1;
	d_cache.cache_size = 8192;
	d_cache.block_size = 32;
	d_cache.associativity = 4;
	l2_cache.cache_size = 65536;
	l2_cache.block_size = 32;
	l2_cache.associativity = 2;
	start_counting_after = 1000000;
}
