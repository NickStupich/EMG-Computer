#include <stdio.h>

#include "SerialTest.h"
#include "ProtocolTest.h"


int main(int argc, char* argv[])
{
	//testSerial();
	testProtocol();
	printf("done testing, enter to quit\n");
	getchar();
	return 0;
}

