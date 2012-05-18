#include "../Computer2/Algorithms/Predictor.h"
#include "../Computer2/Algorithms/Model.h"

class TestModel : public Model
{
public:
	TestModel(void) : Model() {}
	int Train(std::vector<int**> data, std::vector<double> output){return 0;}
	double Predict(unsigned int** data){return 0;}
};

void testPredictor()
{
	Predictor<TestModel>* p = new Predictor<TestModel>(1, 1, NULL, NULL);

}