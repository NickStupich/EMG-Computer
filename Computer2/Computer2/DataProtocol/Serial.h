#ifndef _SERIAL
#define _SERIAL

#include "DataProtocol.h"
#include <Windows.h>
#include "../Utils/ReturnValues.h"

class Serial{

public:
	Serial(wchar_t* port, int baudRate, DataProtocol* protocol);
	~Serial();
	int open();
	int close(); 

#ifdef _WIN32
#include "WinSerial.h"
#endif

};

#endif