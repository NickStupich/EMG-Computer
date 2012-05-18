#include <vector>

class Model
{
public:
	Model();
	virtual int Train(std::vector<int**> data, std::vector<double> output, void* info);
	virtual double Predict(unsigned int** data);
};