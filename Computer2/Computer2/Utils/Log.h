#ifndef _LOGGING
#define _LOGGING

#include "Mutex.h"
#include <stdio.h>
#include <time.h>
#include <direct.h>

#define LOG_DEBUG(msg) log(Debug, __FILE__, __FUNCTION__, __LINE__, msg)
#define LOG_ERROR(msg) log(Error, __FILE__, __FUNCTION__, __LINE__, msg)

#define LOG_LAST_WIN_ERROR(errorNumber, msg)		WCHAR errorMsg[1024]; \
													FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
													NULL,\
													errorNumber,\
													MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),\
													errorMsg,\
													1024, NULL);\
													LOG_ERROR(msg);\
													LOG_ERROR((char*)errorMsg);\


enum Priority{
	Debug = 1,
	Info = 2,
	Error = 3,
	None = 4
};

void log(Priority priority, char* file, char* function, int line, char* msg);


#endif