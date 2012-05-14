#include <stdio.h>

#include "SerialTest.h"

int main(int argc, char* argv[])
{
	testSerial();

	printf("done testing, enter to quit\n");
	getchar();
	return 0;
}

