#include "cache.hpp"
#include "cache_configuration.hpp"
#include "instruction.hpp"

#include <iostream>
#include <fstream>




int main() {
	uint64_t current_cycle;
	Cache_Configurations cache_configurations;
	Instruction instruction;
	Cache *test_cache;
	std::ifstream config_file("config.default");

	if(config_file.is_open()) {
		config_file >> cache_configurations;
		config_file.close();
	} else {
		std::cout << "Something is wrong with confing file; using default parameters" << std::endl;
	}


	std::cout << cache_configurations << std::endl;
	test_cache = new Cache(cache_configurations.i_cache);


	test_cache -> sets[0x6].cache_blocks[0].tag = 0x200264;
	test_cache -> sets[0x6].cache_blocks[0].is_available = false;
	test_cache -> sets[0x6].available_blocks = 3;

	//test_cache -> sets->clear();
	std::cout << *test_cache << std::endl;


	for(current_cycle = 0; !std::cin.eof(); current_cycle++) {

		std::cin >> instruction;
		std::cout << "Cycle: " << std::dec << current_cycle << std::endl;
		std::cout << instruction << std::endl;
		std::cout << "Block offset: " << std::hex << "0x" << test_cache -> get_block_offset(instruction.get_address());
		std::cout << "; Index: " << std::hex << "0x" << test_cache -> get_index(instruction.get_address());
		std::cout << "; Tag: " << std::hex << "0x" << test_cache -> get_tag(instruction.get_address()) << std::endl;
		if (test_cache -> is_hit(instruction.get_address())) {
			std::cout << "Hit";
		} else {
			std::cout << "Miss";
		}
		std::cout << std::endl << std::endl;

	}

	delete test_cache;


	return 0;
}
