#include "ContinuousModel.h"

ContinuousModel::ContinuousModel(int numInputs)
	: Model(numInputs)
{
	this->_pearsons = new double*[numInputs];
	this->_yInts = new double*[numInputs];
	this->_slopes = new double*[numInputs];

	for(int input = 0;input<numInputs;input++)
	{
		this->_pearsons[input] = new double[DATA_LENGTH];
		this->_yInts[input] = new double[DATA_LENGTH];
		this->_slopes[input] = new double[DATA_LENGTH];
	}
}

ContinuousModel::~ContinuousModel()
{

}

int ContinuousModel::Train(std::vector<unsigned int**> data, std::vector<double> output, void* info)
{
	if(data.size() != output.size())
		return R_DATA_LENGTHS_NOT_MATCHING;

	LOG_DEBUG("Length of training data: %d", data.size());

	this->CalculatePearsonCorrelations(data, output);
	this->CalculateSlopeAndInts(data, output);

	this->_outputDenominator = 0;
	for(int input = 0;input < this->_numInputs;input++)
	{
		for(int bin = 0; bin < DATA_LENGTH;bin++)
		{
			this->_outputDenominator += fabs(this->_pearsons[input][bin]);
		}
	}

	return R_SUCCESS;
}

void ContinuousModel::CalculatePearsonCorrelations(std::vector<unsigned int**> data, std::vector<double> output)
{
	unsigned int i;

	for(int input = 0;input < this->_numInputs;input++)
	{
		for(int bin = 0; bin < DATA_LENGTH;bin++)
		{
			//calculate the pearson correlation between input and output
			double meanIn = 0, meanOut = 0;
			for(i = 0;i < data.size();i++)
			{
				meanIn += data[i][input][bin];
				meanOut += output[i];
			}

			meanIn /= data.size();
			meanOut /= data.size();

			double covariance = 0, varianceIn = 0, varianceOut = 0;

			for(i=0;i<data.size();i++)
			{
				covariance += (data[i][input][bin] - meanIn) * (output[i] - meanOut);
				varianceIn += (data[i][input][bin] - meanIn) * (data[i][input][bin] - meanIn);
				varianceOut += (output[i] - meanOut) * (output[i] - meanOut);
			}

			varianceIn /= (double)(data.size() - 1);
			varianceOut /= (double)(data.size() - 1);
			covariance /= (double)(data.size() - 1);

			double pearson;

			if(varianceIn == 0 || varianceOut == 0)
			{
				LOG_ERROR("Variance %s in 0 - returning 0 as Pearson[%d][%d] score so this input/bin is ignored",  varianceIn == 0 ? "In" : "Out", input, bin);
				pearson = 0;
			}
			else
			{
				pearson = covariance / sqrt(varianceIn * varianceOut);
				LOG_DEBUG("Pearson[%d][%d] calculated as %lf", input, bin, pearson);
			}

			this->_pearsons[input][bin] = pearson;
		}
	}
}

void ContinuousModel::CalculateSlopeAndInts(std::vector<unsigned int**> data, std::vector<double> output)
{
	unsigned int i;

	//copy of the log() of data for a single bin of a single input
	std::vector<double> singleData(data.size());

	for(int input = 0;input < this->_numInputs;input++)
	{
		for(int bin = 0; bin < DATA_LENGTH;bin++)
		{
			for(i=0;i<data.size();i++)
			{
				singleData[i] = log10((double)data[i][input][bin]);
			}

			this->LineOfBestFit(singleData, output, &this->_slopes[input][bin], &this->_yInts[input][bin]);
		}
	}
}

void ContinuousModel::LineOfBestFit(std::vector<double> x, std::vector<double> y, double* m, double* b)
{
	double sx2 = 0, sxy = 0, sx = 0, sy = 0;
	double n = (double) x.size();

	for(unsigned int i=0;i<x.size();i++)
	{
		sx += x[i];
		sy += y[i];
		sxy += x[i] * y[i];
		sx2 += x[i] * x[i];
	}

	double denom = n * sx2 - sx * sx;

	*m = (n * sxy - sx * sy) / denom;
	*b = (sy * sx2 - sx * sxy) / denom;
}

double ContinuousModel::Predict(unsigned int** data)
{
	double num = 0, pred;

	for(int input = 0;input < this->_numInputs;input++)
	{
		for(int bin = 0; bin < DATA_LENGTH;bin++)
		{
			pred = log10((double)data[input][bin]) * this->_slopes[input][bin] + this->_yInts[input][bin];
			num += this->_pearsons[input][bin] * pred;
		}
	}
	
	return num / this->_outputDenominator;
}