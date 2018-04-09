/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "word_list.h"
#include "utils.h"
#include "MSG.h"
#include "symbol_list.h"
#include "cycle1And2.h"

wordPtr headCommandWords = NULL; /*Binary tree head of the commands words*/
outLinePtr headEntFile = NULL; /*Binary tree head of the .ent lines*/
outLinePtr headExtFile = NULL;/*Binary tree head of the .ext lines*/
int instCounterlocal; /*local IC counter*/

void addCommandWord(int cycleNum, char *args1, char *args2, int pType1, int pType2,
                    int instCounterTemp, char *funcName, char *params) {
    /*
     * Used by other c files to add command words
     * "cycleNum" is the cycle number (1 or 2)
     * "args1/2" holds the operand symbol.
     * instCounterTemp holds the IC_counter
     * "funcName" is the name of the function (e.g. "cmp" or "lea")
     * "pType1/2" is the type of the operands by enum (imm = 0, direct = 1, struct = 2, register = 3, error = -2)
     * "p1/2reg" holds the number of the register for the struct indexes.
     * "p1/2Symbol" holds the symbol of the operand
     */
    int p1reg = eUndef, p2reg = eUndef;
    char p1Symbol[MAX_LABEL_LENGTH + 1];
    char p2Symbol[MAX_LABEL_LENGTH + 1];
    char *reg_pointer;
    instCounterlocal = instCounterTemp;

    if (args1)
        memcpy(p1Symbol, args1, MAX_LABEL_LENGTH); /*Copy the label to local string*/
    if (args2)
        memcpy(p2Symbol, args2, MAX_LABEL_LENGTH); /*Copy the label to local string*/
    if (pType1 == eStruct)
        sscanf(params + strlen(args1),"%*[^A-Za-z0-9]%d", &p1reg);
    if (pType2 == eStruct){
        reg_pointer = strstr(params, p2Symbol);
        sscanf(reg_pointer + strlen(args2),"%*[^A-Za-z0-9]%d", &p2reg);
    }
    addCommandWordFunc(cycleNum, funcName, pType1, pType2, p1reg, p2reg, p1Symbol, p2Symbol);
}

void addCommandWordFunc(int cycle, char *funcName, int pType1, int pType2,
        int p1reg, int p2reg, char p1Symbol[WORD_LENGTH+1], char p2Symbol[WORD_LENGTH+1]) {
    /*
     * The main function that handle the add of new command words.
     * "cycle" is the cycle number (1 or 2)
     * "funcName" is the name of the function (e.g. "cmp" or "lea")
     * "pType1/2" is the type of the operands by enum (imm = 0, direct = 1, struct = 2, register = 3, error = -2)
     * "p1/2reg" holds the number of the register for the struct indexes.
     * "p1/2Symbol" holds the symbol of the operand
     */
    if (cycle == 1) /*All the first words of the commands are added on cycle 1*/
        addCommandFirstWord(funcName, pType1, pType2);
    if (pType1 == eUndef && pType2 == eUndef) /*If an error was found on both operands*/
        return;
    if (pType1 == eReg && pType2 == eReg && cycle == 1) {
        /*
         * Special case where the source and destination operands share the same word
         */
        addRegWord(atoi(p1Symbol + 1), atoi(p2Symbol + 1));
        return;
    }
    if (pType1 == eReg) {
        if (cycle == 1) {
            addRegWord(atoi(p1Symbol + 1), NONE);
        } else
            instCounterlocal++;

    } else if (pType1 == eImm) {
        if (cycle == 1) {
            addImmWord(atoi(p1Symbol + 1));
        } else
            instCounterlocal++;
    } else if (pType1 == eDirect) {
        if (cycle == 1)
            instCounterlocal += 1;
        else { /*cycle = 2 */
            addDirectWord(p1Symbol);
        }
    } else if (pType1 == eStruct) {
        if (cycle == 1)
            instCounterlocal += 2;
        else { /*cycle = 2 */
            addStructWord(p1Symbol, p1reg);
        }
    }
    if (pType2 == eReg && cycle == 1) {
        addRegWord(NONE, atoi(p2Symbol + 1));
    } else if (pType2 == eImm && cycle == 1) {
        addImmWord(atoi(p2Symbol + 1));
    } else if (pType2 == eDirect) {
        if (cycle == 1)
            instCounterlocal += 1;
        else { /*cycle = 2 */
            addDirectWord(p2Symbol);
        }
    } else if (pType2 == eStruct) {
        if (cycle == 1)
            instCounterlocal += 2;
        else { /*cycle = 2 */
            addStructWord(p2Symbol, p2reg);
        }
    }
    return;
}

