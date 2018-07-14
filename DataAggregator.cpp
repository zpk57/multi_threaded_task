/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/
   
#include "DataAggregator.h"
#include <iostream>
#include "Events.h"

int DataAggregator::start( uint64_t block_size,
						   uint64_t blocks_num,
						   uint32_t generator_threads_num,
						   uint32_t crc32_threads_num ) 
{	
	m_crc32_threads_num = crc32_threads_num;
	generators_pool.start( generator_threads_num );
	
	m_pools.reserve( m_crc32_threads_num );		
	for ( uint32_t i = 0; i < m_crc32_threads_num; ++i ) {
		ThreadPool *pool = new ThreadPool();
		m_pools.emplace_back( pool );
		pool->start( 1 );
	}
	
	for ( uint64_t i = 0; i < blocks_num; ++i ) {
		generators_pool.add_event( new DataGeneratorEvent( *this, block_size ) );
	}
	
	return 0;
}
	
int DataAggregator::add_data_wrapper( std::shared_ptr<DataWrapper> &data_wrapper )
{
	{
		std::lock_guard<std::mutex> lg( m_wrappers_mutex );
		m_wrappers.push_back( data_wrapper );
	}
	for ( auto &pool: m_pools ) {
		auto event = new DataProcessorEvent( *this, data_wrapper );
		pool->add_event( event );
	}
	return 0;
}

int DataAggregator::notify_process_finish( std::shared_ptr<DataWrapper> &data_ptr, uint32_t finished_threads_num )
{
	if ( finished_threads_num != m_crc32_threads_num ) {
		return 0;
	}
	if ( 0 != data_ptr->check_crc32() ) 
	{
		save_block( data_ptr );
	}
	else {
		std::lock_guard<std::mutex> lg( m_output_mutex );
		std::cout << "Block " << std::hex << data_ptr->get_crc32() << " has been finished." << std::endl;			
	}
	return 0;
}

int DataAggregator::save_block( std::shared_ptr<DataWrapper> &data_ptr) 
{
	std::lock_guard<std::mutex> lg( m_output_mutex );
	std::cout << "Block " << std::hex << data_ptr->get_crc32() << " has various crc32" << std::endl;
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
	