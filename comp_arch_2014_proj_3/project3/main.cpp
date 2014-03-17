#include "cache.hpp"
#include "cache_configuration.hpp"
#include "instruction.hpp"

#include <iostream>
#include <fstream>
#include <string>


class Cache_Access_Result {
	public:
		bool is_hit;
		bool allocated_without_eviction;
		uint32_t evicted_address;
};

Cache_Access_Result cache_read(Cache *l1_cache, Cache *l2_cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info = false);
Cache_Access_Result store_block_in_cache(Cache *cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info = false);
Cache_Access_Result cache_write(Cache *l1_cache, Cache *l2_cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info = false);
Cache_Access_Result store_block_in_cache_write(Cache *cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info = false);
void show_cache_tag_index_bo(Cache *cache, Instruction instruction);


int main() {
	uint64_t current_cycle;
	Cache_Configurations cache_configurations;
	Instruction instruction;
	uint64_t address_to_evict;
	Cache_Access_Result cache_access_result;
	Cache *i_cache, *d_cache, *l2_cache;


	std::ifstream config_file("config.default");

	if(config_file.is_open()) {
		config_file >> cache_configurations;
		config_file.close();
	} else {
		std::cout << "Something is wrong with confing file; using default parameters" << std::endl;
	}

	std::cout << cache_configurations << std::endl;


	i_cache = new Cache(cache_configurations.i_cache);
	d_cache = new Cache(cache_configurations.d_cache);
	l2_cache = new Cache(cache_configurations.l2_cache);

	std::string tmp("[L1-Inst]");
	i_cache -> stats.prefix = tmp;
	tmp = "[L1-Data]";
	d_cache -> stats.prefix = tmp;
	tmp = "[L2-Unif]";
	l2_cache -> stats.prefix = tmp;	


	for(current_cycle = 0; !std::cin.eof(); current_cycle++) {

		std::cin >> instruction;
		if (std::cin.eof()) break; //Fixing some wierd stuff with cin
		std::cout << "Cycle: " << std::dec << current_cycle << std::endl;
		std::cout << instruction << std::endl;

		//cach_access_result = cache_read(test_cache, instruction, current_cycle, true);

		switch(instruction.type) {
			case PC:
					std::cout << "I-cache" << std::endl;					
					cache_read(i_cache, l2_cache, instruction, current_cycle, true);
					i_cache->stats.accesses++;
					i_cache->stats.reads++;
					break;

			case LOAD:
					std::cout << "D-cache" << std::endl;
					cache_read(d_cache, l2_cache, instruction, current_cycle, true);
					d_cache->stats.accesses++;
					d_cache->stats.reads++;
					break;
			case STORE:
					std::cout << "D-cache" << std::endl;
					//show_cache_tag_index_bo(d_cache, instruction);
					cache_write(d_cache, l2_cache, instruction, current_cycle, true);
					d_cache->stats.accesses++;
					d_cache->stats.writes++;
					break;
		}
		std::cout << std::endl << std::endl << std::endl;	
	}

	return 0;
}


Cache_Access_Result cache_read(Cache *l1_cache, Cache *l2_cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info) {
	Cache_Access_Result cache_access_result;
	uint64_t address_to_evict;
	show_cache_tag_index_bo(l1_cache, instruction);
	if (l1_cache -> is_hit(instruction.get_address())) 
	{
		std::cout << "L1-cache hit, updating cycle counter" << std::endl;
		l1_cache -> update_cycle_counter(instruction.get_address(), current_cycle);
	} 
	else
	{	
		l1_cache -> stats.read_misses++;
		std::cout << "Miss, trying L2-cache" << std::endl;
		show_cache_tag_index_bo(l2_cache, instruction);
		if (l2_cache -> is_hit(instruction.get_address())) 
		{
			std::cout << "L2-cache hit, updating cycle counter" << std::endl;
			l2_cache -> update_cycle_counter(instruction.get_address(), current_cycle);
			std::cout << "Allocating a block in L1-cache" << std::endl;
			store_block_in_cache(l1_cache, instruction, current_cycle, true);
		} 
		else 
		{
			l2_cache -> stats.read_misses++;
			std::cout << "L2-cache miss, allocaking a block" << std::endl;
			cache_access_result = store_block_in_cache(l2_cache, instruction, current_cycle, true);
			if(cache_access_result.allocated_without_eviction) 
			{
				std::cout << "Block in L2 allocated without evictions, no force L1 evictions needed" << std::endl;
				std::cout << "Checking if we can allocate a block in L1" << std::endl;
				cache_access_result = store_block_in_cache(l1_cache, instruction, current_cycle, true);
			} 
			else 
			{
				std::cout << "Block 0x" << std::hex << cache_access_result.evicted_address << " evicted from L2" << std::endl;
				std::cout << "Checking if it's in L1" << std::endl;
				if (l1_cache -> is_hit(cache_access_result.evicted_address)) 
				{
					std::cout << "Evicted block found in L1, forcefully evicting" << std::endl;
					l1_cache -> stats.forced_clean_evictions++;
					l1_cache -> evict_block(cache_access_result.evicted_address);
					if((l1_cache -> get_dirty_bit(cache_access_result.evicted_address)) == true)
					{
						l1_cache ->stats.forced_dirty_writebacks++;
					}
				} 
				else
				{
					std::cout << "Evicted block not found in L1, no need for forced eviction" << std::endl;
				}
				std::cout << "Checking if we can allocate a block in L1" << std::endl;
				cache_access_result = store_block_in_cache(l1_cache, instruction, current_cycle, true);
			}
		}
	}
	return cache_access_result;
}

