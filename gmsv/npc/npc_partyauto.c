#include "npc_partyauto.h"
#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "enemy.h"
#include "handletime.h"
#include "log.h"
#include "lssproto_serv.h"
#include "npc_eventaction.h"
#include "npc_vipshop.h"
#include "npcutil.h"
#include "object.h"
#include "readmap.h"
#include "saacproto_cli.h"
#include "version.h"
#include <string.h>


#ifdef _CHAR_PARTAUTO
enum {
  WINDOW_START = 1,
  WINDOW_SELECT,
  WINDOW_PREV,
  WINDOW_NEXT,
  WINDOW_WARP,
  WINDOW_END,
};
enum {
  NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
};
int page;
int PartyAuto_ShowMenulist(int charaindex);
static void NPC_PartyAuto_selectWindow(int meindex, int toindex, int num,
                                       int select);
BOOL GetSelectMenu(int charaindex, int select, char *StrMenu, char *check);
BOOL GetStrMenuPart(int charaindex, char *token, int index);

BOOL NPC_PartyAutoInit(int meindex) {
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

  if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL) {
    print("TRANSER_MAN: GetArgStrErr!!");
    return FALSE;
  }

  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TRANSERMANS);
  // CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
  CHAR_setWorkInt(meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
  return TRUE;
}

void NPC_PartyAutoTalked(int meindex, int talkerindex, char *msg, int color) {
  /*if( !NPC_Util_isFaceToFace( meindex, talkerindex, 2)) {
                          if (strcmp(msg, "/매칭") != 0) { //
          if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
          }
  }*/

  CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);
  NPC_PartyAuto_selectWindow(meindex, talkerindex, WINDOW_START, 0);
}

