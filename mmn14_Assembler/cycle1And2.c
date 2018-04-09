/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cycle1And2.h"
#include "MSG.h"
#include "utils.h"
#include "word_list.h"
#include "data_list.h"
#include "symbol_list.h"

static char *currentLine;
static char *currentFile;
static int lineNumber;
int pType1 = eUndef, pType2 = eUndef;
char p1Symbol[MAX_LABEL_LENGTH + 1];
char p2Symbol[MAX_LABEL_LENGTH + 1];
char cycle1Status = 1;
int instCounter = IC_OFFSET;
char *args[TWO_PARAMETER];
int cycleNum = 1;

static regT regs[] = { 	{"r0", 0 },
                        {"r1", 1 },
                        {"r2", 2 },
                        {"r3", 3 },
                        {"r4", 4 },
                        {"r5", 5 },
                        {"r6", 6 },
                        {"r7", 7 },
                        { NULL, 0 } };

static instructT instructions[] = { {".data", addData },
                                    {".string", addStr },
                                    {".struct", addStruct },
                                    {".entry", addEnt },
                                    {".extern", addExt },
                                    { NULL, NULL } };

cmdT commands[] = { {"mov", 0, funcBothOper },
                    {"cmp", 1, funcBothOper },
                    {"add", 2, funcBothOper },
                    {"sub", 3, funcBothOper },
                    {"not", 4, funcOnlyDestOper },
                    {"clr", 5, funcOnlyDestOper },
                    {"lea", 6, funcBothOper },
                    {"inc", 7, funcOnlyDestOper },
                    {"dec", 8, funcOnlyDestOper },
                    {"jmp", 9, funcOnlyDestOper },
                    {"bne", 10,funcOnlyDestOper },
                    {"red", 11,funcOnlyDestOper },
                    {"prn", 12,funcOnlyDestOper },
                    {"jsr", 13,funcOnlyDestOper },
                    {"rts", 14,funcNoOper },
                    {"stop", 15, funcNoOper },
                    { NULL, 0, NULL } };


void firstAnalyze(char* line, int lineNum, char *fileName) {
    /*
     * Analyze which operation should be done
     * This method split the input line for later analyze
     */

    char *label = NULL;
    char *command = NULL;
    char *params = NULL;
    char *beforeTrimLeft = NULL;
    char trimmed = 0;
    initParams();
    cycleNum = 1;
    beforeTrimLeft = line;
    line = trimLeft(line);
    if (beforeTrimLeft != line)
        trimmed = 1;
    line = trimRight(line);
    lineNumber = lineNum;
    if (!line) {
        return;
    }
    /*cpy line to private memory*/
    currentLine = (char*) calloc(strlen(line) + 1, sizeof(char));
    if (!currentLine) {
        fprintf( stderr, MSG_ALLOC_ERR);
        exit(1);
    }
    strcpy(currentLine, line);
    /*check if line is a comment */
    if (line[0] == ';') {
        return;
    }

    /*parse line to get all sub operation*/
    label = my_strtok(line, ":");
    label = trimRight(label);
    if (label) {
        if (trimmed) {
            printHandler(MSG_LABEL_WRONG_POS);
            return;
        } else if (strlen(label) && !checkValidLabel(label)) {
            free(currentLine);
            return;
        }

    }

    command = my_strtok(NULL, " \t");
    /*if command is single word without params we don't expect any chars after single command*/
    if (!command) {
        command = my_strtok(NULL, "\0");
        params = NULL;
    } else {
        params = my_strtok(NULL, "\0");
    }
    currentFile = fileName;
    callFunc(trimLeft(command), trimLeft(params), label);
    free(currentLine);
}

