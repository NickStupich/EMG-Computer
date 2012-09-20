#include "../Computer2/ApplicationHelpers/KeyPresser.h"

void testKeyPresser()
{
	KeyPressTrainInfo kpti;

	kpti.trainSize = 100;
	kpti.numOutputs = 2;
	kpti.keys = new KeyDefinition[4];

	kpti.keys[0].key = 0x41;	//A
	kpti.keys[0].keyDescription = "letter A";

	kpti.keys[1].key = 0x42;	//B
	kpti.keys[1].keyDescription = "letter B";
	
	kpti.keys[2].key = 0x43;	//C
	kpti.keys[2].keyDescription = "letter C";

	kpti.keys[3].key = 0x44;	//D
	kpti.keys[3].keyDescription = "letter D";

	KeyPresser *kp = new KeyPresser(kpti);
	kp->Train();
	kp->Start();
	/*
	bool testStates[2] = {0, 1};
	kp->SignalKeyPresses(testStates);
	Sleep(1000);
	testStates[1] = 0;
	kp->SignalKeyPresses(testStates);
	*/
	printf("hit enter to finish");
	getchar();
	kp->Stop();
}