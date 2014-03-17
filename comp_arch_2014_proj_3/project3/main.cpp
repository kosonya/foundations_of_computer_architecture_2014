#include "cache.hpp"
#include "cache_configuration.hpp"
#include "instruction.hpp"

#include <iostream>
#include <fstream>


class Cach_Access_Result {
	public:
		bool is_hit;
		bool allocated_without_eviction;
		uint32_t evicted_address;
};

Cach_Access_Result cache_read(Cache *cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info = false);



int main() {
	uint64_t current_cycle;
	Cache_Configurations cache_configurations;
	Instruction instruction;
	uint64_t address_to_evict;
	Cach_Access_Result cach_access_result;


	std::ifstream config_file("config.default");

	if(config_file.is_open()) {
		config_file >> cache_configurations;
		config_file.close();
	} else {
		std::cout << "Something is wrong with confing file; using default parameters" << std::endl;
	}


	std::cout << cache_configurations << std::endl;



	Cache *test_cache;
	test_cache = new Cache(cache_configurations.i_cache);
	std::cout << *test_cache << std::endl;


	for(current_cycle = 0; !std::cin.eof(); current_cycle++) {

		std::cin >> instruction;
		if (std::cin.eof()) break; //Fixing some wierd stuff with cin
		std::cout << "Cycle: " << std::dec << current_cycle << std::endl;
		std::cout << instruction << std::endl;

		cach_access_result = cache_read(test_cache, instruction, current_cycle, true);

		std::cout << std::endl << std::endl << std::endl;

	}

	delete test_cache;


	return 0;
}


Cach_Access_Result cache_read(Cache *cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info) {
	Cach_Access_Result res;
	uint64_t address_to_evict;
	if(show_debug_info) {
		std::cout << "Block offset: " << std::hex << "0x" << cache -> get_block_offset(instruction.get_address());
		std::cout << "; Index: " << std::hex << "0x" << cache -> get_index(instruction.get_address());
		std::cout << "; Tag: " << std::hex << "0x" << cache -> get_tag(instruction.get_address()) << std::endl;
	}

	if (cache -> is_hit(instruction.get_address())) {
		cache -> update_cycle_counter(instruction.get_address(), current_cycle);
		res.is_hit = true;
		if(show_debug_info) {
			std::cout << "Hit, updating cycle counter"<< std::endl;
			std::cout << *cache << std::endl;
		}
	} else {
		if(show_debug_info) {
			std::cout << "Miss, chechking if we can allocate a block without evictions"<< std::endl;
		}
		if(cache -> has_available_blocks(instruction.get_address())) {
			cache -> allocate_block(instruction.get_address(), current_cycle);
			if(show_debug_info) {
				std::cout << "Some space available, allocating" << std::endl;
				std::cout << *cache << std::endl;
			}
			res.allocated_without_eviction = true;
		} else {

			address_to_evict = cache -> find_lru_block(instruction.get_address());

			cache -> evict_block(address_to_evict);
			cache -> allocate_block(instruction.get_address(), current_cycle);

			res.evicted_address=address_to_evict;

			if(show_debug_info) {
				std::cout << "No available blocks, we need to evict something" << std::endl;
				std::cout << "Will evict the block with index 0x" << std::hex <<  cache -> get_index(address_to_evict);
				std::cout << " and tag 0x" << std::hex << cache -> get_tag(address_to_evict) << std::endl;
				std::cout << *cache << std::endl;

			}
			res.allocated_without_eviction = false;
		}
		res.is_hit = false;
	}
	return res;
}
