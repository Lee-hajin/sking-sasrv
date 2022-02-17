#include "npc_windowhealer.h"
#include "char.h"
#include "char_base.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "object.h"
#include "version.h"
#include <string.h>

#define RATE 1000

/*
 * ??今木凶?奴件玉?毛?允NPC
 *
 */

enum {
  CHAR_WORK_LEVEL = CHAR_NPCWORKINT1,
  CHAR_WORK_RANGE = CHAR_NPCWORKINT2,
  CHAR_WORK_HP = CHAR_NPCWORKINT3,
  CHAR_WORK_MP = CHAR_NPCWORKINT4,

};

static void NPC_WindowHealer_selectWindow(int meindex, int toindex, int num);
void NPC_WindowHealerAllHeal(int talker, int mode);
BOOL NPC_WindowHealerLevelCheck(int meindex, int talker);
BOOL NPC_WindowMoneyCheck(int meindex, int talker, int mode);
int NPC_WindowCostCheck(int meindex, int talker);
BOOL NPC_PetHealerCheck(int talker);
int NPC_WindowCostCheckMp(int meindex, int talker);

/*********************************
*********************************/
BOOL NPC_WindowHealerInit(int meindex) {

  char *npcarg;
  char buf2[256];
  int range = 1;
  int rate;
  double drate;
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TYPEHEALER);

  npcarg = CHAR_getChar(meindex, CHAR_NPCARGUMENT);

  /*--??允月午五及云?毛?月伊矛?毛筏盛--*/
  if (getStringFromIndexWithDelim(npcarg, "|", 1, buf2, sizeof(buf2)) !=
      FALSE) {
    CHAR_setWorkInt(meindex, CHAR_WORK_LEVEL, atoi(buf2));

  } else {
    return FALSE;
  }
  if (getStringFromIndexWithDelim(npcarg, "|", 2, buf2, sizeof(buf2)) !=
      FALSE) {
    drate = atof(buf2);
    if (drate == 0) {
      rate = 500;
    } else {
      rate = (int)(drate * RATE);
    }
    CHAR_setWorkInt(meindex, CHAR_WORK_HP, rate);
  }
  if (getStringFromIndexWithDelim(npcarg, "|", 3, buf2, sizeof(buf2)) !=
      FALSE) {
    drate = atof(buf2);
    if (drate == 0) {
      rate = 2000;
    } else {
      rate = (int)(drate * RATE);
    }
    CHAR_setWorkInt(meindex, CHAR_WORK_MP, rate);
  }
  if (getStringFromIndexWithDelim(npcarg, "|", 4, buf2, sizeof(buf2)) !=
      FALSE) {
    range = atoi(buf2);
    if (range == 0) {
      range = 1;
    }
  }
  CHAR_setWorkInt(meindex, CHAR_WORK_RANGE, range);
  return TRUE;
}

