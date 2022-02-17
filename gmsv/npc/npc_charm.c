#include "npc_charm.h"
#include "char.h"
#include "char_base.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "object.h"
#include "version.h"
#include <string.h>

//    医毛  仁煌遥挚反

//伊矛伙←        ←  蜇箕及    /

//荚汊汹反｛ㄤ匹允［
/*
#define RATE  4		//伊□玄＂
#define CHARMHEAL 5 //    及荚汊汹
#define WARU	3	//    毛喃月袄

*/

#define RATE 10     //伊□玄＂
#define CHARMHEAL 5 //    及荚汊汹
#define WARU 3      //    毛喃月袄

static void NPC_Charm_selectWindow(int meindex, int toindex, int num);
int NPC_CharmCost(int meindex, int talker);
void NPC_CharmUp(int meindex, int talker);

/*********************************
 * 赓渝质
 *********************************/
BOOL NPC_CharmInit(int meindex) {
  /*--平乓仿及正奶皿毛涩烂--*/
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TYPECHARM);
  return TRUE;
}

/*********************************
 *   仄井仃日木凶凛及质
 *********************************/
void NPC_CharmTalked(int meindex, int talkerindex, char *szMes, int color) {

  /* 皿伊奶乩□卞覆仄化分仃  杀允月 */
  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    return;
  }

  /*--  及蟆卞中月井升丹井＂--*/
  if (NPC_Util_isFaceToFace(meindex, talkerindex, 2) == FALSE) {
    /* ㄠ弘伉永玉动  及心 */
    if (NPC_Util_isFaceToChara(talkerindex, meindex, 1) == FALSE)
      return;
  }

  NPC_Charm_selectWindow(meindex, talkerindex, 0);
}

/*
 * 备质  卞坌仃月
 */
static void NPC_Charm_selectWindow(int meindex, int toindex, int num) {

  char token[1024];
  char escapedname[1024];
  int fd = getfdFromCharaIndex(toindex);
  int buttontype = 0;
  int windowtype = 0;
  int windowno = 0;
  int cost = 0;
  int chartype;

  /*--它奶件玉它正奶皿丢永本□斥互云云中及匹燮卞涩烂--*/
  windowtype = WINDOW_MESSAGETYPE_MESSAGE;

  switch (num) {
  case 0:
    /*--蓟      --*/
    sprintf(token, "4\n 　　　       固侩包府荤     "
                   "\n\n 　公郊老捞具，概仿阑 棵府妨备？"
                   "\n "
                   "\n\n　　　　《　概仿雀汗　》 "
                   "\n\n　　　　《 唱啊扁 》 ");

    buttontype = WINDOW_BUTTONTYPE_NONE;
    windowtype = WINDOW_MESSAGETYPE_SELECT;
    windowno = CHAR_WINDOWTYPE_CHARM_START;
    break;

  case 1:
    cost = NPC_CharmCost(meindex, toindex);
    if (cost == -1) {
      sprintf(token, "　　　　         固侩包府荤      "
                     "\n\n　　　　概仿阑 棵副 鞘夸啊 绝绢"
                     "\n\n　　　肯傈 岗乐绰措!!?");
      buttontype = WINDOW_BUTTONTYPE_OK;
    } else {
      sprintf(token,
              "　　　　         固侩包府荤       "
              "\n　概仿阑 棵副波具??"
              "\n\n 概仿阑 棵府妨搁 胶沛捞 鞘夸秦"
              "\n　　　　陛咀:%6dstone捞具！"
              "\n\n　　　弊贰档 棵副波具？",
              cost);
      buttontype = WINDOW_BUTTONTYPE_YESNO;
    }
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_CHARM_END;

    break;

  case 2:
    cost = NPC_CharmCost(meindex, toindex);
    chartype = CHAR_getInt(toindex, CHAR_IMAGETYPE);

    if (cost > CHAR_getInt(toindex, CHAR_GOLD)) {
      sprintf(token, "　　　　         固侩包府荤      "
                     "\n\n　　 角菩!家瘤陛 何练！"
                     "\n　　    概仿阑 棵府妨搁 捣捞 鞘夸秦！");

    } else {
      NPC_CharmUp(meindex, toindex);

      /*--平乓仿及正奶皿卞方匀化丢永本□斥毛  尹化心凶--*/
      switch (chartype) {
      case CHAR_IMAGETYPE_GIRL:
        sprintf(token, "　　　　         固侩包府荤       "
                       "\n\n 　概仿 5啊 刘啊登菌嚼聪促！"
                       "\n\n　　　肯傈 捞慧措！");

        break;
      case CHAR_IMAGETYPE_BOY:
        sprintf(token, "　　　　         固侩包府荤     "
                       "\n\n 　概仿 5啊 刘啊登菌嚼聪促！"
                       "\n\n　　肯傈 岗廉焊咯 ！");
        break;

      case CHAR_IMAGETYPE_CHILDBOY:
      case CHAR_IMAGETYPE_CHILDGIRL:
        sprintf(token, "　　　　         固侩包府荤      "
                       "\n\n 　概仿 5啊 刘啊登菌嚼聪促！"
                       "\n\n　　  虐啊 磊耳备唱！");
        break;

      case CHAR_IMAGETYPE_MAN:
        sprintf(token, "　　　　         固侩包府荤       "
                       "\n\n 　概仿 5啊 刘啊登菌嚼聪促！"
                       "\n\n 　　　岗乐绢夸！");
        break;

      case CHAR_IMAGETYPE_WOMAN:
        sprintf(token, "　　　　         固侩包府荤      "
                       "\n\n 　概仿 5啊 刘啊登菌嚼聪促！"
                       "\n\n    任纠 俐绢瘤继匙夸！");
        break;
      }
    }

    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_CHARM_END;
    break;
  }

  makeEscapeString(token, escapedname, sizeof(escapedname));
  /*-仇仇匹霜耨允月--*/
  lssproto_WN_send(fd, windowtype, buttontype, windowno,
                   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), escapedname);
}

