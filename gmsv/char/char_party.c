#include "version.h"
#include <stdio.h>

#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "configfile.h"
#include "family.h" // shan
#include "lssproto_serv.h"
#include "npc_airplane.h" // Arminius 7.10 Airplane
#include "npc_bus.h"
#include "npcutil.h"
#include "object.h"
#include "readmap.h"
#ifdef _ITEM_QUITPARTY
#include "init.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
// shan add
extern struct FM_PKFLOOR fmpkflnum[FAMILY_FMPKFLOOR];

/*------------------------------------------------------------
 * 由□  奴楮  及末□旦
 ------------------------------------------------------------*/

/*------------------------------------------------------------
 * 坞中化中月由□  奴  毛茧允
 * 卅仃木壬-1毛忒允［
 ------------------------------------------------------------*/
int CHAR_getEmptyPartyArray(int charaindex) {
  int i = -1;
  int rc = FALSE;
  int toindex;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    toindex = charaindex;
  } else {
    toindex = CHAR_getPartyIndex(charaindex, 0);
  }
  if (CHAR_CHECKINDEX(toindex)) {
    for (i = 1; i < CHAR_PARTYMAX; i++) {
      if (CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1) == -1) {
        rc = TRUE;
        break;
      }
    }
  }
  return (rc ? i : -1);
}
/*------------------------------------------------------------
 *   端卞由□  奴卞  月质
 *
 *  charaindex		int		愤坌
 *  targetindex		int		  月锹澎及谛
 ------------------------------------------------------------*/
BOOL CHAR_JoinParty_Main(int charaindex, int targetindex) {
  int firstflg = FALSE;
  int i;
  char c[3];
  char buf[64], buf1[64];
  int party_expnum = 0;
  int partynum = 1;
  int toindex;
  int parray;

  /* 褪互中凶日娄匀舰曰请允 */
  if (CHAR_getWorkInt(targetindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    toindex = targetindex;
  } else {
    toindex = CHAR_getPartyIndex(targetindex, 0);
  }
  if (!CHAR_CHECKINDEX(toindex)) {
    print(" %s:%d err\n", __FILE__, __LINE__);
    return FALSE;
  }
#ifdef _ALLBLUES_LUA_1_5
  if (FreePartyJoin(charaindex, toindex) == FALSE) {
    return FALSE;
  }
#endif
  char token[256];
  int battlepet, petindex;
  battlepet = CHAR_getInt(charaindex, CHAR_DEFAULTPET);
  petindex = CHAR_getCharPet(charaindex, battlepet);

  if (CHAR_getInt(targetindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC) {
    if (CHAR_getInt(charaindex, CHAR_DEFAULTPET) != -1) {
      if (CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) <
              CHAR_getWorkInt(targetindex, CHAR_WORKPARTYLOWDEX) ||
          CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) >
              CHAR_getWorkInt(targetindex, CHAR_WORKPARTYHIGHDEX) ||
          CHAR_getWorkInt(petindex, CHAR_WORKQUICK) <
              CHAR_getWorkInt(targetindex, CHAR_WORKPARTYLOWDEX) ||
          CHAR_getWorkInt(petindex, CHAR_WORKQUICK) >
              CHAR_getWorkInt(targetindex, CHAR_WORKPARTYHIGHDEX)) {
        sprintf(
            token,
            "嚎 颇萍 鉴惯仿 炼扒俊 嘎瘤 臼栏骨肺 颇萍俊 曼咯且 荐 绝嚼聪促！");
        CHAR_talkToCli(charaindex, -1, token, CHAR_COLORRED);
        return FALSE;
      }
    } else if (CHAR_getInt(charaindex, CHAR_DEFAULTPET) == -1) {
      if (CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) <
              CHAR_getWorkInt(targetindex, CHAR_WORKPARTYLOWDEX) ||
          CHAR_getWorkInt(charaindex, CHAR_WORKQUICK) >
              CHAR_getWorkInt(targetindex, CHAR_WORKPARTYHIGHDEX)) {
        sprintf(
            token,
            "嚎 颇萍 鉴惯仿 炼扒俊 嘎瘤 臼栏骨肺 颇萍俊 曼咯且 荐 绝嚼聪促！");
        CHAR_talkToCli(charaindex, -1, token, CHAR_COLORRED);
        return FALSE;
      }
    } else {
      return TRUE;
    }
  }

  /* 锹澎由□  奴及谛醒反    井＂ */
  parray = CHAR_getEmptyPartyArray(toindex);
  if (parray == -1) {
    print("%s : %d err\n", __FILE__, __LINE__);
    return FALSE;
  }
  /* 窒手  仄  褪及凛反褪卞卅匀凶CA毛霜耨允月 */
  if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE) {
    CHAR_sendLeader(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX), 1);
    /* 锹澎及橇谪及踏五晶尹 */
    /* 褪卞卅月 */
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYMODE, 1);
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYINDEX1, toindex);
    firstflg = TRUE;
  }
  CHAR_setWorkInt(toindex, parray + CHAR_WORKPARTYINDEX1, charaindex);

  CHAR_setWorkChar(charaindex, CHAR_WORKWALKARRAY, "");

  CHAR_setWorkInt(charaindex, CHAR_WORKPARTYMODE, CHAR_PARTY_CLIENT);

  CHAR_setWorkInt(charaindex, CHAR_WORKPARTYINDEX1, toindex);

