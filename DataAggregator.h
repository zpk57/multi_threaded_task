#pragma once

/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include <memory>
#include <mutex>
#include <vector>
#include <ostream>
#include "ThreadPool.h"
#include "DataWrapper.h"
#include "IDataHandler.h"

// class for global threads control,
// data routing and output
class DataAggregator : public IDataHandler
{
public:
	DataAggregator() = default;
	
	int start( uint64_t block_size,
			   uint64_t blocks_num,
			   uint32_t generator_threads_num,
			   uint32_t crc32_threads_num,
			   bool debug = false ) ;
	
	int flush_n_stop();
	
private:	
	// IDataHandler	
	int add_data_wrapper( std::shared_ptr<DataWrapper> &data_wrapper ) override;	
	int notify_process_finish( std::shared_ptr<DataWrapper> &data_ptr, uint32_t finished_threads_num ) override;
	
	// internal
	int save_block( std::shared_ptr<DataWrapper> &data_ptr);	
	int print_cout( std::ostream &stream );
	
private:
	std::vector<std::shared_ptr<DataWrapper>> m_wrappers;
	std::vector<std::unique_ptr<ThreadPool>> m_pools;
	ThreadPool generators_pool;
	std::mutex m_wrappers_mutex;
	std::mutex m_output_mutex;
	uint32_t m_crc32_threads_num;
	bool m_debug;
};