/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "data_list.h"
#include "utils.h"
#include "word_list.h"
#include "MSG.h"

#define MAX_LABEL_LENGTH 30
static dataNodePtr headData = NULL; /*pointer head of the list*/
static dataNodePtr tailData = NULL; /*pointer tail of the list*/

int DC_Counter = DC_OFFSET;

void addDataToList(char *label, int val) {
    if (label) {
        /*add item to list method using FIFO*/
        dataNodePtr pTempItem = (dataNodePtr) malloc(sizeof(dataItem));
        struct Data *pDataTemp = (struct Data *) malloc(sizeof(DataT));
        char *tempStr = (char *) malloc(sizeof(strlen(label)));

        if (!pTempItem || !pDataTemp || !tempStr) {
            fprintf(stderr, MSG_ALLOC_ERR);
            exit(0);
        }
        strcpy(tempStr, label);
        convertIntToBinary(pDataTemp->val, val, WORD_LENGTH);
        pDataTemp->val[WORD_LENGTH] = '\0';
        pDataTemp->nextData = NULL;
        /*fill item data*/
        pTempItem->label = tempStr;
        pTempItem->address = DC_Counter;
        pTempItem->dataHead = pDataTemp;
        pTempItem->dataTail = pDataTemp;
        pTempItem->next = NULL; /*add to the end of the list so next is NULL*/

        if (!headData) /*first item in the list*/
        {
            headData = pTempItem;
            tailData = headData;
        } else {
            tailData->next = (dataNodePtr *) pTempItem;
            tailData = pTempItem;
        }
        DC_Counter++;
    }
}

dataNodePtr searchDataByLabel(char *str) {
    dataNodePtr ptr;
    for (ptr = headData; ptr && strcmp(ptr->label, str);
            ptr = (dataNodePtr) ptr->next)
        ;
    return ptr;
}

void cleanDataList() {
    dataNodePtr nodeNext = headData;
    dataNodePtr nodeTemp = NULL;
    struct Data *dataTemp = NULL;
    struct Data *dataNext = NULL;
    /*remove all dataNodes in the list*/
    while (nodeNext) {
        nodeTemp = nodeNext;
        nodeNext = (dataNodePtr) nodeNext->next;
        dataNext = nodeTemp->dataHead;
        /* remove each data in the dataNode */
        while (dataNext) {
            dataTemp = dataNext;
            dataNext = dataTemp->nextData;
            free(dataTemp);
        }
        free(nodeTemp);
    }
    headData = NULL;
}

void printDataToFile(FILE **file) {

    dataNodePtr temp = headData;
    struct Data *temp2 = NULL;
    char label[MAX_LABEL_LENGTH + 1] = "";
    int headAddress = 0;
    int addAddressFlag = 0;
    headAddress = headData->address;

    while (temp) {
        int j = 0;
        temp2 = temp->dataHead;
        if (strcmp(temp->label, label) != 0) {
            int k;
            for (k = 0; k < MAX_LABEL_LENGTH + 1; k++)
                label[k] = '\0';
            strcpy(label, temp->label);
            addAddressFlag = 0;
        } else {
            addAddressFlag = 1;
        }

        while (temp2) {
            char result[3];
            char codeIn32Weird[WEIRD_LENGTH] = {'0'};

            convertBinTo32Weird(temp2->val, codeIn32Weird);
            fprintf(*file, "%s", get32WeirdByInt(result, temp->address + j + (addAddressFlag * headAddress)));
            fprintf(*file, "\t%s\n", codeIn32Weird);

            temp2 = temp2->nextData;
            j++;

        }
        temp = (dataNodePtr) temp->next;
    }
    return;
}

