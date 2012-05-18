#include "OutputListener.h"
#include "../DataProtocol/DataListener.h"
#include "../DataProtocol/DataProtocol.h"
#include "../Utils/Constants.h"
#include "../Utils/NMutex.h"
#include <vector>

enum PredictorState{
	INIT,
	COLLECTING,
	COLLECTED,
	TRAINED,
	PREDICTING,
	DONE,
	FAIL,
};

template<class T>
class Predictor : public DataListener{
public:
	Predictor(unsigned int channels, int numOutputs, OutputListener* outputListener, void** modelsInfo);
	~Predictor();
	int StartCollecting();
	int StopCollecting();
	void SetOutput(double* output);

	int Train();

	int StartPredicting();
	int StopPredicting();

	/* from DataListener */
	void OnNewData(unsigned int** data) override;
	void OnError(unsigned int code) override;

private:
	OutputListener* _outputListener;
	unsigned int _channels;
	int _numChannels;
	int _numOutputs;
	PredictorState _state;
	T** _models;
	DataProtocol* _dataProtocol;

	//data containers
	std::vector<unsigned int**> _trainingData;
	std::vector<double>* _trainingOutputs;

	double* _currentOutput;
	NMutex* _currentOutputMutex;

	void** _modelsInfo;
};