#include "stdafx.h"
#include <stdlib.h>
#include <Windows.h>
#include "ShannonFanoTree.h"
//#include "FreqCount.h"

static int                gStatus = eSFStatus_Default;
static struct SFTreeNode* gRoot = NULL;
static struct SFMapping   gMapping[FREQTABLESIZE];

static struct SFTreeNode* gCurrentNode = NULL;

static long long unsigned gTotalBit = 0;

struct SFTreeNode* NewNode(struct SFTreeNode* parent, int level);
void NextSFTree(FreqTable* freqTable, int dataOffset, int dataNumm, struct SFTreeNode* thisNode);
void WriteMapping(struct SFTreeNode* current, unsigned int num, int level);
void FreeSFNode(struct SFTreeNode *node);

void IniSFTree(){
	DisposeSFTree();
	for(int i = 0; i < FREQTABLESIZE; ++i){
		memset(&gMapping[i], 0, sizeof(struct SFMapping));
	}
	gTotalBit = 0;
	gStatus = eSFStatus_Collecting;
}

void DisposeSFTree(){
	gStatus = eSFStatus_Default;
	if(gRoot == NULL){
		return;
	}
	FreeSFNode(gRoot);
	gRoot = NULL;
}

void FreeSFNode(struct SFTreeNode *node){
	if(node == NULL){
		return;
	}
	FreeSFNode(node->left);
	FreeSFNode(node->right);
	free(node);
}

long long unsigned GenSFTree(FreqTable* freqTable){
	//IniSFTree();
	gRoot = NewNode(NULL, 0);
	NextSFTree(freqTable, 0, FREQTABLESIZE, gRoot);
	gStatus = eSFStatus_Finished;
	return gTotalBit;
}

//ShannonFano演算法的核心，根據該法切割樹的node，並決定編碼
void NextSFTree(FreqTable* freqTable, int dataOffset, int dataNum, struct SFTreeNode* thisNode){
	if(dataNum == 0 || gStatus == eSFStatus_Finished){
		return;
	}

	//struct SFTreeNode* node = NewNode(parent, level);
	double leftSum, rightSum;
	int leftIdx, rightIdx;

	if(dataNum == 1){
		thisNode->num = freqTable[dataOffset].num;
		thisNode->flag = 1;
		WriteMapping(thisNode, thisNode->num, thisNode->level);
	}else if(dataNum > 1){
		leftIdx = dataOffset;
		rightIdx = dataOffset + dataNum - 1;
		leftSum = freqTable[leftIdx].freq;
		rightSum = freqTable[rightIdx].freq;

		//將一個陣列切割，使得陣列左邊的和和右邊的和最接近
		while(leftIdx + 1 < rightIdx){
			if(leftSum >= rightSum){
				rightSum += freqTable[--rightIdx].freq;
			}else{
				leftSum += freqTable[++leftIdx].freq;
			}
		}

		thisNode->left = NewNode(thisNode, thisNode->level + 1);
		NextSFTree(freqTable, dataOffset, rightIdx - dataOffset, thisNode->left);

		thisNode->right = NewNode(thisNode, thisNode->level + 1);
		NextSFTree(freqTable, rightIdx, dataOffset + dataNum - rightIdx, thisNode->right);
	}
	return;
}

struct SFTreeNode* NewNode(struct SFTreeNode* parent, int level){
	struct SFTreeNode* node = (struct SFTreeNode*)malloc(sizeof(struct SFTreeNode));
	node->parent = parent;
	node->level = level;
	node->flag = 0;
	node->left = NULL;
	node->right = NULL;
	node->num = 0;
	return node;
}

void PrintSFTree(){
	PrintSFNode(gRoot, 0);
}

void PrintSFNode(struct SFTreeNode* node, int level){
	for(int i = 0; i < level; ++i){
		printf("%s", "|  ");
	}

	if(node == NULL){
		printf("%s", "+- ");
		printf("<null>\n");
		return;
	}else{
		printf("%s", "+- ");
		if(node->flag == 0){
			printf("<Lv%d, Space>\n", level);
		}else{
			printf("<Lv%d, %5u>\n", level, node->num);
		}

		PrintSFNode(node->left , level + 1);
		PrintSFNode(node->right, level + 1);
		return;
	}
}


void WriteMapping(struct SFTreeNode* current, unsigned int num, int level){
	long long unsigned scale = (long long unsigned)level;
	long long unsigned *counter;

	gMapping[num].str[level] = '\0';
	for(int i = level - 1; i >= 0; i--){
		if(current->parent->left == current){
			gMapping[num].str[i] = '0';
		}else{
			gMapping[num].str[i] = '1';
		}
		current = current->parent;
	}

	counter = GetCounter();
	gTotalBit += scale * counter[num];
}


void TraversalSFMapping(){
	_mkdir("..\\SF");
	FILE* fp = fopen("..\\SF\\SFMapping.txt", "w");
	for(int i = 0; i < FREQTABLESIZE; ++i){
		fprintf(fp, "Mapping[%4d] = %s\n", i, gMapping[i].str);
	}
	fclose(fp);
}


struct SFMapping* GetSFMapping(bool* check){
	if(gStatus != eSFStatus_Finished){
		*check = false;
		return NULL;
	}
	*check = true;
	return gMapping;
}


struct SFTreeNode* GetSFTreeRoot(bool* check){
	if(gStatus != eSFStatus_Finished || gRoot == NULL){
		*check = false;
		return NULL;
	}
	*check = true;
	return gRoot;
}
