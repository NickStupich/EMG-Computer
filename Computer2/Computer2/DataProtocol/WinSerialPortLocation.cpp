#ifdef _WIN32

#include "SerialPortLocation.h"
#include <Windows.h>
#include "../Utils/Log.h"
#include "../Utils/ReturnValues.h"

int GetPortLocation(wchar_t* buf)
{
	HKEY hKey;
	DWORD dwType, dwSize;
	DWORD dwDisposition;

	DWORD response;
	/*
	response = RegQueryValueEx(	HKEY_LOCAL_MACHINE,
								TEXT("Software\\MuscleMate")

	response = RegCreateKeyEx(	HKEY_LOCAL_MACHINE, 
								TEXT("Software\\MuscleMate"),
								0, 
								NULL,
								0,
								0,
								NULL,
								&hKey,
								&dwDisposition);

	if(response != ERROR_SUCCESS)
	{
		LOG_ERROR("Unable to open registry 'folder' with error %d", response);
		return R_UNKNOWN_FAIL;
	}

	char internalBuf[100];
	DWORD cbData = 100;

	response = RegQueryValueEx(	hKey,
								TEXT("SerialPort"),
								NULL,
								NULL,
								(LPBYTE)internalBuf,
								&cbData);

	if(response != ERROR_SUCCESS)
	{
		LOG_ERROR("Unable to open registry key for serial port, error %d", response);
		return R_UNKNOWN_FAIL;
	}
	*/
	return R_SUCCESS;
}

#endif