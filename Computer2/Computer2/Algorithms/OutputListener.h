#ifndef _OUTPUT_LISTENER
#define _OUTPUT_LISTENER

class OutputListener{
public:
	virtual void OnNewOutput(double* output){};
	virtual void OnError(unsigned int code){};
};

#endif