#ifdef _CHAR_PARTAUTO
  CHAR_setInt(charaindex, CHAR_AUTOPARTYINDEX, -1);
#endif

  if (firstflg) {
    CHAR_sendStatusString(toindex, "N0");
  }

  for (i = 0; i < CHAR_PARTYMAX; i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
      snprintf(c, sizeof(c), "N%d", i);
      CHAR_sendStatusString(charaindex, c);
    }
  }
  for (i = 1; i <= CHAR_PARTYMAX; i++) { // 弥措 颇萍盔 荐 父怒 馆汗
    int index =
        CHAR_getPartyIndex(toindex, i); // 颇萍 牢郸胶甫 toindex俊 淬绰吝
    if (CHAR_CHECKINDEX(index)) { // 弊 牢郸胶啊 粮犁窍绰 牢郸胶 牢瘤 犬牢
                                  // 嘎栏搁 关俊波 角青
      partynum += 1; // 粮犁窍绰 颇萍搁 颇萍盔 荐 +1
    }
  }

  if (partynum == 2)
    party_expnum = 5; // 颇萍盔 荐俊 蝶扼 版氰摹 硅啦
  else if (partynum == 3)
    party_expnum = 10;
  else if (partynum == 4)
    party_expnum = 15;
  else if (partynum == 5)
    party_expnum = 20;
  else if (partynum > 5 || partynum < 1)
    party_expnum = 20;

  snprintf(buf, sizeof(buf),
           "[%s](捞)啊 颇萍俊 曼咯沁嚼聪促. (眠啊版氰摹 +%d％)",
           CHAR_getChar(charaindex, CHAR_NAME), party_expnum);

  for (i = 0; i < CHAR_PARTYMAX; i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
      if (index != charaindex) {
        snprintf(c, sizeof(c), "N%d", parray);
        CHAR_sendStatusString(index, c);
        CHAR_talkToCli(index, -1, buf, CHAR_COLORYELLOW);
      } else {
        if (CHAR_getInt(targetindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC) {
          snprintf(buf1, sizeof(buf1),
                   "府歹嚎 颇萍俊 颇萍俊 曼咯钦聪促. (眠啊版氰摹 +%d％)",
                   party_expnum);
          CHAR_talkToCli(index, -1, buf1, CHAR_COLORYELLOW);
        } else {
          snprintf(buf1, sizeof(buf1),
                   "颇萍俊 曼咯沁嚼聪促. (眠啊版氰摹 +%d％)", party_expnum);
          CHAR_talkToCli(index, -1, buf1, CHAR_COLORYELLOW);
        }
      }
      CHAR_complianceParameter(index);
      CHAR_complianceParameter(index);
    }
  }
  return TRUE;
}
/*------------------------------------------------------------
 * 由□  奴卞  欠丹午允月［
 ------------------------------------------------------------*/
