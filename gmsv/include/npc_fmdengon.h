#ifndef __NPC_FMDENGON_H__
#define __NPC_FMDENGON_H__

#include "common.h"

BOOL NPC_FmDengonInit(int meindex);
void NPC_FmDengonLooked(int meindex, int lookedindex);
void NPC_FmDengonWindowTalked(int index, int talker, int seqno, int select, char *data);

#endif

/*__NPC_FMDENGON_H__*/
