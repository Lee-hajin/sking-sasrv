#include "battle.h"
#include "battle_event.h"
#include "char.h"
#include "char_base.h"
#include "common.h"
#include "magic_base.h"
#include "npcutil.h"
#include "pet_skill.h"
#include "profession_skill.h"
#include "version.h"
#include <stdio.h>


struct B_AI_RESULT {
  BATTLE_COM command; /* 窒毛允月井 */
  int target;         /* 簿卞 */
};

#ifdef BOSS_AI
static int BATTLE_ai_normal(int, int, BATTLE_ENTRY *, BATTLE_ENTRY *,
                            struct B_AI_RESULT *);
static int BATTLE_ai_bossai(int, int, BATTLE_ENTRY *, BATTLE_ENTRY *,
                            struct B_AI_RESULT *);
static int (*functbl[])(int, int, BATTLE_ENTRY *, BATTLE_ENTRY *,
                        struct B_AI_RESULT *) = {
    NULL,
    BATTLE_ai_normal,
    BATTLE_ai_bossai,
};
#else
static int BATTLE_ai_normal(int, int, BATTLE_ENTRY *, struct B_AI_RESULT *);

static int (*functbl[])(int, int, BATTLE_ENTRY *, struct B_AI_RESULT *) = {
    NULL,
    BATTLE_ai_normal,
};
#endif

int BATTLE_ai_all(int battleindex, int side, int turn) {
  int i;
  int rc = FALSE;
  BATTLE_ENTRY *pEntry;
#ifdef BOSS_AI
  BATTLE_ENTRY *pEntry2; //代表同队的资料
#endif
  struct B_AI_RESULT result;
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (BATTLE_CHECKSIDE(side ^ 1) == FALSE)
    return BATTLE_ERR_PARAM;
  if (BattleArray[battleindex].Side[side].type != BATTLE_S_TYPE_ENEMY)
    return FALSE;
  pEntry = BattleArray[battleindex].Side[side ^ 1].Entry;
#ifdef BOSS_AI
  pEntry2 = BattleArray[battleindex].Side[side].Entry;
#endif
  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    int charaindex = BattleArray[battleindex].Side[side].Entry[i].charaindex;
    int mode;
    if (CHAR_CHECKINDEX(charaindex) == FALSE)
      continue;
    mode = CHAR_getWorkInt(charaindex, CHAR_WORKTACTICS);
    if (mode < 0 || mode >= arraysizeof(functbl)) {
      // print( "BATTLE_ai_all 中,战斗逻辑模式很奇怪(%s)(%d)\n",CHAR_getUseName(
      // charaindex ), mode );
      mode = 1;
    }
    if (BATTLE_IsCharge(charaindex) == TRUE) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      continue;
    }
    if (BattleArray[battleindex].Side[side].flg & BSIDE_FLG_SURPRISE) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
    } else {
      if (functbl[mode] != NULL) {
#ifdef BOSS_AI
        rc = functbl[mode](turn, charaindex, pEntry, pEntry2, &result);
#else
        rc = functbl[mode](turn, charaindex, pEntry, &result);
#endif
        if (BATTLE_CanMoveCheck(charaindex) == FALSE) {
          result.command = BATTLE_COM_NONE;
        }
      }
      if (rc) {
#ifdef _BATTLENPC_WARP_PLAYER
        if (result.command == BATTLE_COM_WARP) {
          for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
            int tindex = (pEntry + i)->charaindex;
            if (!CHAR_CHECKINDEX(tindex))
              continue;
            if (CHAR_getFlg(tindex, CHAR_ISDIE))
              continue;
            if (CHAR_getWorkInt(tindex, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_RESCUE)
              continue;
            if (CHAR_getInt(tindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
              char sBuff[1024] = "", sBuff1[1024] = "";
              int ss[3];
              int sii;
              int rc = 1;
              if (NPC_Util_GetStrFromStrWithDelim(
                      CHAR_getWorkChar(charaindex,
                                       CHAR_WORKBATTLE_ACT_CONDITION),
                      "wp", sBuff, sizeof(sBuff)) != NULL) {
                for (sii = 1; sii <= 3; sii++) {
                  if (getStringFromIndexWithDelim(sBuff, ";", sii, sBuff1,
                                                  sizeof(sBuff1))) {
                    ss[sii - 1] = atoi(sBuff1);
                  } else {
                    rc = 0;
                  }
                }
                if (rc) {
                  CHAR_warpToSpecificPoint(tindex, ss[0], ss[1], ss[2]);
                  BATTLE_WatchStop(tindex);
                }
              }
            }
          }
        }
#endif
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, result.command);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                        result.target + (side ^ 1) * SIDE_OFFSET);
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
        // CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
      }
    }
  }
  return TRUE;
}

int BATTLE_ai_one(int charaindex, int battleindex, int side, int turn) {
  int rc = FALSE;
  BATTLE_ENTRY *pEntry;
  struct B_AI_RESULT result;
  int mode;

  /* 由仿丢□正民尼永弁 */
  if (BATTLE_CHECKINDEX(battleindex) == FALSE)
    return BATTLE_ERR_BATTLEINDEX;
  if (BATTLE_CHECKSIDE(side) == FALSE)
    return BATTLE_ERR_PARAM;
  if (BATTLE_CHECKSIDE(side ^ 1) == FALSE)
    return BATTLE_ERR_PARAM;

  /*     及扔奶玉隋垫丹 */
  /* 衬平乓仿动陆反  仃月 */
  if (BattleArray[battleindex].Side[side].type != BATTLE_S_TYPE_ENEMY)
    return 0;

  /*   覆础扔奶玉 */
  pEntry = BattleArray[battleindex].Side[side ^ 1].Entry;

  mode = CHAR_getWorkInt(charaindex, CHAR_WORKTACTICS);
  /* 质  毛蕊曰坌仃月 */
  if (mode < 0 || mode >= arraysizeof(functbl))
    return FALSE;
  /* 质  毛蕊曰坌仃月 */
  if (functbl[mode] != NULL) {
#ifdef BOSS_AI
    rc = functbl[mode](turn, charaindex, pEntry, pEntry, &result);
#else
    rc = functbl[mode](turn, charaindex, pEntry, &result);
#endif
  }
  if (rc) {
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, result.command);
    if (result.command == BATTLE_COM_ATTACK) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2,
                      result.target + (side ^ 1) * SIDE_OFFSET);
    }
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
  }
  return TRUE;
}