/*-----------------------------------------
弁仿奶失件玄井日忒匀化五凶凛卞裟太请今木月［
-------------------------------------------*/
void NPC_CharmWindowTalked(int meindex, int talkerindex, int seqno, int select,
                           char *data) {
  if (NPC_Util_CharDistance(talkerindex, meindex) > 2)
    return;

  switch (seqno) {
  case CHAR_WINDOWTYPE_CHARM_START:
    if (atoi(data) == 2) {
      NPC_Charm_selectWindow(meindex, talkerindex, 1);
    }
    break;

  case CHAR_WINDOWTYPE_CHARM_END:
    if (select == WINDOW_BUTTONTYPE_YES) {
      NPC_Charm_selectWindow(meindex, talkerindex, 2);
    }
    break;
  }
}

/*--        --*/
void NPC_CharmUp(int meindex, int talker) {
  int cost;
  int i;
  int petindex;
  char petsend[64];

  /*--云嗯毛蛹日仄引仄斤丹--*/
  cost = NPC_CharmCost(meindex, talker);
  CHAR_setInt(talker, CHAR_GOLD, CHAR_getInt(talker, CHAR_GOLD) - cost);
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_GOLD);

  /*--    互ㄠㄟㄟ动晓卞卅月桦宁反雄娄卞ㄠㄟㄟ卞允月--*/
  if (CHAR_getInt(talker, CHAR_CHARM) + CHARMHEAL >= 100) {
    CHAR_setInt(talker, CHAR_CHARM, 100);
  } else {
    /*--    毛本永玄--*/
    CHAR_setInt(talker, CHAR_CHARM,
                (CHAR_getInt(talker, CHAR_CHARM) + CHARMHEAL));
  }

  /*--旦  □正旦及凳蕙--*/
  CHAR_complianceParameter(talker);
  CHAR_send_P_StatusString(talker, CHAR_P_STRING_CHARM);

  /*--矢永玄及由仿丢□正毛凳蕙--*/
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    petindex = CHAR_getCharPet(talker, i);

    if (petindex == -1)
      continue;

    /*  平乓仿及    民尼永弁    */
    if (!CHAR_CHECKINDEX(talker))
      continue;

    /*--由仿丢□正譬帮--*/
    CHAR_complianceParameter(petindex);
    sprintf(petsend, "K%d", i);
    CHAR_sendStatusString(talker, petsend);
  }
}

/*--云嗯及煌遥--*/
int NPC_CharmCost(int meindex, int talker) {
  int cost;
  int level;
  int charm;
  int trans;

  level = CHAR_getInt(talker, CHAR_LV);
  charm = CHAR_getInt(talker, CHAR_CHARM);
  trans = CHAR_getInt(talker, CHAR_TRANSMIGRATION);

  if (charm >= 100)
    return -1;

  if (charm <= 1)
    charm = WARU;

  /*-- 煌遥挚 --*/
  cost = level * RATE * (charm / WARU) * (trans + 1);

  return cost;
}
