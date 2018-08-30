#pragma once
#include "ShannonFanoTree.h"

//#define DCBUFFERSIZE (SFSTRINGSCALE * 8)

int IniDecompress(char* fileName, long long unsigned totalBit);
int AddSFDecompressData(unsigned char* data, int dataLen);
int SFDecompressFinish();