/*   骚卞锹澎毛瑁户月 */

#define B_AI_NORMAL_ATTACKOPTION "at" /*   猾左皿扑亦件 */
#define B_AI_NORMAL_GUARDOPTION "gu"  /*   豢左皿扑亦件 */
#define B_AI_NORMAL_MAGICOPTION "ma"  /* 热诸左皿扑亦件 */
#define B_AI_NORMAL_ESCAPEOPTION "es" /*   仆月左皿扑亦件 */
#define B_AI_NORMAL_WAZAOPTION "wa"   /*   左皿扑亦件 */
#ifdef _ENEMY_ATTACK_AI
#define B_AI_NORMAL_RANDAOPTION "rn"
#define B_AI_NORMAL_RANDOMOPTIONNUM 1
#endif
#ifdef BOSS_AI
#define B_AI_BOSSAI_BOSSOPTION "bs"  /*魔王脚色*/
#define B_AI_BOSSAI_BOSSOPTIONNUM 15 /*拥有15种智慧*/
#define B_AI_BOSSAI_AIOPTION1 "A1"
#define B_AI_BOSSAI_AIOPTION2 "A2"
#define B_AI_BOSSAI_AIOPTION3 "A3"
#define B_AI_BOSSAI_AIOPTION4 "A4"
#define B_AI_BOSSAI_AIOPTION5 "A5"
#define B_AI_BOSSAI_AIOPTION6 "A6"
#define B_AI_BOSSAI_AIOPTION7 "A7"
#define B_AI_BOSSAI_AIOPTION8 "A8"
#define B_AI_BOSSAI_AIOPTION9 "A9"
#define B_AI_BOSSAI_AIOPTION10 "A10"
#define B_AI_BOSSAI_AIOPTION11 "A11"
#define B_AI_BOSSAI_AIOPTION12 "A12"
#define B_AI_BOSSAI_AIOPTION13 "A13"
#define B_AI_BOSSAI_AIOPTION14 "A14"
#define B_AI_BOSSAI_AIOPTION15 "A15"
#endif

#define B_AI_NORMAL_ATTACKSUBOPTIONNUM 3
#define B_AI_NORMAL_GUARDSUBOPTIONNUM 1
#define B_AI_NORMAL_MAGICSUBOPTIONNUM 1
#define B_AI_NORMAL_ESCAPESUBOPTIONNUM 1
#define B_AI_NORMAL_WAZASUBOPTIONNUM 7

/* 左皿扑亦件娄醒及  侬及烂聒 */
#define B_AI_NORMAL_TARGET_ALL 1
#define B_AI_NORMAL_TARGET_PLAYER 2
#define B_AI_NORMAL_TARGET_PET 3
#ifdef _ENEMY_ATTACK_AI
#define B_AI_NORMAL_TARGET_LEADER 4
#endif
#define B_AI_NORMAL_SELECT_RANDOM 1
#define B_AI_NORMAL_SELECT_HP_MAX 2
#define B_AI_NORMAL_SELECT_HP_MIN 3
#ifdef _ENEMY_ATTACK_AI
#define B_AI_NORMAL_SELECT_STR_MAX 4
#define B_AI_NORMAL_SELECT_DEX_MAX 5
#define B_AI_NORMAL_SELECT_DEX_MIN 6
#define B_AI_NORMAL_SELECT_ATT_SUBDUE 7
enum { AI_ATT_EARTHAT = 1, AI_ATT_WATERAT, AI_ATT_FIREAT, AI_ATT_WINDAT };
int GetSubdueAttribute(int index) {
  int s_a = CHAR_getWorkInt(index, CHAR_WORKFIXEARTHAT);
  int s_b = CHAR_getWorkInt(index, CHAR_WORKFIXWATERAT);
  int s_c = CHAR_getWorkInt(index, CHAR_WORKFIXFIREAT);
  int s_d = CHAR_getWorkInt(index, CHAR_WORKFIXWINDAT);
  return ((s_a > s_c) ? ((s_b > s_d) ? ((s_a > s_b) ? (2) : (3))
                                     : ((s_a > s_d) ? (2) : (1)))
                      : ((s_b > s_d) ? ((s_c > s_b) ? (4) : (3))
                                     : ((s_c > s_d) ? (4) : (1))));
}
#endif

typedef enum {
  B_AI_ATTACKMODE = 1,
  B_AI_GURADMODE,
  B_AI_MAGICMODE,
  B_AI_ESCAPEMODE,
  B_AI_WAZAMODE0,
  B_AI_WAZAMODE1,
  B_AI_WAZAMODE2,
  B_AI_WAZAMODE3,
  B_AI_WAZAMODE4,
  B_AI_WAZAMODE5,
  B_AI_WAZAMODE6,
#ifdef BOSS_AI
  B_AI_BOSSMODE,
#endif
} B_AI_MODE;

