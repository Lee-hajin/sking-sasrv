#include "addressbook.h"
#include "battle.h"
#include "battle_command.h"
#include "char.h"
#include "common.h"
#include "configfile.h"
#include "event.h"
#include "family.h" // CoolFish: Family 2001/5/24
#include "handletime.h"
#include "item_event.h" // shan: blackmarket
#include "item_gen.h"
#include "log.h" //add this because the second had it
#include "lssproto_serv.h"
#include "magic.h"
#include "map_deal.h" // CoolFish: 2001/4/18
#include "net.h"
#include "object.h"
#include "pet.h"
#include "pet_skill.h"
#include "petmail.h"
#include "readmap.h"
#include "saacproto_cli.h"
#include "trade.h" // CoolFish: Trade 2001/4/18
#include "util.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif
#include "npc_healer.h"
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
#include "profession_skill.h"
#endif
#ifdef _CHATROOMPROTOCOL // (不可开) Syu ADD 聊天室频道
#include "chatroom.h"
#endif
#ifdef _ONLINE_SHOP
#include "NewBilu/newshop.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
BOOL checkStringErr(char *);

extern int luaplayernum;
// shan add
extern struct FM_PKFLOOR fmpkflnum[FAMILY_FMPKFLOOR];

#ifdef _NB_FAMILYBADGE
extern int getFamilyBadge(int index);
#endif

/* -----------------------------------------------------------------------
 * ??����?�����?���髭��髤��ǫë�����?��
 * ----------------------------------------------------------------------*/
static int Callfromcli_Util_getTargetCharaindex(int fd, int toindex) {
  int to_charaindex = -1;
  int fd_charaindex = CONNECT_getCharaindex(fd);

	/* 愤坌愤褥 */
  if (toindex == 0) {
    to_charaindex = fd_charaindex;
  }
  /* �ګë� 1 ��5 */
  else if (toindex > 0 && toindex < 6) {
    to_charaindex = CHAR_getCharPet(fd_charaindex, toindex - 1);
    if (!CHAR_CHECKINDEX(to_charaindex)) {
      to_charaindex = -1;
    }
  }
  /* ���� 6 ��10 */
  else if (toindex > 5 && toindex < 11) {
    to_charaindex = CHAR_getPartyIndex(fd_charaindex, toindex - 6);
  }
  return to_charaindex;
}
/*----------------------------------------
 * 弁仿奶失件玄互夫弘奶件允月 匹手丢乒伉卞卺户月分仃卅及匹民尼永弁反卅中
 ----------------------------------------*/
void lssproto_ClientLogin_recv(int fd, char *cdkey, char *passwd, char *mac1,
                               char *mac2, char *mac3, int servid) {
  if (CONNECT_getState(fd) == NULLCONNECT) {
    CONNECT_setState(fd, NOTLOGIN);
  }
	/*  2褐卞仇木互裟壬木月及反中中    */
	{ // ttom avoid the restore 2001/01/09
    if (CONNECT_isNOTLOGIN(fd) == FALSE) {
      print("\n the Client had  Logined fd=%d", fd);
      return;
    }
  }

  //????
  int i;
  for (i = 0; i < strlen(cdkey); i++) {
    if (cdkey[i] < 0x30 || cdkey[i] > 0x7A) {
      cdkey[i] = 0x30;
    }
  }
  //??��?
  for (i = 0; i < strlen(passwd); i++) {
    if (passwd[i] < 0x30 || passwd[i] > 0x7A) {
      passwd[i] = 0x30;
    }
  }

  if (checkStringErr(cdkey))
    return;
  if (strlen(mac1) > 64) {
    printf("\n %s ��mac1 ����?��", cdkey);
    return;
  }
  if (strlen(mac2) > 128) {
    printf("\n %s ��mac2 ����?��", cdkey);
    return;
  }
  if (strlen(mac3) > 64) {
    printf("\n %s ��mac3 ����?��", cdkey);
    return;
  }
  if (checkStringErr(passwd))
    return;
  if (strlen(cdkey) < 1 || strlen(passwd) < 1) {
    lssproto_ClientLogin_send(fd, "empty error");
    CONNECT_setCloseRequest(fd, 1);
    return;
  }
  // print( "CliLogin cdkey=%s\n" , cdkey );
  /* connect�˫���?���� */
  CONNECT_setCdkey(fd, cdkey);
  CONNECT_setPasswd(fd, passwd);
  CONNECT_setCtype(fd, CLI);

#ifdef _NEWCLISETSERVID
  CONNECT_setServid(fd, servid);
#endif
#ifdef _NEWCLISETMAC
  if (checkStringErr(mac1)) {
    strcpy(mac1, "");
  }
  if (checkStringErr(mac2)) {
    strcpy(mac2, "");
  }
  if (checkStringErr(mac3)) {
    strcpy(mac3, "");
  }
  CONNECT_setMAC(fd, mac1);
  CONNECT_setMAC2(fd, mac2);
  CONNECT_setMAC3(fd, mac3);
#endif

  unsigned long tmpip;
  int a, b, c, d;
  char ip[32];
  tmpip = CONNECT_get_userip(fd);
  a = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  b = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  c = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  d = (tmpip % 0x100);
  sprintf(ip, "%d.%d.%d.%d", a, b, c, d);

#ifdef _STOPLOGININFORMATION
  // print( "\n��???=%s \n",cdkey);
#else
  print("\n��???=%s ��?=%s IP=%s MAC1=%s MAC2=%s MAC3=%s SERVID=%d\n", cdkey,
        passwd, ip, mac1, mac2, mac3, servid);
#endif

  if (sasql_chehk_lock(cdkey)) {
    lssproto_ClientLogin_send(fd, "심각한 위반으로 인해, 당신은 보이기 때문에이 게임에 로그인 할 수 없습니다.");
    CONNECT_setCloseRequest(fd, 1);
    return;
  }
  if (sasql_chehk_lock(ip) || sasql_chehk_lock(mac1) ||
      sasql_chehk_lock(mac2)) {
    lssproto_ClientLogin_send(fd, "IP정지");
    CONNECT_setCloseRequest(fd, 1);
    return;
  }
  int res = sasql_query(cdkey, passwd);
  if (res == 2) {
    // lssproto_ClientLogin_send(fd, "귀하의 계정이 존재하지 않으면 등록되지 않은 경우 공식 웹 사이트에 등록하십시오.");
    lssproto_ClientLogin_send(fd, "귀하의 계정이 존재하지 않으면 등록되지 않은 경우 공식 웹 사이트에 등록하십시오.");
    CONNECT_setCloseRequest(fd, 1);
    return;
  } else if (res == 4) {
    lssproto_ClientLogin_send(fd, "callfromcli res 4");
    CONNECT_setCloseRequest(fd, 1);
    return;
  } else if (res == 3) {
    lssproto_ClientLogin_send(fd, "callfromcli res 3");
    CONNECT_setCloseRequest(fd, 1);
    return;
  } else if (res == 1) {
#ifdef _ALLBLUES_LUA_1_9
#ifdef _NEWCLISETMAC
    if (FreeLoginCheck(fd, cdkey, passwd, ip, mac1, mac2, mac3) == 0) {
      // lssproto_ClientLogin_send( fd, "???�ȣ�??ͧη��?��?��" );
      CONNECT_setCloseRequest(fd, 1);
      // saacproto_ACKick_send(acfd,cdkey,-1,1);
      return;
    }
#endif
#endif
    // lssproto_ClientLogin_send( fd , "ok" );
    sasql_online(cdkey, NULL, ip, mac1, mac2, mac3, 1);
    lssproto_ClientLogin_send(fd, "ok");
  }

  /*
#ifdef _NEWCLISETMAC
saacproto_ACCharLogin_send( acfd, fd, cdkey, passwd, ip, mac1 ,mac2,mac3);
#else
saacproto_ACCharLogin_send( acfd, fd, cdkey, passwd, ip);
#endif
  */
}

void lssproto_CreateNewChar_recv(int fd, int dataplacenum, char *charname,
                                 int imgno, int faceimgno, int vital, int str,
                                 int tgh, int dex, int earth, int water,
                                 int fire, int wind, int hometown) {
  char cdkey[CDKEYLEN];

  if (CONNECT_isCLI(fd) == FALSE)
    return;

  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    lssproto_CreateNewChar_send(fd, FAILED, "Not NOTLOGIN State\n");
    return;
  }
  if (strlen(charname) == 0) {
    lssproto_CreateNewChar_send(fd, FAILED, "0 length name\n");
    return;
  } else if (strlen(charname) >= 32) {
    lssproto_CreateNewChar_send(fd, FAILED, "Too long charname\n");
    return;
            // WON ADD
  } else if (strstr(charname, "gm") || strstr(charname, "GM") ||
             strstr(charname, "Gm") || strstr(charname, "gM") ||
             strstr(charname, "|") || strstr(charname, "'") ||
             strstr(charname, "=") || strstr(charname, ";")
#ifdef _UNREG_NEMA
             || (strstr(charname, getUnregname(0)) &&
                 strlen(getUnregname(0)) > 0) ||
             (strstr(charname, getUnregname(1)) &&
              strlen(getUnregname(1)) > 0) ||
             (strstr(charname, getUnregname(2)) &&
              strlen(getUnregname(2)) > 0) ||
             (strstr(charname, getUnregname(3)) &&
              strlen(getUnregname(3)) > 0) ||
             (strstr(charname, getUnregname(4)) && strlen(getUnregname(4)) > 0)
#endif
             // WON END
  ) {

    unsigned int ip = CONNECT_get_userip(fd);
    int a, b, c, d, ck;

    a = (ip % 0x100);
    ip = ip / 0x100;
    b = (ip % 0x100);
    ip = ip / 0x100;
    c = (ip % 0x100);
    ip = ip / 0x100;
    d = (ip % 0x100);

    ck = (((a == 10) && (b == 0) && (c == 0)) ||
          ((a == 211) && (b == 76) && (c == 176) && (d == 21)) || // 台北wayi
          ((a == 210) && (b == 64) && (c == 97) && ((d >= 21) && (d <= 25))) ||
          ((a == 61) && (b == 222) && (c == 142) && (d == 66)) ||
          ((a == 172) && (b == 16) && (c == 172) && (d == 29)));

    print(" name_WAEI_IP:%d.%d.%d.%d ck:%d ", a, b, c, d, ck);

    if (!ck) {
      lssproto_CreateNewChar_send(fd, FAILED, "생성 할 수없는 잘못된 이름입니다.\n");
      return;
    }
  }

  // Nuke start 0801,0916: Avoid strange name
  {
    int i, ach;
    for (i = 0, ach = 0; i < strlen(charname); i++) {
      if ((unsigned char)charname[i] == 0xff) {
        ach = 1;
        break;
      } // Force no 0xff
      if (((unsigned char)charname[i] >= 0x7f) &&
          ((unsigned char)charname[i] <= 0xa0)) {
        ach = 1;
        break;
      } // Force no 0x7f~0xa0
      if ((unsigned char)charname[i] <= 0x20) {
        ach = 1;
        break;
      } // Force greater than 0x20
      if ((unsigned char)charname[i] == 0x2c) {
        ach = 1;
        break;
      } // Force greater than 0x20
      if ((unsigned char)charname[i] == 0x3b) {
        ach = 1;
        break;
      } // Force greater than 0x20
      if ((unsigned char)charname[i] == 0x3d) {
        ach = 1;
        break;
      } // Force greater than 0x20
      if ((unsigned char)charname[i] == 0x27) {
        ach = 1;
        break;
      } // Force greater than 0x20
      if (ach) {
        if ((((unsigned char)charname[i] >= 0x40) &&
             ((unsigned char)charname[i] <= 0x7e)) ||
            (((unsigned char)charname[i] >= 0xa1) &&
             ((unsigned char)charname[i] <= 0xfe)))
          ach = 0;
      } else {
        if (((unsigned char)charname[i] >= 0xa1) &&
            ((unsigned char)charname[i] <= 0xfe))
          ach = 1;
      }
    }
    if (ach) {
      lssproto_CreateNewChar_send(fd, FAILED, "생성 할 수없는 잘못된 이름입니다.\n");
      return;
    }
  }
  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  CHAR_createNewChar(fd, dataplacenum, charname, imgno, faceimgno, vital, str,
                     tgh, dex, earth, water, fire, wind, hometown, cdkey);
}

void lssproto_CharLogin_recv(int fd, char *charname) {
  char cdkey[CDKEYLEN], passwd[PASSWDLEN];

  if (CONNECT_isCLI(fd) == FALSE)
    return;

#ifdef _STOPLOGININFORMATION
#else
	print("\n尝试登陆: 人物名称=%s\n", charname);
#endif

  if (charname[0] == '\0') {
    lssproto_CharLogin_send(fd, FAILED, "Can't access char have no name\n");
    return;
  }
  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    lssproto_CharLogin_send(fd, FAILED, "Already Logged in\n");
    return;
  }
  CONNECT_setCharname(fd, charname);
  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  CONNECT_getPasswd(fd, passwd, sizeof(passwd));

