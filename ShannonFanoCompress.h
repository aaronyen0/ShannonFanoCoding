#pragma once
#include "ShannonFanoTree.h"


int IniSFCompress(char* fileName);
int AddSFCompressData(unsigned char* data, int dataLen);
long long unsigned SFCompressFinish(bool *check);
int WriteRawData(unsigned char* data, int dataLen);
