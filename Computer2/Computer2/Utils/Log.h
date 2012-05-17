#ifndef _LOGGING
#define _LOGGING

#include "NMutex.h"
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <sys/timeb.h>

#define LOG_DEBUG(msg, ...) log(Debug, __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) log(Error, __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__)

enum Priority{
	Debug = 0,
	Info = 1,
	Error = 2,
	None = 3
};

void log(Priority priority, char* file, char* function, int line, char* msg, ...);


#endif