#include "battle.h"
#include "anim_tbl.h"
#include "battle_ai.h"
#include "battle_command.h"
#include "battle_event.h"
#include "battle_magic.h"
#include "buf.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "common.h"
#include "configfile.h"
#include "encount.h"
#include "enemy.h"
#include "handletime.h"
#include "log.h"
#include "lssproto_serv.h"
#include "lssproto_serv.h"
#include "magic.h"
#include "npc_npcenemy.h"
#include "npcutil.h"
#include "object.h"
#include "pet_skill.h"
#include "pet_skillinfo.h"
#include "readmap.h"
#include "version.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _Item_ReLifeAct
#include "item.h"
#endif
#ifdef _TRADE_PK
#include "trade.h"
#endif
#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif
#include "npc_healer.h"
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
#include "profession_skill.h"
#include "skill.h"
#endif
#include "family.h"

#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#include "mylua/mylua.h"

extern MY_Lua MYLua;
#endif

#ifdef _ABSOLUTE_DEBUG
extern int debugline;
extern int comnum;
#endif

//#define DANTAI
static int Total_BattleNum = 0;
BATTLE *BattleArray;
int BATTLE_battlenum;
static int BATTLE_searchCnt = 0;

#ifdef _PET_LIMITLEVEL // ANDY_ADD
void Pet_Check_Die(int petindex);
#endif
static int BATTLE_SearchTask(void);
static int BATTLE_Battling(int battleindex);

#ifdef _Item_ReLifeAct
BOOL CHECK_ITEM_RELIFE(int battleindex, int toindex);
#endif
#ifdef _LOSE_FINCH_
BOOL CHECK_PET_RELIFE(int battleindex, int petindex);
#endif
char szAllBattleString[BATTLE_STRING_MAX];
#ifdef _OTHER_MAGICSTAUTS
void BATTLE_MagicStatusSeq(int charaindex);
#endif

// Terry 2001/11/28
char szBattleString[1024];
char *pszBattleTop, *pszBattleLast;

char szBadStatusString[1024];

int gWeponType;
float gDamageDiv;
int gItemCrushRate = 400000;
int BoomerangVsTbl[4][5] = {
    {4 + 5 * 0, 2 + 5 * 0, 0 + 5 * 0, 1 + 5 * 0, 3 + 5 * 0},
    {4 + 5 * 1, 2 + 5 * 1, 0 + 5 * 1, 1 + 5 * 1, 3 + 5 * 1},
    {4 + 5 * 2, 2 + 5 * 2, 0 + 5 * 2, 1 + 5 * 2, 3 + 5 * 2},
    {4 + 5 * 3, 2 + 5 * 3, 0 + 5 * 3, 1 + 5 * 3, 3 + 5 * 3},

};

BOOL BATTLE_CHECKINDEX(int battleindex) {
  if (battleindex >= BATTLE_battlenum || battleindex < 0)
    return FALSE;

  return BattleArray[battleindex].use;
}

void BATTLE_BadStatusAllClr(int charaindex) {
  int i;
  for (i = 1; i < BATTLE_ST_END; i++) {
    CHAR_setWorkInt(charaindex, StatusTbl[i], 0);
  }
#ifdef _OTHER_MAGICSTAUTS
  for (i = 1; i < MAXSTATUSTYPE; i++) {
    CHAR_setWorkInt(charaindex, MagicTbl[i], 0);
    CHAR_setWorkInt(charaindex, CHAR_OTHERSTATUSNUMS, 0);
  }
#endif
#ifdef _IMPRECATE_ITEM
  for (i = 0; i < 3; i++) {
    CHAR_setWorkInt(charaindex, CHAR_WORKIMPRECATENUM1 + i, 0);
  }
#endif

#ifdef _PETSKILL_SETDUCK
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLDUCK, 0);
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLDUCKPOWER, 0);
#ifdef _MAGICPET_SKILL
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLSTR, 0);
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLSTRPOWER, 0);
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLTGH, 0);
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLTGHPOWER, 0);
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLDEX, 0);
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLDEXPOWER, 0);
  CHAR_setWorkInt(charaindex, CHAR_MAGICPETMP, 0);
#endif
#endif
#ifdef _STRENGTH_PETSKILL
  CHAR_setWorkInt(charaindex, CHAR_WORK_STRENGTH, 0);
#endif
#ifdef _LOSTLOST_PETSKILL
  CHAR_setWorkInt(charaindex, CHAR_WORK_LOSTLOST, 0);
#endif
  CHAR_setFlg(charaindex, CHAR_ISDIE, 0);

#ifdef _BATTLE_PROPERTY
  {
    Char *ch;
    ch = CHAR_getCharPointer(charaindex);
    if (ch == NULL)
      return;
    strcpysafe(ch->charfunctable[CHAR_BATTLEPROPERTY].string,
               sizeof(ch->charfunctable[CHAR_BATTLEPROPERTY]), ""); //战斗
    CHAR_constructFunctable(charaindex);
  }
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
  CHAR_setWorkInt(charaindex, CHAR_MYSKILLHIT, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_P_DUCK, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKMOD_P_DUCK, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_WEAPON, 0); // 武器专精

  // 火冰电抗性
  for (i = 0; i < 3; i++)
    CHAR_setWorkInt(charaindex, CHAR_WORK_F_RESIST + i,
                    0 /*CHAR_getInt( charaindex, PROFESSION_FIRE_R+i )*/);

#endif

#ifdef _PETSKILL_BECOMEPIG
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      int pindex = CHAR_getCharPet(charaindex, i);
      if (!CHAR_CHECKINDEX(pindex))
        continue;
      CHAR_setInt(pindex, CHAR_BECOMEPIG, -1);
    }
  }
#endif
}

int BATTLE_getTopBattle(int battleindex) {

  BATTLE *pBattleTop;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -1;

  pBattleTop = &BattleArray[battleindex];

  while (pBattleTop != NULL) {
    pBattleTop = BattleArray[battleindex].pBefore;
  }

  if (BATTLE_CHECKINDEX(pBattleTop->battleindex) == FALSE) {
    return -1;
  } else {
    return pBattleTop->battleindex;
  }
}

static int BATTLE_getBattleFieldNo(int floor, int x, int y) {
  int tile[2], map[3], iRet;
  if (!MAP_getTileAndObjData(floor, x, y, &tile[0], &tile[1]))
    return FALSE;
  map[0] = MAP_getImageInt(tile[0], MAP_BATTLEMAP);
  map[1] = MAP_getImageInt(tile[0], MAP_BATTLEMAP2);
  map[2] = MAP_getImageInt(tile[0], MAP_BATTLEMAP3);
  iRet = map[RAND(0, 2)];
  return iRet;
}

void BATTLE_BadStatusString(int defNo, int status) {
  char szWork[256];
  if (status < 1 || status >= BATTLE_ST_END) {
    status = 0;
  }
#ifdef _SKILL_25_FIX
  if (status == BATTLE_ST_DEEPPOISON)
    status = BATTLE_ST_POISON;
  else if (status == BATTLE_ST_WEAKEN)
    status = BATTLE_ST_DRUNK;
#endif
  sprintf(szWork, "BM|%X|%X|", defNo, status);
  strncat(szBadStatusString, szWork, sizeof(szBadStatusString));
}

#ifdef _ATTACK_MAGIC

static int CharTableIdx[20][2] = {
    {3, 2}, {3, 1}, {3, 3}, {3, 0}, {3, 4}, {2, 2}, {2, 1},
    {2, 3}, {2, 0}, {2, 4}, {0, 2}, {0, 1}, {0, 3}, {0, 0},
    {0, 4}, {1, 2}, {1, 1}, {1, 3}, {1, 0}, {1, 4}

};

typedef int (*FUNCSORTLOC)(const void *, const void *);

static int SortLoc(const int *pEle1, const int *pEle2) {
  int ele1basex, ele1basey;
  int ele2basex, ele2basey;

  ele1basex = CharTableIdx[*pEle1][1];
  ele1basey = CharTableIdx[*pEle1][0];
  ele2basex = CharTableIdx[*pEle2][1];
  ele2basey = CharTableIdx[*pEle2][0];

  if (*pEle1 >= 10) {
    if (ele1basey != ele2basey)
      return (ele1basey - ele2basey);
    return (ele1basex - ele2basex);
  } else { // 右下方
    if (ele1basey != ele2basey)
      return (ele2basey - ele1basey);
    return (ele2basex - ele1basey);
  }
  return 0;
}
#endif

int BATTLE_MultiList(int battleindex, int toNo, int ToList[]) {
  int j, i, cnt = 0, nLife = 0, nLifeArea[10];
#ifdef _ATTACK_MAGIC
  // 单人攻击
  if (0 <= toNo && toNo <= 19) {
    memset(nLifeArea, -1, sizeof(nLifeArea));
    // 如果被攻击者是右下方的人
    if (toNo >= 0 && toNo <= 9) {
      for (i = 0; i < 10; i++) {
        // 确定活着的人数,并记录活着的人的号码
        if (BATTLE_TargetCheck(battleindex, i) == TRUE)
          nLifeArea[nLife++] = i;
      }
    }
    // 如果被攻击者是左上方的人
    if (toNo >= 10 && toNo <= 19) {
      for (i = 10; i < 20; i++) {
        // 确定活着的人数,并记录活着的人的号码
        if (BATTLE_TargetCheck(battleindex, i) == TRUE)
          nLifeArea[nLife++] = i;
      }
    }

    // 全死(虽然不太可能,但..)
    if (nLife == 0) {
      print("\nAll die!!");
      return -1;
    } else {
      // 被攻击的对象已经死亡或不在战场上
      if (BATTLE_TargetCheck(battleindex, toNo) == FALSE)
        // 随机找一只来打
        while ((toNo = nLifeArea[rand() % 10]) == -1)
          ;
    }
    ToList[0] = toNo;
    ToList[1] = -1;
    cnt = 1;
  }
  // 右下後一列攻击
  else if (TARGET_SIDE_0_B_ROW == toNo) {
    for (j = 0, i = 0; i < SIDE_OFFSET / 2; i++) {
      if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
    // 如果这一排都没有人可以打
    if (j == 0) {
      // 换前一排
      for (j = 0, i = SIDE_OFFSET / 2; i < SIDE_OFFSET; i++) {
        if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
          ToList[j] = i;
          j++;
        }
      }
      ToList[j] = -1;
      cnt = j;
      // 如果全死
      if (j == 0)
        return -1;
      toNo = TARGET_SIDE_0_F_ROW;
    }
  }
  // 右下前一列攻击
  else if (TARGET_SIDE_0_F_ROW == toNo) {
    for (j = 0, i = SIDE_OFFSET / 2; i < SIDE_OFFSET; i++) {
      if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
    // 如果这一排都没有人可以打
    if (j == 0) {
      // 换後一排
      for (j = 0, i = 0; i < SIDE_OFFSET / 2; i++) {
        if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
          ToList[j] = i;
          j++;
        }
      }
      ToList[j] = -1;
      cnt = j;
      // 如果全死
      if (j == 0)
        return -1;
      toNo = TARGET_SIDE_0_B_ROW;
    }
  }
  // 左下後一列攻击
  else if (TARGET_SIDE_1_B_ROW == toNo) {
    for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET + SIDE_OFFSET / 2; i++) {
      if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
    // 如果这一排都没有人可以打
    if (j == 0) {
      // 换前一排
      for (j = 0, i = SIDE_OFFSET + SIDE_OFFSET / 2; i < SIDE_OFFSET * 2; i++) {
        if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
          ToList[j] = i;
          j++;
        }
      }
      ToList[j] = -1;
      cnt = j;
      // 如果全死
      if (j == 0)
        return -1;
      toNo = TARGET_SIDE_1_F_ROW;
    }
  }
  // 左上前一列攻击
  else if (TARGET_SIDE_1_F_ROW == toNo) {
    for (j = 0, i = SIDE_OFFSET + SIDE_OFFSET / 2; i < SIDE_OFFSET * 2; i++) {
      if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
    // 如果这一排都没有人可以打
    if (j == 0) {
      // 换後一排
      for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET + SIDE_OFFSET / 2; i++) {
        if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
          ToList[j] = i;
          j++;
        }
      }
      ToList[j] = -1;
      cnt = j;
      // 如果全死
      if (j == 0)
        return -1;
      toNo = TARGET_SIDE_1_B_ROW;
    }
  }
  // 右下方所有攻击
  else if (TARGET_SIDE_0 == toNo) {
    // print("BATTLE_MultiList(20)toNo->%d\n",toNo);
    for (j = 0, i = 0; i < SIDE_OFFSET; i++) {
      if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
        ToList[j] = i;
        j++;
      }
    }

    ToList[j] = -1;
    cnt = j;
  }
  // 左上方所有攻击
  else if (toNo == TARGET_SIDE_1) {
    for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET * 2; i++) {
      if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
        ToList[j] = i;
        j++;
      }
    }

    ToList[j] = -1;
    cnt = j;
  }
  // 所有人攻击
  else if (toNo == TARGET_ALL) {
#ifdef _FIX_ARRAYBUG
    for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++) {
#else
    for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++, j++) {
#endif
      if (TRUE == BATTLE_TargetCheck(battleindex, i)) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[i] = -1;
    cnt = j;
    // won add	前後排攻击
  } else if (toNo == TARGER_THROUGH) {
    int toNo2 = -1, count = 0;

    if (BATTLE_TargetCheck(battleindex, toNo) != FALSE) {
      ToList[count] = toNo;
      count++;
      cnt = count;
    }

    if (toNo < (SIDE_OFFSET / 2))
      toNo2 = toNo + (SIDE_OFFSET / 2);
    else if (toNo < (SIDE_OFFSET))
      toNo2 = toNo - (SIDE_OFFSET / 2);
    else if (toNo < (SIDE_OFFSET * 1.5))
      toNo2 = toNo + (SIDE_OFFSET / 2);
    else if (toNo < (SIDE_OFFSET * 2))
      toNo2 = toNo - (SIDE_OFFSET / 2);

    if (BATTLE_TargetCheck(battleindex, toNo2) != FALSE) {
      ToList[count] = toNo2;
      count++;
      cnt = count;
    }
  } else {
    ToList[0] = toNo;
    ToList[1] = -1;
    cnt = 1;
  }

  if (cnt > 1)
    qsort(ToList, cnt, sizeof(ToList[0]), (FUNCSORTLOC)SortLoc);
  return toNo;

#else
  if (0 <= toNo && toNo <= 19) {
    if (BATTLE_TargetCheck(battleindex, toNo) == TRUE) {
      ToList[0] = toNo;
      ToList[1] = -1;
      cnt = 1;
    } else {
      ToList[0] = -1;
      ToList[1] = -1;
      cnt = 0;
    }
  } else if (toNo == TARGET_SIDE_0) {
    for (j = 0, i = 0; i < SIDE_OFFSET; i++) {
      if (BATTLE_TargetCheck(battleindex, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
  } else if (toNo == TARGET_SIDE_1) {
    for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET * 2; i++) {
      if (BATTLE_TargetCheck(battleindex, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
    cnt = j;
  } else if (toNo == TARGET_ALL) {
    for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++, j++) {
      if (BATTLE_TargetCheck(battleindex, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[i] = -1;
    cnt = j;
  } else {
    ToList[0] = toNo;
    ToList[1] = -1;
    cnt = 1;
  }

  return 1;
#endif
}

void BATTLE_MultiListDead(int battleindex, int toNo, int ToList[]) {
  int j, i;
  if (0 <= toNo && toNo <= 19) {
    if (BATTLE_TargetCheckDead(battleindex, toNo) == TRUE) {
      ToList[0] = toNo;
      ToList[1] = -1;
    } else {
      ToList[0] = -1;
      ToList[1] = -1;
    }
  } else if (toNo == TARGET_SIDE_0) {
    for (j = 0, i = 0; i < SIDE_OFFSET; i++) {
      if (BATTLE_TargetCheckDead(battleindex, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
  } else if (toNo == TARGET_SIDE_1) {
    for (j = 0, i = SIDE_OFFSET; i < SIDE_OFFSET * 2; i++) {
      if (BATTLE_TargetCheckDead(battleindex, i) == TRUE) {
        ToList[j] = i;
        j++;
      }
    }
    ToList[j] = -1;
  } else {
    if (toNo == TARGET_ALL) {
#ifdef _FIX_ARRAYBUG
      for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++) {
#else
      for (j = 0, i = 0; i < SIDE_OFFSET * 2; i++, j++) {
#endif
        if (BATTLE_TargetCheckDead(battleindex, i) == TRUE) {
          ToList[j] = i;
          j++;
        }
      }
#ifdef _FIX_ITEMRELIFE // WON ADD 修正替身娃娃问题
      ToList[j] = -1;
#else
      ToList[i] = -1;
#endif
    } else {
      ToList[0] = toNo;
      ToList[1] = -1;
    }
  }
}

BOOL BATTLE_IsThrowWepon(int itemindex) {
  int itemtype;
  if (ITEM_CHECKINDEX(itemindex) == FALSE) {
    return FALSE;
  }
  itemtype = ITEM_getInt(itemindex, ITEM_TYPE);
  if (itemtype == ITEM_BOOMERANG || itemtype == ITEM_BOW ||
      itemtype == ITEM_BREAKTHROW || itemtype == ITEM_BOUNDTHROW) {
    return TRUE;
  } else {
    return FALSE;
  }
}

int BATTLE_ClearGetExp(int charaindex) {
  int i, pindex;

  if (CHAR_CHECKINDEX(charaindex) == FALSE) {
    return BATTLE_ERR_CHARAINDEX;
  }
  CHAR_setWorkInt(charaindex, CHAR_WORKGETEXP, 0);
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    pindex = CHAR_getCharPet(charaindex, i);
    if (CHAR_CHECKINDEX(pindex) == FALSE)
      continue;
    CHAR_setWorkInt(pindex, CHAR_WORKGETEXP, 0);
  }

  return 0;
}

void _BATTLE_ExitAll(char *file, int line, int battleindex) {
  int j, i, charaindex;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      charaindex = BattleArray[battleindex].Side[j].Entry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
      BATTLE_Exit(charaindex, battleindex);
    }
  }
}

void BATTLE_AllCharaFinishSet(int battleindex) {
  int j, i, charaindex;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      charaindex = BattleArray[battleindex].Side[j].Entry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_FINAL);
    }
  }
}

BOOL BATTLE_IsCharge(int charaindex) {
  int com;
  com = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
  if (com == BATTLE_COM_S_CHARGE || com == BATTLE_COM_S_EARTHROUND1 ||
      com == BATTLE_COM_S_EARTHROUND0
#ifdef _PROFESSION_ADDSKILL
      || CHAR_getWorkInt(charaindex, CHAR_DOOMTIME) > 0
#endif
  ) {
    return TRUE;
  }
  return FALSE;
}

void BATTLE_AllCharaCWaitSet(int battleindex) {
  int j, i, charaindex;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      charaindex = BattleArray[battleindex].Side[j].Entry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;

      if (BATTLE_IsCharge(charaindex) == TRUE) {
      } else {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      }
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_WAIT);
    }
  }
}

void BATTLE_AllCharaWatchWaitSet(int battleindex) {
  int i, charaindex;
  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = BattleArray[battleindex].Side[0].Entry[i].charaindex;
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;

    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
        BATTLE_CHARMODE_WATCHINIT) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_WAIT);
    }
  }
}

void BATTLE_SurpriseSet(int battleindex, int side) {
  BATTLE_ENTRY *pEntry;
  int i, charaindex;

  pEntry = BattleArray[battleindex].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = pEntry[i].charaindex;
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
    }
  }
}

BOOL BATTLE_initBattleArray(int battlenum) {
  int i;
  BATTLE_battlenum = battlenum;
  BattleArray = allocateMemory(sizeof(BATTLE) * BATTLE_battlenum);
  if (BattleArray == NULL)
    return FALSE;
  for (i = 0; i < BATTLE_battlenum; i++) {
    memset(&BattleArray[i], 0, sizeof(BATTLE));
  }
  print("分配 %4.2f MB 空间...",
        ((float)sizeof(Char) * BATTLE_battlenum) / 1024.0 / 1024.0);
  return TRUE;
}

static void EntryInit(BATTLE_ENTRY *pEntry) {
  int i;
  pEntry->charaindex = -1;
  pEntry->bid = -1;
  pEntry->escape = 0;
  for (i = 0; i < GETITEM_MAX; i++) {
    pEntry->getitem[i] = -1;
  }
#ifdef BOSS_AI
  pEntry->hatred = 0;
  pEntry->addhpskill = 0;
  pEntry->purgeflg = 0;
  pEntry->proskilluse = 0;
#endif
}

int BATTLE_CreateBattle(void) {
  int battleindex, i, j;
  BATTLE *pBattle;
  battleindex = BATTLE_SearchTask();
  // NUKE 0701
  if (battleindex < 0)
    return -1;
  memset(&BattleArray[battleindex], 0, sizeof(BATTLE));
  pBattle = &BattleArray[battleindex];
  // NUKE 0701
  if (pBattle == NULL)
    return -1;
  pBattle->use = TRUE;
  pBattle->mode = BATTLE_MODE_INIT;
  pBattle->turn = 0;
  pBattle->dpbattle = 0;
  pBattle->norisk = 0;
  pBattle->flg = 0;
  pBattle->field_att = BATTLE_ATTR_NONE;
  pBattle->att_count = 0;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      EntryInit(&pBattle->Side[j].Entry[i]);
    }
  }
  for (i = 0; i < BATTLE_ENTRY_MAX * 2; i++) {
    pBattle->iEntryBack[i] = -1;
    pBattle->iEntryBack2[i] = -1;
  }
  pBattle->WinFunc = NULL;
  pBattle->pNext = NULL;
  pBattle->pBefore = NULL;
  pBattle->battleindex = battleindex;
  Total_BattleNum++;
#ifdef _BATTLECOMMAND_TIME
  pBattle->PartTime = 0;
#endif
#ifdef _PROFESSION_ADDSKILL
  //冰爆术存放
/*	pBattle->ice_count = 0;
        for( i = 0; i < 20; i ++ ){
                pBattle->ice_bout[i]=0;
                pBattle->ice_use[i]=FALSE;
                pBattle->ice_toNo[i]=0;
                pBattle->ice_level[i]=0;
                pBattle->ice_array[i]=0;
                pBattle->ice_attackNo[i]=0;
                pBattle->ice_charaindex[i]=-1;
        }*/
#endif
#ifdef _BATTLE_BOUT_TIME
  pBattle->tv_sec = NowTime.tv_sec;
  pBattle->tv_usec = NowTime.tv_usec;
#endif
  pBattle->topbattleindex = -1;
  return battleindex;
}

static int BATTLE_SearchTask(void) {
  int i, j;
  i = BATTLE_searchCnt;
  for (j = 0; j < BATTLE_battlenum; j++) {
    if (i >= BATTLE_battlenum)
      i = 0;
    if (BattleArray[i].use == FALSE) {
      BATTLE_searchCnt = i + 1;
      return i;
    }
    i++;
  }
  return -1;
}

void BATTLE_DeleteItem(int battleindex) {
  int i, j, k;
  BATTLE_ENTRY *pEntry;

  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      pEntry = &BattleArray[battleindex].Side[j].Entry[i];
      for (k = 0; k < GETITEM_MAX; k++) {
        if (pEntry->getitem[k] >= 0) {
          //					print( "// 找寻(消去)道具(%d),(%s)\n",
          //pEntry->getitem[k], ITEM_getAppropriateName(pEntry->getitem[k]) );
          ITEM_endExistItemsOne(pEntry->getitem[k]);
        }
      }
    }
  }
}

int BATTLE_DeleteBattle(int battleindex) {
  int i, j;
  BATTLE *pBattle;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    fprint("err:battle index error\n");
    return BATTLE_ERR_BATTLEINDEX;
  }
  if (BATTLE_WatchUnLink(battleindex) == FALSE) {
    fprint("err:battle link 不脱离\n");
  }

  pBattle = &BattleArray[battleindex];
  pBattle->use = FALSE;
  pBattle->mode = BATTLE_MODE_NONE;
  BATTLE_DeleteItem(battleindex);
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      EntryInit(&pBattle->Side[j].Entry[i]);
    }
  }
  Total_BattleNum--;
  return 0;
}

int BATTLE_No2Index(int battleindex, int bid) {
  BATTLE_ENTRY *pEntry;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -1;
  if (BATTLE_CHECKNO(bid) == FALSE)
    return -1;

  if (bid >= 10) {
    pEntry = BattleArray[battleindex].Side[1].Entry;
    bid -= SIDE_OFFSET;
  } else {
    pEntry = BattleArray[battleindex].Side[0].Entry;
  }
  if (CHAR_CHECKINDEX(pEntry[bid].charaindex) == FALSE)
    return -1;

  return pEntry[bid].charaindex;
}
#ifdef _Item_ReLifeAct
int BATTLE_getBattleDieIndex(int battleindex, int bid) {
  BATTLE_ENTRY *pEntry;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -1;
  if (BATTLE_CHECKNO(bid) == FALSE)
    return -1;

  if (bid >= 10) {
    pEntry = BattleArray[battleindex].Side[1].Entry;
    bid -= SIDE_OFFSET;
  } else {
    pEntry = BattleArray[battleindex].Side[0].Entry;
  }
  if (CHAR_CHECKINDEX(pEntry[bid].charaindex) == FALSE)
    return -1;
  if (pEntry[bid].flg & BENT_FLG_ULTIMATE)
    return -1;

  return pEntry[bid].charaindex;
}
#endif

#ifdef _PROFESSION_ADDSKILL
BOOL BATTLE_BattleUltimate(int battleindex, int bid) //检查此位D上是否被打飞
{
  BATTLE_ENTRY *pEntry;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return FALSE;
  if (BATTLE_CHECKNO(bid) == FALSE)
    return FALSE;
  if (bid >= 10) {
    pEntry = BattleArray[battleindex].Side[1].Entry;
    bid -= SIDE_OFFSET;
  } else {
    pEntry = BattleArray[battleindex].Side[0].Entry;
  }
  if (pEntry[bid].flg & BENT_FLG_ULTIMATE)
    return FALSE;
  else
    return TRUE;
}
#endif

int BATTLE_Index2No(int battleindex, int charaindex) {
  BATTLE_ENTRY *pEntry;
  int i, j;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -1;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return -1;

  for (j = 0; j < 2; j++) {
    pEntry = BattleArray[battleindex].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      if (pEntry[i].charaindex == charaindex) {
        return i + j * SIDE_OFFSET;
      }
    }
  }

  return -1;
}

int BATTLE_ToRandNo(int battleindex, int flg) {

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -1;
  if (flg < 0 || flg > 1)
    return -1;
  int k = 0, j = 0, toNo = -1, l, index;
  int ToNoList[10];
  memset(ToNoList, -1, sizeof(ToNoList));
  for (k = 10 * flg; k < 10 * (flg + 1); k++) {
    index = BATTLE_No2Index(battleindex, k);
    if (!CHAR_CHECKINDEX(index))
      continue;
    if (CHAR_getInt(index, CHAR_HP) <= 0)
      continue;
    ToNoList[j] = k;
    j++;
  }
  if (j == 0)
    return -1;
  toNo = ToNoList[l = RAND(0, j - 1)];
  if (toNo < 0 || toNo > 20)
    return -1;
  return toNo;
}

char *BATTLE_CharTitle(int charaindex) {
  char *pName;
  static char Len0Buff[2] = "";

  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    pName = Len0Buff;
  } else {
    pName = CHAR_getChar(charaindex, CHAR_OWNTITLE);
    if (pName[0] == 0) {
      pName = Len0Buff;
    }
  }
  return pName;
}

int BATTLE_CharaBackUp(int battleindex) {
  int i, j, k, charaindex;
  BATTLE *pBattle;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    return BATTLE_ERR_BATTLEINDEX;
  }

  pBattle = &BattleArray[battleindex];
  k = 0;

  for (j = 0; j < 2; j++) {
    k = j * SIDE_OFFSET;
    for (i = 0; i < BATTLE_ENTRY_MAX / 2; i++) {
      charaindex = pBattle->Side[j].Entry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == TRUE &&
#ifdef _GM_HELPER /// 颇萍盔 促 磷绢档 傍拜啊霸
          (
#endif
              CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _GM_HELPER
              || CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC)
#endif

      ) {
        pBattle->iEntryBack[k] = pBattle->Side[j].Entry[i].charaindex;
        pBattle->iEntryBack2[k] = pBattle->Side[j].Entry[i].charaindex;
      } else {
        pBattle->iEntryBack[k] = -1;
        pBattle->iEntryBack2[k] = -1;
      }
      k++;
    }
  }

  return 0;
}

int BATTLE_NewEntry(int charaindex, int battleindex, int side) {
  BATTLE_ENTRY *pEntry;
  BATTLE *pBattle;
  int i, iEntryMax, iEntryFirst;

  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  pBattle = &BattleArray[battleindex];

  if (pBattle->use == FALSE)
    return BATTLE_ERR_NOUSE;

  if (side < 0 || side > 1) {
    return BATTLE_ERR_PARAM;
  }

  switch (CHAR_getInt(charaindex, CHAR_WHICHTYPE)) {
    int work;

  case CHAR_TYPEPLAYER:
    if (pBattle->Side[side].type != BATTLE_S_TYPE_PLAYER) {
      return BATTLE_ERR_TYPE;
    }
    iEntryFirst = 0;
    iEntryMax = BATTLE_PLAYER_MAX;
    break;
  case CHAR_TYPEENEMY:
    if (pBattle->Side[side].type != BATTLE_S_TYPE_ENEMY) {
      return BATTLE_ERR_TYPE;
    }
    iEntryFirst = 0;
    iEntryMax = BATTLE_ENTRY_MAX;
    break;
  case CHAR_TYPEPET:
    if (pBattle->Side[side].type != BATTLE_S_TYPE_PLAYER) {
      return BATTLE_ERR_TYPE;
    }

    work = CHAR_getWorkInt(charaindex, CHAR_WORKPLAYERINDEX);
    pEntry = pBattle->Side[side].Entry;

    work = BATTLE_Index2No(battleindex, work);
    work -= side * SIDE_OFFSET;
    work += 5;
    iEntryFirst = work;
    iEntryMax = work + 1;
    break;
#ifdef _PLAYER_NPC
  case CHAR_TYPEPLAYERNPC:
    iEntryFirst = 0;
    iEntryMax = BATTLE_ENTRY_MAX;
    break;

  case CHAR_TYPEPLAYERPETNPC:
    work = CHAR_getWorkInt(charaindex, CHAR_WORKPLAYERINDEX);
    pEntry = pBattle->Side[side].Entry;

    work = BATTLE_Index2No(battleindex, work);
    work -= side * SIDE_OFFSET;
    work += 5;
    iEntryFirst = work;
    iEntryMax = work + 1;
    break;
#endif
  default:
    print("不明(%s)(%d)\n", CHAR_getUseName(charaindex),
          CHAR_getInt(charaindex, CHAR_WHICHTYPE));
    return BATTLE_ERR_TYPE;
  }

  pEntry = pBattle->Side[side].Entry;

  for (i = iEntryFirst; i < iEntryMax; i++) {
    if (pEntry[i].charaindex == -1)
      break;
  }

  if (i == iEntryMax) {
    return BATTLE_ERR_ENTRYMAX;
  }

  EntryInit(&pEntry[i]);

  pEntry[i].charaindex = charaindex;

  pEntry[i].bid = i + side * SIDE_OFFSET;

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEINDEX, battleindex);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_INIT);

  CHAR_setWorkInt(charaindex, CHAR_WORKDBATTLEESCAPE, 0);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLESIDE, side);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEFLG, 0);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, -1);
  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, -1);
  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM3, -1);

  CHAR_setWorkInt(charaindex, CHAR_WORKMODATTACK, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKMODDEFENCE, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKMODQUICK, 0);

  CHAR_setWorkInt(charaindex, CHAR_WORKDAMAGEABSROB, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKDAMAGEREFLEC, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKDAMAGEVANISH, 0);

  CHAR_setWorkInt(charaindex, CHAR_WORKMODCAPTURE, 0);

  CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);

  BATTLE_BadStatusAllClr(charaindex);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEWATCH, 0);

#ifdef _PROFESSION_SKILL                         // WON ADD 人物职业技能
  CHAR_setWorkInt(charaindex, CHAR_WORKTRAP, 0); //陷阱
  BATTLE_ProfessionStatus_init(battleindex, charaindex);
#endif

#ifdef _PETSKILL_ACUPUNCTURE
  CHAR_setWorkInt(charaindex, CHAR_WORKACUPUNCTURE, 0); //针刺外皮
#endif

#ifdef _PETSKILL_RETRACE
  CHAR_setWorkInt(charaindex, CHAR_WORKRETRACE, 0); //追迹攻击
#endif

#ifdef _PETSKILL_BECOMEFOX //将媚惑术中招时间清除为-1
  CHAR_setWorkInt(charaindex, CHAR_WORKFOXROUND, -1);
#endif
#ifdef _PROFESSION_ADDSKILL
  //结界清除
  CHAR_setWorkInt(charaindex, CHAR_WORKFIXEARTHAT_BOUNDARY, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKFIXWATERAT_BOUNDARY, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKFIXFIREAT_BOUNDARY, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKFIXWINDAT_BOUNDARY, 0);
  CHAR_setWorkInt(charaindex, CHAR_DOOMTIME, 0); //世界末日集气
  CHAR_setWorkInt(charaindex, CHAR_WORK_com1, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_toNo, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_mode, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_skill_level, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_array, 0);
#endif
#ifdef _PETSKILL_ADDATTCRAZED
  CHAR_setWorkInt(charaindex, CHAR_WORK_ADDATTCRAZED, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_ADDATTCRAZED2, 0);
#endif

#ifdef _PETSKILL_PROVOKEFIGHT
  CHAR_setWorkInt(charaindex, CHAR_WORK_PROVOKEFIGHT, 0);
#endif
#ifdef _PETSKILL_RIDE
  CHAR_setWorkInt(charaindex, CHAR_WORK_RIDE, MAKE2VALUE(-1, 0));
#endif

#ifdef _PRO_ABSOLUTE_DEFENSE
  CHAR_setWorkInt(charaindex, CHAR_WORK_ABSOLUTE, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_ABSOLUTE2, 0);
#endif

#ifdef _PRO3_ADDSKILL
  CHAR_setWorkInt(charaindex, CHAR_WORK_CRACKBONE, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_CRACKBONE2, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_POISON_DRAGNET, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_PACIFY, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_DARK_SEAL, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_FIRST_AID, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_ROAR, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_MEDICATION, 0);
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
    CHAR_sendStatusString(charaindex, "G");
#endif
  return 0;
}

int _BATTLE_Exit(char *file, int line, int charaindex, int battleindex) {
  BATTLE_ENTRY *pEntry;
  int i, j, k;
  BATTLE *pBattle;
  char szPet[32];
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    return BATTLE_ERR_BATTLEINDEX;
  }

  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    int fd = getfdFromCharaIndex(charaindex);
    if (fd != -1) {
      // lssproto_BATTLETIME_send(fd,99);
    }
#ifdef _BATTLE_OVER_STAR_TIME
    CHAR_setWorkInt(charaindex, CHAR_WORKDBATTLETIME, time(NULL));
#endif
#ifdef _ALLBLUES_LUA_1_4
    BattleFinish(battleindex, charaindex);
#endif
  }
  BattleArray[battleindex].EndTime =
      time(NULL) - BattleArray[battleindex].CreateTime;

#ifdef _PROFESSION_ADDSKILL
  CHAR_setWorkInt(charaindex, CHAR_DOOMTIME, 0); //世界末日集气
#endif
#ifdef _PETSKILL_BECOMEFOX // 离开战斗时将媚惑术造成的变身效果清除
  // print("\n检查图号:%s,%d", CHAR_getChar( charaindex, CHAR_NAME),
  // CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER));

  if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 101749 ||
      CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) != -1) { //变成小狐狸
    CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
                CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER));
    CHAR_setWorkInt(charaindex, CHAR_WORKFOXROUND, -1);
    // print("\n变回去:%d",CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER));
  }
#endif

#ifdef _PETSKILL_BECOMEPIG
  if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1 &&
      CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
                CHAR_getInt(charaindex, CHAR_BECOMEPIG_BBI));
    CHAR_complianceParameter(charaindex);
    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
    CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  }
#endif

#ifdef _MAGIC_NOCAST // 精灵:沉默
  // WON ADD
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
    lssproto_NC_send(getfdFromCharaIndex(charaindex), 0); //复原
#endif
  pBattle = &BattleArray[battleindex];
  if (pBattle->use == FALSE)
    return BATTLE_ERR_NOUSE;

  for (j = 0; j < 2; j++) {
    pEntry = pBattle->Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      if (pEntry[i].charaindex != charaindex)
        continue;

#ifdef _LUA_BATTLEOVERCALL
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
          BattleArray[battleindex].type != BATTLE_TYPE_WATCH) {
        int alive = CHAR_getInt(charaindex, CHAR_HP) > 0 ? 1 : 0;
        if (pEntry[i].escape)
          alive = 0;
        if (CHAR_getFlg(charaindex, CHAR_ISDIE))
          alive = 0;
        FreeBattleOverCall(
            charaindex, alive, time(NULL) - BattleArray[battleindex].CreateTime,
            BattleArray[battleindex].turn, BattleArray[battleindex].type);
      }
#endif

      pEntry[i].charaindex = -1;
      pEntry[i].escape = 0;
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_FINAL);
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEINDEX, -1);

#ifdef _NB_NOBATTLEBUTTON
      CHAR_setWorkInt(charaindex, CHAR_WORK_NOHELP, 0);
      CHAR_setWorkInt(charaindex, CHAR_WORK_NOESCAPE, 0);
      lssproto_BattleButton_send(getfdFromCharaIndex(charaindex), 0, 0);
#endif

#ifdef _LOSE_FINCH_
      CHAR_setWorkInt(charaindex, CHAR_WORKSPETRELIFE, 0);
#endif
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
        CHAR_endCharOneArray(charaindex);
#ifdef _PLAYER_NPC
      } else if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) ==
                     CHAR_TYPEPLAYERNPC ||
                 CHAR_getInt(charaindex, CHAR_WHICHTYPE) ==
                     CHAR_TYPEPLAYERPETNPC) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEINDEX, -1);
        {
          if (CHAR_getInt(charaindex, CHAR_HP) <
              CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP) * 0.8) {
            CHAR_setInt(charaindex, CHAR_HP,
                        CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP));
            CHAR_setInt(charaindex, CHAR_MP,
                        CHAR_getWorkInt(charaindex, CHAR_WORKMAXMP));
          }
          for (i = 0; i < CHAR_MAXPETHAVE; i++) {
            int petindex = CHAR_getCharPet(charaindex, i);
            if (CHAR_CHECKINDEX(petindex) == FALSE)
              continue;
            CHAR_setFlg(petindex, CHAR_ISDIE, 0);
            if (CHAR_getInt(petindex, CHAR_HP) <
                CHAR_getWorkInt(petindex, CHAR_WORKMAXHP) * 0.8) {
              CHAR_setInt(petindex, CHAR_HP,
                          CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));
              CHAR_setInt(petindex, CHAR_MP,
                          CHAR_getWorkInt(petindex, CHAR_WORKMAXMP));
            }
          }
        }
        if (CHAR_getFlg(charaindex, CHAR_ISDIE) == TRUE) {
          CHAR_setFlg(charaindex, CHAR_ISDIE, FALSE);
        }
        BATTLE_BadStatusAllClr(charaindex);
        CHAR_complianceParameter(charaindex);
        CHAR_sendBattleEffect(charaindex, OFF);
#endif
      } else if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
#ifdef _OFFLINE_SYSTEM
        if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) != 0) {
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE,
                          BATTLE_CHARMODE_NONE);
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEINDEX, -1);
          CHAR_sendBattleEffect(charaindex, OFF);
        }
#endif

#ifdef _PET_ITEM
        if (CHAR_CHECKINDEX(CHAR_getCharPet(charaindex, 0))) {
          CHAR_sendPetItemData(charaindex, 0);
        }
        if (CHAR_CHECKINDEX(CHAR_getCharPet(charaindex, 1))) {
          CHAR_sendPetItemData(charaindex, 1);
        }
        if (CHAR_CHECKINDEX(CHAR_getCharPet(charaindex, 2))) {
          CHAR_sendPetItemData(charaindex, 2);
        }
        if (CHAR_CHECKINDEX(CHAR_getCharPet(charaindex, 3))) {
          CHAR_sendPetItemData(charaindex, 3);
        }
        if (CHAR_CHECKINDEX(CHAR_getCharPet(charaindex, 4))) {
          CHAR_sendPetItemData(charaindex, 4);
        }
#endif

        int petindex = pEntry[i + 5].charaindex;
        if (CHAR_getFlg(charaindex, CHAR_ISDIE) == TRUE) {

#ifdef _FIX_SPEED_UPLEVEL // WON ADD 修正加速
                          // Robin cancel
                          // if( CHAR_getFlg( charaindex, CHAR_ISDIE ) == TRUE
                          // ){ }else
          {
#ifdef _OFFLINE_SYSTEM
            if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) == 0)
#endif
            {
              unsigned int Dtimes;
              unsigned int battletime;
              int fd = getfdFromCharaIndex(charaindex);
              if (CONNECT_checkfd(fd) == TRUE) {
                Dtimes = BattleArray[battleindex].CreateTime;
                battletime =
                    (unsigned int)(BattleArray[battleindex].flgTime / 100);

                // if( CHAR_getWorkInt( charaindex, CHAR_WORKFLG) &
                // WORKFLG_DEBUGMODE ){ }else
                {
                  CheckDefBTime(charaindex, fd, Dtimes, battletime,
                                2); // lowTime延迟时间
                }
              }
            }
          }
#endif
          CHAR_setFlg(charaindex, CHAR_ISDIE, 0);
          if (CHAR_getInt(charaindex, CHAR_HP) <= 0) {
            CHAR_setInt(charaindex, CHAR_HP, 1);
          }
          // CoolFish: Faily PK 2001/8/3
          CHAR_setWorkInt(charaindex, CHAR_WORKFMPKFLAG, -1);

        } else {
#ifdef _BATTLE_TIMESPEED
          // Robin cancel
#ifdef _OFFLINE_SYSTEM
          if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) == 0)
#endif
          {
            unsigned int Dtimes;
            unsigned int battletime;

            int fd = getfdFromCharaIndex(charaindex);
            if (CONNECT_checkfd(fd) == TRUE) {
              Dtimes = BattleArray[battleindex].CreateTime;
              battletime =
                  (unsigned int)(BattleArray[battleindex].flgTime / 100);
              // if( CHAR_getWorkInt( charaindex, CHAR_WORKFLG) &
              // WORKFLG_DEBUGMODE )	{ }else
              {
                CheckDefBTime(charaindex, fd, Dtimes, battletime,
                              0); // lowTime延迟时间
              }
            }
          }
#endif
        }
        if (CHAR_CHECKINDEX(petindex) == TRUE) {
          pEntry[i + 5].charaindex = -1;
          CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
          CHAR_setWorkInt(petindex, CHAR_WORKBATTLEINDEX, -1);
        }
        BATTLE_BadStatusAllClr(charaindex);
        CHAR_complianceParameter(charaindex);
        CHAR_Skillupsend(charaindex);
        // Robin 0730
        if (CHAR_getWorkInt(charaindex, CHAR_WORKPETFALL)) {
          CHAR_setWorkInt(charaindex, CHAR_WORKPETFALL, 0);
          CHAR_setInt(charaindex, CHAR_RIDEPET, -2);
        }
        CHAR_send_P_StatusString(
            charaindex, CHAR_P_STRING_HP | CHAR_P_STRING_EXP |
                            CHAR_P_STRING_MP | CHAR_P_STRING_DUELPOINT |
                            CHAR_P_STRING_CHARM | CHAR_P_STRING_EARTH |
                            CHAR_P_STRING_WATER | CHAR_P_STRING_FIRE |
                            CHAR_P_STRING_WIND | CHAR_P_STRING_RIDEPET);
        // Robin 0730
        if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -2) {
          CHAR_setInt(charaindex, CHAR_RIDEPET, -1);
        }
        for (k = 0; k < CHAR_MAXPETHAVE; k++) {
          petindex = CHAR_getCharPet(charaindex, k);
          if (CHAR_CHECKINDEX(petindex) == FALSE)
            continue;
          if (CHAR_getInt(petindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE)
            continue;
          if (CHAR_getFlg(petindex, CHAR_ISDIE) == TRUE ||
              CHAR_getInt(petindex, CHAR_HP) <= 0) {
            CHAR_setFlg(petindex, CHAR_ISDIE, 0);
            CHAR_setInt(petindex, CHAR_HP, 1);
          }
          CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
#ifdef _LOSE_FINCH_
          CHAR_setWorkInt(charaindex, CHAR_WORKSPETRELIFE, 0);
#endif
#ifdef _VARY_WOLF
          if (CHAR_getInt(petindex, CHAR_BASEBASEIMAGENUMBER) !=
              CHAR_getInt(petindex, CHAR_BASEIMAGENUMBER)) // Robin fix
          {
            CHAR_setInt(petindex, CHAR_BASEIMAGENUMBER,
                        CHAR_getInt(petindex, CHAR_BASEBASEIMAGENUMBER));
            sprintf(szPet, "K%d", k);
            CHAR_sendStatusString(charaindex, szPet);
          }
#endif
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
          // 遗忘
          if (CHAR_getWorkInt(petindex, CHAR_WORKOBLIVION) > 0) // Robin fix
          {
            int pet_no = CHAR_getInt(charaindex, CHAR_DEFAULTPET);
            CHAR_setWorkInt(petindex, CHAR_WORKOBLIVION, 0);
            // 宠物编号
            sprintf(szPet, "W%d", pet_no);
            CHAR_sendStatusString(charaindex, szPet);
          }
#endif
          BATTLE_BadStatusAllClr(petindex);
          CHAR_complianceParameter(petindex);
          CHAR_send_K_StatusString(
              charaindex, k,
              CHAR_K_STRING_HP | CHAR_K_STRING_EXP | CHAR_K_STRING_AI |
                  CHAR_K_STRING_MP | CHAR_K_STRING_ATK | CHAR_K_STRING_DEF |
                  CHAR_K_STRING_QUICK | CHAR_K_STRING_EARTH |
                  CHAR_K_STRING_WATER | CHAR_K_STRING_FIRE |
                  CHAR_K_STRING_WIND);
        }
        if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P) {
          int flg = 0;
          CHAR_setFlg(charaindex, CHAR_ISDUEL, 0);
          if (CHAR_getFlg(charaindex, CHAR_ISPARTY))
            flg |= CHAR_FS_PARTY;
          if (CHAR_getFlg(charaindex, CHAR_ISDUEL))
            flg |= CHAR_FS_DUEL;
          if (CHAR_getFlg(charaindex, CHAR_ISPARTYCHAT))
            flg |= CHAR_FS_PARTYCHAT;
          if (CHAR_getFlg(charaindex, CHAR_ISTRADECARD))
            flg |= CHAR_FS_TRADECARD;
#ifdef _CHANNEL_MODIFY
          if (CHAR_getFlg(charaindex, CHAR_ISTELL))
            flg |= CHAR_FS_TELL;
          if (CHAR_getFlg(charaindex, CHAR_ISFM))
            flg |= CHAR_FS_FM;
#ifdef _VERSION_NEW
          if (CHAR_getFlg(charaindex, CHAR_ISOCC))
            flg |= CHAR_FS_WORLD;
#else
          if (CHAR_getFlg(charaindex, CHAR_ISOCC))
            flg |= CHAR_FS_OCC;
#endif
          if (CHAR_getFlg(charaindex, CHAR_ISCHAT))
            flg |= CHAR_FS_CHAT;
          if (CHAR_getFlg(charaindex, CHAR_ISSAVE))
            flg |= CHAR_FS_SAVE;
#ifdef _CHATROOMPROTOCOL
#ifdef _THE_WORLD_SEND
          if (CHAR_getFlg(charaindex, CHAR_ISWORLD))
            flg |= CHAR_FS_WORLD;
#endif
#endif
#endif
          lssproto_FS_send(getfdFromCharaIndex(charaindex), flg);
        }

        lssproto_XYD_send(
            getfdFromCharaIndex(charaindex), CHAR_getInt(charaindex, CHAR_X),
            CHAR_getInt(charaindex, CHAR_Y), CHAR_getInt(charaindex, CHAR_DIR));
      }
      break;
    }
  }
  CHAR_PartyUpdate(charaindex, CHAR_N_STRING_HP);

#ifdef _TIME_TICKET
  if (CHAR_CHECKINDEX(charaindex) == TRUE) {
    int nowtime = time(NULL);
    int tickettime = CHAR_getWorkInt(charaindex, CHAR_WORKTICKETTIME);
    int totaltime;
    char msg[1024];
    if (tickettime > 0 && tickettime < nowtime) {
      // int floor, x, y;
      CHAR_talkToCli(charaindex, -1, "谢谢光顾。", CHAR_COLORYELLOW);
      if (CHAR_getWorkInt(charaindex, CHAR_WORKTICKETTIMESTART) > 0) {
        totaltime =
            nowtime - CHAR_getWorkInt(charaindex, CHAR_WORKTICKETTIMESTART);
        sprintf(msg, "总入场时间%d分%d秒。", totaltime / 60, totaltime % 60);
        CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
      }
      CHAR_setWorkInt(charaindex, CHAR_WORKTICKETTIME, 0);
      CHAR_setWorkInt(charaindex, CHAR_WORKTICKETTIMESTART, 0);
      /*if( CHAR_getElderPosition( CHAR_getInt( charaindex, CHAR_LASTTALKELDER),
      &floor, &x, &y ) != TRUE ) { CHAR_warpToSpecificPoint( charaindex, 1006,
      15, 15);
      }
      else {
              CHAR_warpToSpecificPoint( charaindex, floor, x, y);
      }*/
      CHAR_warpToSpecificPoint(charaindex, 2006, 20, 17);
    }
  }
#endif
  if (CHAR_CHECKINDEX(charaindex) == TRUE) {
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      if (BattleArray[battleindex].type != BATTLE_TYPE_WATCH) {
      }
#ifdef _OFFLINE_BATTLE_RETEUN
      if ((BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P ||
           (CHAR_getInt(charaindex, CHAR_FLOOR) >= 40000 &&
            CHAR_getInt(charaindex, CHAR_FLOOR) <= 40100)) &&
          CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) != 0) {
        CHAR_setWorkInt(charaindex, CHAR_WORK_OFFLINE, 0);
        CHAR_logout(charaindex, TRUE);
      }
#endif
    }
  }

  return BATTLE_ERR_NONE;
}

int BATTLE_PetDefaultExit(int charaindex, int battleindex) {
  int pno, pindex, iRet;

  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return 0;
  pno = CHAR_getInt(charaindex, CHAR_DEFAULTPET);
  if (pno < 0)
    return 0;
  pindex = CHAR_getCharPet(charaindex, pno);

  iRet = BATTLE_Exit(pindex, battleindex);

  if (iRet) {
    iRet *= -1;
  } else {
    return 1;
  }

  return iRet;
}

int BATTLE_PetDefaultEntry(int charaindex, int battleindex, int side) {
  int pindex;

#if 1
  int pno;

  int ret = 0;

  pno = CHAR_getInt(charaindex, CHAR_DEFAULTPET);
  if (pno == -1)
    return 0;
  pindex = CHAR_getCharPet(charaindex, pno);
  if (CHAR_CHECKINDEX(pindex) && !CHAR_getFlg(pindex, CHAR_ISDIE) &&
      CHAR_getInt(pindex, CHAR_HP) > 0) {
    if (BATTLE_NewEntry(pindex, battleindex, side)) {
    } else {
      ret = 0;
    }
  } else {
#ifdef _PLAYER_NPC
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC)
#endif
      CHAR_setInt(charaindex, CHAR_DEFAULTPET, -1);
  }

  return ret;
#else

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    pindex = CHAR_getCharPet(charaindex, i);

    if (CHAR_CHECKINDEX(pindex) == FALSE)
      continue;

    if (CHAR_getFlg(pindex, CHAR_ISDIE) == TRUE)
      continue;
    if (CHAR_getInt(pindex, CHAR_HP) <= 0)
      continue;

    if (BATTLE_NewEntry(pindex, battleindex, side)) {
    } else {
      CHAR_setInt(charaindex, CHAR_DEFAULTPET, i);
      break;
    }
  }

  if (i == CHAR_MAXPETHAVE) {
    CHAR_setInt(charaindex, CHAR_DEFAULTPET, -1);
  }
  return 0;
#endif
}

int BATTLE_RescueEntry(int charaindex, int toindex) {
  int iRet = 0, battleindex, fd, pindex;
  char szBuffer[256] = "";

#ifdef _BATTLE_TIMESPEED
  fd = getfdFromCharaIndex(charaindex);
  if (CONNECT_checkfd(fd) == TRUE) {
    if (CONNECT_getBDTime(fd) > time(NULL)) {
      return 1;
    }
  }
#endif

  battleindex = CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX);
  if (!BATTLE_CHECKINDEX(battleindex))
    return 1;
  if ((fd = getfdFromCharaIndex(charaindex)) < 0)
    return 1;

  if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL ||
      CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX) == -1) {
    iRet = 1;
  }
#ifndef _FIX_P_VS_P_PARENT
  if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P) {
    iRet = 1;
  } else
#endif
  {
    iRet = BATTLE_NewEntry(charaindex, battleindex,
                           CHAR_getWorkInt(toindex, CHAR_WORKBATTLESIDE));
  }

  if (iRet == 0) {
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE);
    iRet = BATTLE_PetDefaultEntry(
        charaindex, battleindex, CHAR_getWorkInt(toindex, CHAR_WORKBATTLESIDE));
  }

  if (iRet == 0) {
    int flg;
    if (CHAR_getInt(charaindex, CHAR_DEFAULTPET) >= 0) {
      pindex =
          CHAR_getCharPet(charaindex, CHAR_getInt(charaindex, CHAR_DEFAULTPET));
      CHAR_setWorkInt(pindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE);
    }
    // snprintf( szBuffer, sizeof( szBuffer ), "(%s)加入作战。",
    //	CHAR_getUseName( charaindex ) );
    // BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORYELLOW );
    if ((fd = getfdFromCharaIndex(charaindex)) != -1) {
      lssproto_EN_send(fd, BattleArray[battleindex].type,
                       BattleArray[battleindex].field_no);
    }
    flg = (BattleArray[battleindex]
               .Side[CHAR_getWorkInt(toindex, CHAR_WORKBATTLESIDE)]
               .flg &
           BSIDE_FLG_HELP_OK)
              ? TRUE
              : FALSE;
    lssproto_HL_send(fd, flg);
    sprintf(szBuffer, "BP|%X|%X|%X", BATTLE_Index2No(battleindex, charaindex),
            BP_FLG_JOIN, CHAR_getInt(charaindex, CHAR_MP));
    BATTLE_CommandSend(charaindex, szBuffer);
  } else {
    snprintf(szBuffer, sizeof(szBuffer), "无法参战。");
    CHAR_talkToCli(charaindex, -1, szBuffer, CHAR_COLORYELLOW);
    if ((fd = getfdFromCharaIndex(charaindex)) != -1) {
      lssproto_EN_send(fd, FALSE, BattleArray[battleindex].field_no);
    }
  }
  BATTLE_ClearGetExp(charaindex);
  return iRet;
}

BOOL BATTLE_RescueTry(int charaindex) {
  int x, y;
  OBJECT object;
  int pfound = FALSE;
  int result = FALSE;
  int fd;
  int i;
  int cnt;

  fd = getfdFromCharaIndex(charaindex);
  if (CONNECT_checkfd(fd) == FALSE)
    return FALSE;
#if 1 // 修正利用参战重复加入战斗	Robin
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex, -1, " 重复加入战斗! ", CHAR_COLORRED);
    print("\n 改封包!!重复加入战斗!:%s ", CHAR_getChar(charaindex, CHAR_CDKEY));
    // lssproto_EN_send( fd, FALSE, 0 );
    return FALSE;
  }
#endif
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    lssproto_EN_send(fd, FALSE, 0);
    return FALSE;
  }
#ifdef _BATTLE_TIMESPEED
  if (CONNECT_getBDTime(fd) > time(NULL)) {
    return FALSE;
  }
#endif
  for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
    CONNECT_setBattlecharaindex(fd, i, -1);
  }
  CHAR_getCoordinationDir(CHAR_getInt(charaindex, CHAR_DIR),
                          CHAR_getInt(charaindex, CHAR_X),
                          CHAR_getInt(charaindex, CHAR_Y), 1, &x, &y);

  cnt = 0;
  for (object = MAP_getTopObj(CHAR_getInt(charaindex, CHAR_FLOOR), x, y);
       object; object = NEXT_OBJECT(object)) {
    int toindex;
    int objindex = GET_OBJINDEX(object);
    if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
      continue;
    toindex = OBJECT_getIndex(objindex);
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
        && CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC
#endif
    )
      continue;
    pfound = TRUE;
    if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
      continue;
    }
    if (BattleArray[CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX)].type !=
        BATTLE_TYPE_P_vs_E) {
      continue;
    }
    if (!(BattleArray[CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX)]
              .Side[CHAR_getWorkInt(toindex, CHAR_WORKBATTLESIDE)]
              .flg &
          BSIDE_FLG_HELP_OK)) {
      continue;
    }
    CONNECT_setBattlecharaindex(fd, cnt, toindex);
    cnt++;
    if (cnt == CONNECT_WINDOWBUFSIZE)
      break;
  }
  if (cnt == 0) {
    if (pfound == FALSE) {
      CHAR_talkToCli(charaindex, -1, "那里没有任何人。", CHAR_COLORYELLOW);
    }
    lssproto_EN_send(fd, FALSE, 0);
  } else if (cnt == 1) {
    if (BATTLE_RescueEntry(charaindex, CONNECT_getBattlecharaindex(fd, 0))) {
      CHAR_talkToCli(charaindex, -1, "无法参战。", CHAR_COLORYELLOW);
    } else {
      result = TRUE;
    }
  } else if (cnt > 1) {
    int strlength;
    char msgbuf[1024];
    char escapebuf[2048];
    strcpy(msgbuf, "1\n加入谁的战斗呢？\n");
    strlength = strlen(msgbuf);
    for (i = 0;
         CONNECT_getBattlecharaindex(fd, i) != -1 && i < CONNECT_WINDOWBUFSIZE;
         i++) {
      char *a = CHAR_getChar(CONNECT_getBattlecharaindex(fd, i), CHAR_NAME);
      char buf[256];
      snprintf(buf, sizeof(buf), "%s\n", a);
      if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
        print("\nerr:%s:%d 视窗的讯息缓冲不足\n", __FILE__, __LINE__);
        break;
      }
      strcpy(&msgbuf[strlength], buf);
      strlength += strlen(buf);
    }
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                     CHAR_WINDOWTYPE_SELECTBATTLE, -1,
                     makeEscapeString(msgbuf, escapebuf, sizeof(escapebuf)));
  }

  return result;
}

BOOL BATTLE_RescueParentTry(int charaindex, int pindex) {
  if (CHAR_CHECKINDEX(charaindex) == FALSE) {
    return FALSE;
  }
  if (CHAR_CHECKINDEX(pindex) == FALSE)
    return FALSE;
  int result = FALSE;
  if (CHAR_getFlg(charaindex, CHAR_ISDIE))
    return FALSE;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1) != pindex)
    return FALSE;
  if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
    return FALSE;
  }
  if (BATTLE_RescueEntry(charaindex, pindex)) {
    result = FALSE;
  } else {
    result = TRUE;
  }
  if (result == FALSE) {
    CHAR_talkToCli(charaindex, -1, "无法参战。", CHAR_COLORYELLOW);
  }
  return result;
}

int BATTLE_PartyNewEntry(int charaindex, int battleindex, int side) {
  int iRet = 0, i, work;
#if 1

  iRet = BATTLE_NewEntry(charaindex, battleindex, side);
  if (iRet)
    return iRet;
  CAflush(charaindex);
  CDflush(charaindex);
  iRet = BATTLE_PetDefaultEntry(charaindex, battleindex, side);
  if (iRet)
    return iRet;
  BATTLE_ClearGetExp(charaindex);
#endif

  for (i = 1; i < CHAR_PARTYMAX; i++) {
    work = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(work) == FALSE)
      continue;
    if (CHAR_getWorkInt(work, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE &&
        CHAR_getWorkInt(work, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_FINAL) {
      continue;
    }
    iRet = BATTLE_NewEntry(work, battleindex, side);

    if (iRet)
      break;
    CAflush(work);
    CDflush(work);
    iRet = BATTLE_PetDefaultEntry(work, battleindex, side);

    if (iRet)
      return iRet;
    BATTLE_ClearGetExp(work);
  }

  return iRet;
}

#if 1

int BATTLE_WatchNewEntry(int charaindex, int battleindex, int side) {
  BATTLE_ENTRY *pEntry;
  BATTLE *pBattle;
  int iEntryFirst = 0, iEntryMax = BATTLE_ENTRY_MAX, i;

  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  pBattle = &BattleArray[battleindex];

  if (pBattle->use == FALSE)
    return BATTLE_ERR_NOUSE;

  if (side < 0 || side > 1) {
    return BATTLE_ERR_PARAM;
  }

  switch (CHAR_getInt(charaindex, CHAR_WHICHTYPE)) {

  case CHAR_TYPEPLAYER:
    if (pBattle->Side[side].type != BATTLE_S_TYPE_PLAYER) {
      return BATTLE_ERR_TYPE;
    }
    iEntryFirst = 0;
    iEntryMax = BATTLE_PLAYER_MAX;
    break;
  default:
    print("不明(%s)(%d)\n", CHAR_getUseName(charaindex),
          CHAR_getInt(charaindex, CHAR_WHICHTYPE));
    return BATTLE_ERR_TYPE;
  }

  pEntry = pBattle->Side[side].Entry;

  for (i = iEntryFirst; i < iEntryMax; i++) {
    if (pEntry[i].charaindex == -1)
      break;
  }

  if (i == iEntryMax) {
    return BATTLE_ERR_ENTRYMAX;
  }

  EntryInit(&pEntry[i]);

  pEntry[i].charaindex = charaindex;

  pEntry[i].bid = i + side * SIDE_OFFSET;

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEINDEX, battleindex);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_WATCHINIT);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLESIDE, side);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEFLG, 0);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, -1);
  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, -1);
  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM3, -1);

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEWATCH, 1);

  return 0;
}

int BATTLE_WatchPartyNewEntry(int charaindex, int battleindex, int side) {
  int iRet = 0, i, work;
#if 1

  iRet = BATTLE_WatchNewEntry(charaindex, battleindex, side);
  if (iRet)
    return iRet;

  CAflush(charaindex);

  CDflush(charaindex);

#endif

  for (i = 1; i < CHAR_PARTYMAX; i++) {

    work = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(work) == FALSE)
      continue;

    if (CHAR_getWorkInt(work, CHAR_WORKBATTLEMODE) != 0) {
      continue;
    }

    iRet = BATTLE_WatchNewEntry(work, battleindex, side);

    if (iRet)
      break;

    CAflush(work);
    CDflush(work);
  }
  return iRet;
}

int BATTLE_CreateForWatcher(int charaindex, int topbattleindex);

int BATTLE_WatchEntry(int charaindex, int toindex) {
  int iRet = 0, battleindex, fd;
  char szBuffer[256] = "";

  battleindex = CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX);

  if (BattleArray[battleindex].type == BATTLE_TYPE_WATCH) {
    battleindex = BattleArray[battleindex].topbattleindex;
  }

  if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL ||
      CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ||
      CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX) == -1) {
    iRet = 1;
  }

#ifdef _ALLBLUES_LUA_1_6
  if (BattleWatch(charaindex, toindex) == 1) {
    iRet = 1;
  }
#endif

  if (iRet == 0) {
    iRet = BATTLE_CreateForWatcher(charaindex, battleindex);
  }
  if (iRet == 0) {
    // snprintf( szBuffer, sizeof( szBuffer ), "(%s)开始观战。",
    //	CHAR_getUseName( charaindex ) );

    // BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORYELLOW );
    if ((fd = getfdFromCharaIndex(charaindex)) != -1) {
      lssproto_EN_send(fd, BattleArray[battleindex].type,
                       BattleArray[battleindex].field_no);
    }
    sprintf(szBuffer, "BP|%X|%X|%X", 20, BP_FLG_JOIN,
            CHAR_getInt(charaindex, CHAR_MP));
    BATTLE_CommandSend(charaindex, szBuffer);
    CHAR_setWorkInt(charaindex, CHAR_WATCHBATTLETYPE, 1);
    /*
    if(BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P){
            int j,i,battlecharaindex;
            for(j=0;j<2;j++){
                    for(i=0;i<5;i++){
                            battlecharaindex =
    BattleArray[battleindex].Side[j].Entry[i].charaindex; if(CHAR_CHECKINDEX(
    battlecharaindex)){ if(CHAR_getInt(battlecharaindex,CHAR_RIDENAMEFLG) == 1){
                                            int ridehaveid = CHAR_getInt(
    battlecharaindex, CHAR_RIDEPET ); int ridepetindex = CHAR_getCharPet(
    battlecharaindex, ridehaveid); if(CHAR_CHECKINDEX( ridepetindex)){ char
    ridebufftmp[128]; sprintf(ridebufftmp,"【温馨提示】玩家 < %s >
    使用特殊皮肤改变造型，提示真实骑宠为 < %s
    >",CHAR_getChar(battlecharaindex,CHAR_NAME),CHAR_getChar(ridepetindex,CHAR_NAME));
                                                    CHAR_talkToCli( charaindex,
    -1, ridebufftmp, CHAR_COLORYELLOW);
                                            }
                                    }
                            }
                    }
            }
    }*/
  } else {
    snprintf(szBuffer, sizeof(szBuffer), "无法观战。");
    CHAR_talkToCli(charaindex, -1, szBuffer, CHAR_COLORYELLOW);

    if ((fd = getfdFromCharaIndex(charaindex)) != -1) {
      lssproto_EN_send(fd, FALSE, 0);
    }
  }

  return iRet;
}

BOOL BATTLE_WatchTry(int charaindex) {
  int x, y;
  OBJECT object;
  int pfound = FALSE;
  int result = FALSE;
  int fd;
  int i;
  int cnt;

  fd = getfdFromCharaIndex(charaindex);
  if (CONNECT_checkfd(fd) == FALSE)
    return FALSE;

  for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
    CONNECT_setBattlecharaindex(fd, i, -1);
  }
  CHAR_getCoordinationDir(CHAR_getInt(charaindex, CHAR_DIR),
                          CHAR_getInt(charaindex, CHAR_X),
                          CHAR_getInt(charaindex, CHAR_Y), 1, &x, &y);

  cnt = 0;
  /*愤坌及  及蟆及平乓仿毛潸  允月 */
  for (object = MAP_getTopObj(CHAR_getInt(charaindex, CHAR_FLOOR), x, y);
       object; object = NEXT_OBJECT(object)) {
    int toindex;
    int objindex = GET_OBJINDEX(object);
    /* 平乓仿弁正□元扎卅中 */
    if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
      continue;
    toindex = OBJECT_getIndex(objindex);
    /* 皿伊奶乩□元扎卅中 */
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
        && CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC
#endif
    )
      continue;
    pfound = TRUE;
    /* 爵    匹卅中卅日戚尺 */
    if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
      continue;
    }

    CONNECT_setBattlecharaindex(fd, cnt, toindex);
    cnt++;
    if (cnt == CONNECT_WINDOWBUFSIZE)
      break;
  }
  if (cnt == 0) {
    if (pfound == FALSE) {
      CHAR_talkToCli(charaindex, -1, "那里没有任何人。", CHAR_COLORYELLOW);
    }
    // 分木手中卅仁化棋爵撩
    lssproto_EN_send(fd, FALSE, 0);
  }
  /* ㄠ谛仄井中卅中桦宁反巨件玄伉□今六月 */
  else if (cnt == 1) {
    if (BATTLE_WatchEntry(charaindex, CONNECT_getBattlecharaindex(fd, 0))) {
      result = FALSE;
    } else {
      result = TRUE;
    }
  }
  /*   醒谛中凶日它奴件玉它毛请仄化  中宁歹六月 */
  else if (cnt > 1) {
    int strlength;
    char msgbuf[1024];
    char escapebuf[2048];
    strcpy(msgbuf, "1\n观看谁的战斗呢？\n");
    strlength = strlen(msgbuf);
    /* 它奴件玉它及丢永本□斥综岳［
     * 爵    及平乓仿及域
     */
    for (i = 0;
         CONNECT_getBattlecharaindex(fd, i) != -1 && i < CONNECT_WINDOWBUFSIZE;
         i++) {
      char *a = CHAR_getChar(CONNECT_getBattlecharaindex(fd, i), CHAR_NAME);
      char buf[256];
      snprintf(buf, sizeof(buf), "%s\n", a);
      if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
        print("\nerr:%s:%d 视窗的讯息缓冲不足\n", __FILE__, __LINE__);
        break;
      }
      strcpy(&msgbuf[strlength], buf);
      strlength += strlen(buf);
    }
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                     CHAR_WINDOWTYPE_SELECTBATTLEWATCH, -1,
                     makeEscapeString(msgbuf, escapebuf, sizeof(escapebuf)));
  }

  return result;
}

#endif

#define MAX_DOUJYOUENEMY 64
static int DoujyouEnemyTbl[10]; // ㄠㄟ手中日氏仃升
/***************************************************************
 *   桦乒□玉迕卞衬平乓仿潸
 ***************************************************************/
int *Doujyou_GetEnemy(int meindex, int charaindex) {
  int i;
  char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
  char buf[1024];
  int ret;
  int insert;
  int WorkEnemyTbl[MAX_DOUJYOUENEMY];

  NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr));

  /* 赓渝祭 */
  for (i = 0; i < arraysizeof(WorkEnemyTbl); i++) {
    WorkEnemyTbl[i] = -1;
  }

  if (NPC_Util_GetStrFromStrWithDelim(argstr, "enemyno", buf, sizeof(buf)) ==
      NULL) {
    return NULL;
  }

  insert = 0;
  /* 娄醒卞踏中化丐月衬毛巨件玄伉□今六月 */
  for (i = 0; i < MAX_DOUJYOUENEMY; i++) {
    int curEnemy;
    char data[128];

    ret = getStringFromIndexWithDelim(buf, ",", i + 1, data, sizeof(data));
    if (ret == FALSE) {
      break;
    }

    curEnemy = ENEMY_getEnemyArrayFromId(atoi(data));
    if (!ENEMY_CHECKINDEX(curEnemy))
      continue;
    WorkEnemyTbl[insert] = curEnemy;
    insert++;
  }
  // 分木手中卅井匀凶日巨仿□
  if (insert <= 0)
    return NULL;

  // ㄠ    仿件母丞涩烂
  DoujyouEnemyTbl[0] = WorkEnemyTbl[RAND(0, insert - 1)];

  if (NPC_Util_GetStrFromStrWithDelim(argstr, "enemypetno", buf, sizeof(buf)) ==
      NULL) {
    // 中卅井匀凶日窒手踏井卅中
    DoujyouEnemyTbl[1] = -1;
  } else {
    insert = 0;
    /* 娄醒卞踏中化丐月衬毛巨件玄伉□今六月 */
    for (i = 0; i < MAX_DOUJYOUENEMY; i++) {
      int curEnemy;
      char data[128];

      ret = getStringFromIndexWithDelim(buf, ",", i + 1, data, sizeof(data));
      if (ret == FALSE) {
        break;
      }

      curEnemy = ENEMY_getEnemyArrayFromId(atoi(data));
      if (!ENEMY_CHECKINDEX(curEnemy))
        continue;
      WorkEnemyTbl[insert] = curEnemy;
      insert++;
    }
    // ㄡ    仿件母丞涩烂
    DoujyouEnemyTbl[1] = WorkEnemyTbl[RAND(0, insert - 1)];
  }

  // ㄢ      仄
  DoujyouEnemyTbl[2] = -1;

  return DoujyouEnemyTbl;
}

static int DoujyouWeponTbl[] = {
    -1,       ITEM_FIST,      ITEM_AXE,        ITEM_CLUB,       ITEM_SPEAR,
    ITEM_BOW, ITEM_BOOMERANG, ITEM_BOUNDTHROW, ITEM_BREAKTHROW,
};
int DoujyouRandomWeponSet(int charaindex) {
  int wepon = -1, itemindex;
  int work = RAND(0, arraysizeof(DoujyouWeponTbl) - 1);
  switch (DoujyouWeponTbl[work]) {
  case ITEM_AXE:
    wepon = 0;
    break;
  case ITEM_CLUB:
    wepon = 100;
    break;
  case ITEM_SPEAR:
    wepon = 200;
    break;
  case ITEM_BOW:
    wepon = 2498;
    break;
  case ITEM_BOOMERANG:
    wepon = 500;
    break;
  case ITEM_BOUNDTHROW:
    wepon = 600;
    break;
  case ITEM_BREAKTHROW:
    wepon = 700;
    break;
  }
  itemindex = CHAR_getItemIndex(charaindex, CHAR_ARM);
  if (ITEM_CHECKINDEX(itemindex) == TRUE) {
    ITEM_endExistItemsOne(itemindex);
  }

  if (wepon >= 0) {
    itemindex = ITEM_makeItemAndRegist(wepon);
    CHAR_setItemIndex(charaindex, CHAR_ARM, itemindex);
    ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, charaindex);
    ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
  }
  switch (DoujyouWeponTbl[work]) {
  case -1:
  case ITEM_AXE:
  case ITEM_CLUB:
  case ITEM_SPEAR:
    return 0;
  }
  return 1;
}

static int BattleERSkill1[] = {
    3,   10,  11,  12,  30,  31,  40,  41,  50,  51,  52,  60,  61,  80,
    90,  110, 120, 150, 210, 303, 309, 315, 321, 503, 504, 506, 507, 541,
    542, 543, 544, 545, 546, 547, 575, 579, 580, 606, 613, 615};

static int BattleERSkill2[] = {
    12,  13,  20,  41,  52,  152, 210, 306, 312, 318, 324, 325, 500, 501, 502,
    505, 508, 541, 542, 543, 544, 545, 546, 547, 576, 580, 594, 606, 613, 616};

static int BattleERSkill3[] = {13,  31,  41,  52,  60,  61,  80,  90,  100,
                               110, 120, 152, 210, 325, 500, 501, 504, 503,
                               504, 505, 506, 507, 508, 516, 610};

static int BattleERSkill4[] = {609, 610, 611};

void BATTLE_EnemyRandowSetSkill(int enemyindex, int skillType) {
  int i;
  for (i = 0; i < 7; i++) {
    int skillarray, skillID, Ri;
    switch (skillType) {
    case 1:
      Ri = RAND(0, arraysizeof(BattleERSkill1) - 1);
      skillID = BattleERSkill1[Ri];
      break;
    case 2:
      Ri = RAND(0, arraysizeof(BattleERSkill2) - 1);
      skillID = BattleERSkill2[Ri];
      break;
    case 3:
      Ri = RAND(0, arraysizeof(BattleERSkill3) - 1);
      skillID = BattleERSkill3[Ri];
      break;
    case 4:
      Ri = RAND(0, arraysizeof(BattleERSkill4) - 1);
      skillID = BattleERSkill4[Ri];
      break;
    case 5:
      skillID = RAND(301, 325);
      break;
    default:
      return;
    }
    skillarray = PETSKILL_getPetskillArray(skillID);
    if (PETSKILL_CHECKINDEX(skillarray) == FALSE) {
      CHAR_setPetSkill(enemyindex, i, 1);
      // andy_log
      print("ANDY EnemyRandowSetSkill( ) skillID:%d array:%d err \n", skillID,
            skillarray);
      continue;
    }
    CHAR_setPetSkill(enemyindex, i, skillID);
  }
}

int BATTLE_CreateVsEnemyNew(int charaindex, int npcindex, int *table) {
  int battleindex, iRet = 0, enemyindex, i, pindex;
  int fd, field_no, baselevel = 0;
  int skillType = 0;
  int EnemyList[20];

  int *enemytable = table;

  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

#ifdef _PLAYER_NPC
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
#endif
  {
#ifdef _STREET_VENDOR
    // 摆摊中不可进入战斗
    if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) > -1)
      return BATTLE_ERR_END;
#endif

#ifdef _ANGEL_SUMMON // 装备使者信物不遇敌
    if (CHAR_getWorkInt(charaindex, CHAR_WORKANGELMODE) == TRUE)
      return BATTLE_ERR_END;
#endif
  }
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  battleindex = BATTLE_CreateBattle();
  if (battleindex < 0)
    return BATTLE_ERR_NOTASK;
  if (CHAR_CHECKINDEX(npcindex)) {
    skillType = CHAR_getWorkInt(npcindex, CHAR_NPCWORKINT11);
  }

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(charaindex, CHAR_FLOOR),
                                     CHAR_getInt(charaindex, CHAR_X),
                                     CHAR_getInt(charaindex, CHAR_Y));
  if (field_no > BATTLE_MAP_MAX || field_no < 0) {
    field_no = RAND(0, BATTLE_MAP_MAX);
  }

  BattleArray[battleindex].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battleindex].Side[1].type = BATTLE_S_TYPE_ENEMY;
  BattleArray[battleindex].leaderindex = charaindex;
  BattleArray[battleindex].type = BATTLE_TYPE_P_vs_E;
  BattleArray[battleindex].createindex = npcindex;
  BattleArray[battleindex].field_no = field_no;
  BattleArray[battleindex].BattleFloor = CHAR_getInt(charaindex, CHAR_FLOOR);

  BattleArray[battleindex].Side[0].flg &= ~BSIDE_FLG_HELP_OK;

  if (CHAR_CHECKINDEX(npcindex)) {
    if (CHAR_getInt(npcindex, CHAR_LV) > 1) {
      baselevel = CHAR_getInt(npcindex, CHAR_LV);
    }
    CHAR_setWorkInt(npcindex, CHAR_WORKBATTLEINDEX, battleindex);
  } else if (npcindex == -1) {
    enemytable = ENEMY_getEnemy(charaindex, CHAR_getInt(charaindex, CHAR_X),
                                CHAR_getInt(charaindex, CHAR_Y));
  }

  BattleArray[battleindex].norisk = 0;
  if (enemytable == NULL) {
    iRet = BATTLE_ERR_NOENEMY;
    goto BATTLE_CreateVsEnemy_End;
  }

  for (i = 0; *(enemytable + i) != -1 && i < arraysizeof(EnemyList) - 1; i++) {
    EnemyList[i] = *(enemytable + i);
  }
  EnemyList[i] = -1;

#ifdef _BATTLE_TIMESPEED
  fd = getfdFromCharaIndex(charaindex);
  BattleArray[battleindex].CreateTime = time(NULL);
  BattleArray[battleindex].flgTime = 200; // 1/100 sec
#endif
  for (i = 0; EnemyList[i] != -1; i++) {
    int work;
    enemyindex = ENEMY_createEnemy(EnemyList[i], baselevel);

    if (!CHAR_CHECKINDEX(enemyindex)) {
      iRet = BATTLE_ERR_NOENEMY;
      goto BATTLE_CreateVsEnemy_End;
    }
    CHAR_setInt(enemyindex, CHAR_WHICHTYPE, CHAR_TYPEENEMY);
    if (skillType > 0) {
      // int ENEMY_RandomChange( int enemyindex, int tempno ){
      BATTLE_EnemyRandowSetSkill(enemyindex, skillType);
    }
#ifdef _ALLBLUES_LUA_1_9
    SetBattleEnmeyFunction(npcindex, enemyindex, i);
#endif

    if ((iRet = BATTLE_NewEntry(enemyindex, battleindex, 1))) {
      goto BATTLE_CreateVsEnemy_End;
    }
    if (CHAR_getInt(enemyindex, CHAR_DUELPOINT) > 0) {
      BattleArray[battleindex].dpbattle = 1;
    }
    work = CHAR_getInt(enemyindex, CHAR_BASEBASEIMAGENUMBER);
    if (100466 <= work && work <= 100471) {
      CHAR_setWorkInt(enemyindex, CHAR_WORKBATTLEFLG,
                      CHAR_getWorkInt(enemyindex, CHAR_WORKBATTLEFLG) |
                          CHAR_BATTLEFLG_ABIO);
      // print( "(%s)无生物\n", CHAR_getUseName( enemyindex ) );
    }
  }

  if ((iRet = BATTLE_PartyNewEntry(charaindex, battleindex, 0))) {
    goto BATTLE_CreateVsEnemy_End;
  }
  {
    BATTLE_ENTRY *pEntry, EntryWork;
    pEntry = BattleArray[battleindex].Side[1].Entry;
    for (i = 0; i < 5; i++) {
      EntryWork = pEntry[i];
      pEntry[i] = pEntry[i + 5];
      pEntry[i + 5] = EntryWork;
      pEntry[i].bid = i + SIDE_OFFSET;
      pEntry[i + 5].bid = i + 5 + SIDE_OFFSET;
    }
  }

BATTLE_CreateVsEnemy_End:;
  fd = getfdFromCharaIndex(charaindex);
  if (iRet) {
    BATTLE_ExitAll(battleindex);
    BATTLE_DeleteBattle(battleindex);
    if (fd != -1)
      lssproto_EN_send(fd, FALSE, field_no);
  } else {
    //		if(npcindex > -1){
    if (fd != -1) {
      if (BattleArray[battleindex].dpbattle) {
        lssproto_EN_send(fd, BATTLE_TYPE_DP_BATTLE, field_no);
      } else {
        lssproto_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
      }
    } else {
#ifdef _OFFLINE_SYSTEM
      if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) == 0)
#endif
      {
        BATTLE_ExitAll(battleindex);
        BATTLE_DeleteBattle(battleindex);
        return iRet;
      }
    }
    //		}

    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL)
        continue;
      fd = getfdFromCharaIndex(pindex);

      if (fd != -1) {
        lssproto_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
      }
    }
    if (CHAR_getWorkInt(charaindex, CHAR_WORKACTION) != -1) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
      CHAR_setWorkInt(charaindex, CHAR_WORKACTION, -1);
    }
    CHAR_sendBattleEffect(charaindex, ON);
    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
      }

      CHAR_sendBattleEffect(pindex, ON);
    }
  }

  return iRet;
}

int BATTLE_CreateVsEnemyLvNew(int charaindex, int npcindex, int *table,
                              int *lvtable) {
  int battleindex, iRet = 0, enemyindex, i, pindex;
  int fd, field_no, baselevel = 0;
  int skillType = 0;
  int EnemyList[20];

  int *enemytable = table;

  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

#ifdef _PLAYER_NPC
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
#endif
  {
#ifdef _STREET_VENDOR
    // 摆摊中不可进入战斗
    if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) > -1)
      return BATTLE_ERR_END;
#endif

#ifdef _ANGEL_SUMMON // 装备使者信物不遇敌
    if (CHAR_getWorkInt(charaindex, CHAR_WORKANGELMODE) == TRUE)
      return BATTLE_ERR_END;
#endif
  }
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  battleindex = BATTLE_CreateBattle();
  if (battleindex < 0)
    return BATTLE_ERR_NOTASK;
  if (CHAR_CHECKINDEX(npcindex)) {
    skillType = CHAR_getWorkInt(npcindex, CHAR_NPCWORKINT11);
  }

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(charaindex, CHAR_FLOOR),
                                     CHAR_getInt(charaindex, CHAR_X),
                                     CHAR_getInt(charaindex, CHAR_Y));
  if (field_no > BATTLE_MAP_MAX || field_no < 0) {
    field_no = RAND(0, BATTLE_MAP_MAX);
  }

  BattleArray[battleindex].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battleindex].Side[1].type = BATTLE_S_TYPE_ENEMY;
  BattleArray[battleindex].leaderindex = charaindex;
  BattleArray[battleindex].type = BATTLE_TYPE_P_vs_E;
  BattleArray[battleindex].createindex = npcindex;
  BattleArray[battleindex].field_no = field_no;
  BattleArray[battleindex].BattleFloor = CHAR_getInt(charaindex, CHAR_FLOOR);

  BattleArray[battleindex].Side[0].flg &= ~BSIDE_FLG_HELP_OK;

  if (CHAR_CHECKINDEX(npcindex)) {
    if (CHAR_getInt(npcindex, CHAR_LV) > 1) {
      baselevel = CHAR_getInt(npcindex, CHAR_LV);
    }
    CHAR_setWorkInt(npcindex, CHAR_WORKBATTLEINDEX, battleindex);
  } else if (npcindex == -1) {
    enemytable = ENEMY_getEnemy(charaindex, CHAR_getInt(charaindex, CHAR_X),
                                CHAR_getInt(charaindex, CHAR_Y));
  }

  BattleArray[battleindex].norisk = 0;
  if (enemytable == NULL) {
    iRet = BATTLE_ERR_NOENEMY;
    goto BATTLE_CreateVsEnemy_End;
  }

  for (i = 0; *(enemytable + i) != -1 && i < arraysizeof(EnemyList) - 1; i++) {
    EnemyList[i] = *(enemytable + i);
  }
  EnemyList[i] = -1;

#ifdef _BATTLE_TIMESPEED
  fd = getfdFromCharaIndex(charaindex);
  BattleArray[battleindex].CreateTime = time(NULL);
  BattleArray[battleindex].flgTime = 200; // 1/100 sec
#endif
  for (i = 0; EnemyList[i] != -1; i++) {
    int work;
    enemyindex = ENEMY_createEnemy(EnemyList[i], lvtable[i]);

    if (!CHAR_CHECKINDEX(enemyindex)) {
      iRet = BATTLE_ERR_NOENEMY;
      goto BATTLE_CreateVsEnemy_End;
    }
    CHAR_setInt(enemyindex, CHAR_WHICHTYPE, CHAR_TYPEENEMY);
    if (skillType > 0) {
      // int ENEMY_RandomChange( int enemyindex, int tempno ){
      BATTLE_EnemyRandowSetSkill(enemyindex, skillType);
    }
#ifdef _ALLBLUES_LUA_1_9
    SetBattleEnmeyFunction(npcindex, enemyindex, i);
#endif

    if ((iRet = BATTLE_NewEntry(enemyindex, battleindex, 1))) {
      goto BATTLE_CreateVsEnemy_End;
    }
    if (CHAR_getInt(enemyindex, CHAR_DUELPOINT) > 0) {
      BattleArray[battleindex].dpbattle = 1;
    }
    work = CHAR_getInt(enemyindex, CHAR_BASEBASEIMAGENUMBER);
    if (100466 <= work && work <= 100471) {
      CHAR_setWorkInt(enemyindex, CHAR_WORKBATTLEFLG,
                      CHAR_getWorkInt(enemyindex, CHAR_WORKBATTLEFLG) |
                          CHAR_BATTLEFLG_ABIO);
      // print( "(%s)无生物\n", CHAR_getUseName( enemyindex ) );
    }
  }

  if ((iRet = BATTLE_PartyNewEntry(charaindex, battleindex, 0))) {
    goto BATTLE_CreateVsEnemy_End;
  }
  {
    BATTLE_ENTRY *pEntry, EntryWork;
    pEntry = BattleArray[battleindex].Side[1].Entry;
    for (i = 0; i < 5; i++) {
      EntryWork = pEntry[i];
      pEntry[i] = pEntry[i + 5];
      pEntry[i + 5] = EntryWork;
      pEntry[i].bid = i + SIDE_OFFSET;
      pEntry[i + 5].bid = i + 5 + SIDE_OFFSET;
    }
  }

BATTLE_CreateVsEnemy_End:;
  fd = getfdFromCharaIndex(charaindex);
  if (iRet) {
    BATTLE_ExitAll(battleindex);
    BATTLE_DeleteBattle(battleindex);
    if (fd != -1)
      lssproto_EN_send(fd, FALSE, field_no);
  } else {
    //		if(npcindex > -1){
    if (fd != -1) {
      if (BattleArray[battleindex].dpbattle) {
        lssproto_EN_send(fd, BATTLE_TYPE_DP_BATTLE, field_no);
      } else {
        lssproto_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
      }
    } else {
#ifdef _OFFLINE_SYSTEM
      if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) == 0)
#endif
      {
        BATTLE_ExitAll(battleindex);
        BATTLE_DeleteBattle(battleindex);
        return iRet;
      }
    }
    //		}

    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL)
        continue;
      fd = getfdFromCharaIndex(pindex);

      if (fd != -1) {
        lssproto_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
      }
    }
    if (CHAR_getWorkInt(charaindex, CHAR_WORKACTION) != -1) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
      CHAR_setWorkInt(charaindex, CHAR_WORKACTION, -1);
    }
    CHAR_sendBattleEffect(charaindex, ON);
    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
      }

      CHAR_sendBattleEffect(pindex, ON);
    }
  }

  return iRet;
}

int BATTLE_CreateVsEnemy(int charaindex, int mode, int npcindex) {
  int battleindex, iRet = 0, enemyindex, i, pindex;
  int *enemytable = NULL, fd, field_no, baselevel = 0;
  int skillType = 0;
  int EnemyList[20];

#ifdef _GM_HELPER // 傈捧啊 矫累且 锭 风橇 技泼
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC) {
    CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, 1000);
  }
#endif

  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT)
    return BATTLE_ERR_END;

#ifdef _STREET_VENDOR
  // 摆摊中不可进入战斗
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) > -1)
    return BATTLE_ERR_END;
#endif

#ifdef _ANGEL_SUMMON // 装备使者信物不遇敌
  if (CHAR_getWorkInt(charaindex, CHAR_WORKANGELMODE) == TRUE)
    return BATTLE_ERR_END;
#endif

  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  battleindex = BATTLE_CreateBattle();
  if (battleindex < 0)
    return BATTLE_ERR_NOTASK;

  if (CHAR_CHECKINDEX(npcindex)) {
    skillType = CHAR_getWorkInt(npcindex, CHAR_NPCWORKINT11);
    CHAR_setWorkInt(npcindex, CHAR_WORKBATTLEINDEX, battleindex);
  }

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(charaindex, CHAR_FLOOR),
                                     CHAR_getInt(charaindex, CHAR_X),
                                     CHAR_getInt(charaindex, CHAR_Y));
  if (field_no > BATTLE_MAP_MAX || field_no < 0) {
    field_no = RAND(0, BATTLE_MAP_MAX);
  }

  BattleArray[battleindex].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battleindex].Side[1].type = BATTLE_S_TYPE_ENEMY;
  BattleArray[battleindex].leaderindex = charaindex;
  BattleArray[battleindex].type = BATTLE_TYPE_P_vs_E;
  BattleArray[battleindex].createindex = npcindex;
  BattleArray[battleindex].field_no = field_no;
  BattleArray[battleindex].BattleFloor = CHAR_getInt(charaindex, CHAR_FLOOR);
  // andy_edit 2002/10/23
  /*
  if( (iRet = BATTLE_PartyNewEntry( charaindex, battleindex, 0 ) ) ){
          goto BATTLE_CreateVsEnemy_End;
  }
  */
  BattleArray[battleindex].Side[0].flg &= ~BSIDE_FLG_HELP_OK;
  if (mode == 0) {
    enemytable = ENEMY_getEnemy(charaindex, CHAR_getInt(charaindex, CHAR_X),
                                CHAR_getInt(charaindex, CHAR_Y));
  } else if (mode == 1) {
    enemytable = NPC_Util_getEnemy(npcindex, charaindex);
  } else if (mode == 2) {
    enemytable = Doujyou_GetEnemy(npcindex, charaindex);
    baselevel = CHAR_getInt(npcindex, CHAR_LV);
    BattleArray[battleindex].norisk = 1;
  }
  if (enemytable == NULL) {
    iRet = BATTLE_ERR_NOENEMY;
    goto BATTLE_CreateVsEnemy_End;
  }

  for (i = 0; *(enemytable + i) != -1 && i < arraysizeof(EnemyList) - 1; i++) {
    EnemyList[i] = *(enemytable + i);
  }
  EnemyList[i] = -1;

#ifdef _BATTLE_TIMESPEED
  BattleArray[battleindex].CreateTime = time(NULL);
  BattleArray[battleindex].flgTime = 200; // 1/100 sec
#endif
  for (i = 0; EnemyList[i] != -1; i++) {
    int work;
    enemyindex = ENEMY_createEnemy(EnemyList[i], baselevel);

    if (enemyindex < 0) {
      iRet = BATTLE_ERR_NOENEMY;
      goto BATTLE_CreateVsEnemy_End;
    }

    if (skillType > 0) {
      // int ENEMY_RandomChange( int enemyindex, int tempno ){
      BATTLE_EnemyRandowSetSkill(enemyindex, skillType);
    }

    if (mode == 2) {
      if (i == 0) {
        CHAR_setInt(enemyindex, CHAR_BASEBASEIMAGENUMBER,
                    CHAR_getInt(npcindex, CHAR_BASEBASEIMAGENUMBER));
        CHAR_setInt(enemyindex, CHAR_BASEIMAGENUMBER,
                    CHAR_getInt(npcindex, CHAR_BASEBASEIMAGENUMBER));
        CHAR_setChar(enemyindex, CHAR_NAME, CHAR_getChar(npcindex, CHAR_NAME));
        CHAR_complianceParameter(enemyindex);
      }
    }
    if ((iRet = BATTLE_NewEntry(enemyindex, battleindex, 1))) {
      goto BATTLE_CreateVsEnemy_End;
    }
    if (CHAR_getInt(enemyindex, CHAR_DUELPOINT) > 0) {
      BattleArray[battleindex].dpbattle = 1;
    }
    work = CHAR_getInt(enemyindex, CHAR_BASEBASEIMAGENUMBER);
    if (100466 <= work && work <= 100471) {
      CHAR_setWorkInt(enemyindex, CHAR_WORKBATTLEFLG,
                      CHAR_getWorkInt(enemyindex, CHAR_WORKBATTLEFLG) |
                          CHAR_BATTLEFLG_ABIO);
      // print( "(%s)无生物\n", CHAR_getUseName( enemyindex ) );
    }
  }

#ifdef _ALLBLUES_LUA_1_7
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_NONE) {
      CharVsEnemyFunction(charaindex);
    }
  }
#endif
  // andy_edit 2002/10/23
  if ((iRet = BATTLE_PartyNewEntry(charaindex, battleindex, 0))) {
    goto BATTLE_CreateVsEnemy_End;
  }
  {
    BATTLE_ENTRY *pEntry, EntryWork;
    pEntry = BattleArray[battleindex].Side[1].Entry;
    for (i = 0; i < 5; i++) {
      EntryWork = pEntry[i];
      pEntry[i] = pEntry[i + 5];
      pEntry[i + 5] = EntryWork;
      pEntry[i].bid = i + SIDE_OFFSET;
      pEntry[i + 5].bid = i + 5 + SIDE_OFFSET;
    }
  }

BATTLE_CreateVsEnemy_End:;
  fd = getfdFromCharaIndex(charaindex);

  if (iRet) {
#ifdef _OFFLINE_SYSTEM
    if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) == 0)
#endif
    {
      BATTLE_ExitAll(battleindex);
      BATTLE_DeleteBattle(battleindex);
      if (fd != -1)
        lssproto_EN_send(fd, FALSE, field_no);
    }
  } else {
    if (fd != -1) {
      if (BattleArray[battleindex].dpbattle) {
        lssproto_EN_send(fd, BATTLE_TYPE_DP_BATTLE, field_no);
      } else {
        if (mode == 1) {
          lssproto_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
        } else {
          lssproto_EN_send(fd, BattleArray[battleindex].type, field_no);
        }
      }
    } else {
#ifdef _OFFLINE_SYSTEM
      if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) == 0)
#endif
      {
        BATTLE_ExitAll(battleindex);
        BATTLE_DeleteBattle(battleindex);
        return iRet;
      }
    }

    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL)
        continue;
      fd = getfdFromCharaIndex(pindex);
      if (fd != -1) {
        if (mode == 1) {
          lssproto_EN_send(fd, BATTLE_TYPE_BOSS_BATTLE, field_no);
        } else {
          lssproto_EN_send(fd, BattleArray[battleindex].type, field_no);
        }
      }
    }
    if (CHAR_getWorkInt(charaindex, CHAR_WORKACTION) != -1) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
      CHAR_setWorkInt(charaindex, CHAR_WORKACTION, -1);
    }
    CHAR_sendBattleEffect(charaindex, ON);
    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
      }
      CHAR_sendBattleEffect(pindex, ON);
    }
  }
  return iRet;
}

int BATTLE_CreateVsPlayer(int charaindex0, int charaindex1) {
  int battleindex, pindex, field_no, i, j, charaindex[2], parent[2], fd,
      iRet = 0;

  if (CHAR_CHECKINDEX(charaindex0) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (CHAR_CHECKINDEX(charaindex1) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  if (CHAR_getWorkInt(charaindex0, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex0, -1, "二重遭遇。", CHAR_COLORYELLOW);
    CHAR_talkToCli(charaindex1, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  if (CHAR_getWorkInt(charaindex1, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex0, -1, "二重遭遇。", CHAR_COLORYELLOW);
    CHAR_talkToCli(charaindex1, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(charaindex0, CHAR_FLOOR),
                                     CHAR_getInt(charaindex0, CHAR_X),
                                     CHAR_getInt(charaindex0, CHAR_Y));

  charaindex[0] = charaindex0;
  charaindex[1] = charaindex1;
  for (j = 0; j < 2; j++) {
    if (CHAR_getWorkInt(charaindex[j], CHAR_WORKBATTLEMODE) != 0) {
      return BATTLE_ERR_ALREADYBATTLE;
    }
  }
  for (j = 0; j < 2; j++) {
    if (CHAR_getWorkInt(charaindex[j], CHAR_WORKPARTYMODE) ==
        CHAR_PARTY_LEADER) {
      parent[j] = charaindex[j];
    } else if (CHAR_getWorkInt(charaindex[j], CHAR_WORKPARTYMODE) ==
               CHAR_PARTY_CLIENT) {
      parent[j] = CHAR_getWorkInt(charaindex[j], CHAR_WORKPARTYINDEX1);
    } else {
      parent[j] = -1;
    }
  }
  if (parent[0] != -1 && parent[0] == parent[1]) {
    return BATTLE_ERR_SAMEPARTY;
  }
  battleindex = BATTLE_CreateBattle();
  if (battleindex < 0)
    return BATTLE_ERR_NOTASK;
  BattleArray[battleindex].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battleindex].Side[1].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battleindex].leaderindex = charaindex0;
#ifdef _AUTO_PK
  strcpy(BattleArray[battleindex].leadercdkey,
         CHAR_getChar(charaindex0, CHAR_CDKEY));
  strcpy(BattleArray[battleindex].leadername,
         CHAR_getChar(charaindex0, CHAR_NAME));
  BattleArray[battleindex].rivalindex = charaindex1;
  strcpy(BattleArray[battleindex].rivalcdkey,
         CHAR_getChar(charaindex1, CHAR_CDKEY));
  strcpy(BattleArray[battleindex].rivalname,
         CHAR_getChar(charaindex1, CHAR_NAME));
#endif
  BattleArray[battleindex].type = BATTLE_TYPE_P_vs_P;
  BattleArray[battleindex].dpbattle = 1;
  BattleArray[battleindex].field_no = field_no;
  BattleArray[battleindex].BattleFloor = CHAR_getInt(charaindex0, CHAR_FLOOR);
#ifdef _BATTLE_TIMESPEED
  BattleArray[battleindex].CreateTime = time(NULL);
#endif
  for (j = 0; j < 2; j++) {
    iRet = BATTLE_PartyNewEntry(charaindex[j], battleindex, j);
    if (iRet) {
      goto BATTLE_CreateVsPlayer_End;
    }
    BattleArray[battleindex].Side[j].flg &= ~BSIDE_FLG_HELP_OK;
  }
BATTLE_CreateVsPlayer_End:;
  if (iRet) {
    BATTLE_ExitAll(battleindex);
    BATTLE_DeleteBattle(battleindex);
    fd = getfdFromCharaIndex(charaindex[0]);
    if (fd != -1)
      lssproto_EN_send(fd, FALSE, field_no);
  } else {
    for (j = 0; j < 2; j++) {
      fd = getfdFromCharaIndex(charaindex[j]);
      if (fd != -1) {
        // lssproto_BATTLETIME_send(fd,99);
        lssproto_EN_send(fd, BattleArray[battleindex].type, field_no);
      }
      if (CHAR_getWorkInt(charaindex[j], CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex[j], CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(charaindex[j], CHAR_WORKACTION, -1);
      }
      CHAR_sendBattleEffect(charaindex[j], ON);
      for (i = 1; i < CHAR_PARTYMAX; i++) {
        pindex = CHAR_getWorkInt(charaindex[j], i + CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(pindex) == FALSE)
          continue;
        if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_FINAL)
          continue;
        fd = getfdFromCharaIndex(pindex);
        if (fd != -1) {
          // lssproto_BATTLETIME_send(fd,99);
          lssproto_EN_send(fd, BattleArray[battleindex].type, field_no);
        }
        if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
          CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                              CHAR_ACTSTAND, NULL, 0, FALSE);
          CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
        }
        CHAR_sendBattleEffect(pindex, ON);
      }
    }
  }
  return iRet;
}

#ifdef _TRADE_PK
int BATTLE_CreateVsPlayerForTrade(STradeList TradeList1,
                                  STradeList TradeList2) {
  int battleindex, pindex, field_no, i, j, charaindex[2], parent[2], fd,
      iRet = 0;

  int charaindex0 = TradeList1.charaindex;
  int charaindex1 = TradeList2.charaindex;

  if (CHAR_CHECKINDEX(charaindex0) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (CHAR_CHECKINDEX(charaindex1) == FALSE)
    return BATTLE_ERR_CHARAINDEX;

  if (CHAR_getWorkInt(charaindex0, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex0, -1, "二重遭遇。", CHAR_COLORYELLOW);
    CHAR_talkToCli(charaindex1, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }
  if (CHAR_getWorkInt(charaindex1, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex0, -1, "二重遭遇。", CHAR_COLORYELLOW);
    CHAR_talkToCli(charaindex1, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }

  field_no = BATTLE_getBattleFieldNo(CHAR_getInt(charaindex0, CHAR_FLOOR),
                                     CHAR_getInt(charaindex0, CHAR_X),
                                     CHAR_getInt(charaindex0, CHAR_Y));

  charaindex[0] = charaindex0;
  charaindex[1] = charaindex1;
  for (j = 0; j < 2; j++) {
    if (CHAR_getWorkInt(charaindex[j], CHAR_WORKBATTLEMODE) != 0) {
      return BATTLE_ERR_ALREADYBATTLE;
    }
  }
  for (j = 0; j < 2; j++) {
    if (CHAR_getWorkInt(charaindex[j], CHAR_WORKPARTYMODE) ==
        CHAR_PARTY_LEADER) {
      parent[j] = charaindex[j];
    } else if (CHAR_getWorkInt(charaindex[j], CHAR_WORKPARTYMODE) ==
               CHAR_PARTY_CLIENT) {
      parent[j] = CHAR_getWorkInt(charaindex[j], CHAR_WORKPARTYINDEX1);
    } else {
      parent[j] = -1;
    }
  }
  if (parent[0] != -1 && parent[0] == parent[1]) {
    return BATTLE_ERR_SAMEPARTY;
  }
  battleindex = BATTLE_CreateBattle();
  if (battleindex < 0)
    return BATTLE_ERR_NOTASK;
  BattleArray[battleindex].Side[0].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battleindex].Side[1].type = BATTLE_S_TYPE_PLAYER;
  BattleArray[battleindex].leaderindex = charaindex0;
  BattleArray[battleindex].rivalindex = charaindex1;

#ifdef _TRADE_PK
  memcpy(&BattleArray[battleindex].TradeList[0], &TradeList1,
         sizeof(STradeList));
  memcpy(&BattleArray[battleindex].TradeList[1], &TradeList2,
         sizeof(STradeList));
#endif

  BattleArray[battleindex].type = BATTLE_TYPE_P_vs_P;
  BattleArray[battleindex].dpbattle = 1;
  BattleArray[battleindex].field_no = field_no;
  BattleArray[battleindex].BattleFloor = CHAR_getInt(charaindex0, CHAR_FLOOR);
#ifdef _BATTLE_TIMESPEED
  BattleArray[battleindex].CreateTime = time(NULL);
#endif
  for (j = 0; j < 2; j++) {
    iRet = BATTLE_PartyNewEntry(charaindex[j], battleindex, j);
    if (iRet) {
      goto BATTLE_CreateVsPlayer_End;
    }
    BattleArray[battleindex].Side[j].flg &= ~BSIDE_FLG_HELP_OK;
  }
BATTLE_CreateVsPlayer_End:;
  if (iRet) {
    BATTLE_ExitAll(battleindex);
    BATTLE_DeleteBattle(battleindex);
    fd = getfdFromCharaIndex(charaindex[0]);
    if (fd != -1)
      lssproto_EN_send(fd, FALSE, field_no);
  } else {
    for (j = 0; j < 2; j++) {
      fd = getfdFromCharaIndex(charaindex[j]);
      if (fd != -1)
        lssproto_EN_send(fd, BattleArray[battleindex].type, field_no);
      if (CHAR_getWorkInt(charaindex[j], CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex[j], CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(charaindex[j], CHAR_WORKACTION, -1);
      }
      CHAR_sendBattleEffect(charaindex[j], ON);
      for (i = 1; i < CHAR_PARTYMAX; i++) {
        pindex = CHAR_getWorkInt(charaindex[j], i + CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(pindex) == FALSE)
          continue;
        if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_FINAL)
          continue;

        fd = getfdFromCharaIndex(pindex);
        if (fd != -1)
          lssproto_EN_send(fd, BattleArray[battleindex].type, field_no);
        if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
          CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                              CHAR_ACTSTAND, NULL, 0, FALSE);
          CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
        }
        CHAR_sendBattleEffect(pindex, ON);
      }
    }
  }
  return iRet;
}

#endif

//*********************************************************
//
// 棋爵迕田玄伙正旦弁毛馨笛［玄永皿及戚卞涩烂允月
//
int BATTLE_WatchLink(int topbattleindex, int battleindex)
//
//
//*********************************************************
{
  BATTLE *pWork, *pTop;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    fprint("err:battle index 奇怪(%d)\n", battleindex);
    return FALSE;
  }
  if (BATTLE_CHECKINDEX(topbattleindex) == FALSE) {
    fprint("err:battle index 奇怪(%d)\n", topbattleindex);
    return FALSE;
  }

  pTop = &BattleArray[topbattleindex];

  if (BATTLE_CHECKADDRESS(pTop) == FALSE) {
    fprint("err:battle address 奇怪(%p)\n", pTop);
    return FALSE;
  }

  pWork = pTop->pNext; // 田永弁失永皿

  if (pWork) {
    if (BATTLE_CHECKADDRESS(pWork) == FALSE) {
      fprint("err:battle address 奇怪(%p)\n", pWork);
      return FALSE;
    }
  }

  // 愤坌毛蟆及戚卞允月
  pTop->pNext = &BattleArray[battleindex];
  // 愤坌及蟆田玄伙毛涩烂
  BattleArray[battleindex].pBefore = pTop;
  // 愤坌及戚田玄伙毛涩烂
  BattleArray[battleindex].pNext = pWork;
  // 戚及蟆反愤坌
  if (pWork) {
    pWork->pBefore = &BattleArray[battleindex];
  }
  return TRUE;
}

//*********************************************************
//
// 棋爵迕田玄伙正旦弁毛夫午勾  仁
//
int BATTLE_WatchUnLink(int battleindex)
//
//
//*********************************************************
{
  BATTLE *pTop;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    fprint("err:battle index 奇怪(%d)\n", battleindex);
    return FALSE;
  }

  // 愤坌毛蟆及田玄伙
  pTop = BattleArray[battleindex].pBefore;

  if (pTop) {
    if (BATTLE_CHECKADDRESS(pTop) == FALSE) {
      fprint("err:battle address 奇怪(%p)\n", pTop);
    } else {
      // 勾卅亢卅云仄
      pTop->pNext = BattleArray[battleindex].pNext;
    }
  }
  if (BattleArray[battleindex].pNext) {
    if (BATTLE_CHECKADDRESS(BattleArray[battleindex].pNext) == FALSE) {
      fprint("err:battle address 奇怪(%p)\n", BattleArray[battleindex].pNext);
    } else {
      BattleArray[battleindex].pNext->pBefore = pTop;
    }
  }
  // 愤坌及蟆田玄伙毛涩烂
  BattleArray[battleindex].pBefore = NULL;
  // 愤坌及戚田玄伙反  仄
  BattleArray[battleindex].pNext = NULL;

  return TRUE;
}

//*********************************************************
//
// 棋爵迕卞田玄伙正旦弁毛综岳允月
//
int BATTLE_CreateForWatcher(int charaindex, int topbattleindex)
//
//    曰袄 BATTLE_ERR
//
//*********************************************************
{
  int battleindex, field_no, pindex, i, fd, iRet = 0;

  // 由仿丢□正民尼永弁
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_CHARAINDEX;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    CHAR_talkToCli(charaindex, -1, "二重遭遇。", CHAR_COLORYELLOW);
    return BATTLE_ERR_ALREADYBATTLE;
  }

  // 允匹卞爵    匹卅中井
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != 0) {
    return BATTLE_ERR_ALREADYBATTLE;
  }

  // 爵  正旦弁毛综岳允月
  battleindex = BATTLE_CreateBattle();
  if (battleindex < 0)
    return BATTLE_ERR_NOTASK; // 正旦弁  苇匹五卅井匀凶巨仿□
  // 仇切日及讽采反皿伊奶乩□讽采匹丐月午烂聒允月［
  BattleArray[battleindex].Side[0].type = BATTLE_S_TYPE_PLAYER;
  // 轾仇丹础及讽采手皿伊奶乩□讽采匹丐月午烂聒允月［
  BattleArray[battleindex].Side[1].type = BATTLE_S_TYPE_PLAYER;
  // 伉□母□毛忡绣
  BattleArray[battleindex].leaderindex = charaindex;
  // 爵  及正奶皿
  BattleArray[battleindex].type = BATTLE_TYPE_WATCH;
  BattleArray[battleindex].mode = BATTLE_MODE_WATCHBC;
  // 田玄伙白奴□伙玉  寞
  field_no = BattleArray[battleindex].field_no =
      BattleArray[topbattleindex].field_no;
  BattleArray[battleindex].turn = BattleArray[topbattleindex].turn;

  if (BattleArray[topbattleindex].type != BATTLE_TYPE_WATCH) {
    BattleArray[battleindex].topbattleindex = topbattleindex;
  } else {
    BattleArray[battleindex].topbattleindex =
        BattleArray[topbattleindex].topbattleindex;
  }

  // 伉件弁今六月
  if (BATTLE_WatchLink(topbattleindex, battleindex) == FALSE) {
    fprint("err:battle link error(%d),(%d)\n", topbattleindex, battleindex);
    // 巨仿□质  匹及 GOTO
    goto BATTLE_CreateForWatcher_End;
  }

  // 由□  奴仍午巨件玄伉□
  iRet = BATTLE_WatchPartyNewEntry(charaindex, battleindex, 0);
  if (iRet) {
    // 巨仿□质  匹及 GOTO
    goto BATTLE_CreateForWatcher_End;
  }

// 巨仿□及桦宁反允什卞戊戊卞  月
BATTLE_CreateForWatcher_End:;

  if (iRet) { // 巨仿□互丐匀凶日正旦弁  滋
    // 蝈够  仃月
    BATTLE_ExitAll(battleindex);
    //｛爵  正旦弁绰轮
    BATTLE_DeleteBattle(battleindex);
    fd = getfdFromCharaIndex(charaindex);
    if (fd != -1)
      lssproto_EN_send(fd, FALSE, field_no);
  } else {
    fd = getfdFromCharaIndex(charaindex);
    /* 巨件市它件玄岳  毛项尹月 */
    if (fd != -1)
      lssproto_EN_send(fd, BattleArray[battleindex].type, field_no);
    /*   切禾□术霜月 */
    if (CHAR_getWorkInt(charaindex, CHAR_WORKACTION) != -1) {
      CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX),
                          CHAR_ACTSTAND, NULL, 0, FALSE);
      CHAR_setWorkInt(charaindex, CHAR_WORKACTION, -1);
    }
    /* 爵  失奶戊件  憎CA霜耨 */
    CHAR_sendBattleWatch(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX), ON);

    // 醮棉卞手项尹月
    for (i = 1; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex) == FALSE)
        continue;
      // 切扎氏午巨件市它件玄匹五化卅中卅日霜日卅中
      // FINAL 分匀凶日蟆及爵  苇化中月
      if (CHAR_getWorkInt(pindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_FINAL)
        continue;

      fd = getfdFromCharaIndex(pindex);
      /* 巨件市它件玄岳   */
      if (fd != -1)
        lssproto_EN_send(fd, BattleArray[battleindex].type, field_no);
      /*   切禾□术霜月 */
      if (CHAR_getWorkInt(pindex, CHAR_WORKACTION) != -1) {
        CHAR_sendWatchEvent(CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX),
                            CHAR_ACTSTAND, NULL, 0, FALSE);
        CHAR_setWorkInt(pindex, CHAR_WORKACTION, -1);
      }
      /* 爵  失奶戊件  憎CA霜耨 */
      CHAR_sendBattleWatch(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX), ON);
    }
    /*
                    print( "以(%s)为领队与敌军交手。",
                            CHAR_getChar( charaindex, CHAR_NAME ) );
    */
  }

  return iRet;
}

/*------------------------------------------------------------
 * 爵  毛  蝇允月
 ------------------------------------------------------------*/
void BATTLE_WatchStop(int charaindex) {
  int battleindex;

  battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return;
  // 平乓仿弁正互中凶日  仃今六月
  BATTLE_Exit(charaindex, battleindex);
  // 由□  奴  仃月
  CHAR_DischargePartyNoMsg(charaindex);

  CHAR_talkToCli(charaindex, -1, "战斗中止。", CHAR_COLORYELLOW);
  CHAR_setWorkInt(charaindex, CHAR_WATCHBATTLETYPE, 0);
  lssproto_B_send(getfdFromCharaIndex(charaindex), "BU");
}

//*************************************************************
//
//    正□件备潘热诸匹  蛹仄凶由仿丢□正毛俅孺仄化中仁
//
void BATTLE_TurnParam(int charaindex, int fixkind, int mod, int last)
//
//
//*************************************************************
{
  int modparam, fixparam, lastparam;

  if (fixkind == -1) {
    fixparam = 0; // 葭互  中桦宁反ㄟ
  } else {
    fixparam = CHAR_getWorkInt(charaindex, fixkind);
  }
  if (last == -1) {
    lastparam = 0;
  } else {
    lastparam = CHAR_getWorkInt(charaindex, last);
  }
  modparam = CHAR_getWorkInt(charaindex, mod);
  // 仇仇匹      ≈        午      及    毛苇化｝
  //       及袄毛俅孺仄化中仁

  // 漆反 MODPARAM 毛ㄡㄟ⊙蛹仄化中仁
  modparam *= 0.8;
  CHAR_setWorkInt(charaindex, mod, modparam);

  //         卞笛尹月
  if (last != -1) {
    CHAR_setWorkInt(charaindex, last, lastparam + modparam * 0.01);
  }
}

void BATTLE_AttReverse(int charaindex) {
  int earth, water, fire, wind;
  // ttom start  because the second had this
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLG) &
      CHAR_BATTLEFLG_REVERSE) {
    //   鳖仄化月卅日戚尺
  } else {
    //   鳖仄化卅中及匹伉正□件
    return;
  }
  // ttom end
  earth = CHAR_getWorkInt(charaindex, CHAR_WORKFIXEARTHAT);
  water = CHAR_getWorkInt(charaindex, CHAR_WORKFIXWATERAT);
  fire = CHAR_getWorkInt(charaindex, CHAR_WORKFIXFIREAT);
  wind = CHAR_getWorkInt(charaindex, CHAR_WORKFIXWINDAT);

  CHAR_setWorkInt(charaindex, CHAR_WORKFIXEARTHAT, fire);
  CHAR_setWorkInt(charaindex, CHAR_WORKFIXWATERAT, wind);
  CHAR_setWorkInt(charaindex, CHAR_WORKFIXFIREAT, earth);
  CHAR_setWorkInt(charaindex, CHAR_WORKFIXWINDAT, water);
}

void BATTLE_PreCommandSeq(int battleindex) {
  BATTLE_ENTRY *pEntry;
  BATTLE *pBattle;
  int i, j, charaindex;
  BATTLE_CharSendAll(battleindex);
  BATTLE_CharaBackUp(battleindex);
  BattleArray[battleindex].timer = NowTime.tv_sec;
  BATTLE_AllCharaCWaitSet(battleindex);
  BATTLE_ActSettingSend(battleindex);
  BattleArray[battleindex].flg |= BATTLE_FLG_FREEDP;
  pBattle = &BattleArray[battleindex];
  for (j = 0; j < 2; j++) {
    int flg;
    pEntry = pBattle->Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      pEntry[i].guardian = -1;
      charaindex = pEntry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;
      flg = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLG);
      // ttom the second only one //flg &= ~CHAR_BATTLEFLG_GUARDIAN;
      flg &= ~CHAR_BATTLEFLG_GUARDIAN;
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEFLG, flg);
      if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_EARTHROUND0)
        continue;

      CHAR_complianceParameter(charaindex);
      BATTLE_TurnParam(charaindex,
                       CHAR_WORKFIXSTR, //   猾
                       CHAR_WORKMODATTACK, CHAR_WORKATTACKPOWER);
      BATTLE_TurnParam(charaindex,
                       CHAR_WORKFIXTOUGH, // 潮
                       CHAR_WORKMODDEFENCE, CHAR_WORKDEFENCEPOWER);
      BATTLE_TurnParam(charaindex,
                       CHAR_WORKFIXDEX, // 豳镀今
                       CHAR_WORKMODQUICK, CHAR_WORKQUICK);
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        BATTLE_TurnParam(charaindex, -1,
                         CHAR_WORKMODCHARM, //
                         CHAR_WORKFIXCHARM);
      }
      BATTLE_TurnParam(charaindex,
                       -1, //   凯
                       CHAR_WORKMODCHARM, -1);
      if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLG) &
          CHAR_BATTLEFLG_REVERSE) {
        BATTLE_AttReverse(charaindex);
      }
    }
  }
}

//**************************************************
//
// 棋爵及桦宁及戊穴件玉谨切尺  月凛及域  及  木
//
void BATTLE_PreWatchWaitSeq(int battleindex)
//
//
//**************************************************
{
  // 仇及凛鳔匹凛对忡绣
  BattleArray[battleindex].timer = NowTime.tv_sec;
  // 蝈够及乒□玉毛戊穴件玉    蟆卞允月
  BATTLE_AllCharaWatchWaitSet(battleindex);
}
static int BATTLE_Init(int battleindex) {
  BATTLE *pBattle;
  int iRet = 0;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  pBattle = &BattleArray[battleindex];
  pBattle->mode = BATTLE_MODE_BATTLE;
  iRet = BATTLE_SurpriseCheck(battleindex);
  if (iRet == 0) {
  } else if (iRet == 1) {
    BattleArray[battleindex].Side[1].flg |= BSIDE_FLG_SURPRISE;
  } else if (iRet == 2) {
    BattleArray[battleindex].Side[0].flg |= BSIDE_FLG_SURPRISE;
  }
  BATTLE_PreCommandSeq(battleindex);
  return 0;
}
int BATTLE_CountEntry(int battleindex, int side) {
  int i;
  BATTLE_ENTRY *pEntry;
  int cnt = 0;

  // 由仿丢□正民尼永弁
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return -BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -BATTLE_ERR_BATTLEINDEX;

  // 巨件玄伉□
  pEntry = BattleArray[battleindex].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    if (pEntry[i].charaindex != -1) { // 平乓仿互中凶日醒尹月
      cnt++;
    }
  }
  return cnt;
}

static BOOL BATTLE_CommandWait(int battleindex, int side) {
  int i, charaindex, BeOk = 0;
  BATTLE_ENTRY *pEntry;
  BOOL iRet = TRUE;
  BOOL TimeOut = FALSE;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return TRUE; //检查值是否在合法  围
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return TRUE;
  if (BattleArray[battleindex].Side[side].type == BATTLE_S_TYPE_ENEMY)
    return TRUE;
  pEntry = BattleArray[battleindex].Side[side].Entry;

#ifdef _BATTLECOMMAND_TIME
  if (BattleArray[battleindex].PartTime > 1 &&
      BattleArray[battleindex].PartTime < time(NULL)) {
    TimeOut = TRUE;
  }
#endif

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = pEntry[i].charaindex;
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    if (CHAR_getFlg(charaindex, CHAR_ISDIE) == TRUE) {
      continue;
    }
#ifdef _GM_HELPER
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
        CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERPETNPC) {
      BeOk++;
      continue;
    }
#endif
    switch (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE)) {
    case BATTLE_CHARMODE_C_OK:
      BeOk++;
    case BATTLE_CHARMODE_INIT:
    case BATTLE_CHARMODE_RESCUE:
    case BATTLE_CHARMODE_WATCHINIT:
      break;
    case BATTLE_CHARMODE_C_WAIT: {
#ifdef _BATTLECOMMAND_TIME
      if (TimeOut == TRUE) {
        CHAR_talkToCli(charaindex, -1,
                       "硅撇 咀记阑 急琶窍绰 矫埃捞 檬苞沁嚼聪促.",
                       CHAR_COLORYELLOW);
        CHAR_DischargeParty(charaindex, 0);
        CHAR_setWorkInt(charaindex, CHAR_WORKFMPKFLAG, -1);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1,
                        BATTLE_COM_COMPELESCAPE);
        BATTLE_Exit(charaindex, battleindex);
        lssproto_B_send(getfdFromCharaIndex(charaindex), "BU");
        break;
      }
#endif
      iRet = FALSE;
    } break;
    default:
      BATTLE_Exit(charaindex, battleindex);
      break;
    }
  }

  if (BeOk > 0) {
#ifdef _BATTLECOMMAND_TIME
    if (BattleArray[battleindex].PartTime <= 0) {
      //如果战役中有任一人执行指令, 则延迟时间设为120秒
      BattleArray[battleindex].PartTime = (int)time(NULL) + 120;
    }
#endif
  }
  return iRet;
}

int BATTLE_GetExp(int charaindex, int midx) {
  int addexp, i;
  int modexp = 0, getexp = 0, modexp2 = 0, getexp2 = 0;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return 0;

#ifdef _BATTLE_GOLD //战后获得石币
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    int gold = CHAR_getInt(charaindex, CHAR_GOLD);
    if ((gold + getBattleGold()) > CHAR_getMaxHaveGold(charaindex)) {
      gold = CHAR_getMaxHaveGold(charaindex);
    } else {
      gold += getBattleGold();
    }
    CHAR_setInt(charaindex, CHAR_GOLD, gold);
    CHAR_complianceParameter(charaindex);
    CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_GOLD);
  }
#endif
  getexp = CHAR_getWorkInt(charaindex, CHAR_WORKGETEXP);
#ifdef _NEWOPEN_MAXEXP
  if (getexp > 1000000000)
    getexp = 1000000000;
  if (getexp < 0)
    getexp = 0;
#endif
  getexp2 = getexp;
  //智慧果加成
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    int ownerindex =
        CHAR_getWorkInt(charaindex, CHAR_WORKPLAYERINDEX); //取得主人索引
    if (CHAR_CHECKINDEX(ownerindex)) {
      if (CHAR_getWorkInt(ownerindex, CHAR_WORKITEM_ADDEXP) > 0
#ifdef _UP_ON_OFF
          && (CHAR_getInt(ownerindex, CHAR_ONOFF01) == 1)
#endif
      ) { //主人的智慧果对宠物产生作用
        modexp = CHAR_getWorkInt(ownerindex, CHAR_WORKITEM_ADDEXP);
      }
    }
  }
#ifdef _UP_ON_OFF
  else if (CHAR_getInt(charaindex, CHAR_ONOFF01) == 1) {
#endif
    if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP) > 0) {
      modexp = CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP);
    }
  }
  addexp = getexp + ((getexp * modexp) / 100);
  //智慧果结束
#ifdef _ITEM_ADDEQUIPEXP //经验装备
  if (CHAR_CHECKINDEX(midx)) {
    for (i = 0; i < CHAR_EQUIPPLACENUM; i++) {
      int id = CHAR_getItemIndex(midx, i); //道具id
      if (ITEM_CHECKINDEX(id)) {
        char *arg, *P;
        arg = ITEM_getChar(id, ITEM_ARGUMENT); //道具参数
        if ((P = strstr(arg, "EXPUP"))) {
          if ((P = strstr(arg, "人"))) { //只对人有效
            if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
              addexp += getexp * atoi(P += 2) * 0.01;
            }
          } else if ((P = strstr(arg, "宠"))) { //只对宠有效
            if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
                (CHAR_getInt(midx, CHAR_RIDEPET) != charaindex)) {
              addexp += getexp * atoi(P += 2) * 0.01;
            }
          } else if ((P = strstr(arg, "骑"))) { //只对骑宠有效
            if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
                (CHAR_getInt(midx, CHAR_RIDEPET) == charaindex)) {
              addexp += getexp * atoi(P += 2) * 0.01;
            }
          } else {
            P = strstr(arg, "EXPUP");
            addexp += getexp * atoi(P += 5) * 0.01;
          }
        }
      }
    }
  }
#endif
  //线路倍数
  addexp *= getBattleexp();
  //线路倍数结束

  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    if (CHAR_getInt(charaindex, CHAR_PETID) ==
        1163) { //宠物经验道具不对1163编号的宠物生效
      addexp = 0;
    }
  }
  float partyi = 1, party_expnum, toindex;
  char *partyi_name;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    partyi = 1;
  } else {
    for (i = 1; i <= CHAR_PARTYMAX; i++) {
      toindex = CHAR_getPartyIndex(charaindex, i);
      if (CHAR_CHECKINDEX(toindex)) {
        partyi_name = CHAR_getChar(toindex, CHAR_NAME);
        if (!strstr(partyi_name, "颇萍版氰摹")) {
          partyi = partyi + 1;
        }
      }
    }
  }

  if (partyi == 2) {
    party_expnum = 0.05;
  } else if (partyi == 3) {
    party_expnum = 0.08;
  } else if (partyi == 4) {
    party_expnum = 0.12;
  } else if (partyi == 5) {
    party_expnum = 0.2;
  } else {
    party_expnum = 0;
  }

  if (party_expnum != 0) {
    addexp = addexp + (addexp * party_expnum);
  }
  if (getexp < 0 ||
      CHAR_GetLevelExp(charaindex, CHAR_getInt(charaindex, CHAR_LV) + 1) ==
          -1) {
#ifdef _ALLBLUES_LUA_1_9
    FreeCharExpSave(charaindex, getexp,
                    addexp); //龙写的经验丹LUA，暂时不用，注释掉
#endif
    addexp = 0;
  }
  addexp = (addexp < 0) ? 0 : addexp;
  if (CHAR_getInt(charaindex, CHAR_LV) >= CHAR_MAXUPLEVEL) {
    addexp = 0;
  }

#ifdef _ALLBLUES_LUA_1_9
  addexp = FreeModeExp(charaindex, getexp, addexp);
#endif

  CHAR_setWorkInt(charaindex, CHAR_WORKGETEXP, addexp); //回存CHAR_WORKGETEXP
  CHAR_AddMaxExp(charaindex, addexp);

  return addexp;
}

int BATTLE_DpCalc(int battleindex) {
  BATTLE_ENTRY *pLooseEntry, *pWinEntry;
  int winside, looseside, i, charaindex, dpadd, dpall, num = 0;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  winside = BattleArray[battleindex].winside;
  looseside = 1 - winside;
  if (winside != -1 && winside != 1)
    return BATTLE_ERR_PARAM;

  // 宁煌袄赓渝祭
  dpall = 0;

  //  仃凶  井日    毛畴丹
  pLooseEntry = BattleArray[battleindex].Side[looseside].Entry;
  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = pLooseEntry[i].charaindex;
    // 皿伊奶乩□动陆反饬    仄
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    // ㄠㄟ坌及ㄠ手日尹月
    dpadd = CHAR_getInt(charaindex, CHAR_DUELPOINT) * DUELPOINT_RATE;
    //   仃凶  卞反    毛穴奶瓜旦涩烂
    CHAR_setWorkInt(charaindex, CHAR_WORKGETEXP,
                    CHAR_getWorkInt(charaindex, CHAR_WORKGETEXP) - dpadd);
    // 宁煌袄卞笛遥
    dpall += dpadd;
  }
  // 今日卞  仆凶曰仄凶谛迕及    毛笛遥
  dpall += BattleArray[battleindex].Side[winside].common_dp;

  pWinEntry = BattleArray[battleindex].Side[winside].Entry;
  for (num = 0, i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = pWinEntry[i].charaindex;
    // 皿伊奶乩□动陆反饬    仄
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    num++;
  }
  // 卅兮井  匀凶幻丹卞簿手中卅中
  if (num <= 0)
    return BATTLE_ERR_BATTLEINDEX;
  dpadd = dpall / num;
  if (dpadd <= 0)
    dpadd = 1; //   斓匹手ㄠ反芨尹月

  for (num = 0, i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = pWinEntry[i].charaindex;
    // 皿伊奶乩□动陆反饬    仄
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;

    //   匀凶  卞反    毛涩烂
    CHAR_setWorkInt(charaindex, CHAR_WORKGETEXP,
                    CHAR_getWorkInt(charaindex, CHAR_WORKGETEXP) + dpadd);
  }
  return 0;
}

//#define RS_LIST_MAX	4	//   凛卞窒谛爵  瑛绊霜月井
#define RS_LIST_MAX 5
typedef struct {
  int num;
  int exp;
  int levelup;
} RS_LIST;

int BATTLE_GetDuelPoint(int battleindex, int side, int num) {
  char szBuffer[1024] = "";
  int charaindex;
  int dpnow, dpadd;
  int fd;
  int floor;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  charaindex = BattleArray[battleindex].Side[side].Entry[num].charaindex;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_PARAM;
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    return 0;
  }
  if (BattleArray[battleindex].Side[side].type != BATTLE_S_TYPE_PLAYER) {
    return 0;
  }
  dpadd = CHAR_getWorkInt(charaindex, CHAR_WORKGETEXP);

#ifdef _VIP_POINT_PK
  floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  if (floor == 11111 || floor == 22222 || floor == 33333 || floor == 44444) {
    int point = 0;
    int oldpoint = CHAR_getInt(charaindex, CHAR_AMPOINT);
    if (floor == 44444) {
      point = getVipPointPK(3);
    } else if (floor == 33333) {
      point = getVipPointPK(2);
    } else if (floor == 22222) {
      point = getVipPointPK(1);
    } else if (floor == 11111) {
      point = getVipPointPK(0);
    }

    if (dpadd < 0) {
      CHAR_setInt(charaindex, CHAR_AMPOINT, oldpoint - point);
#ifdef _AMPOINT_LOG
      LogAmPoint(CHAR_getChar(charaindex, CHAR_NAME),
                 CHAR_getChar(charaindex, CHAR_CDKEY), -point,
                 CHAR_getInt(charaindex, CHAR_AMPOINT), "(乱舞格斗)",
                 CHAR_getInt(charaindex, CHAR_FLOOR),
                 CHAR_getInt(charaindex, CHAR_X),
                 CHAR_getInt(charaindex, CHAR_Y));
#endif
      sprintf(szBuffer, "由于你战败失去积分点:%d\n", point);
      CHAR_talkToCli(charaindex, -1, szBuffer, CHAR_COLORCYAN);

      if (CHAR_getInt(charaindex, CHAR_AMPOINT) < point) {
        sprintf(szBuffer, "你的积分点不足%d，自动脱离团队！", point);
        CHAR_talkToCli(charaindex, -1, szBuffer, CHAR_COLORYELLOW);
        CHAR_DischargePartyNoMsg(charaindex);
      }

      int i;
      int num = 0;
      BATTLE_ENTRY *pWinEntry = BattleArray[battleindex].Side[1 - side].Entry;
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        int charaindex = pWinEntry[i].charaindex;

        if (CHAR_CHECKINDEX(charaindex) == FALSE)
          continue;
        if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
          continue;
        num++;
      }

      int addpoint = point / num;
      addpoint -= addpoint * getVipPointPKCost();
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        int charaindex = pWinEntry[i].charaindex;

        if (CHAR_CHECKINDEX(charaindex) == FALSE)
          continue;
        if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
          continue;

        int oldpoint = CHAR_getInt(charaindex, CHAR_AMPOINT);
        CHAR_setInt(charaindex, CHAR_AMPOINT, oldpoint + addpoint);
#ifdef _AMPOINT_LOG
        LogAmPoint(CHAR_getChar(charaindex, CHAR_NAME),
                   CHAR_getChar(charaindex, CHAR_CDKEY), addpoint,
                   CHAR_getInt(charaindex, CHAR_AMPOINT), "(乱舞格斗)",
                   CHAR_getInt(charaindex, CHAR_FLOOR),
                   CHAR_getInt(charaindex, CHAR_X),
                   CHAR_getInt(charaindex, CHAR_Y));
#endif
        sprintf(szBuffer, "恭喜你赢得积分点:%d\n", addpoint);
        CHAR_talkToCli(charaindex, -1, szBuffer, CHAR_COLORCYAN);
      }
    }
  } else
#endif
  {
    if (dpadd == 0) {
      dpadd = 0;
    } else if (dpadd < 0) {
      dpadd = min(-1, dpadd);
    } else {
      dpadd = max(1, dpadd);
    }
    dpnow = CHAR_getInt(charaindex, CHAR_DUELPOINT) + dpadd;
    dpnow = max(dpnow, 0);

    CHAR_setInt(charaindex, CHAR_DUELPOINT, min(dpnow, CHAR_MAXDUELPOINT));

    szBuffer[0] = 0;
    {
      char szAdd[64], szNow[64];
      cnv10to62(dpadd, szAdd, sizeof(szAdd));
      cnv10to62(dpnow, szNow, sizeof(szNow));
      snprintf(szBuffer, sizeof(szBuffer), "%s|%s|", szAdd, szNow);
    }

    fd = getfdFromCharaIndex(charaindex);
    lssproto_RD_send(fd, szBuffer);

    CHAR_send_DpDBUpdate(charaindex);
#ifndef _NET_REDUCESEND
    CHAR_send_DpDBUpdate_AddressBook(charaindex, TRUE);
#endif
  }
  return 0;
}

int BATTLE_GetExpGold(int battleindex, // 爵  奶件犯永弁旦
                      int side,        // 扔奶玉  ㄟ  ㄠ
                      int num          // 愤坌反    及窒    及平乓仿井
) {
  BATTLE_ENTRY *pEntryChara;
  char szBuffer[1024] = "", szItemString[512], szEscItemString[256];
  int charaindex, UpLevel, petindex, i, j, itemindex = -1;
  int rsCnt = 0;
  RS_LIST aRsList[RS_LIST_MAX];
  int itemgroup[CHAR_getMaxInventory(charaindex) - CHAR_STARTITEMARRAY];
  int itemnum = 0;
  memset(aRsList, 0, sizeof(aRsList));
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  charaindex = BattleArray[battleindex].Side[side].Entry[num].charaindex;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return BATTLE_ERR_PARAM;
  if (CHAR_getFlg(charaindex, CHAR_ISDIE) == TRUE) {
    return 0;
  }
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
#ifdef _PLAYER_NPC
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
        CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERPETNPC) {
      if (CHAR_getFlg(charaindex, CHAR_ISDIE) == FALSE) {
#ifdef _ITEM_ADDEQUIPEXP
        BATTLE_GetExp(charaindex, charaindex);
#else
        BATTLE_GetExp(charaindex);
#endif
        int UpLevel = CHAR_LevelUpCheck(charaindex, -1);
        int i;
        for (i = 0; i < UpLevel; i++) {
          CHAR_PetLevelUp(charaindex);
          CHAR_PetAddVariableAi(charaindex, AI_FIX_PETLEVELUP);
        }
        for (i = 0; i < CHAR_MAXPETHAVE; i++) {
          petindex = CHAR_getCharPet(charaindex, i);
          if (CHAR_CHECKINDEX(petindex) == FALSE)
            continue;
          if (CHAR_getFlg(petindex, CHAR_ISDIE) == TRUE)
            continue;
          if (CHAR_getWorkInt(petindex, CHAR_WORKGETEXP) <= 0) {
            CHAR_complianceParameter(petindex);
            sprintf(szBuffer, "K%d", i);
            CHAR_sendStatusString(charaindex, szBuffer);
            continue;
          }
#ifdef _ITEM_ADDEQUIPEXP
          BATTLE_GetExp(petindex, charaindex);
#else
          BATTLE_GetExp(petindex);
#endif
          UpLevel = CHAR_LevelUpCheck(petindex, charaindex);
          if (UpLevel > 0) {
            for (j = 0; j < UpLevel; j++) {
              CHAR_PetLevelUp(petindex);
              CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
            }
            aRsList[rsCnt].levelup = 1;
          } else {
            aRsList[rsCnt].levelup = 0;
          }
          aRsList[rsCnt].num = i;
          aRsList[rsCnt].exp = CHAR_getWorkInt(petindex, CHAR_WORKGETEXP);
          rsCnt++;
          CHAR_complianceParameter(petindex);
          if (UpLevel > 0) {
            sprintf(szBuffer, "K%d", i);
            CHAR_sendStatusString(charaindex, szBuffer);
          }
        }
      }
    }
#endif
    return 0;
  }
  pEntryChara = &BattleArray[battleindex].Side[side].Entry[num];
  if (CHAR_getFlg(charaindex, CHAR_ISDIE) == FALSE) {
#ifdef _ITEM_ADDEQUIPEXP
    BATTLE_GetExp(charaindex, charaindex);
#else
    BATTLE_GetExp(charaindex);
#endif
    if (getBattleDebugMsg() != 0) {
      snprintf(szBuffer, sizeof(szBuffer), "(%s) 得到 EXP %d",
               CHAR_getUseName(charaindex),
               CHAR_getWorkInt(charaindex, CHAR_WORKGETEXP));
      BATTLE_talkToCli(charaindex, szBuffer, CHAR_COLORYELLOW);
    }
  }
  UpLevel = CHAR_LevelUpCheck(charaindex, -1);
  if (UpLevel > 0) {
    aRsList[rsCnt].levelup = 1;
#ifdef _CHAR_LEVELUPMSG
    char msg[128];
    snprintf(msg, sizeof(msg), "某腐磐狼 饭骇捞 [%d](栏)肺 棵耳嚼聪促.",
             CHAR_getInt(charaindex, CHAR_LV));
    CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
    CHAR_setInt(charaindex, CHAR_HP,
                CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP));
#endif
#ifdef _CHAR_LEVELACHIEVE
    if (CHAR_getInt(charaindex, CHAR_LV) == 140) {
      char token[256];
      int point = CHAR_getInt(charaindex, CHAR_AHIEVEPOINT);
      CHAR_talkToCli(
          charaindex, -1,
          "某腐磐 饭骇 [140]阑 崔己窍咯 券旷 器牢飘 1Ｐ甫 瘤鞭钦聪促.",
          CHAR_COLORYELLOW);
      CHAR_setInt(charaindex, CHAR_AHIEVEPOINT, point + 1);
    }
#endif
  } else {
    aRsList[rsCnt].levelup = 0;
  }
  aRsList[rsCnt].num = -2;
  aRsList[rsCnt].exp = CHAR_getWorkInt(charaindex, CHAR_WORKGETEXP);
  rsCnt++;
  if (UpLevel > 0) {
    int work;

    if (getBattleDebugMsg() != 0) {
      snprintf(szBuffer, sizeof(szBuffer), "(%s) 升级至 %d",
               CHAR_getUseName(charaindex), CHAR_getInt(charaindex, CHAR_LV));
      BATTLE_talkToCli(charaindex, szBuffer, CHAR_COLORYELLOW);
    }
#ifdef _SKILLUPPOINT_CF
    CHAR_setInt(charaindex, CHAR_SKILLUPPOINT,
                CHAR_getInt(charaindex, CHAR_SKILLUPPOINT) +
                    UpLevel * getSkup());
#else
    CHAR_setInt(charaindex, CHAR_SKILLUPPOINT,
                CHAR_getInt(charaindex, CHAR_SKILLUPPOINT) + UpLevel * 3);
#endif
    work = CHAR_getInt(charaindex, CHAR_CHARM);
    work += CH_FIX_PLAYERLEVELUP;
    CHAR_setInt(charaindex, CHAR_CHARM, min(100, work));
    CHAR_complianceParameter(charaindex);
    CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_LV |
                                             CHAR_P_STRING_NEXTEXP |
                                             CHAR_P_STRING_DUELPOINT);
    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
    CHAR_PartyUpdate(charaindex, CHAR_N_STRING_LV);
#ifndef _NET_REDUCESEND
    CHAR_send_DpDBUpdate_AddressBook(charaindex, TRUE);
#endif
  }
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    petindex = CHAR_getCharPet(charaindex, i);
    if (CHAR_CHECKINDEX(petindex) == FALSE)
      continue;
    if (CHAR_getFlg(petindex, CHAR_ISDIE) == TRUE)
      continue;
    if (CHAR_getWorkInt(petindex, CHAR_WORKGETEXP) <= 0) {
      CHAR_complianceParameter(petindex);
      sprintf(szBuffer, "K%d", i);
      CHAR_sendStatusString(charaindex, szBuffer);

      continue;
    }
#ifdef _ITEM_ADDEQUIPEXP
    BATTLE_GetExp(petindex, charaindex);
#else
    BATTLE_GetExp(petindex);
#endif
    if (getBattleDebugMsg() != 0) {
      snprintf(szBuffer, sizeof(szBuffer), "(%s) 得到 EXP %d",
               CHAR_getUseName(petindex),
               CHAR_getWorkInt(petindex, CHAR_WORKGETEXP));
      BATTLE_talkToCli(charaindex, szBuffer, CHAR_COLORYELLOW);
    }
    UpLevel = CHAR_LevelUpCheck(petindex, charaindex);
    if (UpLevel > 0) {
      if (getBattleDebugMsg() != 0) {
        snprintf(szBuffer, sizeof(szBuffer), "(%s) 升级至 %d",
                 CHAR_getUseName(petindex), CHAR_getInt(petindex, CHAR_LV));
        BATTLE_talkToCli(charaindex, szBuffer, CHAR_COLORYELLOW);
      }
      for (j = 0; j < UpLevel; j++) {
        CHAR_PetLevelUp(petindex);
        CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
      }
      aRsList[rsCnt].levelup = 1;
#ifdef _PET_LEVELUPMSG
      char msg[128], msg2[128];
      snprintf(msg, sizeof(msg), "[%s]狼 饭骇捞 [%d](栏)肺 棵耳嚼聪促.",
               CHAR_getUseName(petindex), CHAR_getInt(petindex, CHAR_LV));
      snprintf(msg2, sizeof(msg2),
               "捞傈 : 郴备仿: %d, 傍拜仿: %d, 规绢仿: %d, 鉴惯仿: %d",
               CHAR_getWorkInt(petindex, CHAR_WORKMAXHP),
               CHAR_getWorkInt(petindex, CHAR_WORKATTACKPOWER),
               CHAR_getWorkInt(petindex, CHAR_WORKDEFENCEPOWER),
               CHAR_getWorkInt(petindex, CHAR_WORKQUICK));
      CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORGREEN);
      CHAR_talkToCli(charaindex, -1, msg2, CHAR_COLORGREEN);
#endif
    } else {
      aRsList[rsCnt].levelup = 0;
    }
    aRsList[rsCnt].num = i;
    aRsList[rsCnt].exp = CHAR_getWorkInt(petindex, CHAR_WORKGETEXP);
    rsCnt++;
    CHAR_complianceParameter(petindex);
    if (UpLevel > 0) {
      SQL_UpdatePetRank(petindex);
#ifdef _PET_LEVELUPMSG
      char msg3[128];
      snprintf(msg3, sizeof(msg3),
               "泅犁 : 郴备仿: %d, 傍拜仿: %d, 规绢仿: %d, 鉴惯仿: %d",
               CHAR_getWorkInt(petindex, CHAR_WORKMAXHP),
               CHAR_getWorkInt(petindex, CHAR_WORKATTACKPOWER),
               CHAR_getWorkInt(petindex, CHAR_WORKDEFENCEPOWER),
               CHAR_getWorkInt(petindex, CHAR_WORKQUICK));
      CHAR_talkToCli(charaindex, -1, msg3, CHAR_COLORGREEN);
      CHAR_setInt(petindex, CHAR_HP, CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));
#endif
      sprintf(szBuffer, "K%d", i);
      CHAR_sendStatusString(charaindex, szBuffer);
    }
  }
  szItemString[0] = 0;
  if (CHAR_getFlg(charaindex, CHAR_ISDIE) == FALSE) {
    for (i = 0; i < GETITEM_MAX; i++) {
      int ItemGet = FALSE;
      szEscItemString[0] = 0;
      itemindex = pEntryChara->getitem[i];

      if (itemindex < 0) {
        itemindex = -1;
      } else if (CHAR_findEmptyItemBox(charaindex) >= 0) {
        int ret = CHAR_addItemSpecificItemIndex(charaindex, itemindex);
        if (0 <= ret && ret < CHAR_EQUIPPLACENUM) {
          print("itemget err: 在这里不能取得item(%s)(%d)(%s)\n",
                CHAR_getUseName(charaindex), ret,
                ITEM_getAppropriateName(itemindex));
        }
        if (!CHAR_CHECKITEMINDEX(charaindex, ret)) {
          ItemGet = FALSE;
        } else {
          ItemGet = TRUE;
          itemgroup[itemnum] = ret;
          itemnum++;
        }
      }
      if (ItemGet == TRUE) {
        if (getBattleDebugMsg() != 0) {
          snprintf(szBuffer, sizeof(szBuffer), "%s(阑)甫 颊俊 持菌促.",
                   ITEM_getAppropriateName(itemindex));
          BATTLE_talkToCli(charaindex, szBuffer, CHAR_COLORYELLOW);
        }
        LogItem(CHAR_getChar(charaindex, CHAR_NAME),
                CHAR_getChar(charaindex, CHAR_CDKEY), itemindex,
                "[硅撇]酒捞袍 裙垫", CHAR_getInt(charaindex, CHAR_FLOOR),
                CHAR_getInt(charaindex, CHAR_X),
                CHAR_getInt(charaindex, CHAR_Y),
                ITEM_getChar(itemindex, ITEM_UNIQUECODE), // shan 2001/12/14
                ITEM_getChar(itemindex, ITEM_NAME),
                ITEM_getInt(itemindex, ITEM_ID), charaindex);
        makeEscapeString(ITEM_getAppropriateName(itemindex), szEscItemString,
                         sizeof(szEscItemString));
        strncat(szItemString, szEscItemString, sizeof(szItemString));
#ifdef _CHAR_GETITEMMSG
        int toindex;
        char *partyi_name;

        if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
            CHAR_PARTY_NONE) {
          snprintf(
              szBuffer, sizeof(szBuffer), "[%s](捞)啊 [%s](阑)甫 颊俊 持菌促.",
              CHAR_getUseName(charaindex), ITEM_getAppropriateName(itemindex));
          CHAR_talkToCli(charaindex, -1, szBuffer, CHAR_COLORRED);
        } else {
          for (i = 0; i <= CHAR_PARTYMAX; i++) {
            toindex = CHAR_getPartyIndex(charaindex, i);
            if (CHAR_CHECKINDEX(toindex)) {
              partyi_name = CHAR_getChar(toindex, CHAR_NAME);
              snprintf(szBuffer, sizeof(szBuffer),
                       "[%s](捞)啊 [%s](阑)甫 颊俊 持菌促.",
                       CHAR_getUseName(charaindex),
                       ITEM_getAppropriateName(itemindex));
              CHAR_talkToCli(toindex, -1, szBuffer, CHAR_COLORRED);
            }
          }
        }
#endif
        pEntryChara->getitem[i] = -1;
      }
      strncat(szItemString, "|", sizeof(szItemString));
    }
  }

  szBuffer[0] = 0;
  for (i = 0; i < RS_LIST_MAX; i++) {
    char szWork[256], sz62[64];
    szWork[0] = 0;
    if (aRsList[i].exp > 0 || i == 0) {
      cnv10to62(aRsList[i].exp, sz62, sizeof(sz62));
      snprintf(szWork, sizeof(szWork), "%d|%d|%s", aRsList[i].num,
               aRsList[i].levelup, sz62);
    }
    strncat(szBuffer, szWork, sizeof(szBuffer));
    strncat(szBuffer, ",", sizeof(szBuffer));
  }
  strncat(szBuffer, szItemString, sizeof(szBuffer));
  {
    int fd;
    fd = getfdFromCharaIndex(charaindex);
    lssproto_RS_send(fd, szBuffer);
  }
  if (itemnum > 0) {
    int inum, ie;
    for (inum = 0; inum < itemnum; inum++) {
      ie = CHAR_getItemIndex(charaindex, itemgroup[inum]);
    }
    CHAR_sendItemData(charaindex, itemgroup, itemnum);
  }
  return 0;
}

int BATTLE_GetProfit(int battleindex, int side, int num) {
  if (BattleArray[battleindex].dpbattle == 1) {
    return BATTLE_GetDuelPoint(battleindex, side, num);
  } else {
    return BATTLE_GetExpGold(battleindex, side, num);
  }
}

int BATTLE_FinishSet(battleindex) {

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  BattleArray[battleindex].mode = BATTLE_MODE_FINISH;
  return 0;
}

static int BATTLE_Finish(int battleindex) {
  BATTLE *pBattle;
  BATTLE_ENTRY *pEntry;
  int i, charaindex, j;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  if (BattleArray[battleindex].winside == -1 &&
      BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E &&
      BattleArray[battleindex].WinFunc != NULL) {
    BattleArray[battleindex].WinFunc(battleindex,
                                     BattleArray[battleindex].createindex);
  }
#ifdef _ALLBLUES_LUA
  if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E) {
    if (CHAR_CHECKINDEX(BattleArray[battleindex].createindex) == TRUE) {
      CHAR_setWorkInt(BattleArray[battleindex].createindex, CHAR_WORKBATTLEMODE,
                      BATTLE_CHARMODE_NONE);
      CHAR_sendBattleEffect(BattleArray[battleindex].createindex, OFF);
      if (BattleArray[battleindex].winside == -1) {
        RunCharBattleOverEvent(BattleArray[battleindex].createindex,
                               battleindex, 1);
      } else {
        RunCharBattleOverEvent(BattleArray[battleindex].createindex,
                               battleindex, 0);
      }
    }
  }
#endif
#ifdef _BATTLE_PK
  if (CHAR_CHECKINDEX(BattleArray[battleindex].rivalindex) &&
      CHAR_CHECKINDEX(BattleArray[battleindex].leaderindex)) {
    if (CHAR_getWorkInt(BattleArray[battleindex].rivalindex,
                        CHAR_WORK_BATTLEPK) == TRUE ||
        CHAR_getWorkInt(BattleArray[battleindex].leaderindex,
                        CHAR_WORK_BATTLEPK) == TRUE) {
      int winindex, lostindex;
      if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P) {
        if (BattleArray[battleindex].winside == -1) {
          winindex = BattleArray[battleindex].leaderindex;
          lostindex = BattleArray[battleindex].rivalindex;
        } else {
          winindex = BattleArray[battleindex].rivalindex;
          lostindex = BattleArray[battleindex].leaderindex;
        }

        int fl = 0, x = 0, y = 0;
        CHAR_getElderPosition(CHAR_getInt(lostindex, CHAR_LASTTALKELDER), &fl,
                              &x, &y);
        CHAR_warpToSpecificPoint(lostindex, fl, x, y);
        CHAR_DischargeParty(lostindex, 0);
        CHAR_setWorkInt(winindex, CHAR_WORK_BATTLEPK, FALSE);
        CHAR_setWorkInt(lostindex, CHAR_WORK_BATTLEPK, FALSE);
      }
    }
  }
#endif

#ifdef _TRADE_PK
  if (CHAR_CHECKINDEX(BattleArray[battleindex].TradeList[0].charaindex) &&
      CHAR_CHECKINDEX(BattleArray[battleindex].TradeList[1].charaindex)) {
    if (CHAR_getInt(BattleArray[battleindex].TradeList[0].charaindex,
                    CHAR_FLOOR) == 50000 &&
        CHAR_getInt(BattleArray[battleindex].TradeList[1].charaindex,
                    CHAR_FLOOR) == 50000) {
      if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P) {
        if (BattleArray[battleindex].winside == -1) {
          TRADE_HandleTradeForPK(&BattleArray[battleindex].TradeList[0],
                                 &BattleArray[battleindex].TradeList[1]);
        } else if (BattleArray[battleindex].winside == 1) {
          TRADE_HandleTradeForPK(&BattleArray[battleindex].TradeList[1],
                                 &BattleArray[battleindex].TradeList[0]);
        }
      }
    }
  }
#endif

  if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P) {
#ifdef DANTAI
    BATTLE_DpCalc(battleindex);
#endif
    BATTLE_ENTRY *pWinEntry, *pLostEntry;
    char token[256];
    if (BattleArray[battleindex].winside == -1) {
      pWinEntry = BattleArray[battleindex].Side[0].Entry;
      pLostEntry = BattleArray[battleindex].Side[1].Entry;
    } else {
      pWinEntry = BattleArray[battleindex].Side[1].Entry;
      pLostEntry = BattleArray[battleindex].Side[0].Entry;
    }

    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      int winindex = pWinEntry[i].charaindex;
      if (CHAR_CHECKINDEX(winindex) == TRUE) {

        CHAR_setInt(winindex, CHAR_DUELSTWINCOUNT,
                    CHAR_getInt(winindex, CHAR_DUELSTWINCOUNT) + 1);

#ifdef _ALLBLUES_LUA_1_9
        BattleFinishFunction(winindex, BattleArray[battleindex].CreateTime,
                             BattleArray[battleindex].turn, 1);
#endif

        CHAR_setInt(winindex, CHAR_DUELBATTLECOUNT,
                    CHAR_getInt(winindex, CHAR_DUELBATTLECOUNT) + 1);
        CHAR_setInt(winindex, CHAR_DUELWINCOUNT,
                    CHAR_getInt(winindex, CHAR_DUELWINCOUNT) + 1);

        CHAR_setInt(winindex, CHAR_DUELSTLOSECOUNT, 0);
        if (CHAR_getInt(winindex, CHAR_DUELSTWINCOUNT) >
            CHAR_getInt(winindex, CHAR_DUELMAXSTWINCOUNT)) {
          CHAR_setInt(winindex, CHAR_DUELMAXSTWINCOUNT,
                      CHAR_getInt(winindex, CHAR_DUELSTWINCOUNT));
        }
      }

      int lostindex = pLostEntry[i].charaindex;
      if (CHAR_CHECKINDEX(lostindex) == TRUE) {

        CHAR_setInt(lostindex, CHAR_DUELSTLOSECOUNT,
                    CHAR_getInt(lostindex, CHAR_DUELSTLOSECOUNT) + 1);

#ifdef _ALLBLUES_LUA_1_9
        BattleFinishFunction(lostindex, BattleArray[battleindex].CreateTime,
                             BattleArray[battleindex].turn, 0);
#endif
        CHAR_setInt(lostindex, CHAR_DUELBATTLECOUNT,
                    CHAR_getInt(lostindex, CHAR_DUELBATTLECOUNT) + 1);
        CHAR_setInt(lostindex, CHAR_DUELLOSECOUNT,
                    CHAR_getInt(lostindex, CHAR_DUELLOSECOUNT) + 1);

        CHAR_setInt(lostindex, CHAR_DUELSTWINCOUNT, 0);
      }
    }
  }

  for (j = 0; j < 2; j++) {
    pEntry = BattleArray[battleindex].Side[j].Entry;
#ifdef _PET_TALK
    for (i = 0; i < BATTLE_ENTRY_MAX; i++)
#else
    for (i = BATTLE_ENTRY_MAX - 1; i >= 0; i--)
#endif
    {
      charaindex = pEntry[i].charaindex;

      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;
#ifdef _GM_HELPER // 傈捧啊 场朝 锭 风橇 技泼
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC) {

        if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX5) > -1) {
          CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, 20000);
        } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX4) > -1) {
          CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, 15000);
        } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX3) > -1) {
          CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, 10000);
        } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX2) > -1) {
          CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, 5000);
        }
      }
#endif
      BATTLE_GetProfit(battleindex, j, i); //包括取得经验值
      BATTLE_Exit(charaindex, battleindex);
    }
  }
  if (BattleArray[battleindex].type == BATTLE_TYPE_WATCH) {
  } else {
    pBattle = BattleArray[battleindex].pNext;
    for (; pBattle; pBattle = pBattle->pNext) {
      if (BATTLE_CHECKADDRESS(pBattle) == FALSE) {
        fprint("err:battle address 奇怪(%p)\n", pBattle);
        break;
      }
      for (i = BATTLE_ENTRY_MAX - 1; i >= 0; i--) {
        charaindex = pBattle->Side[0].Entry[i].charaindex;
        if (CHAR_CHECKINDEX(charaindex) == FALSE)
          continue;
        BATTLE_Exit(charaindex, pBattle->battleindex);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_FINAL);
      }
    }
    pBattle = BattleArray[battleindex].pNext;
    for (; pBattle; pBattle = pBattle->pNext) {
      if (BATTLE_CHECKADDRESS(pBattle) == FALSE) {
        fprint("err:battle address 奇怪(%p)\n", pBattle);
        break;
      }
      BATTLE_DeleteBattle(pBattle->battleindex);
    }
  }
  BATTLE_DeleteBattle(battleindex);

  return 0;
}

int BATTLE_StopSet(battleindex) {

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  BattleArray[battleindex].mode = BATTLE_MODE_STOP;

  return 0;
}

static int BATTLE_Stop(int battleindex) {
  BATTLE_ENTRY *pEntry;
  int i, charaindex, j;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  for (j = 0; j < 2; j++) {
    pEntry = BattleArray[battleindex].Side[j].Entry;
    for (i = BATTLE_ENTRY_MAX - 1; i >= 0; i--) {
      charaindex = pEntry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;
      BATTLE_GetProfit(battleindex, j, i);
      BATTLE_Exit(charaindex, battleindex);
    }
  }

  BATTLE_DeleteBattle(battleindex);
  return 0;
}

int BATTLE_DefaultAttacker(int battleindex, int side) {
  int i, rnd, cnt;
  int CharaTbl[BATTLE_ENTRY_MAX];

  BATTLE_ENTRY *pEntry;

  pEntry = BattleArray[battleindex].Side[side].Entry;
  cnt = 0;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    CharaTbl[i] = -1;
    if (CHAR_CHECKINDEX(pEntry[i].charaindex) == FALSE) {
      continue;
    }

    if (CHAR_getWorkInt(pEntry[i].charaindex, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_RESCUE) {
      continue;
    }

    if (BATTLE_TargetCheck(battleindex, i + side * SIDE_OFFSET) == FALSE)
      continue;

    CharaTbl[cnt] = i + side * SIDE_OFFSET;
    cnt++;
  }

  if (cnt == 0) {
    return -1;
  }

  rnd = RAND(0, cnt - 1);
  return CharaTbl[rnd];
}

//*********************************************************
//
// 爵    丐月扔奶玉及戏五酸曰皿伊奶乩□卞    毛笛遥
//
int BATTLE_AddDpAlive(int battleindex, // 田玄伙奶件犯永弁旦
                      int side,        // 扔奶玉( 0 or 1 )
                      int dp)
//
//   曰袄“｛戏五酸匀化中月皿伊奶乩□及醒
//｛｛｛｛｛ 爵  互垫歹木化中卅仃木壬   及袄
//
//*********************************************************
{
  int i, charaindex;
  BATTLE_ENTRY *pEntry;
  int cnt = 0;

  // 由仿丢□正民尼永弁
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return -BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -BATTLE_ERR_BATTLEINDEX;

  // 巨件玄伉□
  pEntry = BattleArray[battleindex].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = pEntry[i].charaindex;
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    // 矢永玄反仇及端楮溢卅中
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET)
      continue;
    // 戏五化中引允［笛遥
    if (CHAR_getFlg(charaindex, CHAR_ISDIE) == FALSE) {
      CHAR_setWorkInt(charaindex, CHAR_WORKGETEXP,
                      CHAR_getWorkInt(charaindex, CHAR_WORKGETEXP) + dp);
    }
  }

  return cnt;
}

#if 1
//*********************************************************
//
// 爵    ｝皿伊奶乩□互戏五酸匀化中月井譬屯月
//
int BATTLE_CountAlive(int battleindex, // 田玄伙奶件犯永弁旦
                      int side         // 扔奶玉( 0 or 1 )
                      )
//
//   曰袄“｛戏五酸匀化中月皿伊奶乩□及醒
//｛｛｛｛｛ 爵  互垫歹木化中卅仃木壬   及袄
//
//*********************************************************
{
  int i, charaindex;
  BATTLE_ENTRY *pEntry;
  int cnt = 0;

  // 由仿丢□正民尼永弁
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return -BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -BATTLE_ERR_BATTLEINDEX;

  // 巨件玄伉□
  pEntry = BattleArray[battleindex].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = pEntry[i].charaindex;
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    // 矢永玄反仇及端楮溢卅中
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET)
      continue;
    // 戏五化中引允［
    if (CHAR_getFlg(charaindex, CHAR_ISDIE) == FALSE) {
      cnt++;
    }
  }

  return cnt;
}
#endif

//*********************************************************
//
// 爵    ｝辅爵仄凶化皿伊奶乩□及心互戏五化中月橇谪井毛民尼永弁
//
int BATTLE_OnlyRescue(int battleindex, // 田玄伙奶件犯永弁旦
                      int side,        // 扔奶玉( 0 or 1 )
                      int *pOnlyFlg)
//
//   曰袄“｛戏五酸匀化中月皿伊奶乩□及醒
//｛｛｛｛｛ 爵  互垫歹木化中卅仃木壬   及袄
//
//*********************************************************
{
  int i, charaindex;
  BATTLE_ENTRY *pEntry;
  int cnt = 0, OnlyRescue = 1;

  (*pOnlyFlg) = 0;

  // 由仿丢□正民尼永弁
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return -BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return -BATTLE_ERR_BATTLEINDEX;

  // 巨件玄伉□
  pEntry = BattleArray[battleindex].Side[side].Entry;

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    charaindex = pEntry[i].charaindex;
    if (CHAR_CHECKINDEX(charaindex) == FALSE) {
      continue;
    }
    // 矢永玄反仇及端楮溢卅中
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET)
      continue;
    // 戏五化中引允［
    if (CHAR_getFlg(charaindex, CHAR_ISDIE) == FALSE) {
      cnt++;
      // 戏五化中月支勾反辅爵    井＂
      if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE) {
      } else {
        OnlyRescue = 0;
      }
    }
#ifdef _PETSKILL_LER
    if (CHAR_getWorkInt(charaindex, CHAR_WORK_RELIFE) > 0)
      cnt++;
#endif
  }

  // 戏五化中月支勾互中化公中勾反辅爵    分匀凶日
  if (cnt > 0 && OnlyRescue) {
    // 白仿弘毛  化月
    (*pOnlyFlg) = 1;
  } else {
    // 切互匀凶日ㄟ卞允月
    (*pOnlyFlg) = 0;
  }

  return cnt;
}
//超时检测
static BOOL BATTLE_TimeOutCheck(int battleindex) {
  int i, j, charaindex;
  BATTLE *pBattle;
  BATTLE_ENTRY *pEntry;
  pBattle = &BattleArray[battleindex];
  //当前时间           战斗开始时间
  if (NowTime.tv_sec > pBattle->timer + BATTLE_TIME_LIMIT) {
  } else {
    return FALSE; //没有超时直接退出函数
  }
  // BATTLE_BroadCast( battleindex, "server时间已到。", CHAR_COLORYELLOW );
  for (j = 0; j < 2; j++) {
    pEntry = pBattle->Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      charaindex = pEntry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
        continue;
      }
      if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_C_WAIT) {           //等待指令当中
        BATTLE_Exit(charaindex, battleindex); //
        CHAR_DischargePartyNoMsg(charaindex);
        CHAR_talkToCli(charaindex, -1, "时间到，结束战斗。", CHAR_COLORYELLOW);
        BATTLE_CommandSend(charaindex, "BU");
      }
    }
  }

  return TRUE;
}

int BATTLE_WatchWait(int battleindex) {
  BATTLE *pBattle;
  BOOL commandflg = TRUE;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  pBattle = &BattleArray[battleindex];
  if (BATTLE_TimeOutCheck(battleindex) == TRUE) {
    commandflg = TRUE;
  }
  if (commandflg == FALSE) {
    return 0;
  }
  pBattle->mode = BATTLE_MODE_WATCHMOVIE;
  pBattle->turn++;
  return 0;
}

int BATTLE_WatchMovie(int battleindex) { return 0; }

int BATTLE_WatchAfter(int battleindex) {
  BattleArray[battleindex].mode = BATTLE_MODE_WATCHPRE;

  return 0;
}

int BATTLE_WatchBC(int battleindex) { return 0; }

int BATTLE_WatchPre(int battleindex) {
  // 戊穴件玉谨切尺  月凛及域  及  木
  BATTLE_PreWatchWaitSeq(battleindex);
  BattleArray[battleindex].mode = BATTLE_MODE_WATCHWAIT;
  return 0;
}

static int BATTLE_Command(int battleindex) {
  BATTLE *pBattle, *pWatchBattle;
  BOOL commandflg = TRUE, iFinish = FALSE;
  int OnlyRescue[2], i, j, charaindex;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  pBattle = &BattleArray[battleindex];
  if (BATTLE_CommandWait(battleindex, 0) == FALSE) {
    commandflg = FALSE; //上册没有指令
  }
  if (BATTLE_CommandWait(battleindex, 1) == FALSE) {
    commandflg = FALSE; //下测没有指令
  }

  pWatchBattle = pBattle->pNext; //检测观战
  for (; pWatchBattle; pWatchBattle = pWatchBattle->pNext) {
    if (BATTLE_CHECKADDRESS(pWatchBattle) == FALSE) {
      fprint("err:观战battle address错误(%p)\n", pWatchBattle);
      break;
    }

    if (pWatchBattle->mode == BATTLE_MODE_WATCHBC) {
      BATTLE_MakeCharaString(battleindex, szAllBattleString,
                             sizeof(szAllBattleString));
      BATTLE_BpSendToWatch(pWatchBattle, szAllBattleString);
      pWatchBattle->mode = BATTLE_MODE_WATCHPRE;
      commandflg = FALSE;
    } else if (pWatchBattle->mode != BATTLE_MODE_WATCHMOVIE) {
      commandflg = FALSE;
    }
  } //观战结束
  if (BATTLE_TimeOutCheck(battleindex) == TRUE) {
    commandflg = TRUE;
  }
  if (commandflg == FALSE) {
    return 0;
  }

  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      charaindex = pBattle->Side[j].Entry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == TRUE) {
        if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          // int fd = getfdFromCharaIndex(charaindex);
          // if( fd != -1 ){
          // lssproto_BATTLETIME_send(fd,99);
          //}
#ifdef _ALLBLUES_LUA_1_9
//				BattleCommand( charaindex, battleindex );
#endif
        }
      }
    }
  }

#ifdef _BATTLECOMMAND_TIME
  BattleArray[battleindex].PartTime = 0;
#endif
  pBattle->turn++; //自己回合数增加
  BATTLE_ai_all(battleindex, 0, 0);
  BATTLE_ai_all(battleindex, 1, 0);
  BATTLE_Battling(battleindex); //处理战斗
  pBattle->Side[0].flg &= ~BSIDE_FLG_SURPRISE;
  pBattle->Side[1].flg &= ~BSIDE_FLG_SURPRISE;
  if (BATTLE_OnlyRescue(battleindex, 0, &OnlyRescue[0]) == 0) {
    pBattle->winside = 1;
    iFinish = TRUE;
  } else if (BATTLE_OnlyRescue(battleindex, 1, &OnlyRescue[1]) == 0) {
    pBattle->winside = -1;
    iFinish = TRUE;
  }
  for (j = 0; j < 2; j++) {
    if (OnlyRescue[j] != 1)
      continue;
    for (i = 0; i < BATTLE_ENTRY_MAX / 2; i++) {
      charaindex = pBattle->Side[j].Entry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;

      if (CHAR_getInt(charaindex, CHAR_HP) <= 0) {
        BATTLE_Exit(charaindex, battleindex);
      }
    }
  }
  BATTLE_PreCommandSeq(battleindex);
  if (iFinish == TRUE) {
    BATTLE_FinishSet(battleindex);
  }
  return 0;
}

extern void BATTLE_changeRideImage(int index);

int BATTLE_Loop(void) {
  int i, cnt = 0;
  rand();
  for (i = 0; i < BATTLE_battlenum; i++) { //最多战斗场数
    // if( BattleArray[i].use == FALSE )continue;
    if (!BATTLE_CHECKINDEX(i))
      continue;
#ifdef _BATTLE_TIME
    struct timeval old;
    old.tv_sec = BattleArray[i].tv_sec;
    old.tv_usec = BattleArray[i].tv_usec;
    if (getBattleTime() > 0 &&
        time_diff_us(NowTime, old) < getBattleTime() * 1000.0)
      continue;
    BattleArray[i].tv_sec = NowTime.tv_sec;
    BattleArray[i].tv_usec = NowTime.tv_usec;
#endif
    if (BattleArray[i].type == BATTLE_TYPE_WATCH) { //若是观战模式
      if (BATTLE_CountAlive(i, 0) == 0) {
        BATTLE_FinishSet(i);
      }
    }

    switch (BattleArray[i].mode) {
    case BATTLE_MODE_NONE: //未战斗
      break;
    case BATTLE_MODE_INIT: //战斗初始化
      BATTLE_Init(i);
      break;
    case BATTLE_MODE_BATTLE: // Server内部战斗中
      BATTLE_Command(i);
      break;
    case BATTLE_MODE_FINISH: //战斗结束
      BATTLE_Finish(i);
      break;
    case BATTLE_MODE_STOP: //战斗中断
      BATTLE_Stop(i);
      break;
    case BATTLE_MODE_WATCHBC: //观战中...
      BATTLE_WatchBC(i);
      break;
    case BATTLE_MODE_WATCHPRE:
      BATTLE_WatchPre(i);
      break;
    case BATTLE_MODE_WATCHWAIT:
      BATTLE_WatchWait(i);
      break;
    case BATTLE_MODE_WATCHMOVIE:
      BATTLE_WatchMovie(i);
      break;
    case BATTLE_MODE_WATCHAFTER:
      BATTLE_WatchAfter(i);
      break;
    }
    cnt++;
  }

  return cnt;
}

typedef struct {
  int charaindex; // 平乓仿弁正奶件犯永弁旦
  int side;       // 扔奶玉
  int dex;        // 豳镀今
  int num;        // 巨件玄伉□  寞
  int combo;      // 宁    猾允月谛棉井＂
#ifdef _EQUIT_SEQUENCE
  int sequence;
#endif
#ifdef _PRO_ABSOLUTE_DEFENSE
  BOOL absolute_defense;
#endif
} BATTLE_CHARLIST;

typedef int (*FUNC)(const void *, const void *);

//************************************************************
//
//  爵  及豳镀今  胜楮醒
//
// 豳镀今反袄互  五中  互穸木化中月［嫦赐末□玄匹丐月［
//
static int EsCmp(const BATTLE_CHARLIST *pC1, const BATTLE_CHARLIST *pC2) {
#ifdef _PRO_ABSOLUTE_DEFENSE
  if (pC2->absolute_defense == TRUE)
    return 1;
  if (pC1->absolute_defense == TRUE)
    return 0;
#endif
#ifdef _EQUIT_SEQUENCE
  return ((pC2->dex + pC2->sequence) > (pC1->dex + pC1->sequence));

#else
  return (pC2->dex - pC1->dex);
#endif
}

#ifdef _EQUIT_SEQUENCE
void Replacement_Entry(BATTLE_CHARLIST *temp1, BATTLE_CHARLIST *temp2) {
  temp1->charaindex = temp2->charaindex;
  temp1->combo = temp2->combo;
  temp1->dex = temp2->dex;
  temp1->num = temp2->num;
  temp1->sequence = temp2->sequence;
  temp1->side = temp2->side;
}
#endif
static void EntrySort(BATTLE_CHARLIST *EntryList, int listsize) {
#ifdef _EQUIT_SEQUENCE
  //	int i, j;
  qsort(EntryList, listsize, sizeof(BATTLE_CHARLIST), (FUNC)EsCmp);
/*
        for( i=0; i<listsize; i++){
                if( EntryList[i].sequence > 0 ){
                        int maxcheck, now;
                        BATTLE_CHARLIST temp;

                        maxcheck = EntryList[i].sequence/8;
                        maxcheck = RAND( 1, maxcheck);
                        maxcheck = ( maxcheck>=i )?(i/3):maxcheck;
                        maxcheck = ( maxcheck<0)?0:maxcheck;
                        now = i;
                        for( j=0; j<maxcheck&&now>0; j++){
                                if( EntryList[now].sequence >
(EntryList[now-1].sequence *0.9) ){

                                        Replacement_Entry( &temp,
&EntryList[now]); Replacement_Entry( &EntryList[now], &EntryList[now-1]);
                                        Replacement_Entry( &EntryList[now-1],
&temp); now = now-1;


//					temp = &EntryList[now];
//					EntryList[now] = EntryList[now-1];
//					EntryList[now-1] = *temp;
//					temp = NULL;

                                }
                        }
                }
        }
*/
#else
  qsort(EntryList, listsize, sizeof(BATTLE_CHARLIST), (FUNC)EsCmp);
#endif
}

//***********************敏捷计算方式******************************
static int BATTLE_DexCalc(int charaindex) {
  int dex = 0;
  int work, COM;
  float lmzs;
  int petindex = BATTLE_getRidePet(charaindex); //取得骑宠索引

  COM = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);

#ifdef _PETSKILL_BECOMEFOX // 攻击顺序中的敏捷降下20%
  if (CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) != -1) {
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work * 0.8; //敏降20%
  }
#endif
#ifdef _PROFESSION_ADDSKILL
  if (CHAR_getWorkInt(charaindex, CHAR_WORKFEAR) > 0) {
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work * 0.8; //敏降20%
  }
#endif
  // 骚橘  猾及桦宁
  switch (COM) {
#ifdef _PROFESSION_SKILL         // WON ADD 人物职业技能
  case BATTLE_COM_S_BLOOD:       // 嗜血成性
  case BATTLE_COM_S_BLOOD_WORMS: // 嗜血蛊
  case BATTLE_COM_S_SIGN:        // 一针见血
#endif

#ifdef _BATTLE_NEWPOWER
  case BATTLE_COM_JYUJYUTU:
  case BATTLE_COM_JYUJYUTU2:
  case BATTLE_COM_S_ATTACK_MAGIC: //使用咒术计算方式
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
#ifdef _BATTLE_RAND_DEX
    dex = work - RAND(0, work * 0.01 * getBattleRandDex());
    // print("[1]乱敏指数：%d 原始敏捷:%d
    // 最终敏捷:%d\n",getBattleRandDex(),work,dex);
#else
    dex = work - RAND(0, work * 0.3);
#endif
    break;
#endif

#ifdef _PROFESSION_SKILL             // WON ADD 人物职业技能
  case BATTLE_COM_S_FIRE_ENCLOSE:    // 火附体
  case BATTLE_COM_S_ICE_ENCLOSE:     // 冰附体
  case BATTLE_COM_S_THUNDER_ENCLOSE: // 雷附体
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work - RAND(work * 0.3, work * 0.5);
    break;
  case BATTLE_COM_S_VOLCANO_SPRINGS: // 火山泉
  case BATTLE_COM_S_SUMMON_THUNDER:  // 召雷术
  case BATTLE_COM_S_ICE_ARROW:       // 冰箭术
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work - RAND(0, work * 0.2);
    break;
  case BATTLE_COM_S_CURRENT:   // 电流术
  case BATTLE_COM_S_FIRE_BALL: // 火星球
  case BATTLE_COM_S_ICE_CRACK: // 冰爆术
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work - RAND(0, work * 0.5);
    break;
  case BATTLE_COM_S_FIRE_SPEAR: // 火龙枪
  case BATTLE_COM_S_STORM:      // 暴风雨
  case BATTLE_COM_S_ICE_MIRROR: // 冰镜术
  case BATTLE_COM_S_ENCLOSE:    // 附身术
  case BATTLE_COM_S_TRANSPOSE:  // 移形换位
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work - RAND(work * 0.2, work * 0.5);
    break;

  case BATTLE_COM_S_DOOM: // 世界末日
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work - RAND(0.3, work * 0.6);
    break;
#endif

#ifdef _SKILL_SPEEDY_ATT // vincent  宠技:疾速攻击
  case BATTLE_COM_S_SPEEDYATTACK:
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work + work * 0.3; //敏加30%
    break;
#endif
#ifdef _PETSKILL_DAMAGETOHP //宠技:暗月狂狼(嗜血技的变体)
  case BATTLE_COM_S_DAMAGETOHP2:
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    dex = work + work * 0.2; //敏加20%
    break;
#endif

  case BATTLE_COM_ITEM: // 使用物品计算方式
    work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
#ifdef _BATTLE_RAND_DEX
    dex = work - RAND(0, work * 0.01 * getBattleRandDex()) + work * 0.15;
    // print("[2]乱敏指数：%d 原始敏捷:%d
    // 最终敏捷:%d\n",getBattleRandDex(),work,dex);
#else
    dex = work - RAND(0, work * 0.3) + work * 0.15;
#endif
    break;
  default: // 默认计算方式
    if (petindex == -1)
      work = CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) + 20;
    else
      work = BATTLE_adjustRidePet3A(charaindex, petindex, CHAR_WORKQUICK,
                                    ATTACKSIDE) +
             20;

#ifdef _BATTLE_RAND_DEX
    dex = work - RAND(0, work * 0.01 * getBattleRandDex());
    // print("[3]乱敏指数：%d 原始敏捷:%d
    // 最终敏捷:%d\n",getBattleRandDex(),work,dex);
#else
    dex = work - RAND(0, work * 0.3);
#endif
    break;
  }
  // if( dex <= 1 )dex = 1;
  if (dex <= 0)
    dex = RAND(1, 10); //修改后是如果负敏，就在1-5之间随机取
  return dex;
}

//*************************************************************
//
//  戊件申生□扑亦件互丐月井升丹井民尼永弁
//
static void ComboCheck(BATTLE_CHARLIST *pEntryList, int entrynum)
//
//*************************************************************
{

  int i, charaindex, com, enemy, side,
      oldside = -3,  // 赝癫卞丐曰尹卅中袄
      oldenemy = -3, // 赝癫卞丐曰尹卅中袄
      armtype, move, per,
      ComboId = 1, // 戊件示
      start = -1;
  for (i = 0; i < entrynum; i++) {

    charaindex = pEntryList[i].charaindex;
    com = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
    enemy = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
    side = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLESIDE);
    armtype = 0;
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
      per = 20; // 衬反ㄡㄟ⊙
    } else
#ifdef _LEADERBOT_COMBO
        if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
            CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERPETNPC) {
      per = 100;
    } else if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC &&
               CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
                   CHAR_PARTY_LEADER) {
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERPETNPC ||
          CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ||
          CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
        per = 100;
      }
    } else
#endif
    {
      per = 50; // 愤坌反ㄤㄟ⊙
    }

    //     互ㄟ井  仃卅井匀凶日
    if (CHAR_getInt(charaindex, CHAR_HP) <= 0 ||
        BATTLE_CanMoveCheck(charaindex) == FALSE) {
      move = 0;
    } else {
      move = 1;
    }

    //   猾础及  湛毛潸  ［髑仆烟卅日母丢
    // 髑仆烟卅日母丢
    if (BATTLE_IsThrowWepon(CHAR_getItemIndex(charaindex, CHAR_ARM)) == TRUE) {
      armtype = 1;
    }

    pEntryList[i].combo = 0; // 赓渝祭

    if (start != -1) {             // 宁    猾
      if (com != BATTLE_COM_ATTACK //   猾戊穴件玉匹卅中
          || enemy != oldenemy     // 谎匀化月衬互  元匹卅中
          || side != oldside       // 扔奶玉互啜丹
          || armtype == 1          // 髑仆烟匹丐月
          || move == 0             //   仃卅中
      ) {
        start = -1;     // 蔽
        oldside = side; // 扔奶玉创尹月
      } else {
        // 戊件示涩烂
        CHAR_setWorkInt(pEntryList[i].charaindex, CHAR_WORKBATTLECOM1,
                        BATTLE_COM_COMBO);
        pEntryList[i].combo = ComboId;
        //   赓及谛手域杀涩烂
        CHAR_setWorkInt(pEntryList[start].charaindex, CHAR_WORKBATTLECOM1,
                        BATTLE_COM_COMBO);
        pEntryList[start].combo = ComboId;
      }
    }
    if (start == -1) { // 宁    猾    仄化中卅中桦宁
      if (com == BATTLE_COM_ATTACK && armtype != 1 // 髑仆烟匹卅中
          && move == 1                             //   仃月
          && RAND(1, 100) <= per) {                // 骚橘  猾  匹丐月
        start = i;
        oldenemy = enemy; // 谎匀化月衬毛创尹月
        oldside = side;   // 扔奶玉创尹月
        ComboId++;
      }
    }
  }
}

//*************************************************************
//
//  戊件申生□扑亦件互  癫卞匹五月井民尼永弁
//｛戚及谛互戊件示匹五月橇谪元扎卅井匀凶日 FALSE
//
static BOOL ComboCheck2(BATTLE_CHARLIST *pEntryList, // 巨件玄伉□伉旦玄
                        int nownum,                  // 蜇箕及赐
                        int entrynum                 // 巨件玄伉□    醒
                        )
//
// 戊件示匹五月    TRUE
//       匹五卅中  FALSE
//
//*************************************************************
{

  int i, iRet = FALSE, ComboId, charaindex;

  // 戊件示    忡绣
  ComboId = pEntryList[nownum].combo;

  charaindex = pEntryList[nownum].charaindex;
  if (!CHAR_CHECKINDEX(charaindex)) {
    // 愤坌互镝擦蘸撩  仄化中凶日撩
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLG) &
        CHAR_BATTLEFLG_AIBAD) {
      //			print( "由於忠诚度不足使用必杀技失败( %s
      //)\n",CHAR_getUseName( charaindex ) );
      return FALSE;
    }
  }
  // 戚及谛互戊件示卞辅笛匹五月井割
  for (i = nownum + 1; i < entrynum; i++) {
    charaindex = pEntryList[i].charaindex;
    if (!CHAR_CHECKINDEX(charaindex))
      continue;
    // 戊件示    啜丹午镀仁手撩
    if (ComboId != pEntryList[i].combo)
      break;

    // 爵  卞辅笛仄化中卅井匀凶日戚尺
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) == 0)
      break;
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_FINAL)
      break;

    if (CHAR_getInt(charaindex, CHAR_HP) <= 0 ||
        BATTLE_CanMoveCheck(charaindex) == FALSE) {
    } else {
      iRet = TRUE;
      break;
    }
  }
  /*
          if( iRet == FALSE ){
                  print( "必杀技失败( %s )\n",
                          CHAR_getUseName( pEntryList[nownum].charaindex ) );
          }
  */
  return iRet;
}

void BATTLE_UltimateExtra(int battleindex, int charaindex, int enemyindex) {
  char szBuffer[256] = "";
  int pindex, pno;
  int floor = 0, x = 0, y = 0;
  szBuffer[0] = 0;

  if (CHAR_getInt(enemyindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
      || CHAR_getInt(enemyindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC
#endif
  ) {
    BATTLE_PetDefaultExit(enemyindex, battleindex);
    if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P) {
      if (CHAR_getInt(enemyindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        CHAR_setInt(enemyindex, CHAR_DUELBATTLECOUNT,
                    CHAR_getInt(enemyindex, CHAR_DUELBATTLECOUNT) + 1);
        CHAR_setInt(enemyindex, CHAR_DUELLOSECOUNT,
                    CHAR_getInt(enemyindex, CHAR_DUELLOSECOUNT) + 1);
        CHAR_setInt(enemyindex, CHAR_DUELSTWINCOUNT, 0);
      }
    } else if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E) {
      int levelflg = 1;
      if (CHAR_getInt(enemyindex, CHAR_LV) <= 10) {
        levelflg = 2;
      }
      if (BattleArray[battleindex].norisk == 0) {
        CHAR_AddCharm(enemyindex, CH_FIX_PLAYEULTIMATE / levelflg);
        pno = CHAR_getInt(enemyindex, CHAR_DEFAULTPET);
        if (0 <= pno && pno < CHAR_MAXPETHAVE) {
          pindex = CHAR_getCharPet(enemyindex, pno);
          if (CHAR_CHECKINDEX(pindex) == TRUE) {
            CHAR_PetAddVariableAi(pindex, AI_FIX_PLAYERULTIMATE / levelflg);
          }
        }
      }
      if (CHAR_getElderPosition(CHAR_getInt(enemyindex, CHAR_LASTTALKELDER),
                                &floor, &x, &y) != TRUE) {
      } else {
        CHAR_warpToSpecificPoint(enemyindex, floor, x, y);
      }
    }
    BATTLE_Exit(enemyindex, battleindex);
    CHAR_DischargePartyNoMsg(enemyindex);
    if (getBattleDebugMsg() != 0) {
      BATTLE_talkToCli(enemyindex, szBuffer, CHAR_COLORYELLOW);
    }

  } else
    // 矢永玄卅日
    if (CHAR_getInt(enemyindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      int levelflg = 1;
      int playerindex = CHAR_getWorkInt(enemyindex, CHAR_WORKPLAYERINDEX);

      // 伊矛伙互斓中桦宁反    蛹月  坌
      if (CHAR_getInt(playerindex, CHAR_LV) <= 10) {
        levelflg = 2;
      }
      // snprintf( szBuffer, sizeof(szBuffer),
      //	"(%s)被击飞。",
      //	CHAR_getUseName( enemyindex ) );

      // 潜谛及犯白巧伙玄井日反内允
      CHAR_setInt(playerindex, CHAR_DEFAULTPET, -1);

      // 犯亘巨伙动陆反镝擦艘毛票仆月
      if (BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P) {
        // 伉旦弁及  中田玄伙井＂丐月卅日镝擦    毛票仆月
        if (BattleArray[battleindex].norisk == 0) {
          CHAR_PetAddVariableAi(enemyindex, AI_FIX_PETULTIMATE / levelflg);
        }
        // 潜谛及矢永玄韶氏分荚醒毛市它件玄允月
        CHAR_setInt(playerindex, CHAR_DEADPETCOUNT,
                    CHAR_getInt(playerindex, CHAR_DEADPETCOUNT) + 1);
      }
      /*
                      // 矢永玄互中凶日爵  井日厄仃今六月［
                      BATTLE_PetDefaultExit( enemyindex, battleindex );
      */

      // 愤坌手  仃月
      BATTLE_Exit(enemyindex, battleindex);

    } else {
      int flg;
      // 公木动陆匹失伙  奴丢永玄韶
      // snprintf( szBuffer, sizeof(szBuffer),
      //	"(%s)被击飞。",
      //	CHAR_getUseName( enemyindex ) );
      // 衬平乓仿反扔□田□卞酸仄化云仁
      //		BATTLE_Exit( enemyindex, battleindex );
      flg = CHAR_getWorkInt(enemyindex, CHAR_WORKBATTLEFLG);
      flg |= CHAR_BATTLEFLG_ULTIMATE; // 失伙  奴丢永玄熬仃凶
      CHAR_setWorkInt(enemyindex, CHAR_WORKBATTLEFLG, flg);
      BATTLE_Exit(enemyindex, battleindex);
    }

  //   须  煤
  // BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORYELLOW );
}

//*************************************************************
//
//  骚橘卞竣濮今六凶桦宁及  溃质
//
void BATTLE_NormalDeadExtra(int battleindex, // 爵  奶件犯永弁旦
                            int charaindex, // 诮仄凶平乓仿及奶件犯永弁旦
                            int enemyindex // 诮今木凶平乓仿及奶件犯永弁旦
                            )
//
//
//*************************************************************
{
  int pindex, pno;
  char szBuffer[256] = "";

  szBuffer[0] = 0;

  // 皿伊奶乩□匹
  // 衬午及爵  分匀凶日
  // 伉旦弁及  中田玄伙卅日
  if (CHAR_getInt(enemyindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E &&
      BattleArray[battleindex].norisk == 0) {
    int levelflg = 1;
    // 愤坌及    毛票仆月
    // 伊矛伙ㄠㄟ动票及谛反      坌分仃票互月
    if (CHAR_getInt(enemyindex, CHAR_LV) <= 10) {
      levelflg = 2;
    }
    CHAR_AddCharm(enemyindex, CH_FIX_PLAYERDEAD / levelflg);
    // 爵  卞辅笛仄化中凶矢永玄及    毛票仆月
    pno = CHAR_getInt(enemyindex, CHAR_DEFAULTPET);
    if (0 <= pno && pno < CHAR_MAXPETHAVE) {
      // 矢永玄及奶件犯永弁旦
      pindex = CHAR_getCharPet(enemyindex, pno);
      if (CHAR_CHECKINDEX(pindex) == TRUE) {
        CHAR_PetAddVariableAi(pindex, AI_FIX_PLAYERDEAD / levelflg);
      }
    }
    // 韶氏分平乓仿及戊穴件玉反侉木月
    CHAR_setWorkInt(enemyindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
  } else
    // 矢永玄卅日
    if (CHAR_getInt(enemyindex, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
        BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E &&
        BattleArray[battleindex].norisk == 0) {
      int levelflg = 1;
      int playerindex = CHAR_getWorkInt(enemyindex, CHAR_WORKPLAYERINDEX);

      // 潜谛及伊矛伙ㄠㄟ动票及谛反      坌分仃票互月
      if (CHAR_getInt(playerindex, CHAR_LV) <= 10) {
        levelflg = 2;
      }
      // 愤坌及镝擦艘毛票仆月
      CHAR_PetAddVariableAi(enemyindex, AI_FIX_PETDEAD / levelflg);
      // 潜谛及矢永玄韶氏分荚醒毛市它件玄允月
      CHAR_setInt(playerindex, CHAR_DEADPETCOUNT,
                  CHAR_getInt(playerindex, CHAR_DEADPETCOUNT) + 1);

      // 韶氏分平乓仿及戊穴件玉反侉木月
      CHAR_setWorkInt(enemyindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);

    } else {
      // 公木动陆
    }

  // snprintf( szBuffer, sizeof(szBuffer),
  //	"(%s)失去意识。",
  //	CHAR_getUseName( enemyindex ) );

  // BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORYELLOW );
}

#ifndef DANTAI
void BATTLE_EscapeDpSend(int battleindex, int charaindex) {

  int enemyside, cnt, dpadd;
  BATTLE_ENTRY *pEntry;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    return;
  }

  if (BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P) {
    return;
  }
  if (BattleArray[battleindex].flg & BATTLE_FLG_FREEDP) {
    CHAR_setWorkInt(charaindex, CHAR_WORKGETEXP, 0);
    return;
  }
  BattleArray[battleindex].flg |= BATTLE_FLG_CHARALOST;
  enemyside = 1 - CHAR_getWorkInt(charaindex, CHAR_WORKBATTLESIDE);
  pEntry = BattleArray[battleindex].Side[enemyside].Entry;
  cnt = BATTLE_CountAlive(battleindex, enemyside);
  if (cnt == 0)
    return;

  dpadd = CHAR_getInt(charaindex, CHAR_DUELPOINT) * DUELPOINT_RATE;

  if (dpadd < 1)
    dpadd = 1;
  CHAR_setWorkInt(charaindex, CHAR_WORKGETEXP, -dpadd * 2);
  dpadd /= cnt;
  if (dpadd < 1)
    dpadd = 1;
  BATTLE_AddDpAlive(battleindex, enemyside, dpadd);

  if (BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P) {
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      CHAR_setInt(charaindex, CHAR_DUELBATTLECOUNT,
                  CHAR_getInt(charaindex, CHAR_DUELBATTLECOUNT) + 1);
      CHAR_setInt(charaindex, CHAR_DUELLOSECOUNT,
                  CHAR_getInt(charaindex, CHAR_DUELLOSECOUNT) + 1);
      CHAR_setInt(charaindex, CHAR_DUELSTWINCOUNT, 0);
    }
  }
}
#else
void BATTLE_EscapeDpSend(int battleindex, int charaindex) {

  int enemyside, dpadd;
  BATTLE_ENTRY *pEntry;

  // Nuke 0725: Avoid too large number
  if ((battleindex < 0) || (battleindex > getBattlenum()))
    return;
  //         动陆反  仃月
  if (BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P) {
    return;
  }

  // 锹澎扔奶玉
  enemyside = 1 - CHAR_getWorkInt(charaindex, CHAR_WORKBATTLESIDE);

  pEntry = BattleArray[battleindex].Side[enemyside].Entry;

  // 锹澎础卞中月谛醒毛醒尹月
  dpadd = CHAR_getInt(charaindex, CHAR_DUELPOINT) * DUELPOINT_RATE;
  if (dpadd < 1)
    dpadd = 1; //   斓匹手ㄠ

  //   谛井日娄中化云仁
  CHAR_setWorkInt(charaindex, CHAR_WORKGETEXP, -dpadd * 2);

  // 锹澎础卞反箫允
  BattleArray[battleindex].Side[enemyside].common_dp += dpadd;
}
#endif

#ifndef DANTAI
int BATTLE_AddDuelPoint(int battleindex, int *pBidList) {
  int enemyindex, i, side, num, j, k, charaindex[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY
  *pEntryEnemy;
  int allnum = 0;
  int bid = pBidList[0];

  if (pBidList < 0)
    return BATTLE_ERR_PARAM;
  if (bid >= SIDE_OFFSET) {
    num = bid - SIDE_OFFSET;
    side = 1;
  } else {
    num = bid;
    side = 0;
  }

  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;

  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++) {
    charaindex[i] = -1;
  }

  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int work;
    work = BATTLE_No2Index(battleindex, pBidList[i]);
    if (work < 0)
      return BATTLE_ERR_PARAM;
    if (CHAR_getInt(work, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
      continue;
    }
    if (CHAR_getInt(work, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      work = CHAR_getWorkInt(work, CHAR_WORKPLAYERINDEX);
    }
    if (work < 0)
      return BATTLE_ERR_PARAM;
    for (k = 0; k < allnum; k++) {
      if (charaindex[i] == work)
        break;
    }
    if (i < allnum)
      continue;
    charaindex[i] = work;
    allnum++;
  }
  charaindex[i] = -1;

  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battleindex].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      int enemytype = 0;
      enemyindex = pEntryEnemy[i].charaindex;
      if (CHAR_CHECKINDEX(enemyindex) == FALSE)
        continue;
      if (CHAR_getInt(enemyindex, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemyindex, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }
      enemytype = CHAR_getInt(enemyindex, CHAR_WHICHTYPE);
      if (enemytype == CHAR_TYPEPLAYER) {
        BattleArray[battleindex].flg |= BATTLE_FLG_CHARALOST;
      }
      if (enemytype == CHAR_TYPEPLAYER || enemytype == CHAR_TYPEENEMY) {
        int dpadd, dpnow;
        dpnow = CHAR_getInt(enemyindex, CHAR_DUELPOINT);
        dpadd = dpnow * DUELPOINT_RATE;

        dpnow -= dpadd;
        CHAR_setWorkInt(enemyindex, CHAR_WORKGETEXP,
                        CHAR_getWorkInt(enemyindex, CHAR_WORKGETEXP) - dpadd);

        if (side != j) {
          for (k = 0; charaindex[k] != -1; k++)
            ;
          if (k <= 0)
            k = 1;
          dpadd /= k;
          if (dpadd <= 0)
            dpadd = 1;
          for (k = 0; charaindex[k] != -1; k++) {
            CHAR_setWorkInt(charaindex[k], CHAR_WORKGETEXP,
                            CHAR_getWorkInt(charaindex[k], CHAR_WORKGETEXP) +
                                dpadd);
          }
        } else {
          int dpdiv = dpadd, alive;
          alive = BATTLE_CountAlive(battleindex, 1 - j);
          if (alive <= 0)
            alive = 1;
          dpdiv /= alive;
          if (dpdiv <= 0)
            dpdiv = 1;
          BATTLE_AddDpAlive(battleindex, 1 - j, dpdiv);
        }
      }
      CHAR_setFlg(enemyindex, CHAR_ISDIE, 1);
      CHAR_setInt(enemyindex, CHAR_DEADCOUNT,
                  CHAR_getInt(enemyindex, CHAR_DEADCOUNT) + 1);
      if (pEntryEnemy[i].flg & BENT_FLG_ULTIMATE) {
        BATTLE_GetProfit(battleindex, j, i);
        BATTLE_UltimateExtra(battleindex, charaindex[0], enemyindex);
      } else {
        BATTLE_NormalDeadExtra(battleindex, charaindex[0], enemyindex);
      }
    }
  }
  return 0;
}
#else

int BATTLE_AddDuelPoint(int battleindex, int *pBidList) {
  int enemyindex, i, otherside, j, charaindex[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY
  *pEntryEnemy;
  if (pBidList < 0)
    return BATTLE_ERR_PARAM;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battleindex].Side[j].Entry;
    otherside = 1 - j;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      enemyindex = pEntryEnemy[i].charaindex;
      if (CHAR_CHECKINDEX(enemyindex) == FALSE)
        continue;
      if (CHAR_getInt(enemyindex, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemyindex, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }

      CHAR_setFlg(enemyindex, CHAR_ISDIE, 1);
      CHAR_setInt(enemyindex, CHAR_DEADCOUNT,
                  CHAR_getInt(enemyindex, CHAR_DEADCOUNT) + 1);
      if (pEntryEnemy[i].flg & BENT_FLG_ULTIMATE) {
        if (CHAR_getInt(enemyindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          int dpadd, dpnow;
          dpnow = CHAR_getInt(enemyindex, CHAR_DUELPOINT);
          dpadd = dpnow * DUELPOINT_RATE; // ㄠㄟ坌及ㄠ手日尹月
          dpnow -= dpadd;
          CHAR_setWorkInt(enemyindex, CHAR_WORKGETEXP,
                          CHAR_getWorkInt(enemyindex, CHAR_WORKGETEXP) - dpadd);
          BattleArray[battleindex].Side[otherside].common_dp += dpadd;
          BATTLE_GetProfit(battleindex, j, i);
        }
        BATTLE_UltimateExtra(battleindex, charaindex[0], enemyindex);
      } else {
        BATTLE_NormalDeadExtra(battleindex, charaindex[0], enemyindex);
      }
    }
  }
  return 0;
}
#endif

int BATTLE_ItemDelCheck(int itemindex) {
  int icnt, jcnt, playernum;
  playernum = getFdnum();
  for (icnt = 0; icnt < playernum; icnt++) {
    if (CHAR_CHECKINDEX(icnt)) {
      for (jcnt = 0; jcnt < CHAR_getMaxInventory(icnt); jcnt++) {
        if (CHAR_getItemIndex(icnt, jcnt) == itemindex) {
          print("err:战利品奇怪的item处理(%s)(%s)index(%d)\n",
                CHAR_getUseName(icnt), ITEM_getAppropriateName(itemindex),
                itemindex);
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

#define EXPGET_MAXLEVEL 5
#define EXPGET_DIV 15

#ifdef _NB_CF_SHARE_EXP
int BATTLE_AddExp(int battleindex, int *pBidList) {
  int enemyindex, i, side, num, proflg = 1, j, exp, k, enemylevel,
                                charaindex[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY
  *pEntryEnemy, *pEntryChara[BATTLE_ENTRY_MAX + 1],
      *pEntryPlayer[BATTLE_ENTRY_MAX + 1];
  int item = 0, itemindex, itemloop, allnum = 0;
  int bid = pBidList[0];
  if (pBidList[0] < 0)
    return BATTLE_ERR_PARAM;
  if (bid >= SIDE_OFFSET) {
    num = bid - SIDE_OFFSET;
    side = 1;
  } else {
    num = bid;
    side = 0;
  }
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++) {
    charaindex[i] = -1;
    pEntryPlayer[i] = NULL;
  }

  if (BattleArray[battleindex].Side[side].type != BATTLE_S_TYPE_PLAYER ||
      BattleArray[battleindex].Side[1 - side].type == BATTLE_S_TYPE_PLAYER) {
    proflg = 0;
  }
  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int subnum;
    charaindex[i] = BATTLE_No2Index(battleindex, pBidList[i]);
    if (charaindex[i] < 0)
      return BATTLE_ERR_PARAM;
    subnum = pBidList[i] - side * SIDE_OFFSET;
    pEntryChara[i] = &BattleArray[battleindex].Side[side].Entry[subnum];
    if (CHAR_getInt(charaindex[i], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      pEntryPlayer[i] = &BattleArray[battleindex].Side[side].Entry[subnum - 5];
    } else {
      pEntryPlayer[i] = pEntryChara[i];
    }
  }

  charaindex[i] = -1;
  allnum = i;
  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battleindex].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      enemyindex = pEntryEnemy[i].charaindex;
      if (CHAR_CHECKINDEX(enemyindex) == FALSE)
        continue;

      if (CHAR_getInt(enemyindex, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemyindex, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }
      if (proflg == 1 && side != j) {

        exp = CHAR_getInt(enemyindex, CHAR_EXP);
        enemylevel = CHAR_getInt(enemyindex, CHAR_LV);
        for (k = 0; charaindex[k] != -1; k++) {
          int nowexp, b_level;
          int ridepet;
          b_level = CHAR_getInt(charaindex[k], CHAR_LV) - enemylevel;
          if (b_level <= EXPGET_MAXLEVEL) { //如 人物大怪物5以下 或怪物大於人物
            nowexp = exp;
          } else { //如果人物大於怪物5以上
            b_level = EXPGET_MAXLEVEL + EXPGET_DIV - b_level;
            if (b_level > EXPGET_DIV)
              b_level = EXPGET_DIV;
            if (b_level <= 0) {
              nowexp = 1;
            } else {
              nowexp = exp * b_level / EXPGET_DIV;
            }
            if (nowexp < 1)
              nowexp = 1;
          }
          CHAR_setWorkInt(charaindex[k], CHAR_WORKGETEXP,
                          CHAR_getWorkInt(charaindex[k], CHAR_WORKGETEXP) +
                              nowexp);
          CHAR_setInt(charaindex[k], CHAR_KILLPETCOUNT,
                      CHAR_getInt(charaindex[k], CHAR_KILLPETCOUNT) + 1);
          ridepet = BATTLE_getRidePet(charaindex[k]);
          // andy_edit
          if (CHAR_CHECKINDEX(ridepet)) {
            int nowexp, b_level;
            b_level = CHAR_getInt(ridepet, CHAR_LV) - enemylevel;
            if (b_level <= EXPGET_MAXLEVEL) {
              nowexp = exp;
            } else {
              b_level = EXPGET_MAXLEVEL + EXPGET_DIV - b_level;
              if (b_level > EXPGET_DIV)
                b_level = EXPGET_DIV;
              if (b_level <= 0) {
                nowexp = 1;
              } else {
                nowexp = exp * b_level / EXPGET_DIV;
              }
              if (nowexp < 1)
                nowexp = 1;
            }
            nowexp *= 0.6;

            CHAR_setWorkInt(ridepet, CHAR_WORKGETEXP,
                            CHAR_getWorkInt(ridepet, CHAR_WORKGETEXP) + nowexp);
            CHAR_setInt(ridepet, CHAR_KILLPETCOUNT,
                        CHAR_getInt(ridepet, CHAR_KILLPETCOUNT) + 1);
          }
          if (BattleArray[battleindex].norisk == 0 &&
              CHAR_getInt(charaindex[k], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
            if (CHAR_getInt(enemyindex, CHAR_LV) >
                CHAR_getInt(charaindex[k], CHAR_LV)) {
              CHAR_PetAddVariableAi(charaindex[k], AI_FIX_PETGOLDWIN);
            } else {
              CHAR_PetAddVariableAi(charaindex[k], AI_FIX_PETWIN);
            }
          }
        }
        CHAR_setMaxExp(enemyindex, 0);
      }
      CHAR_setFlg(enemyindex, CHAR_ISDIE, 1);
#ifdef _PET_LIMITLEVEL
      Pet_Check_Die(enemyindex);
#endif
      CHAR_setInt(enemyindex, CHAR_DEADCOUNT,
                  CHAR_getInt(enemyindex, CHAR_DEADCOUNT) + 1);
      if (pEntryEnemy[i].flg & BENT_FLG_ULTIMATE) {
        BATTLE_UltimateExtra(battleindex, charaindex[0], enemyindex);
      } else {
        BATTLE_NormalDeadExtra(battleindex, charaindex[0], enemyindex);
      }
    }
  }
  return 0;
}

int BATTLE_AddItem(int battleindex, int *pBidList) {
  int enemyindex, i, side, num, proflg = 1, j, exp, k, enemylevel,
                                charaindex[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY
  *pEntryEnemy, *pEntryChara[BATTLE_ENTRY_MAX + 1],
      *pEntryPlayer[BATTLE_ENTRY_MAX + 1];
  int item = 0, itemindex, itemloop, allnum = 0;
  int bid = pBidList[0];
  if (pBidList[0] < 0)
    return BATTLE_ERR_PARAM;
  if (bid >= SIDE_OFFSET) {
    num = bid - SIDE_OFFSET;
    side = 1;
  } else {
    num = bid;
    side = 0;
  }
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++) {
    charaindex[i] = -1;
    pEntryPlayer[i] = NULL;
  }
  if (BattleArray[battleindex].Side[side].type != BATTLE_S_TYPE_PLAYER ||
      BattleArray[battleindex].Side[1 - side].type == BATTLE_S_TYPE_PLAYER) {
    proflg = 0;
  }
  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int subnum;
    charaindex[i] = BATTLE_No2Index(battleindex, pBidList[i]);
    if (charaindex[i] < 0)
      return BATTLE_ERR_PARAM;
    subnum = pBidList[i] - side * SIDE_OFFSET;
    pEntryChara[i] = &BattleArray[battleindex].Side[side].Entry[subnum];
    if (CHAR_getInt(charaindex[i], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      pEntryPlayer[i] = &BattleArray[battleindex].Side[side].Entry[subnum - 5];
    } else {
      pEntryPlayer[i] = pEntryChara[i];
    }
  }

  charaindex[i] = -1;
  allnum = i;
  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battleindex].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      enemyindex = pEntryEnemy[i].charaindex;
      if (CHAR_CHECKINDEX(enemyindex) == FALSE)
        continue;
      if (CHAR_getInt(enemyindex, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemyindex, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }
      if (proflg == 1 && side != j) {
        if (CHAR_getInt(enemyindex, CHAR_WHICHTYPE) != CHAR_TYPEENEMY) {
          print("\nerr: 夺取敌人以外的战利品(%s:%d)\n", __FILE__, __LINE__);
        }
        for (item = CHAR_STARTITEMARRAY; item < CHAR_STARTITEMARRAY + 10;
             item++) {
          itemindex = CHAR_getItemIndex(enemyindex, item);
          if (ITEM_CHECKINDEX(itemindex) == FALSE) {
            continue;
          }
          if (ITEM_getWorkInt(itemindex, ITEM_WORKCHARAINDEX) <= getFdnum()) {
            print("\nerr: 将玩家的item当做战利品(%s:%d)\n", __FILE__, __LINE__);
            continue;
          }
          CHAR_setItemIndex(enemyindex, item, -1);
          k = RAND(0, (allnum - 1));
          for (itemloop = 0; itemloop < 3; itemloop++) {
            if (pEntryPlayer[k]->getitem[itemloop] == -1) {
              pEntryPlayer[k]->getitem[itemloop] = itemindex;
              break;
            }
          }
          if (itemloop >= 3) {
            if (RAND(0, 1)) {
              int olditem;
              int randwork = RAND(0, 2);
              olditem = pEntryPlayer[k]->getitem[randwork];
              pEntryPlayer[k]->getitem[randwork] = itemindex;
              if (BATTLE_ItemDelCheck(olditem) == TRUE) {
                ITEM_endExistItemsOne(olditem);
              }
            } else {
              ITEM_endExistItemsOne(itemindex);
            }
          }
        }
      }
    }
  }
  return 0;
}
#endif

int BATTLE_AddExpItem(int battleindex, int *pBidList) {
  int enemyindex, i, side, num, proflg = 1, j, exp, k, enemylevel,
                                charaindex[BATTLE_ENTRY_MAX + 1];
  BATTLE_ENTRY
  *pEntryEnemy, *pEntryChara[BATTLE_ENTRY_MAX + 1],
      *pEntryPlayer[BATTLE_ENTRY_MAX + 1];
  int item = 0, itemindex, itemloop, allnum = 0;
  int bid = pBidList[0];
  if (pBidList[0] < 0)
    return BATTLE_ERR_PARAM;
  if (bid >= SIDE_OFFSET) {
    num = bid - SIDE_OFFSET;
    side = 1;
  } else {
    num = bid;
    side = 0;
  }
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (num < 0 || num >= BATTLE_ENTRY_MAX)
    return BATTLE_ERR_PARAM;
  for (i = 0; i < BATTLE_ENTRY_MAX + 1; i++) {
    charaindex[i] = -1;
    pEntryPlayer[i] = NULL;
  }

  if (BattleArray[battleindex].Side[side].type != BATTLE_S_TYPE_PLAYER ||
      BattleArray[battleindex].Side[1 - side].type == BATTLE_S_TYPE_PLAYER) {
    proflg = 0;
  }
  for (i = 0; i < BATTLE_ENTRY_MAX && pBidList[i] != -1; i++) {
    int subnum;
    charaindex[i] = BATTLE_No2Index(battleindex, pBidList[i]);
    if (charaindex[i] < 0)
      return BATTLE_ERR_PARAM;
    subnum = pBidList[i] - side * SIDE_OFFSET;
    pEntryChara[i] = &BattleArray[battleindex].Side[side].Entry[subnum];
    if (CHAR_getInt(charaindex[i], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      pEntryPlayer[i] = &BattleArray[battleindex].Side[side].Entry[subnum - 5];
    } else {
      pEntryPlayer[i] = pEntryChara[i];
    }
  }
  charaindex[i] = -1;
  allnum = i;
  for (j = 0; j < 2; j++) {
    pEntryEnemy = BattleArray[battleindex].Side[j].Entry;
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      enemyindex = pEntryEnemy[i].charaindex;
      if (CHAR_CHECKINDEX(enemyindex) == FALSE)
        continue;

      if (CHAR_getInt(enemyindex, CHAR_HP) <= 0 &&
          CHAR_getFlg(enemyindex, CHAR_ISDIE) == FALSE) {
      } else {
        continue;
      }
      if (proflg == 1 && side != j) {
        if (CHAR_getInt(enemyindex, CHAR_WHICHTYPE) != CHAR_TYPEENEMY) {
          print("\nerr: 夺取敌人以外的战利品(%s:%d)\n", __FILE__, __LINE__);
        }
        for (item = CHAR_STARTITEMARRAY; item < CHAR_STARTITEMARRAY + 10;
             item++) {
          itemindex = CHAR_getItemIndex(enemyindex, item);
          if (ITEM_CHECKINDEX(itemindex) == FALSE) {
            continue;
          }
          if (ITEM_getWorkInt(itemindex, ITEM_WORKCHARAINDEX) <= getFdnum()) {
            print("\nerr: 将玩家的item当做战利品(%s:%d)\n", __FILE__, __LINE__);
            continue;
          }
          CHAR_setItemIndex(enemyindex, item, -1);
          k = RAND(0, (allnum - 1));
          for (itemloop = 0; itemloop < 3; itemloop++) {
            if (pEntryPlayer[k]->getitem[itemloop] == -1) {
              pEntryPlayer[k]->getitem[itemloop] = itemindex;
              break;
            }
          }
          if (itemloop >= 3) {
            if (RAND(0, 1)) {
              int olditem;
              int randwork = RAND(0, 2);
              olditem = pEntryPlayer[k]->getitem[randwork];
              pEntryPlayer[k]->getitem[randwork] = itemindex;
              if (BATTLE_ItemDelCheck(olditem) == TRUE) {
                ITEM_endExistItemsOne(olditem);
              }
            } else {
              ITEM_endExistItemsOne(itemindex);
            }
          }
        }
        exp = CHAR_getInt(enemyindex, CHAR_EXP);
        enemylevel = CHAR_getInt(enemyindex, CHAR_LV);
        for (k = 0; charaindex[k] != -1; k++) {
          int nowexp, b_level;
          int ridepet;
          b_level = CHAR_getInt(charaindex[k], CHAR_LV) - enemylevel;
          if (b_level <= EXPGET_MAXLEVEL) { //如 人物大怪物5以下 或怪物大於人物
            nowexp = exp;
          } else { //如果人物大於怪物5以上
            b_level = EXPGET_MAXLEVEL + EXPGET_DIV - b_level;
            if (b_level > EXPGET_DIV)
              b_level = EXPGET_DIV;
            if (b_level <= 0) {
              nowexp = 1;
            } else {
              nowexp = exp * b_level / EXPGET_DIV;
            }
            if (nowexp < 1)
              nowexp = 1;
          }
          CHAR_setWorkInt(charaindex[k], CHAR_WORKGETEXP,
                          CHAR_getWorkInt(charaindex[k], CHAR_WORKGETEXP) +
                              nowexp);
          CHAR_setInt(charaindex[k], CHAR_KILLPETCOUNT,
                      CHAR_getInt(charaindex[k], CHAR_KILLPETCOUNT) + 1);
          ridepet = BATTLE_getRidePet(charaindex[k]);
          // andy_edit
          if (CHAR_CHECKINDEX(ridepet)) {
            int nowexp, b_level;
            b_level = CHAR_getInt(ridepet, CHAR_LV) - enemylevel;
            if (b_level <= EXPGET_MAXLEVEL) {
              nowexp = exp;
            } else {
              b_level = EXPGET_MAXLEVEL + EXPGET_DIV - b_level;
              if (b_level > EXPGET_DIV)
                b_level = EXPGET_DIV;
              if (b_level <= 0) {
                nowexp = 1;
              } else {
                nowexp = exp * b_level / EXPGET_DIV;
              }
              if (nowexp < 1)
                nowexp = 1;
            }
            nowexp *= 0.6;

            CHAR_setWorkInt(ridepet, CHAR_WORKGETEXP,
                            CHAR_getWorkInt(ridepet, CHAR_WORKGETEXP) + nowexp);
            CHAR_setInt(ridepet, CHAR_KILLPETCOUNT,
                        CHAR_getInt(ridepet, CHAR_KILLPETCOUNT) + 1);
          }
          if (BattleArray[battleindex].norisk == 0 &&
              CHAR_getInt(charaindex[k], CHAR_WHICHTYPE) == CHAR_TYPEPET) {
            if (CHAR_getInt(enemyindex, CHAR_LV) >
                CHAR_getInt(charaindex[k], CHAR_LV)) {
              CHAR_PetAddVariableAi(charaindex[k], AI_FIX_PETGOLDWIN);
            } else {
              CHAR_PetAddVariableAi(charaindex[k], AI_FIX_PETWIN);
            }
          }
        }
        CHAR_setMaxExp(enemyindex, 0);
      }
      CHAR_setFlg(enemyindex, CHAR_ISDIE, 1);
#ifdef _PET_LIMITLEVEL
      Pet_Check_Die(enemyindex);
#endif
      CHAR_setInt(enemyindex, CHAR_DEADCOUNT,
                  CHAR_getInt(enemyindex, CHAR_DEADCOUNT) + 1);
      if (pEntryEnemy[i].flg & BENT_FLG_ULTIMATE) {
        BATTLE_UltimateExtra(battleindex, charaindex[0], enemyindex);
      } else {
        BATTLE_NormalDeadExtra(battleindex, charaindex[0], enemyindex);
      }
    }
  }
  return 0;
}

#ifdef _PET_LIMITLEVEL
void Pet_Check_Die(int petindex) {

  int LevelUpPoint;
  int vital, str, tgh, dex;
  float modai;

#define RAND(x, y)                                                             \
  ((x - 1) + 1 + (int)((double)(y - (x - 1)) * rand() / (RAND_MAX + 1.0)))
  //玛宠死亡  扣基本属性及忠诚
  if (CHAR_getInt(petindex, CHAR_PETID) == 718) {
    LevelUpPoint = CHAR_getInt(petindex, CHAR_ALLOCPOINT);
    vital = ((LevelUpPoint >> 24) & 0xff);
    str = ((LevelUpPoint >> 16) & 0xff);
    tgh = ((LevelUpPoint >> 8) & 0xff);
    dex = ((LevelUpPoint >> 0) & 0xff);
    //			enemyindex = pEntryEnemy[i].charaindex;

    print("\n lvup <<%d %d %d %d>>", vital, str, tgh, dex);

    vital = vital - RAND(1, 8);
    str = str - RAND(1, 4);
    tgh = tgh - RAND(1, 4);
    dex = dex - RAND(1, 4);
    print("\n lvup <<%d %d %d %d>>", vital, str, tgh, dex);

    if (vital < 0)
      vital = 0;
    if (str < 0)
      str = 0;
    if (tgh < 0)
      tgh = 0;
    if (dex < 0)
      dex = 0;

    if (vital > 50)
      vital = 50;
    if (str > 50)
      str = 50;
    if (tgh > 50)
      tgh = 50;
    if (dex > 50)
      dex = 50;
    LevelUpPoint = (vital << 24) + (str << 16) + (tgh << 8) + (dex << 0);
    CHAR_setInt(petindex, CHAR_ALLOCPOINT, LevelUpPoint);
    print("\n lvup <<%d %d %d %d>>", vital, str, tgh, dex);
    modai = CHAR_getInt(petindex, CHAR_MODAI);
    modai = modai - ((modai * 5) / 100);
    CHAR_setInt(petindex, CHAR_MODAI, modai);

    LogPetPointChange(CHAR_getChar(petindex, CHAR_OWNERCHARANAME),
                      CHAR_getChar(petindex, CHAR_OWNERCDKEY),
                      CHAR_getChar(petindex, CHAR_NAME), petindex, 2,
                      CHAR_getInt(petindex, CHAR_LV), "battle_die(战斗死亡)",
                      CHAR_getInt(petindex, CHAR_FLOOR), 0, 0);
  }
  return;
}
#endif

int BATTLE_AddProfit(int battleindex, int *pBidList) {
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;

  if (BattleArray[battleindex].dpbattle == 1) {
    return BATTLE_AddDuelPoint(battleindex, pBidList);
  } else {
#ifdef _NB_CF_SHARE_EXP
    if (getExpShare() == 1 &&
        BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E) {
      if (pBidList[0] >= SIDE_OFFSET || pBidList[0] < 0)
        return BATTLE_AddExpItem(battleindex, pBidList);

      BATTLE_AddItem(battleindex, pBidList);
      int aAttackList[BATTLE_ENTRY_MAX + 1];
      int i = 0;
      int k = 0;
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        if (CHAR_CHECKINDEX(
                BattleArray[battleindex].Side[0].Entry[i].charaindex) == FALSE)
          continue;
        if (BATTLE_CanMoveCheck(
                BattleArray[battleindex].Side[0].Entry[i].charaindex) ==
            FALSE) {
          continue;
        }
        if (CHAR_getInt(BattleArray[battleindex].Side[0].Entry[i].charaindex,
                        CHAR_HP) <= 0) {
          continue;
        }
        aAttackList[k++] = BattleArray[battleindex].Side[0].Entry[i].bid;
      }
      aAttackList[k] = -1;
      return BATTLE_AddExp(battleindex, aAttackList);
    } else {
      return BATTLE_AddExpItem(battleindex, pBidList);
    }
#else
    int aAttackList[BATTLE_ENTRY_MAX + 1];
    return BATTLE_AddExpItem(battleindex, pBidList);
#endif
  }
}

int BATTLE_TargetCheck(int battleindex, int defNo) {
  int defindex = BATTLE_No2Index(battleindex, defNo);

  if (CHAR_CHECKINDEX(defindex) == FALSE ||
      CHAR_getWorkInt(defindex, CHAR_WORKBATTLEMODE) == 0 ||
      CHAR_getFlg(defindex, CHAR_ISDIE) == TRUE ||
      CHAR_getInt(defindex, CHAR_HP) <= 0 ||
      CHAR_getFlg(defindex, CHAR_ISATTACKED) == FALSE ||
      CHAR_getWorkInt(defindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE) {
    return FALSE;
  }
  return TRUE;
}

int BATTLE_TargetCheckDead(int battleindex, int defNo) {
  int defindex = BATTLE_No2Index(battleindex, defNo);
  if (CHAR_CHECKINDEX(defindex) == FALSE ||
      CHAR_getWorkInt(defindex, CHAR_WORKBATTLEMODE) == 0 ||
      CHAR_getWorkInt(defindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_RESCUE ||
      CHAR_getFlg(defindex, CHAR_ISATTACKED) == FALSE ||
      CHAR_getFlg(defindex, CHAR_ISDIE) == FALSE) {
    // BATTLE_CHARMODE_RESCUE 5
    /*
                    print("\n [ %d, %d, %d, %d]",
                            CHAR_getWorkInt( defindex, CHAR_WORKBATTLEMODE ),
                            CHAR_getWorkInt( defindex, CHAR_WORKBATTLEMODE ),
                            CHAR_getFlg( defindex, CHAR_ISATTACKED ),
                            CHAR_getFlg( defindex, CHAR_ISDIE ));
    */
    return FALSE;
  }
  return TRUE;
}

int BATTLE_TargetAdjust(int battleindex, int charaindex, int myside) {
  int defNo;

  defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);

  if (BATTLE_TargetCheck(battleindex, defNo) == FALSE) {
    defNo = BATTLE_DefaultAttacker(battleindex, 1 - myside);
  }

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, defNo);

  return defNo;
}

void Compute_Down(int charaindex, int rideindex, int *down1, int *down2,
                  int flg) {
  int hp = 0, pethp = 0;
  int downs = 0;

  *down1 = 0;
  *down2 = 0;
  //人物
  downs =
      CHAR_getInt(charaindex, CHAR_VITAL) + CHAR_getInt(charaindex, CHAR_STR) +
      CHAR_getInt(charaindex, CHAR_DEX) + CHAR_getInt(charaindex, CHAR_TOUGH);
  if ((downs = (((downs / 100) - 20) / 4)) < 1)
    downs = 1;
  hp = CHAR_getInt(charaindex, CHAR_HP);
  if (hp <= downs)
    downs = hp - 1;
  if (downs >= 0) {
    hp = ((hp - downs) < 1) ? 1 : (hp - downs);
    CHAR_setInt(charaindex, CHAR_HP, hp);
    *down1 = downs;
  }
  if (flg == -1) {
  } else { //骑乘宠物
    if (!CHAR_CHECKINDEX(rideindex))
      return;
    downs =
        CHAR_getInt(rideindex, CHAR_VITAL) + CHAR_getInt(rideindex, CHAR_STR) +
        CHAR_getInt(rideindex, CHAR_DEX) + CHAR_getInt(rideindex, CHAR_TOUGH);
    if ((downs = (((downs / 100) - 20) / 4)) < 1)
      downs = 1;
    pethp = CHAR_getInt(rideindex, CHAR_HP);
    if (pethp <= downs)
      downs = pethp - 1;
    if (downs >= 0) {
      pethp = ((pethp - downs) < 1) ? 1 : (pethp - downs);

      CHAR_setInt(rideindex, CHAR_HP, pethp);
      *down2 = downs;
    }
  }
}

#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
void Compute_Down_SARS(int charaindex, int rideindex, int *down1, int *down2,
                       int flg) {
  int hp = 0, pethp = 0;
  int downs = 0;

  *down1 = 0;
  *down2 = 0;
  //人物
  /*
          downs =
                  CHAR_getInt( charaindex, CHAR_VITAL )
                  + CHAR_getInt( charaindex, CHAR_STR )
                  + CHAR_getInt( charaindex, CHAR_DEX )
                  + CHAR_getInt( charaindex, CHAR_TOUGH );
          if( (downs = (((downs/100)-20)/4)) < 1  )
                  downs = 1;
  */

  hp = CHAR_getInt(charaindex, CHAR_HP);
  downs = hp * 10 / 100;

  if (hp <= downs)
    downs = hp - 1;
  if (downs >= 0) {
    hp = ((hp - downs) < 1) ? 1 : (hp - downs);
    CHAR_setInt(charaindex, CHAR_HP, hp);
    *down1 = downs;
  }
  if (flg == -1) {
  } else { //骑乘宠物
    if (!CHAR_CHECKINDEX(rideindex))
      return;
    downs =
        CHAR_getInt(rideindex, CHAR_VITAL) + CHAR_getInt(rideindex, CHAR_STR) +
        CHAR_getInt(rideindex, CHAR_DEX) + CHAR_getInt(rideindex, CHAR_TOUGH);
    if ((downs = (((downs / 100) - 20) / 4)) < 1)
      downs = 1;
    pethp = CHAR_getInt(rideindex, CHAR_HP);
    if (pethp <= downs)
      downs = pethp - 1;
    if (downs >= 0) {
      pethp = ((pethp - downs) < 1) ? 1 : (pethp - downs);

      CHAR_setInt(rideindex, CHAR_HP, pethp);
      *down2 = downs;
    }
  }
}
#endif

#ifdef _PROFESSION_ADDSKILL
void WorkIceCrackPlay(int charaindex, int cnt, int workicecracknum) {
  char szBuffer[256] = "";
  int i, value = 0;
  int battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
  int defNo = CHAR_getWorkInt(
      charaindex, CHAR_WORKBATTLECOM2); // defNo = BATTLE_Index2No( battleindex,
                                        // charaindex );
  int ridepet = BATTLE_getRidePet(charaindex);
  int bid = BATTLE_Index2No(battleindex, charaindex);
  value = CHAR_getWorkInt(charaindex, workicecracknum);

  if (cnt <= 0 && value > 0) {
    int damage = 0, petdamage = 0;

    int ToList[SIDE_OFFSET * 2 + 1];
    memset(ToList, -1, sizeof(ToList));
    BATTLE_MultiList(battleindex, TARGET_SIDE_1, ToList);
    // 将魔法参数代入动画
    analysis_profession_parameter(2, 4, defNo, charaindex);
    PROFESSION_MAGIC_ATTAIC_Effect(battleindex, bid, ToList, 2);

    if (defNo == 20 || defNo == 25 || defNo == 26) //右
      bid = 0;
    else
      bid = 10;

    for (i = bid; i < bid + 10; i++) {
      int defType, charaidx, petidx;
      charaidx = BATTLE_No2Index(battleindex, i);
      if (CHAR_CHECKINDEX(charaidx)) {
        if (CHAR_getInt(charaidx, CHAR_HP) > 0) {
          int hp, pethp = 0;
          petidx = BATTLE_getRidePet(charaidx);
          defType = CHAR_getInt(charaidx, CHAR_WHICHTYPE);
          CHAR_setWorkInt(charaidx, workicecracknum, 0);

          damage = value;

          hp = CHAR_getInt(charaidx, CHAR_HP);
          if (petidx == -1) {
            hp -= damage;
            if (hp < 0)
              hp = 0;
          } else {
            int defquick = 0, petquick = 0, playerdamage = 0;
            pethp = CHAR_getInt(petidx, CHAR_HP);
            defquick = CHAR_getWorkInt(charaidx, CHAR_WORKDEFENCEPOWER);
            petquick = CHAR_getWorkInt(petidx, CHAR_WORKDEFENCEPOWER);
            if (pethp > 0) {
              playerdamage = (damage * petquick) / (defquick + petquick) + 1;
              petdamage = damage - playerdamage + 1;
              hp -= playerdamage;
              pethp -= petdamage;
            } else {
              hp -= damage;
            }
            if (hp < 0)
              hp = 0;
            if (pethp < 0)
              pethp = 0;
            CHAR_setInt(petidx, CHAR_HP, pethp);
          }

          CHAR_setInt(charaidx, CHAR_HP, hp);

          if (hp <= 0) {
            CHAR_setFlg(charaidx, CHAR_ISDIE, 1);
            CHAR_setWorkInt(charaidx, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
          }
          if (pethp <= 0) {
            CHAR_setFlg(petidx, CHAR_ISDIE, 1);
            CHAR_setWorkInt(ridepet, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
          }

          if (damage >= 0) {
            sprintf(szBuffer, "%X|%X|%X|%X|", i, i, (int)(damage),
                    (int)(petdamage));
            BATTLESTR_ADD(szBuffer);
          }
          if (petidx != -1 && petdamage >= 0) {
          }
        }
      }
    }
    sprintf(szBuffer, "%X|", 0x5711438);
    BATTLESTR_ADD(szBuffer);
  }
}
#endif

static int BATTLE_StatusSeq(int charaindex) {
  int cnt, i, bid, battleindex, down;
  char szBuffer[256] = "";
#ifdef _MAGIC_DEEPPOISON
  int defNo, defindex = 0;
  char szCommand[1024];
#endif
  int ridepet = BATTLE_getRidePet(charaindex);
  int hp = 0, pethp = 0;

  battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);

#ifdef _MAGIC_DEEPPOISON
  defNo = BATTLE_Index2No(battleindex, charaindex);
  defindex = BATTLE_No2Index(battleindex, defNo);
#endif
  bid = BATTLE_Index2No(battleindex, charaindex);
  if (BATTLE_CanMoveCheck(charaindex) == FALSE) {
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
  }
#ifdef _PETSKILL_ADDATTCRAZED
  if (CHAR_getWorkInt(charaindex, CHAR_WORK_ADDATTCRAZED) > 0) {
    int tempaddattcrazed = CHAR_getWorkInt(charaindex, CHAR_WORK_ADDATTCRAZED);
    int temp2 = CHAR_getWorkInt(charaindex, CHAR_WORK_ADDATTCRAZED2);
    CHAR_setWorkInt(charaindex, CHAR_WORK_ADDATTCRAZED, tempaddattcrazed - 1);
    if (tempaddattcrazed - 1 == 0) {
      sprintf(szBuffer, "BD|r%X|1|4|%X|", bid, temp2);
      BATTLESTR_ADD(szBuffer);
    }
  }
#endif
#ifdef _PRO_ABSOLUTE_DEFENSE
  if (GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ABSOLUTE2)) > 0) {
    int habsolute, labsolute;
    habsolute =
        GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ABSOLUTE2)) - 1;
    labsolute = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ABSOLUTE2));
    CHAR_setWorkInt(charaindex, CHAR_WORK_ABSOLUTE2,
                    MAKE2VALUE(habsolute, labsolute));
  }
#endif
#ifdef _PETSKILL_RIDE
  if (GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_RIDE)) > 0) {
    int hworkride, lworkride;
    hworkride = GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_RIDE));
    lworkride = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_RIDE)) - 1;
    CHAR_setWorkInt(charaindex, CHAR_WORK_RIDE,
                    MAKE2VALUE(hworkride, lworkride));
  }
#endif
#ifdef _PRO3_ADDSKILL
  if (GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_CRACKBONE)) >
      0) { //裂骨断筋
    int htemp, ltemp, htemp2;
    htemp = GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_CRACKBONE)) - 1;
    ltemp = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_CRACKBONE));
    if (htemp <= 0) {
      htemp = 0;
      ltemp = 0;
      htemp2 = GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_CRACKBONE2));
      CHAR_setWorkInt(charaindex, CHAR_WORK_CRACKBONE2, MAKE2VALUE(htemp2, 0));
      sprintf(szBuffer, "BD|r%X|0|A|%X|", bid, 11);
      BATTLESTR_ADD(szBuffer);
    }
    CHAR_setWorkInt(charaindex, CHAR_WORK_CRACKBONE, MAKE2VALUE(htemp, ltemp));
  }
  if (GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_CRACKBONE2)) >
      0) { //裂骨断筋
    int htemp, ltemp;
    htemp = GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_CRACKBONE2)) - 1;
    ltemp = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_CRACKBONE2));
    CHAR_setWorkInt(charaindex, CHAR_WORK_CRACKBONE2, MAKE2VALUE(htemp, ltemp));
  }
  if (GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_POISON_DRAGNET)) >
      0) { //毒素之网
    int htemp, ltemp;
    int old_hp = CHAR_getInt(charaindex, CHAR_HP);
    hp = old_hp;
    htemp =
        GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_POISON_DRAGNET)) - 1;
    ltemp = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_POISON_DRAGNET));
    CHAR_setWorkInt(charaindex, CHAR_WORK_POISON_DRAGNET,
                    MAKE2VALUE(htemp, ltemp));
    if (htemp >= 0) {
      int dpoison;
      if (ridepet == -1) {
        dpoison = hp * (ltemp * 0.01);
        if (dpoison < 1)
          dpoison = 0;
        hp -= dpoison;
        if (hp <= 0) {
          sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, old_hp - 1);
          BATTLESTR_ADD(szBuffer);
          hp = 1;
        } else {
          sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, dpoison);
          BATTLESTR_ADD(szBuffer);
        }
        CHAR_setInt(charaindex, CHAR_HP, hp);
      } else {
        int old_php;
        int pet_dec_hp;
        int dec_hp = hp * (ltemp / 100.0);
        pethp = CHAR_getInt(ridepet, CHAR_HP);
        old_php = pethp;
        pet_dec_hp = pethp * (ltemp / 100.0);
        if (dec_hp < 1)
          dec_hp = 1;
        if (pet_dec_hp < 1)
          pet_dec_hp = 1;
        hp -= dec_hp;
        pethp -= pet_dec_hp;
        if (hp <= 0) {
          dec_hp = old_hp - 1;
          hp = 1;
        }
        if (pethp <= 0) {
          pet_dec_hp = old_php - 1;
          pethp = 1;
        }
        CHAR_setInt(charaindex, CHAR_HP, hp);
        CHAR_setInt(ridepet, CHAR_HP, pethp);
        sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, dec_hp, pet_dec_hp);
        BATTLESTR_ADD(szBuffer);
      }
    }
    if (htemp == 0) {
      sprintf(szBuffer, "BD|r%X|0|A|%X|", bid, 13);
      BATTLESTR_ADD(szBuffer);
      BATTLE_BadStatusString(bid, 0);
    }
  }
  if (GETTHIRDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_PACIFY)) >
      0) { //抚慰心灵 回合
    int h1, h2, l1, l2, ptemp;
    ptemp = CHAR_getWorkInt(charaindex, CHAR_WORK_PACIFY);
    h1 = GETFOURTHVALUE(ptemp);
    h2 = GETTHIRDVALUE(ptemp) - 1;
    l1 = GETSECONDVALUE(ptemp);
    l2 = GETFIRSTVALUE(ptemp);
    if (h2 <= 0) {
      CHAR_setWorkInt(charaindex, CHAR_WORK_PACIFY, MAKE4VALUE(h1, 0, 0, l2));
      sprintf(szBuffer, "BD|r%X|0|A|%X|", bid, 14);
      BATTLESTR_ADD(szBuffer);
      BATTLE_BadStatusString(bid, 0);
    } else
      CHAR_setWorkInt(charaindex, CHAR_WORK_PACIFY, MAKE4VALUE(h1, h2, l1, l2));
  }
  if (GETFIRSTVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_PACIFY)) >
      0) { //抚慰心灵 冷却
    int h1, h2, l1, l2, ptemp;
    ptemp = CHAR_getWorkInt(charaindex, CHAR_WORK_PACIFY);
    h1 = GETFOURTHVALUE(ptemp);
    h2 = GETTHIRDVALUE(ptemp);
    l1 = GETSECONDVALUE(ptemp);
    l2 = GETFIRSTVALUE(ptemp) - 1;
    CHAR_setWorkInt(charaindex, CHAR_WORK_PACIFY, MAKE4VALUE(h1, h2, l1, l2));
  }
  if (GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DARK_SEAL)) >
      0) { //?灵封印 回合
    int htemp, ltemp;
    htemp = GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DARK_SEAL)) - 1;
    ltemp = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DARK_SEAL));
    htemp = ((htemp >= 0) ? htemp : 0);
    if (htemp == 0) {
      sprintf(szBuffer, "BD|r%X|0|A|%X|", bid, 16);
      BATTLESTR_ADD(szBuffer);
    }
    CHAR_setWorkInt(charaindex, CHAR_WORK_DARK_SEAL, MAKE2VALUE(htemp, ltemp));
  }
  if (GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DARK_SEAL)) >
      0) { //?灵封印 冷却
    int htemp, ltemp;
    htemp = GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DARK_SEAL));
    ltemp = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DARK_SEAL)) - 1;
    ltemp = ltemp >= 0 ? ltemp : 0;
    CHAR_setWorkInt(charaindex, CHAR_WORK_DARK_SEAL, MAKE2VALUE(htemp, ltemp));
  }
  if (GETFOURTHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH)) >
      0) { //恶魔诅咒 回合
    int h1, h2, l1, l2, dtemp;
    dtemp = CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH);
    h1 =
        GETFOURTHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH)) - 1;
    h2 = GETTHIRDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH));
    l1 = GETSECONDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH));
    l2 = GETFIRSTVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH));
    h1 = h1 >= 0 ? h1 : 0;
    if (h1 == 0) {
      sprintf(szBuffer, "BD|r%X|0|A|%X|", bid, 18);
      BATTLESTR_ADD(szBuffer);
    } else if (CHAR_getWorkInt(charaindex, CHAR_WORK_ABSOLUTE) <= 0) {
      //减 MP
      int mp;
      int old_hp = CHAR_getInt(charaindex, CHAR_HP);
      int old_mp = CHAR_getInt(charaindex, CHAR_MP);
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
          old_mp > 0) {
        mp = old_mp - l2;
        if (mp > 0) {
          sprintf(szBuffer, "BD|r%X|1|0|m%X|p%X|", bid, l2, 0);
        } else {
          sprintf(szBuffer, "BD|r%X|1|0|m%X|p%X|", bid, old_mp, 0);
          mp = 0;
        }
        BATTLESTR_ADD(szBuffer);
        CHAR_setInt(charaindex, CHAR_MP, mp);
      }
      //减 HP
      if (ridepet == -1) {
        hp = old_hp - l1;
        if (hp <= 0) {
          sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, old_hp - 1);
          hp = 1;
        } else {
          sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, l1);
        }
        CHAR_setInt(charaindex, CHAR_HP, hp);
      } else {
        int pet_hp = CHAR_getInt(ridepet, CHAR_HP) - l1;
        int old_php = CHAR_getInt(ridepet, CHAR_HP);
        int delhp, delphp;
        hp = old_hp - l1;
        delhp = l1;
        if (hp <= 0) {
          delhp = old_hp - 1;
          hp = 1;
        }
        delphp = l1;
        if (pet_hp <= 0) {
          delphp = old_php - 1;
          pet_hp = 1;
        }
        CHAR_setInt(charaindex, CHAR_HP, hp);
        CHAR_setInt(ridepet, CHAR_HP, pet_hp);
        sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, delhp, delphp);
      }
      BATTLESTR_ADD(szBuffer);
    }
    CHAR_setWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH,
                    MAKE4VALUE(h1, h2, l1, l2));
  }
  if (GETTHIRDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH)) >
      0) { //恶魔诅咒 冷却
    int h1, h2, l1, l2, dtemp;
    dtemp = CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH);
    h1 = GETFOURTHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH));
    h2 = GETTHIRDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH)) - 1;
    l1 = GETSECONDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH));
    l2 = GETFIRSTVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH));
    CHAR_setWorkInt(charaindex, CHAR_WORK_DEMON_SPEECH,
                    MAKE4VALUE(h1, h2, l1, l2));
  }
  if (CHAR_getWorkInt(charaindex, CHAR_WORK_FIRST_AID) > 0) { //战场急救 冷却
    CHAR_setWorkInt(charaindex, CHAR_WORK_FIRST_AID,
                    CHAR_getWorkInt(charaindex, CHAR_WORK_FIRST_AID) - 1);
  }
  if (GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ROAR)) >
      0) { //战狼怒吼 冷却
    int htemp, ltemp;
    htemp = GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ROAR)) - 1;
    ltemp = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ROAR));
    htemp = ((htemp >= 0) ? htemp : 0);
    CHAR_setWorkInt(charaindex, CHAR_WORK_ROAR, MAKE2VALUE(htemp, ltemp));
  }
  if (GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ROAR)) >
      0) { //战狼怒吼 惊吓
    int htemp, ltemp;
    htemp = GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ROAR));
    ltemp = GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ROAR)) - 1;
    ltemp = ((ltemp >= 0) ? ltemp : 0);
    CHAR_setWorkInt(charaindex, CHAR_WORK_ROAR, MAKE2VALUE(htemp, ltemp));
  }
  if (CHAR_getWorkInt(charaindex, CHAR_WORK_MEDICATION) > 0) { //制药学
    CHAR_setWorkInt(charaindex, CHAR_WORK_MEDICATION,
                    CHAR_getWorkInt(charaindex, CHAR_WORK_MEDICATION) - 1);
  }
  CHAR_sendStatusString(charaindex, "g");
#endif

  for (i = 1; i < BATTLE_ST_END; i++) {
    if ((cnt = CHAR_getWorkInt(charaindex, StatusTbl[i])) <= 0)
      continue;
    CHAR_setWorkInt(charaindex, StatusTbl[i], --cnt);
#ifdef _MAGIC_WEAKEN //   虚弱
    if (CHAR_getWorkInt(charaindex, CHAR_WORKWEAKEN) > 0)
      CHAR_setWorkInt(charaindex, StatusTbl[i], cnt + 1);
#endif
#ifdef _MAGIC_BARRIER // vincent  精灵:魔障
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBARRIER) > 0)
      CHAR_setWorkInt(charaindex, StatusTbl[i], cnt + 1);
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    if (cnt <= 0 && (StatusTbl[i] != CHAR_WORKICECRACK)
#ifdef _PROFESSION_ADDSKILL
        && (StatusTbl[i] != CHAR_WORKICECRACK2) &&
        (StatusTbl[i] != CHAR_WORKICECRACK3) &&
        (StatusTbl[i] != CHAR_WORKICECRACK4) &&
        (StatusTbl[i] != CHAR_WORKICECRACK5) &&
        (StatusTbl[i] != CHAR_WORKICECRACK6) &&
        (StatusTbl[i] != CHAR_WORKICECRACK7) &&
        (StatusTbl[i] != CHAR_WORKICECRACK8) &&
        (StatusTbl[i] != CHAR_WORKICECRACK9) &&
        (StatusTbl[i] != CHAR_WORKICECRACK10)
#endif
    ) { // 冰爆术
#else
    if (cnt <= 0) {
#endif
      BATTLE_BadStatusString(bid, 0);
      // snprintf( szBuffer, sizeof( szBuffer ), "(%s)将(%s)修理完毕。",
      //	CHAR_getUseName( charaindex ), aszStatusFull[i] );
      //	BATTLE_BroadCast( battleindex, szBuffer,
      //		(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
// Terry del
//				sprintf( szBuffer, "BM|%X|%X|", bid, 0 );
//				BATTLESTR_ADD( szBuffer );
#ifdef _MAGIC_NOCAST
      if (StatusTbl[i] == CHAR_WORKNOCAST) {
        lssproto_NC_send(getfdFromCharaIndex(charaindex), 0); //复原
      }
#endif
      if (StatusTbl[i] == CHAR_WORKDRUNK) {
        if (ridepet != -1) {
          CHAR_setWorkInt(charaindex, CHAR_WORKQUICK,
                          CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) +
                              CHAR_getWorkInt(ridepet, CHAR_WORKQUICK));
        } else {
          CHAR_setWorkInt(charaindex, CHAR_WORKQUICK,
                          CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) * 2);
        }
      }
      continue;
    }

    switch (StatusTbl[i]) {

    case CHAR_WORKPOISON: {
      int petdown = -1;
      Compute_Down(charaindex, ridepet, &down, &petdown, ridepet);
      if (down >= 0) {
        // snprintf( szBuffer, sizeof( szBuffer ),
        //	"(%s)因(%s)受到(%d)的损伤。",
        //	CHAR_getUseName( charaindex ),
        //	aszStatusFull[i],
        //	down );
        // BATTLE_BroadCast( battleindex, szBuffer,
        //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

#ifdef _NOTRIDE_
        sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, (int)(down));
        BATTLESTR_ADD(szBuffer);
#else
        sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, (int)(down),
                (int)(petdown));
        BATTLESTR_ADD(szBuffer);
#endif
      }
      if (ridepet != -1 && petdown >= 0) {
        // snprintf( szBuffer, sizeof( szBuffer ),
        //	"(%s的骑宠)因(%s)受到(%d)的损伤。",
        //	CHAR_getUseName( charaindex ),
        //	aszStatusFull[i], petdown );
        // BATTLE_BroadCast( battleindex, szBuffer,
        //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
      }
    } break;
#ifdef _MAGIC_DEEPPOISON
    case CHAR_WORKDEEPPOISON:
      if (CHAR_getInt(defindex, CHAR_HP) <= 1) {
        snprintf(szCommand, sizeof(szCommand), "Bd|a%X|r%X|f%X|d%X|p%X|FF|",
                 defNo, defNo, BCF_NORMAL, 1, 0);
        BATTLESTR_ADD(szCommand);
        CHAR_setInt(defindex, CHAR_HP, 0);
        CHAR_setWorkInt(defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        CHAR_setFlg(defindex, CHAR_ISDIE, 1);
        break;
      }

      if (CHAR_getWorkInt(defindex, CHAR_WORKDEEPPOISON) <= 1) {
        snprintf(szCommand, sizeof(szCommand), "Bd|a%X|r%X|f%X|d%X|p%X|FF|",
                 defNo, defNo, BCF_NORMAL, CHAR_getInt(charaindex, CHAR_HP), 0);
        BATTLESTR_ADD(szCommand);
        CHAR_setInt(defindex, CHAR_HP, 0);
        CHAR_setFlg(defindex, CHAR_ISDIE, 1);
        CHAR_setWorkInt(defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        sprintf(szBuffer, "%s身中剧毒未解而阵亡了!!",
                CHAR_getChar(defindex, CHAR_NAME));
        if (CHAR_getInt(defindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
          CHAR_talkToCli(CHAR_getWorkInt(defindex, CHAR_WORKPLAYERINDEX), -1,
                         szBuffer, CHAR_COLORYELLOW);
        } else {
          CHAR_talkToCli(defindex, -1, szBuffer, CHAR_COLORYELLOW);
        }
        break;
      }
      if (ridepet == -1) { // Robin 0728 ride Pet
        down = CHAR_getInt(charaindex, CHAR_VITAL) +
               CHAR_getInt(charaindex, CHAR_STR) +
               CHAR_getInt(charaindex, CHAR_DEX) +
               CHAR_getInt(charaindex, CHAR_TOUGH);
        down = (down / 100 - 20) / 4;
        if (down < 1)
          down = 1;
        hp = CHAR_getInt(charaindex, CHAR_HP);
        if (hp <= down)
          down = hp - 1;
        if (down >= 0) {
          CHAR_setInt(charaindex, CHAR_HP, hp - down);
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( charaindex ),
          //	aszStatusFull[i],
          //	down );
          // BATTLE_BroadCast( battleindex, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
#ifdef _NOTRIDE_
          sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, (int)(down));
          BATTLESTR_ADD(szBuffer);
#else
          sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, (int)(down), 0);
          BATTLESTR_ADD(szBuffer);
#endif
        }
        break;
      } else {
        int petdown;
        down = CHAR_getInt(charaindex, CHAR_VITAL) +
               CHAR_getInt(charaindex, CHAR_STR) +
               CHAR_getInt(charaindex, CHAR_DEX) +
               CHAR_getInt(charaindex, CHAR_TOUGH);
        petdown =
            CHAR_getInt(ridepet, CHAR_VITAL) + CHAR_getInt(ridepet, CHAR_STR) +
            CHAR_getInt(ridepet, CHAR_DEX) + CHAR_getInt(ridepet, CHAR_TOUGH);
        down = (down / 100 - 20) / 4;
        petdown = (petdown / 100 - 20) / 4;
        if (down < 1)
          down = 1;
        if (petdown < 1)
          petdown = 1;
        hp = CHAR_getInt(charaindex, CHAR_HP);
        pethp = CHAR_getInt(ridepet, CHAR_HP);
        if (hp <= down)
          down = hp - 1;
        if (pethp <= petdown)
          petdown = pethp - 1;
        if (down >= 0) {
          CHAR_setInt(charaindex, CHAR_HP, hp - down);
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( charaindex ),
          //	aszStatusFull[i],
          //	down );
          // BATTLE_BroadCast( battleindex, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
#ifdef _NOTRIDE_
          sprintf(szBuffer, "BD|r%X|0|0|%X|", bid, (int)(down));
          BATTLESTR_ADD(szBuffer);
#else
          sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, (int)(down),
                  (int)(petdown));
          BATTLESTR_ADD(szBuffer);
#endif
        }
        if (petdown >= 0) {
          CHAR_setInt(ridepet, CHAR_HP, pethp - petdown);
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s的骑宠)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( charaindex ),
          //	aszStatusFull[i],
          //	petdown );
          // BATTLE_BroadCast( battleindex, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
        }
        break;
      }
#endif
#ifdef _MAGIC_NOCAST // 精灵:沉默
    case CHAR_WORKNOCAST:
      lssproto_NC_send(getfdFromCharaIndex(charaindex), 1); //沉默
      break;
#endif
    case CHAR_WORKCONFUSION: {
      int side, pos, defNo, lop;
      if (RAND(1, 100) > 80) {
        break;
      }
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      side = RAND(0, 1);
      pos = RAND(0, 9);
      for (lop = 0; lop < SIDE_OFFSET; lop++) {
        if (++pos >= SIDE_OFFSET)
          pos = 0;
        defNo = side * SIDE_OFFSET + pos;
        if (defNo == bid)
          continue;
        if (BATTLE_TargetCheck(battleindex, defNo) == TRUE) {
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, defNo);
          break;
        }
      }
      if (lop >= SIDE_OFFSET) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, -1);
      }
    } break;
#ifdef _PROFESSION_ADDSKILL
    case CHAR_WORKANNEX: //附身术
    {
      int side, pos, defNo, lop;

      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      side = RAND(0, 1);
      pos = RAND(0, 9);
      for (lop = 0; lop < SIDE_OFFSET; lop++) {
        if (++pos >= SIDE_OFFSET)
          pos = 0;
        defNo = side * SIDE_OFFSET + pos;
        if (defNo == bid)
          continue;
        if (BATTLE_TargetCheck(battleindex, defNo) == TRUE) {
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, defNo);
          break;
        }
      }
      if (lop >= SIDE_OFFSET) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, -1);
      }

    } break;
#endif
    case CHAR_WORKDRUNK:
      break;

#ifdef _PROFESSION_SKILL    // WON ADD 人物职业技能
    case CHAR_WORKOBLIVION: // 遗忘
    {
      // 恢复技能
      if (cnt <= 1) {
        int defNo = BATTLE_Index2No(battleindex, charaindex);
        int toNo = defNo - 5;
        int toNoindex = -1, pet_no = -1;
        char msg[20];

        memset(msg, -1, sizeof(msg));

        // 主人index
        toNoindex = BATTLE_No2Index(battleindex, toNo);
        CHAR_setWorkInt(charaindex, StatusTbl[BATTLE_ST_OBLIVION], 0);

        // 宠物编号
        pet_no = CHAR_getInt(toNoindex, CHAR_DEFAULTPET);
        sprintf(msg, "W%d", pet_no);

        CHAR_sendStatusString(toNoindex, msg);

        BATTLE_BadStatusString(bid, 0);
        sprintf(szBuffer, "BM|%X|%X|", bid, 0);
        BATTLESTR_ADD(szBuffer);
      }
      break;
    }
    case CHAR_WORK_F_ENCLOSE: // 火附体
    {
      int ToList[SIDE_OFFSET * 2 + 1];
      int defNo = BATTLE_Index2No(battleindex, charaindex);
      int old_hp = CHAR_getInt(charaindex, CHAR_HP);
      int dec_hp = 0, hp = 0;
      int DAMAGE = 50;

      memset(ToList, -1, sizeof(ToList));
      BATTLE_MultiList(battleindex, defNo, ToList);
      BATTLE_MagicEffect(battleindex, defNo, ToList, 101699, 101699);

      // 取魔法伤害值
      DAMAGE = DAMAGE * cnt;
#ifdef _PROFESSION_ADDSKILL
      dec_hp =
          PROFESSION_MAGIC_GET_DAMAGE(charaindex, charaindex, 1, DAMAGE, 0);
#else
      dec_hp = PROFESSION_MAGIC_GET_DAMAGE(charaindex, charaindex, 1, DAMAGE);
#endif
      hp = old_hp - dec_hp;

      if (hp <= 0) {
        CHAR_setInt(charaindex, CHAR_HP, 0);
        CHAR_setFlg(charaindex, CHAR_ISDIE, 1);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      } else {
        CHAR_setInt(charaindex, CHAR_HP, hp);
      }

      sprintf(szCommand, "BD|r%X|0|0|%X|", defNo, dec_hp);
      BATTLESTR_ADD(szCommand);

      break;
    }
    case CHAR_WORK_I_ENCLOSE: // 冰附体
    {
      int old_dex = CHAR_getInt(charaindex, CHAR_DEX);
      int dex = 0;
      int ToList[SIDE_OFFSET * 2 + 1];
      int defNo = BATTLE_Index2No(battleindex, charaindex);

      memset(ToList, -1, sizeof(ToList));
      BATTLE_MultiList(battleindex, defNo, ToList);
      BATTLE_MagicEffect(battleindex, defNo, ToList, 27692, 101700);

      dex = old_dex * 0.9;
      CHAR_setWorkInt(charaindex, CHAR_WORKFIXDEX, dex);
      sprintf(szCommand, "BD|r%X|0|6|%X|", defNo,
              (-1) * (int)((old_dex * 0.1) / 100));
      BATTLESTR_ADD(szCommand);

      break;
    }
    case CHAR_WORKRESIST_F: // 火抗性提升
    {
      // 恢复抗性
      if (cnt <= 1) {
        int add_value = 0;
        int old_value = 0;
        old_value = CHAR_getWorkInt(charaindex, CHAR_WORK_F_RESIST);
        add_value = CHAR_getWorkInt(charaindex, CHAR_WORKMODRESIST_F);

        CHAR_setWorkInt(charaindex, CHAR_WORK_F_RESIST, old_value - add_value);
        CHAR_talkToCli(charaindex, -1, "火抗性恢复", CHAR_COLORYELLOW);
      }
      break;
    }
    case CHAR_WORKRESIST_I: // 冰抗性提升
    {
      // 恢复抗性
      if (cnt <= 1) {
        int add_value = 0;
        int old_value = 0;
        old_value = CHAR_getWorkInt(charaindex, CHAR_WORK_I_RESIST);
        add_value = CHAR_getWorkInt(charaindex, CHAR_WORKMODRESIST_I);

        CHAR_setWorkInt(charaindex, CHAR_WORK_I_RESIST, old_value - add_value);
        CHAR_talkToCli(charaindex, -1, "冰抗性恢复", CHAR_COLORYELLOW);
      }

      break;
    }
    case CHAR_WORKRESIST_T: // 雷抗性提升
    {
      // 恢复抗性
      if (cnt <= 1) {
        int add_value = 0;
        int old_value = 0;
        old_value = CHAR_getWorkInt(charaindex, CHAR_WORK_T_RESIST);
        add_value = CHAR_getWorkInt(charaindex, CHAR_WORKMODRESIST_T);

        CHAR_setWorkInt(charaindex, CHAR_WORK_T_RESIST, old_value - add_value);
        CHAR_talkToCli(charaindex, -1, "雷抗性恢复", CHAR_COLORYELLOW);
      }

      break;
    }
#ifdef _PROFESSION_ADDSKILL
      /*            case CHAR_WORKERSIST_F_I_T:     // 提升火冰雷抗性
                                      {
                                              // 恢复抗性
                                              if( cnt <= 1 ){
                                                      int add_value = 0;
                                                      int old_value = 0;
                                                      old_value =
         CHAR_getWorkInt( charaindex, CHAR_WORK_F_RESIST ); add_value =
         CHAR_getWorkInt( charaindex, CHAR_WORKMODRESIST_F ); CHAR_setWorkInt(
         charaindex, CHAR_WORK_F_RESIST, old_value - add_value );
                                                      CHAR_talkToCli(
         charaindex, -1 , "火抗性恢复" , CHAR_COLORYELLOW );

                                                      old_value =
         CHAR_getWorkInt( charaindex, CHAR_WORK_I_RESIST ); add_value =
         CHAR_getWorkInt( charaindex, CHAR_WORKMODRESIST_I ); CHAR_setWorkInt(
         charaindex, CHAR_WORK_I_RESIST, old_value - add_value );
                                                      CHAR_talkToCli(
         charaindex, -1 , "冰抗性恢复" , CHAR_COLORYELLOW );

                                                      old_value =
         CHAR_getWorkInt( charaindex, CHAR_WORK_T_RESIST ); add_value =
         CHAR_getWorkInt( charaindex, CHAR_WORKMODRESIST_T ); CHAR_setWorkInt(
         charaindex, CHAR_WORK_T_RESIST, old_value - add_value );
                                                      CHAR_talkToCli(
         charaindex, -1 , "雷抗性恢复" , CHAR_COLORYELLOW );
                                              }

                                              break;
                                      }*/
    case CHAR_WORKFEAR:
      if (cnt <= 0)
        CHAR_talkToCli(charaindex, -1, "攻、防、敏恢复", CHAR_COLORYELLOW);
      break;
#endif
    case CHAR_WORKINSTIGATE: // 挑拨
    {
      int side, pos, defNo, lop, rate = 0;
      if (RAND(1, 100) > 80) {
        break;
      }
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      side = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLESIDE);

      rate = CHAR_getWorkInt(charaindex, CHAR_WORKMODINSTIGATE);

      CHAR_setWorkInt(
          charaindex, CHAR_WORKFIXSTR,
          (CHAR_getWorkInt(charaindex, CHAR_WORKFIXSTR) * (100 - rate) / 100));
      CHAR_setWorkInt(charaindex, CHAR_WORKFIXTOUGH,
                      (CHAR_getWorkInt(charaindex, CHAR_WORKFIXTOUGH) *
                       (100 - rate) / 100));
      CHAR_setWorkInt(
          charaindex, CHAR_WORKFIXDEX,
          (CHAR_getWorkInt(charaindex, CHAR_WORKFIXDEX) * (100 - rate) / 100));

      pos = RAND(0, 9);
      for (lop = 0; lop < SIDE_OFFSET; lop++) {
        if (++pos >= SIDE_OFFSET)
          pos = 0;
        defNo = side * SIDE_OFFSET + pos;
        if (defNo == bid)
          continue;
        if (BATTLE_TargetCheck(battleindex, defNo) == TRUE) {
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, defNo);
          break;
        }
      }
      if (lop >= SIDE_OFFSET) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, -1);
      }
      break;
    }
      /*
                              case CHAR_WORK_FOCUS:							//
         专注战斗
                                      {
                                              if( cnt <= 1 ){
                                                      int atk_to_index =
         CHAR_getWorkInt( charaindex, CHAR_WORKMOD_FOCUS ); int atk_to_no =
         BATTLE_Index2No( battleindex, atk_to_index );

                                                      if( BATTLE_TargetCheck(
         battleindex, atk_to_no ) != TRUE )	break;

                                                      CHAR_setWorkInt(
         charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK ); CHAR_setWorkInt(
         charaindex, CHAR_WORKBATTLECOM2, atk_to_no ); CHAR_setWorkInt(
         charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
                                                      CHAR_SETWORKINT_LOW(
         charaindex, CHAR_WORKBATTLECOM3, 1 );
                                              }
                                              break;
                                      }
      */
    case CHAR_WORKSIGN: // 一针见血
    {
      // 吸血
      int skill_level = CHAR_getWorkInt(charaindex, CHAR_WORKMODSIGN);
      int old_hp = CHAR_getInt(charaindex, CHAR_HP);
      int old_mp = CHAR_getInt(charaindex, CHAR_MP);
      int to_index = CHAR_getWorkInt(charaindex, CHAR_WORKSIGNID);
      int to_old_hp = CHAR_getInt(to_index, CHAR_HP);
      int to_old_mp = CHAR_getInt(to_index, CHAR_MP);
      int dec_hp = 0, dec_mp = 0, rate = 0, hp = 0, mp = 0, add_hp = 0,
          add_mp = 0;

      if (old_hp <= 0)
        break;

#ifdef _PROFESSION_ADDSKILL
      if (skill_level >= 10) {
        dec_hp = 200;
        dec_mp = 30;
      } else if (skill_level > 6) {
        dec_hp = 150;
        dec_mp = 20;
      } else if (skill_level > 3) {
        dec_hp = 100;
        dec_mp = 15;
      } else {
        dec_hp = 50;
        dec_mp = 10;
      }
      // 增加hp
      if (skill_level > 8)
        rate = 100;
      else if (skill_level > 7)
        rate = 50;
      else
        rate = 0;
      add_hp = dec_hp * rate / 100;

      // 增加mp
      if (skill_level > 8)
        rate = 100;
      else
        rate = 0;
      add_mp = dec_mp * rate / 100;

#else
      if (skill_level >= 10) {
        dec_hp = 300;
        dec_mp = 70;
      } else if (skill_level >= 9) {
        dec_hp = 200;
        dec_mp = 50;
      } else if (skill_level >= 7) {
        dec_hp = 150;
        dec_mp = 30;
      } else if (skill_level >= 4) {
        dec_hp = 100;
        dec_mp = 20;
      } else {
        dec_hp = 50;
        dec_mp = 10;
      }
      // 增加hp
      if (skill_level >= 9)
        rate = 80;
      else if (skill_level >= 7)
        rate = (skill_level - 1) * 10;
      else if (skill_level >= 4)
        rate = (skill_level - 2) * 10;
      else
        rate = skill_level * 10;
      add_hp = dec_hp * rate / 100;

      // 增加mp
      if (skill_level >= 9)
        rate = 100;
      else if (skill_level >= 7)
        rate = (skill_level - 1) * 10;
      else if (skill_level >= 4)
        rate = (skill_level - 2) * 10;
      else
        rate = skill_level * 10;
      add_mp = dec_mp * rate / 100;
#endif
#ifdef _PRO_ABSOLUTE_DEFENSE
      if (Absolute_Defense_Check(to_index) == TRUE) {
        add_hp = 0;
        dec_hp = 0;
      }
#endif
      // 中嗜血蛊的人减 HP
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        mp = old_mp - dec_mp;
        if (mp < 0) {
          mp = 0;
          add_mp = 0;
        }
        CHAR_setInt(charaindex, CHAR_MP, mp);
      }

      // 中嗜血蛊的人减 HP
      if (ridepet == -1) {
        hp = old_hp - dec_hp;
        if (hp < 0)
          hp = 0;
        CHAR_setInt(charaindex, CHAR_HP, hp);
        sprintf(szCommand, "BD|r%X|0|0|%X|m%X|", bid, dec_hp, dec_mp);
      } else {
        int pet_old_hp = CHAR_getInt(ridepet, CHAR_HP);
        int pet_hp = 0;

        dec_hp /= 2;
        hp = old_hp - dec_hp;
        pet_hp = pet_old_hp - dec_hp;

        if (hp < 0)
          hp = 0;
        if (pet_hp < 0)
          pet_hp = 0;

        CHAR_setInt(charaindex, CHAR_HP, hp);
        CHAR_setInt(ridepet, CHAR_HP, pet_hp);
        if (pet_hp <= 0) {
          CHAR_setFlg(ridepet, CHAR_ISDIE, 1);
          CHAR_setWorkInt(ridepet, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        }
        sprintf(szCommand, "BD|r%X|0|0|%X|m%X|p%X|", bid, dec_hp, dec_mp,
                dec_hp);
      }

      if (hp <= 0) {
        CHAR_setFlg(charaindex, CHAR_ISDIE, 1);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      }

      BATTLESTR_ADD(szCommand);

      // 使用嗜血蛊的人加 HP、MP
      if ((add_hp > 0) && (to_old_hp > 0)) {
        int to_bid = 0, to_hp = 0, to_mp = 0;

        to_bid = BATTLE_Index2No(battleindex, to_index);
        sprintf(szCommand, "BD|r%X|0|1|%X|m%X|", to_bid, add_hp, add_mp);
        BATTLESTR_ADD(szCommand);

        to_hp = to_old_hp + add_hp;
        if (to_hp > CHAR_getWorkInt(to_index, CHAR_WORKMAXHP))
          to_hp = CHAR_getWorkInt(to_index, CHAR_WORKMAXHP);

        to_mp = to_old_mp + add_mp;
        if (to_mp > CHAR_getWorkInt(to_index, CHAR_WORKMAXMP))
          to_mp = CHAR_getWorkInt(to_index, CHAR_WORKMAXMP);

        CHAR_setInt(to_index, CHAR_HP, to_hp);
        CHAR_setInt(to_index, CHAR_MP, to_mp);
      }

      break;
    }
    case CHAR_WORKBLOODWORMS: // 嗜血蛊
    {
      // 吸血
      int skill_level = CHAR_getWorkInt(charaindex, CHAR_WORKMODBLOODWORMS);
      int old_hp = CHAR_getInt(charaindex, CHAR_HP);
      int to_index = CHAR_getWorkInt(charaindex, CHAR_WORKBLOODWORMSID);
      int to_old_hp = CHAR_getInt(to_index, CHAR_HP);
      int dec_hp = 0, rate = 0, hp = 0, add_hp = 0;

      skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

      if (old_hp <= 0)
        break;

      dec_hp = skill_level * 10 + 30;
      if (skill_level >= 10)
        rate = 20;
      else if (skill_level >= 7)
        rate = 15;
      else if (skill_level >= 5)
        rate = 10;
      else
        rate = 5;

      add_hp = dec_hp * rate / 100;
#ifdef _PRO_ABSOLUTE_DEFENSE
      if (Absolute_Defense_Check(to_index) == TRUE) {
        add_hp = 0;
        dec_hp = 0;
      }
#endif
      // 中嗜血蛊的人减 HP
      if (ridepet == -1) {
        hp = old_hp - dec_hp;
        if (hp < 0)
          hp = 0;

        CHAR_setInt(charaindex, CHAR_HP, hp);
        sprintf(szCommand, "BD|r%X|0|0|%X|", bid, dec_hp);
      } else {
        int pet_old_hp = CHAR_getInt(ridepet, CHAR_HP);
        int pet_hp = 0;

        dec_hp /= 2;
        hp = old_hp - dec_hp;
        pet_hp = pet_old_hp - dec_hp;

        if (hp < 0)
          hp = 0;
        if (pet_hp < 0)
          pet_hp = 0;

        CHAR_setInt(charaindex, CHAR_HP, hp);
        CHAR_setInt(ridepet, CHAR_HP, pet_hp);
        if (pet_hp == 0) {
          CHAR_setFlg(ridepet, CHAR_ISDIE, 1);
          CHAR_setWorkInt(ridepet, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        }
        sprintf(szCommand, "BD|r%X|0|0|%X|p%X|", bid, dec_hp, dec_hp);
      }

      if (hp <= 0) {
        CHAR_setFlg(charaindex, CHAR_ISDIE, 1);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      }

      BATTLESTR_ADD(szCommand);

      // 使用嗜血蛊的人加 HP
      if ((add_hp > 0) && (to_old_hp > 0)) {
        int to_bid = 0, to_hp = 0;

        to_bid = BATTLE_Index2No(battleindex, to_index);
        sprintf(szCommand, "BD|r%X|0|1|%X|", to_bid, add_hp);
        BATTLESTR_ADD(szCommand);

        to_hp = to_old_hp + add_hp;
        if (to_hp > CHAR_getWorkInt(to_index, CHAR_WORKMAXHP))
          to_hp = CHAR_getWorkInt(to_index, CHAR_WORKMAXHP);

        CHAR_setInt(to_index, CHAR_HP, to_hp);
      }

      break;
    }
    case CHAR_WORKICEARROW: // 冰箭
    {
      // 减敏
      int dec_dex = CHAR_getWorkInt(charaindex, CHAR_WORKMODICEARROW);
      int old_dex = CHAR_getWorkInt(charaindex, CHAR_WORKFIXDEX);
      int dex = 0;

      dex = old_dex * (100 - dec_dex) / 100;
      CHAR_setWorkInt(charaindex, CHAR_WORKFIXDEX, dex);
      sprintf(szCommand, "BD|r%X|0|6|%X|", bid, (-1) * dec_dex);

      BATTLESTR_ADD(szCommand);

      break;
    }
    case CHAR_WORKICECRACK: // 冰爆术
    {
      int value = 0;
      int defNo = CHAR_getWorkInt(
          charaindex, CHAR_WORKBATTLECOM2); // defNo = BATTLE_Index2No(
                                            // battleindex, charaindex );

      value = CHAR_getWorkInt(charaindex, CHAR_WORKMODICECRACK);

      if (cnt <= 0 && value > 0) {
        int damage = 0, petdamage = 0;

        int ToList[SIDE_OFFSET * 2 + 1];
        memset(ToList, -1, sizeof(ToList));
        BATTLE_MultiList(battleindex, TARGET_SIDE_1, ToList);
        // 将魔法参数代入动画
        analysis_profession_parameter(2, 4, defNo, charaindex);
        PROFESSION_MAGIC_ATTAIC_Effect(battleindex, bid, ToList, 2);

        if (defNo == 20 || defNo == 25 || defNo == 26) //右
          bid = 0;
        else
          bid = 10;

        for (i = bid; i < bid + 10; i++) {
          int defType, charaidx, petidx;
          charaidx = BATTLE_No2Index(battleindex, i);
          if (CHAR_CHECKINDEX(charaidx)) {
            if (CHAR_getInt(charaidx, CHAR_HP) > 0) {
              petidx = BATTLE_getRidePet(charaidx);
              defType = CHAR_getInt(charaidx, CHAR_WHICHTYPE);
              CHAR_setWorkInt(charaidx, CHAR_WORKICECRACK, 0);
              // BATTLE_BadStatusString( bid, 0 );
              // snprintf( szBuffer, sizeof( szBuffer ), "(%s)将(%s)修理完毕。",
              //	CHAR_getUseName( charaindex ), aszStatusFull[i] );
              //	BATTLE_BroadCast( battleindex, szBuffer,
              //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
              // sprintf( szBuffer, "BM|%X|%X|", bid, 0 );
              // BATTLESTR_ADD( szBuffer );

              damage = value;

              hp = CHAR_getInt(charaidx, CHAR_HP);
              if (petidx == -1) {
                hp -= damage;
                if (hp < 0)
                  hp = 0;
              } else {
                int defquick = 0, petquick = 0, playerdamage = 0;
                pethp = CHAR_getInt(petidx, CHAR_HP);
                defquick = CHAR_getWorkInt(charaidx, CHAR_WORKDEFENCEPOWER);
                petquick = CHAR_getWorkInt(petidx, CHAR_WORKDEFENCEPOWER);
                if (pethp > 0) {
                  playerdamage =
                      (damage * petquick) / (defquick + petquick) + 1;
                  petdamage = damage - playerdamage + 1;
                  hp -= playerdamage;
                  pethp -= petdamage;
                } else {
                  hp -= damage;
                }
                if (hp < 0)
                  hp = 0;
                if (pethp < 0)
                  pethp = 0;
                CHAR_setInt(petidx, CHAR_HP, pethp);
              }

              CHAR_setInt(charaidx, CHAR_HP, hp);

              if (hp <= 0) {
                CHAR_setFlg(charaidx, CHAR_ISDIE, 1);
                CHAR_setWorkInt(charaidx, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
              }
              if (pethp <= 0) {
                CHAR_setFlg(petidx, CHAR_ISDIE, 1);
                CHAR_setWorkInt(ridepet, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
              }

              if (damage >= 0) {
                // snprintf( szBuffer, sizeof( szBuffer ),
                //	"(%s)因(%s)受到(%d)的损伤。",
                //	CHAR_getUseName( charaindex ),
                //	aszStatusFull[i],
                //	damage );
                // BATTLE_BroadCast( battleindex, szBuffer,
                //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

                sprintf(szBuffer, "%X|%X|%X|%X|", i, i, (int)(damage),
                        (int)(petdamage));

                // sprintf( szBuffer, "BD|r%X|0|0|%X|p%X|", i, (int)(damage),
                // (int)(petdamage) );
                BATTLESTR_ADD(szBuffer);
              }
              if (petidx != -1 && petdamage >= 0) {
                // snprintf( szBuffer, sizeof( szBuffer ),
                //	"(%s的骑宠)因(%s)受到(%d)的损伤。",
                //	CHAR_getUseName( charaindex ),
                //	aszStatusFull[i], petdamage );
                // BATTLE_BroadCast( battleindex, szBuffer,
                //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
              }
            }
          }
        }
        sprintf(szBuffer, "%X|", 0x5711438);
        BATTLESTR_ADD(szBuffer);
      }
    }
      print("\n冰爆术");
      break;
#ifdef _PROFESSION_ADDSKILL
    case CHAR_WORKICECRACK2: // 冰爆术2
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK2);
      print("\n冰爆术2");
      break;
    case CHAR_WORKICECRACK3: // 冰爆术3
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK3);
      print("\n冰爆术3");
      break;
    case CHAR_WORKICECRACK4: // 冰爆术4
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK4);
      print("\n冰爆术4");
      break;
    case CHAR_WORKICECRACK5: // 冰爆术5
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK5);
      print("\n冰爆术5");
      break;
    case CHAR_WORKICECRACK6: // 冰爆术6
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK6);
      print("\n冰爆术6");
      break;
    case CHAR_WORKICECRACK7: // 冰爆术7
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK7);
      print("\n冰爆术7");
      break;
    case CHAR_WORKICECRACK8: // 冰爆术8
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK8);
      print("\n冰爆术8");
      break;
    case CHAR_WORKICECRACK9: // 冰爆术9
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK9);
      print("\n冰爆术9");
      break;
    case CHAR_WORKICECRACK10: // 冰爆术10
      WorkIceCrackPlay(charaindex, cnt, CHAR_WORKICECRACK10);
      print("\n冰爆术10");
      break;
#endif //_PROFESSION_ADDSKILL

#endif

#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
    case CHAR_WORKSARS: {
      int j;
      int rate = 60; // 传染 SARS 机率

      // 受传染的人受伤
      {
        int petdown = -1;
        int dec_mp = 0;

        Compute_Down_SARS(charaindex, ridepet, &down, &petdown, ridepet);
        if (down >= 0) {
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( charaindex ),
          //	aszStatusFull[i],
          //	down );
          // BATTLE_BroadCast( battleindex, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

          // 如果是玩家扣 MP
          if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
            int old_mp = CHAR_getInt(charaindex, CHAR_MP);
            dec_mp = old_mp / 10;
            if (dec_mp < 0)
              dec_mp = 0;
            CHAR_setInt(charaindex, CHAR_MP, old_mp - dec_mp);
            sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|m%X|", bid, (int)(down),
                    (int)(petdown), dec_mp);
          } else
            sprintf(szBuffer, "BD|r%X|0|0|%X|p%X|", bid, (int)(down),
                    (int)(petdown));

          BATTLESTR_ADD(szBuffer);
        }
        if (ridepet != -1 && petdown >= 0) {
          // snprintf( szBuffer, sizeof( szBuffer ),
          //	"(%s的骑宠)因(%s)受到(%d)的损伤。",
          //	CHAR_getUseName( charaindex ),
          //	aszStatusFull[i], petdown );
          // BATTLE_BroadCast( battleindex, szBuffer,
          //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
        }
      }

      // 主传染者传SARS
      if (CHAR_getWorkInt(charaindex, CHAR_WORKMODSARS) > 0) {
        int buf2[5], ti = 0;
        int tibuf[10] = {3, 1, 0, 2, 4, 8, 6, 5, 7, 9};

        memset(buf2, -1, sizeof(buf2));

        if (defNo > 9) {
          for (j = 0; j < 10; j++) {
            tibuf[j] += 10;
          }
        }

        for (j = 0; j < 10; j++) {
          if (tibuf[j] != defNo)
            continue;
          if (j > 4) {
            if ((j + 1) < 10)
              buf2[ti++] = tibuf[j + 1];
            if ((j - 1) > 4)
              buf2[ti++] = tibuf[j - 1];
            //不等边
            if ((j - 5 + 1) < 5)
              buf2[ti++] = tibuf[j - 5 + 1];
            if ((j - 5 - 1) >= 0)
              buf2[ti++] = tibuf[j - 5 - 1];
            if ((j - 5) >= 0)
              buf2[ti++] = tibuf[j - 5];
          } else {
            if ((j + 1) < 5)
              buf2[ti++] = tibuf[j + 1];
            if ((j - 1) >= 0)
              buf2[ti++] = tibuf[j - 1];
            //不等边
            if ((j + 5 + 1) < 10)
              buf2[ti++] = tibuf[j + 5 + 1];
            if ((j + 5 - 1) > 4)
              buf2[ti++] = tibuf[j + 5 - 1];
            if ((j + 5) < 10)
              buf2[ti++] = tibuf[j + 5];
          }
          break;
        }

        print("buf2[%d,%d,%d,%d,%d]\n", buf2[0], buf2[1], buf2[2], buf2[3],
              buf2[4]);

        for (j = 0; j < 5; j++) {

          int toindex = 0;
          if (buf2[j] == -1)
            continue;

          toindex = BATTLE_No2Index(battleindex, buf2[j]);
          if (!CHAR_CHECKINDEX(toindex))
            continue;
          // 得到sars的离开
          if (CHAR_getWorkInt(toindex, CHAR_WORKSARS) > 0)
            continue;

          if (RAND(1, 100) <= rate) { // 中 SARS
            if (toindex == charaindex)
              continue;
            if (CHAR_getInt(toindex, CHAR_HP) <= 0)
              continue;

            CHAR_setWorkInt(toindex, CHAR_WORKSARS, 3);
          }
        }
      }
    } break;
#endif

    default:
      // snprintf( szBuffer, sizeof( szBuffer ),
      //	"(%s)是(%s)", CHAR_getUseName( charaindex ),
      //	aszStatusFull[i] );
      // BATTLE_BroadCast( battleindex, szBuffer,
      //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
      break;
    }
  }

#ifdef _SUIT_ITEM
  {
    int defCode;
    int AddHp = 0, AddMp = 0;
#ifdef _IMPRECATE_ITEM
    int nums1, nums2, nums3, delMp = 0;
#endif
    if ((defCode = CHAR_getWorkInt(charaindex, CHAR_WORKSUITITEM)) > 0) {
      AddHp = CHAR_getWorkInt(charaindex, CHAR_WORKROUNDHP);
      AddMp = CHAR_getWorkInt(charaindex, CHAR_WORKROUNDMP);
    }

#ifdef _IMPRECATE_ITEM
    if ((nums1 = CHAR_getWorkInt(charaindex, CHAR_WORKIMPRECATENUM1)) > 0) {
      delMp = CHAR_getWorkInt(charaindex, CHAR_WORKHURTMP);
      CHAR_setWorkInt(charaindex, CHAR_WORKIMPRECATENUM1, nums1 - 1);
    }
    if ((nums2 = CHAR_getWorkInt(charaindex, CHAR_WORKIMPRECATENUM2)) > 0) {
      AddHp += CHAR_getWorkInt(charaindex, CHAR_WORKWISHESHP);
      CHAR_setWorkInt(charaindex, CHAR_WORKIMPRECATENUM2, nums2 - 1);
    }
    if ((nums3 = CHAR_getWorkInt(charaindex, CHAR_WORKIMPRECATENUM3)) > 0) {
      AddMp += CHAR_getWorkInt(charaindex, CHAR_WORKWISHESMP);
      CHAR_setWorkInt(charaindex, CHAR_WORKIMPRECATENUM3, nums3 - 1);
    }
    AddMp = AddMp - delMp;
#endif
    if (AddHp != 0 || AddMp != 0) {
      int AddPHp = 0;

#ifdef _TYPE_TOXICATION
      if (AddHp != 0 && CHAR_CanCureFlg(charaindex, "FALSE") != FALSE) {
#else
      if (AddHp != 0) {
#endif
        int MyHp = CHAR_getInt(charaindex, CHAR_HP);
        int MaxHp = CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP);
        MyHp = ((MyHp + AddHp) >= MaxHp) ? MaxHp : (MyHp + AddHp);
        MyHp = (MyHp <= 1) ? 1 : MyHp;
        CHAR_setInt(charaindex, CHAR_HP, MyHp);
        // snprintf( szBuffer, sizeof( szBuffer ), "(%s)增加(%d)的HP。",
        //	CHAR_getUseName( charaindex ), AddHp );
        // BATTLE_BroadCast( battleindex, szBuffer,
        //	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
        if (ridepet != -1) {
          int PetMaxHp = CHAR_getWorkInt(ridepet, CHAR_WORKMAXHP);
          int PetHp = CHAR_getInt(ridepet, CHAR_HP);
          AddPHp = AddHp;
          PetHp = ((PetHp + AddPHp) >= PetMaxHp) ? PetMaxHp : (PetHp + AddPHp);
          PetHp = (PetHp <= 1) ? 1 : PetHp;
          CHAR_setInt(ridepet, CHAR_HP, PetHp);
        }
      }
      if (AddMp != 0) {
        int MyMp = CHAR_getInt(charaindex, CHAR_MP);
        int MaxMp = CHAR_getWorkInt(charaindex, CHAR_WORKMAXMP);
        MyMp = ((MyMp + AddMp) > MaxMp) ? MaxMp : (MyMp + AddMp);
        MyMp = (MyMp < 0) ? 0 : MyMp;
        CHAR_setInt(charaindex, CHAR_MP, MyMp);
        /*
        #ifdef _FIXITEMANISHOW
                                        sprintf( szBuffer,
        "BD|r%X|0|1|%X|p%X|m%X|", bid, AddHp, AddPHp, AddMp); }else	{
                                        sprintf( szBuffer, "BD|r%X|0|1|%X|p%X|",
        bid, AddHp, AddPHp ); #else
        */
        sprintf(szBuffer, "BD|r%X|0|2|%X|p%X|m%X|", bid, AddHp, AddPHp, AddMp);
      } else {
        sprintf(szBuffer, "BD|r%X|0|2|%X|p%X|", bid, AddHp, AddPHp);
        //#endif
      }
      BATTLESTR_ADD(szBuffer);
    }
  }
#endif
#ifdef _PETSKILL_SETDUCK
  {
    int turns = 0, nums = 0;
    if ((turns = CHAR_getWorkInt(charaindex, CHAR_MYSKILLDUCK)) > 0) {
      turns--;
      if (turns == 0) {
        nums = CHAR_getWorkInt(charaindex, CHAR_MYSKILLDUCKPOWER);
        sprintf(szBuffer, "BD|r%X|0|3|%X|", bid, nums * (-1));
        BATTLESTR_ADD(szBuffer);
      }
      CHAR_setWorkInt(charaindex, CHAR_MYSKILLDUCK, turns);
    }
#ifdef _MAGICPET_SKILL
    if ((turns = CHAR_getWorkInt(charaindex, CHAR_MYSKILLSTR)) > 0) {
      turns--;
      if (turns == 0) {
        nums = CHAR_getWorkInt(charaindex, CHAR_MYSKILLSTRPOWER);
        sprintf(szBuffer, "BD|r%X|0|4|%X|", bid, nums * (-1));
        BATTLESTR_ADD(szBuffer);
      }
      CHAR_setWorkInt(charaindex, CHAR_MYSKILLSTR, turns);
    }
    if ((turns = CHAR_getWorkInt(charaindex, CHAR_MYSKILLTGH)) > 0) {
      turns--;
      if (turns == 0) {
        nums = CHAR_getWorkInt(charaindex, CHAR_MYSKILLTGHPOWER);
        sprintf(szBuffer, "BD|r%X|0|5|%X|", bid, nums * (-1));
        BATTLESTR_ADD(szBuffer);
      }
      CHAR_setWorkInt(charaindex, CHAR_MYSKILLTGH, turns);
    }
    if ((turns = CHAR_getWorkInt(charaindex, CHAR_MYSKILLDEX)) > 0) {
      turns--;
      if (turns == 0) {
        nums = CHAR_getWorkInt(charaindex, CHAR_MYSKILLDEXPOWER);
        sprintf(szBuffer, "BD|r%X|0|6|%X|", bid, nums * (-1));
        BATTLESTR_ADD(szBuffer);
      }
      CHAR_setWorkInt(charaindex, CHAR_MYSKILLDEX, turns);
    }
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    if ((turns = CHAR_getWorkInt(charaindex, CHAR_MYSKILLHIT)) > 0) {
      turns--;
      if (turns == 0) {
        int dec_num = CHAR_getWorkInt(charaindex, CHAR_MYSKILLHIT_NUM);
        nums = CHAR_getWorkInt(charaindex, CHAR_WORKHITRIGHT);
        nums -= dec_num;
        CHAR_setWorkInt(charaindex, CHAR_WORKHITRIGHT, nums);

        CHAR_talkToCli(charaindex, -1, "恢复命中率", CHAR_COLORYELLOW);
        // sprintf( szBuffer, "BD|r%X|0|6|%X|", bid, dec_num*(-1));
        // BATTLESTR_ADD( szBuffer );
      }
      CHAR_setWorkInt(charaindex, CHAR_MYSKILLHIT, turns);
    }
#endif

#endif
  }
#endif
  return 0;
}

BOOL BATTLE_CanMoveCheck(int charaindex) {

  //
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARALYSIS) > 0) {
    return FALSE;
  }
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSTONE) > 0) {
    return FALSE;
  }
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSLEEP) > 0) {
    return FALSE;
  }
#ifdef _MAGIC_BARRIER
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBARRIER) > 0) {
    return FALSE;
  }
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
  // 晕眩
  if (CHAR_getWorkInt(charaindex, CHAR_WORKDIZZY) > 0) {
    return FALSE;
  }
  // 天罗地网
  if (CHAR_getWorkInt(charaindex, CHAR_WORKDRAGNET) > 0) {
    return FALSE;
  }
  /*
          // 冰箭术
          if( CHAR_getWorkInt( charaindex, CHAR_WORKICEARROW ) > 0 ){
                  return FALSE;
          }
  */
  /*
          // 专注战斗
          if( CHAR_getWorkInt( charaindex, CHAR_WORK_FOCUS ) > 0 ){
                  return FALSE;
          }
  */
  // 雷附体
  if (CHAR_getWorkInt(charaindex, CHAR_WORK_T_ENCLOSE) > 0) {
    return FALSE;
  }
#endif
#ifdef _PROFESSION_ADDSKILL
  if (CHAR_getWorkInt(charaindex, CHAR_DOOMTIME) > 0) {
    return FALSE;
  }
#endif
#ifdef _PRO_ABSOLUTE_DEFENSE
  if (CHAR_getWorkInt(charaindex, CHAR_WORK_ABSOLUTE) > 0)
    return FALSE;
#endif
#ifdef _PRO3_ADDSKILL
  if (GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_ROAR)) > 0)
    return FALSE;
  if (GETHIGHVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_POISON_DRAGNET)) > 0)
    return FALSE;
#endif
  return TRUE;
}

#ifdef _ITEM_EQUITSPACE
int BATTLE_GetEqShield(int charaindex) {
  int iRet = ITEM_FIST, itemindex;

  itemindex = CHAR_getItemIndex(charaindex, CHAR_EQSHIELD);
  if (ITEM_CHECKINDEX(itemindex) == FALSE) {
    iRet = ITEM_FIST;
  } else {
    iRet = ITEM_getInt(itemindex, ITEM_TYPE);
  }
  return iRet;
}
#endif

int BATTLE_GetWepon(int charaindex) //取得角色手持武器
{
  int iRet = ITEM_FIST, itemindex;

  itemindex = CHAR_getItemIndex(charaindex, CHAR_ARM);
  if (ITEM_CHECKINDEX(itemindex) == FALSE) {
    iRet = ITEM_FIST;
  } else {
    iRet = ITEM_getInt(itemindex, ITEM_TYPE);
  }

  return iRet;
}

static int aBowW[50] = {
    0, 2, 1, 4, 3, 0, 1, 2, 3, 4, 1, 0, 3, 2, 4, 1, 3, 0, 2, 4, 2, 4, 0, 1, 3,
    2, 0, 4, 1, 3, 3, 1, 0, 2, 4, 3, 1, 0, 2, 4, 4, 2, 0, 1, 3, 4, 2, 0, 1, 3,
};

void BATTLE_TargetListSet(int charaindex, int attackNo, int *pList) {
  int i, j = 0, defNo, battleindex = -1;
  defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
  battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);

#ifdef _PREVENT_TEAMATTACK
#ifdef _SKILLLIMIT // (可开放) Syu ADD 不得攻击我方限制
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_FALLRIDE || // 落马
      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_WILDVIOLENTATTACK || //狂暴攻击
      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_TIMID || // 怯战
      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_MPDAMAGE || // MP攻击
      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_WEAKEN || // 虚弱
      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_DAMAGETOHP || // 嗜血
      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_STEALMONEY) { // 捐献
    if (BATTLE_CheckSameSide(charaindex, defNo) == 1) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      //			BATTLE_NoAction( battleindex, attackNo );
      return;
    }
  }
#endif
#endif

  for (i = 0; i < BATTLE_ENTRY_MAX * 2; i++) {
    pList[i] = defNo;
  }
#ifdef _BATTLE_ATTCRAZED // ANDY 疯狂暴走
#ifdef _SHOOTCHESTNUT    // Syu ADD 宠技：丢栗子
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_ATTCRAZED ||
      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_ATTSHOOT) {
#else
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
      BATTLE_COM_S_ATTCRAZED) {
#endif
    int deftop, defsub;
    int plive[10];
    int n = CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3);

#ifdef _PREVENT_TEAMATTACK
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
    if (BATTLE_CheckSameSide(charaindex, defNo) == 1) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      BATTLE_NoAction(battleindex, attackNo);
      return;
    }
#endif
#endif

    if (0 <= defNo && defNo <= 9) {
      defsub = 0;
      deftop = 9;
    } else if (10 <= defNo && defNo <= 19) {
      defsub = 10;
      deftop = 19;
    } else {
      pList[1] = -1;
      return;
    }
    for (i = defsub; i < deftop; i++) {
      if (BATTLE_TargetCheck(battleindex, i) == FALSE)
        continue;
      plive[j++] = i;
    }
    if (j == 0)
      return;
    for (i = 0; i < n; i++) {
      pList[i] = plive[RAND(0, j - 1)];
    }
    pList[i] = -1;
    return;
  }
#endif
  if (BATTLE_GetWepon(charaindex) != ITEM_BOW) {
    return;
  }

  if (0 <= defNo && defNo <= 19) {
    int defsub = defNo % 5;
    int deftop = defNo - defsub;
    int Rnd = RAND(0, 1);
    for (j = 0, i = 0; j < 5; j++) {
      pList[i] = aBowW[defsub * 10 + Rnd * 5 + j] + deftop;
      if (deftop == 0 || deftop == 10) {
        pList[i + 1] = pList[i] + 5;
      } else {
        pList[i + 1] = pList[i] - 5;
      }
      if (pList[i] == attackNo) {
        pList[i] = -1;
      }
      if (pList[i + 1] == attackNo) {
        pList[i + 1] = -1;
      }
      i += 2;
    }
    pList[i] = -1;
  } else {
    pList[1] = -1;
  }

  return;
}

int BATTLE_GetAttackCount(int charaindex) {
  int iRet = 1, itemindex;
  itemindex = CHAR_getItemIndex(charaindex, CHAR_ARM);
  if (ITEM_CHECKINDEX(itemindex) == FALSE) {
    return 0;
  } else {
    iRet = RAND(ITEM_getInt(itemindex, ITEM_ATTACKNUM_MIN),
                ITEM_getInt(itemindex, ITEM_ATTACKNUM_MAX));
  }
  if (iRet <= 0)
    iRet = 1;

  return iRet;
}

int BATTLE_PetRandomSkill(int battleindex, int charaindex) {
#define PETSKILLSERCHTIME 50
  int i, j, iNum, toNo, k, myNo, side, pskill_array;
  int skill_type;
#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
  int pid = CHAR_getInt(charaindex, CHAR_PETID);
  int skNum;
#endif

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);

  myNo = BATTLE_Index2No(battleindex, charaindex);
  side = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLESIDE);

#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
  iNum = RAND(0, CHAR_MAXPETSKILLHAVE - 1);
  skNum = CHAR_getPetSkill(charaindex, iNum);
  if (((pid == 981) || (pid == 982) || (pid == 983) || (pid == 984)) &&
      (skNum == 600)) {
    do {
      iNum = RAND(0, CHAR_MAXPETSKILLHAVE - 1);
      skNum = CHAR_getPetSkill(charaindex, iNum);
    } while (skNum == 600);
  }
#else
  iNum = RAND(0, CHAR_MAXPETSKILLHAVE - 1);
#endif

  for (k = 0; k < 3; k++) {
    toNo = BATTLE_DefaultAttacker(battleindex, 1 - side);
    if (toNo == myNo) {

    } else {
      break;
    }
  }

  if (myNo == toNo) {
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    return 0;
  }

  for (j = 0, i = 0, k = 0; k < PETSKILLSERCHTIME; i++, k++) {
    if (i >= CHAR_MAXPETSKILLHAVE)
      i = 0;

    if (CHAR_getCharHaveSkill(charaindex, i) == NULL) {
      continue;
    }

    pskill_array = PETSKILL_GetArray(charaindex, i);
    skill_type = PETSKILL_getInt(pskill_array, PETSKILL_FIELD);

    if (skill_type == PETSKILL_FIELD_BATTLE ||
        skill_type == PETSKILL_FIELD_ALL) {
    } else {
      continue;
    }

    if (j < iNum) {
      j++;
      continue;
    }

    if (strcmp(PETSKILL_getChar(pskill_array, PETSKILL_FUNCNAME),
               "PETSKILL_AttackMagic") == 0) {
      continue;
    }

    if (strcmp(PETSKILL_getChar(pskill_array, PETSKILL_FUNCNAME),
               "PETSKILL_Combined") == 0) {
      continue;
    }

    if (PETSKILL_Use(charaindex, iNum, toNo, NULL) == TRUE) {

    } else {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    }
    break;
  }

  if (k >= PETSKILLSERCHTIME) {
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
  }
  return 0;
}

enum {
  PETAI_MODE_NORMAL,
  PETAI_MODE_TARGETRANDOM,
  PETAI_MODE_RANDOMACT,
  PETAI_MODE_OWNERATTACK,
  PETAI_MODE_ENEMYATTACK,
  PETAI_MODE_ESCAPE,
  PETAI_MODE_NOACT,
  PETAI_MODE_END
};

static int BATTLE_PetLoyalCheck(int battleindex, int bid, int charaindex) {
  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    return FALSE;
  }
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return FALSE;

  int ai, toNo, mode = 0, type, Rand, toSide = 1, flg, i;
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
    return 0;
  }
  if (CHAR_getFlg(charaindex, CHAR_ISDIE) == TRUE)
    return 0;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) == BATTLE_COM_WAIT) {
    return 0;
  }
  flg = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLG);
  flg &= ~CHAR_BATTLEFLG_AIBAD;
  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEFLG, flg);
  if (BATTLE_CanMoveCheck(charaindex) == FALSE) {
    return 0;
  }
  toNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
  toSide = (0 <= toNo && toNo < BATTLE_ENTRY_MAX) ? (0) : (1);
  ai = CHAR_getWorkInt(charaindex, CHAR_WORKFIXAI);
#ifdef _PRO3_ADDSKILL
  if (GETTHIRDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_PACIFY)) >
      0) //抚慰心灵
    ai += GETSECONDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_PACIFY));
#endif
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) == BATTLE_COM_GUARD ||
      bid == toNo) {
    type = 1;
  } else {
    type = 0;
  }

  Rand = RAND(1, 100);

  mode = 0;                   // 恳仄中垫
  if (ai >= 80) {             // 镝擦蘸ㄧㄟ⊙动晓
    mode = PETAI_MODE_NORMAL; // 濮覆岳
  } else if (ai >= 70) {      // 镝擦蘸ㄦㄟ⊙动晓
    if (Rand < 10)
      mode = PETAI_MODE_TARGETRANDOM; // 正□必永玄
  } else if (ai >= 60) {
    if (Rand < 20)
      mode = PETAI_MODE_TARGETRANDOM; // 正□必永玄
  } else if (ai >= 50) {
    if (Rand < 35)
      mode = PETAI_MODE_TARGETRANDOM; // 正□必永玄
  } else if (ai >= 40) {
    if (Rand < 50)
      mode = PETAI_MODE_TARGETRANDOM; // 正□必永玄
  } else if (ai >= 30) {
    if (Rand < 70)
      mode = PETAI_MODE_RANDOMACT; // 垫  仿件母丞
  } else if (ai >= 20) {
    if (Rand < 70)
      mode = PETAI_MODE_RANDOMACT; // 垫  仿件母丞
  } else if (ai >= 10) {
    if (Rand < 80) {
      mode = PETAI_MODE_OWNERATTACK; // 潜谛  猾
    } else {
      mode = PETAI_MODE_ENEMYATTACK; // 衬  猾
    }
  } else {
    if (Rand < 60) {
      mode = PETAI_MODE_OWNERATTACK; // 潜谛  猾
    } else {
      mode = PETAI_MODE_ESCAPE; //   仆月
    }
  }

  if (mode == PETAI_MODE_NORMAL)
    return 0;

  for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {
    if (CHAR_getCharHaveSkill(charaindex, i)) {
      break;
    }
  }
  if (i >= CHAR_MAXPETSKILLHAVE) {
    mode = PETAI_MODE_NOACT;
  }
  flg = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLG);
  flg |= CHAR_BATTLEFLG_AIBAD;
  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEFLG, flg);

  switch (mode) {
  case PETAI_MODE_NOACT:
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    break;
  case PETAI_MODE_TARGETRANDOM:
    if (type == 1) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    } else {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                      BATTLE_DefaultAttacker(battleindex, toSide));
    }
    break;
  case PETAI_MODE_RANDOMACT: // 垫  互仿件母丞
    // 哗萄ㄠ葱  猾蟆反濮覆井尹切扎母丢
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
        BATTLE_COM_S_EARTHROUND0) {
      return 0;
    }

    // 布□犯奴失件白仿弘毛反内允
    flg = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLG);
    flg &= ~CHAR_BATTLEFLG_GUARDIAN;
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEFLG, flg);

    if (toNo == bid) { // 愤坌毛蓟氏匹仄引匀凶日
      // 窒手匹五卅中仇午卞允月
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    } else {
      // 仿件母丞卞檗  毛本永玄
      BATTLE_PetRandomSkill(battleindex, charaindex);
    }
    break;

  case PETAI_MODE_OWNERATTACK: // 潜谛  猾
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, bid - 5);
    break;

  case PETAI_MODE_ENEMYATTACK: // 衬  猾
  {
    int myside = 0;
    if (bid >= BATTLE_ENTRY_MAX)
      myside = 1;
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
    // 锹澎扔奶玉卞
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                    BATTLE_DefaultAttacker(battleindex, 1 - myside));
  } break;

  case PETAI_MODE_ESCAPE: //   仆月公及  夫旦玄
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_LOSTESCAPE);
    break;
  }

  return 1;
}

#ifdef _ATTACK_MAGIC

#define MAX_MAGIC_NUMBER 29

// 每一种魔法的攻击范围 (id,area)
int TargetIndex[MAX_MAGIC_NUMBER][2] = {
    {301, -1}, {302, -1}, {303, 26}, {304, -1}, {305, 20}, {306, 20},
    {307, -1}, {308, -1}, {309, -1}, {310, -1}, {311, 26}, {312, 20},
    {313, -1}, {314, -1}, {315, -1}, {316, -1}, {317, 26}, {318, 20},
    {319, -1}, {320, -1}, {321, 26}, {322, -1}, {323, 26}, {324, 20},
    {325, 20}, {431, 20}, {432, 20}, {433, 20}, {434, 20}};

int magic, toindex, magic_count;

#endif

static int BATTLE_Battling(int battleindex) {
  BATTLE *pBattle, *pWatchBattle;
  BATTLE_ENTRY *pEntry[2];
  char *aszFieldAttr[] = {"无", "地", "水", "火", "风"}, szBuffer[256] = "",
       szWork[256];
  int i, j, k, len, entrynum, charaindex, attackNo,
      defNo = -1, aAttackList[BATTLE_ENTRY_MAX * 2 + 1],
      aDefList[BATTLE_ENTRY_MAX * 2 + 1], ComboId, itemindex, AllSize = 0;
  BATTLE_CHARLIST EntryList[40];
  int ContFlg, attackNoSub, defNoSub;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  pBattle = &BattleArray[battleindex];
  pEntry[0] = pBattle->Side[0].Entry;
  pEntry[1] = pBattle->Side[1].Entry;

  for (i = 0; i < arraysizeof(EntryList); i++) {
    EntryList[i].charaindex = -1;
    EntryList[i].combo = 0;
#ifdef _PRO_ABSOLUTE_DEFENSE
    EntryList[i].absolute_defense = FALSE;
#endif
  }
  sprintf(szBuffer, "战役 %d 转换 %d 属性 %s", battleindex, pBattle->turn,
          aszFieldAttr[pBattle->field_att]);

  entrynum = 0;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      pEntry[j][i].flg &= ~BENT_FLG_ULTIMATE;
      if (pEntry[j][i].charaindex == -1)
        continue;
      EntryList[entrynum].charaindex = pEntry[j][i].charaindex;
      EntryList[entrynum].side = j;
      EntryList[entrynum].num = pEntry[j][i].bid;
      EntryList[entrynum].dex = BATTLE_DexCalc(pEntry[j][i].charaindex);
#ifdef _EQUIT_SEQUENCE
      EntryList[entrynum].sequence =
          CHAR_getWorkInt(pEntry[j][i].charaindex, CHAR_WORKSEQUENCEPOWER);
#endif
#ifdef _PRO_ABSOLUTE_DEFENSE
      if (CHAR_getWorkInt(pEntry[j][i].charaindex, CHAR_WORKBATTLECOM1) ==
          BATTLE_COM_S_ABSOLUTE_DEFENSE)
        EntryList[entrynum].absolute_defense = TRUE;
#endif
      BATTLE_talkToCli(pEntry[j][i].charaindex, szBuffer, CHAR_COLORYELLOW);
      entrynum++;
    }
  }
  szAllBattleString[0] = 0;
  k = 0;
  for (j = 0; j < 2; j++) {
    k = j * SIDE_OFFSET;
    for (i = 0; i < BATTLE_ENTRY_MAX / 2; i++) {
      szBattleString[0] = 0;
      if (pBattle->iEntryBack[k] >= 0 &&
          pBattle->Side[j].Entry[i].charaindex == -1) {
        if (pBattle->iEntryBack2[k] >= 0) {
          sprintf(szBattleString, "BE|et%X|f1|", k);
          // BATTLE_BroadCast( battleindex, "突然有人不见了。", CHAR_COLORYELLOW
          // );
        } else {
          print("应是输入ID不正确出现的error。\n");
          // BATTLE_BroadCast( battleindex, "ID错误的error。", CHAR_COLORYELLOW
          // );
        }
      }
      k++;
      strcat(szAllBattleString, szBattleString);
    }
  }
  len = strlen(szAllBattleString);
  AllSize += len;
  EntrySort(EntryList, entrynum);
  ComboCheck(EntryList, entrynum);
  for (i = 0; i < entrynum; i++) {
    int COM, myside, otherside, bi, attack_flg = 1, attack_max = 1,
                                    attack_count = 0;
#ifdef _PETSKILL_ADDATTCRAZED
    int addattcrazedToList[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
#endif
    charaindex = EntryList[i].charaindex;
    myside = EntryList[i].side;
    otherside = 1 - myside;
    attackNo = EntryList[i].num;
    bi = attackNo - myside * SIDE_OFFSET;
    aAttackList[0] = attackNo;
    aAttackList[1] = -1;
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    if (CHAR_getFlg(charaindex, CHAR_ISDIE) == TRUE)
      continue;
    szBadStatusString[0] = 0;
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
        BATTLE_CHARMODE_C_OK) {
      // sprintf( szBuffer, "指令输入不完全。(%s)", CHAR_getUseName( charaindex
      // ) ); BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORYELLOW );
      continue;
    }
    if (CHAR_getInt(charaindex, CHAR_HP) <= 0)
      continue;

#ifdef _ABSOLUTE_DEBUG
    comnum = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
#endif
    szBattleString[0] = 0;
    pszBattleTop = szBattleString;
    pszBattleLast = szBattleString + arraysizeof(szBattleString);
    // WON ADD
    if (CHAR_getFlg(charaindex, CHAR_ISDIE) != TRUE)
      BATTLE_StatusSeq(charaindex);
#ifdef _OTHER_MAGICSTAUTS
    BATTLE_MagicStatusSeq(charaindex);
#endif
#ifdef _MAGIC_DEEPPOISON //   剧毒
    if (CHAR_getFlg(charaindex, CHAR_ISDIE) == TRUE) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    }
#endif
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    BATTLE_ProfessionStatusSeq(battleindex, charaindex);
#endif
#ifdef _PETSKILL_PROVOKEFIGHT
    if (GETTHIRDVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_PROVOKEFIGHT)) >
        0) {
      char szcommand[16];
      int tempprovokefight0 =
          CHAR_getWorkInt(charaindex, CHAR_WORK_PROVOKEFIGHT);
      if (GETTHIRDVALUE(tempprovokefight0) > 0 &&
          BATTLE_TargetCheck(battleindex, GETFIRSTVALUE(tempprovokefight0)) &&
          GETFOURTHVALUE(CHAR_getWorkInt(
              BATTLE_No2Index(battleindex, GETFIRSTVALUE(tempprovokefight0)),
              CHAR_WORK_PROVOKEFIGHT)) > 0) {
        int tempprovokefight1;
        CHAR_setWorkInt(charaindex, CHAR_WORK_PROVOKEFIGHT,
                        MAKE4VALUE(GETFOURTHVALUE(tempprovokefight0),
                                   GETTHIRDVALUE(tempprovokefight0) - 1,
                                   GETSECONDVALUE(tempprovokefight0),
                                   GETFIRSTVALUE(tempprovokefight0)));
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                        GETFIRSTVALUE(tempprovokefight0));
        sprintf(szcommand, "BD|r%X|0|A|%X|", attackNo, 1);
        BATTLESTR_ADD(szcommand);
        CHAR_setWorkInt(
            BATTLE_No2Index(battleindex, GETFIRSTVALUE(tempprovokefight0)),
            CHAR_WORKACUPUNCTURE, 1); //针刺外皮
#ifdef _NB_FIX_ACUPUNCTURE
        CHAR_setWorkInt(
            BATTLE_No2Index(battleindex, GETFIRSTVALUE(tempprovokefight0)),
            CHAR_WORKACUPUNCPERCENTAGE, GETSECONDVALUE(tempprovokefight0));
#endif
        tempprovokefight1 = CHAR_getWorkInt(
            BATTLE_No2Index(battleindex, GETFIRSTVALUE(tempprovokefight0)),
            CHAR_WORK_PROVOKEFIGHT);
        CHAR_setWorkInt(
            BATTLE_No2Index(battleindex, GETFIRSTVALUE(tempprovokefight0)),
            CHAR_WORK_PROVOKEFIGHT,
            MAKE4VALUE(GETFOURTHVALUE(tempprovokefight1) - 1,
                       GETTHIRDVALUE(tempprovokefight1),
                       GETSECONDVALUE(tempprovokefight1),
                       GETFIRSTVALUE(tempprovokefight1)));

      } else {
        CHAR_setWorkInt(charaindex, CHAR_WORK_PROVOKEFIGHT, 0);
      }
    }
#endif
    if (BATTLE_CanMoveCheck(charaindex) == FALSE) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
    }
    gBattleDamageModyfy = 1.0;
    gBattleDuckModyfy = 0;
    gBattleStausChange = -1;
    gBattleStausTurn = 0;
#ifdef _PETSKILL_PLUS1
    gBattleStausSuccess = 0;
#endif
    gWeponType = ITEM_FIST;
    gWeponType = BATTLE_GetWepon(charaindex);
#ifdef _PETSKILL_BECOMEFOX
    if (CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) != -1 //若是变成小狐狸
        || CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 101749) {
      int COM = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
      gWeponType = ITEM_FIST;

      if (!(COM == BATTLE_COM_ATTACK || COM == BATTLE_COM_GUARD ||
            COM == BATTLE_COM_NONE)) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        CHAR_talkToCli(charaindex, -1, "变成小狐狸后只能攻击、防御和待机！",
                       CHAR_COLORYELLOW);
      }
    }
#endif
#ifdef _PETSKILL_BECOMEPIG
    if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1 //变成乌力了
        && (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)) {
      int COM = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
      gWeponType = ITEM_FIST;
      // CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );
      if (!(COM == BATTLE_COM_ATTACK || COM == BATTLE_COM_GUARD ||
            COM == BATTLE_COM_NONE || COM == BATTLE_COM_ITEM ||
            COM == BATTLE_COM_ESCAPE || COM == BATTLE_COM_CAPTURE ||
            COM == BATTLE_COM_WAIT || COM == BATTLE_COM_PETIN ||
            COM == BATTLE_COM_PETOUT)) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD);
        // CHAR_talkToCli( charaindex, -1,
        // "变成乌力後只能攻击、防御、待机、逃跑和使用道具！", CHAR_COLORYELLOW
        // );
        CHAR_talkToCli(charaindex, -1, "变成乌力后不能使用咒术和职业技能！",
                       CHAR_COLORYELLOW);
      }
    }
#endif
#ifdef _PRO_KILLME
    if (CHAR_getWorkInt(charaindex, CHAR_WORKANGER) > 0) {
      int target = CHAR_getWorkInt(charaindex, CHAR_WORKANGER_TARGET);
      if (CHAR_CHECKINDEX(target) && CHAR_getInt(target, CHAR_HP) > 0) {
        if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEENEMY) {
          if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) ==
              BATTLE_COM_ATTACK) {
            CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                            BATTLE_Index2No(battleindex, target));
          }
        } else {
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                          BATTLE_Index2No(battleindex, target));
        }
      }
    }
#endif
    gDamageDiv = 1.0;
    attack_max = BATTLE_GetAttackCount(charaindex);
    if (attack_max <= 0) {
      int work, randwork;
      work = CHAR_getInt(charaindex, CHAR_LV);
      if (work < 10 ||
          CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
        attack_max = 1;
      } else {
        int luckwork = CHAR_getInt(charaindex, CHAR_LUCK) * 5;
        if (luckwork > 5 * 5) {
          CHAR_setInt(charaindex, CHAR_LUCK, 25);
          // print( "\nerr:运气奇怪(%s)(%d)\n",CHAR_getUseName( charaindex
          // ),CHAR_getInt( charaindex, CHAR_LUCK ) );
          luckwork = 5 * 5;
        }
        randwork = RAND(1, 1000);
        if (randwork <= 10 + luckwork) {
          attack_max = RAND(5, 10);
        } else if (randwork <= 30 + luckwork) {
          attack_max = 3;
        } else if (randwork <= 70 + luckwork) {
          attack_max = 2;
        } else {
          attack_max = 1;
        }
      }
    } else {
      if (gWeponType == ITEM_FIST)
        gDamageDiv = attack_max;
    }
    if (gWeponType == ITEM_BREAKTHROW) {
      gBattleStausChange = BATTLE_ST_PARALYSIS;
      gBattleStausTurn = 1 - 1;
    } else {
    }

    if (BattleArray[battleindex].Side[myside].flg & BSIDE_FLG_SURPRISE) {
    } else {
      if (BATTLE_PetLoyalCheck(battleindex, attackNo, charaindex)) {
#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
        if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 101428
#ifdef _EXPANSION_VARY_WOLF
            || CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 104109
#endif
        ) {
          if (CHAR_getWorkInt(charaindex, CHAR_WORKTURN) == 0) {
            CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
                        CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER));
            CHAR_setWorkInt(charaindex, CHAR_WORKATTACKPOWER,
                            CHAR_getWorkInt(charaindex, CHAR_WORKFIXSTR));
            CHAR_setWorkInt(charaindex, CHAR_WORKDEFENCEPOWER,
                            CHAR_getWorkInt(charaindex, CHAR_WORKFIXTOUGH));
            CHAR_setWorkInt(charaindex, CHAR_WORKQUICK,
                            CHAR_getWorkInt(charaindex, CHAR_WORKFIXDEX));
          }
        }
#endif
        sprintf(szWork, "BX|%X|", attackNo);
        BATTLESTR_ADD(szWork);
      }
    }
#ifdef _PROFESSION_ADDSKILL
    if (CHAR_getWorkInt(charaindex, CHAR_DOOMTIME) > 0) {
      CHAR_setWorkInt(charaindex, CHAR_DOOMTIME,
                      CHAR_getWorkInt(charaindex, CHAR_DOOMTIME) - 1);
    }
    if (CHAR_getWorkInt(charaindex, CHAR_DOOMTIME) == 0 &&
        (CHAR_getWorkInt(charaindex, CHAR_WORK_com1) ==
             BATTLE_COM_S_FIRE_SPEAR ||
         CHAR_getWorkInt(charaindex, CHAR_WORK_com1) == BATTLE_COM_S_DOOM)) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1,
                      CHAR_getWorkInt(charaindex, CHAR_WORK_com1));
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                      CHAR_getWorkInt(charaindex, CHAR_WORK_toNo));
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE,
                      CHAR_getWorkInt(charaindex, CHAR_WORK_mode));
      CHAR_SETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3,
                           CHAR_getWorkInt(charaindex, CHAR_WORK_skill_level));
      CHAR_SETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3,
                          CHAR_getWorkInt(charaindex, CHAR_WORK_array));
      CHAR_setWorkInt(charaindex, CHAR_WORK_com1, 0);
      CHAR_setWorkInt(charaindex, CHAR_WORK_toNo, 0);
      CHAR_setWorkInt(charaindex, CHAR_WORK_mode, 0);
      CHAR_setWorkInt(charaindex, CHAR_WORK_skill_level, 0);
      CHAR_setWorkInt(charaindex, CHAR_WORK_array, 0);
    }

#endif
    COM = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
    switch (COM) {
    case BATTLE_COM_ATTACK:
      if (gWeponType == ITEM_BOOMERANG) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_BOOMERANG);

#ifdef _PETSKILL_BECOMEFOX
        if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 101749 ||
            CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) !=
                -1) { //若是变成小狐狸不可以使用回力标
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        }
#endif
#ifdef _PETSKILL_BECOMEPIG
        if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1) { //变成乌力了
          gWeponType = ITEM_FIST;
          CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
        }
#endif
      }
      break;
    case BATTLE_COM_COMBO:
      if (ComboCheck2(EntryList, i, entrynum) == FALSE) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      }
      break;

    case BATTLE_COM_S_STATUSCHANGE:
      gBattleStausChange = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      gBattleStausTurn = CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3);
      break;

#ifdef _PETSKILL_HECTOR
    case BATTLE_COM_S_HECTOR:
      gBattleStausChange = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      gBattleStausTurn = CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3);
      break;
#endif

#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
    case BATTLE_COM_S_SARS:
      gBattleStausChange = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      gBattleStausTurn = CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3);
      break;
#endif

    case BATTLE_COM_S_CHARGE:
      BATTLE_Charge(battleindex, attackNo);
      break;
    case BATTLE_COM_S_RENZOKU:
      attack_max = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      gDamageDiv = attack_max;
      break;
#ifdef _VARY_WOLF
    case BATTLE_COM_S_VARY:
      // 先用不到
      break;
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
      //狂暴攻击 vincent add 2002/05/16
    case BATTLE_COM_S_WILDVIOLENTATTACK:
      attack_max = RAND(3, 10); //攻击次数
      gDamageDiv = attack_max;  //每次攻击造成伤害
      gBattleDuckModyfy =
          CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3); //命中率
      break;
#endif
    case BATTLE_COM_S_EARTHROUND0:
      gBattleDamageModyfy =
          1.0 + 0.01 * CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM3);
      break;
#ifdef _BATTLE_ATTCRAZED
    case BATTLE_COM_S_ATTCRAZED:
      attack_max = CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3);
#ifdef _PETSKILL_PLUS1
      //异常状态
      gBattleStausChange = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM6);
      gBattleStausTurn = CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM6);
      gBattleStausSuccess =
          CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM7);
#endif
      break;
#endif
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
    case BATTLE_COM_S_ATTSHOOT:
      attack_max = CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3);
      gDamageDiv = attack_max; //每次攻击造成伤害
      break;
#endif
    case BATTLE_COM_S_MIGHTY:
      gBattleDamageModyfy =
          CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3) * 0.01;
      gBattleDuckModyfy = CHAR_GETWORKINT_HIGH(
          charaindex, CHAR_WORKBATTLECOM3); //   歹今木膜恳骚橘尺
#ifdef _PETSKILL_PLUS1
      //异常状态
      gBattleStausChange = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM6);
      gBattleStausTurn = CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM6);
      gBattleStausSuccess =
          CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM7);
#endif
      break;
    }

    BATTLE_TargetListSet(charaindex, attackNo, aDefList);
    COM = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);

    ContFlg = FALSE;
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLG) &
            CHAR_BATTLEFLG_AIBAD &&
        COM == BATTLE_COM_COMBO) {
      // print( "由於忠诚度不足使用必杀技失败(%s)\n", CHAR_getUseName(
      // charaindex ) );
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
    }
#ifdef _BATTLE_TIMESPEED //动画播放速度
    {
      int flgtime = 0;
      switch (COM) {
#ifdef _BATTLE_ATTCRAZED
      case BATTLE_COM_S_ATTCRAZED:
        flgtime = 200 * 3;
        break;
#endif
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
      case BATTLE_COM_S_ATTSHOOT:
        flgtime = 80 * attack_max;
        break;
#endif
      case BATTLE_COM_JYUJYUTU:
      case BATTLE_COM_JYUJYUTU2:
      case BATTLE_COM_S_ATTACK_MAGIC:
        flgtime = 260; // 1/100sec
        break;
      case BATTLE_COM_COMBO:
        flgtime = 200; // 1/100sec
        break;
      case BATTLE_COM_GUARD:
        break;
      case BATTLE_COM_WAIT:
      default:
        flgtime = 150; // 1/100sec
        break;
      }
      BattleArray[battleindex].flgTime += flgtime;
    }
#endif
#ifdef _PETSKILL_BECOMEFOX // 中了媚惑术後攻防敏能力各降 20%
    if (CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) != -1) { //变成小狐狸
      CHAR_setWorkInt(charaindex, CHAR_WORKATTACKPOWER,
                      CHAR_getWorkInt(charaindex, CHAR_WORKFIXSTR) *
                          0.8); //攻降20%
      CHAR_setWorkInt(charaindex, CHAR_WORKDEFENCEPOWER,
                      CHAR_getWorkInt(charaindex, CHAR_WORKFIXTOUGH) *
                          0.8); //防降20%
      CHAR_setWorkInt(charaindex, CHAR_WORKQUICK,
                      CHAR_getWorkInt(charaindex, CHAR_WORKFIXDEX) *
                          0.8); //敏降20%
    }
#endif
    switch (COM) { //处理攻击

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    //==== 直接攻击系
    //===============================================================
    case BATTLE_COM_S_BRUST:       // 爆击
    case BATTLE_COM_S_CHAIN_ATK:   // 连环攻击
    case BATTLE_COM_S_CHAIN_ATK_2: // 双重攻击
    case BATTLE_COM_S_CAVALRY:     // 座骑攻击
    case BATTLE_COM_S_DEAD_ATTACK: // 濒死攻击
    case BATTLE_COM_S_ATTACK_WEAK: // 弱点攻击
    case BATTLE_COM_S_PLUNDER:     //   体掠夺
    case BATTLE_COM_S_CHAOS:       // 混乱攻击
#ifdef _PRO3_ADDSKILL
    case BATTLE_COM_S_CRACK_BONE: // 裂骨断筋
#endif
#ifdef _PREVENT_TEAMATTACK
    {
      // 防止同队互打
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      if (BATTLE_CheckSameSide(charaindex, defNo) == 1) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
    }
#endif
    case BATTLE_COM_S_ENRAGE_PET: // 激怒宠物
    {
      CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      battle_profession_attack_fun(battleindex, attackNo, defNo, charaindex);
      // change fix 修正使用职业得不到道具
      // BATTLE_AddProfit( battleindex, aAttackList);
    } break;
    //==== 魔法攻击系
    //================================================================
    case BATTLE_COM_S_VOLCANO_SPRINGS: // 火山泉
    case BATTLE_COM_S_FIRE_BALL:       // 火星球
    case BATTLE_COM_S_FIRE_SPEAR:      // 火龙枪
    case BATTLE_COM_S_SUMMON_THUNDER:  // 召雷术
    case BATTLE_COM_S_CURRENT:         // 电流术
    case BATTLE_COM_S_STORM:           // 暴风雨
    case BATTLE_COM_S_ICE_ARROW:       // 冰箭
    case BATTLE_COM_S_ICE_CRACK:       // 冰爆术
    case BATTLE_COM_S_ICE_MIRROR:      // 冰镜术
    case BATTLE_COM_S_DOOM:            // 世界末日
    case BATTLE_COM_S_BLOOD_WORMS:     // 嗜血蛊
    case BATTLE_COM_S_SIGN:            // 一针见血
    case BATTLE_COM_S_ENCLOSE:         // 附身术
    case BATTLE_COM_S_THROUGH_ATTACK:  // 贯穿攻击
    case BATTLE_COM_S_CONVOLUTE:       // 回旋攻击
    {
#ifdef _PROFESSION_ADDSKILL
      /*			if( COM == BATTLE_COM_S_DOOM //世界末日集气
                                      || COM == BATTLE_COM_S_FIRE_SPEAR){
                                      if( CHAR_getWorkInt( charaindex,
         CHAR_DOOMTIME ) > 0 ){ BATTLE_NoAction( battleindex, attackNo );
                                              sprintf( szWork, "bt|%x|",
         attackNo ); BATTLESTR_ADD( szWork ); break;
                                      }
                              }*/
#endif
#ifdef _PREVENT_TEAMATTACK
      // 防止同队互打
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      if (BATTLE_CheckSameSide(charaindex, defNo) == 1) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
#endif
    }
    case BATTLE_COM_S_BLOOD: // 嗜血成性
    {
      CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
#if 1 // Robin fix 修正嗜血吸别人的血
      if (defNo != attackNo && COM == BATTLE_COM_S_BLOOD) {
        print("\n 改封包!嗜血吸别人的血!!:%s ",
              CHAR_getChar(charaindex, CHAR_CDKEY));
        defNo = attackNo;
        CHAR_talkToCli(charaindex, -1, "因封包异常而断线。", CHAR_COLORYELLOW);
        CONNECT_setCloseRequest(CHAR_getWorkInt(charaindex, CHAR_WORKFD), 1);
        break;
      }
#endif
#ifdef _PROFESSION_ADDSKILL
#ifndef _NB_FIX_BATTLE_COM_S_BLOOD
      if (COM == BATTLE_COM_S_ICE_CRACK) {
        if (pBattle->ice_count >= 20)
          pBattle->ice_count = 0;
        pBattle->ice_use[pBattle->ice_count] = TRUE;
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
        pBattle->ice_level[pBattle->ice_count] =
            CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3);
        pBattle->ice_bout[pBattle->ice_count] = 2;
        pBattle->ice_toNo[pBattle->ice_count] =
            CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
        pBattle->ice_array[pBattle->ice_count] =
            CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
        pBattle->ice_charaindex[pBattle->ice_count] = charaindex;
        pBattle->ice_attackNo[pBattle->ice_count] = attackNo;
        ++pBattle->ice_count;
        BATTLE_NoAction(battleindex, attackNo);
        // print("\n进入冰爆");
        break;
      }
#endif
#endif
      battle_profession_attack_magic_fun(battleindex, attackNo, defNo,
                                         charaindex);
      // change fix 修正使用职业得不到道具
      // BATTLE_AddProfit( battleindex, aAttackList);
      break;
    }
    //==== 提升自已能力系
    //====================================================================
    case BATTLE_COM_S_FOCUS:     // 专注战斗
    case BATTLE_COM_S_AVOID:     // 回避
    case BATTLE_COM_S_RECOVERY:  // 补血
    case BATTLE_COM_S_SCAPEGOAT: // 舍已为友
    case BATTLE_COM_S_ENRAGE:    // 激化攻击
    case BATTLE_COM_S_COLLECT:   // 能量聚集
    case BATTLE_COM_S_TRAP:      // 陷阱
    case BATTLE_COM_S_TRANSPOSE: // 移形换位
    case BATTLE_COM_S_DOCILE:    // 驯伏宠物
#ifdef _PROFESSION_ADDSKILL
    case BATTLE_COM_S_CALL_NATURE: // 号召自然
#endif
    {
      CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      battle_profession_assist_fun(battleindex, attackNo, defNo, charaindex);
      break;
    }
    //==== 击中改变状态系
    //====================================================================
    case BATTLE_COM_S_SHIELD_ATTACK: // 盾击
    case BATTLE_COM_S_ENTWINE:       // 树根缠绕
    case BATTLE_COM_S_DRAGNET:       // 天罗地网
    case BATTLE_COM_S_TOXIN_WEAPON:  // 毒素武器
    case BATTLE_COM_S_INSTIGATE:     // 挑拨
    case BATTLE_COM_S_OBLIVION:      // 遗忘
#ifdef _PRO3_ADDSKILL
    case BATTLE_COM_S_POISON_DRAGNET: //毒素之网
    case BATTLE_COM_S_DARK_SEAL:      //?灵封印
    case BATTLE_COM_S_DEMON_SPEECH:   //恶魔诅咒
    case BATTLE_COM_S_PROAR:          //战狼怒吼
#endif
    {
#ifdef _PREVENT_TEAMATTACK
      // 防止同队互打
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      if (BATTLE_CheckSameSide(charaindex, defNo) == 1) {
        BATTLE_NoAction(battleindex, attackNo);
        if (COM == BATTLE_COM_S_TOXIN_WEAPON)
          printf("\nbug-同队互打");
        break;
      }
#endif
    }
    case BATTLE_COM_S_RESIST_FIRE:    // 火抗性提升
    case BATTLE_COM_S_RESIST_ICE:     // 冰抗性提升
    case BATTLE_COM_S_RESIST_THUNDER: // 雷抗性提升
#ifdef _PROFESSION_ADDSKILL
    case BATTLE_COM_S_RESIST_F_I_T: // 自然威能
    case BATTLE_COM_S_BOUNDARY:     // 四属性结界
#endif
    case BATTLE_COM_S_FIRE_ENCLOSE:    // 火附体
    case BATTLE_COM_S_ICE_ENCLOSE:     // 冰附体
    case BATTLE_COM_S_THUNDER_ENCLOSE: // 雷附体
#ifdef _PRO_KILLME
    case BATTLE_COM_S_KILLME:
#endif
#ifdef _PRO_ABSOLUTE_DEFENSE
    case BATTLE_COM_S_ABSOLUTE_DEFENSE:
#endif
#ifdef _PRO3_ADDSKILL
    case BATTLE_COM_S_PACIFY:     //抚慰心灵
    case BATTLE_COM_S_RESTATE:    //斗气导引,神灵之赐
    case BATTLE_COM_S_FIRST_AID:  //战场急救
    case BATTLE_COM_S_MEDICATION: //制药学
#endif
    {

      CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      battle_profession_status_chang_fun(battleindex, attackNo, defNo,
                                         charaindex);

      break;
    }
    //====================================================================================
#endif

#ifdef _PETSKILL_ACUPUNCTURE
    case BATTLE_COM_S_ACUPUNCTURE: //针刺外皮
      CHAR_setWorkInt(charaindex, CHAR_WORKACUPUNCTURE, 1);
      // print("\n针刺:%s",CHAR_getChar(charaindex,CHAR_NAME));
#endif
    case BATTLE_COM_S_CHARGE_OK:
    case BATTLE_COM_S_POWERBALANCE:
    case BATTLE_COM_S_STATUSCHANGE:
    case BATTLE_COM_S_EARTHROUND0:
    case BATTLE_COM_S_MIGHTY:
    case BATTLE_COM_S_GUARDIAN_ATTACK:
    case BATTLE_COM_S_RENZOKU:
#ifdef _BATTLE_ATTCRAZED // ANDY 疯狂暴走
    case BATTLE_COM_S_ATTCRAZED:
#endif
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
    case BATTLE_COM_S_ATTSHOOT:
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
    case BATTLE_COM_S_WILDVIOLENTATTACK: //狂暴攻击 vincent add 2002/05/16
#endif

#ifdef _SKILL_SPEEDY_ATT
    case BATTLE_COM_S_SPEEDYATTACK: //疾速攻击 vincent add 2002/05/20
#endif
    case BATTLE_COM_ATTACK:
#ifdef _PET_SKILL_SARS // WON ADD 毒煞蔓延
    case BATTLE_COM_S_SARS:
#endif

#ifdef _PETSKILL_BECOMEFOX
    case BATTLE_COM_S_BECOMEFOX:
#endif

#ifdef _PETSKILL_BECOMEPIG
    case BATTLE_COM_S_BECOMEPIG:
#endif

#ifdef _PETSKILL_SHOWMERCY
    case BATTLE_COM_S_SHOWMERCY:
#endif
#ifdef _MASSAGE_PETSKILL
    case BATTLE_COM_S_MASSAGE:
#endif
#ifdef _INVERSION_PETSKILL
    case BATTLE_COM_S_INVERSION:
#endif
#ifdef _PETOUT_PETSKILL
    case BATTLE_COM_S_PETOUT:
      if (COM == BATTLE_COM_S_PETOUT) {
        int defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
        int defindex = BATTLE_No2Index(battleindex, defNo);
        if (CHAR_getInt(defindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          int array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
          char *skillarg = PETSKILL_getChar(array, PETSKILL_OPTION);
          char key[32];
          char jl[32];
          getStringFromIndexWithDelim(skillarg, "|", 1, key, sizeof(key));
          getStringFromIndexWithDelim(skillarg, "|", 2, jl, sizeof(jl));

          char token[256];
          if ((rand() % 100) > atoi(jl)) {
            sprintf(token, "BK|%s 使用 %s 不成功|", CHAR_getUseName(defindex),
                    PETSKILL_getChar(array, PETSKILL_OPTION));
            strcat(szBadStatusString, token);
            break;
          }

          int petid;
          for (petid = 0; petid < CHAR_MAXPETHAVE; petid++) {
            if (!CHAR_CHECKINDEX(CHAR_getCharPet(defindex, petid)))
              continue;

            if ((strstr(skillarg, "休")) != NULL) {
              if (CHAR_getWorkInt(defindex, CHAR_WORK_PET0_STAT + petid) !=
                  PET_STAT_NONE) {
                continue;
              }
            } else if ((strstr(skillarg, "邮")) != NULL) {
              if (CHAR_getWorkInt(defindex, CHAR_WORK_PET0_STAT + petid) !=
                  PET_STAT_MAIL) {
                continue;
              }
            } else if ((strstr(skillarg, "待")) != NULL) {
              if (CHAR_getWorkInt(defindex, CHAR_WORK_PET0_STAT + petid) !=
                  PET_STAT_SELECT) {
                continue;
              }
            } else if ((strstr(skillarg, "骑")) != NULL) {
              if (CHAR_getInt(defindex, CHAR_RIDEPET) != petid)
                continue;
            }

            int pet_no = CHAR_getInt(defindex, CHAR_DEFAULTPET);
            if (pet_no != -1) {
              BATTLE_PetIn(battleindex, defNo);
              lssproto_SPET_send(getfdFromCharaIndex(defindex), defNo, TRUE);
            }
            if ((strstr(skillarg, "收")) != NULL)
              break;
            // shan End
            if (BATTLE_S_PetOut(battleindex, defNo, petid) == FALSE) {
              if (pet_no != -1) {
                BATTLE_S_PetOut(battleindex, defNo, pet_no);
              }
            }
            lssproto_KS_send(getfdFromCharaIndex(defindex),
                             CHAR_getInt(defindex, CHAR_DEFAULTPET), TRUE);
            break;
          }
        } else {
          CHAR_talkToCli(charaindex, -1, "该技能只能对玩家人物使用!",
                         CHAR_COLORYELLOW);
        }
        break;
      }
#endif
#ifdef _GRAPPLING_PETSKILL
    case BATTLE_COM_S_GRAPPLING:
#endif
#ifdef _PETSKILL_GYRATE
    case BATTLE_COM_S_GYRATE:
      if (COM == BATTLE_COM_S_GYRATE) {
        int i, j = 0, f_num, temp[5] = {0};
        char szWork[256];
        defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
#ifdef _PETSKILL_PLUS1
        //异常状态
        gBattleStausChange =
            CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM6);
        gBattleStausTurn =
            CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM6);
        gBattleStausSuccess =
            CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM7);
#endif
        if (defNo < 5)
          f_num = 0;
        else if (defNo >= 5 && defNo < 10)
          f_num = 5;
        else if (defNo >= 10 && defNo < 15)
          f_num = 10;
        else
          f_num = 15;

        // 取出战场上存活的人
        for (i = f_num; i < f_num + 5; i++) {
          if (BATTLE_TargetCheck(battleindex, i) != FALSE) {
            temp[j++] = i;
          }
        }
        if (j > 0) {
          sprintf(szWork, "BH|a%X|", attackNo); //下攻击指令
          BATTLESTR_ADD(szWork);
        }
        for (i = 0; i < j; i++) {
          // int atk_index = BATTLE_No2Index( battleindex, attackNo );
          // int def_index = BATTLE_No2Index( battleindex, temp[i] );

          BATTLE_Attack(battleindex, attackNo, temp[i]);
        }

        BATTLESTR_ADD("FF|");
        break;
      }
#endif

#ifdef _PETSKILL_FIREKILL
    case BATTLE_COM_S_FIREKILL:
      if (COM == BATTLE_COM_S_FIREKILL) {
        int i;
        defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
        //目标死亡,或不存在,或地球一周
        if (BATTLE_TargetCheck(battleindex, defNo) == FALSE ||
            CHAR_getWorkInt(BATTLE_No2Index(battleindex, defNo),
                            CHAR_WORKBATTLECOM1) == BATTLE_COM_S_EARTHROUND0) {
          //随意找一只打
          if (defNo < 10) {
            for (i = 0; i < 10; i++)
              if (BATTLE_TargetCheck(battleindex, i) &&
                  CHAR_getWorkInt(BATTLE_No2Index(battleindex, i),
                                  CHAR_WORKBATTLECOM1) !=
                      BATTLE_COM_S_EARTHROUND0) {
                defNo = i;
                break;
              }
          } else {
            for (i = 10; i < 20; i++)
              if (BATTLE_TargetCheck(battleindex, i) &&
                  CHAR_getWorkInt(BATTLE_No2Index(battleindex, i),
                                  CHAR_WORKBATTLECOM1) !=
                      BATTLE_COM_S_EARTHROUND0) {
                defNo = i;
                break;
              }
          }
          if (BATTLE_TargetCheck(battleindex, defNo) == FALSE ||
              CHAR_getWorkInt(BATTLE_No2Index(battleindex, defNo),
                              CHAR_WORKBATTLECOM1) ==
                  BATTLE_COM_S_EARTHROUND0) {
            return 0;
          }
        }
#ifdef _PREVENT_TEAMATTACK
        // 防止同队互打
        if (BATTLE_CheckSameSide(charaindex, defNo) == 1) {
          BATTLE_NoAction(battleindex, attackNo);
          CHAR_talkToCli(charaindex, -1, "不能对同队使用此技能!",
                         CHAR_COLORYELLOW);
          break;
        }
#endif
        //物理攻击减少为80%
        CHAR_setWorkInt(
            charaindex, CHAR_WORKATTACKPOWER,
            ((float)CHAR_getWorkInt(charaindex, CHAR_WORKFIXSTR) * 0.8));

        sprintf(szWork, "Bf|a%X|", attackNo);
        BATTLESTR_ADD(szWork);

        BATTLE_Attack_FIREKILL(battleindex, attackNo, defNo);

        BATTLE_MultiAttMagic_Fire(battleindex, attackNo, defNo, 2,
                                  200); // 2与200代表火属性魔法200伤害
        BATTLESTR_ADD("FF|");

        break;
      }
#endif
#ifdef _PETSKILL_HECTOR
    case BATTLE_COM_S_HECTOR:
      if (COM == BATTLE_COM_S_HECTOR) {
        int def_index = -1;

        defNo = CHAR_getWorkInt(charaindex,
                                CHAR_WORKBATTLECOM2); //取得被攻击者的位置
        def_index = BATTLE_No2Index(battleindex, defNo); //取得被攻击者的idx

#ifdef _PREVENT_TEAMATTACK
        // 防止同队互打
        if (BATTLE_CheckSameSide(charaindex, defNo) == 1) {
          BATTLE_NoAction(battleindex, attackNo);
          break;
        }
#endif
        if (!CHAR_CHECKINDEX(def_index))
          break;
        char *pszOption, *pszP;

        int skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
        pszOption = PETSKILL_getChar(skill, PETSKILL_OPTION);

        int fPer = 60;
        if ((pszP = strstr(pszOption, "命%")) != NULL) {
          sscanf(pszP + 3, "%d", &fPer);
        }

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
        if (PROFESSION_BATTLE_StatusAttackCheck(charaindex, def_index, 2,
                                                fPer)) {
#ifdef _FIX_PETSKILL_HECTOR
          char szTurn[] = "turn";
          int status = -1, i = 0, turn = 3;
          pszP = pszOption;
          for (; status == -1 && pszP[0] != 0; pszP++) {
            for (i = 1; i < BATTLE_ST_END; i++) {
              if (strncmp(pszP, aszStatus[i], 2) == 0) {
                status = i;
                pszP += 2;
                break;
              }
            }
          }

          if ((pszP = strstr(pszP, szTurn)) != NULL) {
            pszP += sizeof(szTurn);
            sscanf(pszP, "%d", &turn);
          }

          CHAR_setWorkInt(def_index, StatusTbl[status], turn); //设定麻痹
#else

          CHAR_setWorkInt(def_index, StatusTbl[2], 1); //设定麻痹

#endif
        }
#endif
      }
#endif
#ifdef _PETSKILL_ANTINTER
    case BATTLE_COM_S_ANTINTER:
      if (COM == BATTLE_COM_S_ANTINTER) {
        int skill;
        defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
        if (CHAR_getInt(BATTLE_No2Index(battleindex, defNo), CHAR_WHICHTYPE) ==
                CHAR_TYPEPET &&
            CHAR_getInt(BATTLE_No2Index(battleindex, defNo), CHAR_HP) <= 0) {
          skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
          ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                          BATTLE_COM_S_ANTINTER, skill);
          break;
        }
      }
#endif
#ifdef _RESURRECTION_PETSKILL
    case BATTLE_COM_S_RESURRECTION:
      if (COM == BATTLE_COM_S_RESURRECTION) {
        defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);

        if (CHAR_getInt(BATTLE_No2Index(battleindex, defNo), CHAR_HP) <= 0) {
          char szcommand[256];
          int skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
          char *pszP = PETSKILL_getChar(skill, PETSKILL_OPTION);
          int attackindex = BATTLE_No2Index(battleindex, attackNo);
          int hp = CHAR_getInt(attackindex, CHAR_HP) - atoi(pszP);
          CHAR_setInt(attackindex, CHAR_HP, atoi(pszP));

          if (atoi(pszP) == 0) {
            CHAR_setFlg(attackindex, CHAR_ISDIE, TRUE);
            CHAR_setWorkInt(attackindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
          }

          BATTLE_MultiRessurect(battleindex, attackNo, defNo, hp, 0,
                                MAGIC_EFFECT_USER, SPR_fukkatu3);
          if (CHAR_getInt(attackindex, CHAR_RIDEPET) != -1) {
            sprintf(szcommand, "BD|r%X|0|0|%X|p%X", attackNo, hp * 60, hp * 40);
            BATTLESTR_ADD(szcommand);
          } else {
            sprintf(szcommand, "BD|r%X|0|0|%X", attackNo, hp);
            BATTLESTR_ADD(szcommand);
          }
          break;
        }
      }
#endif

#ifdef _LOSTLOST_PETSKILL
    case BATTLE_COM_S_LOSTLOST:
      if (COM == BATTLE_COM_S_LOSTLOST) {
        char szCommand[256];
        defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);

        int defHp = CHAR_getInt(BATTLE_No2Index(battleindex, defNo), CHAR_HP);
        int attackHp =
            CHAR_getInt(BATTLE_No2Index(battleindex, attackNo), CHAR_HP);

        CHAR_setInt(BATTLE_No2Index(battleindex, attackNo), CHAR_HP, 1);
        CHAR_setInt(BATTLE_No2Index(battleindex, defNo), CHAR_HP, 1);

        snprintf(szCommand, sizeof(szCommand), "BH|a%X|r%X|0|d%X|FF|", attackNo,
                 defNo, defHp - 1);
        BATTLESTR_ADD(szCommand);

        sprintf(szCommand, "BD|r%X|0|0|%X|", attackNo, attackHp - 1);
        BATTLESTR_ADD(szCommand);
        break;
      }
#endif

#ifdef _PETSKILL_RETRACE
    case BATTLE_COM_S_RETRACE:
      if (COM == BATTLE_COM_S_RETRACE)
        CHAR_setWorkInt(charaindex, CHAR_WORKRETRACE, 1);
#endif
      CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);
      if (gWeponType == ITEM_BOW) {
        for (attack_count = 0; attack_count < 10; attack_count++) {
          defNo = aDefList[attack_count];
          if (attackNo == defNo)
            continue;
          if (BATTLE_TargetCheck(battleindex, defNo) == TRUE)
            break;
        }
        if (attack_count >= 10) {
          BATTLE_NoAction(battleindex, attackNo);
          break;
        }
        defNo = aDefList[0];
      } else if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex,
                                              myside)) < 0 ||
                 defNo == attackNo) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      if (COM == BATTLE_COM_S_EARTHROUND0) {
        sprintf(szWork, "BI|a%X|", attackNo);
      } else {
#ifdef _FIXBUG_ATTACKBOW
        if (CHAR_getWorkInt(charaindex, CHAR_WORKITEMMETAMO) > 0 ||
            CHAR_getWorkInt(charaindex, CHAR_WORKNPCMETAMO) > 0 ||
            CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) !=
                -1) { //若是变成小狐狸

          if (gWeponType == ITEM_BOW || gWeponType == ITEM_BOUNDTHROW ||
              gWeponType == ITEM_BREAKTHROW || gWeponType == ITEM_BOOMERANG) {
            CHAR_talkToCli(charaindex, -1, "变身中无法使用远距离武器!!",
                           CHAR_COLORYELLOW);
            continue;
          }
        }
#endif
        if (gWeponType == ITEM_BOW)
          sprintf(szWork, "BB|a%X|w0|", attackNo);
        else if (gWeponType == ITEM_BOUNDTHROW) //若是丢斧头
          sprintf(szWork, "BB|a%X|w1|", attackNo);
        else if (gWeponType == ITEM_BREAKTHROW)
          sprintf(szWork, "BB|a%X|w2|", attackNo);
        else
          sprintf(szWork, "BH|a%X|", attackNo);
      }
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
      if (COM == BATTLE_COM_S_ATTSHOOT)
        sprintf(szWork, "BB|a%X|w0|", attackNo);
#endif
      BATTLESTR_ADD(szWork);
      if (COM == BATTLE_COM_S_CHARGE_OK || COM == BATTLE_COM_S_EARTHROUND0) {
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      } else {
#ifdef _SHOOTCHESTNUT // Syu ADD 宠技：丢栗子
        if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) !=
            BATTLE_COM_S_ATTSHOOT)
#endif
#ifdef _PETSKILL_SHOWMERCY
          if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) !=
              BATTLE_COM_S_SHOWMERCY)
#endif
#ifdef _MASSAGE_PETSKILL
            if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) !=
                BATTLE_COM_S_MASSAGE)
#endif
#ifdef _GRAPPLING_PETSKILL
              if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) !=
                  BATTLE_COM_S_GRAPPLING)
#endif
#ifdef _INVERSION_PETSKILL
                if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1) !=
                    BATTLE_COM_S_INVERSION)
#endif

                  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1,
                                  BATTLE_COM_ATTACK);
      }

      for (attack_count = 0, k = 0;;) {
        if (gWeponType == ITEM_BOW) {
          if (BATTLE_TargetCheck(battleindex, defNo) == FALSE) {
            attack_flg = 0;
          } else {
            attack_flg = 1;
          }
        }

        if (attack_flg) {
          ContFlg = BATTLE_Attack(battleindex, attackNo, defNo);
#ifdef _PETSKILL_ADDATTCRAZED
          if (COM == BATTLE_COM_S_ATTCRAZED &&
              (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
               BATTLE_RET_MISS) //没有miss
              && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
                  BATTLE_RET_DODGE) //没有闪避过
              && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
                  BATTLE_RET_ALLGUARD) &&
              (BATTLE_TargetCheck(battleindex, defNo))) //还活着
            addattcrazedToList[attack_count] = defNo;
#endif
#ifdef _PETSKILL_RETRACE
          if (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData ==
                  BATTLE_RET_DODGE &&
              COM == BATTLE_COM_S_RETRACE) {
            if (RAND(1, 100) < 80) { // 80%机率发动2次攻击
              //攻+20%

              CHAR_setWorkInt(
                  BATTLE_No2Index(battleindex, attackNo), CHAR_WORKATTACKPOWER,
                  (CHAR_getWorkInt(BATTLE_No2Index(battleindex, attackNo),
                                   CHAR_WORKFIXSTR) +
                   (CHAR_getWorkInt(BATTLE_No2Index(battleindex, attackNo),
                                    CHAR_WORKFIXSTR) *
                    0.2)));

              BATTLE_Attack(battleindex, attackNo, defNo);
            }
            Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = 0;
          }
#endif
          BATTLE_AddProfit(battleindex, aAttackList);
          if (++attack_count >= attack_max)
            break;
          if (CHAR_getInt(charaindex, CHAR_HP) <= 0) {
            ContFlg = FALSE;
            break;
          }
        }
        defNo = aDefList[++k];
        if (defNo < 0)
          break;
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, defNo);
        if (gWeponType == ITEM_BOW) {
        } else if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex,
                                                myside)) < 0) {
          break;
        }
        if (BATTLE_CountAlive(battleindex, 0) == 0 ||
            BATTLE_CountAlive(battleindex, 1) == 0) {
          break;
        }
      }
      gBattleDamageModyfy = 1.0;
      gBattleDuckModyfy = 0;
      for (k = 0; k < 5 && ContFlg == TRUE; k++) {
        if ((k & 1) == 1) {
          attackNoSub = attackNo;
          defNoSub = defNo;
        } else {
          defNoSub = attackNo;
          attackNoSub = defNo;
        }
        ContFlg = BATTLE_Counter(battleindex, attackNoSub, defNoSub);
        aAttackList[0] = attackNoSub;
        aAttackList[1] = -1;
        BATTLE_AddProfit(battleindex, aAttackList);
      }
      aAttackList[0] = -1;
      BATTLESTR_ADD("FF|");

#ifdef _INVERSION_PETSKILL
      if (COM == BATTLE_COM_S_INVERSION) {
        if ((Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
             BATTLE_RET_MISS) //没有miss
            && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
                BATTLE_RET_DODGE) &&
            (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
             BATTLE_RET_ALLGUARD)                       //没有闪避过
            && (BATTLE_TargetCheck(battleindex, defNo)) //还活着
        ) {
          int flg, OnOff;
          int toindex = BATTLE_No2Index(battleindex, defNo);
          flg = CHAR_getWorkInt(toindex, CHAR_WORKBATTLEFLG);
          flg ^= CHAR_BATTLEFLG_REVERSE;
          CHAR_setWorkInt(toindex, CHAR_WORKBATTLEFLG, flg);
          OnOff = (flg & CHAR_BATTLEFLG_REVERSE) ? (1) : (0);
          BATTLE_AttReverse(toindex);
          sprintf(szWork, "FF|BR|%X|%X|FF|", defNo, OnOff);
          BATTLESTR_ADD(szWork);
        }
      }
#endif

#ifdef _PETSKILL_BECOMEFOX // 向对方使用媚惑术
      if ((COM == BATTLE_COM_S_BECOMEFOX) &&
          (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
           BATTLE_RET_MISS) //没有miss
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_DODGE) //没有闪避过
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_ALLGUARD)
#ifdef _EQUIT_ARRANGE
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_ARRANGE)
#endif
          && (BATTLE_TargetCheck(battleindex, defNo)) //还活着
          && (rand() % 100 < 31) &&
          (CHAR_getInt(BATTLE_No2Index(battleindex, defNo), CHAR_WHICHTYPE) !=
           CHAR_TYPEPLAYER) //只有宠物(CHAR_TYPEPET)才可以被媚惑
          && (CHAR_getWorkInt(BATTLE_No2Index(battleindex, defNo),
                              CHAR_WORK_PETFLG) != 0)
#ifdef _PETSKILL_BECOMEPIG
          && (CHAR_getInt(charaindex, CHAR_BECOMEPIG) == -1) //没有处於乌力化
#endif
      ) {
        int ToList[SIDE_OFFSET * 2 + 1];
        int defindex = BATTLE_No2Index(battleindex, defNo);

        BATTLE_MultiList(battleindex, defNo, ToList);
        BATTLE_MagicEffect(battleindex, defNo, ToList, 101120, 101750);

        CHAR_setWorkInt(defindex, CHAR_WORKFOXROUND, pBattle->turn);

        if (CHAR_getInt(defindex, CHAR_RIDEPET) != -1) { //骑宠就要让他落马
          CHAR_setInt(defindex, CHAR_RIDEPET, -1);
          BATTLE_changeRideImage(defindex);
          CHAR_setWorkInt(defindex, CHAR_WORKPETFALL, 1);
        }

        CHAR_setInt(defindex, CHAR_BASEIMAGENUMBER, 101749);
      }
#endif
#ifdef _PETSKILL_BECOMEPIG // 向对方使用乌力化
      if ((COM == BATTLE_COM_S_BECOMEPIG) &&
          (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
           BATTLE_RET_MISS) //没有miss
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_DODGE) //没有闪避过
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_ALLGUARD)
#ifdef _EQUIT_ARRANGE
          && (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
              BATTLE_RET_ARRANGE)
#endif
          && (BATTLE_TargetCheck(battleindex, defNo)) //还活着
          &&
          (CHAR_getInt(BATTLE_No2Index(battleindex, defNo), CHAR_WHICHTYPE) ==
           CHAR_TYPEPLAYER) //只有玩家才可以被乌力化
#ifdef _PREVENT_TEAMATTACK
          && (BATTLE_CheckSameSide(charaindex, defNo) != 1) // 防止同队互打
#endif
          && CHAR_getInt(BATTLE_No2Index(battleindex, defNo), CHAR_BECOMEPIG) <
                 2000000000) //防止破表
      {
        char temp[64];
        int ToList[SIDE_OFFSET * 2 + 1], compute, petrate, pettime,
            pigbbi = 100250;
        char *pszOption = NULL;
        int defindex = BATTLE_No2Index(battleindex, defNo);
        pszOption = PETSKILL_getChar(
            CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3),
            PETSKILL_OPTION);
        if (pszOption == "\0")
          sscanf(pszOption, "%d %d %d", &petrate, &pettime, &pigbbi);
        else
          petrate = 30, pettime = 60, pigbbi = 100250;

        if (rand() % 100 < petrate) {
          CHAR_setWorkInt(defindex, CHAR_WORKITEMMETAMO, 0); //道具变身取消
          CHAR_setWorkInt(defindex, CHAR_WORKNPCMETAMO, 0);  // NPC变身取消
          CHAR_setWorkInt(defindex, CHAR_WORKFOXROUND, -1); //媚惑术变身取消

          BATTLE_MultiList(battleindex, defNo, ToList);
          BATTLE_MagicEffect(battleindex, defNo, ToList, 101120, 101750);
#ifdef _PETSKILL_RIDE
          CHAR_setWorkInt(defindex, CHAR_WORK_RIDE,
                          MAKE2VALUE(CHAR_getInt(defindex, CHAR_RIDEPET),
                                     GETLOWVALUE(CHAR_getWorkInt(
                                         defindex, CHAR_WORK_RIDE))));
#endif
          if (CHAR_getInt(defindex, CHAR_RIDEPET) != -1) { //骑宠就要让他落马
            CHAR_setInt(defindex, CHAR_RIDEPET, -1);
            BATTLE_changeRideImage(defindex);
            CHAR_setWorkInt(defindex, CHAR_WORKPETFALL, 1);
          }

          CHAR_setInt(defindex, CHAR_BECOMEPIG_BBI, pigbbi);

          compute = CHAR_getInt(defindex, CHAR_BECOMEPIG);
          CHAR_setInt(defindex, CHAR_BASEIMAGENUMBER,
                      CHAR_getInt(defindex, CHAR_BECOMEPIG_BBI));
          if (compute == -1) //第一次中
            CHAR_setInt(defindex, CHAR_BECOMEPIG,
                        pettime + 1 + compute); //乌力化时间(单位:秒)
          else
            CHAR_setInt(defindex, CHAR_BECOMEPIG, pettime + compute);
          sprintf(temp, "乌力时间:%d秒", CHAR_getInt(defindex, CHAR_BECOMEPIG));
          CHAR_talkToCli(defindex, -1, temp, CHAR_COLORYELLOW);

          // print("\n乌力化开始0:%s,round:%d", CHAR_getChar( defindex,
          // CHAR_NAME), CHAR_getWorkInt( defindex, CHAR_WORKFOXROUND));
        }
      }
#endif

#ifdef _PETSKILL_ISULTIMATE
      if (COM == BATTLE_COM_S_STATUSCHANGE) {
        if (defNo <= 19) {
          char szWork[256];
          int temp = defNo, temp2 = 0, temp3;
          if (temp >= SIDE_OFFSET) {
            temp2 = 1;
            temp -= SIDE_OFFSET;
          } else {
            temp2 = 0;
          }
          if (BattleArray[battleindex].Side[temp2].Entry[temp].flg &
              BENT_FLG_ULTIMATE) { //是否被打飞
            for (temp3 = (temp2 * SIDE_OFFSET);
                 temp3 < ((temp2 * SIDE_OFFSET) + SIDE_OFFSET); temp3++) {
              if (BATTLE_TargetCheck(battleindex, temp3)) {
                int tempdefidx = BATTLE_No2Index(battleindex, temp3);
                // int tempprefrect = BATTLE_MD_NONE;
                int tempdamage = 0, temppetdamage = 0, temphp = 0;
                int tempdefpet = BATTLE_getRidePet(tempdefidx);
                if (CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM5) > 0) {
                  tempdamage =
                      (CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM5) /
                       100.0) *
                      CHAR_getInt(tempdefidx, CHAR_HP);
                  if (tempdefpet != -1)
                    temppetdamage =
                        (CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM5) /
                         100.0) *
                        CHAR_getInt(tempdefpet, CHAR_HP);
                } else if (CHAR_GETWORKINT_HIGH(charaindex,
                                                CHAR_WORKBATTLECOM5) > 0) {
                  tempdamage =
                      (CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM5) /
                       100.0) *
                      CHAR_getWorkInt(tempdefidx, CHAR_WORKMAXHP);
                  if (tempdefpet != -1)
                    temppetdamage =
                        (CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM5) /
                         100.0) *
                        CHAR_getWorkInt(tempdefpet, CHAR_WORKMAXHP);
                }
                // BATTLE_DamageSub( charaindex, tempdefidx, &tempdamage,
                // &temppetdamage, &tempprefrect );
                temphp = CHAR_getInt(tempdefidx, CHAR_HP) - tempdamage;
                if (temphp < 0)
                  temphp = 0;
                CHAR_setInt(tempdefidx, CHAR_HP, temphp);
                if (tempdefpet != -1) {
                  temphp = CHAR_getInt(tempdefpet, CHAR_HP) - temppetdamage;
                  if (temphp <= 0) {
                    temphp = 0;
                    CHAR_setInt(tempdefidx, CHAR_RIDEPET, -1);
                    BATTLE_changeRideImage(tempdefidx);
                    CHAR_setWorkInt(tempdefidx, CHAR_WORKPETFALL, 1);
                  }
                  CHAR_setInt(tempdefpet, CHAR_HP, temphp);
                }

                if (tempdamage > 0 || temppetdamage > 0) {
                  sprintf(szWork, "BD|r%X|%X|0|d%X|p%X|", temp3, 0, tempdamage,
                          temppetdamage);
                  BATTLESTR_ADD(szWork);
                }
              }
            }
            CHAR_SETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM5, 0);
            CHAR_SETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM5, 0);
          }
        }
      }
#endif

#ifdef _PETSKILL_ADDATTCRAZED
      if (COM == BATTLE_COM_S_ATTCRAZED
          //&& (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
          //BATTLE_RET_MISS)  //没有miss
          //&& (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
          //BATTLE_RET_DODGE) //没有闪避过
          //&& (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
          //BATTLE_RET_ALLGUARD)
          //&& (Battle_Attack_ReturnData_x.Battle_Attack_ReturnData !=
          //BATTLE_RET_ARRANGE)
          //&& (BATTLE_TargetCheck(battleindex, defNo)) //还活着
      ) {
        int array = -1, temp2, temploop;
        int defindex; // = BATTLE_No2Index(battleindex, defNo);
        char *pszOption = NULL, temp[64] = "", szWork[256];
        array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
        pszOption = PETSKILL_getChar(array, PETSKILL_OPTION);
        if (pszOption == NULL) {
          break;
        }
        strcpy(temp, pszOption);
        for (temploop = 0; temploop < 10; temploop++) {
          if (addattcrazedToList[temploop] == -1 ||
              !BATTLE_TargetCheck(battleindex, addattcrazedToList[temploop]))
            continue;
          defindex = BATTLE_No2Index(battleindex, addattcrazedToList[temploop]);
          if (CHAR_getWorkInt(defindex, CHAR_WORK_ADDATTCRAZED))
            continue;
          if ((pszOption = strstr(temp, "Dsd%"))) {
            pszOption += 4;
            sscanf(pszOption, "%d", &temp2);
            CHAR_setWorkInt(defindex, CHAR_WORK_ADDATTCRAZED, 4);      //回合
            CHAR_setWorkInt(defindex, CHAR_WORK_ADDATTCRAZED2, temp2); //值
            int old_dex = CHAR_getWorkInt(defindex, CHAR_WORKFIXDEX);
            CHAR_setWorkInt(defindex, CHAR_WORKFIXDEX,
                            old_dex + old_dex * (temp2 / 100.0));
            sprintf(szWork, "BD|r%X|1|3|%X|", addattcrazedToList[temploop],
                    temp2);
            BATTLESTR_ADD(szWork);
          }
          if ((pszOption = strstr(temp, "Dmp%"))) {
            int mptemp, mp = CHAR_getInt(defindex, CHAR_MP);
            pszOption += 4;
            sscanf(pszOption, "%d", &temp2);
            if (CHAR_getInt(defindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
                CHAR_getInt(defindex, CHAR_HP) > 0 && mp > 0) {
              sprintf(szWork, "BD|r%X|1|0|m%X|", addattcrazedToList[temploop],
                      mptemp = mp * (temp2 / 100.0));
              BATTLESTR_ADD(szWork);
              CHAR_setInt(defindex, CHAR_MP, mp - mptemp);
            }
          }
        }
      }
#endif
      break;
      //以下处理特殊攻击
    case BATTLE_COM_BOOMERANG:
      gBattleDamageModyfy = 0.3;
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      if (defNo < 0) {
        defNo = BATTLE_DefaultAttacker(battleindex, 1 - myside);
      }
      if (0 <= defNo && defNo <= 19) {
        defNo /= 5; //
      } else {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      if ((int)(attackNo / 5) == defNo) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      {
        int *pBoomerangTbl = BoomerangVsTbl[defNo];
        for (k = 0; k < 5; k++) {
          if (BATTLE_TargetCheck(battleindex, pBoomerangTbl[k]) == FALSE) {
            continue;
          } else {
            break;
          }
        }
        if (k == 5) {
          defNo = BATTLE_DefaultAttacker(battleindex, 1 - myside);
          if (0 <= defNo && defNo <= 19) {
            defNo /= 5;
          } else {
            BATTLE_NoAction(battleindex, attackNo);
            break;
          }
        }
      }
      sprintf(szWork, "BO|a%X|", attackNo);
      BATTLESTR_ADD(szWork);
      {
        int *pBoomerangTbl = BoomerangVsTbl[defNo];
        if (myside == 1) {
          k = 4;
          j = -1;
        } else {
          k = 0;
          j = 1;
        }
        for (;; k += j) {
          if (j > 0 && k > 4)
            break;
          if (j < 0 && k < 0)
            break;
          defNo = pBoomerangTbl[k];
          if (BATTLE_TargetCheck(battleindex, defNo) == FALSE) {
            continue;
          }
          BATTLE_Attack(battleindex, attackNo, defNo);
        }
      }
      gBattleDamageModyfy = 1.0;
      gBattleDuckModyfy = 0;
      BATTLESTR_ADD("FF|");
      break;

    //---------------------------------------------
    case BATTLE_COM_CAPTURE:
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      BATTLE_Capture(battleindex, attackNo, defNo);
      break;

      //---------------------------------------------
#ifdef _VARY_WOLF
    case BATTLE_COM_S_VARY: {
      int ToList[SIDE_OFFSET * 2 + 1];
      int bk_amn = 0;

      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
#ifdef _EXPANSION_VARY_WOLF
      if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 101428) {
        bk_amn = (CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER) + 5) +
                 (myside * 4);
      } else {
        bk_amn = (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER));
      }

      BATTLE_MultiList(battleindex, defNo, ToList);
      BATTLE_MagicEffect(battleindex, attackNo, ToList, 101120, bk_amn);
#endif
    } break;
#endif
#ifdef _PETSKILL_PROVOKEFIGHT
    case BATTLE_COM_S_PROVOKEFIGHT: {
      int ToList[SIDE_OFFSET * 2 + 1], tempbasebase, PlayerIndex;
      char szcommand[32], *pszOption = NULL, tempbuf1[8] = "1",
                          tempbuf2[8] = "0";

      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      BATTLE_MultiList(battleindex, defNo, ToList);

      PlayerIndex = CHAR_getWorkInt(charaindex, CHAR_WORKPLAYERINDEX);
      if (!CHAR_CHECKINDEX(PlayerIndex)) {
        printf(
            "\nBATTLE_PetDropEquip():不能由宠物的 index 找出人物的 index!!\n");
      }

      if (BATTLE_TargetCheck(battleindex, defNo) == FALSE) {
        CHAR_talkToCli(PlayerIndex, -1, "挑衅失败!", CHAR_COLORYELLOW);
        break;
      }

      BATTLE_MagicEffect(battleindex, attackNo, ToList, 101120, 101679);
      pszOption =
          PETSKILL_getChar(CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3),
                           PETSKILL_OPTION);
      if (getStringFromIndexWithDelim(pszOption, "|", 1, szcommand,
                                      sizeof(szcommand))) {
        if (rand() % 100 > atoi(szcommand)) {
          CHAR_talkToCli(PlayerIndex, -1, "挑衅失败!", CHAR_COLORYELLOW);
          break;
        }
      }
      getStringFromIndexWithDelim(pszOption, "|", 2, tempbuf1,
                                  sizeof(tempbuf1)); //回合数
      getStringFromIndexWithDelim(pszOption, "|", 3, tempbuf2,
                                  sizeof(tempbuf2));

      sprintf(szcommand, "BD|r%X|0|A|%X|", attackNo, 0);
      BATTLESTR_ADD(szcommand);
      sprintf(szcommand, "BD|r%X|0|A|%X|FF|", defNo, 1);
      BATTLESTR_ADD(szcommand);
      CHAR_setWorkInt(BATTLE_No2Index(battleindex, defNo),
                      CHAR_WORK_PROVOKEFIGHT,
                      MAKE4VALUE(0, atoi(tempbuf1), atoi(tempbuf2), attackNo));
      tempbasebase = CHAR_getWorkInt(charaindex, CHAR_WORK_PROVOKEFIGHT);
      //使用此技能后,打上编号3,之后中此技的角色只能打有打编号的角色
      CHAR_setWorkInt(charaindex, CHAR_WORK_PROVOKEFIGHT,
                      MAKE4VALUE(3, GETTHIRDVALUE(tempbasebase),
                                 GETSECONDVALUE(tempbasebase),
                                 GETFIRSTVALUE(tempbasebase)));
    } break;
#endif

#ifdef _PETSKILL_RIDE
    case BATTLE_COM_S_RIDE: {
      int ToList[SIDE_OFFSET * 2 + 1], ridedefidx = -1, k = 3;
      char szBuffer[256], *pszOption = NULL;
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      if (defNo > 19)
        break;
      ridedefidx = BATTLE_No2Index(battleindex, defNo);
      if (ridedefidx == -1)
        break;
      if (GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_RIDE)) > 0) {
        ridedefidx = CHAR_getWorkInt(charaindex, CHAR_WORKPLAYERINDEX);
        sprintf(szBuffer, "上马术于 %d 回合后才可使用。",
                GETLOWVALUE(CHAR_getWorkInt(charaindex, CHAR_WORK_RIDE)));
        CHAR_talkToCli(ridedefidx, -1, szBuffer, CHAR_COLORYELLOW);
        break;
      }
      BATTLE_MultiList(battleindex, defNo, ToList);
      BATTLE_MagicEffect(battleindex, attackNo, ToList, 101120, 101679);
      pszOption =
          PETSKILL_getChar(CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3),
                           PETSKILL_OPTION);
      if (pszOption && strlen(pszOption) > 0)
        k = atoi(pszOption);
      Battle_RidePet(ridedefidx, defNo, charaindex, k);
    } break;
#endif

    case BATTLE_COM_GUARD:
      BATTLE_Guard(battleindex, attackNo);

      break;
    case BATTLE_COM_ESCAPE:
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
        if (BATTLE_Escape(battleindex, attackNo, 0) == TRUE) {
          if (CHAR_CHECKINDEX(charaindex)) {
            if (CHAR_getWorkInt(charaindex, CHAR_WORKFMPKFLAG) == 1) {
              CHAR_setWorkInt(charaindex, CHAR_WORKFMPKFLAG, -1);
            }
          }
        }
      }
      break;
    case BATTLE_COM_COMPELESCAPE: //强制离开
      if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
        BATTLE_Escape(battleindex, attackNo, 1);
        if (CHAR_getWorkInt(charaindex, CHAR_WORKFMPKFLAG) == 1)
          CHAR_setWorkInt(charaindex, CHAR_WORKFMPKFLAG, -1);
      }
      break;

    case BATTLE_COM_PETIN:
      if (CHAR_getInt(charaindex, CHAR_DEFAULTPET) == -1) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      {
        char szWork[256];
        snprintf(szWork, sizeof(szWork), "K%d",
                 CHAR_getInt(charaindex, CHAR_DEFAULTPET));
        CHAR_sendStatusString(charaindex, szWork);
      }
      BATTLE_PetIn(battleindex, attackNo);
      lssproto_KS_send(getfdFromCharaIndex(charaindex),
                       CHAR_getInt(charaindex, CHAR_DEFAULTPET), 1);
      break;
    case BATTLE_COM_PETOUT:
      if (CHAR_getInt(charaindex, CHAR_DEFAULTPET) != -1) {
        lssproto_KS_send(getfdFromCharaIndex(charaindex),
                         CHAR_getInt(charaindex, CHAR_DEFAULTPET), 1);
        {
          char szWork[256];
          snprintf(szWork, sizeof(szWork), "K%d",
                   CHAR_getInt(charaindex, CHAR_DEFAULTPET));
          CHAR_sendStatusString(charaindex, szWork);
        }

        BATTLE_PetIn(battleindex, attackNo);
      }
      /*
                              // shan 2002/01/14 Begin  五只宠
                              {
                                      int attackindex, petNo;

                                      attackindex = BATTLE_No2Index(
         battleindex, attackNo ); petNo = CHAR_getWorkInt( attackindex,
         CHAR_WORKBATTLECOM2 ); if( CHAR_getWorkInt(charaindex,
         CHAR_WORK_PET0_STAT+petNo) != PET_STAT_SELECT) break;
                              }
      */
      // shan End
      BATTLE_PetOut(battleindex, attackNo);
      lssproto_KS_send(getfdFromCharaIndex(charaindex),
                       CHAR_getInt(charaindex, CHAR_DEFAULTPET), 1);
      break;
// Terry add 2001/11/05
#ifdef _ATTACK_MAGIC
    case BATTLE_COM_S_ATTACK_MAGIC: // 宠物魔法

      magic = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      toindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      //			print("magic->%d,toindex->%d\n",magic,toindex);
      // 判断攻    围
      for (magic_count = 0; magic_count < MAX_MAGIC_NUMBER; magic_count++) {
        // 比对魔法的id值来找出魔法的攻击  围
        if (magic == TargetIndex[magic_count][0]) {
          // 如果是打全体的魔法
          if (TargetIndex[magic_count][1] == 20) {
#ifdef _OPEN_E_PETSKILL
            if (toindex >= 10) {
              toindex = 21;
            } else {
              toindex = 20;
            }
#else
            toindex = TargetIndex[magic_count][1];
#endif
          }
          // 如果不是打单人的魔法
          else if (TargetIndex[magic_count][1] != -1) {
            // 如果是打右下的後面那一排
            if (toindex >= 0 && toindex <= 4)
              toindex = TargetIndex[magic_count][1];
            // 打右下的前面那一排
            else
              toindex = TargetIndex[magic_count][1] - 1;
          }
          break;
        }
      }
      //			print("toindex->%d\n",toindex);
      MAGIC_DirectUse(charaindex, magic, toindex,
                      CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3));
      BATTLE_AddProfit(battleindex, aAttackList);
      break;
#endif
    case BATTLE_COM_JYUJYUTU:
      MAGIC_DirectUse(charaindex,
                      CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3),
                      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2),
                      CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3));
      break;

    case BATTLE_COM_JYUJYUTU2:
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                      BATTLE_ToRandNo(battleindex, 1));
      MAGIC_DirectUse(charaindex,
                      CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3),
                      CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2),
                      CHAR_GETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3));
      break;

    case BATTLE_COM_ITEM:
      itemindex = CHAR_getItemIndex(
          charaindex, CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM3));
      if (ITEM_CHECKINDEX(itemindex) == FALSE) {
        print("\nerr:要使用的item位置奇怪(%s)(%d)\n",
              CHAR_getUseName(charaindex),
              CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM3));
        break;
      }
/*
                        {
                                int itemtype = ITEM_getInt( itemindex, ITEM_TYPE
   ); if( itemtype == ITEM_HELM
                                        || itemtype == ITEM_ARMOUR
                                        || itemtype == ITEM_BRACELET
                                        //|| itemtype == ITEM_MUSIC
                                        || itemtype == ITEM_NECKLACE
                                        || itemtype == ITEM_RING
                                        || itemtype == ITEM_BELT
                                        || itemtype == ITEM_EARRING
                                        || itemtype == ITEM_NOSERING
                                        || itemtype == ITEM_AMULET
                                        //|| itemtype == CHAR_HEAD
                                        )
                                {
                                        print("\n 改封包!战斗不可换防具!!:%s ",
   CHAR_getChar( charaindex, CHAR_CDKEY) ); break;
                                }
                        }
*/
#ifdef _FIXBUG_ATTACKBOW
      {
        if (CHAR_getWorkInt(charaindex, CHAR_WORKITEMMETAMO) > 0 ||
            CHAR_getWorkInt(charaindex, CHAR_WORKNPCMETAMO) > 0 ||
            CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) !=
                -1) { //若是变成小狐狸

          int armtype;
          int itemindex = CHAR_getItemIndex(
              charaindex, CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM3));
          if (!ITEM_CHECKINDEX(itemindex))
            break;
          armtype = ITEM_getInt(itemindex, ITEM_TYPE);

          if (armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
              armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG) {
            CHAR_talkToCli(charaindex, -1, "变身无法使用远距离武器！",
                           CHAR_COLORYELLOW);
            break;
          }
        }
      }
#endif
      CHAR_ItemUse(charaindex, CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2),
                   CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM3));

      int itemtype = ITEM_getEquipPlace(charaindex, itemindex);
      if (itemtype == CHAR_ARM || itemtype == CHAR_HEAD ||
          itemtype == CHAR_BODY || itemtype == CHAR_DECORATION1 ||
          itemtype == CHAR_DECORATION2) {
        sprintf(szWork, "BN|a%X|%X|", attackNo,
                CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER));
        BATTLESTR_ADD(szWork);

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
        // 武器专精
        BATTLE_ProfessionStatus_init(battleindex, charaindex);
#endif
      }

      break;

#ifdef _PSKILL_FALLGROUND
    case BATTLE_COM_S_FALLRIDE: //落马术
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      ContFlg = BATTLE_S_FallGround(battleindex, attackNo, defNo,
                                    BATTLE_COM_S_FALLRIDE);
      break;

#endif
#ifdef _PETSKILL_EXPLODE
    case BATTLE_COM_S_EXPLODE:
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      ContFlg =
          BATTLE_S_Explode(battleindex, attackNo, defNo, BATTLE_COM_S_EXPLODE);
      break;
#endif

#ifdef _PETSKILL_TIMID
    case BATTLE_COM_S_TIMID: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_TIMID, skill);
    } break;
#endif
#ifdef _PETSKILL_2TIMID
    case BATTLE_COM_S_2TIMID: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_2TIMID, skill);
    } break;
#endif

#ifdef _PETSKILL_PROPERTY
    case BATTLE_COM_S_PROPERTYSKILL: {
      int skill;
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      BATTLE_S_PetSkillProperty(battleindex, attackNo, COM, skill);
      BATTLE_NoAction(battleindex, attackNo);
    } break;
#endif

#ifdef _BATTLE_LIGHTTAKE
    case BATTLE_COM_S_LIGHTTAKE: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_LIGHTTAKE, skill);
    } break;
#endif

#ifdef _SKILL_DAMAGETOHP
    case BATTLE_COM_S_DAMAGETOHP: //嗜血技
    {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_DAMAGETOHP, skill);
    } break;
#endif

#ifdef _PETSKILL_DAMAGETOHP
    case BATTLE_COM_S_DAMAGETOHP2: //暗月狂狼(嗜血技的变体)
    {
      int skill, attackidxtemp;
      attackidxtemp = BATTLE_No2Index(battleindex, attackNo);

      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_DAMAGETOHP2, skill);

    } break;
#endif

#ifdef _Skill_MPDAMAGE
    case BATTLE_COM_S_MPDAMAGE: // MP伤害
    {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_MPDAMAGE, skill);
      break;
    } break;
#endif
#ifdef _SKILL_TOOTH
    case BATTLE_COM_S_TOOTHCRUSHE: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_TOOTHCRUSHE, skill);
      break;
    } break;
#endif
#ifdef _PSKILL_MODIFY
    case BATTLE_COM_S_MODIFYATT: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_MODIFYATT, skill);
    } break;
#endif
#ifdef _PSKILL_MDFYATTACK
    case BATTLE_COM_S_MDFYATTACK: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_MDFYATTACK, skill);
    } break;
#endif

#ifdef _PETSKILL_TEAR
    case BATTLE_COM_S_PETSKILLTEAR: {
      int skill;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_PETSKILLTEAR, skill);
    } break;
#endif

#ifdef _SONIC_ATTACK // WON ADD 音波攻击
    case BATTLE_COM_S_SONIC: {
      int skill, defNo2 = -1, index2 = -1;

      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);

      if (defNo >= 15 && defNo < 20)
        defNo2 = defNo - 5;
      else if (defNo >= 5 && defNo < 10)
        defNo2 = defNo - 5;
      else
        defNo2 = -1;

      battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
      index2 = BATTLE_No2Index(battleindex, defNo2);

      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_SONIC, skill);

      if (index2 >= 0) {
        if (defNo2 >= 0) {
          ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo2,
                                          BATTLE_COM_S_SONIC2, skill);
        }
      }

    } break;
#endif

#ifdef _PETSKILL_REGRET
    case BATTLE_COM_S_REGRET: {
      int skill, defNo2 = -1, index2 = -1;

      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);

      if (defNo >= 15 && defNo < 20)
        defNo2 = defNo - 5;
      else if (defNo >= 5 && defNo < 10)
        defNo2 = defNo - 5;
      else
        defNo2 = -1;

      battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
      index2 = BATTLE_No2Index(battleindex, defNo2);

      ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo,
                                      BATTLE_COM_S_REGRET, skill);
      if (index2 >= 0) {
        if (defNo2 >= 0) {
          ContFlg = BATTLE_S_AttackDamage(battleindex, attackNo, defNo2,
                                          BATTLE_COM_S_REGRET2, skill);
        }
      }

    } break;
#endif

#ifdef _MAGIC_SUPERWALL
    case BATTLE_COM_S_SUPERWALL: {
      int skill;
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = PETSKILL_MagicStatusChange_Battle(battleindex, attackNo, defNo,
                                                  skill);
    } break;
#endif
#ifdef _PETSKILL_SETDUCK
    case BATTLE_COM_S_SETDUCK: {
      int skill;
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg =
          PETSKILL_SetDuckChange_Battle(battleindex, attackNo, defNo, skill);
      break;
    }
#endif
#ifdef _MAGICPET_SKILL
    case BATTLE_COM_S_SETMAGICPET: {
      int skill;
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg =
          PETSKILL_SetMagicPet_Battle(battleindex, attackNo, defNo, skill);
      break;
    }
#endif
#ifdef _STRENGTH_PETSKILL
    case BATTLE_COM_S_STRENGTH: {
      int skill;
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      skill = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg =
          PETSKILL_SetStrength_Battle(battleindex, attackNo, defNo, skill);
      break;
    }
#endif
#ifdef _OFFLINE_SYSTEM
    case BATTLE_COM_S_OFFLINE_RECOVERY: {
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      int HP = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = OFFLINE_MultiRessurect(battleindex, attackNo, defNo, HP, 0);
      break;
    }
#endif
#ifdef _PRO_BATTLEENEMYSKILL
    case BATTLE_COM_S_ENEMYRELIFE:
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }

      ContFlg = BATTLE_E_ENEMYREFILE(battleindex, attackNo, defNo,
                                     BATTLE_COM_S_ENEMYRELIFE);
      if (ContFlg == FALSE) {
        CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);
        ContFlg = BATTLE_Attack(battleindex, attackNo, defNo);
        BATTLE_AddProfit(battleindex, aAttackList);
      }
      BATTLE_AddProfit(battleindex, aAttackList);
      break;

    case BATTLE_COM_S_ENEMYREHP:
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      ContFlg = BATTLE_E_ENEMYREHP(battleindex, attackNo, defNo,
                                   BATTLE_COM_S_ENEMYREHP);
      if (ContFlg == FALSE) {
        CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);
        ContFlg = BATTLE_Attack(battleindex, attackNo, defNo);
        BATTLE_AddProfit(battleindex, aAttackList);
      }
      BATTLE_AddProfit(battleindex, aAttackList);
      break;

    case BATTLE_COM_S_ENEMYHELP:
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        print("\n BATTLE_NoAction()");
        break;
      }
      ContFlg = BATTLE_E_ENEMYHELP(battleindex, attackNo, defNo,
                                   BATTLE_COM_S_ENEMYREHP);
      if (ContFlg == FALSE) {
        CHAR_setFlg(charaindex, CHAR_ISATTACKED, 1);
        ContFlg = BATTLE_Attack(battleindex, attackNo, defNo);
        BATTLE_AddProfit(battleindex, aAttackList);
      }
      BATTLE_AddProfit(battleindex, aAttackList);
      break;
#endif
    case BATTLE_COM_S_GBREAK: //破除防御

      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      ContFlg = BATTLE_S_GBreak(battleindex, attackNo, defNo);
      break;
#ifdef _SKILL_GUARDBREAK2 //破除防御2 vincent add 2002/05/20
    case BATTLE_COM_S_GBREAK2:
      //确定攻击对象
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      ContFlg = BATTLE_S_GBreak2(battleindex, attackNo, defNo);
      break;
#endif

#ifdef _SKILL_SACRIFICE
    case BATTLE_COM_S_SACRIFICE: //救援 vincent add 2002/05/30
                                 //确定攻击对象
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      ContFlg = BATTLE_S_Sacrifice(battleindex, attackNo, defNo);

      break;
#endif

#ifdef _SKILL_REFRESH
    case BATTLE_COM_S_REFRESH: //状态回复 vincent add 2002/08/08
    {
      int array;
      array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      //			print("\n vincent--case
      //BATTLE_COM_S_REFRESH:-->defNo:%d",defNo);
      ContFlg = BATTLE_S_Refresh(battleindex, attackNo, defNo, array);
    } break;
#endif

#ifdef _SKILL_WEAKEN // vincent宠技:虚弱
    case BATTLE_COM_S_WEAKEN: {
      int array;
      array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      // print("\n vincent--case BATTLE_COM_S_WEAKEN:-->defNo:%d",defNo);
      ContFlg = BATTLE_S_Weaken(battleindex, attackNo, defNo, array);
    } break;
#endif
#ifdef _PETSKILL_PLUS1
    case BATTLE_COM_S_PLUS1: {
      int array;
      array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      ContFlg = BATTLE_S_Plus1(battleindex, attackNo, defNo, array);
    } break;
#endif
#ifdef _SKILL_DEEPPOISON // vincent宠技:剧毒
    case BATTLE_COM_S_DEEPPOISON: {
      int array;
      array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      ContFlg = BATTLE_S_Deeppoison(battleindex, attackNo, defNo, array);
    } break;
#endif

#ifdef _SKILL_BARRIER // vincent宠技:魔障
    case BATTLE_COM_S_BARRIER: {
      int array;
      array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      ContFlg = BATTLE_S_Barrier(battleindex, attackNo, defNo, array);
    } break;
#endif

#ifdef _SKILL_NOCAST // vincent宠技:沉默
    case BATTLE_COM_S_NOCAST: {
      int array;
      array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      defNo = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM2);
      ContFlg = BATTLE_S_Nocast(battleindex, attackNo, defNo, array);
    } break;
#endif

#ifdef _SKILL_ROAR // vincent宠技:大吼
    case BATTLE_COM_S_ROAR: {
      int array;
      //确定攻击对象
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      ContFlg = BATTLE_S_Roar(battleindex, attackNo, defNo, array);
    } break;
#endif
    case BATTLE_COM_S_ABDUCT: {
      int array;
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      array = CHAR_GETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3);
      BATTLE_Abduct(battleindex, attackNo, defNo, array);
    } break;
    case BATTLE_COM_COMBO:
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      gDamageDiv = 1.0;
      ComboId = EntryList[i].combo;
      aAttackList[0] = EntryList[i].num; // 伉旦玄卞笛尹月
      i++;                               // 戚及谛井日
      k = 1;
      for (; EntryList[i].combo == ComboId && i < entrynum; i++) {
        int charmode;
        // 绣箕允月井民尼永弁
        if (CHAR_CHECKINDEX(EntryList[i].charaindex) == FALSE)
          continue;

        // 爵  卞辅笛仄化中卅中桦宁反戚尺
        charmode =
            CHAR_getWorkInt(EntryList[i].charaindex, CHAR_WORKBATTLEMODE);
        if (charmode == 0 || charmode == BATTLE_CHARMODE_FINAL) {
          continue;
        }

        // 旦  □正旦唱橘荚汊↓  质
        BATTLE_StatusSeq(EntryList[i].charaindex);
#ifdef _OTHER_MAGICSTAUTS
        BATTLE_MagicStatusSeq(EntryList[i].charaindex);
#endif
        //   仃卅井匀凶桦宁
        if (BATTLE_CanMoveCheck(EntryList[i].charaindex) == FALSE) {
          continue;
        }
        if (CHAR_getInt(EntryList[i].charaindex, CHAR_HP) <= 0) {
          continue;
        }
        aAttackList[k++] = EntryList[i].num;
      }
      aAttackList[k] = -1;
      i--;
      if (k == 1) {
        print("\nerr:一人combo bug\n");
      }
      sprintf(szWork, "BY|r%X|", defNo);
      BATTLESTR_ADD(szWork);
      BATTLE_Combo(battleindex, aAttackList, defNo);

#ifdef _Item_ReLifeAct
      BATTLE_AddProfit(battleindex, aAttackList);
#endif
      //     反 FF 毛仁匀勾仃月
      BATTLESTR_ADD("FF|");
      break;

    //---------------------------------------------
    case BATTLE_COM_WAIT:
    case BATTLE_COM_NONE:
      BATTLE_NoAction(battleindex, attackNo);
      break;

    //---------------------------------------------
    case BATTLE_COM_S_EARTHROUND1:
      BATTLE_EarthRoundHide(battleindex, attackNo);
      break;

    //---------------------------------------------
    case BATTLE_COM_S_LOSTESCAPE:
      BATTLE_LostEscape(battleindex, attackNo);
      break;

    case BATTLE_COM_S_STEAL:
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      BATTLE_Steal(battleindex, attackNo, defNo);
      break;

#ifdef _BATTLESTEAL_FIX
    case BATTLE_COM_S_STEALMONEY:
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }
      BATTLE_StealMoney(battleindex, attackNo, defNo);
      break;
#endif
    case BATTLE_COM_S_NOGUARD:
      BATTLE_NoAction(battleindex, attackNo);
      break;
    case BATTLE_COM_S_CHARGE:
      sprintf(szWork, "bt|%x|", attackNo);
      BATTLESTR_ADD(szWork);
      break;
#ifdef _PETSKILL_LER
    case BATTLE_COM_S_BAT_FLY: {
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }

      BATTLE_BatFly(battleindex, attackNo, myside);
    } break;
    case BATTLE_COM_S_DIVIDE_ATTACK: {
      if ((defNo = BATTLE_TargetAdjust(battleindex, charaindex, myside)) < 0) {
        BATTLE_NoAction(battleindex, attackNo);
        break;
      }

      BATTLE_DivideAttack(battleindex, attackNo, myside);
    } break;
#endif
#ifdef _PETSKILL_BATTLE_MODEL
    case BATTLE_COM_S_BATTLE_MODEL:
      BATTLE_BattleModel(battleindex, attackNo, myside);
      break;
#endif
    //---------------------------------------------
    default:
      printf("\nerr:奇怪的命令(%d:%d:%s)\n", charaindex, COM,
             CHAR_getUseName(charaindex));
      sprintf(szWork, "bg|%x|", attackNo);
      BATTLESTR_ADD(szWork);
      break;
    }
    if (CHAR_CHECKINDEX(charaindex)) {
#ifdef _VARY_WOLF
      if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 101428
#ifdef _EXPANSION_VARY_WOLF
          || CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 104109
#endif
      ) {
        if (CHAR_getWorkInt(charaindex, CHAR_WORKTURN) == 0)
          CHAR_setWorkInt(charaindex, CHAR_WORKTURN, 1);
        else
          CHAR_setWorkInt(charaindex, CHAR_WORKTURN,
                          CHAR_getWorkInt(charaindex, CHAR_WORKTURN) + 1);

        if (CHAR_getWorkInt(charaindex, CHAR_WORKTURN) > 5) {
          CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
                      CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER));
          CHAR_setWorkInt(charaindex, CHAR_WORKATTACKPOWER,
                          CHAR_getWorkInt(charaindex, CHAR_WORKFIXSTR));
          CHAR_setWorkInt(charaindex, CHAR_WORKDEFENCEPOWER,
                          CHAR_getWorkInt(charaindex, CHAR_WORKFIXTOUGH));
          CHAR_setWorkInt(charaindex, CHAR_WORKQUICK,
                          CHAR_getWorkInt(charaindex, CHAR_WORKFIXDEX));
          CHAR_setWorkInt(charaindex, CHAR_WORKTURN, 0);
        }
      }
#endif
#ifdef _PETSKILL_BECOMEFOX // 判断中了媚惑术後是否已到恢复的回合数
      // if(CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER)==101749)
      // //用图号判断的话,若是人变身时,会被自动改回去
      if (CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) != -1) //若是变成小狐狸
      {
        if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) != 101749)
          CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, 101749);

        if (pBattle->turn - CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) >
            2) {
          int defNo = BATTLE_Index2No(battleindex, charaindex);
          int toNo = defNo - 5; //主人的编号
          // print("\n变身人物回复:%s,round:%d", CHAR_getChar( charaindex,
          // CHAR_NAME), CHAR_getWorkInt( charaindex, CHAR_WORKFOXROUND));

          CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
                      CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER));

          CHAR_setWorkInt(charaindex, CHAR_WORKATTACKPOWER,
                          CHAR_getWorkInt(charaindex, CHAR_WORKFIXSTR));
          CHAR_setWorkInt(charaindex, CHAR_WORKDEFENCEPOWER,
                          CHAR_getWorkInt(charaindex, CHAR_WORKFIXTOUGH));
          CHAR_setWorkInt(charaindex, CHAR_WORKQUICK,
                          CHAR_getWorkInt(charaindex, CHAR_WORKFIXDEX));
          CHAR_setWorkInt(charaindex, CHAR_WORKFOXROUND, -1);

          // Change fix 回复宠物忘掉的技能
          sprintf(
              szWork, "W%d",
              CHAR_getInt(BATTLE_No2Index(battleindex, toNo), CHAR_DEFAULTPET));
          CHAR_sendStatusString(BATTLE_No2Index(battleindex, toNo), szWork);
        }
      }
#endif
#ifdef _PETSKILL_BECOMEPIG
      if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1 &&
          (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)) {
        char pigbuffer[128];
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
                    CHAR_getInt(charaindex, CHAR_BECOMEPIG_BBI));
        if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > 0) {
          sprintf(pigbuffer, "乌力时间:%d秒",
                  CHAR_getInt(charaindex, CHAR_BECOMEPIG));
          CHAR_talkToCli(charaindex, -1, pigbuffer, CHAR_COLORWHITE);
        } else {
          sprintf(pigbuffer, " 乌力时间结束，离开战斗後可解除乌力化:%d ",
                  CHAR_getInt(charaindex, CHAR_BECOMEPIG));
          CHAR_talkToCli(charaindex, -1, pigbuffer, CHAR_COLORWHITE);
        }
      }
#endif
#ifdef _PROFESSION_ADDSKILL
      {
          /*			for(i=0;i<20;i++){
                                          if( pBattle->ice_use[i] ){//冰爆准备中
                                                  print("\n冰爆准备中");
                                                  if( pBattle->ice_bout[i] > 0
             ){//冰爆倒数 print("\n冰爆倒数"); if( --pBattle->ice_bout[i] == 0
             ){//放冰爆 print("\n放冰爆"); pBattle->ice_use[i] = FALSE; if(
             CHAR_CHECKINDEX(pBattle->ice_charaindex[i]) ){ int battlecom =
             CHAR_getWorkInt( pBattle->ice_charaindex[i], CHAR_WORKBATTLECOM1 );
                                                                          print("\n冰爆");
                                                                          print("\nbattlecom:%d",battlecom);
                                                                          CHAR_setWorkInt(
             pBattle->ice_charaindex[i], CHAR_WORKBATTLECOM1,
             BATTLE_COM_S_ICE_CRACK ); CHAR_SETWORKINT_HIGH(
             pBattle->ice_charaindex[i], CHAR_WORKBATTLECOM3,
             pBattle->ice_level[i] ); CHAR_setWorkInt(
             pBattle->ice_charaindex[i], CHAR_WORKBATTLECOM2,
             pBattle->ice_toNo[i] ); CHAR_SETWORKINT_HIGH(
             pBattle->ice_charaindex[i], CHAR_WORKBATTLECOM3,
             pBattle->ice_level[i] ); CHAR_SETWORKINT_LOW(
             pBattle->ice_charaindex[i], CHAR_WORKBATTLECOM3,
             pBattle->ice_array[i] );
                                                                          battle_profession_attack_magic_fun(battleindex,
             pBattle->ice_attackNo[i], pBattle->ice_toNo[i],
             pBattle->ice_charaindex[i]); CHAR_setWorkInt(
             pBattle->ice_charaindex[i], CHAR_WORKBATTLECOM1, battlecom );
                                                                  }
                                                          }
                                                  }
                                          }
                                  }*/
      } {
        int earth_boundary = GETHIGHVALUE(
            CHAR_getWorkInt(charaindex, CHAR_WORKFIXEARTHAT_BOUNDARY));
        int water_boundary = GETHIGHVALUE(
            CHAR_getWorkInt(charaindex, CHAR_WORKFIXWATERAT_BOUNDARY));
        int fire_boundary = GETHIGHVALUE(
            CHAR_getWorkInt(charaindex, CHAR_WORKFIXFIREAT_BOUNDARY));
        int wind_boundary = GETHIGHVALUE(
            CHAR_getWorkInt(charaindex, CHAR_WORKFIXWINDAT_BOUNDARY));
        int boundary_turn = 0;
        BOOL boundaryclear = FALSE;
        if (earth_boundary > 0) { //地结界
          boundary_turn = GETLOWVALUE(CHAR_getWorkInt(
                              charaindex, CHAR_WORKFIXEARTHAT_BOUNDARY)) -
                          1;
          //				print("\n地:%d ",boundary_turn );
          if (boundary_turn <= -1)
            CHAR_setWorkInt(charaindex, CHAR_WORKFIXEARTHAT_BOUNDARY, 0),
                boundaryclear = TRUE;
          else
            CHAR_setWorkInt(charaindex, CHAR_WORKFIXEARTHAT_BOUNDARY,
                            MAKE2VALUE(earth_boundary, boundary_turn));
        }
        if (water_boundary > 0) { //水结界
          boundary_turn = GETLOWVALUE(CHAR_getWorkInt(
                              charaindex, CHAR_WORKFIXWATERAT_BOUNDARY)) -
                          1;
          //				print("\n水:%d ",boundary_turn );

          if (boundary_turn <= -1)
            CHAR_setWorkInt(charaindex, CHAR_WORKFIXWATERAT_BOUNDARY, 0),
                boundaryclear = TRUE;
          else
            CHAR_setWorkInt(charaindex, CHAR_WORKFIXWATERAT_BOUNDARY,
                            MAKE2VALUE(water_boundary, boundary_turn));
        }
        if (fire_boundary > 0) { //火结界
          boundary_turn = GETLOWVALUE(CHAR_getWorkInt(
                              charaindex, CHAR_WORKFIXFIREAT_BOUNDARY)) -
                          1;
          //				print("\n火:%d ",boundary_turn );

          if (boundary_turn <= -1)
            CHAR_setWorkInt(charaindex, CHAR_WORKFIXFIREAT_BOUNDARY, 0),
                boundaryclear = TRUE;
          else
            CHAR_setWorkInt(charaindex, CHAR_WORKFIXFIREAT_BOUNDARY,
                            MAKE2VALUE(fire_boundary, boundary_turn));
        }
        if (wind_boundary > 0) { //风结界
          boundary_turn = GETLOWVALUE(CHAR_getWorkInt(
                              charaindex, CHAR_WORKFIXWINDAT_BOUNDARY)) -
                          1;
          //				print("\n风:%d ",boundary_turn );

          if (boundary_turn <= -1)
            CHAR_setWorkInt(charaindex, CHAR_WORKFIXWINDAT_BOUNDARY, 0),
                boundaryclear = TRUE;
          else
            CHAR_setWorkInt(charaindex, CHAR_WORKFIXWINDAT_BOUNDARY,
                            MAKE2VALUE(wind_boundary, boundary_turn));
        }
        if (GETHIGHVALUE(CHAR_getWorkInt(charaindex,
                                         CHAR_WORKFIXEARTHAT_BOUNDARY)) == 0 &&
            GETHIGHVALUE(CHAR_getWorkInt(charaindex,
                                         CHAR_WORKFIXWATERAT_BOUNDARY)) == 0 &&
            GETHIGHVALUE(CHAR_getWorkInt(charaindex,
                                         CHAR_WORKFIXFIREAT_BOUNDARY)) == 0 &&
            GETHIGHVALUE(CHAR_getWorkInt(charaindex,
                                         CHAR_WORKFIXWINDAT_BOUNDARY)) == 0 &&
            boundaryclear) {
          int toNo = BATTLE_Index2No(battleindex, charaindex);
          if (toNo < 10) //右方
            snprintf(szWork, sizeof(szWork), "Ba|%X|%X|", 0, 1);
          else
            snprintf(szWork, sizeof(szWork), "Ba|%X|%X|", 0, 0);
          BATTLESTR_ADD(szWork);
          BATTLESTR_ADD("FF|");
        }
      }
#endif
    }
#ifdef _Item_ReLifeAct
    {
      // pEntry
      int k, userindex = -1;
      //		if( BattleArray[ battleindex].type !=
      //BATTLE_TYPE_P_vs_P){
      for (k = 0; k < 20; k++) {
        userindex = BATTLE_getBattleDieIndex(battleindex, k);
        if (!CHAR_CHECKINDEX(userindex))
          continue;
        // Change fix CHECK_ITEM_RELIFE里面有检查了,这里不用
        // if( CHAR_getFlg( userindex, CHAR_ISDIE ) == FALSE )
        //	continue;
        if (CHAR_getInt(userindex, CHAR_HP) > 0) {
#ifdef _PRO3_ADDSKILL
          if (CHAR_getInt(userindex, PROFESSION_CLASS) != 0 &&
              CHAR_getInt(userindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
            CHAR_sendStatusString(userindex, "G");
#endif
#ifdef _PRO_DOCILE
          if (GETTHIRDVALUE(CHAR_getWorkInt(userindex, CHAR_WORK_DOCILE)) > 0) {
            int docile = CHAR_getWorkInt(userindex, CHAR_WORK_DOCILE);
            int dociletemp = GETTHIRDVALUE(docile);
            int dociletemp2 = GETSECONDVALUE(docile);
            int dociletemp3 = GETFIRSTVALUE(docile);

            if (dociletemp > 3) {
              int petdocile = BATTLE_No2Index(battleindex, dociletemp2);
              /////////////////////////////////////////////
              int l, skill_level = 0;
              for (l = 0; l < CHAR_SKILLMAXHAVE; l++) {
                char *skill_name;
                // 技能ID
                int skillid = CHAR_getCharSkill(userindex, l);
                int Pskillid = PROFESSION_SKILL_getskillArray(skillid);
                if (Pskillid <= 0)
                  continue;
                // 技能名称
                skill_name = PROFESSION_SKILL_getChar(
                    Pskillid, PROFESSION_SKILL_FUNCNAME);
                if (skill_name == NULL)
                  continue;
                if ((strcmp(skill_name, "PROFESSION_DOCILE2")) == 0) {
                  int skill = k;
                  CHAR_HaveSkill *hskill;
                  // 人物技能等级
                  hskill = CHAR_getCharHaveSkill(userindex, skill);
                  skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
                  // skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level
                  // );
                }
              }
              ///////////////////////////////////////////
              CHAR_setWorkInt(userindex, CHAR_WORK_DOCILE, 0);
              skill_level -= 20;
              skill_level = skill_level ? skill_level : 1;
              if (rand() % 100 < skill_level) {
                CHAR_setWorkInt(petdocile, CHAR_WORK_DOCILE,
                                MAKE4VALUE(0, 0, 0, 1));
                CHAR_talkToCli(userindex, -1, "驯服完成。", CHAR_COLORYELLOW);
              } else {
                CHAR_setWorkInt(petdocile, CHAR_WORK_DOCILE, 0);
                CHAR_talkToCli(userindex, -1, "驯服失败。", CHAR_COLORYELLOW);
              }
            } else
              CHAR_setWorkInt(
                  userindex, CHAR_WORK_DOCILE,
                  MAKE4VALUE(0, dociletemp + 1, dociletemp2, dociletemp3));
          }
#endif
          continue;
        }
        if (CHAR_getInt(userindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          CHECK_ITEM_RELIFE(battleindex, userindex);
        }
#ifdef _LOSE_FINCH_
        else if (CHAR_getInt(userindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
          CHECK_PET_RELIFE(battleindex, userindex);
        }
#endif
      }
    }
#endif
    BATTLESTR_ADD(szBadStatusString);
    BATTLE_AddProfit(battleindex, aAttackList); //取得经验值
    len = pszBattleTop - szBattleString;
    if (AllSize + len >= sizeof(szAllBattleString) - 1) {
      //     引匹中匀凶井日窒手仄卅中
    } else {
      memcpy(szAllBattleString + AllSize, szBattleString, len);
      szAllBattleString[AllSize + len] = 0; // NULL 毛本永玄
      AllSize += len;
    }
    if (BATTLE_CountAlive(battleindex, 0) == 0 ||
        BATTLE_CountAlive(battleindex, 1) == 0) {
      break;
    }
  }
#ifdef _PETSKILL_LER
  {
    register int i, dieindex;
    for (i = 0; i < 20; i++) {
      dieindex = BATTLE_getBattleDieIndex(battleindex, i);
      if (!CHAR_CHECKINDEX(dieindex))
        continue;
      if (CHAR_getFlg(dieindex, CHAR_ISDIE) == FALSE)
        continue;
      if (CHAR_getInt(dieindex, CHAR_HP) > 0)
        continue;
      // 雷尔死亡,变身
      if (CHAR_getInt(dieindex, CHAR_BASEBASEIMAGENUMBER) == 101813 ||
          CHAR_getInt(dieindex, CHAR_BASEBASEIMAGENUMBER) == 101814)
        BATTLE_LerChange(battleindex, dieindex, i);
    }
  }
#endif
  szBattleString[0] = 0;
  pszBattleTop = szBattleString;
  pszBattleLast = szBattleString + arraysizeof(szBattleString);
  if (pBattle->field_att != BATTLE_ATTR_NONE) {
    pBattle->att_count--;
    if (pBattle->att_count <= 0) {
      pBattle->field_att = BATTLE_ATTR_NONE;
      // snprintf( szBuffer, sizeof(szBuffer),
      //	"field回复成无属性。"
      //);
      // BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORYELLOW );
      BATTLESTR_ADD("BV|15|0|");
    }
  }

  strncat(szAllBattleString, szBattleString, sizeof(szAllBattleString));
  for (i = 0; i < entrynum; i++) {
    charaindex = EntryList[i].charaindex;
    if (!CHAR_CHECKINDEX(charaindex))
      continue;
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_RESCUE)
      continue;

    if (BATTLE_CommandSend(charaindex, szAllBattleString) == TRUE) {
    }
  }
  pWatchBattle = pBattle->pNext;
  for (; pWatchBattle; pWatchBattle = pWatchBattle->pNext) {
    if (BATTLE_CHECKADDRESS(pWatchBattle) == FALSE) {
      print("err:观战battle address错误(%p)\n", pWatchBattle);
      break;
    }
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      charaindex = pWatchBattle->Side[0].Entry[i].charaindex;
      if (CHAR_CHECKINDEX(charaindex) == FALSE)
        continue;
      BATTLE_CommandSend(charaindex, szAllBattleString);
    }
    pWatchBattle->mode = BATTLE_MODE_WATCHAFTER;
  }
  return 0;
}

#ifdef _Item_ReLifeAct
BOOL CHECK_ITEM_RELIFE(int battleindex, int toindex) {
  int i, itemindex;
  CHAR_EquipPlace ep;
  typedef void (*DIERELIFEFUNC)(int, int, int);
  DIERELIFEFUNC Drf;

  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    return FALSE;
  }

  if (CHAR_CHECKINDEX(toindex) == FALSE)
    return FALSE;

  if (CHAR_getInt(toindex, CHAR_HP) > 0)
    return FALSE;

#ifdef _DUMMYDIE
  if (CHAR_getFlg(toindex, CHAR_ISDIE) == FALSE &&
      CHAR_getFlg(toindex, CHAR_ISDUMMYDIE) == FALSE)
#else
  if (CHAR_getFlg(toindex, CHAR_ISDIE) == FALSE)
#endif
    return FALSE;

  for (i = 0; i < CHAR_EQUIPPLACENUM; i++) {
    itemindex = CHAR_getItemIndex(toindex, i);
    if (!ITEM_CHECKINDEX(itemindex))
      continue;
    ep = ITEM_getEquipPlace(toindex, itemindex);
    if (ep == -1)
      continue;
    Drf = (DIERELIFEFUNC)ITEM_getFunctionPointer(itemindex, ITEM_DIERELIFEFUNC);

    if (Drf) {
      Drf(toindex, itemindex, i);
      return TRUE;
    }
#ifdef _ALLBLUES_LUA_1_2
    else {
      if (RunItemDieReLifeEvent(toindex, itemindex, i) == TRUE)
        return TRUE;
    }
#endif
  }

  return FALSE;
}
#endif

#ifdef _LOSE_FINCH_
BOOL CHECK_PET_RELIFE(int battleindex, int petindex) {
  int ReceveEffect = -1;
  int toNo;
  int attackNo = -1;
  int WORK_HP = 1;
  int i;
  struct _RePet {
    int petid;
    int Effect;
    int Addhp;
    // Syu Mod
    //	}rePet[]={{1,SPR_fukkatu3,100},{-1,0,0}};
  } rePet[] = {{-1, 0, 0}};

  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    return FALSE;
  }
  if (CHAR_CHECKINDEX(petindex) == FALSE)
    return FALSE;

  if (CHAR_getFlg(petindex, CHAR_ISDIE) == FALSE) {
    print("\n PET_RELIFE:[ %d, CHAR_ISDIE FALSE] return !", petindex);
    return FALSE;
  }
  if (CHAR_getWorkInt(petindex, CHAR_WORKSPETRELIFE) != 0)
    return FALSE;
  for (i = 0; i < arraysizeof(rePet); i++) {
    if (CHAR_getInt(petindex, CHAR_PETID) == rePet[i].petid) //朱雀
      break;
  }
  if (i == arraysizeof(rePet))
    return FALSE;
  // ReceveEffect = SPR_fukkatu3;
  ReceveEffect = rePet[i].Effect; //朱雀动画 未定

  WORK_HP = CHAR_getWorkInt(petindex, CHAR_WORKMAXHP) * (rePet[i].Addhp / 100);
  toNo = BATTLE_Index2No(battleindex, petindex);
  attackNo = -1;
  BATTLE_MultiReLife(battleindex, attackNo, toNo, WORK_HP, ReceveEffect);
  CHAR_setWorkInt(petindex, CHAR_WORKSPETRELIFE, 1);
  return TRUE;
}
#endif

#ifdef _OTHER_MAGICSTAUTS
void BATTLE_MagicStatusSeq(int charaindex) {
  int cnt, i, bid, battleindex;

  battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
  bid = BATTLE_Index2No(battleindex, charaindex);

  for (i = 1; i < MAXSTATUSTYPE; i++) {
    if ((cnt = CHAR_getWorkInt(charaindex, MagicTbl[i])) <= 0)
      continue;
    CHAR_setWorkInt(charaindex, MagicTbl[i], --cnt);
    if (cnt <= 0) {
      // char szBuffer[256];
      CHAR_setWorkInt(charaindex, MagicTbl[i], 0);
      //			sprintf( szBuffer, "Bm|%X|%X|", bid, 0 );
      //			BATTLESTR_ADD( szBuffer );
      continue;
    }
  }
  return;
}
#endif

int Battle_getTotalBattleNum() { return Total_BattleNum; }

#ifdef _TYPE_TOXICATION
void CHAR_ComToxicationHp(int charaindex) {
  int hp, dhp, nums;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
  dhp = (CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP) / 100) + 1;
  if (dhp < 1)
    dhp = 1;
  hp = CHAR_getInt(charaindex, CHAR_HP);
  hp -= dhp;
  nums = CHAR_getWorkInt(charaindex, CHAR_WORKTOXICATION) + 1;
  CHAR_setWorkInt(charaindex, CHAR_WORKTOXICATION, nums);
  if (hp <= 0) {
    int floor, x, y;
    char buf[256];
    int fd = getfdFromCharaIndex(charaindex);
    sprintf(buf, "因中毒严重，昏迷回纪录点。");
    CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
    CHAR_DischargePartyNoMsg(charaindex); //解散团队
    if (CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
                              &floor, &x, &y) != TRUE) {
      CHAR_warpToSpecificPoint(charaindex, 1006, 15, 15);
    } else {
      CHAR_warpToSpecificPoint(charaindex, floor, x, y);
    }

    setToxication(fd, 0);
    hp = 1;
  } else if (nums % 10 == 0 || hp <= (dhp + 1) * 5) {
    char buf[256];
    sprintf(buf, "因中毒，HP剩余%d。", hp);
    CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
  }
  CHAR_setInt(charaindex, CHAR_HP, hp);
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_HP);
}
#endif

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能

void BATTLE_ProfessionStatus_init(int battleindex, int charaindex) {
  int profession_class = CHAR_getInt(charaindex, PROFESSION_CLASS);

  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return;
  if (profession_class <= PROFESSION_CLASS_NONE)
    return;

  // 初始化
  CHAR_setWorkInt(charaindex, CHAR_WORK_WEAPON, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKMOD_WEAPON, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORK_P_DUCK, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKMOD_P_DUCK, 0);

  {
    int i = 0, Pskillid = -1, skillid = -1;
    int value = 0, old_value = 0, WeaponType = -1;
    int rate = 0;
    char *skill_name, *pszOption, *pszP;
    CHAR_HaveSkill *hskill;

    for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {

      // 技能ID
      skillid = CHAR_getCharSkill(charaindex, i);
      Pskillid = PROFESSION_SKILL_getskillArray(skillid);
      if (Pskillid <= 0)
        continue;

      // 技能名称
      skill_name =
          PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
      if (skill_name == "\0")
        continue;

      // 火熟练度
      if ((strcmp(skill_name, "PROFESSION_FIRE_PRACTICE")) == 0) {
        int old_practice = CHAR_getInt(charaindex, PROFESSION_FIRE_P);
        int add_practice = 0;
        int skill_level = 0;
        // 技能等级
        hskill = CHAR_getCharHaveSkill(charaindex, i);
        skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
        skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

        if (skill_level >= 6)
          add_practice = (skill_level - 5) * 3 + 10;
        else
          add_practice = skill_level * 2;
        if (add_practice > 25)
          add_practice = 25;

        add_practice += old_practice;
        // 因为old_practice=0所以add_practice为2~25

        CHAR_setWorkInt(charaindex, CHAR_WORK_F_PROFICIENCY, add_practice);

      } else
        // 冰熟练度
        if ((strcmp(skill_name, "PROFESSION_ICE_PRACTICE")) == 0) {
          int old_practice = CHAR_getInt(charaindex, PROFESSION_ICE_P);
          int add_practice = 0;
          int skill_level = 0;

          // 技能等级
          hskill = CHAR_getCharHaveSkill(charaindex, i);
          skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
          skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

          if (skill_level >= 6)
            add_practice = (skill_level - 5) * 3 + 10;
          else
            add_practice = skill_level * 2;
          if (add_practice > 25)
            add_practice = 25;

          add_practice += old_practice;

          CHAR_setWorkInt(charaindex, CHAR_WORK_I_PROFICIENCY, add_practice);

        } else
          // 雷熟练度
          if ((strcmp(skill_name, "PROFESSION_THUNDER_PRACTICE")) == 0) {
            int old_practice = CHAR_getInt(charaindex, PROFESSION_THUNDER_P);
            int add_practice = 0;
            int skill_level = 0;

            // 技能等级
            hskill = CHAR_getCharHaveSkill(charaindex, i);
            skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
            skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

            if (skill_level >= 6)
              add_practice = (skill_level - 5) * 3 + 10;
            else
              add_practice = skill_level * 2;
            if (add_practice > 25)
              add_practice = 25;

            add_practice += old_practice;

            CHAR_setWorkInt(charaindex, CHAR_WORK_T_PROFICIENCY, add_practice);

          } else
            // 回避
            if ((strcmp(skill_name, "PROFESSION_AVOID")) == 0) {
              int skill_level = 0;

              if (profession_class !=
                  PROFESSION_SKILL_getInt(Pskillid,
                                          PROFESSION_SKILL_PROFESSION_CLASS))
                return;

              // 技能等级
              hskill = CHAR_getCharHaveSkill(charaindex, i);
              skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
              skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A(skill_level);

              if (skill_level <= 5)
                value = skill_level * 2;
              else
                value = (skill_level - 5) * 3;
              if (value > 25)
                value = 25;

              CHAR_setWorkInt(charaindex, CHAR_WORK_P_DUCK, 1);
              CHAR_setWorkInt(charaindex, CHAR_WORKMOD_P_DUCK, value);

            } else
              // 格档
              if ((strcmp(skill_name, "PROFESSION_DEFLECT")) == 0) {
                int skill_level = 0;
                int old_value = 0, add_value = 0;

                if (profession_class !=
                    PROFESSION_SKILL_getInt(Pskillid,
                                            PROFESSION_SKILL_PROFESSION_CLASS))
                  return;

                // 技能等级
                hskill = CHAR_getCharHaveSkill(charaindex, i);
                skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
                skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A(skill_level);

                old_value = CHAR_getWorkInt(charaindex, CHAR_WORKFIXARRANGE);
                add_value = old_value + skill_level + 10;

                CHAR_setWorkInt(charaindex, CHAR_WORKFIXARRANGE, add_value);
                CHAR_complianceParameter(charaindex);

              } else
                // 武器专精
                if ((strcmp(skill_name, "PROFESSION_WEAPON_FOCUS")) == 0) {
                  int skill_level = 0;

                  rate = 0;
                  if (profession_class !=
                      PROFESSION_SKILL_getInt(
                          Pskillid, PROFESSION_SKILL_PROFESSION_CLASS))
                    continue;

                  // 技能等级
                  hskill = CHAR_getCharHaveSkill(charaindex, i);
                  skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
                  skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A(skill_level);

                  // 技能参数
                  pszOption = PROFESSION_SKILL_getChar(Pskillid,
                                                       PROFESSION_SKILL_OPTION);
                  if (pszOption == "\0")
                    continue;

                  // 装备武器
                  WeaponType = BATTLE_GetWepon(charaindex);

                  switch (WeaponType) {
                  case ITEM_AXE:
                    if ((pszP = strstr(pszOption, "斧")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_CLUB:
                    if ((pszP = strstr(pszOption, "棍")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_SPEAR:
                    if ((pszP = strstr(pszOption, "枪")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_BOW:
                    if ((pszP = strstr(pszOption, "弓")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_BOOMERANG: // 回旋标
                    if ((pszP = strstr(pszOption, "镖")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_BOUNDTHROW: // 投掷斧头
                    if ((pszP = strstr(pszOption, "投")) != NULL)
                      rate = 1;
                    break;
                  case ITEM_BREAKTHROW: // 投掷石
                    if ((pszP = strstr(pszOption, "石")) != NULL)
                      rate = 1;
                    break;
                  default:
                    rate = 0;
                    break;
                  }

                  if (rate == 1) {
                    old_value =
                        CHAR_getWorkInt(charaindex, CHAR_MYSKILLSTRPOWER);

                    if (skill_level <= 5)
                      value = skill_level * 2 + old_value;
                    else
                      value = (skill_level - 5) * 3 + 10 + old_value;

                    if (value > 25)
                      value = 25;

                    CHAR_setWorkInt(charaindex, CHAR_WORK_WEAPON, 1);
                    CHAR_setWorkInt(charaindex, CHAR_WORKMOD_WEAPON, value);
                  }
                }
    }
  }
  return;
}

void BATTLE_ProfessionStatusSeq(int battleindex, int charaindex) {
  int profession_class = 0, i = 0, j = 0, Pskillid = -1, skillid = -1;
  int skill_level = 0, value = 0, old_value = 0;
  int bid = -1, hp = 0, flag = 0;
  char *skill_name, szcommand[256];
  CHAR_HaveSkill *hskill;

  int status_table[9] = {
      2,  //麻痹
      3,  //睡眠
      4,  //石化
      12, //晕眩
      13, //缠绕
      14, //天罗
      15, //冰暴
      17, //冰箭
      23,
  }; //雷附体

#ifdef _PETSKILL_DAMAGETOHP
/*	{
                char msg[32]={0};
                int pet,pindex;
                pet = CHAR_getInt( charaindex, CHAR_DEFAULTPET );
                if( pet >= 0 ){
                sprintf( msg, "o%d", pet );
                //if(CHAR_getInt( petindex, CHAR_PETID) == "狼" //宠物ID
                //   && CHAR_getInt( petindex, CHAR_HP) )
                CHAR_sendStatusString( charaindex, msg );
                pindex = CHAR_getCharPet( charaindex, pet );
                print("\n宠物名:%s",CHAR_getChar( pindex, CHAR_NAME));
                }
                else
                        print("\无宠物");

        } */
#endif

  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return;

  profession_class = CHAR_getInt(charaindex, PROFESSION_CLASS);
  if (profession_class <= PROFESSION_CLASS_NONE)
    return;

  for (i = 0; i < CHAR_SKILLMAXHAVE; i++) {

    // 技能ID
    skillid = CHAR_getCharSkill(charaindex, i);
    Pskillid = PROFESSION_SKILL_getskillArray(skillid);
    if (Pskillid <= 0)
      return;

    // 技能名称
    skill_name = PROFESSION_SKILL_getChar(Pskillid, PROFESSION_SKILL_FUNCNAME);
    if (skill_name == "\0")
      return;

    // 状态回复
    if ((strcmp(skill_name, "PROFESSION_REBACK")) == 0) {

      if (profession_class !=
          PROFESSION_SKILL_getInt(Pskillid, PROFESSION_SKILL_PROFESSION_CLASS))
        return;

      // 判断异常状态
      for (j = 0; j < 9; j++) {
        if ((CHAR_getWorkInt(charaindex, StatusTbl[status_table[j]])) > 0) {
          flag = 1;
          break;
        }
      }

      if (flag > 0) {
        // 技能等级
        hskill = CHAR_getCharHaveSkill(charaindex, i);
        skill_level = SKILL_getInt(&hskill->skill, SKILL_LEVEL);
        skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M(skill_level);

        value = skill_level * 2;
        if (value > 20)
          value = 20;

        old_value = CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP);

        value = old_value * value / 100;

        hp = CHAR_getInt(charaindex, CHAR_HP);

        if (value + hp > old_value)
          value = old_value - hp;

        if (value < 0)
          value = 0;

        CHAR_setInt(charaindex, CHAR_HP, value + hp);

        bid = BATTLE_Index2No(battleindex, charaindex);
        sprintf(szcommand, "BD|r%X|0|1|%X|", bid, value);
        BATTLESTR_ADD(szcommand);

        PROFESSION_SKILL_LVEVEL_UP(charaindex, "PROFESSION_REBACK");
      }
    }

    // 陷阱
    if ((strcmp(skill_name, "PROFESSION_TRAP")) == 0) {
      int count = 0;

      count = CHAR_getWorkInt(charaindex, CHAR_WORKTRAP);
      if (count > 0) {
        count--;
        CHAR_setWorkInt(charaindex, CHAR_WORKTRAP, count);
      } else if (count == 0) {
        CHAR_setWorkInt(charaindex, CHAR_WORKTRAP, 0);
        CHAR_setWorkInt(charaindex, CHAR_WORKMODTRAP, 0);
      }
    }
  }

  return;
}

#endif

#ifdef _ALLBLUES_LUA_1_4
BOOL BATTLE_setLUAFunction(int battleindex, int functype, lua_State *L,
                                  const char *luafunctable) {
  if (!BATTLE_CHECKINDEX(battleindex))
    return FALSE;

  if (functype < 0 || functype >= BATTLE_FUNCTABLENUM)
    return FALSE;

  BattleArray[battleindex].lua[functype] = L;
  BattleArray[battleindex].luafunctable[functype] =
      allocateMemory(strlen(luafunctable));
  memset(BattleArray[battleindex].luafunctable[functype], 0,
         strlen(luafunctable));
  strcpy(BattleArray[battleindex].luafunctable[functype], luafunctable);

  return TRUE;
}

lua_State *BATTLE_getLUAFunction(int battleindex, int functype) {

  if (!BATTLE_CHECKINDEX(battleindex))
    return NULL;

  if (functype < 0 || functype >= BATTLE_FUNCTABLENUM)
    return NULL;

  if (BattleArray[battleindex].lua[functype] == NULL) {
    return NULL;
  }

  lua_getglobal(BattleArray[battleindex].lua[functype],
                BattleArray[battleindex].luafunctable[functype]);
  return BattleArray[battleindex].lua[functype];
}
#endif

int BATTLE_getType(int battleindex) {
  if (battleindex >= BATTLE_battlenum || battleindex < 0)
    return -1;

  return BattleArray[battleindex].type;
}
#ifdef _BATTLE_TIMESPEED
int BATTLE_getCreateTime(int battleindex) {
  if (battleindex >= BATTLE_battlenum || battleindex < 0)
    return -1;

  return BattleArray[battleindex].CreateTime;
}
#endif

int BATTLE_getBattleFloor(int battleindex) {
  if (battleindex >= BATTLE_battlenum || battleindex < 0)
    return -1;

  return BattleArray[battleindex].BattleFloor;
}
