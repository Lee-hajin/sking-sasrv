#ifndef __NPC_FMHEALER_H__
#define __NPC_FMHEALER_H__

#include "common.h"

void NPC_FmHealerTalked(int meindex, int talkerindex, char *msg,
                        int color);
BOOL NPC_FmHealerInit(int meindex);

#endif
/*__NPC_FMHEALER_H__*/
