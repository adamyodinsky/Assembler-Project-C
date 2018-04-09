/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#define WORD_LENGTH 10
#define DC_OFFSET 0
#include <stdio.h>
extern int DC_Counter;

typedef struct Data {
    char val[WORD_LENGTH + 1];
    struct Data *nextData;
} DataT;

typedef struct dataNode *dataNodePtr;

typedef struct dataNode {
    int address;
    DataT *dataHead;
    DataT *dataTail;
    char *label;
    dataNodePtr *next;
} dataItem;

void addDataToList(char*, int);
dataNodePtr searchDataByLabel(char *);
void printData();
void printDataWords();
void cleanDataList();
void printDataToFile(FILE **file);


