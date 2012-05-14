#ifndef _SERIAL
#define _SERIAL

#include "DataProtocol.h"
#include <Windows.h>
#include "../Utils/ReturnValues.h"
#include "../Utils/Log.h"

class Serial{

public:
	Serial(wchar_t* port, int baudRate, DataProtocol* protocol);
	~Serial();
	int open();
	int close(); 
	int write(char* buf, int len);

#ifdef _WIN32
#include "WinSerial.h"
#endif

private:
	static void StaticThreadStart(void* args);
	void MemberThreadStart(void* args);
};

#endif