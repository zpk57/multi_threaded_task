#pragma once

/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include <cstdlib>
#include <memory>
#include <mutex>
#include <vector>

// wrapper class for data entry
// provides generation, calculation and check functionality
class DataWrapper
{
public:
	DataWrapper( uint64_t size );
	
	// not a thread-safely
	int generate_data( unsigned int rand_pos );	
	
	// calculate crc32
	uint32_t calculate_crc32() const;
	
	// register calculated crc32 from some thread
	uint32_t register_thread_crc32( uint32_t crc_value );
	
	// return 0 if all crc32 is match
	int check_crc32();
	
	// data accessors
	uint32_t get_crc32();
	const uint8_t * get_rd() const;	
	uint64_t get_size() const;
	
private:
	uint64_t m_size;
	std::unique_ptr<uint8_t[]> m_data_ptr;
	std::vector<uint32_t> m_crc32s;
	std::mutex m_mutex;
};