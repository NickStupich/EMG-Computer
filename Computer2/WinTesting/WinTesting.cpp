#include <stdio.h>

#include "SerialTest.h"
#include "ProtocolTest.h"
#include "PortLocationTest.h"
#include "ProtocolIntegrationTest.h"
#include "PredictorTest.h"
#include "ContinuousPredictorTest.h"

int main(int argc, char* argv[])
{
	//testSerial();
	//testProtocol();
	
	//createRegistryKeys();
	//testPortLocation();
	
	//TestProtocolIntegration();
	//testPredictor();
	testContinuousPredictor();

	printf("done testing, enter to quit\n");
	getchar();
	return 0;
}

