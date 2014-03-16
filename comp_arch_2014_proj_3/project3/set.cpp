#include "set.hpp"
#include "cache_configuration.hpp"

Set::Set(Cache_Configuration config) {
	block_size = config.block_size;
	associativity = config.associativity;
}
