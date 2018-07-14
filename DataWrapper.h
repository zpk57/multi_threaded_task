#pragma once

/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include <cstdlib>
#include <memory>
#include <mutex>
#include <vector>

class DataWrapper
{
public:
	DataWrapper( uint64_t size );
	
	int generate_data();
	
	const uint8_t * get_rd() const;
	
	uint32_t calculate_crc32() const;
	
	uint32_t register_thread_crc32( uint32_t crc_value );
	
	int check_crc32();
	
	uint32_t get_crc32();
	
private:
	uint64_t m_size;
	std::unique_ptr<uint8_t[]> m_data_ptr;
	std::vector<uint32_t> m_crc32s;
	std::mutex m_mutex;
};