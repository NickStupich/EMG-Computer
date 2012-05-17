#ifndef NMUTEX
#define NMUTEX

#ifdef WIN32
#include <Windows.h>
#endif

class NMutex{
public:
	NMutex();
	bool lock();
	bool unlock();

private:

#ifdef WIN32
	CRITICAL_SECTION _mutex;
#endif


};


#endif