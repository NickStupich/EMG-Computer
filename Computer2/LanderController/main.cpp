#include <stdlib.h>
#include <stdio.h>

#include "../Computer2/ApplicationHelpers/KeyPresser.h"

int main()
{
	printf("lunar controller\n");
	double MIN_PROB = 0.6;
	KeyPressTrainInfo kpti;

	kpti.trainSize = 100;
	kpti.numOutputs = 4;
	kpti.keys = new KeyDefinition[3];

	kpti.keys[0].key = 0x41;
	kpti.keys[0].key = VK_LEFT;	//Left arrow
	kpti.keys[0].keyDescription = "Rotate left";
	kpti.keys[0].minProb = MIN_PROB;

	kpti.keys[1].key = 0x42;
	kpti.keys[1].key = VK_RIGHT;	//Right arrow
	kpti.keys[1].keyDescription = "Rotate right";
	kpti.keys[1].minProb = MIN_PROB;

	kpti.keys[2].key = 0x43;
	kpti.keys[2].key = VK_UP;	//Up arrow
	kpti.keys[2].keyDescription = "Jets";
	kpti.keys[2].minProb = MIN_PROB;

	kpti.keys[3].key = 0x44;
	kpti.keys[3].key = 'X';	//Up arrow
	kpti.keys[3].keyDescription = "Super Jets";
	kpti.keys[3].minProb = MIN_PROB;

	KeyPresser* kp = new KeyPresser(kpti);
	kp->Train();

	kp->Start();
	int x;
	do
	{
		//system("lunarLander.exe");
		printf("Type something other than just enter to quit\n");
		x = getchar();
	}
	while(x==10);

	return 0;
}