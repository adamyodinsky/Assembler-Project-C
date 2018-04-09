/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#include <string.h>
#include <stdlib.h>
#include "MSG.h"
#include "cycle1And2.h"
#include "word_list.h"
#include "data_list.h"
#include "symbol_list.h"

#define MAX_LINE_LENGTH 80

/* Creates a file (for writing) from a given name and ending, and returns a pointer to it. */
FILE *openFile(char *name, char *ending, const char *mode)
{
	FILE *file;
	char *mallocStr = (char *)malloc(strlen(name) + strlen(ending) + 1), *fileName = mallocStr;
	sprintf(fileName, "%s%s", name, ending);

	file = fopen(fileName, mode);
	free(mallocStr);

	return file;
}

int main(int argc, char **argv)
{
    FILE *file;
    FILE *obOut;
    FILE *entOut;
    FILE *extOut;
    int fileCounter = 1;
    char *line;
    char *token;
    int lineCount;
    char *tempName;

    /* Handle input errors */
    if (argc == 1) {
        fprintf( stderr, MSG_ARGS_MISSING);
        exit(1);
    }

    /* read all as files from args */
    while (fileCounter < argc) {
        DC_Counter = 0;
        lineCount = 1;
        cycle1Status = 1;
        instCounter = IC_OFFSET;

        /* preparing file output name argv[fileCounter].index */
        tempName = (char *) calloc(strlen(argv[fileCounter]), sizeof(char));
        if (!tempName) {
            fprintf(stderr, MSG_ALLOC_ERR);
            exit(1);
        }
        memcpy(tempName, argv[fileCounter], strlen(argv[fileCounter]));
        tempName[strlen(argv[1])]='\0';
        if (!(file = openFile(tempName, ".as", "r"))) {
            fprintf(stderr, MSG_FILE_OPEN_ERR, tempName);
            exit(1);
        }

        line = (char *) calloc(MAX_LINE_LENGTH, sizeof(char));
        if (!line) {
            fprintf(stderr, MSG_ALLOC_ERR);
            exit(1);
        }

        /* read file and index each word */
        while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
            token = strtok(line, "\n");
            firstAnalyze(token, lineCount, tempName);
            lineCount++;
        }

        if (cycle1Status) {/*Means there is no error in cycle 1*/
            /* cycle 2 start */
            recalculateSymbolAddress();
            instCounter = IC_OFFSET;
            rewind(file);
            lineCount = 1;
            cycle1Status = 1;
            while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
                token = strtok(line, "\n");
                secondAnalyze(token, lineCount, tempName);
                lineCount++;
            }
            if (cycle1Status){ /*Means there is no error in cycle 2*/
                strcat(tempName, ".ob");
                obOut = fopen(tempName, "w+");
                if (!obOut) {
                    fprintf(stderr, MSG_FILE_WRITE_OBJ_ERR);
                    exit(0);
                }

                printWordToFile(&obOut, instCounter - IC_OFFSET, DC_Counter); /*Adds the command words to the .ob file*/
                printDataToFile(&obOut); /*Adds the data words to the .ob files*/
                fclose(obOut);
                tempName[strlen(tempName) - 3] = '\0';

                if (existsEntFile ()) {
                    strcat(tempName, ".ent");
                    entOut = fopen(tempName, "w+");
                    if (!entOut) {
                        fprintf(stderr, MSG_FILE_WRITE_ENT_ERR);
                        exit(0);
                    }
                    printEntFile(&entOut); /*Adds the lines to .ent file*/
                    fclose(entOut);
                    tempName[strlen(tempName) - 4] = '\0';
                }

                if (existsExtFile ()) {
                    strcat(tempName, ".ext");
                    extOut = fopen(tempName, "w+");
                    if (!extOut) {
                        fprintf(stderr, MSG_FILE_WRITE_EXT_ERR);
                        exit(0);
                    }
                    printExtFile(&extOut);/*Adds the lines to .ext file*/
                    fclose(extOut);
                    tempName[strlen(tempName) - 4] = '\0';
                }
            }
        }
        fclose(file);
        cleanCommandWords(); /*Clean the command word*/
        cleanDataList(); /*Clean the data words*/
        cleanSymbols(); /*Clean the symbol list*/
        clearOtherFiles(); /*Clean the .ent and .ext lines*/
        free(line);
        fileCounter++;
    }/*end while of read args*/
    return 0;
}

