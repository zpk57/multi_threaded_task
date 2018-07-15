/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include "DataAggregator.h"
#include <string>
#include <iostream>
#include "string.h"

int print_usage()
{
	std::cerr << "Usage: \"program <block_size> <blocks_num> <generator_threads_num> <crc32_threads_num> [debug]\"" << std::endl;
	std::cerr << "block_size, blocks_num, generator_threads_num and crc32_threads_num should be a decimal natural numbers." << std::endl;
	return -1;
}

int check_digits( char * str ) 
{
	while ( *str != '\0' ) {
		if ( *str > '9' || *str < '0' ) {
			return -1;
		}
		++str;
	}
	return 0;
}

int main( int argc, char * argv[]) 
{
	if ( argc < 5 || argc > 6 ) {
		return print_usage();
	}
	
	if ( check_digits( argv[1] ) || check_digits( argv[2] ) 
		|| check_digits( argv[3] ) || check_digits( argv[4] ) ) {
		return print_usage();
	}
	
	uint64_t block_size, blocks_num;
	uint32_t generator_threads_num, crc32_threads_num;
	bool debug = false;
	
	try {
		block_size = std::stoull( argv[1] );
		blocks_num = std::stoull( argv[2] );
		generator_threads_num = std::stoul( argv[3] );
		crc32_threads_num = std::stoul( argv[4] );		
	}
	catch (...) {
		return print_usage();
	}
	
	if ( !(block_size && blocks_num 
		   && generator_threads_num && crc32_threads_num) ) {
		return print_usage();
	}
	
	if ( argc == 6 ) {
		if ( 0 == strcmp( argv[5], "debug" ) ) {
			debug = true;
		}
		else {
			return print_usage();
		}
	}
	
	if ( debug ) {
		std::cout << block_size << " " << blocks_num << " " 
		<< generator_threads_num << " " << crc32_threads_num << std::endl;
	}
	
	DataAggregator aggregator;	
	aggregator.start( block_size, 
					  blocks_num, 
					  generator_threads_num, 
					  crc32_threads_num, 
					  debug );	
	aggregator.flush_n_stop();
	
	return 0; 
}