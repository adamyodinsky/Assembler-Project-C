/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "MSG.h"

char *trimLeft(char *c) {
    /*check null */
    if (!c) {
        return c;
    }
    /*<= 32 are chars which are not invisible */
    while (*c <= 32 && *c > 0) {
        c++;
    }
    return c;
}
char *trimRight(char *c) {
    char* back;

    /*check null */
    if (!c) {
        return c;
    }
    back = c + strlen(c);
    /*<= 32 are chars which are not invisible */
    while (*--back <= 32 && *back > 0)
        ;
    *(++back) = '\0';
    return c;
}

char checkValidNumber(char *str, char isData, void (*printFunc)(char *)) {
    int i = 0;
    int num;
    int length;
    if (!str) {
        /*str is null */
        return 0;
    }
    str = trimLeft(trimRight(str));
    length = strlen(str);
    if (length) {
        /*check invalid prefix*/
        if (!isDigit(str[0])) {
            if (str[0] != '+' && str[0] != '-') {
                printFunc("invalid immediate number");
                return 0;
            }
            i = 1;
        }
        /* check if rest of the string contains only digits */
        while (i < length) {
            if (!isDigit(str[i])) {
                /*check num is not decimal */
                if (str[i] == '.') {
                    printFunc("invalid number, decimal is not supported");
                    return 0;
                } else {
                    printFunc("invalid number format");
                    return 0;
                }
            }
            i++;
        }
    } else {
        /*str is not null but length 0*/
        printFunc("missing number");
        return 0;
    }
    num = atoi(str);
    if (isData) {
        /* Check if number is in ragne of -510 to 511*/
        if (num < LOW_NUM_DATA_LIMIT || num > HIGH_NUM_DATA_LIMIT) {
            printFunc(MSG_DATA_RANGE_ERR);
            return 0;
        }
    } else {
        if (num < LOW_NUM_IMM_LIMIT || num > HIGH_NUM_IMM_LIMIT) {
            printFunc(MSG_IMM_RANGE_ERR);
            return 0;
        }
    }
    return 1;
}

char isDigit(char c) {
    if (c > '9' || c < '0')
        return 0;
    else
        return 1;
}

char *my_strtok(char *str, char const *delims) {
    static char *src = NULL;
    char *p, *ret = 0;

    if (str) {
        src = str;
    }

    /*Handle NULL case and end of str*/
    if (!src || *src == '\0') {
        return NULL;
    }

    ret = src;
    if ((p = strpbrk(src, delims))) {
        /*change delims to 0 for terminate string */
        *p = 0;
        /* Prepare src for next call*/
        src = ++p;
    } else {
        if (strcmp(delims, "\0") == 0) {
            ret = src;
        } else {
            /* delims no found, so we return null and init src */
            ret = NULL;
        }
    }
    /*remove left white chars. We considered all white chars as single instance*/
    src = trimLeft(src);
    return ret;
}

char *my_reverse_strtok(char *str, char const *delims) {
	static char *src = NULL;
	char *p;

	if (str) {
		src = str;
	}

	/*Handle NULL case and end of str*/
	if (!src || *src == '\0') {
		return NULL;
	}
	if ((p = strpbrk(src, delims))) {
		/*change delims to 0 for terminate string */
		*p = 0;
		/* Prepare src for next call*/
		src = ++p;
	}
	/*remove left white chars. We considered all white chars as single instance*/
	src = trimLeft(src);
	return src;
}

void convertIntToBinary(char *binaryRep, int n, int bitLength) {
    /* forcing evaluation as an unsigned value prevents complications*/
    /* with negative numbers at the left-most bit*/
    unsigned int u = *(unsigned int*) &n;
    int i;
    /* determine the number of bits needed ("sizeof" returns bytes)*/
    int nbits = bitLength;
    char *s = malloc((size_t) nbits + 1); /* +1 for '\0' terminator*/

    unsigned int mask = 1 << (nbits - 1); /* fill in values right-to-left*/
    s[nbits] = '\0';
    for (i = 0; i < nbits; i++, mask >>= 1)
        s[i] = ((u & mask) != 0) + '0';
    memcpy(binaryRep, s, bitLength);
    return;
}

int NeededDigitsInBinByPosDec(int num) {
    /*Determine how many bits are needed to represent num in binary*/
    int counter = 0;
    while (num > 0) {
        num = num / 2;
        counter++;
    }
    if(counter > 5)
        return WORD_LENGTH;

    return (WORD_LENGTH/2);
}

void convertBinTo32Weird(char *bin, char* result) {
	/*Convert binary number to 32 weird number.*/
	int i, j;
	const char base32[32] = "!@#$%^&*<>abcdefghijklmnopqrstuv"; /*weird 32 base array*/
	int tempDecimal = 0;

	/*1st digit of 32 weird*/
	for (i = 4, j = 0; i >= 0; i--, j++) {
		tempDecimal += pow(2, j) * (bin[i] - '0');
	}
	result[0] = base32[tempDecimal]; /*save 1st digit in result string*/

    /*2nd digit of 32 weird*/
	tempDecimal = 0;
	for (i = 9, j = 0; i >= 5; i--, j++) {
		tempDecimal += pow(2, j) * (bin[i] - '0');
	}
	result[1] = base32[tempDecimal]; /*save 2nd digit in result string*/

    /*put EOF string*/
	result[2] = '\0';

	return;
}

