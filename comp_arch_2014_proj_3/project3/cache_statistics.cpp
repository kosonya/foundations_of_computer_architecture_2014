#include "cache_statistics.hpp"
#include <string>
#include <iostream>

Cache_Statistics::Cache_Statistics() {
	std::string tmp("");
	prefix = tmp;
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
}

std::ostream& operator<<(std::ostream& os, const Cache_Statistics& stats) {
	os << stats.prefix << " Accesses: " << std::dec << stats.acceses << std::endl;
	os << stats.prefix << " Misses: " << std::dec << stats.misses << std::endl;
	os << std::endl;
	os << stats.prefix << " Reads: " << std::dec << stats.reads << std::endl;
	os << stats.prefix << " Read misses: " << std::dec << stats.read_misses << std::endl;
	os << stats.prefix << " Read miss rate: ";
	if (stats.reads == 0) {
		os << "N/A" << std::endl;
	} else {
		os << std::dec << ( (double)stats.read_misses) / ( (double)stats.reads ) << std:endl;
	}
	os << std::endl;
	os << stats.prefix << " Miss rate: ";
	if (stats.accesses == 0) {
		os << "N/A" << std::endl;
	} else {
		os << std::dec << ( (double)stats.misses) / ( (double)stats.accesses ) << std::endl;
	}
	os << stats.prefix << " Clean evictions: " << std::dec << stats.clean_evictions << std::endl;
	os << std::end;
	os << stats.prefix << " Forced clean evictions: " << std::dec << stats.forced_clean_evictions;
	return os;
}
