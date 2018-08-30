#pragma once
#define FREQTABLESIZE 256

struct FreqTable{
	unsigned int num;
	double freq;
};


void  InitialFreqTable();
int   AddNum(unsigned int num);
struct FreqTable* GetFreqTable();

int LoadCounter(unsigned int num, long long unsigned count);
long long unsigned* GetCounter();



void  TraversalFreqTable();