void secondAnalyze(char* line, int lineNum, char *fileName) {
    /*
     * Performs the second check of the algorithm, check that all labels defined and correct
     * and fill the additional info in the data/symbol list
     */

    char *label;
    char *command;
    char *params;
    instructT *instTemp = NULL;
    lineNumber = lineNum;
    initParams();

    line = trimLeft(line);
    line = trimRight(line);
    cycleNum = 2;
    if (!line) {
        return;
    }
    /*check if line is a comment */
    if (line[0] == ';') {
        return;
    }

    /*parse line to get all sub operation*/
    label = my_strtok(line, ":");
    label = trimRight(label);
    if (label) {
        line = line + strlen(label) + 1;
        line = trimLeft(line);
    }
    /*cpy line to private memory*/
    currentLine = (char*) calloc(strlen(line) + 1, sizeof(char));
    if (!currentLine) {
        fprintf( stderr, MSG_ALLOC_ERR);
        exit(1);
    }
    strcpy(currentLine, line);

    command = my_strtok(NULL, " \t");
    /*if command is single word without params we don't expect any chars after single command*/
    if (!command) {
        command = my_strtok(NULL, "\0");
        params = NULL;
    } else {
        params = my_strtok(NULL, "\0");
    }
    /*check if command is an instruction*/
    if ((instTemp = findInstByName(command))) {
        if (strcmp(command, ".entry") == 0) {
            symbolPtr temp = NULL;
            temp = searchSymbolByName(params);
            if (temp) {
                temp->symbolData->isEntry = 1;
                addToEntFile(params,
                        searchSymbolByName(params)->symbolData->address);
            } else {
                printHandler(MSG_LABEL_UNDEFINED);
            }
        }
        /* Do nothing in cycle2 if it's directives */
        return;
    } else {
        currentFile = fileName;
        callFunc(trimLeft(command), trimLeft(params), label);
    }
    free(currentLine);
}

void callFunc(char *cmdName, char *params, char *label) {
    /*
     * This method checks which command should be run and call to the specific method
     * to handle that command
     */
    cmdT *temp = NULL;
    instructT *instTemp = NULL;
    if (cycleNum == 1 && !cmdName && label) {
        printHandler(MSG_LABEL_DEFINITION_ERR);
        return;
    }
    if (cycleNum == 1 && !cmdName) {
        /*no cmd passed*/
        return;
    }
    if ((temp = findFuncByName(cmdName))) {
        if (cycleNum == 1 && label) {
            /* TODO:add label with function*/
            addSymbol(label, 0, 1, instCounter, printHandler);
        }
        /* call to specific function */
        (*temp->ptrFunct)(cmdName, params);

    } else if (cycleNum == 1) {
        /*check if cmdName is an instruction*/
        if ((instTemp = findInstByName(cmdName))) {
            /* call to specific function */
            (*instTemp->ptrFunct)(label, params);
        } else {
            printHandler(MSG_OPERATION_INVALID);
        }
    }
    return;
}

void funcOnlyDestOper(char *funcName, char *params) {
    /*
     * Checks the correction of command syntax with only destination operand
     */
    /* Parsing Params */
    if (!parseCommand(params, args, ONE_PARAMETER)) {
        /*parse is not correct */
        return;
    }
    pType2 = checkParamType(args[0]);
    args[1] = args[0];
    addCommandWord(cycleNum, args[0], args[1], pType1, pType2, instCounter,
            funcName, params);
    instCounter++;
    if (pType2 < eImm) {
        return;
    } else if (pType2 == eImm) {
        if (strcmp(funcName, "prn") == 0) {
            instCounter += checkParamsMemSize(pType2, eError);
        } else {
            printHandler(MSG_WRONG_TYPE_PARAM);
            return;
        }
    } else {
        instCounter += checkParamsMemSize(pType2, eError);
    }
    return;
}

void funcBothOper(char *funcName, char * params)

