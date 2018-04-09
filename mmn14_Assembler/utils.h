/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#ifndef UTILS_H
#define UTILS_H

#define WORD_LENGTH 10
#define LOW_NUM_DATA_LIMIT -512
#define HIGH_NUM_DATA_LIMIT 511
#define LOW_NUM_IMM_LIMIT -128
#define HIGH_NUM_IMM_LIMIT 127
#include <math.h>

typedef enum {
    eUndef = -2, eError = -1, eImm = 0, eDirect = 1, eStruct = 2, eReg = 3
} addressingType;


char *trimLeft(char *c);
char *trimRight(char *c);
char checkValidNumber(char *str, char isData, void (*printFunc)(char *));
char isDigit(char c);
char *my_strtok(char *, char const *);
char *my_reverse_strtok(char *str, char const *delims);
void convertIntToBinary(char binaryRep[WORD_LENGTH + 1], int n, int bitLength); /* determine the number of bits needed ("sizeof" returns bytes)*/
int NeededDigitsInBinByPosDec(int num); /*Determine how many bits are needed to represent num in binary*/
void convertBinTo32Weird(char*, char*); /*Convert binary number to 32 weird number.*/

#endif 