void NPC_WindowHealerTalked(int meindex, int talkerindex, char *szMes,
                            int color) {
  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }

  if (NPC_Util_CharDistance(talkerindex, meindex) >
      CHAR_getWorkInt(meindex, CHAR_WORK_RANGE)) {
    return;
  }
  if ((CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) == 0) ||
      (CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) == 2)) {
    NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);
  } else {
    int i = 0;
    int otherindex;

    for (i = 0; i < CHAR_PARTYMAX; i++) {
      otherindex = CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYINDEX1 + i);
      if (otherindex != -1) {
        NPC_WindowHealer_selectWindow(meindex, otherindex, 0);
      }
    }
  }
}
void NPC_WindowHealerLooked(int meindex, int lookedindex) {
  if (CHAR_getInt(lookedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }
  if (NPC_Util_CharDistance(lookedindex, meindex) >
      CHAR_getWorkInt(meindex, CHAR_WORK_RANGE)) {
    return;
  }
  NPC_WindowHealer_selectWindow(meindex, lookedindex, 0);
}

static void NPC_WindowHealer_selectWindow(int meindex, int toindex, int num) {

  char token[1024];
  char escapedname[1024];
  int fd = getfdFromCharaIndex(toindex);
  int buttontype = 0;
  int windowtype = 0;
  int windowno = 0;

  /*--??件玉?正?皿?永本□斥互云云中及匹燮卞??--*/
  windowtype = WINDOW_MESSAGETYPE_MESSAGE;

  switch (num) {
  case 0:
    sprintf(token, "　무엇을 하시겠습니까？　\n\n"
                   "　【 내구력회복 】　\n"
                   "　【 기력회복 】　\n"
                   "　【 내구력·기력회복 】　\n\n"
                   "　【 페트회복(무료) 】　");

    buttontype = WINDOW_BUTTONTYPE_CANCEL;
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG;
    break;

  case 1:
    if (CHAR_getInt(toindex, CHAR_HP) ==
        CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)) {
      if (NPC_PetHealerCheck(toindex) == FALSE) {
        sprintf(token, "　【　내구력회복　】"
                       "\n\n\n\n　회복할 필요가 없습니다！　");
      } else {
        sprintf(token, "　【　내구력회복　】"
                       "\n\n　회복할 필요가 없습니다！"
                       "\n\n　페트가 다친것같아！"
                       "\n　회복되었습니다！");

        NPC_WindowHealerAllHeal(toindex, 0);
      }

      buttontype = WINDOW_BUTTONTYPE_OK;
      windowno = CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG;
      break;
    } else if (NPC_WindowHealerLevelCheck(meindex, toindex) == TRUE) {
      sprintf(token, "　【　내구력회복　】"
                     "\n\n\n　초보자 님이 시군요！　　　 "
                     "\n\n　초보자님들은 무료회복이 가능합니다！");

    } else {
      int gold;
      gold = NPC_WindowCostCheck(meindex, toindex);
      sprintf(token,
              "　【　내구력회복　】"
              "\n\n\n　당신은 레벨이 높군요！　　　 "
              "\n\n　당신은%dSTONE이 필요합니다 。",
              gold);
    }

    buttontype = WINDOW_BUTTONTYPE_YESNO;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG;
    break;

  case 2:
    if (CHAR_getInt(toindex, CHAR_MP) ==
        CHAR_getWorkInt(toindex, CHAR_WORKMAXMP)) {
      if (NPC_PetHealerCheck(toindex) == FALSE) {
        sprintf(token, "　【　기력회복　】"
                       "\n\n\n\n　회복 할 필요가 없습니다！　");
      } else {
        sprintf(token, "　【　기력회복　】"
                       "\n\n　회복할 필요가 없습니다！"
                       "\n\n　페트가 다친것같아！"
                       "\n　회복되었습니다！");
        NPC_WindowHealerAllHeal(toindex, 0);
      }
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowno = CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG;
      break;
    }

    if (NPC_WindowHealerLevelCheck(meindex, toindex) == TRUE) {
      sprintf(token, "　【 기력회복】"
                     "\n\n　기력이 없으시군요！　　　　 "
                     "\n　당신은 무료로 치료가 가능합니다！");
    } else {
      int cost;
      cost = NPC_WindowCostCheckMp(meindex, toindex);
      sprintf(token,
              "　【 기력회복】"
              "\n\n\n　당신은 레벨이 높군요！ "
              "\n\n　당신은 %d STONE이 필요합니다 。",
              cost);
    }
    buttontype = WINDOW_BUTTONTYPE_YESNO;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG;
    break;

  case 3:
    sprintf(token, "\n　【내구력 회복】"
                   "\n　【내구력이 모두 회복되었습니다】"
                   "\n\n\n　다음에 또 이용해 주세요！");

    NPC_WindowHealerAllHeal(toindex, 1);
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG;
    break;

  case 4:
    sprintf(token, "\n　【기력 회복】"
                   "\n　【기력이 모두 회복되었습니다】"
                   "\n\n\n　다음에 또 이용해 주세요！");
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG;
    break;

  case 6: {
    int cost = 0;

    if (CHAR_getInt(toindex, CHAR_HP) <
        CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)) {
      cost += NPC_WindowCostCheck(meindex, toindex);
    }

    if (CHAR_getInt(toindex, CHAR_MP) <
        CHAR_getWorkInt(toindex, CHAR_WORKMAXMP)) {
      cost = cost + NPC_WindowCostCheckMp(meindex, toindex);
    }

    if (cost == 0) {
      if (NPC_PetHealerCheck(toindex) == FALSE) {
        sprintf(token, "　【　내구력및 기력회복　】"
                       "\n\n\n\n　회복 할 필요가 없습니다！　");
      } else {
        sprintf(token, "　【　내구력 및 기력회복　】"
                       "\n\n　회복 할 필요가 없습니다！　"
                       "\n\n　다음에 또 이용해 주세요！"
                       "\n　다음시합화이팅！");
        NPC_WindowHealerAllHeal(toindex, 0);
      }

      buttontype = WINDOW_BUTTONTYPE_OK;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG;
      break;

    } else {
      sprintf(token,
              "　【　내구력및 기력회복　】"
              "\n\n\n　내구력과 기력이 없으시군요！"
              "\n\n　당신은%d STONE 이 필요합니다 。",
              cost);
    }
  }

    if (NPC_WindowHealerLevelCheck(meindex, toindex) == TRUE) {
      sprintf(token, "　【　내구력및 기력회복　】"
                     "\n\n\n　내구력과 기력이 없으시군요！"
                     "\n\n　당신은 무료회복이 가능합니다！");
    }

    buttontype = WINDOW_BUTTONTYPE_YESNO;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG;
    break;

  case 7:
    sprintf(token, "　당신의 내구력과 기력이 회복되었습니다."
                   "\n　페트의 내구력도 회복되었습니다."
                   "\n\n\n　이제 당신과 페트는 건강해졌습니다.");

    NPC_WindowHealerAllHeal(toindex, 3);
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG;
    break;

  case 8:
    sprintf(token, "\n\n\n\n 　　회복할 필요가 없습니다！");

    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG;
    break;

  case 9:
    sprintf(token, "　　　　　　　【페트회복】　　　　　　"
                   "\n\n\n　　　　　　　페트가 많이 다쳤군요！　　　　　"
                   "\n\n　　회복되었습니다！　　");
    NPC_WindowHealerAllHeal(toindex, 0);
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG;
    break;

  case 10:
    sprintf(token, "　　　　　　　【페트회복】　　　　　　"
                   "\n\n\n　 페트가 건강하군요。　"
                   "\n\n　　회복되었습니다！　　");
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG;
    break;
  }

  makeEscapeString(token, escapedname, sizeof(escapedname));
  /*-仇仇匹霜?允月--*/
  lssproto_WN_send(fd, windowtype, buttontype, windowno,
                   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), escapedname);
}