BOOL CHAR_JoinParty(int charaindex) {

  int result = -1;
  int x, y;
  OBJECT object;
  int found = FALSE;
  int fd;
  int cnt;
  int i;

  fd = getfdFromCharaIndex(charaindex);
  if (fd == -1) {
    print("%s : %d err\n", __FILE__, __LINE__);
    return FALSE;
  }

  /* 愤坌互由□  奴赚氏匹凶日蛲   */
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    lssproto_PR_send(fd, 1, FALSE);
    return FALSE;
  }

  /*   及蟆及甄  毛  月 */
  CHAR_getCoordinationDir(CHAR_getInt(charaindex, CHAR_DIR),
                          CHAR_getInt(charaindex, CHAR_X),
                          CHAR_getInt(charaindex, CHAR_Y), 1, &x, &y);

  /* 赓渝祭允月 */
  for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
    CONNECT_setJoinpartycharaindex(fd, i, -1);
  }
  cnt = 0;

  /*愤坌及  及蟆及平乓仿毛潸  允月 */

  for (object = MAP_getTopObj(CHAR_getInt(charaindex, CHAR_FLOOR), x, y);
       object; object = NEXT_OBJECT(object)) {
    int toindex;
    int parray;
    int objindex = GET_OBJINDEX(object);
    int targetindex = -1;

    /* 平乓仿弁正□元扎卅中 */
    if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
      continue;
    toindex = OBJECT_getIndex(objindex);
    print("charaindex:%d\n", toindex);

    // shan begin
    if (CHAR_getInt(charaindex, CHAR_FMINDEX) > 0 &&
        CHAR_getInt(toindex, CHAR_FMINDEX) > 0) {
      for (i = 0; i < FAMILY_FMPKFLOOR; i++) {
        if (fmpkflnum[i].fl == CHAR_getInt(charaindex, CHAR_FLOOR))
          if (CHAR_getInt(charaindex, CHAR_FMINDEX) !=
              CHAR_getInt(toindex, CHAR_FMINDEX)) {
            lssproto_PR_send(fd, 1, FALSE);
            return FALSE;
          }
      }
    }
    // shan end
    print("fm pass\n");

    /* 皿伊奶乩□及凛 */
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
        || CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC
#endif
    ) {
      print("p o top\n");
      found = TRUE;
      /* 锹澎互阂分匀凶日褪毛娄匀舰曰请允 */
      if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
        targetindex = CHAR_getWorkInt(toindex, CHAR_WORKPARTYINDEX1);
        if (!CHAR_CHECKINDEX(targetindex)) {
          print(" %s:%d err\n", __FILE__, __LINE__);
          continue;
        }
        if (CHAR_getInt(targetindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
          continue;
        }
      } else {
        targetindex = toindex;
      }

      /*   褪午  ㄠ汹动  卞中月井 */
      if (NPC_Util_CharDistance(charaindex, targetindex) > 1) {
        continue;
      }

      /* 爵    反匹卅中仪［*/

      // if( CHAR_getWorkInt( targetindex, CHAR_WORKBATTLEMODE) !=
      // BATTLE_CHARMODE_NONE ){ 	continue;
      // }

      /* PARTY FLAG */
      // if( !CHAR_getFlg( targetindex, CHAR_ISPARTY) ) continue;

#ifdef _NO_JOIN_FLOOR
      {
        int i;
        int floor = CHAR_getInt(charaindex, CHAR_FLOOR);
        for (i = 0; i < 32; i++) {
          if (floor == getNoJoinFloor(i)) {
            CHAR_talkToCli(charaindex, -1,
                           "【组队失败】该地图无法组队哦 ~囧~ 自求多福吧！",
                           CHAR_COLORYELLOW);
            break;
          }
        }
        if (i < 32) {
          continue;
        }
      }
#endif

#ifdef _ANGEL_SUMMON
      if (CHAR_getWorkInt(targetindex, CHAR_WORKANGELMODE) == TRUE) {
        CHAR_talkToCli(charaindex, -1, "使者不可以当领队。", CHAR_COLORYELLOW);
        continue;
      }
#endif
    }
    /* 穴件乒旦田旦互中月凛反］谛棉方曰穸燮允月［ */
    else if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
      print("bus\n");
      targetindex = toindex;
      cnt = 0;
      if (!NPC_BusCheckJoinParty(toindex, charaindex, TRUE)) {
        /* 椭瘀毛  凶今卅井匀凶［醮棉  月及反蔽歹月［谛棉及质  手仄卅中［
         * 支支仇仄中及匹［
         */
        break;
      }
      { // Arminius 7.10 Airplane
        int busimg = CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER);
        if ((busimg != 100355) && (busimg != 100461)) {
          CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, busimg);
          CHAR_sendCToArroundCharacter(
              CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
          // Robin debug 01/11/21
          if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
            CHAR_setInt(charaindex, CHAR_RIDEPET, -1);
            CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_RIDEPET);
          }
          /*
CHAR_sendPMEToArroundCharacterFLXY(charaindex,
  CHAR_getInt( charaindex, CHAR_FLOOR),
  CHAR_getInt( charaindex, CHAR_X),
  CHAR_getInt( charaindex, CHAR_Y),
  0,1,CHAR_getInt( charaindex, CHAR_PETMAILEFFECT)
  );
          */
        }
      }
    }
    /* 皿伊奶乩□坭反穴件乒旦田旦动陆反  骰允月 */
    else {
      print("none\n");
      continue;
    }
    /* 锹澎由□  奴及谛醒反    井＂ */
    parray = CHAR_getEmptyPartyArray(targetindex);
    if (parray == -1)
      continue;

    /* 仇仇引匹仁木壬     */
    CONNECT_setJoinpartycharaindex(fd, cnt, toindex);
    cnt++;
    if (cnt == CONNECT_WINDOWBUFSIZE)
      break;

    /* 穴件乒旦田旦  苇仄分中］伙□皿毛  仃月［ */
    if (CHAR_getInt(targetindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS)
      break;
  }

  if (cnt == 0) {
    if (found == TRUE) {
      CHAR_talkToCli(charaindex, -1, "颇萍俊 曼咯 且 荐 绝嚼聪促。",
                     CHAR_COLORYELLOW);
    }
    result = FALSE;
  } else if (cnt == 1) {
    {
      print("cnt1\n");
      CHAR_JoinParty_Main(charaindex, CONNECT_getJoinpartycharaindex(fd, 0));
      result = TRUE;
    }
  } else {
    int strlength;
    char msgbuf[1024];
    char escapebuf[2048];
    strcpy(msgbuf, "1\n和谁组成团队呢？\n");
    strlength = strlen(msgbuf);
    for (i = 0; CONNECT_getJoinpartycharaindex(fd, i) != -1 &&
                i < CONNECT_WINDOWBUFSIZE;
         i++) {
      char *a = CHAR_getChar(CONNECT_getJoinpartycharaindex(fd, i), CHAR_NAME);
      char buf[256];
      snprintf(buf, sizeof(buf), "%s\n", a);
      if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
        print("%s:%d视窗讯息buffer不足。\n", __FILE__, __LINE__);
        break;
      }
      strcpy(&msgbuf[strlength], buf);
      strlength += strlen(buf);
    }
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                     CHAR_WINDOWTYPE_SELECTPARTY, -1,
                     makeEscapeString(msgbuf, escapebuf, sizeof(escapebuf)));
  }

  if (result != -1) {
    lssproto_PR_send(fd, 1, result);
  }

  return result;
}

