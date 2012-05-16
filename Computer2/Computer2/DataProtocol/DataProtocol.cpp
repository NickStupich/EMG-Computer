#include "DataProtocol.h"
#include "SerialPortLocation.h"

#define CONTROL_BYTE	255
#define DATA_LENGTH		8
#define COM_PORT		L"COM4"
#define START_COMMAND	1<<7

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
	
	this->_dataMutex = new BMutex();
}

int DataProtocol::Start()
{
	int response;

	wchar_t* portAddr;
	response = GetPortLocation(&portAddr);
	if(response != R_SUCCESS)
	{
		LOG_ERROR("Get port location failed with code: %d", response);
		return R_PORT_NOT_KNOWN;
	}

	this->_serial = new Serial(portAddr, 57600, this);

	this->_receivedStartAck = false;
	this->_expected = FIRST;

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

int DataProtocol::GetData(unsigned int** data)
{
	this->_dataMutex->lock();

	for(unsigned int i=0;i<this->_numChannels;i++)
	{
		memcpy(this->_cleanData[i], data[i], sizeof(int) * DATA_LENGTH);
	}

	this->_dataMutex->unlock();

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

	//LOG_DEBUG("Received byte: %d", (unsigned int)n);

	switch(this->_expected)
	{

	case FIRST:
		if(n != CONTROL_BYTE)
		{
			LOG_ERROR("did not receive control byte first, instead got %d", n);
		}
		else
		{
			this->_expected = GAIN;
		}
		break;

	case FAILURE:
		if(n == CONTROL_BYTE)
		{
			LOG_DEBUG("Received control byte in failure state");
			this->_channelIndex = 0;
			this->_dataIndex = 0;
			this->_expected = GAIN;
		}
		break;

	case CONTROL:
		if(n == CONTROL_BYTE)
		{
			if(this->_dataListener != NULL)
			{
				LOG_DEBUG("Got complete data chunk");

				this->_dataMutex->lock();

				//copy raw data to clean data
				for(unsigned int i=0;i<this->_numChannels;i++)
				{
					for(int j=0;j<DATA_LENGTH;j++)
					{
						this->_cleanData[i][j] = this->_gains[i] * this->_rawData[i][j];
					}
				}

				this->_dataMutex->unlock();
				
				
				//TODO:  figure out the interfacing with data listener here

			}

			this->_channelIndex = 0;
			this->_dataIndex = 0;
			this->_expected = GAIN;
		}
		else
		{
			LOG_ERROR("Expected control byte, instead received %d", n);
			this->_expected = FAILURE;
		}
		break;

	case GAIN:
		if(n == CONTROL_BYTE)
		{
			LOG_ERROR("Expected gain, received control byte");
			this->_expected = FAILURE;
		}
		else
		{
			this->_gains[this->_channelIndex] = n;
			this->_expected = DATA;
		}
		break;

	case DATA:
		if(n == CONTROL_BYTE)
		{
			LOG_ERROR("Expected data, received control byte");
			this->_expected = FAILURE;
		}
		else
		{
			this->_rawData[this->_channelIndex][this->_dataIndex] = n;
			this->_dataIndex++;

			if(this->_dataIndex == DATA_LENGTH)
			{
				this->_dataIndex = 0;
				this->_channelIndex ++;

				if(this->_channelIndex == this->_numChannels)
				{
					this->_channelIndex = 0;
					this->_expected = CONTROL;
				}
				else
				{
					this->_expected = GAIN;
				}
			}
		}

		break;
	}

}

