/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include "DataWrapper.h"
#include "stdlib.h"
#include "crc32/crc.h"

DataWrapper::DataWrapper( uint64_t size )
: m_size( size ),
  m_data_ptr( new uint8_t[size] )
{}

int DataWrapper::generate_data( unsigned int rand_pos )
{
	srand( rand_pos );
	for ( uint64_t i = 0; i < m_size; ++i ) {
		uint16_t value = rand();
		m_data_ptr.get()[i] = (uint8_t)((value^(value>>8)) & 0xff);
	}
	return 0;
}

const uint8_t * DataWrapper::get_rd() const
{
	return m_data_ptr.get();
}

uint64_t DataWrapper::get_size() const
{
	return m_size;
}

uint32_t DataWrapper::calculate_crc32() const
{
	return crcSlow( (const unsigned char*)m_data_ptr.get(), m_size );
}

uint32_t DataWrapper::register_thread_crc32( uint32_t crc_value )
{
	std::lock_guard<std::mutex> lg( m_mutex );
	m_crc32s.push_back( crc_value );
	
	return m_crc32s.size();
}

int DataWrapper::check_crc32() 
{
	std::lock_guard<std::mutex> lg( m_mutex );
	for ( uint32_t i = 1; i < m_crc32s.size(); ++i ) {
		if ( m_crc32s[i] != m_crc32s[i-1] ) {
			return -1;
		}
	}
	// if each crc32 pair is match, then all crc32 is match
	return 0;
}

uint32_t DataWrapper::get_crc32()
{
	std::lock_guard<std::mutex> lg( m_mutex );
	if ( m_crc32s.empty() ) {
		return 0;
	}
	//return a first calculated crc32
	return m_crc32s.front();
}