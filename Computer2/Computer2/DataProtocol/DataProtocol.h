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
	DataProtocol(unsigned int channels, DataListener* dataListener);
	int Start();
	int Stop();
	~DataProtocol();
	void AddByte(unsigned char n);
	int GetData(unsigned int** data);

private:
	unsigned int* _gains;
	unsigned int** _rawData;
	unsigned int** _cleanData;
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
};

#endif