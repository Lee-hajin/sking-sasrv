#ifndef __BATTLE_COMMAND_H__
#define __BATTLE_COMMAND_H__

#include "NewBilu/version.h"
#include "common.h"

void BattleCommandDispach(
	int fd,
	char *command);

void BattleEncountOut(
	int charaindex);

BOOL BATTLE_CommandSend(int charaindex, char *pszCommand);

#ifdef _NB_BATTLEHALO
BOOL BATTLE_CommandSendHalo(int charaindex, int battleindex);
#endif

BOOL BATTLE_MakeCharaString(
	int battleindex,
	char *pszCommand, // 平乓仿弁正□树  请  燮
	int size		  // 扔奶术
);

void BATTLE_CharSendAll(int battleindex);
void BATTLE_CharSendOne(int battleindex, int mycharaindex);

BOOL BATTLE_PetDefaultCommand(int petindex);

void BATTLE_ActSettingSend(int battleindex);

#endif
