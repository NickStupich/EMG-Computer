#include "DataProtocol.h"
#include "SerialPortLocation.h"
#include "../Utils/Constants.h"
#include "../Utils/CommonFuncs.h"

//threading
#ifdef WIN32
#include <process.h>
#endif

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
	
	this->_dataMutex = new NMutex();
	this->_serial = NULL;

	this->_dataReadyEvent = CreateEvent(	NULL, 
											true, 
											false, 
											NULL);

	this->_isRunning = false;
}

int DataProtocol::Start()
{
	int response;
	for(int retry = 0; retry < NUM_RETRIES_START;retry++)
	{
		response = this->DoStart();
		if(response == R_SUCCESS)
		{
			this->_isRunning = true;
			return R_SUCCESS;
		}
		LOG_ERROR("Start attempt %d / %d failed with error code %d", retry, NUM_RETRIES_START, response);
		this->Stop();
	}

	//if we failed 3 times return the last error code
	return response;
}

int DataProtocol::DoStart()
{
	int response;
	int i;
	unsigned char bytesToSend[COMMAND_LENGTH] = {0, 0, 0, 0};

	wchar_t* portAddr;
	
	this->_startAckIndex = 0;
	this->_isFirstLoop = true;

	response = GetPortLocation(&portAddr);
	if(response != R_SUCCESS)
	{
		LOG_ERROR("Get port location failed with code: %d", response);
		return R_PORT_NOT_KNOWN;
	}

	this->_serial = new Serial(portAddr, 115200, this);

	this->_expected = GAIN;

	response = this->_serial->Open();
	if(response != R_SUCCESS)
	{
		return R_SERIAL_NOT_OPEN;
	}

	bytesToSend[COMMAND_INDEX] = START_FFT_COMMAND;
	bytesToSend[CHANNELS_INDEX] = (unsigned char)this->_channels;

	response = this->_serial->Write(bytesToSend, COMMAND_LENGTH);

	if(response != R_SUCCESS)
	{
		this->Stop();
		return R_SERIAL_WRITE_FAILED;
	}
	
	clock_t start = clock();
	while(this->_startAckIndex < COMMAND_LENGTH && (clock() - start) *1000 / CLOCKS_PER_SEC < START_ACK_TIMEOUT_MS)
	{
		Sleep(50);
	}

	if(this->_startAckIndex < COMMAND_LENGTH)
	{
		LOG_ERROR("Failed to receive start ack in time");
		this->Stop();
		return R_START_ACK_TIMEOUT;
	}
	
	LOG_DEBUG("Start ack delay time: %d ms", (clock() - start)*1000 / CLOCKS_PER_SEC);
		
	if(memcmp(this->_startAck, bytesToSend, sizeof(unsigned char) * COMMAND_LENGTH))
	{
		LOG_ERROR("Improper start ack.  Received: %d %d %d %d    Expected: %d %d %d %d", 
			this->_startAck[0], this->_startAck[1], this->_startAck[2], this->_startAck[3],
			bytesToSend[0], bytesToSend[1], bytesToSend[2], bytesToSend[3]);
		
		this->Stop();
		return R_WRONG_START_ACK;
	}

	//start the thread that actually interfaces with an application / algorithm
	this->_syncThreadHandle = (HANDLE) _beginthread(	DataProtocol::StaticThreadStart, 
														0, 
														this);

	return R_SUCCESS;
}

int DataProtocol::Stop()
{
	this->_isRunning = false;
	if(this->_serial != NULL && this->_serial->isOpen())
	{
		unsigned char stopCmd[COMMAND_LENGTH] = {STOP_COMMAND, 0, 0, 0};
		int response = this->_serial->Write(stopCmd, COMMAND_LENGTH);
		if(response != R_SUCCESS)
		{
			LOG_ERROR("Writing 0 to stop serial port communication - response code %d", response);
			return false;
		}
		
		response = this->_serial->Close();
		if(response != R_SUCCESS)
		{
			LOG_ERROR("closing serial port - response code %d", response);
			return R_UNKNOWN_FAIL;
		}


	}

	return R_SUCCESS;
}

int DataProtocol::GetData(unsigned int*** data)
{
	if(this->_expected == FIRST)
	{
		return R_NO_DATA_AVAILABLE;
	}
	this->_dataMutex->lock();

	for(unsigned int i=0;i<this->_numChannels;i++)
	{
		memcpy(this->_cleanData[i], (*data)[i], sizeof(int) * DATA_LENGTH);
	}

	this->_dataMutex->unlock();

	return R_SUCCESS;
}

void DataProtocol::AddByte(unsigned char n)
{
	if(this->_startAckIndex < COMMAND_LENGTH)
	{
		this->_startAck[this->_startAckIndex++] = n;
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
				//LOG_DEBUG("Got complete data chunk");

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
				
				//signal the synchronization thread that there is data ready
				SetEvent(this->_dataReadyEvent);
				//LOG_DEBUG("Set data ready event");
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

void DataProtocol::StaticThreadStart(void* args)
{
	static_cast<DataProtocol*>(args)->MemberThreadStart();
}

void DataProtocol::MemberThreadStart()
{
	DWORD response;
	unsigned int** externalData = new unsigned int*[this->_numChannels];
	for(unsigned int i=0;i<this->_numChannels;i++)
	{
		externalData[i] = new unsigned int[DATA_LENGTH];
	}

	LOG_DEBUG("Started data protocol synchronization thread");

	while(this->IsRunning())
	{
		//LOG_DEBUG("Before waitforsingleobject");

		if(this->_isFirstLoop)
		{
			response = WaitForSingleObject(	this->_dataReadyEvent,
											DATA_TIMEOUT_MS_FIRST_LOOP);
			this->_isFirstLoop = false;
		}
		else
		{
			response = WaitForSingleObject(	this->_dataReadyEvent,
											DATA_TIMEOUT_MS);
		}


		//LOG_DEBUG("waitforsingleobject returned %d", response);
		switch(response)
		{
		case WAIT_OBJECT_0:
			response = ResetEvent(this->_dataReadyEvent);
			if(response == 0)
			{
				LOG_ERROR("failed to reset data ready event, last error: %d", GetLastError());
			}
			else
			{
				if(this->_dataListener != NULL)
				{
					this->_dataMutex->lock();
					CopyProtocolData(&this->_cleanData, &externalData, this->_numChannels);
					this->_dataMutex->unlock();

					this->_dataListener->OnNewData(externalData);
				}
			}
			break;

		case WAIT_TIMEOUT:

			//in case the connection stops once we're already inside the loop
			if(this->_serial == NULL || !this->_serial->isOpen())
				break;

			LOG_ERROR("WaitforSingleObject timed out on the data sync thread");
			if(this->_dataListener != NULL)
			{
				this->_dataListener->OnError(BACKEND_ERROR_TIMEOUT);
			}
			break;

		case WAIT_ABANDONED:
		case WAIT_FAILED:
			LOG_ERROR("WaitForSingleObject failed in the data synchronization thread, return value: %d", response);
			break;
		default:
			LOG_ERROR("reached default part of switch statement that we shouldn't have, code: %d", response);
		}
	}

	LOG_DEBUG("Data protocol synchronization thread is stopping");
}

bool DataProtocol::IsRunning()
{
	return this->_isRunning;
}