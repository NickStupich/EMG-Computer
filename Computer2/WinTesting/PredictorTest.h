#include "../Computer2/Algorithms/Predictor.h"
#include "../Computer2/Algorithms/Model.h"

class TestModel : public Model
{
public:
	virtual int Train(std::vector<int**> data, std::vector<double> output);
	virtual double Predict(unsigned int** data);
};

void testPredictor()
{
	Predictor<TestModel>* p = new Predictor<TestModel>(1, 1, NULL, NULL);

}