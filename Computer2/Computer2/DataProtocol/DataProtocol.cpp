#include "DataProtocol.h"

#define CONTROL_BYTE	255
#define DATA_LENGTH		8

enum ExpectedNext{
	CONTROL,
	GAIN,
	DATA,
	FAILURE
};

#define ONES_IN_BYTE(x)	(x&(1<<7)) + (x&(1<<6)) + (x&(1<<5)) + (x&(1<<4)) + (x&(1<<3)) + (x&(1<<2)) + (x&(1<<1)) + (x&1)

DataProtocol::DataProtocol(unsigned int channels, DataListener* dataListener)
{
	this->_channels = channels;
	this->_numChannels = ONES_IN_BYTE(channels);

	this->_gains = new unsigned int[this->_channels];
	this->_rawData = new unsigned int*[this->_channels];
	this->_cleanData = new unsigned int*[this->_channels];
}

void DataProtocol::AddByte(unsigned char n)
{
	LOG_DEBUG("Received byte");
}