#ifdef _OFFLINE_BATTLE_RETEUN
  int i;
  for (i = 0; i < CHAR_getPlayerMaxNum(); i++) {
    if (CHAR_CHECKINDEX(i) == FALSE)
      continue;
    if (strcmp(CHAR_getChar(i, CHAR_CDKEY), cdkey) != 0)
      continue;
    else {
      if (strcmp(CHAR_getChar(i, CHAR_NAME), charname) != 0)
        break;
    }
    char mac[128];
    char mac2[128];
    CONNECT_getMAC(fd, mac, sizeof(mac));
    CONNECT_getMAC2(fd, mac2, sizeof(mac2));
    if (strcmp(mac, CHAR_getWorkChar(i, CHAR_WORK_MAC)) != 0 ||
        strcmp(mac2, CHAR_getWorkChar(i, CHAR_WORK_MAC2)) != 0)
      break;
    int oldfd = CHAR_getWorkInt(i, CHAR_WORKFD);
    if (oldfd > -1) {
      CONNECT_setCharaindex(oldfd, -1);
      CONNECT_setCharname(oldfd, "");
    }
    CHAR_setWorkInt(i, CHAR_WORKFD, fd);
    CONNECT_setCharname(fd, charname);
    CONNECT_setCharaindex(fd, i);
    CHAR_setWorkChar(i, CHAR_WORK_MAC, mac);
    CHAR_setWorkChar(i, CHAR_WORK_MAC2, mac2);
    CONNECT_setState(fd, LOGIN);
    lssproto_CharLogin_send(fd, SUCCESSFUL, "");
    int offlineflg = CHAR_getWorkInt(i, CHAR_WORK_OFFLINE);
    CHAR_setWorkInt(i, CHAR_WORK_OFFLINE, 0);
    if (CHAR_getWorkInt(i, CHAR_WORKBATTLEINDEX) < 0) {
      CHAR_setWorkInt(i, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);
    }
    CHAR_complianceParameter(i);
    {
      int j;
      char *statuscat[] = {"C", "P",  "I",  "S",  "D",  "E",
                           "F", "J0", "J1", "J2", "J3", "J4"};
      for (j = 0; j < arraysizeof(statuscat); j++)
        CHAR_sendStatusString(i, statuscat[j]);
    }
    {
      int j;
      char category[3];
      for (j = 0; j < CHAR_MAXPETHAVE; j++) {
        snprintf(category, sizeof(category), "K%d", j);
        CHAR_sendStatusString(i, category);
        snprintf(category, sizeof(category), "W%d", j);
        CHAR_sendStatusString(i, category);
      }
    }
    //{
    int petarray = CHAR_getInt(i, CHAR_DEFAULTPET);
    if (CHAR_CHECKPETINDEX(petarray)) {
      if (!CHAR_CHECKINDEX(CHAR_getCharPet(i, petarray))) {
        petarray = -1;
      }
    } else {
      petarray = -1;
    }
    lssproto_KS_send(fd, petarray, TRUE);
    //}
    {
      int flg = 0;

      CHAR_setFlg(i, CHAR_ISSHOWBATTLEMSG, 0);
      CHAR_setFlg(i, CHAR_ISDUEL, 0);
      CHAR_setFlg(i, CHAR_ISTRADECARD, 0);
      if (CHAR_getFlg(i, CHAR_ISPARTY))
        flg |= CHAR_FS_PARTY;
      if (CHAR_getFlg(i, CHAR_ISDUEL))
        flg |= CHAR_FS_DUEL;
      if (CHAR_getFlg(i, CHAR_ISPARTYCHAT))
        flg |= CHAR_FS_PARTYCHAT;
      if (CHAR_getFlg(i, CHAR_ISTRADECARD))
        flg |= CHAR_FS_TRADECARD;
#ifdef _CHATROOMPROTOCOL

#ifdef _THE_WORLD_SEND
      //ߣ�������ͣ����?Գ���
      // CHAR_setFlg(i,CHAR_ISWORLD,1);
      // if( CHAR_getFlg( i, CHAR_ISWORLD)) flg |= CHAR_FS_WORLD;
      CHAR_setFlg(i, CHAR_ISOCC, 1);
      if (CHAR_getFlg(i, CHAR_ISOCC))
        flg |= CHAR_FS_WORLD;

#endif
#endif
#ifdef _CHANNEL_MODIFY
      CHAR_setFlg(i, CHAR_ISTELL, 0);
      CHAR_setFlg(i, CHAR_ISFM, 0);
#ifdef _VERSION_NEW
      CHAR_setFlg(i, CHAR_ISOCC, 1);
      if (CHAR_getFlg(i, CHAR_ISOCC))
        flg |= CHAR_FS_WORLD;
#endif
      CHAR_setFlg(i, CHAR_ISCHAT, 0);
      CHAR_setFlg(i, CHAR_ISSAVE, 0);
#endif
      // CoolFish: 2001/4/18
      CHAR_setFlg(i, CHAR_ISTRADE, 0);
      lssproto_FS_send(fd, flg);
    }
    MAP_sendArroundCharNeedFD(fd, i);
    CHAR_setInt(i, CHAR_LOGINCOUNT, CHAR_getInt(i, CHAR_LOGINCOUNT) + 1);
    int objindex = CHAR_getWorkInt(i, CHAR_WORKOBJINDEX);
    CHAR_sendCToArroundCharacter(objindex);
    CHAR_sendArroundCharaData(i);
    if (CHAR_getWorkInt(i, CHAR_WORKACTION) == -1)
      CHAR_sendWatchEvent(objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
    else
      CHAR_sendWatchEvent(objindex, CHAR_getWorkInt(i, CHAR_WORKACTION), NULL,
                          0, TRUE);

    char c[3];
    int toindex = -1;
    if (CHAR_getWorkInt(i, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
      if (CHAR_getWorkInt(i, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
        CHAR_sendStatusString(i, "N0");
        toindex = i;
      } else {
        toindex = CHAR_getWorkInt(i, CHAR_WORKPARTYINDEX1);
      }
      int jj;
      for (jj = 0; jj < CHAR_PARTYMAX; jj++) {
        int index = CHAR_getWorkInt(toindex, jj + CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(index)) {
          snprintf(c, sizeof(c), "N%d", jj);
          CHAR_sendStatusString(i, c);
        }
      }
    }
    lssproto_SKUP_send(fd, CHAR_getInt(i, CHAR_SKILLUPPOINT));
#ifdef _RECORD_IP
    CHAR_setWorkInt(i, CHAR_WORK_RECORD_IP, CONNECT_get_userip(fd));
#endif
    CHAR_setWorkInt(i, CHAR_OFFLINEBATTLENUM, 0);
    if (CHAR_getWorkInt(i, CHAR_WORKBATTLEINDEX) > -1) {
      lssproto_EN_send(
          fd, BattleArray[CHAR_getWorkInt(i, CHAR_WORKBATTLEINDEX)].type,
          BattleArray[CHAR_getWorkInt(i, CHAR_WORKBATTLEINDEX)].field_no);
      CHAR_sendBattleEffect(i, ON);
      // lssproto_AC_recv(fd, CHAR_getInt(i,CHAR_X), CHAR_getInt(i,CHAR_Y), 3);
      BATTLE_CharSendOne(CHAR_getWorkInt(i, CHAR_WORKBATTLEINDEX), i);
      if (petarray == -1)
        lssproto_B_send(fd, "BA|18000|0|");
    }
    if (offlineflg == 3) {
      setStayEncount(fd);
    }
    if (oldfd > -1)
      CONNECT_endOne_debug(oldfd);
    // print("\n???���ڪ?������\n");
    return;
  }
#endif

  saacproto_ACCharLoad_send(acfd, cdkey, passwd, charname, 1, "",
                            CONNECT_getFdid(fd));
  CONNECT_setState(fd, WHILELOGIN);
}

#ifdef _ITEM_CHECKDROPATLOGOUT
BOOL CheckDropatLogout(int charaindex) {
  int i;
  for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
    int itemindex;
    itemindex = CHAR_getItemIndex(charaindex, i);
    if (ITEM_CHECKINDEX(itemindex) == FALSE)
      continue;
    if (ITEM_getInt(itemindex, ITEM_DROPATLOGOUT) == TRUE) {
      return TRUE;
    }
  }
  return FALSE;
}
#endif

void lssproto_CharLogout_recv(int fd, int flg) {
  char cdkey[CDKEYLEN], charname[CHARNAMELEN];
  int charaindex = CONNECT_getCharaindex(fd);

  if (CONNECT_isCLI(fd) == FALSE)
    return;
  if (CONNECT_isLOGIN(fd) == FALSE) {
    lssproto_CharLogout_send(fd, FAILED, "Not Logged in\n");
    return;
  }
#ifdef _OFFLINE_SYSTEM
  if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) > 0) {
    CHAR_setWorkInt(charaindex, CHAR_WORKFD, -1);
  }
#ifdef _OFFLINE_BATTLE_RETEUN
  else if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX) > -1 &&
           (BattleArray[CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX)]
                    .type == BATTLE_TYPE_P_vs_P ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) >= 40000 &&
             CHAR_getInt(charaindex, CHAR_FLOOR) <= 40100)) &&
           CHAR_getFlg(charaindex, CHAR_ISDIE) != 1) {
    CHAR_setWorkInt(charaindex, CHAR_WORK_OFFLINE, 1);
    CHAR_setWorkInt(charaindex, CHAR_WORKFD, -1);
  }
#endif
  else
#endif
  {
    int fl, x, y;
    // CoolFish: 2001/10/18
    if (!CHAR_CHECKINDEX(charaindex))
      return;
    if (CHAR_getInt(charaindex, CHAR_LASTTALKELDER) >= 0) {
      CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER), &fl,
                            &x, &y);

#ifdef _CHAR_NEWLOGOUT
      if (flg == 1) { //��??��
        if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
            BATTLE_CHARMODE_NONE) {
					CHAR_talkToCli(charaindex, -1, "전투의 기록 지점으로 돌아갈 수 없습니다!", CHAR_COLORYELLOW);
          return;
        }
#ifdef _ITEM_CHECKWARES
        if (CHAR_CheckInItemForWares(charaindex, 0) == FALSE) {
					CHAR_talkToCli(charaindex, -1, "운반 상품을 사용할 수 없습니다.", CHAR_COLORYELLOW);
          return;
        }
#endif
        if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
            CHAR_PARTY_CLIENT) {
					CHAR_talkToCli(charaindex, -1, "팀은 레코드 포인트로 돌아갈 수 없습니다!", CHAR_COLORYELLOW);
          return;
        }
#ifdef _ITEM_CHECKDROPATLOGOUT
        if (CheckDropatLogout(charaindex)) {
					CHAR_talkToCli(charaindex, -1, "휴대 항목은 기록 포인트로 돌아갈 수 없습니다!", CHAR_COLORYELLOW);
          return;
        }
#endif
#ifdef _AUTO_PK
        if (CHAR_getInt(charaindex, CHAR_FLOOR) == 20000 &&
            CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_NONE &&
            CHAR_getWorkInt(charaindex, CHAR_WORK_AUTOPK) != -1) {
          char buf[64];
          int i, num = 0, winindex = -1;
          int playernum = CHAR_getPlayerMaxNum();
					sprintf(buf, "겁쟁이 %s 현재 게임을 종료하십시오!", CHAR_getChar(charaindex, CHAR_NAME));
          AutoPk_PKSystemTalk(buf, buf);
          if (AutoPk_PKTimeGet() <= 0) {
            for (i = 0; i < playernum; i++) {
              if (CHAR_CHECKINDEX(i) == FALSE ||
                  CHAR_getWorkInt(i, CHAR_WORK_AUTOPK) == -1)
                continue;
              if (CHAR_getInt(i, CHAR_FLOOR) == 20000) {
                if (CHAR_getWorkInt(i, CHAR_WORK_AUTOPK) != -1) {
                  if (i != charaindex)
                    winindex = i;
                  num++;
                }
              }
            }
            if (num == 2) {
              int fl = 0, x = 0, y = 0;
              CHAR_getElderPosition(CHAR_getInt(winindex, CHAR_LASTTALKELDER),
                                    &fl, &x, &y);
              CHAR_warpToSpecificPoint(winindex, fl, x, y);
              AutoPk_ChampionShipSet(
                  winindex, CHAR_getWorkInt(winindex, CHAR_WORK_AUTOPK), 1);
              AutoPk_ChampionShipSet(
                  charaindex, CHAR_getWorkInt(charaindex, CHAR_WORK_AUTOPK), 2);
              AutoPk_GetChampionShip();
#ifdef _FORMULATE_AUTO_PK
              //					  	saacproto_FormulateAutoPk_send(acfd,
              //CHAR_getChar(winindex, CHAR_CDKEY),
              //CHAR_getWorkInt(winindex,CHAR_WORK_AUTOPK));
              CHAR_setInt(winindex, CHAR_AMPOINT,
                          CHAR_getInt(winindex, CHAR_AMPOINT) +
                              CHAR_getWorkInt(winindex, CHAR_WORK_AUTOPK));
              CHAR_setWorkInt(winindex, CHAR_WORK_AUTOPK, 0);
#endif
            }
          }
        }
#endif
        if (CHAR_getInt(charaindex, CHAR_FLOOR) != 117 &&
            CHAR_getInt(charaindex, CHAR_FLOOR) != 887
#ifdef _ADD_DUNGEON //追加地牢
            && CHAR_getInt(charaindex, CHAR_FLOOR) != 8513
#endif
        ) {

          CHAR_warpToSpecificPoint(charaindex, fl, x, y);

          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
              CHAR_PARTY_LEADER) {
            int i;
            for (i = 1; i < CHAR_PARTYMAX; i++) {
              int pindex =
                  CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
              if (CHAR_CHECKINDEX(pindex)) {
                CHAR_warpToSpecificPoint(pindex, fl, x, y);
              }
            }
          }
        }
      } else {

        CONNECT_endOne_debug(fd);
      }
#else
      if (CHAR_getInt(charaindex, CHAR_FLOOR) == 117) {
        CHAR_setInt(charaindex, CHAR_X, 225);
        CHAR_setInt(charaindex, CHAR_Y, 13);
      } else {
        CHAR_setInt(charaindex, CHAR_FLOOR, fl);
        CHAR_setInt(charaindex, CHAR_X, x);
        CHAR_setInt(charaindex, CHAR_Y, y);
      }
#endif
    }
    // Robin add
    // CHAR_setInt( charaindex, CHAR_LASTLEAVETIME, (int)time(NULL));

    // CHAR_logout(charaindex,TRUE);
    // CONNECT_setState( fd, WHILELOGOUTSAVE );
  }

  // CONNECT_setCharaindex( fd, -1 );
  // CONNECT_getCdkey( fd, cdkey, sizeof(cdkey ));
  // CONNECT_getCharname( fd, charname, sizeof(charname));
  // printf( "Logout cdkey:%s charname=%s\n" , cdkey, charname );
  // lssproto_CharLogout_send( fd, SUCCESSFUL , "success" );
}

