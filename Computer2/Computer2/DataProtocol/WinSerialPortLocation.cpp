#ifdef _WIN32

#include "SerialPortLocation.h"
#include <Windows.h>
#include "../Utils/Log.h"
#include "../Utils/ReturnValues.h"
#include "../Utils/Constants.h"

int GetPortLocation(wchar_t** buf)
{
	HKEY key;
	*buf = new wchar_t[PORT_ADDRESS_BUFFER_LENGTH];
	DWORD bufLen = lstrlen(*buf);
	long ret;
	LPDWORD type = 0;

	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						TEXT("SOFTWARE\\MuscleMate"),
						0, 
						KEY_QUERY_VALUE,
						&key);

	if(ret != ERROR_SUCCESS)
	{
		LOG_ERROR("Opening MuscleMate reg key, error code: %d", ret);
		return R_UNKNOWN_FAIL;
	}

	ret = RegQueryValueEx(	key,
							TEXT("Serial"),
							0,
							0,
							(LPBYTE) (*buf),
							&bufLen);

	if(ret != ERROR_SUCCESS)
	{
		LOG_ERROR("Opening Serial reg key, error code: %d", ret);
		return R_UNKNOWN_FAIL;
	}

	RegCloseKey(key);

	return R_SUCCESS;
}

#endif