#include <stdio.h>

#include "SerialTest.h"
#include "../Computer2/Utils/Log.h"

int main(int argc, char* argv[])
{
	//testSerial();
	LOG_DEBUG("the number four: %d", 4);
	log(Debug, __FILE__, __FUNCTION__, __LINE__, "the number five: %d", 5);

	printf("done testing, enter to quit\n");
	getchar();
	return 0;
}

