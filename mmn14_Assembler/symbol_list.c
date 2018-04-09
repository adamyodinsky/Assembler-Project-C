/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#include "symbol_list.h"
#include "data_list.h"
#include "MSG.h"
#include <stdlib.h>
#include <string.h>

static symbolPtr headSymbol = NULL; /*pointer head of the list*/

void addSymbol(char *str, char isExternal, char isFunction, int address,
        void (*funcPrint)(char *)) {
    symbolPtr newNode = (symbolPtr) malloc(sizeof(symbolPtr));
    SymbolT *newLabel = (SymbolT *) malloc(sizeof(SymbolT));
    char *tempStr = (char *) malloc(sizeof(strlen(str)));

    if (!newNode || !newLabel || !tempStr) {
        fprintf(stderr, MSG_ALLOC_ERR);
        exit(1);
    }
    strcpy(tempStr, str);
    /*build label node*/
    newLabel->name = tempStr;
    newLabel->isExtern = isExternal;
    newLabel->isFunction = isFunction;
    newLabel->address = address;
    newLabel->isEntry = 0;

    newNode->leftS = NULL;
    newNode->rightS = NULL;
    newNode->symbolData = newLabel;
    /*first item in the list*/
    if (!headSymbol) {
        headSymbol = newNode;
        return;
    }
    /*needs to look for correct location to keep list sorted*/
    addSortedSymbol(newNode, funcPrint);
}

void addSortedSymbol(symbolPtr newLabel, void (*funcPrint)(char *)) {
    symbolPtr current = headSymbol;
    int cmp;

    while (current) {
        cmp = strcmp(newLabel->symbolData->name, current->symbolData->name);

        /* item already exist case*/
        if (cmp == 0) {
            if (current->symbolData->isExtern == 0)
                (*funcPrint)(MSG_DUPLICATE_SYMBOL);
            return;
        }
        if (cmp < 0) {
            /* insert smaller word if left side is null */
            if (current->leftS == NULL) {
                current->leftS = (symbolPtr *) newLabel;
                return;
            }
            /* if it's not null, check left item */
            else {
                current = (symbolPtr) current->leftS;
            }
        } else {
            /* insert bigger word if right side is null */
            if (current->rightS == NULL) {
                current->rightS = (symbolPtr *) newLabel;
                return;
            }
            /* if it's not null, check right item */
            else {
                current = (symbolPtr) current->rightS;
            }
        }

    }

}

symbolPtr searchSymbolByName(char *label) {
    symbolPtr current = headSymbol;
    int cmp;

    while (current) {
        cmp = strcmp(label, current->symbolData->name);

        /* item already exist case*/
        if (cmp == 0) {
            return current;
        }
        if (cmp < 0) {
            /* label can not be found */
            if (current->leftS == NULL) {
                break;
            }
            /* if it's not null, check if label is in left item */
            else {
                current = (symbolPtr) current->leftS;
            }
        } else {
            /* label can not be found */
            if (current->rightS == NULL) {
                break;
            }
            /* if it's not null, check if label is in right item */
            else {
                current = (symbolPtr) current->rightS;
            }
        }
    }
    return NULL;

}

void recalculationDataAddress(int icOffset, void (*funcPrint)(char *)) {
    recalculationDataAddressr(headSymbol, icOffset, funcPrint);
}

/*print binary tree recursive LMR*/
void recalculationDataAddressr(symbolPtr root, int icOffset,
        void (*funcPrint)(char *)) {
    dataNodePtr temp;
    if (root) {
        if (root->leftS) {
            recalculationDataAddressr((symbolPtr) root->leftS, icOffset,
                    funcPrint);
        }

        if (root->symbolData->address == 0 && !root->symbolData->isFunction
                && !root->symbolData->isExtern) {
            temp = searchDataByLabel(root->symbolData->name);
            if (temp) {
                root->symbolData->address = icOffset + temp->address;
                temp->address += icOffset;
            } else {
                funcPrint(MSG_MISSING_LABEL);
            }
        }

        if (root->rightS) {
            recalculationDataAddressr((symbolPtr) root->rightS, icOffset,
                    funcPrint);
        }
    }
}

/*print binary tree recursive LMR*/
void print_Symbolsr(symbolPtr root) {
    if (root) {
        if (root->leftS) {
            print_Symbolsr((symbolPtr) root->leftS);
        }

        printf("%s,\tisFunction= %d,\tisExtern = %d,\taddress=%d\tisEntry=%d\n",
                root->symbolData->name, root->symbolData->isFunction,
                root->symbolData->isExtern, root->symbolData->address,
                root->symbolData->isEntry);

        if (root->rightS) {
            print_Symbolsr((symbolPtr) root->rightS);
        }
    }
}

/*free memory allocated for the list*/
void cleanSymbols() {
    if (!headSymbol) {
        cleanSymbolsr(headSymbol);
    }
    headSymbol = NULL;
}

/* recursive method to free all nodes memory */
void cleanSymbolsr(symbolPtr root) {
    if (root) {
        if (!root->leftS) {
            cleanSymbolsr((symbolPtr) root->leftS);
        }
        free(root->leftS);
        free(root->rightS);
        free(root->symbolData->name);
        free(root->symbolData);
        if (root->rightS) {
            cleanSymbolsr((symbolPtr) root->rightS);
        }
    }
}

