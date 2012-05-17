#ifdef WIN32

#include "NMutex.h"


NMutex::NMutex()
{
	InitializeCriticalSection(&this->_mutex);
}

bool NMutex::lock()
{
	EnterCriticalSection(&this->_mutex);
	return true;
}

bool NMutex::unlock()
{
	LeaveCriticalSection(&this->_mutex);
	return true;
}

#endif