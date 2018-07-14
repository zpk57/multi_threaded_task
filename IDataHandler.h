#pragma once

/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include <memory>
#include "DataWrapper.h"

class IDataHandler
{
public:
	virtual int add_data_wrapper( std::shared_ptr<DataWrapper> &data_wrapper ) = 0;
	
	virtual int notify_process_finish( std::shared_ptr<DataWrapper> &data_ptr, uint32_t finished_threads_num ) = 0;
};
