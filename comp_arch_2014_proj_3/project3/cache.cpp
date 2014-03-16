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

int Cache::get_bits(int size)
{
	int count = 0;
	while(size!= 1)
	{
		count++;
		size = size/2;
	}
	return count;
}


bool Cache::Is_hit(Instruction inst)
{
	unsigned int index, tag, current_set;
	uint32_t address;
	address = inst.get_instruction_address();
    index = (address >> (Cache.get_bits(block_size))) % (1 << (Cache.get_bits(number_of_sets)));
    tag = address>>((Cache.get_bits(block_size))+ (Cache.get_bits(number_of_sets)));
    /* Iterate through the sets */
	for(int i = 0; i < number_of_sets; i++)
	{
		if(index == sets[i])
		{
			current_set = sets[i];
		}
	}
    /* Iterate through the cache blocks */
	for(int i = 0; i < associativity; i++)
	{
		if(tag == current_set.cache_blocks[i].tag)
		{
			return true;
			break;
		}
	}

	return false;	   
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
