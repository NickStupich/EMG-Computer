#include "KeyPresser.h"

KeyPresser::KeyPresser(KeyPressTrainInfo info)
{
	this->state = KP_UNTRAINED;
	this->predictor = new Predictor<BinaryModel>(255, info.numOutputs, this, NULL);
	this->info = info;
	this->currentOutputs = new bool[info.numOutputs];
}

void KeyPresser::Train()
{
	int commandDelayMs = 500;
	int updateDelayMs = 100;
	int i, previousDataCount;
	double* outputs = new double[this->info.numOutputs];

	for(i=0;i<this->info.numOutputs;i++) outputs[i] = 0.0;

	this->predictor->SetOutput(outputs);

	printf("Gathering relaxed data");
	Sleep(commandDelayMs);
	//gather some relaxed data
	this->predictor->StartCollecting();
	while(this->predictor->GetDataLength() < this->info.trainSize)
	{
		Sleep(updateDelayMs);
		printf(".");
	}
	printf("  done\n");
	this->predictor->StopCollecting();
	Sleep(updateDelayMs);	//wait to get an accurate count
	previousDataCount = this->predictor->GetDataLength();

	for(i = 0;i<this->info.numOutputs;i++)
	{
		outputs[i] = 1.0;
		printf("Gathering activated data for key: %c  with description: %s");
		this->predictor->SetOutput(outputs);
		Sleep(commandDelayMs);
		while(this->predictor->GetDataLength() < (this->info.trainSize + previousDataCount))
		{
			Sleep(updateDelayMs);
			printf(".");
		}

		printf("  done\n");
		this->predictor->StopCollecting();
		Sleep(updateDelayMs);	
		previousDataCount = this->predictor->GetDataLength();
		outputs[i] = 0.0;
	}

	printf("Done gathering data, training the classifiers...");
	this->predictor->Train();
	printf("done\n");
}

void KeyPresser::Start()
{
	this->predictor->StartPredicting();
}

void KeyPresser::Stop()
{
	this->predictor->StopPredicting();
}


void KeyPresser::OnNewOutput(double* output)
{
	int i;
	for(i=0;i<this->info.numOutputs;i++)
	{
		this->currentOutputs[i] = output[i] > 0.5 ? true : false;
	}

	this->SignalKeyPresses(this->currentOutputs, this->info.numOutputs);
}


void KeyPresser::OnError(unsigned int code)
{
	printf("ERROR: %d\n", code);
}