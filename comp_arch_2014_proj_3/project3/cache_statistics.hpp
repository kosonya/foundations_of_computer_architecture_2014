#ifndef _CACHE_STATISTICS_HPP_
#define _CACHE_STATISTICS_HPP_ 0

#include <string>

class Cache_Statistics
{
	public:
		Cache_Statistics();
		friend std::ostream& operator<<(std::ostream& os, const Cache_Statistics& stats);
		std::string prefix;
		unsigned int accesses;
		unsigned int misses;
		unsigned int reads;
		unsigned int read_misses;
		unsigned int writes;
		unsigned int write_misses;
		unsigned int clean_evictions;
		unsigned int dirty_writebacks;
		unsigned int forced_clean_evictions;
		unsigned int forced_dirty_evictions;
		unsigned int checksum;
};

#endif
