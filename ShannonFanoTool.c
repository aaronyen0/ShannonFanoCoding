// ShannonFanoTool.cpp : 定義 DLL 應用程式的匯出函式。
//

#include "stdafx.h"
#include "DebugLog.h"
#include <string.h>
#include <Windows.h>
#include "FreqCount.h"
#include "ShannonFanoTree.h"
#include "ShannonFanoCompress.h"
#include "ShannonFanoDecompress.h"
#include "ShannonFanoTool.h"

#define MAINVER 2
#define SUBVER1 0
#define SUBVER2 1

#define VERSION "aaron, V1.0.2 " __DATE__

#pragma pack(push) //把對齊設定push進stack
#pragma pack(1)   //把對齊設定設成1
struct SFC_AaronHead{
	char version[32];
	long long unsigned totalBit;
	long long unsigned counter[FREQTABLESIZE];
	unsigned int reserve[16];
};
#pragma pack(pop)  //把對齊設定pop出來

const int BUFFERSIZE = 1024;
struct SFC_AaronHead gHeader;
static char* gSrcFile = NULL;
static char* gDstFile = NULL;

static volatile HANDLE gThread = NULL;
static int gErrorCode = 0; //0:default, 1:finish, others:error,  

static double gInvFileLen = 1.0f;
static double gCurrent = 0.0f;
//void (*GetProgress)(float percent, int errorCode) = NULL;
static float gProgress = 0.0f;


//function declare
void IniHeader();
DWORD WINAPI SFCompressMain(void* vPtr);
DWORD WINAPI SFDecompressMain(void* vPtr);
void SendProgress(float progress);
unsigned int GetFileLen(FILE* fp);


void __stdcall SFVersion(char* buffer, int size){
	if(size < strlen(VERSION)){
		return;
	}
	sprintf(buffer, VERSION);
}

DLLEXPORT float __stdcall GetProgress(int* check){
	*check = gErrorCode;
	return gProgress;
}

DLLEXPORT int __stdcall SFCompress(char* srcFile, char* dstFile){
	if(gThread || srcFile == NULL || dstFile == NULL){
		return -1;
	}
	gErrorCode = 0;
	gInvFileLen = 0.0f;
	gCurrent = 0.0f;

	gSrcFile = srcFile;
	gDstFile = dstFile;

	gThread = CreateThread(NULL, 0, SFCompressMain, NULL, 0, NULL);
	
	return 0;
}

DLLEXPORT int __stdcall SFDecompress(char* srcFile, char* dstFile){
	if(gThread || srcFile == NULL || dstFile == NULL){
		return -1;
	}
	gErrorCode = 0;
	gInvFileLen = 0.0f;
	gCurrent = 0.0f;

	gSrcFile = srcFile;
	gDstFile = dstFile;

	gThread = CreateThread(NULL, 0, SFDecompressMain, NULL, 0, NULL);
	return 0;
}

//DLLEXPORT void __stdcall SetProgressFun(void (*ptr)(float, int)){
//	GetProgress = ptr;
//}

void IniHeader(){
	char* ptr = (char*)&gHeader;
	for(int i = 0; i < sizeof(struct SFC_AaronHead); ++i){
		*ptr++ = 0;
	}
}

