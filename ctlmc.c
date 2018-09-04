#include <stdio.h> //for size_t, printf, NULL etc.
#include <string.h>
#include <stdbool.h>
#include <gc.h>  //Boehem GC
//------ CUDD includes -----
#include <cudd.h>
#include <util.h>
//----- Includes for local libs -----
#include "ddStack.h"
#include "tokenisation.h"
//-----------------------------------
#include "aux.c"

//----Declarations-----
DdNode* check(char* propForm, modelContext context);

DdNode* EX(DdNode* propStates, modelContext context);
DdNode* AX(DdNode* propStates, modelContext context);
DdNode* EF(DdNode* propStates, modelContext context);
DdNode* AF(DdNode* propStates, modelContext context);
DdNode* EG(DdNode* propStates, modelContext context);
DdNode* AG(DdNode* propStates, modelContext context);
DdNode* EU(DdNode* beforeStates, DdNode* finallyStates, modelContext context);
DdNode* AU(DdNode* beforeStates, DdNode* finallyStates, modelContext context);
//------- GLOBALS (for backtracking) ------- //not implemented.
char* _RECENT_CTLPROP; //points to last checked CTL property token.
DdNode* _EGPROP; //points to most recent property checked with EG.
//------------------------------------------

DdNode* check(char* propForm, modelContext context) {
  /**
  *@brief Returns BDD for all property satifying states. (Regradless of reachability.)
  *
  */
  //------------------- Tokenise an prepare property formula -------------------
  size_t propTokCount;
  char** propTokens = tokenize(propForm, &propTokCount);

  // size_t old_numExistingVars = context.numUnprimedNodes;

  size_t numPropUniqVars;
  char** propUniqVars = getUniqVars(propTokens, propTokCount, &numPropUniqVars);

  
  //-------------------- Build BDD from property. Bottom-up. --------------------
  size_t currentVariableIndex = context.numUnprimedNodes;

  ddStack* stack = GC_MALLOC(sizeof(ddStack)+sizeof(DdNode*)*(propTokCount));
  ddStack_init(stack, propTokCount); 

  DdNode* recentNode; //always point to most recent node.

  for(size_t i = 0; i < propTokCount; i++) {

    //------ Check for primed ------
    if ( propTokens[i][strlen(propTokens[i])-1] == '\'') {  //Complain
      printf("Can not have primed variables in CTL formula. Quitting...\n");
      exit(3);
    }
    //------------------------------

    if( strncmp(propTokens[i], OP_OR, MAX_OP_LEN) == 0) {         //OR
      DdNode* a = ddStack_pop(stack);
      DdNode* b = ddStack_pop(stack);
      recentNode = Cudd_bddOr(context.manager, a, b);
    }
    else if ( strncmp(propTokens[i], OP_AND, MAX_OP_LEN) == 0 ) {  //AND
      DdNode* a = ddStack_pop(stack);
      DdNode* b = ddStack_pop(stack);
      recentNode = Cudd_bddAnd(context.manager, a, b);
    }
    else if ( strncmp(propTokens[i], OP_XOR, MAX_OP_LEN) == 0) {   //XOR
      DdNode* a = ddStack_pop(stack);
      DdNode* b = ddStack_pop(stack);
      recentNode = Cudd_bddXor(context.manager, a, b);
    }
    else if ( strncmp(propTokens[i], OP_IFF, MAX_OP_LEN) == 0) {   //IFF
      DdNode* a = ddStack_pop(stack);
      DdNode* b = ddStack_pop(stack);
      recentNode = Cudd_Not(Cudd_bddXor(context.manager, a, b));
    }
    else if ( strncmp(propTokens[i], OP_NOT, MAX_OP_LEN) == 0 ) {  //NOT
      DdNode* a = ddStack_pop(stack);
      recentNode = Cudd_Not(a);
    }
    else if ( strncmp(propTokens[i], OP_EX, MAX_OP_LEN) == 0  ) {  //EX
      _RECENT_CTLPROP = propTokens[i];
      DdNode* a = ddStack_pop(stack);
      recentNode = EX(a, context);
    }
    else if ( strncmp(propTokens[i], OP_AX, MAX_OP_LEN) == 0  ) {  //AX
      _RECENT_CTLPROP = propTokens[i];
      DdNode* a = ddStack_pop(stack);
      recentNode = AX(a, context);
    }
    else if ( strncmp(propTokens[i], OP_EF, MAX_OP_LEN) == 0  ) {  //EF
      _RECENT_CTLPROP = propTokens[i];
      DdNode* a = ddStack_pop(stack);
      recentNode = EF(a, context);
    }
    else if ( strncmp(propTokens[i], OP_AF, MAX_OP_LEN) == 0  ) {  //AF
      _RECENT_CTLPROP = propTokens[i];
      DdNode* a = ddStack_pop(stack);
      recentNode = AF(a, context);
    }
    else if ( strncmp(propTokens[i], OP_EG, MAX_OP_LEN) == 0  ) {  //EG
      _RECENT_CTLPROP = propTokens[i];
      DdNode* a = ddStack_pop(stack);
      _EGPROP = a;
      recentNode = EG(a, context);
    }
    else if ( strncmp(propTokens[i], OP_AG, MAX_OP_LEN) == 0  ) {  //AG
      _RECENT_CTLPROP = propTokens[i];
      DdNode* a = ddStack_pop(stack);
      recentNode = AG(a, context);
    }
    else if ( strncmp(propTokens[i], OP_EU, MAX_OP_LEN) == 0  ) {  //EU  'a b EU' == 'E ( a U b )'
      _RECENT_CTLPROP = propTokens[i];
      DdNode* event = ddStack_pop(stack); //eventually 
      DdNode* befor = ddStack_pop(stack); //before
      recentNode = EU(befor, event, context);
    }
    else if ( strncmp(propTokens[i], OP_AU, MAX_OP_LEN) == 0  ) {  //AU  'a b AU' == 'A ( a U b )'
      _RECENT_CTLPROP = propTokens[i];
      DdNode* event = ddStack_pop(stack);
      DdNode* befor = ddStack_pop(stack);
      recentNode = AU(befor, event, context);
    }
    else {                                                   //Found var. Must already exist. Put on stack.

      augmentedDdVar* var;

      if( (var = find(propTokens[i], context.existingVars, context.numExistingVars)) ) { //find unprimed version and push to stack
          recentNode = var->node;
      }
      else {  //Complain
        printf("Found undeclared variable '%s' in CTL formula. Quitting...\n", propTokens[i]);
        exit(3);
      }
    }
    ddStack_push(stack, recentNode);
  }

  //---- assert well formed expression ----
  if( stack->top != 1) {
    printf("Malformed property expression. Quitting...\n");
    exit(3);
  }
  //---------------------------------------


  //---------------------------------------
  return recentNode;
}

