#include "cache.hpp"
#include "cache_configuration.hpp"
#include "instruction.hpp"

#include <iostream>
#include <fstream>




int main() {
	uint64_t current_cycle;
	Cache_Configurations cache_configurations;
	Instruction instruction;
	uint64_t address_to_evict;


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
		std::cout << "Cycle: " << std::dec << current_cycle << std::endl;
		std::cout << instruction << std::endl;
		std::cout << "Block offset: " << std::hex << "0x" << test_cache -> get_block_offset(instruction.get_address());
		std::cout << "; Index: " << std::hex << "0x" << test_cache -> get_index(instruction.get_address());
		std::cout << "; Tag: " << std::hex << "0x" << test_cache -> get_tag(instruction.get_address()) << std::endl;
		if (test_cache -> is_hit(instruction.get_address())) {
			std::cout << "Hit, updating cycle counter"<< std::endl;
			test_cache -> update_cycle_counter(instruction.get_address(), current_cycle);
			std::cout << *test_cache << std::endl;

		} else {
			std::cout << "Miss, chechking if we can allocate a block without evictions"<< std::endl;
			if(test_cache -> has_available_blocks(instruction.get_address())) {
				std::cout << "Some space available, allocating" << std::endl;
				test_cache -> allocate_block(instruction.get_address(), current_cycle);
				std::cout << *test_cache << std::endl;
			} else {
				std::cout << "No available blocks, we need to evict something" << std::endl;
				address_to_evict = test_cache -> find_lru_block(instruction.get_address());
				std::cout << "Will evict the block with index 0x" << std::hex <<  test_cache -> get_index(address_to_evict);
				std::cout << " and tag 0x" << std::hex << test_cache -> get_tag(address_to_evict) << std::endl;
			}
		}
		std::cout << std::endl << std::endl;

	}

	delete test_cache;


	return 0;
}
