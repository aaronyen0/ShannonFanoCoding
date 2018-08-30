#include "stdafx.h"
#include "FreqCount.h"

static FreqTable gFreqTable[FREQTABLESIZE];
static long long unsigned gCounter[FREQTABLESIZE];

void SortFreqTable();


void InitialFreqTable(){
	for(int i = 0; i < FREQTABLESIZE; ++i){
		gCounter[i] = 0;
		gFreqTable[i].num = (unsigned int)i;
	}
}

int AddNum(unsigned int num){
	if(num >= FREQTABLESIZE){
		return -1;
	}
	gCounter[num]++;
	
	return 0;
}

int LoadCounter(unsigned int num, long long unsigned count){
	if(num >= FREQTABLESIZE){
		return -1;
	}
	gCounter[num] = count;
	return 0;
}

long long unsigned* GetCounter(){
	return gCounter;
}

int ReadCounter(char* data, int dataLen){
	//long long unsigned sum;
	unsigned char* ptr = (unsigned char*)data;
	unsigned char* tmp = (unsigned char*)&gCounter;

	InitialFreqTable();
	if(dataLen % 8 != 0 || (dataLen>>3) != FREQTABLESIZE){
		return -1;
	}
	for(int i = 0; i < dataLen; i++){
		tmp[i] = ptr[i];
	}
	return 0;
}


struct FreqTable* GetFreqTable(){
	double sum = 0;
	for(int i = 0; i < FREQTABLESIZE; ++i){
		sum += gCounter[i];
	}

	for(int i = 0; i < FREQTABLESIZE; ++i){
		gFreqTable[i].freq = (double)gCounter[i] / sum;
	}

	SortFreqTable();
	return &gFreqTable[0];
}

void SortFreqTable(){
	FreqTable tmp;
	int maxIndex;

	for(int i = 0; i < FREQTABLESIZE; ++i){
		maxIndex = i;
		for(int j = i + 1; j < FREQTABLESIZE; ++j){
			if(gFreqTable[maxIndex].freq < gFreqTable[j].freq){
				maxIndex = j;
			}
		}
		tmp = gFreqTable[i];
		gFreqTable[i] = gFreqTable[maxIndex];
		gFreqTable[maxIndex] = tmp;
	}
}


void TraversalFreqTable(){
	_mkdir("..\\SF");
	FILE* fp;
	
	fp = fopen("..\\SF\\FerqTable.txt", "w");
	for(int i = 0; i < FREQTABLESIZE; ++i){
		fprintf(fp, "FreqTable[%4d]： num = %9u, freq = %8.4f\n", i, gFreqTable[i].num, gFreqTable[i].freq);
	}
	fclose(fp);

	fp = fopen("..\\SF\\CounterTable.txt", "w");
	for(int i = 0; i < FREQTABLESIZE; ++i){
		fprintf(fp, "CounterTable[%4d]： counter = %10u\n", i, gCounter[i]);
	}
	fclose(fp);
}
//void CountFreq(){
//	double sum = 0;
//	for(int i = 0; i < FREQTABLESIZE; ++i){
//		sum += gCounter[i];
//	}
//
//	for(int i = 0; i < FREQTABLESIZE; ++i){
//		gFreqTable[i] = (double)gCounter[i] / sum;
//	}
//}
