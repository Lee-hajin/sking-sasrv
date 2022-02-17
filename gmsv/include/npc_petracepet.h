#ifndef NPC_PetRace_Pet_
#define NPC_PetRace_Pet_

#include "common.h"

BOOL NPC_PetRacePetInit(int meindex);
void NPC_PetRacePetLoop(int meindex);
void NPC_PetRacePetTalked(int meindex, int talkerindex, char *szMes, int color);

#endif
