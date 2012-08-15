#include "../Computer2/Algorithms/Predictor.h"
#include "../Computer2/Algorithms/ContinuousModel.h"
#include "../Computer2/Algorithms/OutputListener.h"
#include "../Computer2/Algorithms/BinaryModel.h"

class TestOutputListener : public OutputListener
{
	void OnNewOutput(double* output) override {
		printf("Prediction: %lf\n", output[0]);
		//Sleep(500);
	}
	void OnError(unsigned int code) override {
		printf("Error: %d\n", code);}
};

template <class T>
void testPredictor()
{
	TestOutputListener* o = new TestOutputListener();
	Predictor<T>* p = new Predictor<T>(255, 1, o, NULL);
	double output[1] = {0.0};

	int response;
	p->SetOutput(output);
	
	printf("low output starting\n");
	Sleep(1000);
	response = p->StartCollecting();
	if(response != R_SUCCESS)
		printf("fail1a\n");

	Sleep(5000);
	
	response = p->StopCollecting();
	if(response != R_SUCCESS)
		printf("fail1b\n");

	printf("High output starting\n");
	Sleep(1000);
	
	output[0] = 1.0;
	p->SetOutput(output);
	response = p->StartCollecting();
	if(response != R_SUCCESS)
		printf("fail2a\n");

	Sleep(5000);

	response = p->StopCollecting();
	if(response != R_SUCCESS)
		printf("fail2b\n");

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
	
	response = p->StopConnection();
	if(response != R_SUCCESS)
		printf("fail6\n");

	printf("Done continuous predictor test\n");
}


void testContinuousPrediction()
{
	testPredictor<ContinuousModel>();
}

void testBinaryPrediction()
{
	testPredictor<BinaryModel>();

	/*std::vector<unsigned int**> data;
	unsigned int** x = (unsigned int**) malloc(sizeof(int*));
	unsigned int** y = (unsigned int**) malloc(sizeof(int*));
	x[0] = (unsigned int*) malloc(10 * sizeof(int));
	x[0][1] = 0;x[0][2] = 0;x[0][3] = 0;x[0][4] = 0;x[0][5] = 0;x[0][6] = 0;x[0][7] = 0;x[0][8] = 0;x[0][9] = 0;
	x[0][0] = 1;
	data.push_back(x);
	
	y[0] = (unsigned int*) malloc(10 * sizeof(int));
	y[0][1] = 0;y[0][2] = 0;y[0][3] = 0;y[0][4] = 0;y[0][5] = 0;y[0][6] = 0;y[0][7] = 0;y[0][8] = 0;y[0][9] = 0;
	y[0][0] = 1;

	data.push_back(y);

	std::vector<double> output;
	output.push_back(1);
	output.push_back(0);

	BinaryModel* bm = new BinaryModel(1);
	bm->Train(data, output, NULL);
	*/

}