/*-----------------------------------------
-------------------------------------------*/
void NPC_WindowHealerWindowTalked(int meindex, int talkerindex, int seqno,
                                  int select, char *data) {

  if (NPC_Util_CharDistance(talkerindex, meindex) >
      (CHAR_getWorkInt(meindex, CHAR_WORK_RANGE) + 4))
    return;

  switch (seqno) {
  case CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG:
    if (atoi(data) == 2) /*--?  仄凶嫩   1--*/
    {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 1);

    } else if (atoi(data) == 3) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 2);

    } else if (atoi(data) == 4) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 6);

    } else if (atoi(data) == 6) {
      if (NPC_PetHealerCheck(talkerindex) == TRUE) {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, 9);
      } else {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, 10);
      }
    } else if (select == WINDOW_BUTTONTYPE_CANCEL) {
    }
    break;

  case CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);

    } else if (select == WINDOW_BUTTONTYPE_YES) {
      /*--云?及民尼永弁--*/
      if (NPC_WindowMoneyCheck(meindex, talkerindex, 1) == TRUE) {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, 3);
      } else {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, 8);
      }
    } else if (select == WINDOW_BUTTONTYPE_NO) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);
    }
    break;

  case CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);

    } else if (select == WINDOW_BUTTONTYPE_YES) {
      /*--云?及民尼永弁--*/
      if (NPC_WindowMoneyCheck(meindex, talkerindex, 2) == TRUE) {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, 4);
      } else {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, 8);
      }

    } else if (select == WINDOW_BUTTONTYPE_NO) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);
    }

    break;

  case CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);
    }
    break;

  case CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);
    }
    break;

  /*--???今六月--*/
  case CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG:
    if (select == WINDOW_BUTTONTYPE_OK) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);

    } else if (select == WINDOW_BUTTONTYPE_YES) {
      if (NPC_WindowMoneyCheck(meindex, talkerindex, 3) == TRUE) {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, 7);
      } else {
        NPC_WindowHealer_selectWindow(meindex, talkerindex, 8);
      }

    } else if (select == WINDOW_BUTTONTYPE_NO) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);
    }
    break;

  /*--???及瑛?--*/
  case CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG:
    if (select == WINDOW_BUTTONTYPE_YES) {
      NPC_WindowHealer_selectWindow(meindex, talkerindex, 0);
    }
    break;
  }

  if (select == WINDOW_BUTTONTYPE_CANCEL &&
      CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) == 2) {
    CHAR_sendWatchEvent(CHAR_getWorkInt(talkerindex, CHAR_WORKOBJINDEX),
                        CHAR_ACTPLEASURE, NULL, 0, TRUE);
    CHAR_setWorkInt(talkerindex, CHAR_WORKACTION, CHAR_ACTPLEASURE);
  }
}