void lssproto_CharDelete_recv(int fd, char *charname, char *safepasswd) {
  char cdkey[CDKEYLEN], passwd[PASSWDLEN];
  int fdid;
  if (CONNECT_isCLI(fd) == FALSE)
    return;
  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    lssproto_CharDelete_send(fd, FAILED, "Already Logged in\n");
    return;
  }

  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  CONNECT_getPasswd(fd, passwd, sizeof(passwd));
  fdid = CONNECT_getFdid(fd);
  FreeCharDelete(fd, fdid, cdkey);
  saacproto_ACCharDelete_send(acfd, cdkey, passwd, charname, safepasswd, fdid);

  {
    char buff[512];
    char escapebuf[1024];
    snprintf(buff, sizeof(buff), "%s_%s", cdkey, charname);
    makeEscapeString(buff, escapebuf, sizeof(escapebuf));
    saacproto_DBDeleteEntryInt_send(acfd, DB_DUELPOINT, escapebuf, fdid, 0);
    saacproto_DBDeleteEntryString_send(acfd, DB_ADDRESSBOOK, escapebuf, fdid,
                                       0);
  }
  saacproto_Broadcast_send(acfd, cdkey, charname, "chardelete", 0);
  CONNECT_setState(fd, WHILECHARDELETE);
}

void lssproto_CharList_recv(int fd) {
  char cdkey[CDKEYLEN], passwd[PASSWDLEN];
  int fdid = -1;
  int charlistflg = 0;

  if (CONNECT_isCLI(fd) == FALSE)
    return;

  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
		lssproto_CharList_send(fd, FAILED, "Already Logged in\n");
    return;
  }

  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  CONNECT_getPasswd(fd, passwd, sizeof(passwd));
  fdid = CONNECT_getFdid(fd);
  {
    int i;
    int playernum = CHAR_getPlayerMaxNum();
    for (i = 0; i < playernum; i++) {
      if (!CHAR_CHECKINDEX(i))
        continue;
      if (!strcmp(CHAR_getChar(i, CHAR_CDKEY), cdkey)) {
#ifdef _OFFLINE_SYSTEM
        if (CHAR_getWorkInt(i, CHAR_WORK_OFFLINE) != 0) {
#ifdef _OFFLINE_BATTLE_RETEUN
          if (CHAR_getWorkInt(i, CHAR_WORKBATTLEINDEX) < 0 ||
              (BattleArray[CHAR_getWorkInt(i, CHAR_WORKBATTLEINDEX)].type !=
                   BATTLE_TYPE_P_vs_P &&
               (CHAR_getInt(i, CHAR_FLOOR) < 40000 ||
                CHAR_getInt(i, CHAR_FLOOR) > 40100))) {
#endif
            if (!CHAR_logout(i, TRUE)) {
              print("err %s:%d\n", __FILE__, __LINE__);
            }
            lssproto_CharList_send(fd, FAILED, "반복 착륙으로 인해 선을 끄십시오!");
            CONNECT_setCloseRequest(getfdFromCharaIndex(i), 1);
            luaplayernum = luaplayernum - 1;
            break;
#ifdef _OFFLINE_BATTLE_RETEUN
          } else {
            charlistflg = 1;
            break;
          }
#endif
        } else
#endif
        {
          CHAR_talkToCli(i, -1, "����?��?���?!", CHAR_COLORYELLOW);
          int oldfd = getfdFromCharaIndex(i);
          CONNECT_endOne_debug(oldfd);
          // CHAR_logout( i, TRUE );
          // CONNECT_setCloseRequest( oldfd , 1 );
          charlistflg = 1;
          // lssproto_CharList_send( fd, FAILED, "?��???���?��?��?������?��?��"
          // );
          break;
        }
        //				lssproto_CharList_send( fd, FAILED, "-1"
        //); 				CONNECT_setState( fd, NOTLOGIN ); 				return;
      }
    }
  }
#ifdef _NEWCLISETMAC
  char mac1[64];
  char mac2[64];
  char mac3[64];
  CONNECT_getMAC(fd, mac1, sizeof(mac1));
  CONNECT_getMAC(fd, mac2, sizeof(mac2));
  CONNECT_getMAC(fd, mac3, sizeof(mac3));
#endif

  unsigned long tmpip;
  int a, b, c, d;
  char ip[32];
  tmpip = CONNECT_get_userip(fd);
  a = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  b = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  c = (tmpip % 0x100);
  tmpip = tmpip / 0x100;
  d = (tmpip % 0x100);
  sprintf(ip, "%d.%d.%d.%d", a, b, c, d);

  saacproto_ACCharList_send(acfd, cdkey, passwd, ip, mac1, mac2, mac3, fdid,
                            charlistflg);

  CONNECT_setState(fd, WHILEDOWNLOADCHARLIST);
}

void lssproto_Echo_recv(int fd, char *arg0) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  lssproto_Echo_send(fd, arg0);
}

#define CHECKFD                                                                \
  if (CONNECT_isCLI(fd) == FALSE)                                              \
    return;                                                                    \
  if (CONNECT_isLOGIN(fd) == FALSE)                                            \
    return;
#define CHECKFDANDTIME                                                         \
  if (CONNECT_isCLI(fd) == FALSE)                                              \
    return;                                                                    \
  if (CONNECT_isLOGIN(fd) == FALSE)                                            \
    return;

void lssproto_W_recv(int fd, int x, int y, char *direction) {
  int fd_charaindex, ix, iy;
  fd_charaindex = CONNECT_getCharaindex(fd);
  ix = CHAR_getInt(fd_charaindex, CHAR_X);
  iy = CHAR_getInt(fd_charaindex, CHAR_Y);
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE) {
    return;
  }
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) {
    return;
  }
  if (checkNu(fd) < 0) {
    print(" NU-Err ");
    CHAR_talkToCli(fd_charaindex, -1, "신호 오류.", CHAR_COLORYELLOW);
    CONNECT_setCloseRequest(fd, 1);
    return;
  }
  if ((x == 0) && (y == 0)) {
    // CHAR_talkToCli(fd_charaindex, -1, "���???�??��", CHAR_COLORYELLOW);
    return;
  }
  {
    int i_diff_x, i_diff_y;
    i_diff_x = abs(ix - x);
    i_diff_y = abs(iy - y);
    if ((i_diff_x > 1) || (i_diff_y > 1)) {
      // CHAR_talkToCli(fd_charaindex, -1, "�������???�??��", CHAR_COLORYELLOW);
      // return;
      x = ix;
      y = iy;
    }
  }
  if (!(MAP_walkAble(fd_charaindex, CHAR_getInt(fd_charaindex, CHAR_FLOOR), x,
                     y))) {
    x = ix;
    y = iy;
  } else {
  }
  CHAR_walk_init(fd, x, y, direction, TRUE);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_W2_recv(int fd, int x, int y, char *direction) {
  int fd_charaindex, ix, iy, i_fl;
  fd_charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(fd_charaindex))
    return;
  ix = CHAR_getInt(fd_charaindex, CHAR_X);
  iy = CHAR_getInt(fd_charaindex, CHAR_Y);
  i_fl = CHAR_getInt(fd_charaindex, CHAR_FLOOR);
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

#ifdef _YD_ENEMY
  if (strstr(direction, "gcgc") != NULL) {
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKPARTYMODE) !=
        CHAR_PARTY_CLIENT) {
      if (CONNECT_getBDTime(fd) < time(NULL)) {
        lssproto_EN_recv(fd, CHAR_getInt(fd_charaindex, CHAR_X),
                         CHAR_getInt(fd_charaindex, CHAR_Y));
        CONNECT_setBDTime(fd, (int)time(NULL));
      }
      lssproto_EN_recv(fd, x, y);
    }
    return;
  }
#endif
  {
    int i_diff_x, i_diff_y;
    i_diff_x = abs(ix - x);
    i_diff_y = abs(iy - y);
    if ((i_diff_x > 1) || (i_diff_y > 1)) {
      x = ix;
      y = iy;
    }
  }
  if (!(MAP_walkAble(fd_charaindex, CHAR_getInt(fd_charaindex, CHAR_FLOOR), x,
                     y))) {
    x = ix;
    y = iy;
  }
  CHAR_walk_init(fd, x, y, direction, FALSE);
}

void lssproto_SKD_recv(int fd, int dir, int index) { CHECKFDANDTIME; }

void lssproto_ID_recv(int fd, int x, int y, int haveitemindex, int toindex) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  int to_charaindex;
  int fd_charaindex;

  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  // ttom avoid the warp at will 12/5
  {
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    x = ix;
    y = iy;
  }
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  to_charaindex = Callfromcli_Util_getTargetCharaindex(fd, toindex);
  CHAR_ItemUse(fd_charaindex, to_charaindex, haveitemindex);
}

/*------------------------------------------------------------
 * 惫寞毛蓟少
 ------------------------------------------------------------*/
void lssproto_ST_recv(int fd, int titleindex) {
  CHECKFDANDTIME;
  CHAR_selectTitle(CONNECT_getCharaindex(fd), titleindex);
}
/*------------------------------------------------------------
 * 惫寞毛绰轮允月
 ------------------------------------------------------------*/
void lssproto_DT_recv(int fd, int titleindex) {
  CHECKFDANDTIME;
  CHAR_deleteTitle(CONNECT_getCharaindex(fd), titleindex);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_FT_recv(int fd, char *data) {
  CHECKFDANDTIME;

  // Robin 04/23 debug
  if (strlen(data) > 12)
    return;

  if (checkStringErr(data))
    return;

  CHAR_inputOwnTitle(CONNECT_getCharaindex(fd), data);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PI_recv(int fd, int x, int y, int dir) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  { // ttom avoid warp at will
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      // print("\n<PI>--Error!!!!");
      // print("\n<PI>origion x=%d,y=%d",ix,iy);
      // print("\n<PI>modify  X=%d,Y=%d",x,y);
      x = ix;
      y = iy;
    }
  } // ttom end

  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  CHAR_PickUpItem(fd_charaindex, dir);
}

void lssproto_DI_recv(int fd, int x, int y, int itemindex) {
  int charaindex;
  CHECKFDANDTIME;
  charaindex = CONNECT_getCharaindex(fd);

  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif
#ifdef _NB_CDKEYLOCK
  char *cdkey = CHAR_getChar(charaindex, CHAR_CDKEY);
  if (sasql_getLock(cdkey) > 0) {
    CHAR_talkToCli(charaindex, -1, "?��??���ު�����£�����?��??��", 6);
    return;
  }
#endif

#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(charaindex, CHAR_LOCKED) == 1 && getItemPetLocked() == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "_ITEM_PET_LOCKED\nSOME MESSAGE"
                     "??\n\n??PS?/safe?");

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif

  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;

  CHAR_setMyPosition(charaindex, CHAR_getInt(charaindex, CHAR_X),
                     CHAR_getInt(charaindex, CHAR_Y), TRUE);

  CHAR_DropItem(charaindex, itemindex);
}

void lssproto_DP_recv(int fd, int x, int y, int petindex) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
#ifdef _NB_CDKEYLOCK
  char *cdkey = CHAR_getChar(fd_charaindex, CHAR_CDKEY);
  if (sasql_getLock(cdkey) > 0) {
    CHAR_talkToCli(fd_charaindex, -1, "lssproto_DP_recv message 1", 6);
    return;
  }
#endif
#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(fd_charaindex, CHAR_LOCKED) == 1 && getItemPetLocked() == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "lssproto_DP_recv message 2 \n ??"
                     "??\n\nPS/safe?");

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
  {
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    x = ix;
    y = iy;
  }
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  PET_dropPet(fd_charaindex, petindex);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_DG_recv(int fd, int x, int y, int amount) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  // ttom avoid the warp at will 12/15
  {
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    x = ix;
    y = iy;
  }
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);

  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

  CHAR_DropMoney(fd_charaindex, amount);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_MI_recv(int fd, int fromid, int toid) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif

  if (fromid != toid) {
    int fromitemindex = CHAR_getItemIndex(fd_charaindex, fromid);
    int toitemindex = CHAR_getItemIndex(fd_charaindex, toid);
    if (ITEM_CHECKINDEX(fromitemindex) && ITEM_CHECKINDEX(toitemindex)) {
      CHAR_setWorkInt(fd_charaindex, CHAR_WORKITEMINDEX, fromitemindex);
      CHAR_setWorkInt(fd_charaindex, CHAR_WORKTOITEMINDEX, toitemindex);

#ifdef _ITEM_COLOER
      if (strcmp(ITEM_getChar(fromid, ITEM_USEFUNC), "ITEM_COLOER") == 0) {
        char *itemarg = ITEM_getChar(fromitemindex, ITEM_ARGUMENT);
        ITEM_setInt(toitemindex, ITEM_COLOER, atoi(itemarg));
        ITEM_setInt(fromitemindex, ITEM_COLOER, atoi(itemarg));
        CHAR_sendItemDataOne(fd_charaindex, toid);
        CHAR_DelItem(fd_charaindex, fromid);
        return;
      }
#endif
#ifdef _ITEM_OVER_LAP
      if (ItemOverlapFunction(fd_charaindex, fromitemindex, toitemindex, fromid,
                              toid)) {
        return;
      }
#endif
    }
  }

  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  CHAR_moveEquipItem(fd_charaindex, fromid, toid);
}

#ifdef _PET_ITEM
void lssproto_PETITEM_recv(int fd, int x, int y, int petindex, int fromindex,
                           int toindex) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(fd_charaindex, CHAR_LOCKED) == 1 && getItemPetLocked() == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "lssproto_PETITEM_recv \n"
                     "??\n\nPS/safe");

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

	/* 爵    反轮仁   仿弘匹仇木卞娄匀井井月第  岭丐曰  */
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  CHAR_movePetItem(fd_charaindex, petindex, fromindex, toindex);
}
#endif

