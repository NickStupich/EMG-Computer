#ifndef _IDATAPROTOCOL
#define _IDATAPROTOCOL

#include "../Utils/Log.h"
#include "DataListener.h"
#include "Serial.h"

//avoid a circular dependency compilation problem
class Serial;

class DataProtocol{
public:
	DataProtocol(unsigned int channels, DataListener* dataListener);
	int Start();
	int Stop();
	~DataProtocol();
	void AddByte(unsigned char n);
	unsigned int** GetData();

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

	Serial* _serial;
	DataListener* _dataListener;


};

#endif