DWORD WINAPI SFCompressMain(void* vPtr){
	int rv;
	unsigned char buffer[BUFFERSIZE];
	long long unsigned *counter, totalBit;
	struct FreqTable* freqTable;
	bool check;

	FILE* fp = fopen(gSrcFile, "rb");
	if(fp == NULL){
		gErrorCode = -1;
		LOG("srcFp = NULL");
		goto EXIT;
	}
	gInvFileLen = 1.0f/GetFileLen(fp);

	//1.統計每個byte的數量及排序FreqTable
	IniHeader();
	InitialFreqTable();
	sprintf(gHeader.version, "%s", VERSION);
	while(1){
		rv = fread(buffer, 1, BUFFERSIZE, fp);
		if(rv <= 0){
			break;
		}
		gCurrent = _ftelli64(fp);
		SendProgress(0.1f * gCurrent * gInvFileLen);
		for(int i = 0; i < rv; ++i){
			AddNum(buffer[i]);
		}
	}

	counter = GetCounter();
	for(int i = 0; i < FREQTABLESIZE; ++i){
		gHeader.counter[i] = counter[i];
	}
	freqTable = GetFreqTable();

	//2.Build SFTree
	IniSFTree();
	totalBit = GenSFTree(freqTable);
	gHeader.totalBit = totalBit;

	//3.Read the File and compress
	rv = IniSFCompress(gDstFile);
	if(sizeof(struct SFC_AaronHead) != WriteRawData((unsigned char*)&gHeader, sizeof(struct SFC_AaronHead))){
		gErrorCode = -2;
		LOG("Header Size Error");
		goto EXIT;
	}
	if(rv < 0){
		LOG("IniSFCompress Error");
		gErrorCode = -3;
		goto EXIT;
	}
	fseek(fp, 0L,SEEK_SET);
	while(1){
		rv = fread(buffer, 1, BUFFERSIZE, fp);
		if(rv <= 0){
			break;
		}
		//currentLen = (unsigned int)ftell(fp);
		gCurrent = _ftelli64(fp);
		SendProgress(0.1f + 0.9f * gCurrent * gInvFileLen);
		rv = AddSFCompressData(buffer, rv);
		//if(rv < 0){
		//	goto EXIT;
		//}
	}

	if(SFCompressFinish(&check) != totalBit){
		LOG("TotalBit Error");
		gErrorCode = -4;
		goto EXIT;
	}
	if(check == false){
		LOG("Check Finish Fail");
		gErrorCode = -5;
		goto EXIT;
	}
	gErrorCode = 1;
EXIT:

	SFCompressFinish(&check);
	DisposeSFTree();
	if(fp){
		fclose(fp);
	}
	SendProgress(gInvFileLen * gCurrent);
	gThread = NULL;
	return 0;
}


DWORD WINAPI SFDecompressMain(void* vPtr){
	int rv;
	//unsigned long currentLen;
	unsigned char buffer[BUFFERSIZE];
	long long unsigned totalBit;
	struct FreqTable* freqTable;

	FILE* fp = fopen(gSrcFile, "rb");
	if(fp == NULL){
		LOG("srcFp = NULL");
		gErrorCode = -1;
		goto EXIT;
	}
	gInvFileLen = 1.0f/GetFileLen(fp);

	fseek(fp, 0L,SEEK_END); 
	if(ftell(fp) < sizeof(SFC_AaronHead)){
		LOG("FILE Length Error");
		gErrorCode = -2;
		goto EXIT;
	}
	fseek(fp, 0L,SEEK_SET);

	//讀檔並確認header資料
	IniHeader();
	InitialFreqTable();
	if(sizeof(gHeader) != fread((unsigned char*)&gHeader, 1 , sizeof(gHeader), fp)){
		LOG("Header Length Error");
		gErrorCode = -3;
		goto EXIT;
	}
	if(strcmp(gHeader.version, VERSION)){
		LOG("Header Info Error");
		gErrorCode = -4;
		goto EXIT;
	}
	for(int i = 0; i < FREQTABLESIZE; ++i){
		if(LoadCounter((unsigned int)i, gHeader.counter[i]) < 0){
			LOG("Write Counter Error");
			gErrorCode = -5;
			goto EXIT;
		}
	}
	freqTable = GetFreqTable();

	//2.Build SFTree
	IniSFTree();
	totalBit = GenSFTree(freqTable);

	//3.Decompress
	rv = IniDecompress(gDstFile, totalBit);
	if(rv < 0){
		LOG("IniDecompress Error");
		gErrorCode = -6;
		goto EXIT;
	}
	while(1){
		rv = fread(buffer, 1, BUFFERSIZE, fp);
		if(rv <= 0){
			break;
		}
		gCurrent = _ftelli64(fp);

		SendProgress(0.1f + 0.9f * gInvFileLen * gCurrent);

		rv = AddSFDecompressData(buffer, rv);
		//if(rv < 0){
		//	goto EXIT;
		//}
	}

	if(SFDecompressFinish() < 0){
		gErrorCode = -7;
		goto EXIT;
	}
	gErrorCode = 1;
EXIT:
	SFDecompressFinish();
	DisposeSFTree();
	if(fp){
		fclose(fp);
	}
	SendProgress(gInvFileLen * gCurrent);
	gThread = NULL;
	return 0;
}

void SendProgress(float progress){
	gProgress = progress;
	//printf("%f\n", progress);
	//if(GetProgress){
	//	GetProgress(progress, gErrorCode);
	//}
}

unsigned int GetFileLen(FILE* fp){
	long long current, totalLen;
	if(!fp){
		return 0.0f;
	}
	current = _ftelli64(fp);

	fseek(fp, 0L,SEEK_END); 
	totalLen = _ftelli64(fp);
	fseek(fp, current, SEEK_SET); 

	return totalLen;
}


