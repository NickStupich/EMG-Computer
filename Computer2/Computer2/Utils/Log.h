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

FILE* fp;
BMutex* _mutex = new BMutex();
Priority _priority = Debug;

void log(Priority priority, char* file, char* function, int line, char* msg)
{
	if(priority >= _priority)
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		file += strlen(file)-2;
		while(*(file-1) != '\\'){file--;};

		_mutex->lock();
		if(fp == NULL)
		{
			//get filename
			char name[50];
			int response;
			strftime(name, strlen(name), "%Y-%m-%d.log", timeinfo);

			char* base = getenv("APPDATA");
			char* folder = "\\Musclemate";
			char* musclemateFolder = (char*) malloc((strlen(base) + strlen(folder)) * sizeof(char));
			sprintf_s(musclemateFolder, strlen(musclemateFolder), "%s%s", base, folder);

			int resp = _mkdir(musclemateFolder);
			if(resp == 0	//success
				|| resp == 17	//folder already exists
				|| 1)
			{
				char* filename = (char*) malloc((strlen(musclemateFolder) + strlen(name) + 1) * sizeof(char));
				sprintf_s(filename, strlen(filename), "%s\\%s", musclemateFolder, name);
				
				//free(musclemateFolder);
				
				response = fopen_s(&fp, filename, "a");
				//free(filename);

				if (response != 0)
				{
					_priority = None;	//disable logging
					goto end;
				}
			}
		}

		char timebuf[20];
		strftime(timebuf, strlen(timebuf), "%c", timeinfo);

		fprintf(fp, "%s\t%s: %s()\tline %d:\t%s\n", timebuf, file, function, line, msg);

		end:
		_mutex->unlock();
	}
}

#endif