void addCommandFirstWord(char *funcName, int pType1, int pType2) {
    /*
     * Adds the first command word
     */
    wordPtr tempWord = (wordPtr) malloc(sizeof(wordNode));
    if (!tempWord) {
        fprintf(stderr, MSG_ALLOC_ERR);
        exit(0);
    }
    instCounterlocal--;
    wordPtrInitialize(tempWord);
    setCommandFirstWordData((tempWord->data), getCommandNumberByString(funcName),
            pType1, pType2);
    addWord(&headCommandWords, &tempWord);
}

void addWord(wordPtr *root, wordPtr *new) {
    /*
     * Add "new" to "root" binary tree
     */
    if (!(*root)) {
        (*root) = (*new);
        return;
    } else {
        if ((*root)->address < (*new)->address)
            addWord(&(*root)->rightS, new);
        else
            addWord(&(*root)->leftS, new);
    }
}

int getCommandNumberByString(char *command) {
    /*
     * returns command code by string
     */
    cmdT *temp = NULL;
    if ((temp = findFuncByName(command))) {
        return temp->code;
    }
    return -1;
}

void setCommandFirstWordData(char output[WORD_LENGTH + 1], int commandNum,
        int pType1, int pType2) {
    /*
     * Set the first word bits the the "output" string
     */
    char binaryRep[WORD_LENGTH + 1];
    binaryRep[WORD_LENGTH] = '\0';
    convertIntToBinary(binaryRep, commandNum, WORD_LENGTH);
    memcpy(output, binaryRep + (WORD_LENGTH - OPCODE_LENGTH), OPCODE_LENGTH);
    if (pType1 != eUndef) {
        convertIntToBinary(binaryRep, pType1, WORD_LENGTH);
        memcpy(output + OPCODE_LENGTH,
                binaryRep + (WORD_LENGTH - OPERAND_CODE_LENGTH),
                OPERAND_CODE_LENGTH);
    }
    if (pType2 != eUndef) {
        convertIntToBinary(binaryRep, pType2, WORD_LENGTH);
        memcpy(output + OPCODE_LENGTH + OPERAND_CODE_LENGTH,
                binaryRep + (WORD_LENGTH - OPERAND_CODE_LENGTH),
                OPERAND_CODE_LENGTH);
    }
    memcpy(output + OPCODE_LENGTH + OPERAND_CODE_LENGTH + OPERAND_CODE_LENGTH,
            "00", ERA_CODE_LENGTH);
    return;
}

void addRegWord(int sourceRegNum, int destRegNum) {
    /*
     * add command word with register address
     */
    wordPtr temp = (wordPtr) malloc(sizeof(wordNode));
    if (!temp) {
        printHandler(MSG_ALLOC_ERR);
        return;
    }
    wordPtrInitialize(temp);
    convertIntToBinary((temp->data), sourceRegNum, REG_CODE_LENGTH);
    convertIntToBinary((temp->data) + REG_CODE_LENGTH, destRegNum,
            REG_CODE_LENGTH);
    addWord(&headCommandWords, &temp);
}

void addImmWord(int ImmVal) {
    /*
     * add command word with Immidiate number
     */
    wordPtr temp = (wordPtr) malloc(sizeof(wordNode));
    if (!temp) {
        printHandler(MSG_ALLOC_ERR);
        return;
    }
    wordPtrInitialize(temp);
    convertIntToBinary((temp->data), ImmVal, WORD_LENGTH - 2);
    addWord(&headCommandWords, &temp);
}

