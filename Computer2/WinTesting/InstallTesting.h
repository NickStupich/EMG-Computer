#include "../WinInstall/InstallBluetooth.h"

void TestInstall()
{
	char buf[50];
	int response;
	
	response = InstallMuscleMate(); if(response!= R_SUCCESS) printf("fail1\n");
	
}