#ifdef BOSS_AI
static int BATTLE_ai_normal(int turn, int charaindex, BATTLE_ENTRY *pEntry,
                            BATTLE_ENTRY *pEntry2, struct B_AI_RESULT *result)
#else
static int BATTLE_ai_normal(int turn, int charaindex, BATTLE_ENTRY *pEntry,
                            struct B_AI_RESULT *result)
#endif
{
  int at[B_AI_NORMAL_ATTACKSUBOPTIONNUM] = {0, 0, 0}; /*   猾左皿扑亦件 */
  int gu[B_AI_NORMAL_GUARDSUBOPTIONNUM] = {0};        /* 左皿扑亦件 */
  int ma[B_AI_NORMAL_MAGICSUBOPTIONNUM] = {0};        /* 左皿扑亦件 */
  int es[B_AI_NORMAL_ESCAPESUBOPTIONNUM] = {0};       /* 左皿扑亦件 */
  int wa[B_AI_NORMAL_WAZASUBOPTIONNUM] = {0, 0, 0, 0, 0, 0, 0}; /* 左皿扑亦件 */
#ifdef _ENEMY_ATTACK_AI
  int rn[B_AI_NORMAL_RANDOMOPTIONNUM] = {1};
#endif
  int target[BATTLE_ENTRY_MAX];
  int cnt;
  int i;
  int r;
  int rc;
  int mode = 0;
  char buff[256];
  char buff2[256];

  if (turn == 1) {
    print("应该没这回事。\n");
    return FALSE;
  }
  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
          B_AI_NORMAL_ATTACKOPTION, buff, sizeof(buff)) != NULL) {
    for (i = 1; i < B_AI_NORMAL_ATTACKSUBOPTIONNUM + 1; i++) {
      rc = getStringFromIndexWithDelim(buff, ";", i, buff2, sizeof(buff2));
      if (rc != TRUE) {
        print("battle_ai.c:Invarid Param [%s]\n",
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
        return FALSE;
      }
      at[i - 1] = atoi(buff2);
    }
  }

  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
          B_AI_NORMAL_GUARDOPTION, buff, sizeof(buff)) != NULL) {
    for (i = 1; i < B_AI_NORMAL_GUARDSUBOPTIONNUM + 1; i++) {
      rc = getStringFromIndexWithDelim(buff, ";", i, buff2, sizeof(buff2));
      if (rc != TRUE) {
        print("battle_ai.c:Invarid Param [%s]\n",
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
        return FALSE;
      }
      gu[i - 1] = atoi(buff2);
    }
  }
  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
          B_AI_NORMAL_MAGICOPTION, buff, sizeof(buff)) != NULL) {
    for (i = 1; i < B_AI_NORMAL_MAGICSUBOPTIONNUM + 1; i++) {
      rc = getStringFromIndexWithDelim(buff, ";", i, buff2, sizeof(buff2));
      if (rc != TRUE) {
        print("battle_ai.c:Invarid Param [%s]\n",
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
        return FALSE;
      }
      ma[i - 1] = atoi(buff2);
    }
  }
  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
          B_AI_NORMAL_ESCAPEOPTION, buff, sizeof(buff)) != NULL) {
    for (i = 1; i < B_AI_NORMAL_ESCAPESUBOPTIONNUM + 1; i++) {
      rc = getStringFromIndexWithDelim(buff, ";", i, buff2, sizeof(buff2));
      if (rc != TRUE) {
        print("battle_ai.c:Invarid Param [%s]\n",
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
        return FALSE;
      }
#ifdef _LV_1_NO_ESCAPE
      if (CHAR_getInt(charaindex, CHAR_LV) == 1)
        es[i - 1] = 0;
      else
        es[i - 1] = atoi(buff2);
#else
      es[i - 1] = atoi(buff2);
#endif
    }
  }
  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
          B_AI_NORMAL_WAZAOPTION, buff, sizeof(buff)) != NULL) {
    for (i = 1; i < B_AI_NORMAL_WAZASUBOPTIONNUM + 1; i++) {
      rc = getStringFromIndexWithDelim(buff, ";", i, buff2, sizeof(buff2));
      if (rc != TRUE) {
      } else {
        wa[i - 1] = atoi(buff2);
      }
    }
  }
#ifdef _ENEMY_ATTACK_AI
  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
          B_AI_NORMAL_RANDAOPTION, buff, sizeof(buff)) != NULL) {
    for (i = 1; i < B_AI_NORMAL_RANDOMOPTIONNUM + 1; i++) {
      rc = getStringFromIndexWithDelim(buff, ";", i, buff2, sizeof(buff2));
      if (rc != TRUE) {
        print("battle_ai.c:Invarid Param [%s]\n",
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION));
        return FALSE;
      }
      rn[i - 1] = atoi(buff2);
    }
  }
#endif
#ifdef _BATTLENPC_WARP_PLAYER
  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_ACT_CONDITION), "tn",
          buff, sizeof(buff)) != NULL) {
    int sTurn = atoi(buff);
    if (CHAR_getWorkInt(charaindex, CHAR_WORKTURN) == 0)
      CHAR_setWorkInt(charaindex, CHAR_WORKTURN, 1);
    else
      CHAR_setWorkInt(charaindex, CHAR_WORKTURN,
                      CHAR_getWorkInt(charaindex, CHAR_WORKTURN) + 1);
    if (sTurn == CHAR_getWorkInt(charaindex, CHAR_WORKTURN)) {
      result->command = BATTLE_COM_WARP;
      return TRUE;
    }
  }
