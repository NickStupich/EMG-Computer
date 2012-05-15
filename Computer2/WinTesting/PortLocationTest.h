#include "../Computer2/DataProtocol/SerialPortLocation.h"

void testPortLocation()
{
	printf("Starting port location test\n");

	wchar_t buf[100];
	int response = GetPortLocation(buf);
	if(response != R_SUCCESS)
		printf("Failed test1\n");

	printf("Done port location test\n\n");
}