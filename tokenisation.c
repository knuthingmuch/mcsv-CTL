//------------------tokenisation.c------------------
#include <stdio.h> //for size_t, printf, NULL etc.
#include <gc.h>
#include <string.h>
#include "tokenisation.h"
//----------------- Local Declares --------------------
bool isOperator(char* token);
//-----------------------------------------------------

char** getUniqVars(char** tokens, size_t numTokens, size_t* numUniqVar) {

  //--------------------------------------------------
  char** seenVars = GC_MALLOC(numTokens*sizeof(char*));   //Allocate on heap to allow large formulae.
  for(size_t i = 0; i < numTokens; i++)
    seenVars[i] = GC_MALLOC(MAX_VAR_LEN*sizeof(char));
  //--------------------------------------------------

  size_t currentVarIndex = 0;

  for(size_t i = 0; i < numTokens; i++) {
    size_t j;
    bool exists = false;

    if( ! isOperator(tokens[i]) ) { //if not an operator
      for(j = 0; j < currentVarIndex; j++) {
        if(strncmp(seenVars[j], tokens[i], MAX_VAR_LEN) == 0) {
          exists = true;
          break;
        }
      }
      if(!exists){
        strncpy(seenVars[currentVarIndex], tokens[i], MAX_VAR_LEN);
        currentVarIndex++;
      }
    }
  }
  *numUniqVar = currentVarIndex;

  // printf("currentVarIndex: %li\n", currentVarIndex);

  char** uniqVars = GC_MALLOC(*numUniqVar*sizeof(char*));

  for(size_t i = 0; i < *numUniqVar; i++) {
    uniqVars[i] = GC_MALLOC(MAX_VAR_LEN*sizeof(char));
    strncpy(uniqVars[i], seenVars[i], MAX_VAR_LEN);
  }

  // *numUniqVar = currentVarIndex; //set earlier
  return uniqVars;
}

char** tokenize(char* extString, size_t* countPtr) {
  size_t count = 0;
  bool outside = true;

  char string[strlen(extString) +1];
  strcpy(string, extString);    //strtok modifies string, make copy to avoid surprises in caller.

  for( char* chr = string; *chr; *chr++) {  //count tokens beforehand to allocate m
    if(outside && *chr != ' ') {
      count++;
      outside = false;
    }
    else if(!outside && *chr == ' ') {
      outside = true;
    }
  }
 
  char** tokens = GC_MALLOC(sizeof(char*)*count);
  
  char* token = strtok(string, " ");

  for(size_t i = 0; i < count; i++) {

    if( !(strlen(token)< MAX_VAR_LEN) ) {
      printf("Too long symbol: %s\n Exiting...\n", token);
      exit(2);
    }
    tokens[i] = GC_MALLOC(sizeof(char)*MAX_VAR_LEN);
    strncpy(tokens[i], token, MAX_VAR_LEN); //'\0' is guranteed to be within MAX_VAR_LEN bytes.
    token = strtok(NULL, " ");
  }

  *countPtr = count; //set value of count in caller.
  return tokens;
}

bool isOperator(char* token) {
  char operators[NUM_OPS][MAX_OP_LEN] = { OP_OR, OP_AND, OP_XOR, OP_NOT, OP_IFF, OP_EX, OP_EF, OP_EG, OP_EU, OP_AX, OP_AF, OP_AG, OP_AU };

  for (int i = 0; i < NUM_OPS; ++i)
  {
    if( strncmp(token, operators[i], MAX_OP_LEN) == 0 )
      return true;
  }
  return false;
}