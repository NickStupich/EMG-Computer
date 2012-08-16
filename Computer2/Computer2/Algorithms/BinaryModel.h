#ifndef _BINARY_MODEL
#define _BINARY_MODEL

#include "Model.h"
#include "../Utils/Constants.h"
#include "../Utils/ReturnValues.h"
#include "../Utils/Log.h"
#include "svm.h"

typedef struct {
	double weight0;
	double weight1;
} BinaryModelParams;

class BinaryModel: public Model
{
public:
	BinaryModel(int numInputs);
	~BinaryModel();
	int Train(std::vector<unsigned int**> data, std::vector<double> output, void* info) override;
	double Predict(unsigned int** data) override;

private:
	svm_model* _model;
	void EvalCrossValidation(struct svm_problem* prob, struct svm_parameter* param);
};

#endif