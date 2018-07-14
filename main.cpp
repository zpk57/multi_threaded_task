#include <iostream>
#include "ThreadPool.h"

std::mutex g_print_mutex;

class PrintEvent : public ThreadPool::IThreadEvent
{
public:
	PrintEvent( unsigned int num ) : 
		m_num( num ) 
	{}
	
	void execute() override
	{
		std::lock_guard<std::mutex> lg( g_print_mutex );
		std::cout << "Printing... " << m_num << std::endl;
	}
	unsigned int m_num;
};

int main( int argc, char * argv[]) 
{
	ThreadPool tp;
	tp.start( 2 );
	unsigned int count = 0;
	for ( int i = 0; i < 10; ++i ) {
		tp.add_event( new PrintEvent( ++count ) );
	}
	tp.flush();
	tp.stop();
	return 0; 
}