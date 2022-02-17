#ifndef __NPC_PETMAKER_H__
#define __NPC_PETMAKER_H__

#include "common.h"

void NPC_PetMakerTalked(int meindex, int talkerindex, char *msg,
                        int color);
BOOL NPC_PetMakerInit(int meindex);

#endif
/*__NPC_PETMAKER_H__*/
