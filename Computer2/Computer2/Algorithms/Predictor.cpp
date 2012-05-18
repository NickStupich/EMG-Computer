#include "Predictor.h"

template<class T>
Predictor<T>::Predictor(unsigned int channels, int numOutputs, OutputListener* outputListener, void** modelsInfo)
{
	this->_models = new T*[numOutputs];
	for(int i=0;i<numOutputs;i++)
		this->_models[i] = new T();

	this->_state = INIT;
	this->_outputListener = outputListener;
	this->_channels = channels;
	this->_numChannels = ONES_IN_BYTE(channels);
	this->_numOutputs = numOutputs;
	this->_dataProtocol = new DataProtocol(channels, this);

	this->_trainingOutputs = new std::vector<double>[numOutputs];
	//no need to do anything with _trainingData?

	this->_currentOutput = new double[numOutputs];
	this->_currentOutputMutex = new NMutex();
	this->_modelsInfo = modelsInfo;
}

template<class T>
Predictor<T>::~Predictor()
{
	//free up stuffs
}

template<class T>
int Predictor<T>::StartCollecting()
{
	if(	this->_state = COLLECTING 
		|| this->_state == PREDICTING 
		|| this->_state == FAIL
		|| this->_state == DONE
		)
		return R_INVALID_STATE;

	if(!this->_dataProtocol->IsRunning())
	{
		int response = this->_dataProtocol->Start();
		if(response != R_SUCCESS)
		{
			this->_state = ERROR;
			return response;
		}
	}

	this->_state = COLLECTING;
}

template<class T>
int Predictor<T>::StopCollecting()
{
	//keep the data protocol running, so we can quickly restart collecting, or quickly began outputting 
	//results once trained
	if(this->_state == COLLECTING)
	{
		this->_state = COLLECTED;
		return R_SUCCESS;
	}
	else
	{
		return R_INVALID_STATE;
	}
}

template<class T>
void Predictor<T>::SetOutput(double* output)
{
	memcpy(this->_currentOutput, output, this->_numOutputs * sizeof(double));
}

template<class T>
int Predictor<T>::Train()
{
	if(this->_state != COLLECTED)
		return R_INVALID_STATE;

	int result = R_SUCCESS;

	for(int i=0;i<numOutputs;i++)
	{
		int response = this->_models[i]->Train(this->_trainingData, this->_trainingOutputs[i], this->_modelsInfo[i]);
		LOG_DEBUG("Finished training model %d with result %d", i, response);
		if(response != R_SUCCESS)
			result = response;
	}
	
	this->_state = TRAINED:

	return result;
}

template<class T>
int Predictor<T>::StartPredicting()
{
	if(this->_state != TRAINED)
		return R_INVALID_STATE;

	this->_state = PREDICTING;
	return R_SUCCESS;
}

template<class T>
int Predictor<T>::StopPredicting()
{
	if(this->_state != PREDICTING)
		return R_INVALID_STATE;

	this->_state = DONE;
	return R_SUCCESS;
}

template<class T>
void Predictor<T>::OnNewData(unsigned int** data)
{
	switch(this->_state)
	{

	}
}

template<class T>
void Predictor<T>::OnError(unsigned int code)
{
	if(this->_outputListener != NULL)
	{
		this->_outputListener->OnError(code);
	}
}