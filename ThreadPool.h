#pragma once

/* Copyright (C) 2018 Petr Zotov.  You may use this program, or *
 * code, as desired without restriction.						*/

#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>

// Class provides thread pool with one FIFO queue for threads set
class ThreadPool
{
public:	
	class IThreadEvent
	{
	public:
		virtual ~IThreadEvent() = default;
		virtual void execute() = 0;
	};
	
public:	
	ThreadPool();
	~ThreadPool();
	
	int start( unsigned int threads_num );
	
	int flush();
	
	int stop();
	
	int add_event( IThreadEvent* event );

private:	
	IThreadEvent* get_event();
	
	bool can_sleep();
	
	void thread_function();
	
private:
	std::queue<IThreadEvent*> m_queue;
	std::mutex m_mutex;
	
	std::condition_variable	 m_cv;
	bool m_stopped;
	
	std::vector<std::thread> m_threads;
};