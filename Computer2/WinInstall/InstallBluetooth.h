#include <WinSock2.h>
#include <Windows.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <bthdef.h>
#include <SetupAPI.h>
#include <vector>
#include <string.h>

#define R_SUCCESS					0
#define R_NO_DEVICE_FOUND			1
#define R_REG_FAIL					2
#define R_REG_PORTS_RETRIEVAL_FAIL	3
#define R_UNKNOWN_SERIAL_PORT		4
#define R_REG_ADD_FAIL				5

int InstallMuscleMate();

int InstallBluetoothDevice(wchar_t** portName);
int AddToRegistry(wchar_t* portName);
int GetBluetoothStruct(BLUETOOTH_DEVICE_INFO* pbtdi);
int AddDevice(BLUETOOTH_DEVICE_INFO* pbtdi, wchar_t ** portName);
int GetComPortsFromRegistry(std::vector<wchar_t*>* ports);
bool wchar_tStrSort(const wchar_t* s1, const wchar_t* s2);
int SaveComPortToRegistry(wchar_t* portName);