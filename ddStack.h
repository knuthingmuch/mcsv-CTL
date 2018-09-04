#pragma once
//--------------------------
#include <stdio.h> //for size_t, printf, NULL etc.
#include <stdbool.h> 
//------ CUDD includes -----
//include these here so that a user of ddStack can simply include this file and be on their way without needing to include cudd stuff.
#include <cudd.h>
#include <util.h>
#include "tokenisation.h" //for MAX_VAR_LEN
//--------------------------

typedef struct ddStack
  {
    size_t top;
    size_t capacity;
    DdNode* stack[];
  } ddStack;

void ddStack_init(ddStack* d, size_t capacity);

bool ddStack_push(ddStack* d, DdNode* node);

DdNode* ddStack_pop(ddStack* d);

bool ddStack_isEmpty(ddStack* d); 

//-------------------------------

typedef struct augmentedDdVar
{
  bool parity; //true if prime.
  char name[MAX_VAR_LEN];
  DdNode* node;
} augmentedDdVar;
