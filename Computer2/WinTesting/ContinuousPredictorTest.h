#include "../Computer2/Algorithms/Predictor.h"
#include "../Computer2/Algorithms/ContinuousModel.h"
#include "../Computer2/Algorithms/OutputListener.h"

class TestOutputListener : public OutputListener
{
	void OnNewOutput(double* output) override {printf("Prediction: %lf\n", output[0]);}
	void OnError(unsigned int code) override {
		printf("Error: %d\n", code);}
};

void testContinuousPredictor()
{
	TestOutputListener* o = new TestOutputListener();
	Predictor<ContinuousModel>* p = new Predictor<ContinuousModel>(1, 1, o, NULL);
	double output[1] = {0.0};

	int response;
	p->SetOutput(output);
	
	response = p->StartCollecting();
	if(response != R_SUCCESS)
		printf("fail1\n");
	printf("low output starting\n");

	Sleep(5000);

	printf("High output starting\n");
	output[0] = 1.0;
	p->SetOutput(output);
	Sleep(5000);

	response = p->StopCollecting();
	if(response != R_SUCCESS)
		printf("fail2\n");

	response = p->Train();
	if(response != R_SUCCESS)
		printf("fail3\n");

	response = p->StartPredicting();
	if(response != R_SUCCESS)
		printf("fail4\n");

	printf("hit enter to stop\n");getchar();
//	Sleep(2000);

	response = p->StopPredicting();
	if(response != R_SUCCESS)
		printf("fail5\n");
	
	printf("Done continuous predictor test\n");
}