#ifdef _CHAR_PARTAUTO

#include "common.h"

BOOL NPC_PartyAutoInit(int meindex);
void NPC_PartyAutoTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_PartyAutoWindowTalked(int meindex, int talkerindex, int seqno,
							   int select, char *data);
#endif