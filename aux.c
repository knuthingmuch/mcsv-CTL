#pragma once

#include <string.h>
#include "ddStack.h" //for augmentedDdVar

//---------------------------- Declarations --------------------------------
augmentedDdVar* find(char* key, augmentedDdVar* array[], size_t arrSize);
DdNode* translatedImage(DdManager* manager, DdNode* states, DdNode* transition, DdNode* primeNodes[], DdNode* unprimedNodes[], size_t numNodes);

//--------------------------------------------------------------------------

augmentedDdVar* find(char* key, augmentedDdVar* array[], size_t arrSize) {
  for (int j = 0; j < arrSize; j++)
  {
    if( strcmp(key, array[j]->name) == 0 ) {
      return array[j];
    }
  }
  return NULL;
} 

DdNode* translatedImage(DdManager* manager, DdNode* states, DdNode* transition, DdNode** primedNodes, DdNode** unprimedNodes, size_t numNodes) {
  /**
  @brief Takes a set of states and transition relation as BDDs, and returns the Image with the primed variables(in primedNodes[]) 
  swapped with unprimed ones(in unprimedNodes[]), so that the Image can be directly combined with the previous set of states to obtain reachable states.

  @details To find the image, first takes Cudd_bddAnd of states and transition; then abstracts out all the variables provided in unprimedNodes.

  Finally primed variables are replaced by unprimed ones(from unprimedNodes[]), before the Image BDD is returned.

  numNodes is the number of variables in each of the primedNodes and unprimedNodes. Both MUST be equal! No checks are performed.

  IMPORTANT: Also check that ith node in primedNodes[] represents a variable iff ith node in unprimedNodes[] represents its unprimed counterpart.
  */
  DdNode* image = Cudd_bddAnd(manager, states, transition);

  // static int k = 0;

  for(size_t i = 0; i < numNodes; i++) {
    image =  Cudd_bddExistAbstract(manager, image, unprimedNodes[i]);
  }

  // return image;
  
  if( (image = Cudd_bddSwapVariables(manager, image, primedNodes, unprimedNodes, numNodes)) ) {
    // printf("Swap count: %i\n", ++k);
    return image;
  }
  else {
    printf("Failed to swap vars. Check formula.\n Quitting...\n");
    exit(3);
  }
}