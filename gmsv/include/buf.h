#ifndef __BUF_H__
#define __BUF_H__

#include "NewBilu/version.h"
#include "common.h"

void memEnd(void);
BOOL configmem(int unit, int unitnumber);
BOOL memInit(void);
#ifdef _NB_FIX_MEMORY
void *allocateMemory(const unsigned long nbyte);
#else
void *allocateMemory(const unsigned int nbyte);
#endif
void freeMemory(void *freepointer);
void showMem(char *buf);
#endif
