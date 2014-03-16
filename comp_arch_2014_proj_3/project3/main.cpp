#include "cache.hpp"
#include "cache_configuration.hpp"
#include "instruction.hpp"

#include <iostream>
#include <fstream>


const unsigned int BLOCK_BITS = 5, INDEX_BITS = 8;
int main() {
	uint64_t current_cycle;
	Cache_Configurations cache_configurations;
	Instruction instruction;
	std::ifstream config_file("config.default");

	if(config_file.is_open()) {
		config_file >> cache_configurations;
		config_file.close();
	} else {
		std::cout << "Something is wrong with confing file; using default parameters" << std::endl;
		cache_configurations.i_cache.cache_size = 8192;
		cache_configurations.i_cache.block_size = 32;
		cache_configurations.i_cache.associativity = 1;
		cache_configurations.d_cache.cache_size = 8192;
		cache_configurations.d_cache.block_size = 32;
		cache_configurations.d_cache.associativity = 4;
		cache_configurations.l2_cache.cache_size = 65536;
		cache_configurations.l2_cache.block_size = 32;
		cache_configurations.l2_cache.associativity = 2;
		cache_configurations.start_counting_after = 1000000;
	}

	int a;
	std::string str;
	unsigned int command_type, address, block_offset, index, tag;
	Cache *test_cache;


	std::cout << cache_configurations << std::endl;
	test_cache = new Cache(cache_configurations.i_cache);
	std::cout << *test_cache << std::endl;
	delete test_cache;

	for(current_cycle = 0; !std::cin.eof(); current_cycle++) {
/*		std::cin >> command_type;
		std::cin >> std::hex >> address;
		block_offset = address % (1<<BLOCK_BITS);
		index = (address>>BLOCK_BITS) % (1<<INDEX_BITS);
		tag = address>>(BLOCK_BITS + INDEX_BITS);
		std::cout << "Current cycle: " << current_cycle << std::endl;
		std::cout << "Command type: " << command_type << std::endl;
		std::cout << "Address: " << std::hex << address << ", block offset: " << std::hex << block_offset;
		std::cout << ", index: " << std::hex << index << ", tag: " << std::hex << tag << std::endl;
*/
		std::cin >> instruction;
		std::cout << instruction << std::endl;

	}

	/* Check if the instruction/data is present in L1, if not check in L2 */
	Cache *L1_I, *L2_U, *L1_D;
	Instruction_Type_t type = instruction.get_instruction_type();
	uint32_t instruction_address = instruction.get_instruction_address();
	if(type == PC)
	{
		if((L1_I.Is_hit(instruction)) == true)
			{
				/* Update L1 cache statistics */
			}
		else
		{
			if(L2_U.Is_hit(instruction) == true)
			{
				/* Update L2 Cache statistics */
			}
		}
	}

	return 0;
}
