#ifndef _IDATAPROTOCOL
#define _IDATAPROTOCOL

#include "../Utils/Log.h"
#include "DataListener.h"
#include "Serial.h"

class DataProtocol{
public:
	DataProtocol(unsigned int channels, DataListener* dataListener);
	int start();
	int stop();
	~DataProtocol();
	void AddByte(unsigned char n);
	unsigned int** GetData();

private:
	unsigned int* _gains;
	unsigned int** _rawData;
	unsigned int** _cleanData;
	unsigned int _channels;
	unsigned int _numChannels;

	Serial* _serial;
	DataListener* _dataListener;
};

#endif