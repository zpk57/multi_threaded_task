/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/
   
#include "ThreadPool.h"
#include <iostream>

//Flush event for queue flush
class FlushEvent : public ThreadPool::IThreadEvent
{
public: 
	FlushEvent( std::mutex * mutex_ptr ) :
		m_mutex_ptr(mutex_ptr)
	{
		m_mutex_ptr->lock();
	}
	
	void execute() override
	{
		m_mutex_ptr->unlock();
	}
	
	std::mutex *m_mutex_ptr;
};

ThreadPool::ThreadPool() : 
	m_stopped( false )
{}

ThreadPool::~ThreadPool()
{
	stop();
}

int ThreadPool::start( unsigned int threads_num )
{
	m_threads.reserve( threads_num );
	for ( unsigned int i = 0; i < threads_num; ++i ) {
		m_threads.emplace_back( &ThreadPool::thread_function, this );
	}
	return 0;
}

int ThreadPool::flush()
{
	std::mutex flush_mutex;
	add_event( new FlushEvent( &flush_mutex ) );
	//mutex can be aquired only after flush event is done
	std::lock_guard<std::mutex> lg( flush_mutex );
	return 0;
}

int ThreadPool::stop()
{
	{
		std::unique_lock<std::mutex> ul( m_mutex );
		if ( m_stopped ) {
			return 0;
		}
		m_stopped = true;
	}
	m_cv.notify_all();
	
	for ( auto &thread : m_threads ) {
		thread.join();
	}
	return 0;
}

int ThreadPool::add_event( ThreadPool::IThreadEvent* event )
{
	{
		std::unique_lock<std::mutex> ul( m_mutex );
		m_queue.push( event );
	}
	m_cv.notify_one();
	return 0;
}

ThreadPool::IThreadEvent* ThreadPool::get_event()
{
	if ( 0 == m_queue.size() ) {
		return nullptr;
	}
	IThreadEvent* retval = m_queue.front();
	m_queue.pop();
	return retval;
}

bool ThreadPool::can_sleep()
{
	return m_queue.empty() && !m_stopped;
}

void ThreadPool::thread_function()
{
	while ( true ) {
		IThreadEvent* event;
		{
			std::unique_lock<std::mutex> ul( m_mutex );
			m_cv.wait(ul, [&](){ return !can_sleep(); });
			
			if ( m_stopped ) {
				return;
			}
			
			event = get_event();
		}
		
		if ( nullptr == event ) {
			continue;
		}
		
		event->execute();
		delete event;
	}
}