/*------------------------------------------------------------
 * 旦平伙失永皿
 ------------------------------------------------------------*/
void lssproto_SKUP_recv(int fd, int skillid) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);

  /* ��?�����Ϊ� ���髰�Ǫ������ê������?�ê���?*/
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  CHAR_SkillUp(fd_charaindex, skillid);
}

/*------------------------------------------------------------
 * 戊生弁扑亦件锹澎卞丢永本□斥毛霜耨
 ------------------------------------------------------------*/
void lssproto_MSG_recv(int fd, int index, char *message, int color) {
  int fd_charaindex;
  CHECKFD;
  fd_charaindex = CONNECT_getCharaindex(fd);
  ADDRESSBOOK_sendMessage(fd_charaindex, index, message, color);
}

/*------------------------------------------------------------
 * ���ɫ쫹�֫ë������Ϫ�������?�ɪ�����?������
 ------------------------------------------------------------*/
void lssproto_AB_recv(int fd) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  ADDRESSBOOK_sendAddressbookTable(fd_charaindex);
}

/*------------------------------------------------------------
 * ���ɫ쫹�֫ë���δ?��?�Ϊ���
 ------------------------------------------------------------*/
void lssproto_DAB_recv(int fd, int index) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  ADDRESSBOOK_deleteEntry(fd_charaindex, index);
}

void lssproto_AAB_recv(int fd, int x, int y) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  {
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      x = ix;
      y = iy;
    }
  }
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  ADDRESSBOOK_addEntry(fd_charaindex);
}

void lssproto_L_recv(int fd, int dir) {
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  CHAR_Look(fd_charaindex, dir);
}

/*------------------------------------------------------------
 * 民乓永玄迕丢永本□斥及霜耨
 ------------------------------------------------------------*/
void lssproto_TK_recv(int fd, int x, int y, char *message, int color,
                      int area) {
  int fd_charaindex, ix, iy; // ttom+2
  int fmindex, channel;

  CHECKFD;
  fd_charaindex = CONNECT_getCharaindex(fd);
  fmindex = CHAR_getInt(fd_charaindex, CHAR_FMINDEX);
  channel = CHAR_getWorkInt(fd_charaindex, CHAR_WORKFMCHANNEL);

  { // Robin 0629 silent
    int silentSec, talkCount;
    silentSec = CHAR_getInt(fd_charaindex, CHAR_SILENT);
    if (silentSec > 0) {
      int loginTime;
      char buf[256];
      int leftSec;
      loginTime = CHAR_getWorkInt(fd_charaindex, CHAR_WORKLOGINTIME);
			// 防止时间修正回朔後 异常禁言  Robin 20040817
      if ((int)NowTime.tv_sec < loginTime) {
        CHAR_setInt(fd_charaindex, CHAR_SILENT, 0);
        return;
      }
      if (((int)NowTime.tv_sec - loginTime) > silentSec) {
        CHAR_setInt(fd_charaindex, CHAR_SILENT, 0);
        return;
      }
      silentSec += 10; //多禁10秒

      leftSec = silentSec - ((int)NowTime.tv_sec - loginTime);
			sprintf(buf, "금지에서 !!도 % d 초, 10 초 이상이 있습니다.", leftSec);
      CHAR_talkToCli(fd_charaindex, -1, buf, color);
      CHAR_setInt(fd_charaindex, CHAR_SILENT, silentSec);
      return;
    }
#ifdef _NB_SILENT_SWITCH
    if (getsilentswitch())
#endif
    {
      talkCount = CHAR_getWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT);
      talkCount++;
      CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT, talkCount);
      if (talkCount > 8) {
        int lastTalkTime = CHAR_getWorkInt(fd_charaindex, CHAR_WORKTALKTIME);
        if ((int)NowTime.tv_sec - lastTalkTime < 10) {
          CHAR_setInt(fd_charaindex, CHAR_SILENT, 60);
          CHAR_setWorkInt(fd_charaindex, CHAR_WORKLOGINTIME,
                          (int)NowTime.tv_sec);
				  CHAR_talkToCli(fd_charaindex, -1, "당신은 너무 많아서 입에 잠시 시간을내어주세요!", color);
          CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT, 0);
          return;
        } else {
          CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKTIME,
                          (int)NowTime.tv_sec);
          CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT, 0);
        }
      }
    }
  }
  ix = CHAR_getInt(fd_charaindex, CHAR_X);
  iy = CHAR_getInt(fd_charaindex, CHAR_Y);
  x = ix;
  y = iy;
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
#ifdef _TALK_AREA_CF
  if (area < getTalkMin())
    area = getTalkMin();
  if (area > getTalkMax())
    area = getTalkMax();
#endif
#ifdef _ALLBLUES_LUA_1_6
  if (CharTalkFunction2(fd_charaindex, message, color) == TRUE) {
    CHAR_Talk(fd, fd_charaindex, "P|hi", color, area);
  } else
#endif
  {
    CHAR_Talk(fd, fd_charaindex, message, color, area);
  }

  //}
}

void lssproto_M_recv(int fd, int fl, int x1, int y1, int x2, int y2) {
  char *mapdata;
  RECT seek = {x1, y1, x2 - x1, y2 - y1}, ret;
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
#ifdef _MASK_ENCOUNTER
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
    if (getStayEncount(fd))
      return;
  } else {
    int oyaindex = CHAR_getWorkInt(fd_charaindex, CHAR_WORKPARTYINDEX1);
    if (getStayEncount(getfdFromCharaIndex(oyaindex)))
      return;
  }
#endif
  mapdata = MAP_getdataFromRECT(fl, &seek, &ret);
  if (mapdata != NULL) {
    lssproto_M_send(fd, fl, ret.x, ret.y, ret.x + ret.width, ret.y + ret.height,
                    mapdata);
  }
}

/*------------------------------------------------------------
 * ������?�� ��?��
 ------------------------------------------------------------*/
void lssproto_C_recv(int fd, int index) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  int fd_charaindex;

  fd_charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(fd_charaindex)) {
    return;
  }
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif

  CHAR_sendCSpecifiedObjindex(fd, index);
}

void lssproto_S_recv(int fd, char *category) {
  char *string;
  int fd_charaindex;

  fd_charaindex = CONNECT_getCharaindex(fd);
  string = CHAR_makeStatusString(fd_charaindex, category);
  if (string != NULL)
    lssproto_S_send(fd, string);
}

void lssproto_EV_recv(int fd, int event, int seqno, int x, int y, int dir) {
  int rc;
  int fx, fy;
  int fd_charaindex;

  CHECKFD;
  fd_charaindex = CONNECT_getCharaindex(fd);
  {
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      goto CK1;
    }
    goto OK;
  }
CK1 : {
  OBJECT object;
  int ix, iy, ifloor, i, j;
  int warp_point_x[9];
  int warp_point_y[9];
  int warp_point = 0;
  ix = CHAR_getInt(fd_charaindex, CHAR_X);
  iy = CHAR_getInt(fd_charaindex, CHAR_Y);
  ifloor = CHAR_getInt(fd_charaindex, CHAR_FLOOR);
  for (i = iy - 1; i <= iy + 1; i++) {
    for (j = ix - 1; j <= ix + 1; j++) {
      for (object = MAP_getTopObj(ifloor, j, i); object;
           object = NEXT_OBJECT(object)) {
        int o = GET_OBJINDEX(object);
        if (OBJECT_getType(o) == OBJTYPE_CHARA) {
          int etype;
          int charaindex = OBJECT_getIndex(o);
          if (!CHAR_CHECKINDEX(charaindex))
            continue;
          etype = CHAR_getWorkInt(charaindex, CHAR_WORKEVENTTYPE);
          if (etype != CHAR_EVENT_NONE) {
            if (etype == CHAR_EVENT_WARP) {
              warp_point_x[warp_point] = j;
              warp_point_y[warp_point] = i;
              warp_point++;
            }
          }
        }
#ifdef _MAP_WARPPOINT
        else if (OBJECT_getType(o) == OBJTYPE_WARPPOINT) {
          int etype = OBJECT_getchartype(o);
          if (etype != CHAR_EVENT_NONE) {
            warp_point_x[warp_point] = j;
            warp_point_y[warp_point] = i;
            warp_point++;
            break;
          }
        }
#endif
      }
    }
  }

  for (i = 0; i < warp_point; i++) {
    if ((x == warp_point_x[i]) && (y == warp_point_y[i]))
      goto OK;
  }
  x = CHAR_getInt(fd_charaindex, CHAR_X);
  y = CHAR_getInt(fd_charaindex, CHAR_Y);
}
OK:
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    int i;
    for (i = 0; i < CHAR_PARTYMAX; i++) {
      int subindex = CHAR_getWorkInt(fd_charaindex, CHAR_WORKPARTYINDEX1 + i);
      if (CHAR_CHECKINDEX(subindex) == FALSE)
        continue;
    }
  }
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  CHAR_setWorkChar(fd_charaindex, CHAR_WORKWALKARRAY, "");

  if (dir < 0 || dir > 7) {
    fx = CHAR_getInt(fd_charaindex, CHAR_X);
    fy = CHAR_getInt(fd_charaindex, CHAR_Y);
  } else {
    CHAR_getCoordinationDir(dir, CHAR_getInt(fd_charaindex, CHAR_X),
                            CHAR_getInt(fd_charaindex, CHAR_Y), 1, &fx, &fy);
  }
  rc = EVENT_main(fd_charaindex, event, fx, fy);
  lssproto_EV_send(fd, seqno, rc);

