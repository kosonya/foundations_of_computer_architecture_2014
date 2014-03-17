#include "cache_block.hpp"
#include <iostream>

Cache_block::Cache_block() {
	last_used_cycle = 0;
	tag = 0;
	valid_bit = false;
	dirty_bit = false;
	is_available = true;
}

std::ostream& operator<<(std::ostream& os, const Cache_block& cache_block) {
	os << "Valid bit: " << cache_block.valid_bit;
	os << "; Dirty bit: " << cache_block.dirty_bit;
	os << "; Is available: " << cache_block.is_available;
	os << "; Last used cycle: " << std::dec << cache_block.last_used_cycle;
	os << "; Tag: " << std::hex << "0x" << cache_block.tag;
	return os;
}