{
    /*
     * Checks the correction of command syntax with 2 operands
    */

    /* Parsing Params */
    if (!parseCommand(params, args, TWO_PARAMETER)) {
        /*parse is not correct */
        return;
    }
    pType1 = checkParamType(args[0]); /*Source operand */
    pType2 = checkParamType(args[1]); /*dest operand */
    addCommandWord(cycleNum, args[0], args[1], pType1, pType2, instCounter,
            funcName, params);
    instCounter++;
    if (pType1 <= eError || pType2 <= eError) {
        return;
    } else if ((pType1 == eImm || pType1 == eReg)
            && strcmp(funcName, "lea") == 0) {
        printHandler(MSG_WRONG_TYPE_PARAM);
        return;
    } else if (pType2 == eImm && strcmp(funcName, "cmp") != 0) {
        printHandler(MSG_WRONG_TYPE_PARAM);
        return;
    }
    instCounter += checkParamsMemSize(pType1, pType2);
    return;
}


void funcNoOper(char *funcName, char *params) {
    /*
     * Checks the correction of command syntax without operands
    */
    if (params) {
        params = trimLeft(params);
        params = trimRight(params);
        /* expect parameters to be null */
        if (strlen(params)) {
            printHandler(MSG_NO_PARAM_NEEDED);
            return;
        }
    }
    addCommandWord(cycleNum, args[0], args[1], pType1, pType2, instCounter,
            funcName, params);
    instCounter++;
}

void addData(char *label, char *params) {
    /*
     * handle .data instruction
     * stores data value in the data list
     */
    params = trimLeft(params);
    params = trimRight(params);
    if (label) {
        addSymbol(label, 0, 0, 0, printHandler);
    }
    if (params) {
        ParseDataToList(label, params);
    } else {
        printHandler(MSG_MISSING_ARGS_DIRECTIVE);
    }
}
void addStr(char *label, char *params) {
    /*
     * handle .string instruction
     * stores string value in the data list
     */
    char *token = NULL;
    int i = 0;
    params = trimLeft(params);
    params = trimRight(params);
    if (label) {
        addSymbol(label, 0, 0, 0, printHandler);
    }
    if (!params) {
        printHandler(MSG_MISSING_ARGS_DIRECTIVE);
    }
    token = my_strtok(params, "\"");
    token = my_strtok(NULL, "\"");
    if (!token) {
        printHandler(MSG_STRING_INVALID_QUOTATIONS);
    } else {
        while (token[i]) {
            addDataToList(label, (int) token[i]);
            i++;
        }
        addDataToList(label, 0);
    }
    token = my_strtok(NULL, "\0");
    if (token) {
        printHandler(MSG_STRING_INVALID_EXTRA_TEXT);
    }
}


void addStruct(char *label, char *params) {
    /*
     * handle .struct instruction
     * stores struct values in the data lists
     */

    char *str_parm = NULL; /* stores string parameter*/
    char *num_parm = NULL; /* stores numeric parameter */
    char *str_valid = NULL; /*stores valid string*/
    int num_valid = 0;      /*store valid number*/
    int i = 0;
    bool str_is_ok = true;
    if (label)
        addSymbol(label, 0, 0, 0, printHandler);

    /* isolate parms */
    num_parm = strtok(params, ",");
    str_parm = strtok(NULL, "\0");
    /*trim white spaces from parameters*/
    num_parm = trimLeft(num_parm);
    num_parm = trimRight(num_parm);
    str_parm = trimLeft(str_parm);
    str_parm = trimRight(str_parm);

    /* check if number parameter is well formated and valid*/
    if(!num_parm){
        printHandler(MSG_PARSE_MISSING_PARAM);
        return;
    }
    if(num_parm[i] == '-' || num_parm[i] == '+') /*if signed*/
        i++;
    while (num_parm[i]){
            if(!isDigit(num_parm[i])){
                printHandler(MSG_STRUCT_INVALID_NUM);
               return;
            }
            i++;
    }
    /*convert to int and save as a valid number*/
    num_valid = atoi(num_parm);

    /*check if string is valid*/
    if(!str_parm){
        printHandler(MSG_PARSE_MISSING_PARAM);
        return;
    }

    i=0;
    if(str_parm[i] == '"')
        i++;
    else
        str_is_ok = false;

    while(str_parm[i]){ /*go to last char*/
        if(i>0 && i<strlen(str_parm)-1)
            if(str_parm[i] == '"')
                str_is_ok=false;
        i++;
    }
    i--;

    if(str_parm[i] != '"')
        str_is_ok = false;

    if(!str_is_ok){
        printHandler(MSG_STRUCT_INVALID_STR);
        return;
    }

    str_valid = my_strtok(str_parm, "\"");
    str_valid = my_strtok(NULL, "\"");

    /*add data into list*/
    i=0;
    addDataToList(label, num_valid); /*add num*/
    while(str_valid[i]){ /*add string*/
        addDataToList(label, str_valid[i]);
        i++;
    }
    addDataToList(label, 0); /*add end of string*/
}