BOOL NPC_PetHealerCheck(int talker) {

  int petindex;
  int i;

  /*--矢永玄反云引?匹??仄化??月--*/
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    petindex = CHAR_getCharPet(talker, i);

    if (petindex == -1)
      continue;

    if (!CHAR_CHECKINDEX(talker))
      continue;

    if (CHAR_getInt(petindex, CHAR_HP) !=
        CHAR_getWorkInt(petindex, CHAR_WORKMAXHP)) {
      return TRUE;
    }
  }

  return FALSE;
}

/*-------------------------------------
---------------------------------------*/
void NPC_WindowHealerAllHeal(int talker, int mode) {
  int i;
  int petindex;
  char petsend[64];
  char msgbuf[64];

  if (mode == 1) {
    CHAR_setInt(talker, CHAR_HP, CHAR_getWorkInt(talker, CHAR_WORKMAXHP));
  } else if (mode == 2) {
    CHAR_setInt(talker, CHAR_MP, CHAR_getWorkInt(talker, CHAR_WORKMAXMP));
  } else if (mode == 3) {
    /*--???---*/
    CHAR_setInt(talker, CHAR_HP, CHAR_getWorkInt(talker, CHAR_WORKMAXHP));
    CHAR_setInt(talker, CHAR_MP, CHAR_getWorkInt(talker, CHAR_WORKMAXMP));
  }

  /*--矢永玄反云引?匹??仄化??月--*/
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    petindex = CHAR_getCharPet(talker, i);

    if (petindex == -1)
      continue;

    if (!CHAR_CHECKINDEX(talker))
      continue;

    /*--??--*/
    CHAR_setFlg(petindex, CHAR_ISDIE, 0);
    CHAR_setInt(petindex, CHAR_HP, CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));
    CHAR_setInt(petindex, CHAR_MP, CHAR_getWorkInt(petindex, CHAR_WORKMAXMP));

    /*--由??□正譬?--*/
    CHAR_complianceParameter(petindex);
    sprintf(petsend, "K%d", i);
    CHAR_sendStatusString(talker, petsend);
  }

  /*--由??□正霜曰--*/
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_HP);
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_MP);

  /*---醮棉互中木壬醮棉卞手霜?--*/
  if (CHAR_getWorkInt(talker, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    int topartyarray = -1;
    int oyaindex = CHAR_getWorkInt(talker, CHAR_WORKPARTYINDEX1);

    if (CHAR_CHECKINDEX(oyaindex)) {
      int i;

      for (i = 0; i < CHAR_PARTYMAX; i++) {
        int workindex = CHAR_getWorkInt(oyaindex, CHAR_WORKPARTYINDEX1 + i);

        if (CHAR_CHECKINDEX(workindex)) {
          if (workindex == talker) {
            topartyarray = i;
            break;
          }
        }
      }

      for (i = 0; i < CHAR_PARTYMAX; i++) {
        int otherindex = CHAR_getPartyIndex(talker, i);

        /* 醮棉由??□正毛霜月 */
        if (CHAR_CHECKINDEX(otherindex)) {
          snprintf(msgbuf, sizeof(msgbuf), "N%d", topartyarray);
          if (otherindex != talker) {
            CHAR_sendStatusString(otherindex, msgbuf);
          }
        }
      }
    }
  }
}

