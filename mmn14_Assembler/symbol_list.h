/* ============================================================================
 Name        : Maman 14
 Author      : Adam Yodinsky 200359776 & Noam Carmon 212173660
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


typedef struct {
    char *name;
    char isFunction;
    char isExtern;
    int address;
    char isEntry;
} SymbolT;
typedef struct symbolNode *symbolPtr;

typedef struct symbolNode {
    symbolPtr *leftS;
    symbolPtr *rightS;
    SymbolT *symbolData;
} labelNodeT;

void addSymbol(char *, char, char, int, void (*funcPtr)(char *));
symbolPtr searchSymbolByName(char *);
void addSortedSymbol(symbolPtr, void (*funcPtr)(char *));

void recalculationDataAddress(int, void (*funcPtr)(char *));
void recalculationDataAddressr(symbolPtr, int, void (*funcPtr)(char *));

void print_Symbolsr(symbolPtr root);
void print_Symbols();

void cleanSymbolsr(symbolPtr);
void cleanSymbols();


