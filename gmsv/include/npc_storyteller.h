#ifndef _NPC_STORYTELLER_H_
#define _NPC_STORYTELLER_H_

#include "common.h"

BOOL NPC_StoryTellerInit(int meindex);
void NPC_StoryTellerTalked(int meindex, int talker, char *msg, int col);

#endif