void addEnt(char *label, char *params) {
    /*
     * handle .ent instruction
     * stores ent values in the symbol list
     */
    symbolPtr temp = NULL;

    params = trimRight(params);
    params = trimLeft(params);
    if (!params) {
        printHandler(MSG_MISSING_ARGS_DIRECTIVE);
        return;
    }
    temp = searchSymbolByName(params);
    if (temp && temp->symbolData->isExtern == 1) {
        printHandler(MSG_LABEL_ALREADY_EXT);
        return;

    } else {
        checkValidLabel(params);
        return;
    }

}

void addExt(char *label, char *params) {
    /*
     * handle .est instruction
     * stores ext values in the symbol list
     */
    if (checkValidLabel(params)) {
        symbolPtr temp;
        temp = searchSymbolByName(params);
        if (temp && temp->symbolData->isExtern == 0) {
            printHandler(MSG_LABEL_ALREADY_INT);
            return;
        }
        addSymbol(params, 1, 0, 0, printHandler);
    } else {
        /*invalid label*/
        cycle1Status = 0;
    }
}

regT *findRegByName(char *c) {
    /*
     * Help function to check if str is register name
     */
    regT *reg;
    if (!c) {
        return NULL;
    }
    c = trimLeft(trimRight(c));
    for (reg = regs; reg->name && strcmp(reg->name, c); reg++);
    if (reg->name) {
        return reg;
    } else {
        return NULL;
    }
}

cmdT *findFuncByName(char *c) {
    /*
     * Help function to check if c is command name
     */
    cmdT *temp;
    if (!c) {
        return NULL;
    }
    c = trimLeft(trimRight(c));
    for (temp = commands; temp->cmd && strcmp(temp->cmd, c); temp++);
    if (temp->cmd) {
        return temp;;
    } else {
        return NULL;
    }
}

instructT *findInstByName(char *c) {
    /*
     * Help function to check if str is instruction name
     */
    instructT *instTemp;
    if (!c) {
        return NULL;
    }
    c = trimLeft(trimRight(c));
    for (instTemp = instructions; instTemp->type && strcmp(instTemp->type, c);
            instTemp++);
    if (instTemp->type) {
        return instTemp;;
    } else {
        return NULL;
    }
}

