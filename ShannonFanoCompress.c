#include "stdafx.h"
#include <string.h>
#include "ShannonFanoCompress.h"

const unsigned ConstGap = ((SFSTRINGSCALE - 1) * FREQTABLESIZE) & 0xfffffff8;
static char gSFCoding[SFSTRINGSCALE * FREQTABLESIZE];
static unsigned  gCodingLen = 0;
static long long unsigned  gTotalBit = 0;
static FILE* gFp = NULL;

static int   gStatus = eSFStatus_Default;
struct SFMapping* gSFMapping = NULL;

int WriteFile(FILE* fp, char* CodingStr, int codingLen);


int IniSFCompress(char* fileName){
	bool check;
	gSFMapping = GetSFMapping(&check);
	if(check == false || gSFMapping == NULL){
		gStatus = eSFStatus_Default;
		gSFMapping = NULL;
		return -1;
	}
	gFp = fopen(fileName, "wb");
	if(gFp == NULL){
		return -2;
	}

	gStatus = eSFStatus_Finished;
	gCodingLen = 0;
	gTotalBit = 0;
	return 0;
}

int AddSFCompressData(unsigned char* data, int dataLen){
	if(gStatus != eSFStatus_Finished){
		return -1;
	}

	//unsigned char* tmpData = (unsigned char*)data;
	char* tmpStr;
	int rv;
	for(int i = 0; i < dataLen; ++i){
		tmpStr = gSFMapping[data[i]].str;

		rv = strlen(tmpStr);
		memcpy(&gSFCoding[gCodingLen], tmpStr, rv);
		gCodingLen += rv;
		if(gCodingLen > ConstGap){
			rv = WriteFile(gFp, gSFCoding, ConstGap);
			if(rv < 0){
				return -2;
			}
			gTotalBit += rv;
			gCodingLen -= rv;
			memcpy(&gSFCoding[0], &gSFCoding[rv], gCodingLen);
		}
	}
	return 0;
}

long long unsigned SFCompressFinish(bool *check){
	int rv;

	if(gStatus != eSFStatus_Finished){
		*check = false;
		goto EXIT;
	}

	rv = WriteFile(gFp, gSFCoding, gCodingLen);
	if(rv < 0){
		*check = false;
		goto EXIT;
	}
	gCodingLen -= rv;
	gTotalBit += rv;

	fclose(gFp);
	gFp = NULL;
	*check = true;

EXIT:
	gStatus = eSFStatus_Default;
	return gTotalBit;
}

int WriteFile(FILE* fp, char* CodingStr, int codingLen){
	if(gStatus != eSFStatus_Finished){
		return -1;
	}

	unsigned char tmpStr[TMPARRSIZE];
	int offset = 0, totalBit = 0, rv;
	const int fullLen = codingLen &0xfffffff8;
	const int remainLen = codingLen & 0x7;

	for(int i = 0; i < fullLen; i+=8){
		tmpStr[offset] = 0;
		for(int j = 0; j < 8; j++){
			//printf("%c ", CodingStr[i+j]);
			if(CodingStr[i+j] == '1'){
				tmpStr[offset] += (1<<j);
			}
		}
		if(++offset == TMPARRSIZE){
			rv = fwrite(tmpStr, 1, TMPARRSIZE, fp);
			if(rv < 0){
				return -2;
			}
			totalBit += (TMPARRSIZE<<3);
			//printf("%02x ", tmpStr[j]);
			offset = 0;
		}
	}

	rv = fwrite(tmpStr, 1, offset, fp);
	if(rv < 0){
		return -2;
	}
	totalBit += (offset<<3);


	if(remainLen){
		tmpStr[0] = 0;
		for(int i = 0; i < remainLen; ++i){
			if(CodingStr[fullLen+i] == '1'){
				tmpStr[0] += (1<<i);
			}
		}
		rv = fwrite(&tmpStr[0], 1, 1 , fp);
		if(rv < 0){
			return -2;
		}
	}

	return codingLen;
}

int WriteRawData(unsigned char* data, int dataLen){
	if(gFp == NULL || gStatus != eSFStatus_Finished){
		return -1;
	}
	return fwrite(data, 1, dataLen, gFp);
}
