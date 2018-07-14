/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include "DataAggregator.h"

int main( int argc, char * argv[]) 
{
	uint64_t block_size = 256;
	uint64_t blocks_num = 10;
	uint32_t generator_threads_num = 5;
	uint32_t crc32_threads_num = 5;
	
	DataAggregator aggregator;
	
	aggregator.start( block_size, blocks_num, generator_threads_num, crc32_threads_num );	
	aggregator.flush_n_stop();
	return 0; 
}