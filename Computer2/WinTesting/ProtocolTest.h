#include <stdio.h>
#include "../Computer2/DataProtocol/DataProtocol.h"
#include "../Computer2/DataProtocol/DataListener.h"

class TestDataListener : public DataListener{
public:
	int dataCount, delay;

	TestDataListener(int delayMs)
	{
		this->delay = delayMs;
		this->dataCount = 0;
	}

	void OnNewData(unsigned int** data) override
	{
		Sleep(this->delay);
		//printf("got data\n");
		for(int i=0;i<DATA_LENGTH;i++)
		{
			printf("%d\t", data[7][i]);
		}
		printf("\n");
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
	/*
	DataProtocol* dp1 = new DataProtocol(1, NULL);
	response = dp1->Start();

	if(response != R_SUCCESS)
		printf("Failed test1\n");

	Sleep(1000);
	response = dp1->Stop();

	if(response != R_SUCCESS)
		printf("Failed test2\n");

	Sleep(500);
	*/	
	/*TestDataListener* dl = new TestDataListener(0);
	DataProtocol* dp2 = new DataProtocol(1, (DataListener*)dl);
	response = dp2->Start();
	if(response != R_SUCCESS)
		printf("failed test3\n");
	
	Sleep(5000);

	response = dp2->Stop();
	if(response != R_SUCCESS)
		printf("failed test4\n");

	printf("Number of data updates to the listener: %d\n", dl->dataCount);
	*/
	TestDataListener* dl2 = new TestDataListener(1);
	DataProtocol* dp3 = new DataProtocol(255, (DataListener*)dl2);
	response = dp3->Start();
	if(response != R_SUCCESS)
		printf("failed test5\n");

	Sleep(500000);

	response = dp3->Stop();
	if(response != R_SUCCESS)
		printf("failed test6\n");

	printf("received %d updates\n", dl2->dataCount);
	if(dl2->dataCount > 6)
		printf("received too many updates...");


	printf("Done data protocol test\n");
}