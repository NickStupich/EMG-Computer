#include <stdlib.h>
#include <stdio.h>

#include "../Computer2/ApplicationHelpers/KeyPresser.h"

int main()
{
	printf("lunar controller\n");
	double MIN_PROB = 0.5;
	KeyPressTrainInfo kpti;

	kpti.trainSize = 100;
	kpti.numOutputs = 3;
	kpti.keys = new KeyDefinition[3];

	kpti.keys[0].key = 0x41;//VK_LEFT;	//Left arrow
	//kpti.keys[0].key = VK_LEFT;	//Left arrow
	kpti.keys[0].keyDescription = "Rotate left";
	kpti.keys[0].minProb = MIN_PROB;

	kpti.keys[1].key = 0x42;//VK_RIGHT;	//Right arrow
	//kpti.keys[1].key = VK_RIGHT;	//Right arrow
	kpti.keys[1].keyDescription = "Rotate right";
	kpti.keys[1].minProb = MIN_PROB;

	kpti.keys[2].key = 0x43;//VK_UP;	//Up arrow
	//kpti.keys[2].key = VK_UP;	//Up arrow
	kpti.keys[2].keyDescription = "Jets";
	kpti.keys[2].minProb = MIN_PROB;

	KeyPresser* kp = new KeyPresser(kpti);
	kp->Train();

	kp->Start();
	//system("lunarLander.exe");
	
	getchar();
	return 0;
}