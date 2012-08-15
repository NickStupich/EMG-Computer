#ifndef _IDATAPROTOCOL
#define _IDATAPROTOCOL

#include "../Utils/Log.h"
#include "DataListener.h"
#include "Serial.h"
#include "../Utils/NMutex.h"
#include "../Utils/Constants.h"

#define BACKEND_ERROR_TIMEOUT		10

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
	Deconstructor
	*/
	~DataProtocol();
	
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

	/*
	Called by the serial port.  Don't call this unless you're a serial port
	*/
	void AddByte(unsigned char n);

	/* 
	whether everything is connected up and running
	*/
	bool IsRunning();

private:
	unsigned int* _gains;
	unsigned int** _rawData;
	unsigned int** _cleanData;
	unsigned int** _applicationData;
	unsigned int _channels;
	unsigned int _numChannels;

	unsigned int _channelIndex;
	unsigned int _dataIndex;
	bool _isRunning;

	unsigned char _startAck[COMMAND_LENGTH];
	unsigned int _startAckIndex;
	bool _isFirstLoop;
	ExpectedNext _expected;

	Serial* _serial;
	DataListener* _dataListener;

	NMutex* _dataMutex;

	static void StaticThreadStart(void* args);
	void MemberThreadStart();
	HANDLE _syncThreadHandle;
	HANDLE _dataReadyEvent;

	int DoStart();
};

#endif