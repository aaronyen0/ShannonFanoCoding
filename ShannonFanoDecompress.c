#include "stdafx.h"
#include "ShannonFanoDecompress.h"

const int cBufferSize = 0x100;
const int cBufferGap  = 0x80; 

static int                gStatus = eSFStatus_Default;
static struct SFTreeNode* gRoot = NULL;
static struct SFTreeNode* gCurrentNode = NULL;
static long long unsigned gTotalBit = 0;

static FILE* gFp = NULL;
static unsigned char gBuffer[cBufferSize];
static int  gBufferLen = 0;

int IniDecompress(char* fileName, long long unsigned totalBit){
	bool check;
	gRoot = GetSFTreeRoot(&check);
	if(check == false || gRoot == NULL){
		gStatus = eSFStatus_Default;
		gRoot = NULL;
		return -1;
	}
	gFp = fopen(fileName, "wb");
	if(gFp == NULL){
		return -2;
	}

	gStatus = eSFStatus_Finished;
	gCurrentNode = gRoot;
	gTotalBit = totalBit;
	gBufferLen = 0;
	return 0;
}


int AddSFDecompressData(unsigned char* data, int dataLen){
	if(gStatus != eSFStatus_Finished){
		return -1;
	}
	//unsigned char *uData = (unsigned char*)data, oneBit;
	unsigned char  oneBit;
	int rv;

	for(int i = 0; i < dataLen; ++i){
		oneBit = 0x1;
		for(int j = 0; j < 8 && gTotalBit > 0; ++j, --gTotalBit){
			if(data[i] & oneBit){
				gCurrentNode = gCurrentNode->right;
			}else{
				gCurrentNode = gCurrentNode->left;
			}
			oneBit <<= 1;

			if(gCurrentNode->flag == 1){
				//printf("%3u ", gCurrentNode->num);
				gBuffer[gBufferLen++] = gCurrentNode->num;
				gCurrentNode = gRoot;
			}
		}
		if(gBufferLen > cBufferGap){
			rv = fwrite(gBuffer, 1, cBufferGap, gFp);
			memcpy(&gBuffer[0], &gBuffer[rv], gBufferLen - rv);
			gBufferLen -= rv;
		}
	}
	if(gBufferLen > 0){
		rv = fwrite(gBuffer, 1, gBufferLen, gFp);
		gBufferLen = 0;
	}
	return 0;
}

int SFDecompressFinish(){
	if(gStatus != eSFStatus_Finished){
		gStatus = eSFStatus_Default;
		return -1;
	}
	fclose(gFp);

	gRoot = NULL;
	gCurrentNode = NULL;
	gTotalBit = 0;
	gFp = NULL;
	gBufferLen = 0;
	gStatus = eSFStatus_Default;
	return 0;
}


unsigned int Decompress_Test(int num, bool* check){
	unsigned int output;
	if(gStatus != eSFStatus_Finished){
		*check = false;
		return 0xffffffff;
	}

	if(num == 0){
		gCurrentNode = gCurrentNode->left;
	}else{
		gCurrentNode = gCurrentNode->right;
	}
	if(gCurrentNode->flag == 1){
		*check = true;
		output = gCurrentNode->num;
		gCurrentNode = gRoot;
		return output;
	}
	*check = false;
	return 0;
}
