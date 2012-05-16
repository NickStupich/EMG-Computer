#include "../Computer2/DataProtocol/SerialPortLocation.h"

#include <Windows.h>


void createRegistryKeys()
{
	HKEY hKey;
	DWORD dwDisposition;
	int response;

	response = RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
								TEXT("SOFTWARE\\MuscleMate"),
								0,
								NULL,
								0,
								KEY_WRITE,
								NULL,
								&hKey,
								&dwDisposition);
	if(response != ERROR_SUCCESS)
	{
		printf("Fail 1\n");
		return;
	}

	DWORD dwType = REG_SZ;
	TCHAR* val = L"COM4";
	


	response = RegSetValueEx(	hKey,
								TEXT("Serial"),
								0,
								dwType,
								(PBYTE) val,
								lstrlen(val)*sizeof(*val));


	
}

void testPortLocation()
{
	printf("Starting port location test\n");

	wchar_t* buf;
	int response = GetPortLocation(&buf);
	if(response != R_SUCCESS)
		printf("Failed test1\n");

	printf("Done port location test\n\n");
}