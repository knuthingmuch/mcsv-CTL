//-----------------ddStack.c------------------------
#include "ddStack.h"
//--------------------------------------------------

void ddStack_init(ddStack* d, size_t capacity) {
  d->top = 0;
  d->capacity = capacity;
}

bool ddStack_push(ddStack* d, DdNode* node) {
  if(d->top < d->capacity) {
    d->stack[(d->top)++]=node;
    // printf("Push: %ld\n",d->top);
    return true;
  }
  else
   return false;    //if fail
}

DdNode* ddStack_pop(ddStack* d) {
  if(d->top != 0) {
    // printf("Pop: %ld\n",d->top);
    return d->stack[--(d->top)];
  }
  return NULL;
}

bool ddStack_isEmpty(ddStack* d) {
  return (d->top == 0);
} 
