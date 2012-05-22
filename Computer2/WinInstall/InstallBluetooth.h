#include <WinSock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>

int InstallMuscleMate()
{
	return 0;
}

int AddToWindows(char** portName)
{
	return 0;

}

int GetBluetoothStruct()
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS bdsp;

	bdsp.fReturnAuthenticated = false;
	bdsp.fReturnRemembered = false;
	bdsp.fReturnUnknown = true;
	bdsp.fReturnConnected = false;
	bdsp.cTimeoutMultiplier = 10;	//12.8 second timeout
	bdsp.hRadio = NULL;				//all bluetooth radios

	BLUETOOTH_DEVICE_INFO pbtdi;

	HANDLE btHandle = BluetoothFindFirstDevice(	&bdsp,
												&pbtdi);

	if(btHandle == NULL)
	{
		int error = GetLastError();
		return error;
	}

	return 0;
}