static BOOL CHAR_DischargePartySub(int charaindex, int msgflg) {
  char buf[64], c[3];
  int toindex, flg, i;
#ifdef _ITEM_QUITPARTY
  int j = 0, k;
#endif

  if (!CHAR_CHECKINDEX(charaindex))
    return FALSE;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    int pindex, airplaneflag = 0;
    // Arminius 7.10 Airplane
    if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
      if ((CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) != 100355) &&
          (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) != 100461)) {
        airplaneflag = 1;
      }
    }
    for (i = 0; i < CHAR_PARTYMAX; i++) {
      pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(pindex)) {
        int fd = getfdFromCharaIndex(pindex);
        CHAR_setWorkInt(pindex, CHAR_WORKPARTYINDEX1, -1);
        CHAR_setWorkInt(pindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
#ifdef _PLAYER_NPC
        if (CHAR_getInt(pindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC ||
            CHAR_getInt(pindex, CHAR_WHICHTYPE) == CHAR_TYPELUANPC) {
          CHAR_CharaDeleteHavePet(pindex);
          CHAR_CharaDelete(pindex);
          continue;
        }
#endif
        if (msgflg) {
          CHAR_talkToCli(pindex, -1, "颇萍秦魂！", CHAR_COLORYELLOW);
#ifdef _ITEM_QUITPARTY
          // won fix
          for (j = 0; j < CHAR_getMaxInventory(pindex); j++) {
            int del_item_index = CHAR_getItemIndex(pindex, j);
            if (ITEM_CHECKINDEX(del_item_index)) { //格子内有道具
              for (k = 0; k < itemquitparty_num; k++) {
                if (ITEM_getInt(del_item_index, ITEM_ID) ==
                    atoi(Disappear_Item[k].string)) { //若等於所设定的道具ID
                  CHAR_setItemIndex(pindex, j, -1); //格子内道具消失
                  ITEM_endExistItemsOne(del_item_index);
                  CHAR_sendItemDataOne(pindex, j);
                }
              }
            }
          }
#endif
        }
        if (fd != -1) {
          lssproto_PR_send(fd, 0, 1);
        }
        if (airplaneflag &&
            (CHAR_getInt(pindex, CHAR_WHICHTYPE) != CHAR_TYPEBUS)) {
          int bi, bbi, ii, category;
          bbi = CHAR_getInt(pindex, CHAR_BASEBASEIMAGENUMBER);
          ii = CHAR_getItemIndex(pindex, CHAR_ARM);
          if (!ITEM_CHECKINDEX(ii))
            category = ITEM_FIST;
          else
            category = ITEM_getInt(ii, ITEM_TYPE);
          bi = CHAR_getNewImagenumberFromEquip(bbi, category);
          if (bi == -1)
            bi = bbi;
          CHAR_setInt(pindex, CHAR_BASEIMAGENUMBER, bi);
          // Robin 0810 debug
          CHAR_complianceParameter(pindex);
          CHAR_sendCToArroundCharacter(
              CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX));
        }
      }
      CHAR_setWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1, -1);
    }
    CHAR_sendLeader(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX), 0);
  } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
             CHAR_PARTY_CLIENT) {
    int myarray = -1;
    int fd = getfdFromCharaIndex(charaindex);
    CHAR_setWorkInt(charaindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
    toindex = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
    if (!CHAR_CHECKINDEX(toindex))
      return FALSE;
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
      NPC_BusCheckAllowItem(toindex, charaindex, TRUE);
      // Arminius 7.9 Airplane
      if ((CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER) != 100355) &&
          (CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER) != 100461)) {
        int bi, bbi, ii, category;
        bbi = CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER);
        ii = CHAR_getItemIndex(charaindex, CHAR_ARM);
        if (!ITEM_CHECKINDEX(ii))
          category = ITEM_FIST;
        else
          category = ITEM_getInt(ii, ITEM_TYPE);
        // bi=CHAR_getNewImagenumberFromEquip(bbi,category);
        // if (bi==-1) bi=bbi;
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, bbi);
        if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
          CHAR_setInt(charaindex, CHAR_RIDEPET, -1);
          CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_RIDEPET);
        }
        // Robin 0810 debug
        CHAR_complianceParameter(charaindex);

        CHAR_sendCToArroundCharacter(
            CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
        if (CHAR_getWorkInt(toindex, CHAR_NPCWORKINT5) == 1) {
          if (CHAR_getInt(charaindex, CHAR_LASTTALKELDER) >= 0) {
            int fl, x, y;
            CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
                                  &fl, &x, &y);
            CHAR_warpToSpecificPoint(charaindex, fl, x, y);
          }
        }
      }
    }
    CHAR_setWorkInt(charaindex, CHAR_WORKPARTYINDEX1, -1);
    for (i = 0; i < CHAR_PARTYMAX; i++) {
      int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
        if (index == charaindex) {
          myarray = i;
          break;
        }
      }
    }
    if (myarray == CHAR_PARTYMAX) {
      print("DischargeParty(): 真奇怪！");
      return FALSE;
    }
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYINDEX1 + myarray, -1);
    snprintf(buf, sizeof(buf), "[%s]丛膊辑 颇萍俊辑 呕硼窍看嚼聪促！",
             CHAR_getChar(charaindex, CHAR_NAME));
    if (msgflg) {
      CHAR_talkToCli(charaindex, -1, "颇萍俊辑 呕硼窍看嚼聪促！",
                     CHAR_COLORYELLOW);
#ifdef _ITEM_QUITPARTY
      // won fix
      for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
        int del_item_index = CHAR_getItemIndex(charaindex, j);
        if (ITEM_CHECKINDEX(del_item_index)) { //格子内有道具
          for (j = 0; j < itemquitparty_num; j++) {
            if (ITEM_getInt(del_item_index, ITEM_ID) ==
                atoi(Disappear_Item[j].string)) { //若等於所设定的道具ID
              CHAR_setItemIndex(charaindex, i, -1); //格子内道具消失
              ITEM_endExistItemsOne(del_item_index);
              CHAR_sendItemDataOne(charaindex, i);
            }
          }
        }
      }
