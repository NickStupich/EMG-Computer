#ifndef _IDATAPROTOCOL
#define _IDATAPROTOCOL

#include "../Utils/Log.h"
#include "DataListener.h"
#include "Serial.h"
#include "../Utils/Mutex.h"

enum ExpectedNext{
	CONTROL,
	GAIN,
	DATA,
	FAILURE,
	FIRST
};

//avoid a circular dependency compilation problem
class Serial;

class DataProtocol{
public:

	/* External functions to be called */


	/*
	Initializes data protocol for the provided channels
	Each bit in channels indicates whether the channel should be on.
	Ex: channels = 3 = 0000 0011b
	-> Channels 0 and 1 are on

	dataListener can be provided to receive updates with new data, or null and
	GetData() can be polled for new data
	*/
	DataProtocol(unsigned int channels, DataListener* dataListener);

	/*
	Starts the bluetooth connection, sends appropriate start command,
	threads and whatnot to read serial data
	*/
	int Start();

	/*
	Sends stop byte, stops threads, closes bluetooth connection
	*/
	int Stop();

	/*
	makes a copy of the current data to the provided address
	enough memory must already be allocated for the copy
	*/
	int GetData(unsigned int*** data);

	~DataProtocol();
	void AddByte(unsigned char n);

private:
	unsigned int* _gains;
	unsigned int** _rawData;
	unsigned int** _cleanData;
	unsigned int** _applicationData;
	unsigned int _channels;
	unsigned int _numChannels;

	unsigned int _channelIndex;
	unsigned int _dataIndex;


	unsigned char _startAck;
	bool _receivedStartAck;
	ExpectedNext _expected;

	Serial* _serial;
	DataListener* _dataListener;

	BMutex* _dataMutex;

	static void StaticThreadStart(void* args);
	void MemberThreadStart();
	HANDLE _syncThreadHandle;
};

#endif