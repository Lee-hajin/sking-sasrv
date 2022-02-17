#ifndef __NPC_TRANSMIGRATION_H__
#define __NPC_TRANSMIGRATION_H__

#include "common.h"

void NPC_TransmigrationTalked(int meindex, int talkerindex, char *msg,
                              int color);
BOOL NPC_TransmigrationInit(int meindex);
void NPC_TransmigrationWindowTalked(int meindex, int talkerindex, int seqno,
                                    int select, char *data);
BOOL NPC_TransmigrationStatus(int meindex, int toindex, int *work);
int NPC_TransmigrationFlg_CLS(int meindex, int toindex);
BOOL NPC_PetTransManStatus_Lua(int meindex, int petindex, int att1, int att2,
                               int att3, int att4);
#endif

/*__NPC_TRANSMIGRATION_H__*/
