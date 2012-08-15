#ifndef _COMMON_FUNCS
#define _COMMON_FUNCS
#include "Constants.h"
#include <string.h>	//for memcpy

#define ONES_IN_BYTE(x)	((x&(1<<7))>>7) + ((x&(1<<6))>>6) + ((x&(1<<5))>>5) + ((x&(1<<4))>>4) + ((x&(1<<3))>>3) + ((x&(1<<2))>>2) + ((x&(1<<1))>>1) + (x&1)

void CopyProtocolData(unsigned int*** source, unsigned int*** destination, unsigned int numChannels);

#endif