#endif
    }
    snprintf(c, sizeof(c), "N%d", myarray);
    if (fd != -1) {
      lssproto_PR_send(fd, 0, 1);
    }
    for (i = 0; i < CHAR_PARTYMAX; i++) {
      int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
#ifdef _ITEM_QUITPARTY
        // won fix
        for (j = 0; j < CHAR_getMaxInventory(index); j++) {
          int del_item_index = CHAR_getItemIndex(index, j);
          if (ITEM_CHECKINDEX(del_item_index)) { //格子内有道具
            for (k = 0; k < itemquitparty_num; k++) {
              if (ITEM_getInt(del_item_index, ITEM_ID) ==
                  atoi(Disappear_Item[k].string)) { //若等於所设定的道具ID
                CHAR_setItemIndex(index, j, -1);    //格子内道具消失
                ITEM_endExistItemsOne(del_item_index);
                CHAR_sendItemDataOne(index, j);
              }
            }
          }
        }
#endif
        if (msgflg) {
          CHAR_talkToCli(index, -1, buf, CHAR_COLORYELLOW);
        }
        CHAR_sendStatusString(index, c);
      }
    }
    flg = FALSE;
    for (i = 1; i < CHAR_PARTYMAX; i++) {
      int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
        flg = TRUE;
        break;
      }
    }
    if (!flg) {
      CHAR_setWorkInt(toindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
      CHAR_sendLeader(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX), 0);
    } else {
      POINT start, end;
      int previndex = toindex;
      end.x = CHAR_getInt(charaindex, CHAR_X);
      end.y = CHAR_getInt(charaindex, CHAR_Y);
      for (i = 1; i < CHAR_PARTYMAX; i++) {
        int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(index)) {
          if (NPC_Util_CharDistance(index, previndex) > 1) {
            int parent_dir;
            start.x = CHAR_getInt(index, CHAR_X);
            start.y = CHAR_getInt(index, CHAR_Y);
            parent_dir = NPC_Util_getDirFromTwoPoint(&start, &end);
            end = start;
            if (parent_dir != -1) {
              CHAR_walk(index, parent_dir, 0);
            }
          }
          previndex = index;
        }
      }
    }
  }
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    if (CHAR_DischargePartyLeader(charaindex, msgflg) == FALSE)
      return FALSE;
  } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
             CHAR_PARTY_CLIENT) {
    if (CHAR_DischargePartyClient(charaindex, msgflg) == FALSE)
      return FALSE;
  }
  return TRUE;
}
BOOL CHAR_DischargeParty(int charaindex, int flg) {
  return CHAR_DischargePartySub(charaindex, 1);
}

