
#include <Windows.h>
#include <WinUser.h>
#include "../Algorithms/Predictor.h"
#include "../Algorithms/BinaryModel.h"
#include "../Algorithms/OutputListener.h"

/*
Helper class, will get the user to train the system, and fake key presses whenever certains actions are recognized
*/

typedef struct{
	WORD key;
	char* keyDescription;
	double probability;
} KeyDefinition;

typedef struct {
	int trainSize;
	int numOutputs;
	KeyDefinition* keys;
} KeyPressTrainInfo;

enum STATE{
	KP_UNTRAINED,
	KP_TRAINED,
	KP_RUNNING,
};

class KeyPresser : OutputListener{
public:
	KeyPresser(KeyPressTrainInfo info);
	void Train();
	void Start();
	void Stop();

private:
	Predictor<BinaryModel> *predictor;
	STATE state;
	KeyPressTrainInfo info;
	bool* currentOutputs;
	void SignalKeyPresses(bool keyStates[], int numKeys);
	void OnNewOutput(double* output);
	void OnError(unsigned int code);
};