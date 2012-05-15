#ifdef _WIN32

#include "Serial.h"
#include <process.h>

#define READ_BUFFER_SIZE	10

Serial::Serial(wchar_t* port, int baudRate, DataProtocol* protocol)
{
	this->_port = port;
	this->_protocol = protocol;
	switch(baudRate){
		case 57600: this->_baud = CBR_57600; break;
		default: this->_baud = NULL; break;
	}
	this->_isOpen = false;
}

int Serial::open()
{
	if(this->_isOpen)
		return R_ALREADY_OPEN;

	if(this->_port == NULL || this->_baud == NULL)
	{
		LOG_DEBUG("port or baud rate is null");
		return R_UNKNOWN_FAIL;
	}

	this->_handle = CreateFile(	this->_port,
								GENERIC_WRITE | GENERIC_READ,
								0,
								0,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								0);	

	if(this->_handle == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		if(err == ERROR_FILE_NOT_FOUND)
		{
			LOG_ERROR("port not found");
			return R_PORT_DOES_NOT_EXIST;
		}
		else if(err == ERROR_ACCESS_DENIED)
		{
			LOG_ERROR("access denied - port already open?");
			return R_ACCESS_DENIED;
		}

		LOG_LAST_WIN_ERROR(err, "CreateFile() - error message follows:");

		R_UNKNOWN_FAIL;
	}

	DCB dcbParams = {0};
	dcbParams.DCBlength = sizeof(dcbParams);
	if(!GetCommState(this->_handle, &dcbParams))
	{
		LOG_ERROR("Error getting state of serial port handle");
		goto error;
	}

	dcbParams.BaudRate = this->_baud;
	dcbParams.ByteSize = 8;
	dcbParams.StopBits = ONESTOPBIT;
	dcbParams.Parity = NOPARITY;

	if(!SetCommState(this->_handle, &dcbParams)){
		LOG_ERROR("Error setting state of serial port handle");
		goto error;
	}

	COMMTIMEOUTS timeouts = {0};
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if(!SetCommTimeouts(this->_handle, &timeouts))
	{
		LOG_ERROR("Error setting timeouts of serial port handle");
		goto error;
	}

	this->_isOpen = true;

	//start up the read thread
	this->_readThreadHandle = (HANDLE) _beginthread(Serial::StaticThreadStart, 0, this);

	return R_SUCCESS;

error:
	LOG_DEBUG("Closing handle due to error opening");
	this->close();

	return R_UNKNOWN_FAIL;
}

int Serial::close()
{
	if(this->_isOpen)
	{
		this->_isOpen = false;
		//read thread will automatically stop once _isOpen is false
		CloseHandle(this->_handle);
	}

	return R_SUCCESS;
}

int Serial::write(char* buf, int len)
{
	if(!this->_isOpen)
	{
		return R_HANDLE_NOT_OPEN;
	}

	DWORD bytesRead;
	if(!WriteFile(this->_handle, (LPCVOID) buf, len, &bytesRead, NULL))
	{
		LOG_LAST_WIN_ERROR(GetLastError(), "writing to file:");
	}

	return R_SUCCESS;
}

Serial::~Serial()
{
	this->close();
}

void Serial::MemberThreadStart(void* args)
{
	LOG_DEBUG("Started serial port read thread");

	while(this->_isOpen)
	{
		DWORD bytesRead;
		unsigned char buf[READ_BUFFER_SIZE];
		if(!ReadFile(this->_handle, buf, READ_BUFFER_SIZE, &bytesRead, NULL))
		{
			//LOG_LAST_WIN_ERROR(GetLastError(), "reading from file:");
			int err = GetLastError();
			if(err != ERROR_INVALID_HANDLE)
			{
				LOG_ERROR("Non-invalid handle error");
			}
		}
		else
		{
			//successful read
			for(unsigned int i=0;i<bytesRead;i++)
			{
				if(this->_protocol != NULL)
					this->_protocol->AddByte(buf[i]);
			}
		}
	}
}

void Serial::StaticThreadStart(void* args)
{
	static_cast<Serial*>(args)->MemberThreadStart(NULL);
}

#endif