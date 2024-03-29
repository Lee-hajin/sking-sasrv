#include "version.h"
#include <stdio.h>

#include "char.h"
#include "char_base.h"
#include "common.h"
#include "magic.h"
#include "magic_base.h"
#include "magic_field.h"

/*
 * 地上での魔法の処理
 */
/*----------------------------------------------------------------------
 * 自分のみ回復
 * 引数
 * charaindex		int		自分のindex
 * magicindex		int		魔法のindex
 *---------------------------------------------------------------------*/
int MAGIC_Recovery_Field(int charaindex, int magicindex) {
  char *magicarg;
  float power;
  int workhp;
  int prevhp;
  char msgbuf[64];

#ifdef _TYPE_TOXICATION
  if (CHAR_CanCureFlg(charaindex, "HP") == FALSE)
    return TRUE;
#endif
  magicarg = MAGIC_getChar(magicindex, MAGIC_OPTION);
  power = atoi(magicarg);
  power = RAND((power * 0.9), (power * 1.1));
#ifndef _MAGIC_REHPAI
  power *= GetRecoveryRate(charaindex);
#endif
  prevhp = CHAR_getInt(charaindex, CHAR_HP);
  workhp = prevhp + (int)power;
  CHAR_setInt(charaindex, CHAR_HP,
              min(workhp, CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP)));
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_HP | CHAR_P_STRING_MP);
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    int mypartyarray = -1;
    int oyaindex = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(oyaindex)) {
      int i;
      for (i = 0; i < CHAR_PARTYMAX; i++) {
        int workindex = CHAR_getWorkInt(oyaindex, CHAR_WORKPARTYINDEX1 + i);
        if (CHAR_CHECKINDEX(workindex)) {
          if (workindex == charaindex) {
            mypartyarray = i;
            break;
          }
        }
      }
      if (mypartyarray != -1) {
        for (i = 0; i < CHAR_PARTYMAX; i++) {
          int index = CHAR_getPartyIndex(charaindex, i);
          if (CHAR_CHECKINDEX(index)) {
            if (index != charaindex) {
              CHAR_send_N_StatusString(index, mypartyarray,
                                       CHAR_N_STRING_HP | CHAR_N_STRING_MP);
            }
          }
        }
      }
    }
  }
  snprintf(msgbuf, sizeof(msgbuf), "耐久力回复%d",
           min(workhp, CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP)) - prevhp);
  CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORWHITE);
  return TRUE;
}
/*----------------------------------------------------------------------
 * 他人を回復
 * 引数
 * charaindex		int		自分のindex
 * toindex			int		回復する人のindex
 * magicindex		int		魔法のindex
 *---------------------------------------------------------------------*/
int MAGIC_OtherRecovery_Field(int charaindex, int toindex, int magicindex) {
  char *magicarg;
  float power;
  int workhp;
  int prevhp;
  char msgbuf[64];

#ifdef _TYPE_TOXICATION
  if (CHAR_CanCureFlg(charaindex, "HP") == FALSE)
    return FALSE;
#endif
  magicarg = MAGIC_getChar(magicindex, MAGIC_OPTION);
  power = atoi(magicarg);
  power = RAND((power * 0.9), (power * 1.1));
#ifndef _MAGIC_REHPAI
  power *= GetRecoveryRate(toindex);
#endif
  prevhp = CHAR_getInt(toindex, CHAR_HP);
  workhp = prevhp + (int)power;
  CHAR_setInt(toindex, CHAR_HP,
              min(workhp, CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)));
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (charaindex != toindex) {
      CHAR_send_P_StatusString(toindex, CHAR_P_STRING_HP);
    }
  }
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_HP | CHAR_P_STRING_MP);
  if (charaindex != toindex) {
    CHAR_PartyUpdate(charaindex, CHAR_N_STRING_MP);
    CHAR_PartyUpdate(toindex, CHAR_N_STRING_HP);
  } else {
    CHAR_PartyUpdate(charaindex, CHAR_N_STRING_MP | CHAR_N_STRING_HP);
  }
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    int i;
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      int workindex = CHAR_getCharPet(charaindex, i);
      if (workindex == toindex) {
        CHAR_send_K_StatusString(charaindex, i, CHAR_K_STRING_HP);
      }
    }
  }

  if (charaindex != toindex) {
    snprintf(msgbuf, sizeof(msgbuf), "%s的耐久力回复%d",
             CHAR_getUseName(toindex),
             min(workhp, CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)) - prevhp);
    CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORWHITE);

    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      snprintf(msgbuf, sizeof(msgbuf), "藉由%s耐久力回复%d",
               CHAR_getUseName(charaindex),
               min(workhp, CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)) - prevhp);
      CHAR_talkToCli(toindex, -1, msgbuf, CHAR_COLORWHITE);
    }
  } else {
    snprintf(msgbuf, sizeof(msgbuf), "耐久力回复%d",
             min(workhp, CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP)) - prevhp);
    CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORWHITE);
  }

  return TRUE;
}
