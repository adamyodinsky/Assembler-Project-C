/* ============================================================================
 Name        : Maman 14
 Author      : Adam 
 Version     : 24/03/18
 Copyright   :
 Description : assembler
 ============================================================================ */


#define MSG_ALLOC_ERR "ERROR - Can't allocate memory\n"
#define MSG_ARGS_MISSING "No file path was passed\n"
#define MSG_MUST_AS_FILE "%s name is not valid input, file must ends with .as\n"
#define MSG_FILE_OPEN_ERR "File %s is not exist or can't be opened!\n"
#define MSG_FILE_WRITE_ENT_ERR "Cannot write an .ent file\n"
#define MSG_FILE_WRITE_OBJ_ERR "Cannot write an .ob file\n"
#define MSG_FILE_WRITE_EXT_ERR "Cannot write an .ext file\n"


#define MSG_LABEL_UNDEFINED "undefined label"
#define MSG_LABEL_WRONG_POS "label does not start in first column"
#define MSG_LABEL_DEFINITION_ERR "label can't be defined without data or operation"
#define MSG_LABEL_ALREADY_EXT "label already set as external"
#define MSG_LABEL_ALREADY_INT "local label cannot be declared as external"
#define MSG_LABEL_ERR_LENGTH "label can't be longer than 30 chars"
#define MSG_LABEL_ERR_START "Label can't starts with digit"
#define MSG_LABEL_ERR_SPACE "Label can't use white spaces"
#define MSG_LABEL_ERR_REG_NAME "Label can't use register name"
#define MSG_LABEL_ERR_FUNC_NAME "Label can't use function name"
#define MSG_LABEL_ERR_INST_NAME "Label can't use instruction name"
#define MSG_LABEL_ERR_CHARS_RANGE "Label can't contains chars other then 0-9,a-z,A-Z"


#define MSG_OPERATION_INVALID "invalid command or instruction"
#define MSG_WRONG_TYPE_PARAM "wrong type of parameter"
#define MSG_NO_PARAM_NEEDED "no parameters is needed"
#define MSG_PARAM_PASSED "No parameters passed"

#define MSG_MISSING_PARAM "missing argument in directive"
#define MSG_MISSING_ARGS_DIRECTIVE "missing argument in directive"
#define MSG_STRING_INVALID_QUOTATIONS "Invalid string, missing \""
#define MSG_STRING_INVALID_EXTRA_TEXT "invalid string, extra text"

#define MSG_PARSE_EXTRA_TEXT "excessive text"
#define MSG_PARSE_DUBBLE_COMMA "multiple consecutive commas"
#define MSG_PARSE_MISSING_PARAM "missing Parameter"
#define MSG_PARSE_MISSING_LAST_PARAM "missing last param"

#define MSG_STRUCT_EXTRA_TEXT "invalid struct format, extra text"
#define MSG_STRUCT_WRONG_FORMAT "wrong struct format"
#define MSG_STRUCT_MISSING_NAME "Missing struct name"
#define MSG_STRUCT_INVALID_NUM "number is not valid"
#define MSG_STRUCT_INVALID_STR "wrong struct format, missing valid string"

#define MSG_DATA_TOO_MANY "too many data items"

#define MSG_DUPLICATE_SYMBOL "duplicate symbol"

#define MSG_DATA_RANGE_ERR "invalid Data number, must be in range of 10 bits"
#define MSG_IMM_RANGE_ERR "invalid immediate number, must be in range of 8 bits"


#define MSG_SYMBOL_NOT_FOUND "Symbol %s not found"
#define MSG_MISSING_LABEL "Missing definition of label"

