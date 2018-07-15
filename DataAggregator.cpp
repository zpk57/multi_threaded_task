/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/
   
#include "DataAggregator.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "Events.h"

int DataAggregator::start( uint64_t block_size,
						   uint64_t blocks_num,
						   uint32_t generator_threads_num,
						   uint32_t crc32_threads_num,
						   bool debug ) 
{	
	m_crc32_threads_num = crc32_threads_num;
	m_debug = debug;
	generators_pool.start( generator_threads_num );
	
	//create pool instance for each processing thread (queue scalable)
	m_pools.reserve( m_crc32_threads_num );		
	for ( uint32_t i = 0; i < m_crc32_threads_num; ++i ) {
		ThreadPool *pool = new ThreadPool();
		m_pools.emplace_back( pool );
		pool->start( 1 );
	}
	
	for ( uint64_t i = 0; i < blocks_num; ++i ) {
		generators_pool.add_event( new DataGeneratorEvent( *this, block_size, i ) );
	}
	
	return 0;
}
	
int DataAggregator::add_data_wrapper( std::shared_ptr<DataWrapper> &data_wrapper )
{
	{
		//save block
		std::lock_guard<std::mutex> lg( m_wrappers_mutex );
		m_wrappers.push_back( data_wrapper );
	}
	//start processing for each thread
	for ( auto &pool: m_pools ) {
		auto event = new DataProcessorEvent( *this, data_wrapper );
		pool->add_event( event );
	}
	return 0;
}

int DataAggregator::notify_process_finish( std::shared_ptr<DataWrapper> &data_ptr, uint32_t finished_threads_num )
{
	if ( finished_threads_num != m_crc32_threads_num ) {
		//notify should be received from all process threads
		return 0;
	}
	if ( 0 != data_ptr->check_crc32() ) 
	{
		//save block if crc32 don't match
		save_block( data_ptr );
	}
	else {
		if ( m_debug ) {
			std::stringstream ss;
			ss << "Block " << std::hex << data_ptr->get_crc32() << " has been finished." << std::endl;
			ss << std::hex;
			for ( uint64_t i = 0; i < data_ptr->get_size(); ++i ) {
				//hack: uint8_t prints as char
				uint16_t value = data_ptr->get_rd()[i];
				ss << value;
			}
			ss << std::endl;	
			print_cout( ss );		
		}
	}
	return 0;
}

int DataAggregator::save_block( std::shared_ptr<DataWrapper> &data_ptr) 
{
	uint32_t crc32 = data_ptr->get_crc32();
	std::stringstream ss;
	ss << "Block " << std::hex << crc32 << " has various crc32" << std::endl;
	print_cout( ss );
	
	ss.str(std::string());
	ss << std::hex << crc32;
	
    std::ofstream ostrm( ss.str(), std::ios::binary | std::ios::out );
	if ( !ostrm.is_open() ) {
		ss.str(std::string());
		ss << "Error: can't open file for " << std::hex << crc32 << " data block." << std::endl;
		print_cout( ss );
		return -1;
	}
	
	for ( uint64_t i = 0; i < data_ptr->get_size(); ++i ) {
		ostrm << data_ptr->get_rd()[i];;
	}
	
	return 0;
}

int DataAggregator::print_cout( std::ostream &stream )
{
	std::lock_guard<std::mutex> lg( m_output_mutex );
	std::cout << stream.rdbuf();
	return 0;
}

int DataAggregator::flush_n_stop()
{
	generators_pool.flush();
	for ( auto &pool: m_pools ) {
		pool->flush();
	}
	
	generators_pool.stop();
	for ( auto &pool: m_pools ) {
		pool->stop();
	}
	return 0;
}
	