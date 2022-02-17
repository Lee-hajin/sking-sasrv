#ifndef __NPC_WINDOWHEALER_H__
#define __NPC_WINDOWHEALER_H__

#include "common.h"

void NPC_WindowHealerTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_WindowHealerInit(int meindex);
void NPC_WindowHealerWindowTalked(int meindex, int talkerindex, int seqno,
                                  int select, char *data);
void NPC_WindowHealerLooked(int meindex, int lookedindex);

#endif
/*__NPC_WINDOWHEALER_H__*/