/*-------------------------------------
伊矛?民尼永弁
        ??伊矛????日	FALSE
---------------------------------------*/
BOOL NPC_WindowHealerLevelCheck(int meindex, int talker) {
  int level;

  level = CHAR_getWorkInt(meindex, CHAR_WORK_LEVEL);

  if (level > CHAR_getInt(talker, CHAR_LV)) {
    return TRUE;
  }

  return FALSE;
}

/*-------------------------------------------
?醒
talker’’’’平??弁正□及?件犯永弁旦
---------------------------------------------*/
BOOL NPC_WindowMoneyCheck(int meindex, int talker, int mode) {
  int cost = 0;
  int level;

  level = CHAR_getWorkInt(meindex, CHAR_WORK_LEVEL);

  if (mode == 1) {
    if (level <= CHAR_getInt(talker, CHAR_LV)) {
      /*---云?毛?曰?允---*/
      cost = NPC_WindowCostCheck(meindex, talker);
      /*--?箕反移??---*/
      /*---云?互?曰月井升丹井及民尼永弁---*/
      if (CHAR_getInt(talker, CHAR_GOLD) < cost) {
        return FALSE;
      }
      CHAR_DelGold(talker, cost);
    }
  }
  if (mode == 2) {
    if (level <= CHAR_getInt(talker, CHAR_LV)) {
      /*---云?毛?曰?允---*/
      cost = NPC_WindowCostCheckMp(meindex, talker);

      /*--?箕反移??---*/
      /*---云?互?曰月井升丹井及民尼永弁---*/
      if (CHAR_getInt(talker, CHAR_GOLD) < cost) {
        return FALSE;
      }
      CHAR_DelGold(talker, cost);
    }
  }

  if (mode == 3) {
    if (level <= CHAR_getInt(talker, CHAR_LV)) {

      if (CHAR_getInt(talker, CHAR_HP) <
          CHAR_getWorkInt(talker, CHAR_WORKMAXHP)) {
        cost = NPC_WindowCostCheck(meindex, talker);
      }

      if (CHAR_getInt(talker, CHAR_MP) <
          CHAR_getWorkInt(talker, CHAR_WORKMAXMP)) {
        cost = cost + NPC_WindowCostCheckMp(meindex, talker);
      }

      /*--移??--*/
      if (CHAR_getInt(talker, CHAR_GOLD) < cost) {
        return FALSE;
      }

      /*---云?毛?日允---*/
      CHAR_DelGold(talker, cost);
    }
  }

  CHAR_send_P_StatusString(talker, CHAR_P_STRING_GOLD);

  return TRUE;
}

/*-----------------------------------------
戊旦玄及煌?
------------------------------------------*/
int NPC_WindowCostCheck(int meindex, int talker) {
  int cost;
  double drate;
  int rate;

  rate = CHAR_getWorkInt(meindex, CHAR_WORK_HP);
  drate = (double)rate / 1000;
  cost = CHAR_getInt(talker, CHAR_LV);
  cost = (int)cost * drate;
  if (cost < 1)
    cost = 1;
  return cost;
}

/*-----------------------------------------
------------------------------------------*/
int NPC_WindowCostCheckMp(int meindex, int talker) {
  int cost;
  double drate;
  int rate;

  rate = CHAR_getWorkInt(meindex, CHAR_WORK_MP);
  drate = (double)rate / 1000;
  cost = CHAR_getInt(talker, CHAR_LV);
  cost = (int)cost * drate;
  if (cost == 0)
    cost = 1;
  return cost;
}
