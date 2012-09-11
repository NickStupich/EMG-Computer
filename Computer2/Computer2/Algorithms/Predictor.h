#ifndef _PREDICTOR_H
#define _PREDICTOR_H

#include "OutputListener.h"
#include "../DataProtocol/DataListener.h"
#include "../DataProtocol/DataProtocol.h"
#include "../Utils/Constants.h"
#include "../Utils/CommonFuncs.h"
#include "../Utils/NMutex.h"
#include "OutputListener.h"
#include "Model.h"
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

	int StopConnection();

	/* from DataListener */
	void OnNewData(unsigned int** data) override;
	void OnError(unsigned int code) override;

	int GetDataLength();

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

	double* _currentPrediction;
	NMutex* _currentPredictionMutex;

	HANDLE _predictionReadyEvent;
	HANDLE _syncThreadHandle;

	void DoPrediction(unsigned int** data);
	void SaveData(unsigned int** data);

	static void StaticSyncThreadStart(void * args);
	void MemberSyncThreadStart();
};

//ugly hack to make things compile
//source: http://forums.codeguru.com/showthread.php?t=250284
#include "Predictor.cpp"

#endif