#ifndef _MODEL
#define _MODEL

#include <vector>

class Model
{
public:
	Model(void){}
	virtual int Train(std::vector<int**> data, std::vector<double> output, void* info){return 0;}
	virtual double Predict(unsigned int** data){return 0;}
};

#endif