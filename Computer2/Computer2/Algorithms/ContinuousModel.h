#ifndef _CONTINUOUS_MODEL
#define _CONTINUOUS_MODEL

#include "Model.h"
#include "../Utils/Constants.h"
#include "../Utils/ReturnValues.h"
#include "../Utils/Log.h"

class ContinuousModel : public Model
{
public:
	ContinuousModel(int numInputs);
	int Train(std::vector<unsigned int**> data, std::vector<double> output, void* info) override;
	double Predict(unsigned int** data) override;

private:
	double** _pearsons;
	double** _yInts;
	double** _slopes;
	double _outputDenominator;

	void CalculatePearsonCorrelations(std::vector<unsigned int**> data, std::vector<double> output);
	void CalculateSlopeAndInts(std::vector<unsigned int**> data, std::vector<double> output);
	void LineOfBestFit(std::vector<double> x, std::vector<double> y, double* m, double* b);
};


#endif