#endif
  if (at[0] == 0 && gu[0] == 0 && ma[0] == 0 && es[0] == 0) {
    for (i = 0; i < B_AI_NORMAL_WAZASUBOPTIONNUM; i++) {
      if (wa[i] != 0)
        break;
    }
    if (i >= B_AI_NORMAL_WAZASUBOPTIONNUM) {
      // print( "无指定任何的攻击方式。\n" );
      return FALSE;
    }
  }
  while (!mode) {
    int work = 0;
    work = at[0] + gu[0] + ma[0] + es[0];
    for (i = 0; i < B_AI_NORMAL_WAZASUBOPTIONNUM; i++) {
      work += wa[i];
    }
    r = RAND(0, work - 1);
    if (at[0] != 0 && r < at[0])
      mode = B_AI_ATTACKMODE;
    else if (gu[0] != 0 && r < at[0] + gu[0])
      mode = B_AI_GURADMODE;
    else if (ma[0] != 0 && r < at[0] + gu[0] + ma[0])
      mode = B_AI_MAGICMODE;
    else if (es[0] != 0 && r < at[0] + gu[0] + ma[0] + es[0]) {
      mode = B_AI_ESCAPEMODE;
    }
    if (mode)
      break;
    work = at[0] + gu[0] + ma[0] + es[0];
    for (i = 0; i < B_AI_NORMAL_WAZASUBOPTIONNUM; i++) {
      work += wa[i];
      if (wa[i] != 0 && r < work) {
        mode = B_AI_WAZAMODE0 + i;
        break;
      }
    }
  }
  if (mode == B_AI_ATTACKMODE ||
      (B_AI_WAZAMODE0 <= mode && mode <= B_AI_WAZAMODE6)) {
    while (1) {
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        target[i] = -1;
      }
      cnt = 0;
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        int tindex = (pEntry + i)->charaindex;
        if (!CHAR_CHECKINDEX(tindex))
          continue;
        if (CHAR_getFlg(tindex, CHAR_ISDIE))
          continue;
        if (CHAR_getWorkInt(tindex, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_RESCUE)
          continue;
        switch (at[1]) {
        case B_AI_NORMAL_TARGET_PLAYER:
          if (CHAR_getInt(tindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
            target[cnt++] = i;
          }
          break;
        case B_AI_NORMAL_TARGET_PET:
          if (CHAR_getInt(tindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
            target[cnt++] = i;
          }
          break;
#ifdef _ENEMY_ATTACK_AI
        case B_AI_NORMAL_TARGET_LEADER:
          if (CHAR_getWorkInt(tindex, CHAR_WORKPARTYMODE) ==
              CHAR_PARTY_LEADER) {
            target[cnt++] = i;
          } else if (!RAND(0, 2)) {
            target[cnt++] = i;
          }
          break;
#endif
        default:
          target[cnt++] = i;
          break;
        }
      }
      if (cnt == 0 && at[1] == B_AI_NORMAL_TARGET_ALL)
        return FALSE;
      if (cnt > 0)
        break;
      at[1] = B_AI_NORMAL_TARGET_ALL;
    }
    result->command = BATTLE_COM_NONE;
    if (at[2] == B_AI_NORMAL_SELECT_RANDOM) {
      r = RAND(0, cnt - 1);
      result->command = BATTLE_COM_ATTACK;
      result->target = target[r];
    } else if (at[2] == B_AI_NORMAL_SELECT_HP_MAX ||
               at[2] == B_AI_NORMAL_SELECT_HP_MIN) {
      int top = 0;
      for (i = 0; i < cnt; i++) {
        if (i == 0)
          top = target[i];
        else {
          int tophp = CHAR_getInt((pEntry + top)->charaindex, CHAR_HP);
          int comphp = CHAR_getInt((pEntry + target[i])->charaindex, CHAR_HP);
          if (at[2] == B_AI_NORMAL_SELECT_HP_MAX) {
            if (comphp > tophp)
              top = target[i];
          } else {
            if (comphp < tophp)
              top = target[i];
          }
        }
      }
#ifdef _ENEMY_ATTACK_AI
      if (!RAND(0, rn[0]))
        result->target = target[RAND(0, cnt - 1)];
      else
        result->target = top;
      result->command = BATTLE_COM_ATTACK;
#else
      result->command = BATTLE_COM_ATTACK;
      result->target = top;
#endif
    }
#ifdef _ENEMY_ATTACK_AI
    else if (at[2] == B_AI_NORMAL_SELECT_STR_MAX) {
      int top = 0;
      for (i = 0; i < cnt; i++) {
        if (i == 0)
          top = target[i];
        else {
          int topstr = CHAR_getInt((pEntry + top)->charaindex, CHAR_STR);
          int compstr = CHAR_getInt((pEntry + target[i])->charaindex, CHAR_STR);
          if (compstr > topstr)
            top = target[i];
        }
      }
      if (!RAND(0, rn[0]))
        result->target = target[RAND(0, cnt - 1)];
      else
        result->target = top;
      result->command = BATTLE_COM_ATTACK;
    } else if (at[2] == B_AI_NORMAL_SELECT_DEX_MAX ||
               at[2] == B_AI_NORMAL_SELECT_DEX_MIN) {
      int top = 0;
      for (i = 0; i < cnt; i++) {
        if (i == 0)
          top = target[i];
        else {
          int topdex = CHAR_getInt((pEntry + top)->charaindex, CHAR_DEX);
          int compdex = CHAR_getInt((pEntry + target[i])->charaindex, CHAR_DEX);
          if (at[2] == B_AI_NORMAL_SELECT_DEX_MAX) {
            if (compdex > topdex)
              top = target[i];
          } else {
            if (compdex < topdex)
              top = target[i];
          }
        }
      }
      if (!RAND(0, rn[0]))
        result->target = target[RAND(0, cnt - 1)];
      else
        result->target = top;
      result->command = BATTLE_COM_ATTACK;
    } else if (at[2] == B_AI_NORMAL_SELECT_ATT_SUBDUE) {
      int top = 0;
      for (i = 0; i < cnt; i++) {
        if (i == 0)
          top = target[i];
        else {
          switch (GetSubdueAttribute(charaindex)) {
          case AI_ATT_EARTHAT: {
            int topatt = CHAR_getWorkInt((pEntry + top)->charaindex,
                                         CHAR_WORKFIXEARTHAT);
            int compatt = CHAR_getWorkInt((pEntry + target[i])->charaindex,
                                          CHAR_WORKFIXEARTHAT);
            if (compatt > topatt)
              top = target[i];
          } break;
          case AI_ATT_WATERAT: {
            int topatt = CHAR_getWorkInt((pEntry + top)->charaindex,
                                         CHAR_WORKFIXWATERAT);
            int compatt = CHAR_getWorkInt((pEntry + target[i])->charaindex,
                                          CHAR_WORKFIXWATERAT);
            if (compatt > topatt)
              top = target[i];
          } break;
          case AI_ATT_FIREAT: {
            int topatt =
                CHAR_getWorkInt((pEntry + top)->charaindex, CHAR_WORKFIXFIREAT);
            int compatt = CHAR_getWorkInt((pEntry + target[i])->charaindex,
                                          CHAR_WORKFIXFIREAT);
            if (compatt > topatt)
              top = target[i];
          } break;
          case AI_ATT_WINDAT: {
            int topatt =
                CHAR_getWorkInt((pEntry + top)->charaindex, CHAR_WORKFIXWINDAT);
            int compatt = CHAR_getWorkInt((pEntry + target[i])->charaindex,
                                          CHAR_WORKFIXWINDAT);
            if (compatt > topatt)
              top = target[i];
          } break;
          default:
            break;
          }
        }
      }
      if (!RAND(0, rn[0]))
        result->target = target[RAND(0, cnt - 1)];
      else
        result->target = top;
      result->command = BATTLE_COM_ATTACK;
    }
#endif
    else {
      return FALSE;
    }
    if (B_AI_WAZAMODE0 <= mode && mode <= B_AI_WAZAMODE6) {
      if (PETSKILL_Use(charaindex, mode - B_AI_WAZAMODE0, result->target,
                       NULL) == TRUE) {
        result->command = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
        return TRUE;
      } else {
        //				print( "此项技能尚未设定(%s):(%d)\n",
        //					CHAR_getUseName( charaindex), mode -
        //B_AI_WAZAMODE0 );
        return FALSE;
      }
    } else if (mode == B_AI_ATTACKMODE) {
      result->command = BATTLE_COM_ATTACK;
      return TRUE;
    }
  } else if (mode == B_AI_GURADMODE) {
    result->command = BATTLE_COM_GUARD;
    return TRUE;
  } else if (mode == B_AI_ESCAPEMODE) {
    result->command = BATTLE_COM_ESCAPE;
    return TRUE;
  }
  return FALSE;
}