void addDirectWord(char Symbol[MAX_LABEL_LENGTH + 1]) {
    /*
     * add command word with direct operand
     */
    symbolPtr tempSymbol = searchSymbolByName(Symbol);
    if (tempSymbol) {
        wordPtr temp = (wordPtr) malloc(sizeof(wordNode));
        if (!temp) {
            printHandler(MSG_ALLOC_ERR);
            return;
        }
        wordPtrInitialize(temp);
        convertIntToBinary((temp->data), ((tempSymbol->symbolData)->address),
                WORD_LENGTH - 2);
        if ((tempSymbol->symbolData)->isExtern == 0)
            strcpy(((temp->data) + WORD_LENGTH - 2), RELOCATABLE);
        else {
            addToExtFile(Symbol, instCounterlocal);
            strcpy(((temp->data) + WORD_LENGTH - 2), EXTERNAL);
        }
        addWord(&headCommandWords, &temp);
    } else {
        char *errorMessage = (char *) malloc(sizeof(MAX_ERROR_MESSAGE));
        sprintf(errorMessage, MSG_SYMBOL_NOT_FOUND, Symbol);
        printHandler(errorMessage);
        free(errorMessage);
    }
}

void addStructWord(char symbol[WORD_LENGTH+1], int reg) {
    /*
     * adds command word with struct operand
     */
    symbolPtr tempSymbolPtr = searchSymbolByName(symbol);
    if (tempSymbolPtr) {
        wordPtr tempWordPtr = (wordPtr) malloc(sizeof(wordNode));
        if (!tempWordPtr) {
            printHandler(MSG_ALLOC_ERR);
            return;
        }
        wordPtrInitialize(tempWordPtr);
        convertIntToBinary((tempWordPtr->data), ((tempSymbolPtr->symbolData)->address),
                WORD_LENGTH - 2);
        strcpy(((tempWordPtr->data) + WORD_LENGTH - 2), RELOCATABLE);
        addWord(&headCommandWords, &tempWordPtr);
        tempWordPtr = (wordPtr) malloc(sizeof(wordNode));
        wordPtrInitialize(tempWordPtr);
        convertIntToBinary((tempWordPtr->data) + REG_CODE_LENGTH, reg, REG_CODE_LENGTH);
        addWord(&headCommandWords, &tempWordPtr);
    } else {
        char *errorMessage = (char *) malloc(sizeof(MAX_ERROR_MESSAGE));
        sprintf(errorMessage, MSG_SYMBOL_NOT_FOUND, symbol);
        printHandler(errorMessage);
    }
}


void printWord(wordPtr *root) {
    /*
     * Prints the root in order
     */
    if (*root) {
        if ((*root)->leftS) {
            printWord(&((*root)->leftS));
        }
        printf("\n%d\t%s", (*root)->address, (*root)->data);
        if ((*root)->rightS) {
            printWord(&((*root)->rightS));
        }
    }
}

void addToEntFile(char *symbol, int address) {
    /*
     * Adds "symbol" and "address" to ent binary tree
     */
    addToFile(&headEntFile, symbol, address);
}

void addToFile(outLinePtr *root, char *symbol, int address) {
    /*
     * adds a line to the .ent or .ext file.
     */
    int symbolLength;
    outLinePtr temp = (outLinePtr) malloc(sizeof(outLine));
    if (!temp) {
        fprintf(stderr, MSG_ALLOC_ERR);
        exit(0);
    }
    symbolLength = (int) strlen(symbol);
    memcpy(temp->symbol, symbol, (size_t) symbolLength);
    temp->symbol[symbolLength] = '\0';
    temp->address = address;
    temp->next = (outLinePtr) malloc(sizeof(outLine));
    temp->next = NULL;
    if (!(*root)) {
        (*root) = temp;
    } else {
        outLinePtr curr = (outLinePtr) malloc(sizeof(outLine));
        if (!curr) {
            fprintf(stderr, MSG_ALLOC_ERR);
            exit(0);
        }
        curr = (*root);
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = temp;
    }
}

