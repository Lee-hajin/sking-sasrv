#ifndef __NPC_WELFARE_H__
#define __NPC_WELFARE_H__

#include "common.h"

BOOL NPC_WelfareInit(int meindex);
void NPC_WelfareTalked(int meindex, int talker, char *msg, int color);
void NPC_WelfareWindowTalked(int meindex, int talkerindex, int seqno,
                             int select, char *data);
void NPC_WelfareMakeStr(int meindex, int toindex, int select);
#ifdef _PRO3_ADDSKILL
void delprosk(int talkerindex, int delskid);
#endif

#endif
