#include "cache_configuration.hpp"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Cache_Configuration& cache_configuration) {
	os << "Cache size: " << cache_configuration.cache_size;
	os << "; Block size: " << cache_configuration.block_size;
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
