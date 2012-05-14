#include <stdio.h>
#include <Windows.h>
#include "../Computer2/DataProtocol/Serial.h"

#define COM_PORT	L"COM4"

void testSerial()
{
	int response;
	Serial* ser = new Serial(COM_PORT, 57600, NULL);
	response = ser->open();
	if(response != R_SUCCESS)
		printf("Failed test 1");

	Sleep(2000);

	Serial* ser2 = new Serial(COM_PORT, 57600, NULL);
	response = ser2->open();
	if(response != R_ACCESS_DENIED)
		printf("Failed test2");

	response = ser2->close();
	if(response != R_SUCCESS)
		printf("Failed test3");

	response = ser->open();
	if(response != R_ALREADY_OPEN)
		printf("Failed test4");

	response = ser->close();
	if(response != R_SUCCESS)
		printf("Failed test5");

	Serial* ser3 = new Serial(COM_PORT, 57600, NULL);
	response = ser3->open();
	if(response != R_SUCCESS)
		printf("Failed test6");

	char buf[] = {1<<7 | 1};
	response = ser3->write(buf, 1);

	ser->close();

	printf("Serial test complete\n\n");
}