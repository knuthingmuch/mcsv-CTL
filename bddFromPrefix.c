//------- bddFromPrefix.c -------
DdNode* bddFromPrefix(DdManager* manager, char** tokens, size_t numTokens, size_t numVars, augmentedDdVar** seenVars, size_t* numSeenVars);
//------------local-------------
bool isPrimedToken(char* token);
//-------------------------------

DdNode* bddFromPrefix(DdManager* manager, char** tokens, size_t numTokens, size_t numVars , augmentedDdVar** seenVars, size_t* numSeenVars) {
  /**
  *@brief Creates a BDD from an array of tokens representing boolean formula and returns root node.
  *Don't forget to increment reference count of returned bdd root node in he caller function by using Cudd_Ref().
  *
  *Also takes an array of created bdd variable nodes(as an array of augmentedDdVar ptrs) and stores newly created DdNode* in the array for future use. 
  *
  *Be sure to allocate sufficient memory. No checks are performed whatsoever.
  *
  *@details 
  *numTokens: #tokens in the array of tokens called tokens.
  *seenVars: #variables seen so far by the program. New variables encountered will be added to this array.
  *numSeenVars: Size of te seeenVars Array. This will be incremented for each new variable encountered.
  *
  *Misc: PRIMED and UNPRIMED are macros defined in the main file so that primed and unprimed variables 
  *  can be tagged once and for all.
  */

  // augmentedDdVar seenVars[numTokens]
  size_t currentVariableIndex = *numSeenVars;

  ddStack* stack = GC_MALLOC(sizeof(ddStack)+sizeof(DdNode*)*(numTokens)); //GC_MALLOC initialises mem to zero.
  ddStack_init(stack, numTokens); //use depth? Meh! Mem is cheap.

  DdNode* recentNode; //always point to most recent node.

  for(size_t i = 0; i < numTokens; i++) {
    size_t j;
    bool exists = false;

    if( strncmp(tokens[i], OP_OR, MAX_OP_LEN) == 0) {         //OR
      DdNode* a = ddStack_pop(stack);
      DdNode* b = ddStack_pop(stack);
      recentNode = Cudd_bddOr(manager, a, b);
    }
    else if ( strncmp(tokens[i], OP_AND, MAX_OP_LEN) == 0 ) {  //AND
      DdNode* a = ddStack_pop(stack);
      DdNode* b = ddStack_pop(stack);
      recentNode = Cudd_bddAnd(manager, a, b);
    }
    else if ( strncmp(tokens[i], OP_XOR, MAX_OP_LEN) == 0) {   //XOR
      DdNode* a = ddStack_pop(stack);
      DdNode* b = ddStack_pop(stack);
      recentNode = Cudd_bddXor(manager, a, b);
    }
    else if ( strncmp(tokens[i], OP_IFF, MAX_OP_LEN) == 0) {   //IFF
      DdNode* a = ddStack_pop(stack);
      DdNode* b = ddStack_pop(stack);
      recentNode = Cudd_Not(Cudd_bddXor(manager, a, b));
      // recentNode = Cudd_bddAnd(manager, Cudd_bddOr(manager, Cudd_Not(a), b), Cudd_bddOr(manager, Cudd_Not(b), a));
    }
    else if ( strncmp(tokens[i], OP_NOT, MAX_OP_LEN) == 0 ) {  //NOT
      DdNode* a = ddStack_pop(stack);
      recentNode = Cudd_Not(a);
    }
    else {                                                      //VAR
      for(j = 0; j < currentVariableIndex; j++) {                  // Look for it amongst existing variables.
        if(strncmp(seenVars[j]->name, tokens[i], MAX_VAR_LEN) == 0) {
          exists = true;
          // printf("Token: %s already exists at: %li\n", seenVars[j]->name, seenVars[j]->node);
          break;
        }
      }
      if(exists) {
        recentNode = seenVars[j]->node;
      }
      else {
        DdNode* newNode = Cudd_bddNewVar(manager);
        recentNode = newNode;
        
        //Now ccreate new aug var 
        augmentedDdVar* newAugVar = GC_MALLOC(sizeof(augmentedDdVar));
        strcpy(newAugVar->name, tokens[i]);
        newAugVar->node = newNode;
        Cudd_Ref(newNode);
        
        newAugVar->parity = isPrimedToken(tokens[i]) ? PRIMED : UNPRIMED; //Macros defined in main() file: main

        //and add it to seenVars array.
        seenVars[currentVariableIndex] = newAugVar;
        // printf("Node for token %s created at: %li\n", seenVars[currentVariableIndex]->name, seenVars[currentVariableIndex]->node);
        currentVariableIndex++; //increment size of var array.
      }
    }
    ddStack_push(stack, recentNode);
  }

  // assert( stack->top == 1 ); //well formed expression
  if( stack->top != 1) {
    printf("Malformed expression. Quitting...\n");
    exit(3);
  }

  *numSeenVars = currentVariableIndex;  //set number of seen variables in caller.
  return recentNode; //return root
}

bool isPrimedToken(char* token) {
  /**
  *Returns true if given token has a ' character on the end.
  */
  if((token[strlen(token)-1]) == '\'')
    return true;
  else
    return false;
}