#ifdef _NB_FAMILYBADGE
  if (CHAR_getInt(fd_charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      getFamilyBadge(fd_charaindex) >= 0) {
    char msg[256];
    sprintf(msg, "1|%d", getFamilyBadge(fd_charaindex));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
#ifdef _NB_CHARDISPTITLE
  if (CHAR_getInt(fd_charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(fd_charaindex, CHAR_NB_NAMEMOTO) > 0) {
    char msg[256];
    sprintf(msg, "4|%d", CHAR_getInt(fd_charaindex, CHAR_NB_NAMEMOTO));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
#ifdef _NB_CHARHALO
  // CHAR_HALO
  // CHAR_DISPTITLE
  if (CHAR_getInt(fd_charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(fd_charaindex, CHAR_NB_HALO) > 0) {
    char msg[256];
    sprintf(msg, "5|%d", CHAR_getInt(fd_charaindex, CHAR_NB_HALO));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_EN_recv(int fd, int x, int y) {
  int ret = FALSE, err = 0;
  int fd_charaindex;
  CHECKFD;
  fd_charaindex = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
    CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
    CHAR_setWorkChar(fd_charaindex, CHAR_WORKWALKARRAY, "");
    err = BATTLE_CreateVsEnemy(fd_charaindex, 0, -1);
    if (err != 0) {
      ret = FALSE;
    } else {
      ret = TRUE;
    }
  }
}
/*------------------------------------------------------------
 * �׫쫤��?���̪ǫ��󫫫���ȡ�???��?
 ------------------------------------------------------------*/
void lssproto_DU_recv(int fd, int x, int y) {
  OBJECT object;
  int fd_charaindex;
  int ret = FALSE, charaindex = -1, enemyindex;
  int frontx, fronty;
  int cnt = 0;
  BOOL found = FALSE;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  {
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      x = ix;
      y = iy;
    }
  }

  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
    int i;
    charaindex = fd_charaindex;
    CHAR_setMyPosition(charaindex, x, y, TRUE);
    CHAR_setWorkChar(charaindex, CHAR_WORKWALKARRAY, "");
    for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
      CONNECT_setDuelcharaindex(fd, i, -1);
    }
    CHAR_getCoordinationDir(
        CHAR_getInt(charaindex, CHAR_DIR), CHAR_getInt(charaindex, CHAR_X),
        CHAR_getInt(charaindex, CHAR_Y), 1, &frontx, &fronty);
    for (object =
             MAP_getTopObj(CHAR_getInt(charaindex, CHAR_FLOOR), frontx, fronty);
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
      found = TRUE;
      if (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
        continue;

#ifdef _ALLBLUES_LUA_1_5
      int flg = FreeVsPlayer(charaindex, toindex);
      if (flg == 1) {
        lssproto_EN_send(fd, FALSE, 0);
        return;
      } else if (flg == 2) {
        return;
      } else
#endif
          if (!CHAR_getFlg(toindex, CHAR_ISDUEL)) {
        int floor = CHAR_getInt(charaindex, CHAR_FLOOR);
#ifdef _AUTO_PK
        if (floor == 20000) {
          if (AutoPk_PKTimeGet() > 0) {
            CHAR_talkToCli(charaindex, -1, "PK message",
                           CHAR_COLORYELLOW);
            continue;
          } else {
            if (!CHAR_getFlg(toindex, CHAR_ISDUEL)) {
              if (CHAR_getWorkInt(charaindex, CHAR_WORK_AUTOPK) == -1)
                continue;
              if (CHAR_getWorkInt(toindex, CHAR_WORK_AUTOPK) == -1)
                continue;
            }
          }
        } else
#endif
#ifdef _BATTLE_FLOOR
        {
          if (strcmp(getBattleFloorCF(), "예") != 0) {
            int i;
            for (i = 0; i < 32; i++) {
              if (floor == getBattleFloor(i)) {
                break;
              }
            }
            if (i == 32)
              continue;
          }
        }
#else
        continue;
#endif
      } // shan begin
      {
        int i;
        for (i = 0; i < FAMILY_FMPKFLOOR; i++) {
          if (fmpkflnum[i].fl == CHAR_getInt(charaindex, CHAR_FLOOR)) {
            if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEFLAG) == -1) {
              lssproto_EN_send(fd, FALSE, 0);
              return;
            }
            if (CHAR_getInt(charaindex, CHAR_FMINDEX) ==
                CHAR_getInt(toindex, CHAR_FMINDEX)) {
              lssproto_EN_send(fd, FALSE, 0);
              return;
            }
          }
        }
      }
      // shan end

      // ���˪ʪ�Ρ��?��Ǫ���
      if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
        int tmpindex = CHAR_getWorkInt(toindex, CHAR_WORKPARTYINDEX1);
        /* ��?���׫쫤��?�Ǫʪ�?�⪢�� */
        if (CHAR_CHECKINDEX(tmpindex)) {
          if (CHAR_getWorkInt(tmpindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC
              && CHAR_getInt(tmpindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC
#endif
          ) {
            continue;
          }
        }
      }

      CONNECT_setDuelcharaindex(fd, cnt, toindex);
      cnt++;
      if (cnt == CONNECT_WINDOWBUFSIZE)
        break;
    }
    /* ��?��?�� */
    if (cnt == 0) {
      goto lssproto_DU_recv_Err;
    }
    /* ���Ԫ������ê���?����ȫ�? */
    else if (cnt == 1) {
      // ?��?�Ϋ����Ϋ���ǫë���
      enemyindex = CONNECT_getDuelcharaindex(fd, 0);
      // ��?��Ρ�ʪ骽�Ϊުޫ��󫫫���Ȫ����몬
      // ���˪ʪ�Ρ��?��Ǫ���
      if (CHAR_getWorkInt(enemyindex, CHAR_WORKPARTYMODE) ==
          CHAR_PARTY_CLIENT) {
        enemyindex = CHAR_getWorkInt(enemyindex, CHAR_WORKPARTYINDEX1);
        // �ʪ���Ρ�����ʪ�
        if (enemyindex < 0)
          goto lssproto_DU_recv_Err;
      }

#ifdef _VIP_POINT_PK
      {
        char token[256];
        if ((CHAR_getInt(charaindex, CHAR_FLOOR) == 11111 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 11111) ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) == 22222 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 22222) ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) == 33333 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 33333) ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) == 44444 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 44444) ||
            (CHAR_getInt(charaindex, CHAR_FLOOR) == 20000 &&
             CHAR_getInt(enemyindex, CHAR_FLOOR) == 20000)) {
          int menum = 0, tonum = 0;
          if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
              CHAR_PARTY_NONE) {
            menum = 1;
          } else {
            for (i = 0; i < CHAR_PARTYMAX; i++) {
              int index = CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1);
              if (CHAR_CHECKINDEX(index)) {
                menum++;
              }
            }
          }

          if (CHAR_getWorkInt(enemyindex, CHAR_WORKPARTYMODE) ==
              CHAR_PARTY_NONE) {
            tonum = 1;
          } else {
            for (i = 0; i < CHAR_PARTYMAX; i++) {
              int index = CHAR_getWorkInt(enemyindex, i + CHAR_WORKPARTYINDEX1);
              if (CHAR_CHECKINDEX(index)) {
                tonum++;
              }
            }
          }

          if (menum != tonum) {
            sprintf(token, "�۰??��?%d��?۰??��?%d�������?��??������??��", menum,
                    tonum);
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            lssproto_EN_send(fd, FALSE, 0);
            return;
          }
        }

        if (CHAR_getInt(charaindex, CHAR_FLOOR) == 44444 &&
            CHAR_getInt(enemyindex, CHAR_FLOOR) == 44444) {
          if (CHAR_getInt(charaindex, CHAR_AMPOINT) < getVipPointPK(3)) {
            sprintf(token, "?��?��������%d������?��PK��", getVipPointPK(3));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          } else if (CHAR_getInt(enemyindex, CHAR_AMPOINT) < getVipPointPK(3)) {
            sprintf(token, "?۰?��������%d������?��PK��", getVipPointPK(3));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          }
        } else if (CHAR_getInt(charaindex, CHAR_FLOOR) == 33333 &&
                   CHAR_getInt(enemyindex, CHAR_FLOOR) == 33333) {
          if (CHAR_getInt(charaindex, CHAR_AMPOINT) < getVipPointPK(2)) {
            sprintf(token, "?��?��������%d������?��PK��", getVipPointPK(2));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          } else if (CHAR_getInt(enemyindex, CHAR_AMPOINT) < getVipPointPK(2)) {
            sprintf(token, "?۰?��������%d������?��PK��", getVipPointPK(2));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          }
        } else if (CHAR_getInt(charaindex, CHAR_FLOOR) == 22222 &&
                   CHAR_getInt(enemyindex, CHAR_FLOOR) == 22222) {
          if (CHAR_getInt(charaindex, CHAR_AMPOINT) < getVipPointPK(1)) {
            sprintf(token, "?��?��������%d������?��PK��", getVipPointPK(1));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          } else if (CHAR_getInt(enemyindex, CHAR_AMPOINT) < getVipPointPK(1)) {
            sprintf(token, "?۰?��������%d������?��PK��", getVipPointPK(1));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          }
        } else if (CHAR_getInt(charaindex, CHAR_FLOOR) == 11111 &&
                   CHAR_getInt(enemyindex, CHAR_FLOOR) == 11111) {
          if (CHAR_getInt(charaindex, CHAR_AMPOINT) < getVipPointPK(0)) {
            sprintf(token, "?��?��������%d������?��PK��", getVipPointPK(0));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          } else if (CHAR_getInt(enemyindex, CHAR_AMPOINT) < getVipPointPK(0)) {
            sprintf(token, "?۰?��������%d������?��PK��", getVipPointPK(0));
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
            goto lssproto_DU_recv_Err;
          }
        }
      }
#endif

      ret = BATTLE_CreateVsPlayer(charaindex, enemyindex);
      if (ret != 0) {
        ret = FALSE;
      } else {
        ret = TRUE;
      }
    }
    /* ����??������Ф�ϫ�����ɫ���У����𦪤Ф�請�� */
    else if (cnt > 1) {
      int strlength;
      char msgbuf[1024];
      char escapebuf[2048];
      strcpy(msgbuf, "1\n누가 싸우고 싶어?\n");
      strlength = strlen(msgbuf);
      /* ������ɫ��Ϋ�ë�?��������
       * ��?���Ϋ�����?��
       */
      for (i = 0;
           CONNECT_getDuelcharaindex(fd, i) != -1 && i < CONNECT_WINDOWBUFSIZE;
           i++) {
        char *a = CHAR_getChar(CONNECT_getDuelcharaindex(fd, i), CHAR_NAME);
        char buf[256];
        snprintf(buf, sizeof(buf), "%s [%s]\n", a,
                 CHAR_getWorkInt(CONNECT_getDuelcharaindex(fd, i),
                                 CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE
                    ? "그룹"
                    : "홀로");
        if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
          print("%s:%d창 메시지 버퍼가 충분하지 않습니다.\n", __FILE__, __LINE__);
          break;
        }
        strcpy(&msgbuf[strlength], buf);
        strlength += strlen(buf);
      }

      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL,
                       CHAR_WINDOWTYPE_SELECTDUEL, -1,
                       makeEscapeString(msgbuf, escapebuf, sizeof(escapebuf)));
      ret = TRUE;
    }
  }

lssproto_DU_recv_Err:;
  if (ret == FALSE) {
    /* ��?��? */
    lssproto_EN_send(fd, FALSE, 0);
    if (cnt > 0)
			// CHAR_talkToCli(charaindex, -1, "실패가 있습니다!", CHAR_COLORYELLOW);
      return;
    else if (found)
			CHAR_talkToCli(charaindex, -1, "아무도 싸울 수 없습니다.", CHAR_COLORYELLOW);
    else
			CHAR_talkToCli(charaindex, -1, "거기에있는 사람이 없습니다.", CHAR_COLORYELLOW);
  }
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_EO_recv(int fd, int dummy) {
  int fd_charaindex;
  CHECKFD;
  fd_charaindex = CONNECT_getCharaindex(fd);
  BattleEncountOut(fd_charaindex);
#ifdef _NB_FAMILYBADGE
  if (CHAR_getInt(fd_charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      getFamilyBadge(fd_charaindex) >= 0) {
    char msg[256];
    sprintf(msg, "1|%d", getFamilyBadge(fd_charaindex));
    lssproto_CHAREFFECT_send(fd, msg);
  }
#endif
#ifdef _NB_CHARDISPTITLE
  if (CHAR_getInt(fd_charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(fd_charaindex, CHAR_NB_NAMEMOTO) > 0) {
    char msg[256];
    sprintf(msg, "4|%d", CHAR_getInt(fd_charaindex, CHAR_NB_NAMEMOTO));
    lssproto_CHAREFFECT_send(getfdFromCharaIndex(fd_charaindex), msg);
  }
#endif
#ifdef _NB_CHARHALO
  if (CHAR_getInt(fd_charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(fd_charaindex, CHAR_NB_HALO) > 0) {
    char msg[256];
    sprintf(msg, "5|%d", CHAR_getInt(fd_charaindex, CHAR_NB_HALO));
    lssproto_CHAREFFECT_send(getfdFromCharaIndex(fd_charaindex), msg);
  }
#endif
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_BU_recv(int fd, int dummy) {
  int fd_charaindex;
  CHECKFD;
  fd_charaindex = CONNECT_getCharaindex(fd);
  // shan 2001/12/25
  // BATTLE_WatchStop( fd_charaindex );
}

void lssproto_B_recv(int fd, char *command) {
  int fd_charaindex;
  int battle_index; // ttom++
  CHECKFD;
  fd_charaindex = CONNECT_getCharaindex(fd);
  // print("Recv:%s\n",command);
  BattleCommandDispach(fd, command);
  // Nuke +1 0827: Battle acceleration
  battle_index = CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEINDEX);
  if (BATTLE_CHECKINDEX(battle_index) == FALSE) {
    return;
  }
}

void lssproto_FS_recv(int fd, int flg) {
  int fd_charaindex;
  CHECKFDANDTIME;

  fd_charaindex = CONNECT_getCharaindex(fd);
  /* ?������ܦ׿?ܦ����������?��?�� */
  CHAR_setFlg(fd_charaindex, CHAR_ISPARTY,
              (flg & CHAR_FS_PARTY) ? TRUE : FALSE);
  //	CHAR_setFlg( fd_charaindex, CHAR_ISBATTLE,
  //				(flg & CHAR_FS_BATTLE )? TRUE:FALSE);

  CHAR_setFlg(fd_charaindex, CHAR_ISDUEL, (flg & CHAR_FS_DUEL) ? TRUE : FALSE);
  CHAR_setFlg(fd_charaindex, CHAR_ISPARTYCHAT,
              (flg & CHAR_FS_PARTYCHAT) ? TRUE : FALSE);
  CHAR_setFlg(fd_charaindex, CHAR_ISTRADECARD,
              (flg & CHAR_FS_TRADECARD) ? TRUE : FALSE);
#ifdef _CHANNEL_MODIFY
  //��??Գ??
  CHAR_setFlg(fd_charaindex, CHAR_ISTELL, (flg & CHAR_FS_TELL) ? TRUE : FALSE);
  //ʫ��?Գ??
  CHAR_setFlg(fd_charaindex, CHAR_ISFM, (flg & CHAR_FS_FM) ? TRUE : FALSE);
  //???Գ??
  CHAR_setFlg(fd_charaindex, CHAR_ISOCC, (flg & CHAR_FS_OCC) ? TRUE : FALSE);
  //������
  CHAR_setFlg(fd_charaindex, CHAR_ISCHAT, (flg & CHAR_FS_CHAT) ? TRUE : FALSE);
  //?��????
  CHAR_setFlg(fd_charaindex, CHAR_ISSAVE, (flg & CHAR_FS_SAVE) ? TRUE : FALSE);

#endif
  // CoolFish: Trade 2001/4/18
  CHAR_setFlg(fd_charaindex, CHAR_ISTRADE,
              (flg & CHAR_FS_TRADE) ? TRUE : FALSE);
  /*
  if (CHAR_getFlg(fd_charaindex, CHAR_ISTRADECARD) == TRUE)
          CHAR_setFlg(fd_charaindex, CHAR_ISTRADE, FALSE);
  if (CHAR_getFlg(fd_charaindex, CHAR_ISTRADE) == TRUE)
          CHAR_setFlg(fd_charaindex, CHAR_ISTRADECARD, FALSE);
  */
  lssproto_FS_send(fd, flg);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PR_recv(int fd, int x, int y, int request) {
  int result = FALSE;
  int fd_charaindex;
  CHECKFDANDTIME;

  fd_charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(fd_charaindex))
    return;
  print("request::%d\n", request);

  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) !=
      BATTLE_CHARMODE_NONE)
    return;
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

#if 1 // ���???�
  if (request == 1) {
    int nowFloor;
    nowFloor = CHAR_getInt(fd_charaindex, CHAR_FLOOR);
    if (nowFloor == 31706 || nowFloor == 10204 ||
        (10601 <= nowFloor && nowFloor <= 10605) || nowFloor == 10919 ||
        nowFloor == 10920 || nowFloor == 20711 || nowFloor == 20712 ||
        nowFloor == 1008 || nowFloor == 1021 || nowFloor == 3008 ||
        nowFloor == 3021 || (nowFloor <= 8213 && nowFloor >= 8200) ||
        (nowFloor >= 30017 && nowFloor <= 30021)
#ifdef _TIME_TICKET
        || check_TimeTicketMap(nowFloor)
#endif
    ) {
      print("\n 봉인 된 팩! 그것은 팀을 위로하는 것이 금지되어 있습니다 !!:%s ", CHAR_getChar(fd_charaindex, CHAR_CDKEY));
      return;
    }
  }
#endif

  {
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      // print("\n<PR>--Error!!!!");
      // print("\n<PR>origion x=%d,y=%d",ix,iy);
      // print("\n<PR>modify  X=%d,Y=%d",x,y);
      x = ix;
      y = iy;
    }
  }
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  print("sxy::\n");
  if (request == 0) {
#ifdef _LUA_CHECKPARTYOUT
    FreeDischargeParty(fd_charaindex, request);
#endif
    result = CHAR_DischargeParty(fd_charaindex, 0);
  } else if (request == 1) {
    result = CHAR_JoinParty(fd_charaindex);
  }
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_KS_recv(int fd, int petarray) {
  int ret, fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(fd_charaindex))
    return;
  /*if(petarray != -1 && CHAR_getInt( fd_charaindex, CHAR_RIDEPET) == petarray
    ){ lssproto_KS_send( fd, petarray, FALSE); return;
        }*/
  ret = PET_SelectBattleEntryPet(fd_charaindex, petarray);
  lssproto_KS_send(fd, petarray, ret);
}

