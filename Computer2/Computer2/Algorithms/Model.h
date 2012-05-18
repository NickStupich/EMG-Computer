#ifndef _MODEL
#define _MODEL

#include <vector>

class Model
{
public:
	Model(int numInputs){
		this->_numInputs = numInputs;
	}

	virtual int Train(std::vector<unsigned int**> data, std::vector<double> output, void* info){return 0;}
	virtual double Predict(unsigned int** data){return 0;}

protected:
	int _numInputs;
};

#endif