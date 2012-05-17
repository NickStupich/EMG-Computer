#ifndef _DATALISTENER
#define _DATALISTENER

class DataListener{
public:

	/*
	This will be called every time new data is received, UNLESS the previous call to OnNewData()
	has not yet returned.  This allows the backend to throttle updates if listener can't 
	keep up, so the listener doesn't have to worry about backing things up

	data is a 2-d array, as data[channelNumber][frequencyBin].  Channels collapse into the lowest numbers
	(Ex: running channels 0 and 2 (with start number 0000 0101 = 5), channel 0 data = data[0], channel 2 data = data[1]

	data is guaranteed to not be changed before OnNewData() returns, but will be changed after so the application 
	should make a copy of the data if it needs to
	*/
	virtual void OnNewData(unsigned int** data){};

	/* 
	Error codes are found in DataProtocol.h
	*/
	virtual void OnError(unsigned int code){};
};

#endif