#include "cache_statistics.hpp"
#include <string>
#include <iostream>

Cache_Statistics::Cache_Statistics() {
	accesses = 0;
	misses = 0;
	reads = 0;
	read_misses = 0;
	writes = 0;
	write_misses = 0;
	clean_evictions = 0;
	dirty_writebacks = 0;
	forced_clean_evictions = 0;
	forced_dirty_evictions = 0;
	checksum = 0;
	forced_dirty_writebacks = 0;
}

void Cache_Statistics::reset() {
	accesses = 0;
	misses = 0;
	reads = 0;
	read_misses = 0;
	writes = 0;
	write_misses = 0;
	clean_evictions = 0;
	dirty_writebacks = 0;
	forced_clean_evictions = 0;
	forced_dirty_evictions = 0;
	checksum = 0;
	forced_dirty_writebacks = 0;
}

std::ostream& operator<<(std::ostream& os, const Cache_Statistics& stats) {
	std::string prefix;
	switch(stats.type) {
		case I:
			prefix = "[L1-Inst]";
			break;
		case D:
			prefix = "[L1-Data]";
			break;
		case L2:
			prefix = "[L2-Unif]";
			break;
	}
	os << prefix << " Accesses: " << std::dec << stats.accesses << std::endl;
	os << prefix << " Misses: " << std::dec << stats.misses << std::endl;
	os << std::endl;
	os << prefix << " Reads: " << std::dec << stats.reads << std::endl;
	os << prefix << " Read misses: " << std::dec << stats.read_misses << std::endl;
	os << prefix << " Read miss rate: ";
	if(stats.reads == 0) {
		os << "N/A (division by zero)" << std::endl;
	} else {
		os << std::dec << ( (double)stats.read_misses) / ( (double)stats.reads ) << std::endl;
	}
	if(stats.type != I) {
		os << prefix << " Writes: " << std::dec << stats.writes << std::endl;
		os << prefix << " Write misses: " << std::dec << stats.write_misses << std::endl;
		os << prefix << " Write miss rate: ";
		if(stats.writes == 0) {
			os << "N/A (division by zero)" << std::endl;
		} else {
			os << std::dec << ( (double)stats.write_misses) / ( (double)stats.writes ) << std::endl;
		}
	}
	os << std::endl;
	os << prefix << " Miss rate: ";
	if(stats.accesses == 0) {
		os << "N/A (division by zero)" << std::endl;
	} else {
		os << std::dec << ( (double)stats.misses) / ( (double)stats.accesses ) << std::endl;
	}
	os << prefix << " Clean evictions: " << std::dec << stats.clean_evictions << std::endl;

	switch(stats.type) {
		case I:
			os << std::endl;
			os << prefix << " Forced clean evictions: " << std::dec << stats.forced_clean_evictions << std::endl;
			break;
		case D:
			os << prefix << " Dirty writebacks: " << std::dec << stats.dirty_writebacks << std::endl;
			os << std::endl;
			os << prefix << " Forced clean evictions: " << std::dec << stats.forced_clean_evictions << std::endl;
			os << prefix << " Forced dirty evictions: " << std::dec << stats.forced_dirty_evictions << std::endl;
			break;
		case L2:
			os << prefix << " Dirty writebacks: " << std::dec << stats.dirty_writebacks << std::endl;
			break;
		default:
			;
	}

	os << prefix << " Checksum: " << std::hex << stats.checksum;

	return os;
}
