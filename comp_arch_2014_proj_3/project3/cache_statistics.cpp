#include "cache_statistics.hpp"

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
}