void lssproto_SPET_recv(int fd, int standbypet) {
  int fd_charaindex;
  int i, s_pet = 0, cnt = 0;

  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(fd_charaindex))
    return;
  // if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE) !=
  // BATTLE_CHARMODE_NONE
  //	&& standbypet >= CHAR_getWorkInt( fd_charaindex, CHAR_WORKSTANDBYPET) )
  //{ 	print("\n ������!???������SPET��ʥ����?!!:%s ", CHAR_getChar(
  //fd_charaindex, CHAR_CDKEY) ); 	return;
  // }

  // if( CHAR_getInt( fd_charaindex, CHAR_RIDEPET) == petarray ) {
  //	lssproto_SPET_send( fd, petarray, FALSE);
  // }

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    if (standbypet & (1 << i)) {

      if (CHAR_getInt(fd_charaindex, CHAR_RIDEPET) == i)
        continue;

      cnt++;
      // if( cnt > 4 ) {
      //	print("\n ������!����?��??��!!:%s ", CHAR_getChar(
      //fd_charaindex, CHAR_CDKEY) ); lssproto_SPET_send( fd, s_pet, FALSE);
      //	break;
      //}

      s_pet |= (1 << i);
    }
  }
  CHAR_setWorkInt(fd_charaindex, CHAR_WORKSTANDBYPET, s_pet);

  lssproto_SPET_send(fd, s_pet, TRUE);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_AC_recv(int fd, int x, int y, int actionno) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  int fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);

  { // ttom avoid the warp at will
    Char *ch;
    ch = CHAR_getCharPointer(fd_charaindex);
    // CoolFish: +1 2001/11/05
    if (!ch)
      return;
    if ((ch->data[CHAR_X] != x) || (ch->data[CHAR_Y] != y)) {
      return;
    }
  }
#ifdef _ALLBLUES_LUA_1_9
  FreeAction(fd_charaindex, x, y, actionno);
#endif
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  CHAR_sendAction(fd_charaindex, actionno, FALSE);
  return;
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_MU_recv(int fd, int x, int y, int array, int toindex) {
  int to_charaindex = -1, fd_charaindex;
  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  { // ttom avoid warp at will
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      // print("\n<MU>--Error!!!!");
      // print("\n<MU>origion x=%d,y=%d",ix,iy);
      // print("\n<MU>modify  X=%d,Y=%d",x,y);
      x = ix;
      y = iy;
    }
  }

  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  to_charaindex = Callfromcli_Util_getTargetCharaindex(fd, toindex);
  MAGIC_Use(fd_charaindex, array, to_charaindex);
}

void lssproto_JB_recv(int fd, int x, int y) {
  int charaindex, floor;

  CHECKFDANDTIME;
  charaindex = CONNECT_getCharaindex(fd);
  {
    int ix, iy;
    ix = CHAR_getInt(charaindex, CHAR_X);
    iy = CHAR_getInt(charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      x = ix;
      y = iy;
    }
  }

  CHAR_setMyPosition(charaindex, x, y, TRUE);
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  if (floor == 1007 || floor == 2007 || floor == 3007 || floor == 4007 ||
      floor == 130
#ifdef _AUTO_PK
      || (floor == 20000 && CHAR_getWorkInt(charaindex, CHAR_WORK_AUTOPK) == -1)
#endif
#ifdef _WATCH_FLOOR
      || floor == getWatchFloor(1) || floor == getWatchFloor(2) ||
      floor == getWatchFloor(3) || floor == getWatchFloor(4) ||
      floor == getWatchFloor(5) || !strcmp(getWatchFloorCF(), "��")
#endif
  ) {
    BATTLE_WatchTry(charaindex);
  } else {
    if (BATTLE_RescueTry(charaindex) == FALSE) {
      BATTLE_WatchTry(charaindex);
    }
  }
}

void lssproto_KN_recv(int fd, int havepetindex, char *data) {
  int fd_charaindex;
  CHECKFD;
  fd_charaindex = CONNECT_getCharaindex(fd);

  // Robin 04/26 debug
  if (strlen(data) > 16)
    return;

  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;

  if (checkStringErr(data))
    return;

  CHAR_inputUserPetName(fd_charaindex, havepetindex, data);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_WN_recv(int fd, int x, int y, int seqno, int objindex, int select,
                      char *data) {
  int fd_charaindex;

  CHECKFDANDTIME;

  if (checkStringErr(data))
    return;
  fd_charaindex = CONNECT_getCharaindex(fd);
#ifdef _NB_SAVEWININDEX
  // if (seqno == WINDOW_MESSAGETYPE_MESSAGE){
  if (CHAR_getWorkInt(fd_charaindex, CHAR_NBWORKWININDX) != seqno) {
    return;
  }
  //}
#endif
  // CoolFish: Prevent Trade Cheat 2001/4/18
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) {
    return;
  }
  // Robin
  if (checkStringErr(data))
    return;

#ifdef _ANGEL_SUMMON
  if (seqno == CHAR_WINDOWTYPE_ANGEL_ASK) {
    print("\n CHAR_WINDOWTYPE_ANGEL_ASK objindex:%d select:%d data:%s ",
          objindex, select, data);

    if (select == WINDOW_BUTTONTYPE_YES) {
      if (AngelCreate(fd_charaindex) == FALSE) {
        sendAngelCleanToCli(fd);
      }
    } else if (select == WINDOW_BUTTONTYPE_NO) {
      int mindex;
      char nameinfo[64];
      mindex = checkIfAngel(fd_charaindex);
      print(" ====소환 작업을 수락하지 마십시오==== ");
      getMissionNameInfo(fd_charaindex, nameinfo);
      saacproto_ACMissionTable_send(acfd, mindex, 3, nameinfo, "");

      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK, -1,
                       -1,
                       "어떤 동정이 있습니다. \n 당신의 도움이 적게, 그것은 모즈가 본토의 백성을 지속적으로 위험에 빠뜨릴 것으로 보인다.");

      // CHAR_talkToCli( fd_charaindex, -1, "����?��?��ʦୣ�?ʦ��?������??��",
      // CHAR_COLORYELLOW);

      sendAngelCleanToCli(fd);
    }
  }
#endif

  { // ttom avoid the warp at will
    Char *ch;
    ch = CHAR_getCharPointer(fd_charaindex);
    // CoolFish: +1 2001/11/05
    if (!ch)
      return;
    if ((ch->data[CHAR_X] != x) || (ch->data[CHAR_Y] != y)) {
      // Robin 04/20 test
      return;
      x = ch->data[CHAR_X];
      y = ch->data[CHAR_Y];
    }
    // ttom avoid WN at will
    if (seqno == CHAR_WINDOWTYPE_NPCENEMY_START) {
      //      Char *ch;
      OBJECT object;
      int ix, iy, ifloor, i, j;
      int whichtype = -1;
      int enemy = 0;
      int enemy_index;
      if (!CHECKOBJECT(objindex)) {
        goto FIRST;
      }
      enemy_index = OBJECT_getIndex(objindex); // ttom 11/15/2000
      ix = ch->data[CHAR_X];
      iy = ch->data[CHAR_Y];
      ifloor = ch->data[CHAR_FLOOR];
      for (i = iy - 1; i <= iy + 1; i++) {
        for (j = ix - 1; j <= ix + 1; j++) {
          for (object = MAP_getTopObj(ifloor, j, i); object;
               object = NEXT_OBJECT(object)) {
            int objindex = GET_OBJINDEX(object);
            switch (OBJECT_getType(objindex)) {
            case OBJTYPE_CHARA:
              whichtype =
                  CHAR_getInt(OBJECT_getIndex(objindex), CHAR_WHICHTYPE);
              if (whichtype == CHAR_TYPENPCENEMY) {
                int i_ene_temp;
                i_ene_temp = OBJECT_getIndex(objindex);
                // print("\n<WN>--enetemp=%d",i_ene_temp);
                if (i_ene_temp == enemy_index) {
                  goto START_WN;
                } else {
                  enemy = 0;
                }
              }
              break;
            case OBJTYPE_ITEM:
              break;
            case OBJTYPE_GOLD:
              break;
            default:
              break;
            }
          }
        }
      }
      if (enemy == 0) {
      FIRST:

        lssproto_EN_send(fd, FALSE, 0);
        CHAR_talkToCli(fd_charaindex, -1, "이벤트 오류.", CHAR_COLORYELLOW);
        goto END_WN;
      }
    }
  }
  // ttom end
START_WN:
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) ==
      BATTLE_CHARMODE_NONE) {
    if (seqno == CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN) {
      if (!CONNECT_get_first_warp(fd)) {
        select = 1;
      }
    }
    CHAR_processWindow(fd_charaindex, seqno, select, objindex,
                       makeStringFromEscaped(data));
  }
// ttom+1
END_WN:
  return;
  // CONNECT_set_pass(fd,TRUE);//ttom
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_HL_recv(int fd, int flg) {
  char msgbuf[128];
  int i, fd_charaindex;
  CHECKFD;

  fd_charaindex = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEMODE) ==
      BATTLE_CHARMODE_NONE) {
    return;
  }
#ifdef _LOCKHELP_OK // (不可开) Syu ADD 锁定不可加入战斗
  if ((CHAR_getInt(fd_charaindex, CHAR_FLOOR) >= 8200 &&
       CHAR_getInt(fd_charaindex, CHAR_FLOOR) <= 8213) ||
      (CHAR_getInt(fd_charaindex, CHAR_FLOOR) >= 30017 &&
       CHAR_getInt(fd_charaindex, CHAR_FLOOR) <= 30021) ||
      CHAR_getInt(fd_charaindex, CHAR_FLOOR) == 20000) {
    return;
  }
#endif
#ifdef _NO_HELP_MAP
  {
    int floor = CHAR_getInt(fd_charaindex, CHAR_FLOOR);
    int i;
    for (i = 0; i < 32; i++) {
      if (floor == getNoHelpMap(i)) {
        CHAR_setWorkInt(fd_charaindex, CHAR_WORK_NOHELP, 1);
      }
    }
  }
#endif
#ifdef _NB_NOBATTLEBUTTON
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORK_NOHELP)) {
    lssproto_HL_send(fd, FALSE);
    CHAR_talkToCli(fd_charaindex, -1, "CHAR_WORK_NOHELP", CHAR_COLORRED);
    return;
  }
