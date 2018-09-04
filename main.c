/**
*\mainpage 
* To compile, set CUDD_PATH in makefile and then simply do: make
*
* P.S. The included source uses the Boehm Garbage Collector. It can be installed on Debian/Ubuntu with the libgc or libgc-dev package.
*
* It takes exactly 5 arguments:
* ./executable [state formula] [transition formula] [property] [output dot filename] [expected states]
* 
* Read the file named 'test_cases' for explanation of these arguments.
*
* This simple model checker has two independent features:
*
* 1]
* It will output a BDD to specified file, consisting of all the reachable states satisfying the CTL property(say S). 
* It will also check and report whether S == E, where E is the set of states you expect should satisy the given property. 
* If you do not wish to use this feature, just input a dummy formula like "x0" where 'x0' appears somewhere in the transition system formula.
*
* 2]
* Checks if the initial states are contained in the set of states satisfying the CTL property.
* i.e. Does the given T.S. satify the CTL property?  (Yes/No)
*
* P.P.S: Single CLI argument on linux can be 128KiB, so no worries there. 
*/
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

//--------- Shared structs ----------
typedef struct modelContext //dont make const to avoid unnecessary incompatible ptr errors.
{
  DdManager* manager;
  DdNode* transition;  

  augmentedDdVar** existingVars;
  augmentedDdVar** primedVars;
  augmentedDdVar** unprimedVars;

  size_t numExistingVars;  //2*numUnprimedNodes

  DdNode** primedNodes;
  DdNode** unprimedNodes;
  size_t numUnprimedNodes; //same as numPrimedNodes

} modelContext;

//----------- Other src -------------
#include "aux.c"   //find, translatedImage et al.
#include "ctlmc.c"

//-----------------------------------
#define PRIMED true
#define UNPRIMED false

//-------------------------------------------------
#include "bddFromPrefix.c"

// ------------------------------------------------

