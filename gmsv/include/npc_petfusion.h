#ifndef __NPC_PETFUSION_H__
#define __NPC_PETFUSION_H__

#include "common.h"

void NPC_PetFusionManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_PetFusionManInit(int meindex);
void NPC_PetFusionManWindowTalked(int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetFusionManLoop(int meindex);

#ifdef _NB_DRAGON_FUSION
void NPC_DragonFusionManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_DragonFusionManInit(int meindex);
void NPC_DragonFusionManWindowTalked(int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_DragonFusionManLoop(int meindex);
#endif

#endif