Cache_Access_Result cache_write(Cache *l1_cache, Cache *l2_cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info)
{
	Cache_Access_Result cache_access_result;
	uint64_t address_to_evict;
	show_cache_tag_index_bo(l1_cache, instruction);
	if(l1_cache->is_hit(instruction.get_address()))
	{
		std::cout << "Updating cycle counter and dirty bit of the particular block" << std::endl;
		l1_cache -> update_cycle_counter(instruction.get_address(), current_cycle);
		l1_cache -> set_dirty_bit(instruction.get_address());
		l2_cache -> set_dirty_bit(instruction.get_address());
		l2_cache -> update_cycle_counter(instruction.get_address(), current_cycle);
	}
	else
	{
		l1_cache->stats.write_misses++;
		std::cout << "L1 cache miss - trying L2 cache" << std::endl;
		show_cache_tag_index_bo(l2_cache, instruction);
		if(l2_cache->is_hit(instruction.get_address()))
		{
			std::cout << "L2-cache hit, updating cycle counter and dirty bit" << std::endl;
			l2_cache -> update_cycle_counter(instruction.get_address(), current_cycle);
			l2_cache -> set_dirty_bit(instruction.get_address());
			std::cout << "Allocating a block in L1-cache" << std::endl;
			store_block_in_cache_write(l1_cache, instruction, current_cycle, true);
		}
	
		else 
		{
			l2_cache->stats.write_misses++;
			std::cout << "L2-cache miss, allocating a block" << std::endl;
			cache_access_result = store_block_in_cache_write(l2_cache, instruction, current_cycle, true);
			if(cache_access_result.allocated_without_eviction) 
			{
				std::cout << "Block in L2 allocated without evictions, no force L1 evictions needed" << std::endl;
				std::cout << "Checking if we can allocate a block in L1" << std::endl;
				cache_access_result = store_block_in_cache_write(l1_cache, instruction, current_cycle, true);
	    	}
			else 
			{
				std::cout << "Block 0x" << std::hex << cache_access_result.evicted_address << " evicted from L2" << std::endl;
				std::cout << "Checking if it's in L1" << std::endl;
				if (l1_cache -> is_hit(cache_access_result.evicted_address)) 
				{
					std::cout << "Evicted block found in L1, forcefully evicting" << std::endl;
					l1_cache -> evict_block(cache_access_result.evicted_address);
				} 
				else 
				{
					std::cout << "Evicted block not found in L1, no need for forced eviction" << std::endl;
				}
				std::cout << "Checking if we can allocate a block in L1" << std::endl;
				cache_access_result = store_block_in_cache_write(l1_cache, instruction, current_cycle, true);
			}
		}
	}
	return cache_access_result;
}



Cache_Access_Result store_block_in_cache(Cache *cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info){
	Cache_Access_Result res;
	uint64_t address_to_evict;
	if(cache -> has_available_blocks(instruction.get_address())) {
		cache -> allocate_block(instruction.get_address(), current_cycle);
		if(show_debug_info) {
			std::cout << "Some space available, allocating" << std::endl;
			//std::cout << *cache << std::endl;
		}
		res.allocated_without_eviction = true;
	} else {
		cache -> stats.clean_eviction++;
		address_to_evict = cache -> find_lru_block(instruction.get_address());
		cache -> evict_block(address_to_evict);
		cache -> allocate_block(instruction.get_address(), current_cycle);
		if((cache -> get_dirty_bit(address_to_evict)) == true)
		{
			cache -> stats.dirty_write_backs++;
		}
		res.evicted_address=address_to_evict;
		if(show_debug_info) {
			std::cout << "No available blocks, we need to evict something" << std::endl;
			std::cout << "Will evict the block with index 0x" << std::hex <<  cache -> get_index(address_to_evict);
			std::cout << " and tag 0x" << std::hex << cache -> get_tag(address_to_evict) << std::endl;
			//std::cout << *cache << std::endl;
			}
		res.allocated_without_eviction = false;
	}
	res.is_hit = false;
	return res;
}

Cache_Access_Result store_block_in_cache_write(Cache *cache, Instruction instruction, uint64_t current_cycle, bool show_debug_info){
	Cache_Access_Result res;
	uint64_t address_to_evict;
	if(cache -> has_available_blocks(instruction.get_address())) {
		cache -> allocate_block_for_write(instruction.get_address(), current_cycle);
		if(show_debug_info) {
			std::cout << "Some space available, allocating" << std::endl;
			//std::cout << *cache << std::endl;
		}
		res.allocated_without_eviction = true;
	} else {
		address_to_evict = cache -> find_lru_block(instruction.get_address());
		cache -> evict_block(address_to_evict);
		cache -> allocate_block_for_write(instruction.get_address(), current_cycle);
		res.evicted_address=address_to_evict;
		if(show_debug_info) {
			std::cout << "No available blocks, we need to evict something" << std::endl;
			std::cout << "Will evict the block with index 0x" << std::hex <<  cache -> get_index(address_to_evict);
			std::cout << " and tag 0x" << std::hex << cache -> get_tag(address_to_evict) << std::endl;
			//std::cout << *cache << std::endl;
			}
		res.allocated_without_eviction = false;
	}
	res.is_hit = false;
	return res;
}


void show_cache_tag_index_bo(Cache *cache, Instruction instruction) {
	std::cout << "Block offset: " << std::hex << "0x" << cache -> get_block_offset(instruction.get_address());
	std::cout << "; Index: " << std::hex << "0x" << cache -> get_index(instruction.get_address());
	std::cout << "; Tag: " << std::hex << "0x" << cache -> get_tag(instruction.get_address()) << std::endl;
}
