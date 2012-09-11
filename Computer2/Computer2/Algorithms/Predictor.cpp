#ifndef _PREDICTOR_CPP
#define _PREDICTOR_CPP

#include "Predictor.h"
#include <process.h>

template<class T>
Predictor<T>::Predictor(unsigned int channels, int numOutputs, OutputListener* outputListener, void** modelsInfo)
{
	this->_state = INIT;
	this->_outputListener = outputListener;
	this->_channels = channels;
	this->_numChannels = ONES_IN_BYTE(channels);
	this->_numOutputs = numOutputs;
	this->_dataProtocol = new DataProtocol(channels, this);

	this->_models = new T*[numOutputs];
	for(int i=0;i<numOutputs;i++)
		this->_models[i] = new T(this->_numChannels);

	this->_trainingOutputs = new std::vector<double>[numOutputs];
	//no need to do anything with _trainingData?

	this->_currentOutput = new double[numOutputs];
	this->_currentPrediction = new double[numOutputs];
	this->_modelsInfo = modelsInfo;
	
	//these could probably be the same thing, but I think it's a bit easier to follow this way
	this->_currentOutputMutex = new NMutex();
	this->_currentPredictionMutex = new NMutex();

	this->_predictionReadyEvent = CreateEvent(	NULL,
												true, 
												false,
												NULL);
}

template<class T>
int Predictor<T>::GetDataLength()
{
	return this->_trainingData.size();
}

template<class T>
Predictor<T>::~Predictor()
{
	this->_dataProtocol->Stop();
}

template<class T>
int Predictor<T>::StartCollecting()
{
	LOG_DEBUG("called");
	if(	this->_state == COLLECTING 
		|| this->_state == PREDICTING 
		|| this->_state == FAIL
		|| this->_state == DONE
		)
	{
		LOG_ERROR("Invalid state of %d", this->_state);
		return R_INVALID_STATE;
	}

	if(!this->_dataProtocol->IsRunning())
	{
		int response = this->_dataProtocol->Start();
		if(response != R_SUCCESS)
		{
			this->_state = FAIL;
			return response;
		}
	}

	this->_state = COLLECTING;
	return R_SUCCESS;
}

template<class T>
int Predictor<T>::StopCollecting()
{	
	LOG_DEBUG("called");
	//keep the data protocol running, so we can quickly restart collecting, or quickly began outputting 
	//results once trained
	if(this->_state == COLLECTING)
	{
		this->_state = COLLECTED;
		return R_SUCCESS;
	}
	else
	{
		LOG_ERROR("Invalid state of %d", this->_state);
		return R_INVALID_STATE;
	}
}

template<class T>
void Predictor<T>::SetOutput(double* output)
{
	this->_currentOutputMutex->lock();
	memcpy(this->_currentOutput, output, this->_numOutputs * sizeof(double));
	this->_currentOutputMutex->unlock();
}

template<class T>
int Predictor<T>::Train()
{
	LOG_DEBUG("called");
	if(this->_state != COLLECTED)
	{
		LOG_ERROR("Invalid state of %d", this->_state);
		return R_INVALID_STATE;
	}

	int result = R_SUCCESS;

	for(int i=0;i<this->_numOutputs;i++)
	{
		int response;
		if(this->_modelsInfo == NULL)
			response = this->_models[i]->Train(this->_trainingData, this->_trainingOutputs[i], NULL);
		else
			response = this->_models[i]->Train(this->_trainingData, this->_trainingOutputs[i], this->_modelsInfo[i]);

		LOG_DEBUG("Finished training model %d with result %d", i, response);
		if(response != R_SUCCESS)
			result = response;
	}
	
	this->_state = TRAINED;

	return result;
}

template<class T>
int Predictor<T>::StartPredicting()
{
	LOG_DEBUG("called");
	if(this->_state != TRAINED)
	{
		LOG_ERROR("invalid state of %d", this->_state);
		return R_INVALID_STATE;
	}
	
	this->_state = PREDICTING;
	this->_syncThreadHandle = (HANDLE) _beginthread(	Predictor<T>::StaticSyncThreadStart,
														0,
														this);

	return R_SUCCESS;
}

