#ifdef _WIN32

#include "KeyPresser.h"
#include <Windows.h>
#include <WinUser.h>

void KeyPresser::SignalKeyPresses(bool keyStates[])
{
	int i;

	/*
	for(i=0;i<numKeys;i++)
	{ 
		if(keyStates[i] != this->previousStates[i])
		{			
			if(keyStates[i])
			{
				keybd_event(this->info.keys[i].key, 0, KEYEVENTF_EXTENDEDKEY, 0);
			}
			else
			{
				keybd_event(this->info.keys[i].key, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
		}
	}*/
	
	
	for(i=0;i<numKeys;i++)
	{
		if(keyStates[i])
		{
			INPUT input;
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = this->info.keys[i].key;
			input.ki.dwFlags = 0;
			input.ki.time = 0;
			input.ki.dwExtraInfo = 0;

			int x = SendInput(1, &input, sizeof(input));
			//printf("press, channel %d - %d\n", i, x);
			if( x== 0)
			{
				int y = GetLastError();
				printf("last error: %d\n", y);
			}

			input.ki.dwFlags = KEYEVENTF_KEYUP;
			x = SendInput(1, &input, sizeof(input));
			//printf("release, channel %d - %d\n", i, x);
			if( x== 0)
			{
				int y = GetLastError();
				printf("last error: %d\n", y);
			}
		}
	}

	/*
	for(i=0;i<numKeys;i++)
	{
		if(keyStates[i] != this->previousStates[i])
		{
			INPUT input[1];
			memset(input, 0, sizeof(input));
			input[0].type = INPUT_KEYBOARD;
			input[0].ki.wVk = this->info.keys[i].key;
			if(keyStates[i])	//just pressed
				input[0].ki.dwFlags = 0;
			else			//just released
				input[0].ki.dwFlags = KEYEVENTF_KEYUP;
			

			int x = SendInput(numKeys, input, sizeof(INPUT) * numKeys);
			
			printf("sending, channel %d got %d\n", i, x);
		}
	}*/
	/*

	INPUT *input = new INPUT[numKeys];
	memset(input, 0, sizeof(INPUT) * numKeys);
	for(int i=0;i<numKeys;i++)
	{
		input[i].type = INPUT_KEYBOARD;
		input[i].ki.wVk = this->info.keys[i].key;
		input[i].ki.dwFlags = keyStates[i] ? 0 : KEYEVENTF_KEYUP;
	}

	SendInput(numKeys, input, sizeof(INPUT) * numKeys);
	*/
	/*
	for(i=0;i<numKeys;i++)
	{
		printf("%d ", (int) keyStates[i]);
	}
	printf("\n");*/
	
}

#endif