#ifdef BOSS_AI // pEntry玩家队伍资料 pEntry2电脑队伍资料
static int BATTLE_ai_bossai(int turn, int charaindex, BATTLE_ENTRY *pEntry,
                            BATTLE_ENTRY *pEntry2, struct B_AI_RESULT *result) {
  int bs[B_AI_BOSSAI_BOSSOPTIONNUM] = {0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0};
  int target[BATTLE_ENTRY_MAX];
  int cnt, i, j, aindex, rc, top = 0;

  char buff[256];
  char buff2[256];
  int tindex;
  int arg1, arg2;
  int skilltable[10];

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

  if (turn == 1) {
    print("应该没这回事。\n");
    return FALSE;
  }

  memset(skilltable, -1, sizeof(skilltable));

  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
          B_AI_BOSSAI_BOSSOPTION, buff, sizeof(buff)) != NULL) {
    for (i = 1; i < B_AI_BOSSAI_BOSSOPTIONNUM + 1; i++) {
      rc = getStringFromIndexWithDelim(buff, ";", i, buff2, sizeof(buff2));
      if (rc != TRUE) {
        /*print( "param[%s] bs[%d %d %d %d %d %d %d %d]\n",
                        CHAR_getWorkChar( charaindex,
           CHAR_WORKBATTLE_TACTICSOPTION),
                        bs[0],bs[1],bs[2],bs[3],bs[4],bs[5],bs[6],bs[7]);*/
        break;
      }
      bs[i - 1] = atoi(buff2);
    }
  }

#ifdef _BATTLENPC_WARP_PLAYER
  if (NPC_Util_GetStrFromStrWithDelim(
          CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_ACT_CONDITION), "tn",
          buff, sizeof(buff)) != NULL) {
    int sTurn = atoi(buff);
    if (CHAR_getWorkInt(charaindex, CHAR_WORKTURN) == 0)
      CHAR_setWorkInt(charaindex, CHAR_WORKTURN, 1);
    else
      CHAR_setWorkInt(charaindex, CHAR_WORKTURN,
                      CHAR_getWorkInt(charaindex, CHAR_WORKTURN) + 1);
    if (sTurn == CHAR_getWorkInt(charaindex, CHAR_WORKTURN)) {
      result->command = BATTLE_COM_WARP;
      return TRUE;
    }
  }