void addToExtFile(char *symbol, int address) {
    /*
     * adds a line to the .ext or .ext file.
     */
    addToFile(&headExtFile, symbol, address);
}

char* get32WeirdByInt(char *result, int num) {
    /*Returns a string that is the num received in 32 weird base
     * It first convert the number to binary and than to 32 weird. */

    char numInBinary[WORD_LENGTH]={'0'};

    convertIntToBinary(numInBinary, num, WORD_LENGTH); /*Returns binary representation of num to numInBinary. */
    convertBinTo32Weird(numInBinary, result);
    return result;
}

void printWordToFile(FILE **file, int IC, int DC) {
    /*
     * prints command words to file
     */
    char result[WEIRD_LENGTH] = {'0'};
    fprintf(*file, "%s", get32WeirdByInt(result, IC));
    fprintf(*file, "\t%s\n", get32WeirdByInt(result, DC));

    if(headCommandWords)
        printWord32Weird(file, headCommandWords);

    return;
}

void printWord32Weird(FILE **file, wordPtr root) {
    /*
     * Prints the command words binary tree in the root the file
     */
    if (root->leftS)
        printWord32Weird(file, root->leftS);
    {
        char codeIn32Weird[WEIRD_LENGTH]= {'0'};
        char result[WEIRD_LENGTH] = {'0'};
        convertBinTo32Weird(root->data, codeIn32Weird);
        fprintf(*file, "%s", get32WeirdByInt(result, root->address));
        fprintf(*file, "\t%s\n", codeIn32Weird);
    }

    if (root->rightS)
        printWord32Weird(file, root->rightS);

    return;
}

void printEntFile(FILE **file) {
    /*
     * prints the headEntFile binary tree to file in-order
     */
    printOtherFile(file, &headEntFile);
}

void printOtherFile(FILE **file, outLinePtr *root) {
    /*
     * general function used by printExtFile and printEntFile the prints root binary tree to file
     */
    outLinePtr temp = *root;
    if (!temp) {
        printHandler(MSG_ALLOC_ERR);
        return;
    }
    while (temp) {
        char *result = (char *) malloc(sizeof(char *));
        if (!result) {
            printHandler(MSG_ALLOC_ERR);
            return;
        }
        get32WeirdByInt(result, temp->address);
        fprintf((*file), "%s\t%s\n", temp->symbol, result);
        free(result);
        temp = temp->next;
    }
}

void printExtFile(FILE **file) {
    /*
     * prints the headExtFile binary tree to file in-order
     */
    printOtherFile(file, &headExtFile);
}

void cleanCommandWords() {
    /*
     * clean the headCommandWords binary tree
     */

    cleanCommandWordsr(headCommandWords);
    headCommandWords = NULL;
    return;
}

void cleanCommandWordsr(wordPtr root) {
    /*
     * Called by cleanCommandWords func and clean the root recursively
     */
    if (root) {
        if (root->leftS)
            cleanCommandWordsr(root->leftS);
        if (root->rightS)
            cleanCommandWordsr(root->rightS);
        free(root);
    }
}

void clearOtherFiles(void) {
    /*
     * clear the .ent and .ext binary trees
     */
    clearOtherFilesr(headEntFile);
    clearOtherFilesr(headExtFile);
    headEntFile = NULL;
    headExtFile = NULL;
    return;
}

void clearOtherFilesr(outLinePtr root) {
    /*
     * Called by clearOtherFiles and clean the root recursively
     */
    if (root) {
        if (root->next)
            clearOtherFilesr(root->next);
        free(root);
    }
    return;
}

void wordPtrInitialize(wordPtr ptr) {
    /*
     * Initialize the ptr
     */
    int i;
    instCounterlocal++;
    ptr->leftS = NULL;
    ptr->rightS = NULL;
    for (i = 0; i < WORD_LENGTH; i++)
        ptr->data[i] = '0';
    ptr->data[WORD_LENGTH] = '\0';
    ptr->address = instCounterlocal;
}

int existsEntFile (){
    if (headEntFile)
        return 1;
    return 0;
}

int existsExtFile (){
    if (headExtFile)
        return 1;
    return 0;
}