char parseCommand(char *params, char **args, int numOfParams) {
    /* 	Parsing params into args array
     look for , first, if I don't find it, then look for \n
     and treat it as last parameters.
     */

    int index = 0;
    int startIndex = 0;
    int argsIndex = 0;
    int commaCounter = 0;
    char *token = NULL;
    char *tempParams;
    /*indicate if last check found comma*/
    char lastComma = 0;

    /*if params are null then don't look for ,*/
    if (!params) {
        if (!numOfParams) {
            return 1;
        } else {
            printHandler(MSG_PARAM_PASSED);
            return 0;
        }
    }

    tempParams = (char *) malloc((strlen(params) + 1) * sizeof(char));
    if (!tempParams) {
        printHandler(MSG_ALLOC_ERR);
        exit(1);
    }

    memcpy(tempParams, params, (strlen(params) + 1) * sizeof(char));

    while (tempParams[index]) {
        if (tempParams[index] == ',') {
            if (argsIndex > (numOfParams - 1)) {
                printHandler(MSG_PARSE_EXTRA_TEXT);
                return 0;
            }
            if (lastComma) {
                printHandler(MSG_PARSE_DUBBLE_COMMA);
                return 0;
            }
            lastComma = 1;

            tempParams[index] = '\0';
            commaCounter++;

            args[argsIndex] = &tempParams[startIndex];

            args[argsIndex] = trimLeft(args[argsIndex]);
            args[argsIndex] = trimRight(args[argsIndex]);

            argsIndex++;
            startIndex = index + 1;
            index++;
            continue;
        }
        lastComma = 0;
        index++;
    }

    /*add last parameter*/
    if (commaCounter < numOfParams - 1) {
        printHandler(MSG_PARSE_MISSING_PARAM);
        free(tempParams);
        return 0;
    }

    if (startIndex < index) {
        if (argsIndex + 1 > numOfParams) {
            printHandler(MSG_PARSE_EXTRA_TEXT);
            return 0;
        }
        args[argsIndex] = &tempParams[startIndex];

        args[argsIndex] = trimLeft(args[argsIndex]);
        args[argsIndex] = trimRight(args[argsIndex]);

        if (lastComma) {
            printHandler("MSG_PARSE_DUBBLE_COMMA");
            return 0;
        }
        /* if there is text append by space or tab to last arg */
        token = my_strtok(args[argsIndex], " \t");

        if (token) {
            printHandler(MSG_PARSE_EXTRA_TEXT);
            return 0;
        }
    } else {
        if (argsIndex < numOfParams) {
            printHandler(MSG_PARSE_MISSING_PARAM);
        } else {
            printHandler(MSG_PARSE_EXTRA_TEXT);
        }
        return 0;

    }
    return 1;
}

char ParseDataToList(char *label, char *params) {
    /* 	Parsing params into args array
     look for , first, if I don't find it, then look for \n
     and treat it as last parameters.
     */

    int index = 0;
    int startIndex = 0;
    int argsNum = 0;
    char lastComma = 0;
    char *tempParams;
    int tempVal = 0;

    /*if params are null then don't look for ,*/
    if (!params) {
        return 1;
    }
    /*copy params to new memory, we don't want to damage original memory */
    tempParams = (char *) malloc(strlen(params) * sizeof(char));
    if (!tempParams) {
        printHandler(MSG_ALLOC_ERR);
        exit(1);
    }

    strcpy(tempParams, params);

    while (tempParams[index]) {
        if (tempParams[index] == ',') {
            if (lastComma) {
                printHandler(MSG_MISSING_PARAM);
                return 0;
            }
            tempParams[index] = '\0';

            if (!checkValidNumber(&tempParams[startIndex], 1, printHandler)) {
                return 0;
            }
            tempVal = atoi(&tempParams[startIndex]);
            addDataToList(label, tempVal);

            argsNum++;
            startIndex = index + 1;
            lastComma = 1;
            index++;
            continue;
        }
        index++;
        lastComma = 0;
    }
    if (lastComma) {
        printHandler(MSG_PARSE_MISSING_LAST_PARAM);
        return 0;
    }
    if (startIndex < index) {

        if (!checkValidNumber(&tempParams[startIndex], 1, printHandler)) {
            return 0;
        }
        addDataToList(label, atoi(&tempParams[startIndex]));
        argsNum++;
    }

    return argsNum;
}

char checkParamsMemSize(int paramT1, int paramT2) {
    /*
     * Help function which calculates memory size which current command needs to be stored in.
     */
    char res = 0;

    if (paramT1 == eReg && paramT2 == eReg) {
        return 1;
    }
    if (paramT1 == eStruct) {
        res += 2;
    } else {
        if (paramT1 > eError) {
            res += 1;
        }
    }
    if (paramT2 == eStruct) {
        res += 2;
    } else {
        if (paramT2 > eError) {
            res += 1;
        }
    }

    return res;
}

