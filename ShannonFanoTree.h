#pragma once
#include "FreqCount.h"

#define SFSTRINGSCALE 16
#define TMPARRSIZE 16

enum eSFStatus{eSFStatus_Default, eSFStatus_Collecting, eSFStatus_Finished};

struct SFTreeNode{
	int flag;
	unsigned int num;
	char* str;
	int level;
	struct SFTreeNode* parent;
	struct SFTreeNode* left;
	struct SFTreeNode* right;
};


struct SFMapping{
	char str[FREQTABLESIZE];
};


void IniSFTree();
void DisposeSFTree();
long long unsigned GenSFTree(FreqTable* freqTable);
//char* GetSFStirng(unsigned int num, bool* check);
struct SFMapping*  GetSFMapping(bool* check);
struct SFTreeNode* GetSFTreeRoot(bool* check);
//int IniDecompress();
//unsigned int Decompress(int num, bool* check);

void PrintSFTree();
void PrintSFNode(struct SFTreeNode* node, int level);
void TraversalSFMapping();