#endif

  if (flg == TRUE) {
    BattleArray[CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEINDEX)]
        .Side[CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLESIDE)]
        .flg |= BSIDE_FLG_HELP_OK;
    snprintf(msgbuf, sizeof(msgbuf), "%s 돕다!",
             CHAR_getChar(fd_charaindex, CHAR_NAME));
  } else {
    BattleArray[CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEINDEX)]
        .Side[CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLESIDE)]
        .flg &= ~BSIDE_FLG_HELP_OK;
    snprintf(msgbuf, sizeof(msgbuf), "%s 도움을 거부하기로 결정하십시오.",
             CHAR_getChar(fd_charaindex, CHAR_NAME));
  }

  for (i = 0; i < 5; i++) {
    int toindex =
        BattleArray[CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLEINDEX)]
            .Side[CHAR_getWorkInt(fd_charaindex, CHAR_WORKBATTLESIDE)]
            .Entry[i]
            .charaindex;
    if (CHAR_CHECKINDEX(toindex)) {
      int tofd = getfdFromCharaIndex(toindex);
      if (tofd != -1) {
        lssproto_HL_send(tofd, flg);
      }
			/* 丢永本□斥霜耨 */
      CHAR_talkToCli(toindex, -1, msgbuf, CHAR_COLORYELLOW);
      CHAR_sendBattleEffect(toindex, ON);
    }
  }
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_ProcGet_recv(int fd) { outputNetProcLog(fd, 1); }
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PlayerNumGet_recv(int fd) {
  /*
          int		i;
          int		clicnt  =0;
          int		playercnt = 0;
      for( i = 0; i < ConnectLen; i ++ ) {
          if( CONNECT_getUse_debug(i,1017) ){
              if( CONNECT_getCtype(i) == CLI) {
                  clicnt ++;
                  if( CONNECT_getCharaindex(i) >= 0 ) playercnt++;
              }
                  }
          }
          lssproto_PlayerNumGet_send( fd, clicnt * 0.6, playercnt * 0.6);
  */
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_LB_recv(int fd, int x, int y) {
  int fd_charaindex;

  CHECKFDANDTIME;
  fd_charaindex = CONNECT_getCharaindex(fd);
  { // ttom avoid warp at will
    int ix, iy;
    ix = CHAR_getInt(fd_charaindex, CHAR_X);
    iy = CHAR_getInt(fd_charaindex, CHAR_Y);
    if ((ix != x) || (iy != y)) {
      // print("\n<LB>--Error!!!!");
      // print("\n<LB>origion x=%d,y=%d",ix,iy);
      // print("\n<LB>modify  X=%d,Y=%d",x,y);
      x = ix;
      y = iy;
    }
  }
  CHAR_setMyPosition(fd_charaindex, x, y, TRUE);

  BATTLE_WatchTry(fd_charaindex);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_Shutdown_recv(int fd, char *passwd, int min) {
  char buff[32];
  if (strcmp(passwd, "hogehoge") == 0) {
    int i;
    int playernum = CHAR_getPlayerMaxNum();
    snprintf(buff, sizeof(buff), "%s발표.", getGameservername());
    for (i = 0; i < playernum; i++) {
      if (CHAR_getCharUse(i) != FALSE) {
        CHAR_talkToCli(i, -1, buff, CHAR_COLORYELLOW);
      }
    }
    SERVSTATE_setLimittime(min);
    SERVSTATE_setShutdown(NowTime.tv_sec);
    SERVSTATE_setDsptime(0);
  }
}
void lssproto_PMSG_recv(int fd, int index, int petindex, int itemindex,
                        char *message, int color) {

  // CoolFish: Prevent Trade Cheat 2001/4/18
  int fd_charaindex;
  fd_charaindex = CONNECT_getCharaindex(fd);

  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
#ifdef _NB_CDKEYLOCK
  char *cdkey = CHAR_getChar(fd_charaindex, CHAR_CDKEY);
  if (sasql_getLock(cdkey) > 0) {
    CHAR_talkToCli(fd_charaindex, -1, "_NB_CDKEYLOCK message", 6);
    return;
  }
#endif
#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif
#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(fd_charaindex, CHAR_LOCKED) == 1 && getItemPetLocked() == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "_ITEM_PET_LOCKED message");

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif

  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
  PETMAIL_sendPetMail(CONNECT_getCharaindex(fd), index, petindex, itemindex,
                      message, color);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PS_recv(int fd, int havepetindex, int havepetskill, int toindex,
                      char *data) {
  int charaindex = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
  int to_charaindex = Callfromcli_Util_getTargetCharaindex(fd, toindex);
  int petindex;
  BOOL ret;
  petindex = CHAR_getCharPet(charaindex, havepetindex);
  if (!CHAR_CHECKINDEX(petindex))
    return;

  ret = PETSKILL_Use(petindex, havepetskill, to_charaindex, data);
  lssproto_PS_send(fd, ret, havepetindex, havepetskill, toindex);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_SP_recv(int fd, int x, int y, int dir) {
  int fd_charaindex;

  fd_charaindex = CONNECT_getCharaindex(fd);
  { // ttom avoid the warp at will
    int i_x, i_y;
    i_x = CHAR_getInt(fd_charaindex, CHAR_X);
    i_y = CHAR_getInt(fd_charaindex, CHAR_Y);

    if ((i_x != x) || (i_y != y)) {
      x = i_x;
      y = i_y;
    }
  } // ttom

  CHAR_setMyPosition_main(fd_charaindex, x, y, dir, TRUE);
}

/*------------------------------------------------------------
 * CoolFish: Trade Command 2001/4/18
 ------------------------------------------------------------*/
void lssproto_TD_recv(int fd, char *message) {
  int fd_charaindex;
  CHECKFDANDTIME;

  fd_charaindex = CONNECT_getCharaindex(fd);
  //      print(" MAP_TRADEPICKUP_check0 ");

  if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKSTREETVENDOR) != -1) {
    CHAR_talkToCli(fd_charaindex, -1, "Trade Command 1", CHAR_COLORYELLOW);
    return;
  }
  CHAR_Trade(fd, fd_charaindex, message);
}

/*------------------------------------------------------------
 * CoolFish: Family Command 2001/5/24
 ------------------------------------------------------------*/
void lssproto_FM_recv(int fd, char *message) {
  int fd_charaindex;
  struct timeval recvtime;
  CHECKFDANDTIME;

  // add code by shan
  CONNECT_getLastrecvtime(fd, &recvtime);
  if (time_diff(NowTime, recvtime) < 0.5) {
    return;
  }
  CONNECT_setLastrecvtime(fd, &NowTime);

  fd_charaindex = CONNECT_getCharaindex(fd);

  if (checkStringErr(message))
    return;

  CHAR_Family(fd, fd_charaindex, message);
}

// shan 2002/01/10
void lssproto_PETST_recv(int fd, int nPet, int sPet) {
  int charaindex;
  int i, nums = 0;
  CHECKFDANDTIME;

  charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;

  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;

  for (i = 0; i < 5; i++) {
    if (CHAR_getWorkInt(charaindex, CHAR_WORK_PET0_STAT + i) == TRUE)
      nums++;
  }

  if (nPet < 0)
    nPet = 0;
  if (nPet > 4)
    nPet = 4;

  if (nums <= 3) {
    CHAR_setWorkInt(charaindex, CHAR_WORK_PET0_STAT + nPet, sPet);
  }

  if (nPet != CHAR_getInt(charaindex, CHAR_DEFAULTPET)) {
    lssproto_PETS_send(fd, nPet, sPet);
  }
}

#ifdef _MIND_ICON
void lssproto_MA_recv(int fd, int x, int y, int nMind) {
  int charaindex;
  CHECKFDANDTIME;

  charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;

  {
    int i_x, i_y;
    i_x = CHAR_getInt(charaindex, CHAR_X);
    i_y = CHAR_getInt(charaindex, CHAR_Y);

    if ((i_x != x) || (i_y != y)) {
      x = i_x;
      y = i_y;
    }
  }

  // print("\nshan------------------>mind action->%d x->%d y->%d", nMind, x, y);
  CHAR_setMyPosition(charaindex, x, y, TRUE);
  CHAR_setWorkInt(charaindex, CHAR_MIND_NUM, nMind);
  CHAR_sendMindEffect(charaindex, CHAR_getWorkInt(charaindex, CHAR_MIND_NUM));
  if (CHAR_getWorkInt(charaindex, CHAR_MIND_NUM) != 101290 &&
      CHAR_getWorkInt(charaindex, CHAR_MIND_NUM) != 101294 &&
      CHAR_getWorkInt(charaindex, CHAR_MIND_NUM) != 101288)
    CHAR_setWorkInt(charaindex, CHAR_MIND_NUM, 0);
  // print("\nshan------------------>end");

  return;
}
#endif
BOOL checkStringErr(char *checkstring) {
  int i, ach;
  for (i = 0, ach = 0; i < strlen(checkstring); i++) {
    if ((unsigned char)checkstring[i] == 0xff) {
      ach = 1;
      break;
    } // Force no 0xff
    if ((unsigned char)checkstring[i] == 0x80) {
      ach = 1;
      break;
    } // Force no 0x80
    if ((unsigned char)checkstring[i] == 0x7f) {
      ach = 1;
      break;
    } // Force no 0x7f
    if ((unsigned char)checkstring[i] <= 0x20) {
      ach = 1;
      break;
    } // Force greater than 0x20
    if ((unsigned char)checkstring[i] == 0x27) {
      ach = 1;
      break;
    } // Force greater than 0x27
    if ((unsigned char)checkstring[i] == 0x3b) {
      ach = 1;
      break;
    } // Force greater than 0x3b
    if ((unsigned char)checkstring[i] == 0x2c) {
      ach = 1;
      break;
    } // Force greater than 0x2c
    if ((unsigned char)checkstring[i] == 0x3d) {
      ach = 1;
      break;
    } // Force greater than 0x2c
    if (ach) {
      if ((((unsigned char)checkstring[i] >= 0x40) &&
           ((unsigned char)checkstring[i] <= 0x7e)) ||
          (((unsigned char)checkstring[i] >= 0xa1) &&
           ((unsigned char)checkstring[i] <= 0xfe)))
        ach = 0;
    } else {
      if (((unsigned char)checkstring[i] >= 0xa1) &&
          ((unsigned char)checkstring[i] <= 0xfe))
        ach = 1;
    }
  }
  if (ach) {
    print(" StringDog! ");
    return TRUE;
  }

  return FALSE;
}

#ifdef _TEAM_KICKPARTY
void lssproto_KTEAM_recv(int fd, int si) {
  int charaindex = -1, pindex;
  if (si < 0 || si > 5)
    return;
  charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;

  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER)
    return;

  pindex = CHAR_getWorkInt(charaindex, si + CHAR_WORKPARTYINDEX1);
  if (!CHAR_CHECKINDEX(pindex))
    return;
  if (CHAR_getWorkInt(pindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER)
    return;

  if (CHAR_DischargeParty(pindex, 0) == FALSE) {
    CHAR_talkToCli(charaindex, -1, "추방 오류!", CHAR_COLORYELLOW);
  } else {
    char buf1[256];
    sprintf(buf1, "팀 리더[%s] 추방!", CHAR_getUseName(charaindex));
    CHAR_talkToCli(pindex, -1, buf1, CHAR_COLORYELLOW);
    sprintf(buf1, "[%s]팀을 쫓아 낸다!", CHAR_getUseName(pindex));
    CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
  }
}
#endif

#ifdef _CHATROOMPROTOCOL // (不可开) Syu ADD 聊天室频道
void lssproto_CHATROOM_recv(int fd, char *data) { ChatRoom_recvall(fd, data); }
#endif

#ifdef _NEWREQUESTPROTOCOL // (不可开) Syu ADD 新增Protocol要求细项
void lssproto_RESIST_recv(int fd) {
  int charindex = -1;

  char token[256];
  charindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charindex))
    return;
  sprintf(token, "%d|%d|%d|%d|%d|%d|%d|%d",
          CHAR_getInt(charindex, CHAR_EARTH_RESIST),
          CHAR_getInt(charindex, CHAR_WATER_RESIST),
          CHAR_getInt(charindex, CHAR_FIRE_RESIST),
          CHAR_getInt(charindex, CHAR_WIND_RESIST),
          CHAR_getInt(charindex, CHAR_EARTH_EXP),
          CHAR_getInt(charindex, CHAR_WATER_EXP),
          CHAR_getInt(charindex, CHAR_FIRE_EXP),
          CHAR_getInt(charindex, CHAR_WIND_EXP));
  lssproto_RESIST_send(fd, token);
}
#endif

#ifdef _OUTOFBATTLESKILL // (不可开) Syu ADD 非战斗时技能Protocol
void lssproto_BATTLESKILL_recv(int fd, int iNum) {
#ifdef _CHAR_PROFESSION // WON ADD ��ڪ??
  int charaindex = CONNECT_getCharaindex(fd);
#ifndef _PROSKILL_OPTIMUM
  int skillindex = -1, char_pskill = -1, profession_skill = -1;
#endif

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;

#ifndef _PROSKILL_OPTIMUM // Robin fix cancel 此处略过职业检查, 改在 PROFESSION_SKILL_Use 中检查
	// 人物的职业
  char_pskill = CHAR_getInt(charaindex, PROFESSION_CLASS);

  skillindex = PROFESSION_SKILL_GetArray(charaindex, iNum);
  int Pskillid = skillindex;
	// 技能的职业
  profession_skill =
      PROFESSION_SKILL_getInt(Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);

  if ((char_pskill > 0) && (char_pskill == profession_skill)) {
#else
  if (1) {
#endif
    if (PROFESSION_SKILL_Use(charaindex, iNum, 0, NULL) != 1) {
      if (CHAR_getWorkInt(charaindex, CHAR_ENCOUNT_NUM) - (int)time(NULL) > 0) {
//				print("\n 玩家(%s)使用技能错误!", CHAR_getUseName( charaindex ) );
#ifdef _PROSKILL_ERR_KICK
        CONNECT_setCloseRequest(fd, 1);
#endif
      }
    }
  }
#endif
}

#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_recv(int fd, char *message) {
  int charaindex = CONNECT_getCharaindex(fd);

  if (!CHAR_CHECKINDEX(charaindex))
    return;

#ifdef _NB_CDKEYLOCK
  {
    char *cdkey = CHAR_getChar(charaindex, CHAR_CDKEY);
    if (sasql_getLock(cdkey) > 0) {
      CHAR_talkToCli(charaindex, -1, "_NB_CDKEYLOCK", 6);
      return;
    }
  }
#endif
#ifdef _ITEM_PET_LOCKED
  if (CHAR_getInt(charaindex, CHAR_LOCKED) == 1 && getItemPetLocked() == 1) {
    char message[256];
    char buf[256];
    sprintf(message, "_ITEM_PET_LOCKED message");

    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                     WINDOW_BUTTONTYPE_OKCANCEL,
                     CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
                     makeEscapeString(message, buf, sizeof(buf)));

    return;
  }
#endif

  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    CHAR_talkToCli(charaindex, -1, "组队状态下不能交易", CHAR_COLORYELLOW);
    return;
  }

  CHAR_sendStreetVendor(charaindex, message);
}
#endif

#ifdef _RIGHTCLICK
void lssproto_RCLICK_recv(int fd, int type, char *data) {
  print("\n RCLICK_recv( type=%d data=%s) ", type, data);
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_recv(int fd, char *data) {
  int charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;

  CHAR_JobDaily(charaindex, data);
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_recv(int fd, char *data) {
  int charaindex = CONNECT_getCharaindex(fd);

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  CHAR_Teacher_system(charaindex, data);
}
#endif

#ifdef _ONLINE_SHOP
void lssproto_VIP_SHOP_recv(int fd, int type, int page) {
  int charaindex = CONNECT_getCharaindex(fd);

  if (!CHAR_CHECKINDEX(charaindex))
    return;

  OnlineShop_ShowList(fd, charaindex, type, page);
}

void lssproto_VIP_SHOP_buy_recv(int fd, int type, int page, int id, int num) {
  int charaindex = CONNECT_getCharaindex(fd);

  if (!CHAR_CHECKINDEX(charaindex))
    return;

  OnlineShop_Buy(fd, charaindex, type, page, id, num);
}
#endif

void lssproto_SaMenu_recv(int fd, int index1, int index2, int index3) {
  int charaindex;
  int i, nums = 0;
  CHECKFDANDTIME;

  charaindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charaindex))
    return;

  //ʥ˭������??��������
  int result = -1;
  int x, y;
  OBJECT objecttm;
  int found = FALSE;
  int toindex;
  CHAR_getCoordinationDir(CHAR_getInt(charaindex, CHAR_DIR),
                          CHAR_getInt(charaindex, CHAR_X),
                          CHAR_getInt(charaindex, CHAR_Y), 1, &x, &y);
  for (objecttm = MAP_getTopObj(CHAR_getInt(charaindex, CHAR_FLOOR), x, y);
       objecttm; objecttm = NEXT_OBJECT(objecttm)) {
    int objindex = GET_OBJINDEX(objecttm);
    if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
      continue;
    toindex = OBJECT_getIndex(objindex);
    found = TRUE;
    break;
  }
  if (found == FALSE)
    toindex = -1;
  //ʥ˭������??��������?��

  FreeSaMenu(charaindex, toindex, index1, index2, index3);

  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
}

