#include <stdio.h>
#include "../Computer2/DataProtocol/DataProtocol.h"

void testProtocol()
{
	printf("Testing data protocol\n\n");
	int response;
	
	DataProtocol* dp1 = new DataProtocol(1, NULL);
	response = dp1->Start();

	if(response != R_SUCCESS)
		printf("Failed test1\n");

	Sleep(1000);
	response = dp1->Stop();

	if(response != R_SUCCESS)
		printf("Failed test2\n");

	printf("Done data protocol test\n");
}