#ifndef _DATALISTENER
#define _DATALISTENER

class DataListener{
	DataListener();

	int Start(unsigned int channels);
	void OnNewData(unsigned int** data);

};

#endif