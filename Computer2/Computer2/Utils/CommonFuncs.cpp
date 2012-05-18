#include "CommonFuncs.h"

void CopyProtocolData(unsigned int*** source, unsigned int*** destination, unsigned int numChannels)
{
	for(unsigned int i=0;i<numChannels;i++)
	{
		memcpy((*destination)[i], (*source)[i], DATA_LENGTH * sizeof(unsigned int));
	}
}
