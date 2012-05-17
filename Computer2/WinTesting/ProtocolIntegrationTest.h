#include <stdio.h>
#include "../Computer2/DataProtocol/DataProtocol.h"
#include "../Computer2/DataProtocol/DataListener.h"


class IntegrationTestDataListener : public DataListener{
public:

	void OnNewData(unsigned int** data) override
	{
		printf("%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t\n", data[0][0], data[0][1], data[0][2], data[0][3], data[0][4], data[0][5], data[0][6], data[0][7]);
	}

	void OnError(unsigned int code) override
	{
		printf("*******ERROR*******\n");
	}
};

void TestProtocolIntegration()
{
	IntegrationTestDataListener* tl = new IntegrationTestDataListener();
	DataProtocol* dp = new DataProtocol(1, tl);
	int response;

	response = dp->Start();
	if(response != ERROR_SUCCESS)
	{
		printf("failed to start: %d\n", response);
		return;
	}
	
	printf("Hit Enter to finish\n");
	Sleep(2000);
	//getchar();

	response = dp->Stop();
	if(response != ERROR_SUCCESS)
	{
		printf("failed to stop\n");
		return;
	}

	printf("Done integration test\n");
}