template<class T>
int Predictor<T>::StopPredicting()
{
	LOG_DEBUG("called");
	if(this->_state != PREDICTING)
	{
		LOG_ERROR("Invalid state of %d", this->_state);
		return R_INVALID_STATE;
	}

	this->_state = DONE;
	return R_SUCCESS;
}

template<class T>
void Predictor<T>::OnNewData(unsigned int** data)
{
	switch(this->_state)
	{
	case COLLECTING:
		this->SaveData(data);
		break;

	case PREDICTING:
		this->DoPrediction(data);
		break;

	case INIT:
	case COLLECTED:
	case TRAINED:
	case DONE:
	case FAIL:
	default:
		//do nothing
		break;
	}
}

template<class T>
void Predictor<T>::SaveData(unsigned int** data)
{
	//make a copy of the data first
	unsigned int** dataCopy = new unsigned int*[this->_numChannels];
	for(int i=0;i<this->_numChannels;i++)
		dataCopy[i] = new unsigned int[DATA_LENGTH];

	CopyProtocolData(&data, &dataCopy, this->_numChannels);
		
	this->_trainingData.push_back(dataCopy);
		
	this->_currentOutputMutex->lock();
	for(int i=0;i<this->_numOutputs;i++)
		this->_trainingOutputs[i].push_back(this->_currentOutput[i]);
	this->_currentOutputMutex->unlock();
}

template<class T>
void Predictor<T>::DoPrediction(unsigned int** data)
{
	double* tempPredictions = new double[this->_numChannels];
	for(int i=0;i<this->_numOutputs;i++)
	{
		tempPredictions[i] = this->_models[i]->Predict(data);
	}

	this->_currentPredictionMutex->lock();
	memcpy(this->_currentPrediction, tempPredictions, this->_numChannels * sizeof(double));
	this->_currentPredictionMutex->unlock();

	SetEvent(this->_predictionReadyEvent);
}

template<class T>
void Predictor<T>::OnError(unsigned int code)
{
	if(this->_outputListener != NULL)
	{
		this->_outputListener->OnError(code);
	}
}

template<class T>
void Predictor<T>::StaticSyncThreadStart(void* args)
{
	static_cast<Predictor<T>*>(args)->MemberSyncThreadStart();
}

template<class T>
void Predictor<T>::MemberSyncThreadStart()
{
	double* predictions = new double[this->_numOutputs];

	DWORD response;
	while(this->_state == PREDICTING)
	{
		response = WaitForSingleObject(	this->_predictionReadyEvent,
										DATA_TIMEOUT_MS_PREDICTION);
		switch(response)
		{
		case WAIT_OBJECT_0:
			if(!ResetEvent(this->_predictionReadyEvent))
			{
				LOG_ERROR("Failed to reset prediction ready event, code: %d", GetLastError());
			}
			else
			{
				//make a copy of the data that the listener is allowed to take
				memcpy(predictions, this->_currentPrediction, this->_numOutputs * sizeof(double));

				//alert the output listener
				if(this->_outputListener != NULL)
					this->_outputListener->OnNewOutput(predictions);
			}
			break;

		case WAIT_TIMEOUT:
			if(this->_state != PREDICTING)
			{
				//state changed but nothing stopped this, so we didn't REALLY timeout
				break;
			}
			LOG_ERROR("WaitForsingleObject timed out waiting for predictions");
			if(this->_outputListener != NULL)
				this->_outputListener->OnError(BACKEND_ERROR_TIMEOUT);
			break;

		case WAIT_ABANDONED:
		case WAIT_FAILED:
			LOG_ERROR("WaitForSingleObject failed in the prediction synchronization thread, return value: %d", response);
			break;

		default:
			LOG_ERROR("reached default part of switch statement that we shouldn't have, code: %d", response);
			break;

		}
	}

	LOG_DEBUG("Stopping prediction synchronization thread");
}

template<class T>
int Predictor<T>::StopConnection()
{
	this->_state = DONE;
	return this->_dataProtocol->Stop();
}

#endif