#pragma once

/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include <memory>
#include <thread>
#include <functional>
#include "ThreadPool.h"
#include "DataWrapper.h"
#include "IDataHandler.h"

// event class for a data processing
// calculated crc32 and notify IDataHandler
class DataProcessorEvent : public ThreadPool::IThreadEvent
{
public:
	DataProcessorEvent( IDataHandler &aggregator, std::shared_ptr<DataWrapper> &data_ptr ) :
		m_aggregator( aggregator ),
		m_data_ptr( data_ptr )
	{}
	
	void execute() override
	{
		uint32_t crc32 = m_data_ptr->calculate_crc32();
		uint32_t threads_num = m_data_ptr->register_thread_crc32( crc32 );
		m_aggregator.notify_process_finish( m_data_ptr, threads_num );
	}

private:	
	IDataHandler &m_aggregator;
	std::shared_ptr<DataWrapper> m_data_ptr;
};

// event class for a data generation
// generates data and notify IDataHandler
class DataGeneratorEvent : public ThreadPool::IThreadEvent
{
public:
	DataGeneratorEvent( IDataHandler &aggregator, uint64_t block_size, unsigned int rand_pos ) :
		m_aggregator( aggregator ),
		m_block_size( block_size ),
		m_rand_pos( rand_pos )
	{}
	
	void execute() override
	{
		std::shared_ptr<DataWrapper> data_wrapper 
			= std::make_shared<DataWrapper>( m_block_size );
			
		data_wrapper->generate_data( m_rand_pos ^ time(NULL) ^ std::hash<std::thread::id>{}( std::this_thread::get_id() ));
		m_aggregator.add_data_wrapper( data_wrapper );
	}

private:	
	IDataHandler &m_aggregator;
	uint64_t m_block_size;
	unsigned int m_rand_pos;
};