DdNode* EX(DdNode* propStates, modelContext context) { 
/**   
  *@brief Returns all x such that: exists x' . T(x, x') && prop(x')
  *
  *@detail 
  * propStates are the states satisfying the state formula for the given property.
  *Note that propStates formula is given in terms of unprimed vars.
*/
  DdNode* propStatesPrimed = Cudd_bddSwapVariables(context.manager, propStates, context.primedNodes, context.unprimedNodes, context.numUnprimedNodes);

  DdNode* preExist = Cudd_bddAnd(context.manager, context.transition, propStatesPrimed);

  for (int i = 0; i < context.numUnprimedNodes; ++i)
  {
    preExist = Cudd_bddExistAbstract(context.manager, preExist, context.primedNodes[i]);
    // printf("Abstracting: %li\n", context.primedNodes[i]);
  }

  return preExist;
}

DdNode* AX(DdNode* propStates, modelContext context) { 
/**   
  *@brief Returns all x such that: forall x' . T(x, x') => prop(x')
  *
  *@detail 
  propStates are the states satisfying the state formula for the given property.
  *Note that propStates formula is given in terms of unprimed vars.
*/
  return Cudd_Not(EX(Cudd_Not(propStates), context));
}

DdNode* EF(DdNode* propStates, modelContext context) {
  /** 
  *Note that propStates formula is given in terms of unprimed vars.
  */
  DdNode* prev = propStates;
  DdNode* current = Cudd_bddOr(context.manager, propStates, EX(propStates, context));

  while(  Cudd_EquivDC(context.manager, prev, current, Cudd_ReadLogicZero(context.manager)) != 1 ) {
    prev = current;
    current = Cudd_bddOr(context.manager, propStates, EX(current, context));
  }
  return current;
}

DdNode* AF(DdNode* propStates, modelContext context) {
  /** 
  *Note that propStates formula is given in terms of unprimed vars.
  */
  DdNode* prev = propStates;
  DdNode* current = Cudd_bddOr(context.manager, propStates, AX(propStates, context));

  while(  Cudd_EquivDC(context.manager, prev, current, Cudd_ReadLogicZero(context.manager)) != 1 ) {
    prev = current;
    current = Cudd_bddOr(context.manager, propStates, AX(current, context));
  }
  return current;
}

DdNode* EG(DdNode* propStates, modelContext context) {
  /**
  *propStates is given in terms of unprimed vars.
  */

  DdNode* prev = propStates;
  DdNode* current = Cudd_bddAnd(context.manager, propStates, EX(propStates, context));

  while(  Cudd_EquivDC(context.manager, prev, current, Cudd_ReadLogicZero(context.manager)) != 1 ) {
    prev = current;
    current = Cudd_bddAnd(context.manager, propStates, EX(current, context));
  }
  return current;
}

DdNode* AG(DdNode* propStates, modelContext context) {
  DdNode* prev = propStates;
  DdNode* current = Cudd_bddAnd(context.manager, propStates, AX(propStates, context));

  while(  Cudd_EquivDC(context.manager, prev, current, Cudd_ReadLogicZero(context.manager)) != 1 ) {
    prev = current;
    current = Cudd_bddAnd(context.manager, propStates, AX(current, context));
  }
  return current;
}

DdNode* EU(DdNode* beforeStates, DdNode* finallyStates, modelContext context) {
  DdNode* prev = finallyStates;
  DdNode* current = Cudd_bddOr(context.manager, finallyStates, Cudd_bddAnd(context.manager, beforeStates, EX(finallyStates, context)));

  while(  Cudd_EquivDC(context.manager, prev, current, Cudd_ReadLogicZero(context.manager)) != 1 ) {
    prev = current;
    current = Cudd_bddOr(context.manager, finallyStates, Cudd_bddAnd(context.manager, beforeStates, EX(current, context)));
  }
  return current;
}

DdNode* AU(DdNode* beforeStates, DdNode* finallyStates, modelContext context) {
  DdNode* prev = finallyStates;
  DdNode* current = Cudd_bddOr(context.manager, finallyStates, Cudd_bddAnd(context.manager, beforeStates, AX(finallyStates, context)));

  while(  Cudd_EquivDC(context.manager, prev, current, Cudd_ReadLogicZero(context.manager)) != 1 ) {
    prev = current;
    current = Cudd_bddOr(context.manager, finallyStates, Cudd_bddAnd(context.manager, beforeStates, AX(current, context)));
  }
  return current;
}