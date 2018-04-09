/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */

#include <stdio.h>
#define IC_OFFSET 100
#define OPCODE_LENGTH 4
#define OPERAND_CODE_LENGTH 2
#define ERA_CODE_LENGTH 2
#define MAX_LABEL_LENGTH 30
#define REG_CODE_LENGTH 4
#define ABS "00"
#define RELOCATABLE "10" /*In terms of ERA*/
#define EXTERNAL "01" /*In terms of ERA*/
#define WORD_LENGTH 10 /*The final word length in bits*/
#define MAX_ERROR_MESSAGE 256
#define NONE 0
#define WEIRD_LENGTH 3



typedef struct wordNode *wordPtr;
typedef struct wordNode{
    /*
     * Holds the data of the final command words in binary
     */
    int address;
    char data [WORD_LENGTH+1];
    wordPtr leftS;
    wordPtr rightS;
} wordNode;

typedef struct outLine *outLinePtr;
typedef struct outLine {
    /*
     * Holds data the represents lines in the .ent and .ext files
     */
    char symbol [MAX_LABEL_LENGTH+1];
    int address;
    outLinePtr next;
}outLine;

void addCommandFirstWord (char *FuncName, int pType1, int pType2); /*Adds the first command word*/
int getCommandNumberByString (char *command); /*returns command code by string*/
void setCommandFirstWordData (char output[WORD_LENGTH], int commandNum, int pType1, int pType2); /*Set the first word bits the the "output" string */
void addCommandWordFunc (int cycle, char *funcName,int pType1,int pType2,int p1reg, int p2reg,char p1Symbol[MAX_LABEL_LENGTH+1],char p2Symbol [MAX_LABEL_LENGTH+1]); /*The main function that handle the add of new command words.*/
void addWord(wordPtr *root,wordPtr *new); /*Add "new" to "root" binary tree*/

void printWord (wordPtr *root); /*Prints the root binary tree in-order*/
void printCommandWords (void);
void addToEntFile (char *symbol, int address); /*Adds line to headEntFile binary tree with the "symbol" and "address" values*/
void printWordToFile (FILE **file, int IC, int DC); /*prints command words to file*/
void printEntFile (FILE **file); /*prints the headEntFile binary tree to file in-order*/
void printOtherFile (FILE **file, outLinePtr *root); /*general function used by printExtFile and printEntFile the prints root binary tree to file*/
void printExtFile (FILE **file); /*prints the headEntFile binary tree to file in-order*/
void printHandler(char* str); /*Handle errors*/
void cleanCommandWords(void); /*clean the headCommandWords binary tree*/
void cleanCommandWordsr (wordPtr root); /*Called by cleanCommandWords func and clean the root recursively*/
void clearOtherFiles (void); /*clear the .ent and .ext binary trees*/
void clearOtherFilesr (outLinePtr root); /*Called by clearOtherFiles and clean the root recursively*/
void addToFile (outLinePtr *root,char *symbol, int address); /*Called by addToExtFile and addToEntFile and adds the "address" and "symbol" to "root"*/
void addToExtFile (char *symbol, int address); /*Adds "symbol" and "address" to ext binary tree*/
void printWord32Weird (FILE **file,wordPtr root); /*Prints the command words binary tree in the root the file*/
void addCommandWord (int cycleNum, char *args1, char *args2, int pType1, int pType2, int instCounter, char *funcName, char *params); /*Used by other c files to add command words*/
void addRegWord (int sourceRegNum, int destRegNum); /*add command word with register address*/
void addImmWord (int ImmVal);/*add comand word with Immidiate number*/
void wordPtrInitialize (wordPtr ptr); /*Initialize the ptr*/
void addStructWord(char *symbol, int reg) ;
void addDirectWord (char Symbol [MAX_LABEL_LENGTH+1]); /*add command word with direct operand*/
char * get32WeirdByInt(char *result, int num); /*returns a string that represents the num in 32 weird*/
int existsExtFile ();
int existsEntFile ();
