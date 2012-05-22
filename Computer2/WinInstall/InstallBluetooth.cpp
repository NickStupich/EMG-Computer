#include "InstallBluetooth.h"
#include "../Computer2/Utils/Constants.h"

int InstallMuscleMate()
{
	wchar_t* portName = new wchar_t[50];
	int result = InstallBluetoothDevice(&portName);
	return result;
}

int InstallBluetoothDevice(wchar_t** portName)
{
	BLUETOOTH_DEVICE_INFO pbtdi;
	int response = GetBluetoothStruct(&pbtdi);
	if(response != R_SUCCESS)
	{
		return response;
	}

	response = AddDevice(&pbtdi, portName);
	if(response != R_SUCCESS)
	{
		return R_SUCCESS;
	}

	response = SaveComPortToRegistry(*portName);

	return 0;
}

int SaveComPortToRegistry(wchar_t* portName)
{
	HKEY hKey;
	DWORD dwDisposition;
	int response;

	response = RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
								WIN_REG_KEY_FOLDER,
								0,
								NULL,
								0,
								KEY_WRITE,
								NULL,
								&hKey,
								&dwDisposition);
	if(response != ERROR_SUCCESS)
	{
		return R_REG_ADD_FAIL;
	}

	DWORD dwType = REG_SZ;
	
	response = RegSetValueEx(	hKey,
								WIN_REG_KEY_SERIAL,
								0,
								dwType,
								(PBYTE) portName,
								wcslen(portName)*sizeof(*portName));
	
	if(response != ERROR_SUCCESS)
	{
		return R_REG_ADD_FAIL;
	}

	return R_SUCCESS;
}

int AddDevice(BLUETOOTH_DEVICE_INFO* pbtdi, wchar_t** portName)
{
	PWCHAR passKey = L"1234";
	DWORD result = BluetoothAuthenticateDevice(	NULL,
												NULL,
												pbtdi,
												passKey,
												wcslen(passKey));

	if(result != ERROR_SUCCESS)
	{
		return result;
	}

	/*PROBLEM: as far as i can tell, you can't get the port name (Ex: COM4) 
	from the windows api when adding and activating a bluetooth device.
	SOLUTION: check the registry for serial ports before and after, 
	and take the difference as being the new port
	*/

	std::vector<wchar_t*> serialPortsBefore, serialPortsAfter;

	result = GetComPortsFromRegistry(&serialPortsBefore);
	if(result != R_SUCCESS)
	{
		return R_REG_PORTS_RETRIEVAL_FAIL;
	}

	//set service state on device
	GUID service = SerialPortServiceClass_UUID;
	result = BluetoothSetServiceState(	NULL,
										pbtdi,
										&service,
										BLUETOOTH_SERVICE_ENABLE);

	if(result != ERROR_SUCCESS)
	{
		return result;
	}

	//it can take some time for the registry keys to show up, so we'll retry a couple times and delay between them
	for(int retry = 0; retry < 3;retry++)
	{
		result = GetComPortsFromRegistry(&serialPortsAfter);
		if(result != R_SUCCESS)
		{
			return R_REG_PORTS_RETRIEVAL_FAIL;
		}

		//if theres a size difference assume it's the new port and stop retrying
		if(serialPortsBefore.size() != serialPortsAfter.size())
			break;

		//wait a bit for the reg key to show up
		Sleep(2000);
	}

	std::vector<wchar_t*> diff;
	for(int i=0;i<serialPortsAfter.size();i++)
	{
		bool found = false;
		for(int j=0;j<serialPortsBefore.size();j++)
		{
			if(wcscmp(serialPortsAfter[i], serialPortsBefore[j]) == 0)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			diff.push_back(serialPortsAfter[i]);
		}
	}

	if(diff.size() == 0)
	{
		return R_UNKNOWN_SERIAL_PORT;
	}
	else if(diff.size() > 1)
	{
		return R_UNKNOWN_SERIAL_PORT;
	}

	//copy the string to the provided buffer
	wcscpy(*portName, diff[0]);

	return result;

}

bool wchar_tStrSort(const wchar_t* s1, const wchar_t* s2)
{
	return wcscmp(s1, s2) > 0;
}

int GetComPortsFromRegistry(std::vector<wchar_t*>* ports)
{
	HKEY key;
	long ret;

	ret = RegOpenKey(	HKEY_LOCAL_MACHINE,
						TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
						&key);

	if(ret != ERROR_SUCCESS)
	{
		return R_REG_FAIL;
	}

	DWORD index = 0;
	const int keyBufLenConst = 255;
	const int valBufLenConst = 32767;

	wchar_t keyBuf[keyBufLenConst];
	wchar_t valBuf[32767];
	DWORD keyBufLen;
	DWORD valBufLen;

	while(TRUE)
	{
		//reset lengths to avoid ERROR_MORE_DATA return value
		keyBufLen = keyBufLenConst;
		valBufLen = valBufLenConst;

		ret = RegEnumValue(	key,
							index++,
							keyBuf,
							&keyBufLen,
							NULL,
							NULL,
							(LPBYTE)valBuf,
							&valBufLen);

		if(ret == ERROR_SUCCESS){
			ports->push_back(valBuf);
		}
		else if(ret == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		else
		{			
			return R_REG_FAIL;
		}
	}


	return R_SUCCESS;
}

int AddToRegistry(wchar_t* portName)
{
	return 0;
}

bool IsMuscleMateDevice(BLUETOOTH_DEVICE_INFO* pbtdi)
{
	WCHAR name[] = L"FireFly-9B21";
	return (wcscmp(name, pbtdi->szName) == 0);
}

int GetBluetoothStruct(BLUETOOTH_DEVICE_INFO* pbtdi)
{
	pbtdi->dwSize = sizeof(BLUETOOTH_DEVICE_INFO);

	BLUETOOTH_DEVICE_SEARCH_PARAMS bdsp;

	bdsp.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	bdsp.fReturnAuthenticated = false;
	bdsp.fReturnRemembered = false;
	bdsp.fReturnUnknown = true;
	bdsp.fReturnConnected = false;
	bdsp.cTimeoutMultiplier = 5;	//8 second timeout
	bdsp.hRadio = NULL;				//all bluetooth radios

	//BLUETOOTH_DEVICE_INFO pbtdi;

	HANDLE btHandle = BluetoothFindFirstDevice(	&bdsp,
												pbtdi);

	
	int result = -1;

	//handle errors
	if(btHandle == NULL)
	{
		int error = GetLastError();
		if(error == ERROR_NO_MORE_ITEMS)
			result = R_NO_DEVICE_FOUND;

		result = error;
	}


	while(result < 0)
	{
		//check if we found the right device
		if(IsMuscleMateDevice(pbtdi))
		{
			result = R_SUCCESS;
		}
		//look for the next device
		else if(!BluetoothFindNextDevice(btHandle, pbtdi))
		{
			//error(s), deal with it
			int error = GetLastError();
			if(error == ERROR_NO_MORE_ITEMS)
				result = R_NO_DEVICE_FOUND;
			else
				result = error;
		}

	}	
	
	//free up the list
	BluetoothFindDeviceClose(btHandle);

	return result;
}