void lssproto_NewCharDelete_recv(int fd, char *charname, char *passwd) {
  char cdkey[CDKEYLEN];
  int fdid;

  if (CONNECT_isCLI(fd) == FALSE)
    return;
  if (CONNECT_isNOTLOGIN(fd) == FALSE) {
    lssproto_CharDelete_send(fd, FAILED, "Already Logged in\n");
    return;
  }
  if (strlen(passwd) < 6) {
    lssproto_CharDelete_send(fd, FAILED, "보안 코드가 안전하지 않아 삭제할 수 없습니다!");
    return;
  }
  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));
  fdid = CONNECT_getFdid(fd);
  if (strcmp(sasql_ItemPetLocked(cdkey, passwd), "unknown message") != 0) {
    lssproto_CharDelete_send(fd, FAILED, "unknown message");
    return;
  }
  FreeCharDelete(fd, fdid, cdkey);
  saacproto_ACCharDelete_send(acfd, cdkey, passwd, charname, "", fdid);

  {
    char buff[512];
    char escapebuf[1024];
    snprintf(buff, sizeof(buff), "%s_%s", cdkey, charname);
    makeEscapeString(buff, escapebuf, sizeof(escapebuf));
    saacproto_DBDeleteEntryInt_send(acfd, DB_DUELPOINT, escapebuf, fdid, 0);
    saacproto_DBDeleteEntryString_send(acfd, DB_ADDRESSBOOK, escapebuf, fdid,
                                       0);
  }
  saacproto_Broadcast_send(acfd, cdkey, charname, "chardelete", 0);
  CONNECT_setState(fd, WHILECHARDELETE);
}

void lssproto_MAC_recv(int fd, int flg) {
  if (flg == 213) {
    int charaindex = CONNECT_getCharaindex(fd);
    if (!CHAR_CHECKINDEX(charaindex))
      return;
    CHAR_logout(charaindex, TRUE);
    CONNECT_setCloseRequest(fd, 1);
  }
}

#ifdef _ALCHEMIST
void lssproto_ALCHEPLUS_recv(int fd) {
  int charindex = -1;
  int i;
  char sendbuf[1024];
  char token[64];

  charindex = CONNECT_getCharaindex(fd);
  if (!CHAR_CHECKINDEX(charindex))
    return;

  sendbuf[0] = '\0';
  for (i = 0; i < 25; i++) {
    sprintf(token, "%d|", CHAR_getInt(charindex, CHAR_ITEMCHECK01 + i));
    strcat(sendbuf, token);
  }
  lssproto_ALCHEPLUS_send(fd, sendbuf);
}
#endif

#ifdef _UP_ON_OFF
void lssproto_expup_recv(int fd) {
  int charaindex;
  char message[256];
  CHECKFDANDTIME;

  charaindex = CONNECT_getCharaindex(fd);

  if (CHAR_getInt(charaindex, CHAR_ADDEXPTIME) > 0 &&
      CHAR_getInt(charaindex, CHAR_ONOFF01) == 1) {
    int sec = 0, min = 0, hour = 0;
    sec = CHAR_getInt(charaindex, CHAR_ADDEXPTIME) -
          (time(NULL) - CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ONOFFTIME));
    CHAR_setInt(charaindex, CHAR_ADDEXPTIME, sec);

    if (sec == 0) {
      hour = 0;
      min = 0;
      sec = 0;
    }
    if (sec >= 60 * 60) {
      hour = sec / (60 * 60);
      sec = sec % (60 * 60);
    }
    if (sec >= 60) {
      min = sec / 60;
      sec = sec % 60;
    }

    sprintf(message, "����ġ ������ ����� �����մϴ�. [%02d�ð� %02d��]", hour,
            min);
    CHAR_talkToCli(charaindex, -1, message, CHAR_COLORWHITE);
    CHAR_setInt(charaindex, CHAR_ONOFF01, 0);
    CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ONOFFTIME, -1);
  } else if (CHAR_getInt(charaindex, CHAR_ADDEXPTIME) > 0 &&
             CHAR_getInt(charaindex, CHAR_ONOFF01) == 0) {
    int sec = 0, min = 0, hour = 0;
    sec = CHAR_getInt(charaindex, CHAR_ADDEXPTIME);
    if (sec == 0) {
      hour = 0;
      min = 0;
      sec = 0;
    }
    if (sec >= 60 * 60) {
      hour = sec / (60 * 60);
      sec = sec % (60 * 60);
    }
    if (sec >= 60) {
      min = sec / 60;
      sec = sec % 60;
    }

    sprintf(message, "����ġ ������ ����� �����մϴ�. [%02d�ð� %02d��]", hour,
            min);
    CHAR_talkToCli(charaindex, -1, message, CHAR_COLORYELLOW);
    CHAR_setInt(charaindex, CHAR_ONOFF01, 1);
    CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ONOFFTIME, time(NULL));
  } else {
    CHAR_talkToCli(charaindex, -1, "unknown message",
                   CHAR_COLORRED);
  }
  return;
}
#endif
#ifdef _ITEM_NOENEMY
void lssproto_noenemy_recv(int fd) {
  int charaindex;
  char message[256];
  CHECKFDANDTIME;

  charaindex = CONNECT_getCharaindex(fd);
  int floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  if (floor == 120 || floor == 201 || floor == 1000 || floor == 1006 ||
      floor == 1032 || floor == 1100 || floor == 1200 || floor == 1300 ||
      floor == 1400 || floor == 1500 || floor == 2000 || floor == 2006 ||
      floor == 3000 || floor == 3006 || floor == 3100 || floor == 3200 ||
      floor == 3300 || floor == 3400 || floor == 4000 || floor == 4006 ||
      floor == 5000 || floor == 5005 || floor == 5100 || floor == 5106) {
    CHAR_talkToCli(charaindex, -1, "unknown message",
                   CHAR_COLORRED);
    return;
  } else {

    if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME) > 0 &&
        CHAR_getInt(charaindex, CHAR_ONOFF02) == 1) {
      // CHAR_setWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME,
      // (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME)-60));

      char msg[256];
      int hour, min, sec;
      sec = CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME);

      min = sec / 60;
      hour = min / 60;
      sec = sec % 60;
      min = min % 60;

      sprintf(message, "unknown message [%02d�ð� %02d��]", hour, min);
      sprintf(msg, "unknown message");
      CHAR_talkToCli(charaindex, -1, message, CHAR_COLORWHITE);
      CHAR_talkToCli(index, -1, msg, CHAR_COLORRED);
      CHAR_setInt(charaindex, CHAR_ONOFF02, 0);
      // CHAR_setWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME,
      // CHAR_getInt(charaindex, CHAR_NOENEMYONOFF));
      clearEqNoenemy(CHAR_getWorkInt(charaindex, CHAR_WORKFD));
    } else if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME) > 0 &&
               CHAR_getInt(charaindex, CHAR_ONOFF02) == 0) {
      int hour, min, sec;
      sec = CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME);

      min = sec / 60;
      hour = min / 60;
      sec = sec % 60;
      min = min % 60;

      sprintf(message, "unknown message. [%02d�ð� %02d��]", hour, min);
      CHAR_talkToCli(charaindex, -1, message, CHAR_COLORYELLOW);
      CHAR_setInt(charaindex, CHAR_ONOFF02, 1);
      // CHAR_setWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME, time(NULL));
      setEqNoenemy(CHAR_getWorkInt(charaindex, CHAR_WORKFD), 200);
      // CHAR_setInt(charaindex, CHAR_NOENEMYONOFF, CHAR_getWorkInt(charaindex,
      // CHAR_WORKITEM_NOENEMYONTIME));

      /*char test2[256];
      sprintf(test2, "workint:%d", CHAR_getWorkInt(charaindex,
      CHAR_WORKITEM_NOENEMYONTIME)); CHAR_talkToCli(charaindex, -1, test2,
      CHAR_COLORWHITE);*/
    } else {
      CHAR_talkToCli(charaindex, -1, "unknown message",
                     CHAR_COLORRED);
    }
    return;
  }
}
#endif
#ifdef _CHAR_PARTAUTO
void lssproto_partyauto_recv(int fd) {
  int charaindex;
  char token[256];
  int r;
  int playernum = CHAR_getCharNum();
  CHECKFDANDTIME;

  charaindex = CONNECT_getCharaindex(fd);

  for (r = 0; r < playernum; r++) {
    if (CHAR_getCharUse(r) != FALSE) {
      if (CHAR_getInt(r, CHAR_FLOOR) == 777) {
        sprintf(token, "%s", CHAR_getChar(r, CHAR_NAME));
        // TODO: fix
        if (strcmp(token, "��Ī") == 0)
          break;
      }
    }
  }
  NPC_PartyAutoTalked(r, charaindex, token, 0);
}
#endif
#ifdef _CHAR_INFOSTORAGE
void lssproto_petshopcall_recv(int fd) {
  int charaindex;
  char token[256];
  int r;
  int playernum = CHAR_getCharNum();
  CHECKFDANDTIME;
  charaindex = CONNECT_getCharaindex(fd);

  for (r = 0; r < playernum; r++) {
    if (CHAR_getCharUse(r) != FALSE) {
      if (CHAR_getInt(r, CHAR_FLOOR) == 777) {
        sprintf(token, "%s", CHAR_getChar(r, CHAR_NAME));
        // TODO: fix
        if (strcmp(token, "��Ʈâ��") == 0)
          break;
      }
    }
  }
  NPC_PetShopTalked(r, charaindex, "petshop", 0);
  return;
}
void lssproto_poolitemcall_recv(int fd) {
  int charaindex;
  char token[256];
  int r;
  int playernum = CHAR_getCharNum();
  CHECKFDANDTIME;
  charaindex = CONNECT_getCharaindex(fd);

  for (r = 0; r < playernum; r++) {
    if (CHAR_getCharUse(r) != FALSE) {
      if (CHAR_getInt(r, CHAR_FLOOR) == 777) {
        sprintf(token, "%s", CHAR_getChar(r, CHAR_NAME));
        // TODO: fix
        if (strcmp(token, "â������") == 0)
          break;
      }
    }
  }
  NPC_PoolItemShopTalked(r, charaindex, "poolitemshop", 0);
  return;
}
#endif

int MAMMOTH_REWARDS[][2] = {{10, 46028}, //���������۹�ȣ
                            {15, 1260},  {20, 60015}, {25, 57626},  {30, 67012},
                            {45, 60016}, {70, 60023}, {100, 40054}, {-1, -1}};

void lssproto_MAMMOTH_recv(int fd, int couponindex) {
  print("\nlssproto_MAMMOTH_recv");
  CHECKFDANDTIME;
  const int COUPON_ITEM_ID = 40050;

  int charaindex = CONNECT_getCharaindex(fd);
  int chance = RAND(1, 100);
  int i;
  int winid = -1;
  int itemindex;
  int emptyslot;

  char chat_buffer[256];

  if (CHAR_findEmptyItemBox(charaindex) == -1) {
    print("\nbag full");
    return;
  }

  if (CHAR_CHECKITEMINDEX(charaindex, couponindex) == FALSE) {
    print("\nHACK");
    return;
  }

  itemindex = CHAR_getItemIndex(charaindex, couponindex);

  if (ITEM_CHECKINDEX(itemindex) == FALSE) {
    print("\nHACK");
    return;
  }

  if (ITEM_getInt(itemindex, ITEM_ID) != COUPON_ITEM_ID) {
    print("\nHACK");
    return;
  }

  for (i = 0; i < arraysizeof(MAMMOTH_REWARDS); i++) {
    if (MAMMOTH_REWARDS[i][0] >= chance && MAMMOTH_REWARDS[i][0] != -1) {
      winid = MAMMOTH_REWARDS[i][1];
      break;
    }
  }

  if (winid == -1) {
    print("\nNO WIN");
    return;
  }

  itemindex = ITEM_makeItemAndRegist(winid);

  if (ITEM_CHECKINDEX(itemindex) == FALSE) {
    print("\nITEM DONT EXIST");
    return;
  }

  emptyslot = CHAR_addItemSpecificItemIndex(charaindex, itemindex);
  if (emptyslot < 0 || emptyslot >= CHAR_getMaxInventory(charaindex)) {
    ITEM_endExistItemsOne(itemindex);
    print("\n bag full error!!");
    return;
  }

  sprintf(chat_buffer, "�����մϴ� %s ��(��) ȹ���Ͽ����ϴ�",
          ITEM_getChar(itemindex, ITEM_NAME));
  CHAR_talkToCli(charaindex, -1, chat_buffer, CHAR_COLORYELLOW);

  CHAR_sendItemDataOne(charaindex, chat_buffer);

  CHAR_DelItem(charaindex, couponindex);

  lssproto_MAMMOTH_send(fd, emptyslot);
}

void lssproto_GET_PET_RANK_recv(int fd, int enemybaseid) {
  CHECKFDANDTIME;
  int charaindex;
  char *Packet;

  charaindex = CONNECT_getCharaindex(fd);

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;

  Packet = SQL_GetTopPetRank(enemybaseid);
  if (Packet != "\0") {
    lssproto_PET_RANK_send(fd, Packet);
  }
}