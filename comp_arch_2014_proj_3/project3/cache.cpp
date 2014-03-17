#include "cache.hpp"
#include "cache_configuration.hpp"
#include "set.hpp"
#include <vector>
#include <iostream>
#include <bitset>
#include <cstdint>

Cache::Cache(Cache_Configuration config) {
	Set set(config);
	cache_size = config.cache_size;
	block_size = config.block_size;
	associativity = config.associativity;
	number_of_sets = cache_size / (block_size * associativity);
	block_offset_bit_width = get_bit_width(block_size-1);
	index_bit_width = get_bit_width(number_of_sets-1);
	tag_bit_width = 32 - (index_bit_width + block_offset_bit_width);
	for(unsigned i = 0; i < number_of_sets; i++) {
		sets.push_back(set);
	}
}

Cache::~Cache() {
	sets.clear();
}

unsigned int Cache::get_bit_width(unsigned int size)
{
	unsigned int bit_width;
	for(bit_width = 0; size > 0; bit_width++) {
		size = size / 2;
	}
	return bit_width;
}


uint32_t Cache::get_block_offset(uint32_t address)
{
	return address % (1 << block_offset_bit_width);
}

uint32_t Cache::get_index(uint32_t address)
{
	return (address >> block_offset_bit_width) % (1 << index_bit_width);
}

uint32_t Cache::get_tag(uint32_t address)
{
	return (address >> (block_offset_bit_width + index_bit_width));
}

bool Cache::is_hit(uint32_t address)
{
	return sets[this -> get_index(address)].is_hit(this -> get_tag(address));	   
}

bool Cache::has_available_blocks(uint32_t address)
{
	return sets[this -> get_index(address)].has_available_blocks();
}

int Cache::update_cycle_counter(uint32_t address, uint64_t cycle)
{
	return sets[this -> get_index(address)].update_cycle_counter(this -> get_tag(address), cycle);
}

int Cache::allocate_block(uint32_t address, uint64_t cycle)
{
	return sets[this -> get_index(address)].allocate_block(this -> get_tag(address), cycle);
}

uint32_t Cache::find_lru_block(uint32_t address)
{
	return (sets[this -> get_index(address)].find_lru_block() << (block_offset_bit_width + index_bit_width)) | ( (this -> get_index(address)) << block_offset_bit_width);

}

std::ostream& operator<<(std::ostream& os, const Cache& cache) {
	os << "Cache size: " << std::dec << cache.cache_size;
	os << "; Block size: " << std::dec << cache.block_size;
	os << "; Associativity: " << std::dec << cache.associativity;
	os << "; Number of sets: " << std::dec << cache.number_of_sets;
	os << "; Block offset bits: " << std::dec << cache.block_offset_bit_width;
	os << "; Index bits: " << std::dec << cache.index_bit_width;
	os << "; Tag bits: " << std::dec << cache.tag_bit_width << std::endl;
	for(unsigned int index = 0; index < cache.sets.size(); index++) {
		os << "Set " << std::hex << "0x" << index << std::endl;
		os << cache.sets[index];
	}
	return os;
}