BOOL CHAR_DischargePartyNoMsg(int charaindex) {
  return CHAR_DischargePartySub(charaindex, 0);
}

/*------------------------------------------------------------
 * 愤坌互伉□母□井升丹井毛霜耨允月［
 ------------------------------------------------------------*/
void CHAR_sendLeader(int objindex, int leader) {
  int opt[1];
  opt[0] = leader;
  CHAR_sendWatchEvent(objindex, CHAR_ACTLEADER, opt, 1, TRUE);
}
/*------------------------------------------------------------
 * 醮棉毛赐    CHAR_WORKPARTYINDEX)隙烂匹平乓仿index毛娄匀舰月［
 * 愤坌互褪匹手阂匹手    ［
 ------------------------------------------------------------*/
int CHAR_getPartyIndex(int index, int num) {
  int nindex = -1;

  /* 醮棉及奶件犯永弁旦毛潸   */
  /* 褪及桦宁 */
  if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    nindex = CHAR_getWorkInt(index, CHAR_WORKPARTYINDEX1 + num);
  }
  /* 阂及桦宁 */
  else {
    int oyaindex = CHAR_getWorkInt(index, CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(oyaindex)) {
      nindex = CHAR_getWorkInt(oyaindex, CHAR_WORKPARTYINDEX1 + num);
    }
  }
  return nindex;
}
/*------------------------------------------------------------
 * 丢永本□斥毛霜耨允月［
 * 醮棉互中木壬公及醮棉卞手丢永本□斥毛霜耨允月［
 ------------------------------------------------------------*/
