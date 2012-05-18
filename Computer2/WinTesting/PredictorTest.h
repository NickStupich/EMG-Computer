#include "../Computer2/Algorithms/Predictor.h"
#include "../Computer2/Algorithms/Model.h"

class TestModel : public Model
{
public:
	TestModel(int numInputs) : Model(numInputs) {}
	int Train(std::vector<unsigned int**> data, std::vector<double> output, void* info){printf("training\n"); return 0;}
	double Predict(unsigned int** data){printf("predicting...\n"); return 0;}
};

void testPredictor()
{
	Predictor<TestModel>* p = new Predictor<TestModel>(1, 1, NULL, NULL);
	int response = p->StartCollecting();
	if(response != R_SUCCESS)
		printf("fail1\n");

	Sleep(1000);
	response = p->StopCollecting();
	if(response != R_SUCCESS)
		printf("fail2\n");

	response = p->Train();
	if(response != R_SUCCESS)
		printf("fail3\n");

	response = p->StartPredicting();
	if(response != R_SUCCESS)
		printf("fail4\n");

	Sleep(1000);
	response = p->StopPredicting();
	if(response != R_SUCCESS)
		printf("fail5\n");

	printf("done testing predictor\n");
}