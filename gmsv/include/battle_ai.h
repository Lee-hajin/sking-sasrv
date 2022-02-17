#ifndef __BATTLE_AI_H__
#define __BATTLE_AI_H__

#include "version.h"
#include "NewBilu/version.h"
#include "common.h"

#ifdef BOSS_AI
#define TACTICS_BOSS 2
BOOL BATTLE_ai_checkskilltarget(int charaindex, int type, int skillid, int *target, int side);
#endif
/*   曰轼 */
int BATTLE_ai_all(int battleindex, int side, int turn);
int BATTLE_ai_one(int charaindex, int battleindex, int side, int turn);
#ifdef _ENEMY_ATTACK_AI
int GetSubdueAtt(int index);
#endif
#endif
