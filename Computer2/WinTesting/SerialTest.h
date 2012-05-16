#include <stdio.h>
#include "../Computer2/DataProtocol/Serial.h"

#define COM_PORT	L"COM4"

void testSerial()
{
	int response;
	printf("Running serial test");

	Serial* ser = new Serial(COM_PORT, 57600, NULL);
	response = ser->Open();
	if(response != R_SUCCESS)
		printf("Failed test 1");

	Serial* ser2 = new Serial(COM_PORT, 57600, NULL);
	response = ser2->Open();
	if(response != R_ACCESS_DENIED)
		printf("Failed test2");

	response = ser2->Close();
	if(response != R_SUCCESS)
		printf("Failed test3");

	response = ser->Open();
	if(response != R_ALREADY_OPEN)
		printf("Failed test4");

	response = ser->Close();
	if(response != R_SUCCESS)
		printf("Failed test5");
		
	Serial* ser3 = new Serial(COM_PORT, 57600, NULL);
	response = ser3->Open();
	if(response != R_SUCCESS)
		printf("Failed test6");

	response = ser3->Write(1<<7 | 1);
	if(response != R_SUCCESS)
		printf("Failed test7");

	//need this here to stop the microcontroller
	response = ser3->Write(0);
	if(response != R_SUCCESS)
		printf("Failed test8");


	Sleep(2000);

	ser3->Close();

	printf("Serial test complete\n\n");
}