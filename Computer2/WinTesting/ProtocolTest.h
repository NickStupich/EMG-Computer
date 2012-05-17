#include <stdio.h>
#include "../Computer2/DataProtocol/DataProtocol.h"
#include "../Computer2/DataProtocol/DataListener.h"

class TestDataListener : public DataListener{
public:
	int dataCount;

	TestDataListener()
	{
		this->dataCount = 0;
	}

	void OnNewData(unsigned int** data) override
	{
		printf("got data\n");
		this->dataCount++;
	}

	void OnError(unsigned int code) override
	{
		printf("Got error, code: %d", code);
	}
};

void testProtocol()
{
	printf("Testing data protocol\n\n");
	int response;
	
	/*DataProtocol* dp1 = new DataProtocol(1, NULL);
	response = dp1->Start();

	if(response != R_SUCCESS)
		printf("Failed test1\n");

	Sleep(1000);
	response = dp1->Stop();

	if(response != R_SUCCESS)
		printf("Failed test2\n");
		*/
	TestDataListener* dl = new TestDataListener();
	DataProtocol* dp2 = new DataProtocol(1, (DataListener*)dl);
	response = dp2->Start();
	if(response != R_SUCCESS)
		printf("failed test3\n");

	Sleep(1000);

	response = dp2->Stop();
	if(response != R_SUCCESS)
		printf("failed test4\n");

	printf("Number of data updates to the listener: %d\n", dl->dataCount);

	printf("Done data protocol test\n");
}