char checkParamType(char *param) {
    /*
     * Help function which checks the type of param
     */
    char temp = 0;
    if (!param) {
        return eError;
    }
    if (param[0] == '#') {
        if (checkValidNumber(++param, 0, printHandler)) {
            return eImm;
        } else {
            return eError;
        }
    }
    if (findRegByName(param)) {
        return eReg;
    }
    /*temp store params type, which is bigger than eImm*/
    temp = checkValidStructOrLabel(param);
    if (temp > eImm) {
        return temp;
    }

    return eError;
}

char checkValidLabel(char *str) {
    /*
     * Help function which checks if params is valid Label
     */
    int i = 0;
    int length;
    if (!str) {
        return 0;
    } else {
        str = trimRight(str);
        if (strlen(str) > 30) {
            printHandler(MSG_LABEL_ERR_LENGTH);
            return 0;
        }
        if (isDigit(str[0])) {
            printHandler(MSG_LABEL_ERR_START);
            return 0;
        }
        /*check white space inside label*/
        if (strstr(str, " ") || strstr(str, "\t")) {
            printHandler(MSG_LABEL_ERR_SPACE);
            return 0;
        }
        /* check if label using reserved words*/
        if (findRegByName(str)) {
            printHandler(MSG_LABEL_ERR_REG_NAME);
            return 0;
        }
        if (findFuncByName(str)) {
            printHandler(MSG_LABEL_ERR_FUNC_NAME);
            return 0;
        }
        if (findInstByName(str)) {
            printHandler(MSG_LABEL_ERR_INST_NAME);
            return 0;
        }
        length = (int) strlen(str);
        while (i < length) {
            /*check if str include only a-z,9-0,A-Z */
            if (!((str[i] <= 'z' && str[i] >= 'a')
                    || (str[i] <= 'Z' && str[i] >= 'A')
                    || (str[i] <= '9' && str[i] >= '0'))) {
                printHandler(MSG_LABEL_ERR_CHARS_RANGE);
                return 0;
            }
            i++;
        }
    }
    return 1;
}

char checkValidStructOrLabel(char *str){
    /*
     * Helper function that checks if params is valid Label or Struct
	*/
    char *structName;
    char *fieldNum;
    char *originalStr;

    originalStr =  (char *) malloc(strlen(str) * sizeof(char));
    strcpy(originalStr,str);

    structName = trimRight(str);
    /* if there is a dot (.) then it probably not a label, then needs to check if its a struct*/
    structName = my_strtok(str, ".");


    if (structName && !strcmp(structName, "")) {
        /*empty struct name*/
        printHandler(MSG_STRUCT_MISSING_NAME);
        return 0;
    }

    /*didn't find a dot, so we check if it's legal struct*/
    if (!structName) {
        if (checkValidLabel(str)) {
            return eDirect;
        } else {
            return 0;
        }
    }
    /*check if structName is valid label*/
    if (!checkValidLabel(structName)) {
        return 0;
    }

    fieldNum = my_reverse_strtok(originalStr,".");
    if (!(fieldNum[0]==49|| fieldNum[0]==50)) {
        printHandler(MSG_STRUCT_INVALID_NUM);
        return 0;
    }
    /* check if there is an extra chars after struct field number*/
    if (fieldNum[1]) {
            printHandler(MSG_STRUCT_INVALID_NUM);
            return 0;
        }

    return eStruct;
}

void recalculateSymbolAddress() {
    /*increase IC to be next address for dataSegment*/
    recalculationDataAddress(instCounter, printHandler);
}

void printHandler(char* str) {
    /*
     * Help function which prints all errors in specific format
     */
    fprintf(stderr, "%s:%d:'%s' - %s\n", currentFile, lineNumber, currentLine,
            str);
    cycle1Status = 0;
}

void initParams() {
    /*
     * Initialize global variables.
     */
    pType1 = eUndef;
    pType2 = eUndef;
    p1Symbol[0] = '\0';
    p2Symbol[0] = '\0';
    args[0] = NULL;
    args[1] = NULL;
}