static void NPC_PartyAuto_selectWindow(int meindex, int toindex, int num,
                                       int select) {
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  char token[256];
  char buf1[256];
  char buf2[256];
  char buf3[256];
  int buttontype = 0, windowtype = 0, windowno = 0;
  int fd = getfdFromCharaIndex(toindex);
  static int select1;
  int i, playernum = CHAR_getPlayerMaxNum();
  char msg[256];
  char check[6];
  int autoindex;
  int autox[3] = {-1, 1};
  int autoy[3] = {-1, 1};
  int Autox, Autoy;
  windowtype = WINDOW_MESSAGETYPE_MESSAGE;

  /*if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
          if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
                  return;
  }*/

  memset(npcarg, 0, sizeof(npcarg));
  if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL) {
    print("VipShop_MAN: GetArgStrErr!!");
    return;
  }
  memset(token, 0, sizeof(token));

  switch (num) {
  case WINDOW_START:
    if (NPC_Util_GetStrFromStrWithDelim(npcarg, "START_MSG", token,
                                        sizeof(token)) == NULL) {
      print("PartyAuto Get START_MSG ERROR !");
      return;
    }
    CHAR_setWorkInt(toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
    buttontype = WINDOW_BUTTONTYPE_YESNO;
    windowno = NPC_TRANSERMAN_SELECT;
    break;
  case WINDOW_SELECT: // 예 선택 시
    CHAR_setInt(toindex, CHAR_LISTPAGE, 0);
    page = PartyAuto_ShowMenulist(toindex);

    if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
      sprintf(msg, "               《 파티 검색 시스템 》\n\n현재 파티에 "
                   "가입되어 있는 상태 입니다.");

      lssproto_WN_send(getfdFromCharaIndex(toindex), WINDOW_MESSAGETYPE_MESSAGE,
                       WINDOW_BUTTONTYPE_OK, -1, -1,
                       makeEscapeString(msg, buf1, sizeof(buf1)));
      return;
    }

    if (GetStrMenuPart(toindex, token, CHAR_getInt(toindex, CHAR_LISTPAGE)) ==
        FALSE) {
      sprintf(msg, "               《 파티 검색 시스템 》\n\n해당 지역에는 "
                   "파티가 존재하지 않습니다.");

      lssproto_WN_send(getfdFromCharaIndex(toindex), WINDOW_MESSAGETYPE_MESSAGE,
                       WINDOW_BUTTONTYPE_OK, -1, -1,
                       makeEscapeString(msg, buf1, sizeof(buf1)));
      return;
    }
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    if (page > 1)
      buttontype = WINDOW_BUTTONTYPE_CANCEL | WINDOW_BUTTONTYPE_NEXT;
    else
      buttontype = WINDOW_BUTTONTYPE_CANCEL;
    windowno = NPC_TRANSERMAN_WARP;
    break;
  case WINDOW_NEXT:
    if (CHAR_getInt(toindex, CHAR_LISTPAGE) <= page)
      CHAR_setInt(toindex, CHAR_LISTPAGE,
                  CHAR_getInt(toindex, CHAR_LISTPAGE) + 1);
    if (GetStrMenuPart(toindex, token, CHAR_getInt(toindex, CHAR_LISTPAGE)) ==
        FALSE) {
      print("ERR GetStrMenuPart !! \n");
      return;
    }
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    if (CHAR_getInt(toindex, CHAR_LISTPAGE) + 1 == page)
      buttontype = WINDOW_BUTTONTYPE_CANCEL | WINDOW_BUTTONTYPE_PREV;
    else
      buttontype = WINDOW_BUTTONTYPE_CANCEL | WINDOW_BUTTONTYPE_PREV |
                   WINDOW_BUTTONTYPE_NEXT;
    windowno = NPC_TRANSERMAN_WARP;
    break;
  case WINDOW_PREV:
    CHAR_talkToCli(toindex, -1, "WINDOW_PREV", CHAR_COLORYELLOW);
    if (CHAR_getInt(toindex, CHAR_LISTPAGE) > 0)
      CHAR_setInt(toindex, CHAR_LISTPAGE,
                  CHAR_getInt(toindex, CHAR_LISTPAGE) - 1);
    if (GetStrMenuPart(toindex, token, CHAR_getInt(toindex, CHAR_LISTPAGE)) ==
        FALSE) {
      print("ERR GetStrMenuPart !! \n");
      return;
    }
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    if (CHAR_getInt(toindex, CHAR_LISTPAGE) == 0)
      buttontype = WINDOW_BUTTONTYPE_CANCEL | WINDOW_BUTTONTYPE_NEXT;
    else
      buttontype = WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_CANCEL |
                   WINDOW_BUTTONTYPE_NEXT;
    windowno = NPC_TRANSERMAN_WARP;
    break;
  case WINDOW_WARP:
    select1 = CHAR_getInt(toindex, CHAR_LISTPAGE) * 7 + select;
    GetSelectMenu(toindex, select1, msg, check);

    if (!strcmp(check, "OFF")) {
      sprintf(token,
              "               《 파티 검색 시스템 》\n\n%s\n해당 파티는 현재 "
              "[%s] 상태 입니다.",
              msg, check);
      buttontype = WINDOW_BUTTONTYPE_OK;
    }

    autoindex = CHAR_getInt(toindex, CHAR_AUTOPARTYINDEX);
    int battlepet, petindex;
    char petinfo[256];
    battlepet = CHAR_getInt(toindex, CHAR_DEFAULTPET);
    petindex = CHAR_getCharPet(toindex, battlepet);
    if (battlepet == -1) {
      sprintf(petinfo, "배틀 상태의 페트는 없습니다.");
    } else if (battlepet != -1) {
      sprintf(petinfo, "페트[%s]의 순발력:　%d",
              CHAR_getChar(petindex, CHAR_NAME),
              CHAR_getWorkInt(petindex, CHAR_WORKQUICK));
    }

    if (CHAR_getInt(autoindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC) {
      if (CHAR_getInt(toindex, CHAR_DEFAULTPET) != -1) {
        if (CHAR_getWorkInt(toindex, CHAR_WORKQUICK) <
                CHAR_getWorkInt(autoindex, CHAR_WORKPARTYLOWDEX) ||
            CHAR_getWorkInt(toindex, CHAR_WORKQUICK) >
                CHAR_getWorkInt(autoindex, CHAR_WORKPARTYHIGHDEX) ||
            CHAR_getWorkInt(petindex, CHAR_WORKQUICK) <
                CHAR_getWorkInt(autoindex, CHAR_WORKPARTYLOWDEX) ||
            CHAR_getWorkInt(petindex, CHAR_WORKQUICK) >
                CHAR_getWorkInt(autoindex, CHAR_WORKPARTYHIGHDEX)) {
          sprintf(token,
                  "               《 파티 검색 시스템 》\n\n%s캐릭터의 "
                  "순발력:　%d\n%s\n\n페트가 봇 파티 순발력 조건에 맞지 "
                  "않으므로 파티에 참여할 수 없습니다.",
                  msg, CHAR_getWorkInt(toindex, CHAR_WORKQUICK), petinfo,
                  check);
          buttontype = WINDOW_BUTTONTYPE_OK;
        } else if (CHAR_getInt(toindex, CHAR_DEFAULTPET) > -1) {
          sprintf(token,
                  "               《 파티 검색 시스템 》\n\n%s캐릭터의 "
                  "순발력:　%d\n%s\n\n봇 파티 순발력 조건에 적합 "
                  "합니다.\n파티에 참여 하시겠습니까?",
                  msg, CHAR_getWorkInt(toindex, CHAR_WORKQUICK), petinfo,
                  check);
          buttontype = WINDOW_BUTTONTYPE_YESNO;
          windowtype = WINDOW_MESSAGETYPE_MESSAGE;
          windowno = NPC_TRANSERMAN_END;
        }
      } else if (CHAR_getWorkInt(toindex, CHAR_WORKQUICK) <
                     CHAR_getWorkInt(autoindex, CHAR_WORKPARTYLOWDEX) ||
                 CHAR_getWorkInt(toindex, CHAR_WORKQUICK) >
                     CHAR_getWorkInt(autoindex, CHAR_WORKPARTYHIGHDEX)) {
        sprintf(token,
                "               《 파티 검색 시스템 》\n\n%s캐릭터의 "
                "순발력:　%d\n%s\n\n캐릭터가 봇 파티 순발력 조건에 맞지 "
                "않으므로 파티에 참여할 수 없습니다.",
                msg, CHAR_getWorkInt(toindex, CHAR_WORKQUICK), petinfo, check);
        buttontype = WINDOW_BUTTONTYPE_OK;
      } else {
        sprintf(token,
                "               《 파티 검색 시스템 》\n\n%s캐릭터의 "
                "순발력:　%d\n%s\n\n봇 파티 순발력 조건에 적합 합니다.\n파티에 "
                "참여 하시겠습니까?",
                msg, CHAR_getWorkInt(toindex, CHAR_WORKQUICK), petinfo, check);
        buttontype = WINDOW_BUTTONTYPE_YESNO;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = NPC_TRANSERMAN_END;
      }
    } else {
      sprintf(token,
              "               《 파티 검색 시스템 》\n\n%s\n해당 파티에 참여 "
              "하시겠습니까？",
              msg);
      buttontype = WINDOW_BUTTONTYPE_YESNO;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = NPC_TRANSERMAN_END;
    }
    break;
  case WINDOW_END:
    autoindex = CHAR_getInt(toindex, CHAR_AUTOPARTYINDEX);
    CHAR_JoinParty_Main(toindex, autoindex);
    Autox = autox[RAND(0, 1)];
    Autoy = autoy[RAND(0, 1)];

    CHAR_warpToSpecificPoint(toindex, CHAR_getInt(autoindex, CHAR_FLOOR),
                             CHAR_getInt(autoindex, CHAR_X) + Autox,
                             CHAR_getInt(autoindex, CHAR_Y) + Autoy);
    ActionNpc_CheckMenuFree(meindex, toindex, npcarg, select1);
    return;
    break;
  }

  lssproto_WN_send(fd, windowtype, buttontype, windowno,
                   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_PartyAutoWindowTalked(int meindex, int talkerindex, int seqno,
                               int select, char *data) {
  if (select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
    return;
  switch (seqno) {
  case NPC_TRANSERMAN_START:
    break;
  case NPC_TRANSERMAN_SELECT:
    NPC_PartyAuto_selectWindow(meindex, talkerindex, WINDOW_SELECT, atoi(data));
    break;
  case NPC_TRANSERMAN_WARP:
    if (select == WINDOW_BUTTONTYPE_CANCEL)
      return;
    if (select == WINDOW_BUTTONTYPE_NEXT) {
      NPC_PartyAuto_selectWindow(meindex, talkerindex, WINDOW_NEXT, atoi(data));
      return;
    }
    if (select == WINDOW_BUTTONTYPE_PREV) {
      NPC_PartyAuto_selectWindow(meindex, talkerindex, WINDOW_PREV, atoi(data));
      return;
    }
    NPC_PartyAuto_selectWindow(meindex, talkerindex, WINDOW_WARP, atoi(data));
    break;
  case NPC_TRANSERMAN_END:
    NPC_PartyAuto_selectWindow(meindex, talkerindex, WINDOW_END, atoi(data));
    break;
  }
}

int PartyAuto_ShowMenulist(int charaindex) {
  int floor, i, j, partynum, playernum = CHAR_getCharNum();
  int result = 1;

  floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  for (i = 0; i < playernum; i++) {
    if (!CHAR_CHECKINDEX(i) || charaindex == i)
      continue; // 검사할 캐릭터가 아닐 경우 걸러내기
    if (CHAR_getInt(i, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC &&
        CHAR_getInt(i, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    if (CHAR_getInt(i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
        CHAR_getWorkInt(i, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER)
      continue;
    if (floor != CHAR_getInt(i, CHAR_FLOOR))
      continue; // 플레이어와 동일한 맵에 있는 인원 걸러내기
    //	if(CHAR_getWorkInt(i,CHAR_WORKPARTYMODE)!=CHAR_PARTY_LEADER) continue;
    //// 리더 아닌 플레이어 걸러내기
    partynum = 1;
    for (j = 1; j < CHAR_PARTYMAX; j++) {
      int partindex = CHAR_getPartyIndex(i, j);
      if (CHAR_CHECKINDEX(partindex))
        partynum++;
    }
    if (partynum == CHAR_PARTYMAX)
      continue; // 파티원 풀이면 안됨
    result++;
  }
  return (result - 3) / 7 + 1;
}

BOOL GetStrMenuPart(int charaindex, char *token, int index) { //
  int floor, i, j, partynum,
      playernum = CHAR_getCharNum(); // CHAR_getPlayerMaxNum();
  char StrMenu[256];
  char temp[5];
  int talkNo = index * 7 + 1;
  int checkcnt = index * 7 + 1; // 8
  int cnt = 0;
  int printcnt = 0;
  BOOL FINDS = FALSE;

  floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  sprintf(token, "               《 파티 검색 시스템 》\n");
  for (i = 0; i < playernum; i++) {
    if (!CHAR_CHECKINDEX(i) || charaindex == i)
      continue; // 검사할 캐릭터가 아닐 경우 걸러내기
    if (CHAR_getInt(i, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC &&
        CHAR_getInt(i, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    if (CHAR_getInt(i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
        CHAR_getWorkInt(i, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER)
      continue;
    if (floor != CHAR_getInt(i, CHAR_FLOOR))
      continue; // 플레이어와 동일한 맵에 있는 인원 걸러내기
    // if(CHAR_getWorkInt(i,CHAR_WORKPARTYMODE)!=CHAR_PARTY_LEADER) continue; //
    // 리더 아닌 플레이어 걸러내기
    FINDS = TRUE;
    partynum = 1;
    for (j = 1; j < CHAR_PARTYMAX; j++) {
      int partindex = CHAR_getPartyIndex(i, j);
      if (CHAR_CHECKINDEX(partindex))
        partynum++;
    }
    if (partynum == CHAR_PARTYMAX)
      continue; // 파티원 풀이면 안됨
    cnt++;
    if (checkcnt > cnt)
      continue; // 8 1 2 3 4 5 6 7

    talkNo++;
    sprintf(temp, "%s", CHAR_getFlg(i, CHAR_ISPARTY) ? "ON" : "OFF");
    if (CHAR_getInt(i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC) {
      sprintf(StrMenu, "%s[최소 순:%d 최대 순: %d] %d명\n",
              CHAR_getChar(i, CHAR_NAME),
              CHAR_getWorkInt(i, CHAR_WORKPARTYLOWDEX),
              CHAR_getWorkInt(i, CHAR_WORKPARTYHIGHDEX), partynum);
    } else {
      sprintf(StrMenu, "%s[%s] %d명 상태[%s]\n", CHAR_getChar(i, CHAR_NAME),
              CHAR_getChar(i, CHAR_OWNTITLE), partynum, temp);
    }
    strcat(token, StrMenu);
    if (talkNo > (index * 7 + 7))
      return FINDS;
  }
  return FINDS;
}

BOOL GetSelectMenu(int charaindex, int select, char *StrMenu, char *check) {
  int floor, i, j, partynum, playernum = CHAR_getCharNum();
  int cnt = 0;

  floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  for (i = 0; i < playernum; i++) {
    if (!CHAR_CHECKINDEX(i) || charaindex == i)
      continue; // 검사할 캐릭터가 아닐 경우 걸러내기
    if (CHAR_getInt(i, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC &&
        CHAR_getInt(i, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      continue;
    if (CHAR_getInt(i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
        CHAR_getWorkInt(i, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER)
      continue;
    if (floor != CHAR_getInt(i, CHAR_FLOOR))
      continue; // 플레이어와 동일한 맵에 있는 인원 걸러내기
    // if(CHAR_getWorkInt(i,CHAR_WORKPARTYMODE)!=CHAR_PARTY_LEADER) continue; //
    // 리더 아닌 플레이어 걸러내기
    sprintf(check, "%s", CHAR_getFlg(i, CHAR_ISPARTY) ? "ON" : "OFF");

    partynum = 1;
    for (j = 1; j < CHAR_PARTYMAX; j++) {
      int partindex = CHAR_getPartyIndex(i, j);
      if (CHAR_CHECKINDEX(partindex))
        partynum++;
    }
    if (partynum == CHAR_PARTYMAX)
      continue; // 파티원 풀이면 안됨
    cnt++;
    if (CHAR_getInt(i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC) {
      sprintf(StrMenu, "%s[최소 순:%d 최대 순: %d] %d명\n",
              CHAR_getChar(i, CHAR_NAME),
              CHAR_getWorkInt(i, CHAR_WORKPARTYLOWDEX),
              CHAR_getWorkInt(i, CHAR_WORKPARTYHIGHDEX), partynum);
    } else {
      sprintf(StrMenu, "%s[%s] 인원: %d명\n", CHAR_getChar(i, CHAR_NAME),
              CHAR_getChar(i, CHAR_OWNTITLE), partynum);
    }
    CHAR_setInt(charaindex, CHAR_AUTOPARTYINDEX, i);
    if (select == cnt)
      return TRUE;
  }
  return FALSE;
}

#endif