int main(int argc, char** argv) {

  printf("argc: %i\n\n", argc);

  if(argc != 6) { //TODO
    printf("Takes 5 arguments.\nFormulae in quotes. eg: \"a ! b +\"\nUsage: <executable> <state formula> <transition formula> <property> <output dot filename> <expected states> \nCheck 'test_cases' for examples.\n");
    return 0;
  }

  GC_INIT();

  char* initForm = argv[1];
  char* transForm = argv[2];
  char* propForm = argv[3];
  char* dotfileName = argv[4];

  char* expectedForm = argv[5];

// ---------------- Make BDDs from init and trans formulae ---------------   
  size_t initTokCount, transTokCount;
  char** initTokens = tokenize(initForm, &initTokCount);
  char** transTokens = tokenize(transForm, &transTokCount);

  size_t numInitUniqVars, numTransUniqVars;
  char** uniqInitVars = getUniqVars(initTokens, initTokCount, &numInitUniqVars);
  char** uniqTransVars = getUniqVars(transTokens, transTokCount, &numTransUniqVars);

//--------------------------------------------------------------------------
  DdManager* manager = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
  if(manager == NULL) {
    printf("Error while initialization");
    exit(1);    //for non main function, do cleanup and return.
  }

// ------------------------ Build BDDs --------------------------

  size_t max_num_possib_vars = numInitUniqVars + numTransUniqVars;
  
  augmentedDdVar** existingVars = GC_MALLOC(max_num_possib_vars*sizeof(augmentedDdVar*)); ///< Keep all the variables in the formulae in existingVars. 
  //allocate on heap to provide for large sizes.
  size_t numExistingVars = 0;

  DdNode* initial = bddFromPrefix(manager, initTokens, initTokCount, numInitUniqVars, existingVars, &numExistingVars);
  Cudd_Ref(initial);
  // printf("Seen vars till init: %li\n", numExistingVars);

  DdNode* transition = bddFromPrefix(manager, transTokens, transTokCount, numTransUniqVars, existingVars, &numExistingVars);
  Cudd_Ref(transition);
  // printf("Seen vars till trans: %li\n", numExistingVars);

//----------------- EXPECTED STATES FOR TESTING -------------------------- 
  size_t expectedTokCount;
  char** expectedTokens = tokenize(expectedForm, &expectedTokCount);
  
  size_t numExpectedUniqVars;
  char** uniqExpectedVars = getUniqVars(expectedTokens, expectedTokCount, &numExpectedUniqVars);

  size_t old_numExistingVars = numExistingVars;

  DdNode* expected = bddFromPrefix(manager, expectedTokens, expectedTokCount, numExpectedUniqVars, existingVars, &numExistingVars);
  Cudd_Ref(expected);

  assert(old_numExistingVars == numExistingVars); //Expected formula should not add new vars.
  
//------------------------------------------------------------------------

// ---- Separate primed and unprimed vars ------

  //------------ Allocation -------------
  size_t numPrimedVars = 0;
  augmentedDdVar** primedVars = GC_MALLOC(numExistingVars*sizeof(augmentedDdVar*));
  DdNode** primedNodes = GC_MALLOC(numExistingVars*sizeof(DdNode*));
  
  size_t numUnprimedVars = 0;
  augmentedDdVar** unprimedVars = GC_MALLOC(numExistingVars*sizeof(augmentedDdVar*));
  DdNode** unprimedNodes = GC_MALLOC(numExistingVars*sizeof(DdNode*));   //no need to allocate inner. Will only assign existing pointers.
  //-------------------------------------

  for (int i = 0; i < numExistingVars; i++) {   //copy unprimed variables *s
    if( existingVars[i]->parity == UNPRIMED ) {
      unprimedVars[numUnprimedVars] = existingVars[i];
      unprimedNodes[numUnprimedVars] = existingVars[i]->node;
      numUnprimedVars++;
    }
    else {
      //don't assign primed here, vars need to be in one-to-one correspondence.
      numPrimedVars++;
    }
  } 
  // printf("numUnprimedVars: %li\n", numUnprimedVars);
  
  if (numPrimedVars != numUnprimedVars) { //Complain
    printf("Fix your formulae. All primed and unprimed variables should show up in the union of the two input formulae.\n");
    exit(2);
  }

  //-- Now build primedVars such that variable at ith position in primedVars the primed counterpart of ith variable in unprimedVars.

  for (int i = 0; i < numUnprimedVars; i++)
  {
    augmentedDdVar* found;
    char* primed = GC_MALLOC(MAX_VAR_LEN*sizeof(char));
    strcpy(primed, unprimedVars[i]->name);
    strcat(primed, "'");

    for (int j = 0; j < numExistingVars; j++)   //look for primed counterpart of each unprimed var .
    {
      if( (found = find(primed, existingVars, numExistingVars)) ) {
        primedVars[i] = found;
        primedNodes[i] = found->node;
    }
    }
  }
// ------------------- Find reachable state space -----------------------------

  DdNode* prevReachable = initial;
  DdNode* image = translatedImage(manager, initial, transition, primedNodes, unprimedNodes, numUnprimedVars);
  DdNode* reachable = Cudd_bddOr(manager, initial, image);

  while( Cudd_EquivDC(manager, prevReachable, reachable, Cudd_ReadLogicZero(manager)) != 1 ) { // == 1 means equal.
    prevReachable = reachable;
    image = translatedImage(manager, image, transition, primedNodes, unprimedNodes, numUnprimedVars);
    reachable = Cudd_bddOr(manager, reachable, image);
  }

//--------------------------- Model Check ---------------------------

  modelContext context = { 
    .manager = manager, 
    .transition = transition, 
    .existingVars = existingVars, 
    .numExistingVars = numExistingVars, 
    .primedVars = primedVars, 
    .unprimedVars = unprimedVars, 
    .primedNodes = primedNodes, 
    .unprimedNodes = unprimedNodes, 
    .numUnprimedNodes = numUnprimedVars 
  };

  DdNode* satisfyingStates = check(propForm, context); //subset of entire state space satisfying property specified.
  Cudd_Ref(satisfyingStates);

  DdNode* alsoReachable = Cudd_bddAnd(manager, satisfyingStates, reachable); //set of property satifying states that are also reachable.
  Cudd_Ref(alsoReachable);

  printf("Reachable satisfying states correct? %s\n\n", Cudd_EquivDC(manager, alsoReachable, expected, Cudd_ReadLogicZero(manager)) ? "Yes" : "No" );

  DdNode* alsoInitial = Cudd_bddAnd(manager, initial, satisfyingStates); //set of property satisfyig states that also initial states.
  Cudd_Ref(alsoInitial);

  printf("Initial states satisfy property? %s\n\n", Cudd_EquivDC(manager, initial, alsoInitial, Cudd_ReadLogicZero(manager)) ? "Yes" : "No" );

  //------------ Backtrace for EG -----------
  // if(strcmp(_RECENT_CTLPROP, OP_EG) == 0) {
  //   if( Cudd_EquivDC(manager, Cudd_bddAnd(manager, _EGPROP, initial), initial, Cudd_ReadLogicZero()) ) { //if all initial states satisfy _EGPROP.
      
  //   }
  //   else
  //   {
  //     printf("Initial state: does not satisfy _EGPROP\n");
  //   }
  // }

// --------------------------- Dump stuff ---------------------------
  DdNode** outputs = &alsoReachable;

  const char** inputNames = GC_MALLOC(numExistingVars*sizeof(char*));
  for (int i = 0; i < numExistingVars; i++) { //copy all. abstracted variables are automatically omitted from inputNames while dumping.
    inputNames[i] = existingVars[i]->name;
  }

  char* outputNames[1];
  outputNames[0] = GC_MALLOC((50+strlen(initForm)+strlen(transForm))*sizeof(char));
  strcpy(outputNames[0], "Image of (");
  strcat(outputNames[0], initForm);
  strcat(outputNames[0], ") under (");
  strcat(outputNames[0], dotfileName);
  strcat(outputNames[0], ")");
//-------------------------
  
  printf("Writing satisfying reachable states to file: \"%s\"\n", dotfileName);
  FILE* f = fopen(dotfileName, "w");
  Cudd_DumpDot(manager, 1, outputs, inputNames, outputNames, f); 
  fclose(f);

  Cudd_RecursiveDeref(manager, satisfyingStates);
  Cudd_RecursiveDeref(manager, alsoReachable);
  Cudd_RecursiveDeref(manager, alsoInitial);
  Cudd_RecursiveDeref(manager, reachable);
  Cudd_RecursiveDeref(manager, image);
  Cudd_RecursiveDeref(manager, prevReachable);
  Cudd_RecursiveDeref(manager, initial);
  Cudd_RecursiveDeref(manager, transition);
  Cudd_RecursiveDeref(manager, expected);

  Cudd_Quit(manager);
}