#endif

  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    target[i] = -1;
  }
  cnt = 0;
  for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
    tindex = (pEntry + i)->charaindex;
    if (!CHAR_CHECKINDEX(tindex))
      continue;
    if (CHAR_getFlg(tindex, CHAR_ISDIE))
      continue;
    if (CHAR_getWorkInt(tindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_RESCUE)
      continue;
    target[cnt++] = i;
  }
  if (cnt == 0)
    return FALSE;
  //到此 cnt-有几个目标  target[]目标的位置
  // 14 situation ai
  for (aindex = 0; aindex < B_AI_BOSSAI_BOSSOPTIONNUM; aindex++) {
    if (bs[aindex] == 0)
      break;
    switch (bs[aindex]) {
    case 1:
      break;
    case 2:
      break;
    case 3: //队伍中是否有人中异常状态   如果有使用净化精灵
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        tindex = (pEntry2 + i)->charaindex;
        if (!CHAR_CHECKINDEX(tindex))
          continue;
        for (j = 0; j < 9; j++) {
          if (CHAR_getWorkInt(tindex, StatusTbl[status_table[j]]) > 0) {
            result->target = i + 10;
            if (BATTLE_ai_checkskilltarget(charaindex, 1, 0, &result->target,
                                           0) == FALSE) {
              result->command = BATTLE_COM_GUARD;
            } else {
              result->command =
                  CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
            }
            return TRUE;
          }
        }
      }

      break;
    case 4: //队伍中是否有人死亡		 如果有 使用复活术 501
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        tindex = (pEntry2 + i)->charaindex;
        if (!CHAR_CHECKINDEX(tindex))
          continue;
        if (CHAR_getFlg(tindex, CHAR_ISDIE) &&
            CHAR_getWorkInt(tindex, CHAR_WORKULTIMATE) != 0) {
          result->target = i;
          // print( "use 4 ai \n" );
          if (BATTLE_ai_checkskilltarget(charaindex, 1, 1, &result->target,
                                         0) == FALSE) {
            result->command = BATTLE_COM_GUARD;
          } else {
            result->command = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
          }
          return TRUE;
        }
      }
      break;
    case 5: //队伍中有人血量低于几%		 如果有 使用补血技能502
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION5, buff, sizeof(buff)) != NULL) {
        getStringFromIndexWithDelim(buff, ";", 1, buff2, sizeof(buff2));
        arg1 = atoi(buff2);
      }
      for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
        tindex = (pEntry2 + i)->charaindex;
        if (!CHAR_CHECKINDEX(tindex))
          continue;
        if (CHAR_getFlg(tindex, CHAR_ISDIE))
          continue;
        if (CHAR_getInt(tindex, CHAR_HP) <
            (CHAR_getWorkInt(tindex, CHAR_WORKMAXHP) * arg1 / 100)) {
          // print( "use 5 ai \n" );
          result->target = i;
          if (BATTLE_ai_checkskilltarget(charaindex, 1, 2, &result->target,
                                         0) == FALSE)
            result->command = BATTLE_COM_GUARD;
          else
            result->command = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
          return TRUE;
        }
      }
      break;
    case 6: //用职业技能攻击使用某职业技能的玩家
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION6, buff, sizeof(buff)) != NULL) {
        i = 0;
        while (getStringFromIndexWithDelim(buff, ";", i + 1, buff2,
                                           sizeof(buff2))) {
          skilltable[i] = atoi(buff2);
          i++;
        }
        for (i = 0; i < cnt; i++) {
          // print( "%s被纪录的职业技能 %d \n" ,CHAR_getChar(
          // (pEntry+target[i])->charaindex,CHAR_NAME),
          // (pEntry+target[i])->proskilluse );
          if ((pEntry + target[i])->proskilluse == skilltable[1] ||
              (pEntry + target[i])->proskilluse == skilltable[2] ||
              (pEntry + target[i])->proskilluse == skilltable[3]) {
            // print( "use 6 ai \n" );
            result->target = target[i];
            if (BATTLE_ai_checkskilltarget(charaindex, 2, skilltable[0],
                                           &result->target, 1) == FALSE)
              result->command = BATTLE_COM_GUARD;
            else
              result->command =
                  CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
            (pEntry + target[i])->proskilluse = 0;
            return TRUE;
          }
        }
      }
      break;
    case 7: //用职业技能攻击HP少于多少的玩家
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION7, buff, sizeof(buff)) != NULL) {
        getStringFromIndexWithDelim(buff, ";", 1, buff2, sizeof(buff2));
        arg1 = atoi(buff2);
        getStringFromIndexWithDelim(buff, ";", 2, buff2, sizeof(buff2));
        arg2 = atoi(buff2);
        // print( "\n使用7号AI:用职业技能(%d)攻击HP少于(%d%%)的玩家",arg2,arg1);
        for (i = 0; i < cnt; i++) {
          if (CHAR_getInt((pEntry + target[i])->charaindex, CHAR_HP) <
              (CHAR_getWorkInt((pEntry + target[i])->charaindex,
                               CHAR_WORKMAXHP) *
               arg1 / 100)) {
            // print( "use 7 ai \n" );
            result->target = target[i];
            if (BATTLE_ai_checkskilltarget(charaindex, 2, arg2, &result->target,
                                           1) == FALSE)
              result->command = BATTLE_COM_GUARD;
            else
              result->command =
                  CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
            return TRUE;
          }
        }
      }
      break;
    case 8: //用职业技能攻击防御力最高的玩家
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION8, buff, sizeof(buff)) != NULL) {
        getStringFromIndexWithDelim(buff, ";", 1, buff2, sizeof(buff2));
        arg1 = atoi(buff2); //技能id
        // print( "\n使用8号AI:用职业技能(%d)攻击防御力最高的玩家",arg1);
        top = 0;
        for (i = 0; i < cnt; i++) {
          if (i == 0)
            top = target[i];
          else {
            int topdef = CHAR_getWorkInt((pEntry + top)->charaindex,
                                         CHAR_WORKDEFENCEPOWER);
            int compdef = CHAR_getWorkInt((pEntry + target[i])->charaindex,
                                          CHAR_WORKDEFENCEPOWER);
            if (compdef > topdef)
              top = target[i];
          }
        }
        result->target = top;
        if (BATTLE_ai_checkskilltarget(charaindex, 2, arg1, &result->target,
                                       1) == FALSE)
          result->command = BATTLE_COM_GUARD;
        else
          result->command = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
        return TRUE;
      }
      break;
    case 9: //用职业技能攻击攻击力最高的玩家
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION9, buff, sizeof(buff)) != NULL) {
        getStringFromIndexWithDelim(buff, ";", 1, buff2, sizeof(buff2));
        arg1 = atoi(buff2);
        // print( "\n使用9号AI:用职业技能编号(%d)攻击攻击力最高的玩家",arg1);
        top = 0;
        for (i = 0; i < cnt; i++) {
          if (i == 0)
            top = target[i];
          else {
            int toppwr = CHAR_getWorkInt((pEntry + top)->charaindex,
                                         CHAR_WORKATTACKPOWER);
            int comppwr = CHAR_getWorkInt((pEntry + target[i])->charaindex,
                                          CHAR_WORKATTACKPOWER);
            if (comppwr > toppwr)
              top = target[i];
          }
        }
        result->target = top;
        // print( "use 9 ai \n" );
        if (BATTLE_ai_checkskilltarget(charaindex, 2, arg1, &result->target,
                                       1) == FALSE)
          result->command = BATTLE_COM_GUARD;
        else
          result->command = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
        return TRUE;
      }
      break;
    case 10: //用职业技能攻击使用净化精灵的玩家
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION10, buff, sizeof(buff)) != NULL) {
        getStringFromIndexWithDelim(buff, ";", 1, buff2, sizeof(buff2));
        arg1 = atoi(buff2);
        getStringFromIndexWithDelim(buff, ";", 2, buff2, sizeof(buff2));
        arg2 = atoi(buff2); //次数
        // print(
        // "\n使用10号AI:用职业技能(%d)攻击使用净化精灵次数%d次的玩家",arg1,arg2);
        for (i = 0; i < cnt; i++) {
          // print( "%s被纪录使用净化精灵的次数 %d\n",CHAR_getChar(
          // (pEntry+target[i])->charaindex, CHAR_NAME),
          // (pEntry+target[i])->purgeflg );
          if ((pEntry + target[i])->purgeflg >= arg2) {
            result->target = target[i];
            // print( "use 10 ai \n" );
            if (BATTLE_ai_checkskilltarget(charaindex, 2, arg1, &result->target,
                                           1) == FALSE)
              result->command = BATTLE_COM_GUARD;
            else
              result->command =
                  CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
            (pEntry + target[i])->purgeflg -= 1;
            // print( "被纪录使用净化精灵的次数 %d\n" ,
            // (pEntry+target[i])->purgeflg );
            return TRUE;
          }
        }
      }
      break;
    case 11: //用职业技能攻击使用多次回复精灵的玩家
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION11, buff, sizeof(buff)) != NULL) {
        getStringFromIndexWithDelim(buff, ";", 1, buff2, sizeof(buff2));
        arg1 = atoi(buff2);
        getStringFromIndexWithDelim(buff, ";", 2, buff2, sizeof(buff2));
        arg2 = atoi(buff2); //次数
        // print(
        // "\n使用11号AI:用职业技能(%d)攻击使用回复精灵次数%d次的玩家",arg1,arg2);
        for (i = 0; i < cnt; i++) {
          // print( "%s被纪录使用回复精灵的次数 %d\n" ,CHAR_getChar(
          // (pEntry+target[i])->charaindex, CHAR_NAME),
          // (pEntry+target[i])->addhpskill );
          if ((pEntry + target[i])->addhpskill >= arg2) {
            result->target = target[i];
            if (BATTLE_ai_checkskilltarget(charaindex, 2, arg1, &result->target,
                                           1) == FALSE)
              result->command = BATTLE_COM_GUARD;
            else
              result->command =
                  CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
            (pEntry + target[i])->addhpskill -= 1;
            // print( "被纪录使用回复精灵的次数 %d\n" ,
            // (pEntry+target[i])->addhpskill );
            return TRUE;
          }
        }
      }
      break;
    case 12: //用职业技能攻击仇恨的玩家
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION12, buff, sizeof(buff)) != NULL) {
        getStringFromIndexWithDelim(buff, ";", 1, buff2, sizeof(buff2));
        arg1 = atoi(buff2);
        getStringFromIndexWithDelim(buff, ";", 2, buff2, sizeof(buff2));
        arg2 = atoi(buff2); //仇恨最大值
        // print( "\n使用12号AI:用职业技能(%d)攻击仇恨值%d的玩家",arg1,arg2);
        for (i = 0; i < cnt; i++) {
          // print( "%s被纪录的仇恨值 %d arg2%d\n" ,CHAR_getChar(
          // (pEntry+target[i])->charaindex,CHAR_NAME),
          // (pEntry+target[i])->hatred,arg2 );
          if ((pEntry + target[i])->hatred >= arg2) {
            result->target = target[i];
            if (BATTLE_ai_checkskilltarget(charaindex, 2, arg1, &result->target,
                                           1) == FALSE)
              result->command = BATTLE_COM_GUARD;
            else
              result->command =
                  CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
            (pEntry + target[i])->hatred = 0;
            return TRUE;
          }
        }
      }
      break;
    case 13: //随机使用参数所设的职业技能 随机选择目标
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION13, buff, sizeof(buff)) != NULL) {
        i = 0;
        while (getStringFromIndexWithDelim(buff, ";", i + 1, buff2,
                                           sizeof(buff2))) {
          skilltable[i] = atoi(buff2);
          i++;
        }
        arg1 = skilltable[rand() % i];
        result->target = target[rand() % cnt];
        // print( "\n使用13号AI:%s",buff);
        if (BATTLE_ai_checkskilltarget(charaindex, 2, arg1, &result->target,
                                       1) == FALSE)
          result->command = BATTLE_COM_GUARD;
        else
          result->command = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
        return TRUE;
      }
      break;
    case 14: //随机使用参数所设的精灵魔法 随机选择目标
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION14, buff, sizeof(buff)) != NULL) {
        i = 0;
        while (getStringFromIndexWithDelim(buff, ";", i + 1, buff2,
                                           sizeof(buff2))) {
          skilltable[i] = atoi(buff2);
          i++;
        }
        arg1 = skilltable[rand() % i];
        result->target = target[rand() % cnt];
        // print( "\n使用14号AI:%s",buff);
        if (BATTLE_ai_checkskilltarget(charaindex, 3, arg1, &result->target,
                                       1) == FALSE)
          result->command = BATTLE_COM_GUARD;
        else
          result->command = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
        return TRUE;
      }
      break;
    case 15: //随机使用身上给予的宠物技能 随机选择目标
      if (NPC_Util_GetStrFromStrWithDelim(
              CHAR_getWorkChar(charaindex, CHAR_WORKBATTLE_TACTICSOPTION),
              B_AI_BOSSAI_AIOPTION15, buff, sizeof(buff)) != NULL) {
        i = 0;
        // print( "\n使用15号AI:%s",buff );
        while (getStringFromIndexWithDelim(buff, ";", i + 1, buff2,
                                           sizeof(buff2))) {
          skilltable[i] = atoi(buff2);
          i++;
        }
        arg1 = skilltable[rand() % i];
        result->target = target[rand() % cnt];

        if (BATTLE_ai_checkskilltarget(charaindex, 1, arg1, &result->target,
                                       1) == FALSE)
          result->command = BATTLE_COM_GUARD;
        else
          result->command = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLECOM1);
        return TRUE;
      }
      break;
    default:
      break;
    }
  }

  result->command = BATTLE_COM_NONE;
  return TRUE;
}
// type 1宠物技能	2职业技能	3精灵魔法
// *target 目前选择的目标
// ex检查技能的目标如果是全体技则改为选择全体
// side 0友方  1敌方
BOOL BATTLE_ai_checkskilltarget(int charaindex, int type, int skillid,
                                int *target, int side) {
  int skillindex = 0, petskillid;
  int set_target = 0;

  switch (type) {
  case 1:
    petskillid = CHAR_getPetSkill(charaindex, skillid);
    skillindex = PETSKILL_getPetskillArray(petskillid);
    set_target = PETSKILL_getInt(skillindex, PETSKILL_TARGET);
    // print("\n[%s 使用 pet_skill petskillid=%d skillid=%d
    // set_target=%d]",CHAR_getChar(charaindex,CHAR_NAME),petskillid,skillid,set_target);
    break;
#ifdef _PROFESSION_SKILL
  case 2:
    set_target = PROFESSION_SKILL_getInt(skillid, PROFESSION_SKILL_TARGET);
    // print("\n[%s 使用 skill set target
    // %d]",CHAR_getChar(charaindex,CHAR_NAME),set_target);
    break;
#endif
  case 3:
    skillindex = MAGIC_getMagicArray(skillid);
    set_target = MAGIC_getInt(skillindex, MAGIC_TARGET);
    // print("\n[%s 使用 magic_skill target=%d skillid=%d side
    // %d]",CHAR_getChar(charaindex,CHAR_NAME),set_target,skillid,side);
    break;
  }

  switch (set_target) {
  case 0:  //自己
  case 1:  //别人
    break; //一个的目标 进来之前应该就选好了 不用检查或修改
  case 2:  //友方全体
    *target = 21;
    break;
  case 3: //敌方全体
    *target = 20;
    break;
  case 4: //所有人
    *target = 22;
    break;
  case 5:
  case 6:
    // print("battle_ai_dontcare \n");
    break;
  case 7:
    *target = 20;
    break;
  case 8: //一排
    if (type == 2) {
      if (*target < 5)
        *target = 26;
      else if (*target < 10)
        *target = 25;
      else if (*target < 15)
        *target = 24;
      else
        *target = 23;
    } else {
      if (side == 0)
        *target = 21;
      else
        *target = 20;
    }
    break;
  case 9: //精灵技能用 单体
    break;
  case 10: //精灵技能用 一列
    if (type == 3) {
      if (*target < 5)
        *target = 26;
      else if (*target < 10)
        *target = 25;
      else if (*target < 15)
        *target = 24;
      else
        *target = 23;
    }
    break;
  case 11: //精灵技能用 全体
    if (side == 0)
      *target = 21;
    else
      *target = 20;
    break;
  }

  switch (type) {
  case 1:
    if (PETSKILL_Use(charaindex, skillid, *target, NULL) == FALSE)
      return FALSE;
    break;
#ifdef _PROFESSION_SKILL
  case 2:
    if (PROFESSION_SKILL_UseByBoss(charaindex, skillid, *target, NULL) == -1)
      return FALSE;
    break;
#endif
  case 3:
    CHAR_SETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3, skillid);
    CHAR_SETWORKINT_HIGH(charaindex, CHAR_WORKBATTLECOM3, 0);
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, *target);
    CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ATTACK_MAGIC);
    break;
  }
  return TRUE;
}
#endif