void CHAR_talkToCliAndParty(int talkedcharaindex, int talkcharaindex,
                            char *message, CHAR_COLOR color) {
  int i;
  /* 引内愤坌 */
  CHAR_talkToCli(talkedcharaindex, talkcharaindex, message, color);

  for (i = 0; i < CHAR_PARTYMAX; i++) {
    int index = CHAR_getPartyIndex(talkedcharaindex, i);
    if (CHAR_CHECKINDEX(index) && index != talkedcharaindex) {
      CHAR_talkToCli(index, talkcharaindex, message, color);
    }
  }
}
BOOL CHAR_DischargePartyLeader(int charaindex, int msgflg) {
  char buf[64], c[3], buf1[64];
  int toindex, flg, i;
  int party_expnum = 0;
  int partynum = 0;
#ifdef _ITEM_QUITPARTY
  int j = 0, k;
#endif

  int pindex, airplaneflag = 0;
  // Arminius 7.10 Airplane
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
    if ((CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) != 100355) &&
        (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) != 100461)) {
      airplaneflag = 1;
    }
  }
  for (i = 0; i < CHAR_PARTYMAX; i++) {
    pindex = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(pindex)) {
      int fd = getfdFromCharaIndex(pindex);
      CHAR_setWorkInt(pindex, CHAR_WORKPARTYINDEX1, -1);
      CHAR_setWorkInt(pindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
      if (msgflg) {
        CHAR_talkToCli(pindex, -1, "颇萍啊 秦魂登菌绢夸！", CHAR_COLORYELLOW);
#ifdef _ITEM_QUITPARTY
        // won fix
        for (j = 0; j < CHAR_getMaxInventory(pindex); j++) {
          int del_item_index = CHAR_getItemIndex(pindex, j);
          if (ITEM_CHECKINDEX(del_item_index)) { //格子内有道具
            for (k = 0; k < itemquitparty_num; k++) {
              if (ITEM_getInt(del_item_index, ITEM_ID) ==
                  atoi(Disappear_Item[k].string)) { //若等於所设定的道具ID
                CHAR_setItemIndex(pindex, j, -1); //格子内道具消失
                ITEM_endExistItemsOne(del_item_index);
                CHAR_sendItemDataOne(pindex, j);
              }
            }
          }
        }
#endif
      }
      if (fd != -1) {
        lssproto_PR_send(fd, 0, 1);
      }
      // Arminius 7.10 Airplane
      if (airplaneflag &&
          (CHAR_getInt(pindex, CHAR_WHICHTYPE) != CHAR_TYPEBUS)) {
        int bi, bbi, ii, category;
        bbi = CHAR_getInt(pindex, CHAR_BASEBASEIMAGENUMBER);
        ii = CHAR_getItemIndex(pindex, CHAR_ARM);
        if (!ITEM_CHECKINDEX(ii))
          category = ITEM_FIST;
        else
          category = ITEM_getInt(ii, ITEM_TYPE);
        bi = CHAR_getNewImagenumberFromEquip(bbi, category);
        if (bi == -1)
          bi = bbi;
        CHAR_setInt(pindex, CHAR_BASEIMAGENUMBER, bi);
        // Robin 0810 debug
        CHAR_complianceParameter(pindex);
        CHAR_sendCToArroundCharacter(
            CHAR_getWorkInt(pindex, CHAR_WORKOBJINDEX));
      }
    }
    CHAR_setWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1, -1);
  }
  CHAR_sendLeader(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX), 0);
}

BOOL CHAR_DischargePartyClient(int charaindex, int msgflg) {
  char buf[64], c[3], buf1[64];
  int toindex, flg, i;
  int party_expnum = 0;
  int partynum = 0;
#ifdef _ITEM_QUITPARTY
  int j = 0, k;
#endif

  int myarray = -1;
  int fd = getfdFromCharaIndex(charaindex);
  CHAR_setWorkInt(charaindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
  toindex = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
  if (!CHAR_CHECKINDEX(toindex))
    return FALSE;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
    NPC_BusCheckAllowItem(toindex, charaindex, TRUE);
    // Arminius 7.9 Airplane
    if ((CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER) != 100355) &&
        (CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER) != 100461)) {
      int bi, bbi, ii, category;

      bbi = CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER);
      ii = CHAR_getItemIndex(charaindex, CHAR_ARM);
      if (!ITEM_CHECKINDEX(ii))
        category = ITEM_FIST;
      else
        category = ITEM_getInt(ii, ITEM_TYPE);
      bi = CHAR_getNewImagenumberFromEquip(bbi, category);
      if (bi == -1)
        bi = bbi;
      CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, bi);

      // Robin 0810 debug
      CHAR_complianceParameter(charaindex);

      CHAR_sendCToArroundCharacter(
          CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
      if (CHAR_getWorkInt(toindex, CHAR_NPCWORKINT5) == 1) {
        if (CHAR_getInt(charaindex, CHAR_LASTTALKELDER) >= 0) {
          int fl, x, y;
          CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
                                &fl, &x, &y);
          CHAR_warpToSpecificPoint(charaindex, fl, x, y);
        }
      }
    }
  }
  CHAR_setWorkInt(charaindex, CHAR_WORKPARTYINDEX1, -1);
  for (i = 0; i < CHAR_PARTYMAX; i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
      if (index == charaindex) {
        myarray = i;
        break;
      }
    }
  }
  if (myarray == CHAR_PARTYMAX) {
    print("DischargeParty(): 俊矾");
    return FALSE;
  }
