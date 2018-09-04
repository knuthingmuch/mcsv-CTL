#pragma once

//--------------------------------------------------
#include <stdlib.h>
#include <stdbool.h>
//--------------------------------------------------
#define MAX_VAR_LEN (5+1) //maximum number of characters per variable name(Actual +1 for null terminator.)
#define MAX_OP_LEN (2+1)
//--------------------------------------------------
#define NUM_OPS 13 //total number operators defined below.
//--------------------------------------------------
#define OP_OR "+"
#define OP_AND "."
#define OP_XOR "^"
#define OP_NOT "!"
#define OP_IFF "="
//--------------------------------------------------
#define OP_EX "EX"
#define OP_EF "EF"
#define OP_EG "EG"
#define OP_EU "EU"
#define OP_AX "AX"
#define OP_AF "AF"
#define OP_AG "AG"
#define OP_AU "AU"
//--------------------------------------------------

char** tokenize(char* string, size_t* countPtr);

char** getUniqVars(char** tokens, size_t numTokens, size_t* numUniqVar);
