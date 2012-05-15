#include "DataProtocol.h"

#define CONTROL_BYTE	255
#define DATA_LENGTH		8
#define COM_PORT		L"COM4"
#define START_COMMAND	1<<7

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

	for(unsigned int i=0;i<this->_channels;i++)
	{
		this->_rawData[i] = new unsigned int[DATA_LENGTH];
		this->_cleanData[i] = new unsigned int[DATA_LENGTH];
	}

	this->_channelIndex = 0;
	this->_dataIndex = 0;

	this->_dataListener = dataListener;
	this->_serial = new Serial(COM_PORT, 57600, this);
}

int DataProtocol::Start()
{
	int response;

	this->_receivedStartAck = false;

	response = this->_serial->Open();
	if(response != R_SUCCESS)
	{
		return R_SERIAL_NOT_OPEN;
	}

	response = this->_serial->Write(START_COMMAND | (unsigned char)this->_channels);
	if(response != R_SUCCESS)
	{
		return R_SERIAL_WRITE_FAILED;
	}

	/*Wait for start ack that will be saved from the serial port reading thread
	** Maximum wait time of 2 seconds
	*/

	//should probably use something like WaitForSingleObject() here, but thats not cross-platform so oh well
	clock_t start = clock();
	while(!this->_receivedStartAck && (clock() - start) *1000 / CLOCKS_PER_SEC < 2000)
	{
		Sleep(50);
	}

	if(!this->_receivedStartAck)
		{
			LOG_ERROR("Failed to receive start ack in time");
			return R_START_ACK_TIMEOUT;
		}

		LOG_DEBUG("Start ack delay time: %d ms", (clock() - start)*1000 / CLOCKS_PER_SEC);
		
		if(this->_startAck != (unsigned char)(START_COMMAND | this->_channels))
		{
			LOG_ERROR("Improper start ack.  Received: %d    Expected: %d", this->_startAck, this->_channels | START_COMMAND);
			return R_WRONG_START_ACK;
		}

	return R_SUCCESS;
}

int DataProtocol::Stop()
{
	int response = this->_serial->Write(0);
	if(response != R_SUCCESS)
	{
		LOG_ERROR("Writing 0 to stop serial port communication - response code %d", response);
		return R_UNKNOWN_FAIL;
	}

	response = this->_serial->Close();
	if(response != R_SUCCESS)
	{
		LOG_ERROR("closing serial port - response code %d", response);
		return R_UNKNOWN_FAIL;
	}

	return R_SUCCESS;
}

void DataProtocol::AddByte(unsigned char n)
{
	if(!this->_receivedStartAck)
	{
		this->_startAck = n;
		this->_receivedStartAck = true;
		return;
	}

	LOG_DEBUG("Received byte: %d", (unsigned int)n);



}