#ifdef _PARTNER_EXP
  for (i = 1; i <= CHAR_PARTYMAX; i++) { // 弥措 颇萍盔 荐 父怒 馆汗
    int index =
        CHAR_getPartyIndex(toindex, i); // 颇萍 牢郸胶甫 toindex俊 淬绰吝
    if (CHAR_CHECKINDEX(index)) { // 弊 牢郸胶啊 粮犁窍绰 牢郸胶 牢瘤 犬牢
                                  // 嘎栏搁 关俊波 角青
      partynum += 1; // 粮犁窍绰 颇萍搁 颇萍盔 荐 +1
    }
  }

  if (partynum == 2)
    party_expnum = 5; // 颇萍盔 荐俊 蝶扼 版氰摹 硅啦
  else if (partynum == 3)
    party_expnum = 10;
  else if (partynum == 4)
    party_expnum = 15;
  else if (partynum == 5)
    party_expnum = 20;
  else if (partynum > 5 || partynum < 1)
    party_expnum = 20;
#endif
  CHAR_setWorkInt(toindex, CHAR_WORKPARTYINDEX1 + myarray, -1);
  snprintf(buf, sizeof(buf), "%s (捞)啊 颇萍俊辑 狐脸绢夸！ (眠啊 版氰摹 +%d%)",
           CHAR_getChar(charaindex, CHAR_NAME), party_expnum);
  if (msgflg) {
    snprintf(buf1, sizeof(buf1), "颇萍俊辑 狐脸绢夸！ (眠啊 版氰摹 +%d%)",
             party_expnum);
    CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
#ifdef _ITEM_QUITPARTY
    // won fix
    for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
      int del_item_index = CHAR_getItemIndex(charaindex, j);
      if (ITEM_CHECKINDEX(del_item_index)) { //格子内有道具
        for (j = 0; j < itemquitparty_num; j++) {
          if (ITEM_getInt(del_item_index, ITEM_ID) ==
              atoi(Disappear_Item[j].string)) { //若等於所设定的道具ID
            CHAR_setItemIndex(charaindex, i, -1); //格子内道具消失
            ITEM_endExistItemsOne(del_item_index);
            CHAR_sendItemDataOne(charaindex, i);
          }
        }
      }
    }
#endif
  }
  snprintf(c, sizeof(c), "N%d", myarray);
  if (fd != -1) {
    lssproto_PR_send(fd, 0, 1);
  }
  for (i = 0; i < CHAR_PARTYMAX; i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
#ifdef _ITEM_QUITPARTY
      // won fix
      for (j = 0; j < CHAR_getMaxInventory(index); j++) {
        int del_item_index = CHAR_getItemIndex(index, j);
        if (ITEM_CHECKINDEX(del_item_index)) { //格子内有道具
          for (k = 0; k < itemquitparty_num; k++) {
            if (ITEM_getInt(del_item_index, ITEM_ID) ==
                atoi(Disappear_Item[k].string)) { //若等於所设定的道具ID
              CHAR_setItemIndex(index, j, -1);    //格子内道具消失
              ITEM_endExistItemsOne(del_item_index);
              CHAR_sendItemDataOne(index, j);
            }
          }
        }
      }
#endif
      if (msgflg) {
        CHAR_talkToCli(index, -1, buf, CHAR_COLORYELLOW);
      }
      CHAR_sendStatusString(index, c);
    }
  }
  flg = FALSE;
  for (i = 1; i < CHAR_PARTYMAX; i++) {
    int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(index)) {
      flg = TRUE;
      break;
    }
  }
  if (!flg) {
    CHAR_setWorkInt(toindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
    CHAR_sendLeader(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX), 0);
  } else {

    POINT start, end;
    int previndex = toindex;
    end.x = CHAR_getInt(charaindex, CHAR_X);
    end.y = CHAR_getInt(charaindex, CHAR_Y);
    for (i = 1; i < CHAR_PARTYMAX; i++) {
      int index = CHAR_getWorkInt(toindex, i + CHAR_WORKPARTYINDEX1);
      if (CHAR_CHECKINDEX(index)) {
        if (NPC_Util_CharDistance(index, previndex) > 1) {
          int parent_dir;
          start.x = CHAR_getInt(index, CHAR_X);
          start.y = CHAR_getInt(index, CHAR_Y);
          parent_dir = NPC_Util_getDirFromTwoPoint(&start, &end);
          end = start;
          if (parent_dir != -1) {
            CHAR_walk(index, parent_dir, 0);
          }
        }
        previndex = index;
      }
    }
  }
}
