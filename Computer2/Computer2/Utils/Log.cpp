#include "Log.h"

FILE* fp;
NMutex* _mutex = new NMutex();
Priority _priority = Debug;

char* levels[] = {"DEBUG", "INFO", "ERROR"};

void log(Priority priority, char* file, char* function, int line, char* msg, ...)
{
	if(priority >= _priority)
	{
		time_t rawtime;
		struct tm* timeinfo;
		struct _timeb tstruct;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		_ftime(&tstruct);


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
		strftime(timebuf, strlen(timebuf), "%X", timeinfo);

		char msgBuf[1024];
		va_list argptr;
		va_start(argptr, msg);
		vsprintf_s(msgBuf, 1024, msg, argptr);
		va_end(argptr);


		fprintf(fp, "%s\t%s.%.3d\t%s: %s()\tline %d:\t%s\n",
			levels[(int)priority], timebuf, tstruct.millitm, file, function, line, msgBuf);

		end:
		_mutex->unlock();
	}
}
