#ifdef _WIN32

#include "KeyPresser.h"
#include <Windows.h>
#include <WinUser.h>

void KeyPresser::SignalKeyPresses(bool keyStates[])
{
	int i;

	
	for(i=0;i<numKeys;i++)
	{ 
		if(keyStates[i] != this->previousStates[i])
		{			
			INPUT input;
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = this->info.keys[i].key;
			input.ki.time = 0;
			input.ki.dwExtraInfo = 0;

			int x;
			if(keyStates[i])
			{
				input.ki.dwFlags = 0;
				x = SendInput(1, &input, sizeof(input));
			}
			else
			{
				input.ki.dwFlags = KEYEVENTF_KEYUP;
				x = SendInput(1, &input, sizeof(input));
			}
		}
	}
	
	/*
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
	}*/
}

#endif