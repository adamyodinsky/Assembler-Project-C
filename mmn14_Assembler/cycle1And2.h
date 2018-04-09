/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#define ZERO_PARAMETER 0
#define ONE_PARAMETER 1
#define TWO_PARAMETER 2

extern int instCounter;
extern char cycle1Status;
typedef enum { false, true } bool;

typedef struct {
    char *name;
    char code;

} regT;

typedef struct {
    char *cmd;
    int code;
    void (*ptrFunct)(char *, char *);
} cmdT;

typedef struct {
    char *type;
    void (*ptrFunct)(char *, char *);
} instructT;

regT *findRegByName(char *);
cmdT *findFuncByName(char *);
instructT *findInstByName(char *);

void firstAnalyze(char*, int, char *);
void callFunc(char*, char*, char *);

void funcNoOper(char *, char *);
void funcOnlyDestOper(char *, char *);
void funcBothOper(char *, char *);

void addData(char *, char *);
void addStr(char *, char *);
void addStruct(char *, char *);
void addExt(char *, char *);
void addEnt(char *, char *);

char parseCommand(char *, char **, int);
char ParseDataToList(char *, char *);
char checkParamsMemSize(int, int);
char checkParamType(char *);char checkValidStructOrLabel(char *);
char checkValidLabel(char *);

void recalculateSymbolAddress();
void printHandler(char*);
void secondAnalyze(char* line, int lineNum, char *fileName);
void initParams();

