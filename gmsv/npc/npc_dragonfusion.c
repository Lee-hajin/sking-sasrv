#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "chatmagic.h"
#include "enemy.h"
#include "handletime.h"
#include "log.h"
#include "lssproto_serv.h"
#include "npc_eventaction.h"
#include "npc_petfusion.h"
#include "npcutil.h"
#include "object.h"
#include "pet.h"
#include "readmap.h"
#include "saacproto_cli.h"
#include "version.h"
#include <string.h>

#ifdef _NB_DRAGON_FUSION

enum {
  WINDOW_START = 10,
  WINDOW_SELECT,
  WINDOW_FUSION,
  WINDOW_WARP, //跳跃
  WINDOW_END,  //结束

  WINDOW_TALKSTART = 20,
  WINDOW_TALKSELECT,
  WINDOW_RETURN,
  WINDOW_TALKEND,
};
enum {
  NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
  /*
          NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
          NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,
          NPC_WORK_MODE = CHAR_NPCWORKINT5,
          NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6,
          NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
          NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
          NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
          NPC_WORK_SEFLG = CHAR_NPCWORKINT10,
  */
};

static void NPC_DragonFusion_selectWindow(int meindex, int toindex, int num,
                                          int select);
BOOL NPC_DoDragonFusion(int meindex, int toindex, char *data, int sizes);
void getDragonFusionBase(int toindex, int *pindex, int *work);
int getDragonFusionPetlist(int toindex, int *pindex, char *data);
BOOL DRAGONFUSION_DelPet(int toindex, int *pindex);
int getDragonFusionTable(int toindex, int *pindex);

BOOL NPC_DragonFusionManInit(int meindex) {
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL) {
    print("err NPC_DragonFusionManInit: GetArgStrErr!!");
    return FALSE;
  }
#ifdef _PET_FUSION
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_PETFUSIONMAN);
#endif
  // CHAR_setInt( meindex, CHAR_LOOPINTERVAL, MAKEPAIR_STANDBY);
  // CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
  // CHAR_setWorkInt( meindex, NPC_WORK_PAIRTEAMS, 0);
  return TRUE;
}
void NPC_DragonFusionManTalked(int meindex, int talkerindex, char *msg,
                               int color) {
  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return;
  if (NPC_Util_isFaceToFace(meindex, talkerindex, 2) == FALSE) {
    if (NPC_Util_isFaceToChara(talkerindex, meindex, 1) == FALSE)
      return;
  }
#ifdef _ITEM_CHECKWARES
  if (CHAR_CheckInItemForWares(talkerindex, 0) == FALSE) {
    CHAR_talkToCli(talkerindex, meindex, "无法服务携带货物的人！",
                   CHAR_COLORYELLOW);
    return;
  }
#endif

  CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
  NPC_DragonFusion_selectWindow(meindex, talkerindex, WINDOW_START, 0);
}

char nb_npcdat1[2048];
char nb_npcdat2[2048];

static void NPC_DragonFusion_selectWindow(int meindex, int toindex, int num,
                                          int select) {
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  char token[256];
  int buttontype = 0, windowtype = 0, windowno = 0;
  int fd = getfdFromCharaIndex(toindex);
  windowtype = WINDOW_MESSAGETYPE_MESSAGE;
  if (Action_PartyCheck(meindex, toindex) == FALSE) {
    CHAR_talkToCli(toindex, meindex, "请一个一个来！", CHAR_COLORYELLOW);
    return;
  }
  memset(npcarg, 0, sizeof(npcarg));
  if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL) {
    print("err NPC_MakePairMan: GetArgStrErr!!");
    return;
  }
  memset(token, 0, sizeof(token));

  switch (num) {
  case WINDOW_START:
    if (NPC_Util_GetStrFromStrWithDelim(npcarg, "StartMsg", token,
                                        sizeof(token)) == NULL) {
      return;
    }

    //新增自定义人龙编号
    if (NPC_Util_GetStrFromStrWithDelim(npcarg, "DelPetData", nb_npcdat1,
                                        sizeof(nb_npcdat1)) == NULL) {
      sprintf(nb_npcdat1, "91,92,93,94,95");
    }
    if (NPC_Util_GetStrFromStrWithDelim(npcarg, "GelPetData", nb_npcdat2,
                                        sizeof(nb_npcdat2)) == NULL) {
      sprintf(nb_npcdat2, "2799,2800,2803,2801,2802");
    }
    //新增自定义人龙编号
    buttontype = WINDOW_BUTTONTYPE_YESNO;
    windowno = WINDOW_TALKSTART;
    break;
  case WINDOW_SELECT:
    if (ActionNpc_CheckFree(meindex, toindex, npcarg) == FALSE) {
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowno = WINDOW_TALKEND;
    } else {
      if (NPC_Util_GetStrFromStrWithDelim(npcarg, "SelectMsg", token,
                                          sizeof(token)) == NULL) {
        return;
      }
      windowtype = WINDOWS_MESSAGETYPE_DRAGONFUSION;
      buttontype = WINDOW_BUTTONTYPE_YESNO;
      windowno = WINDOW_TALKSELECT;
    }
    break;
  case WINDOW_FUSION:
    //融合处理
    break;
  case WINDOW_WARP:
    break;
  case WINDOW_END:
    break;
  }
  CHAR_setWorkInt(toindex, CHAR_WORKSHOPRELEVANT, windowno);
  lssproto_WN_send(fd, windowtype, buttontype, windowno,
                   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_DragonFusionManWindowTalked(int meindex, int talkerindex, int seqno,
                                     int select, char *data) {
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return;
  if (select == WINDOW_BUTTONTYPE_NO || select == WINDOW_BUTTONTYPE_CANCEL) {
    CHAR_setWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT, 0);
    return;
  }
  if (CHAR_getWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT) != seqno)
    return;
  memset(npcarg, 0, sizeof(npcarg));
  if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL) {
    print("err NPC_MakePairMan: GetArgStrErr!!");
    return;
  }
  if (CHAR_getWorkInt(talkerindex, CHAR_WORKSHOPRELEVANT) != seqno)
    return;

  switch (seqno) {
  case WINDOW_TALKSTART:
    NPC_DragonFusion_selectWindow(meindex, talkerindex, WINDOW_SELECT, 0);
    break;
  case WINDOW_TALKSELECT: {
    if (NPC_DoDragonFusion(meindex, talkerindex, data, sizeof(data)) == FALSE)
      return;
    return;
    NPC_DragonFusion_selectWindow(meindex, talkerindex, WINDOW_FUSION, 0);
  } break;
  case WINDOW_RETURN:
    break;
  case WINDOW_TALKEND:
    break;
  }
}

void NPC_DragonFusionManLoop(int meindex) {}

BOOL NPC_DoDragonFusion(int meindex, int toindex, char *data, int sizes) {
  char buf[256];
  int i;
  int work[4], skill[7];
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  int pindex[5] = {-1, -1, -1, -1, -1};
  int newpetid, newpetindex;
  int enemynum, LevelUpPoint;

  memset(buf, 0, sizeof(buf));
  sprintf(buf, "%s", data);
  if (!CHAR_CHECKINDEX(meindex))
    return FALSE;
  if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL) {
    print("err NPC_MakePairMan: GetArgStrErr!!");
    return FALSE;
  }

  // 取得融宠列表
  if (getDragonFusionPetlist(toindex, pindex, data) == FALSE) {
    CHAR_talkToCli(toindex, -1, "宠物数目不足。", CHAR_COLORYELLOW);
    return FALSE;
  }

  // 检查融宠列表 取得新融宠ID
  newpetid = getDragonFusionTable(toindex, pindex);
  if (newpetid == -1) {
    CHAR_talkToCli(toindex, -1, "宠物进化失败！", CHAR_COLORYELLOW);
    return FALSE;
  }

  // 计算融宠新成长值
  // getDragonFusionBase( toindex, pindex, work);

  // 取得宠技
  if (skill != NULL) {
    for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {
      skill[i] = CHAR_getPetSkill(pindex[0], i);
    }
  }

  // 制造新宠
  enemynum = ENEMY_getEnemyNum();
  for (i = 0; i < enemynum; i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == newpetid) { // ENEMYNO
      break;
    }
  }
  if (i == enemynum)
    return FALSE;
  newpetid = i; // ENEMYTEMPNO

  // 暂时删除旧宠
  for (i = 0; i < CHAR_MAXPETHAVE; i++)
    CHAR_setCharPet(toindex, i, -1);

  newpetindex = ENEMY_createPetFromEnemyIndex(toindex, newpetid);
  if (!CHAR_CHECKINDEX(newpetindex)) {

    // 失败的话归还旧宠
    for (i = 0; i < CHAR_MAXPETHAVE; i++)
      CHAR_setCharPet(toindex, i, pindex[i]);
    CHAR_talkToCli(toindex, -1, "宠物进化失败！", CHAR_COLORYELLOW);
    return FALSE;
  } else {
    // 成功的话永久删除旧宠
    DRAGONFUSION_DelPet(toindex, pindex);
  }
  /*
  LevelUpPoint = ( work[0] << 24 )
          + (work[1] << 16)
          + (work[2] << 8 )
          + (work[3] << 0 );*/
  /*
  int levelup = CHAR_getInt(newpetindex, CHAR_ALLOCPOINT);//取得成长值
  int base[5][4];

  base[i][0] = ((levelup>>24) & 0xFF);
  base[i][1] = ((levelup>>16) & 0xFF);
  base[i][2] = ((levelup>> 8) & 0xFF);
  base[i][3] = ((levelup>> 0) & 0xFF);

  CHAR_setInt( newpetindex, CHAR_ALLOCPOINT, LevelUpPoint);
  */
  CHAR_setInt(newpetindex, CHAR_SLOT, 7); //修改7技
#ifdef _NB_DragonFunsionId
  CHAR_setInt(newpetindex, CHAR_TRANSMIGRATION, getDragonTrans());
#endif
  for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {
    CHAR_setPetSkill(newpetindex, i, skill[i]);
  }

  CHAR_complianceParameter(newpetindex);

  CHAR_setInt(newpetindex, CHAR_HP,
              CHAR_getWorkInt(newpetindex, CHAR_WORKMAXHP));

  if (CHAR_CHECKINDEX(toindex)) {
    CHAR_setWorkInt(newpetindex, CHAR_WORKPLAYERINDEX, toindex);
    CHAR_setChar(newpetindex, CHAR_OWNERCDKEY,
                 CHAR_getChar(toindex, CHAR_CDKEY));
    CHAR_setChar(newpetindex, CHAR_OWNERCHARANAME,
                 CHAR_getChar(toindex, CHAR_NAME));
  }
  CHAR_setInt(newpetindex, CHAR_WHICHTYPE, CHAR_TYPEPET);
  // CHAR_setInt ( newpetindex, CHAR_TRANSMIGRATION, 2);
  // CHAR_setInt ( newpetindex, CHAR_FUSIONBEIT, 1);

  // 更新全部宠物
  for (i = 0; i < 5; i++) {
    char szPet[256];

    snprintf(szPet, sizeof(szPet), "K%d", i);
    CHAR_sendStatusString(toindex, szPet);
#ifdef _PET_ITEM
    snprintf(szPet, sizeof(szPet), "B%d", i);
    CHAR_sendStatusString(toindex, szPet);
#endif
  }

  CHAR_talkToCli(toindex, -1, "人龙进化完成！", CHAR_COLORWHITE);

  return TRUE;
}

int getDragonFusionPetlist(int toindex, int *pindex, char *data) {
  int i, petindex;
  char buf1[256];
  int trans_flag = 0;

  for (i = 0; i < 5; i++) {

    if (getStringFromIndexWithDelim(data, "|", i + 1, buf1, sizeof(buf1)) ==
        FALSE) {
      return FALSE;
    }
    pindex[i] = atoi(buf1) - 1;

    if (pindex[i] < 0)
      return FALSE;
    petindex = CHAR_getCharPet(toindex, pindex[i]);
    if (!CHAR_CHECKINDEX(petindex))
      return FALSE;
    // 最多只能有一只二转宠物
    /*
    if( CHAR_getInt( petindex, CHAR_TRANSMIGRATION ) > 1 ) {
            if( trans_flag <= 0 ) {
                    trans_flag++;
            }else {
                    CHAR_talkToCli( toindex, -1, "此组合无法进化。",
    CHAR_COLORYELLOW); return FALSE;
            }
    }*/
    // 检查是否为融合宠
    if (CHAR_getInt(toindex, CHAR_FUSIONBEIT) == 1
        /*&& CHAR_getInt( toindex, CHAR_FUSIONRAISE) > 0 */) {
      CHAR_talkToCli(toindex, -1, "融合宠物不能进化。", CHAR_COLORYELLOW);
      return FALSE;
    }
    pindex[i] = petindex;
  }
  return TRUE;
}

int getDragonFusionTable(int toindex, int *pindex) {
  int i;
  int petid;
  //新增自定义人龙编号
  int delpet[5] = {91, 92, 93, 94, 95};
  int gelpet[5] = {5020, 5021, 5024, 5022, 5023};
  char buf1[256];
  char nobuf[256];

  int delpet2, gelpet2;

  for (i = 0; i < 5; i++) {
    if (getStringFromIndexWithDelim(nb_npcdat1, ",", i + 1, buf1,
                                    sizeof(buf1))) {
      delpet2 = atoi(buf1);
      if (delpet2 > 0) {
        delpet[i] = delpet2;
      }
    }
  }

  for (i = 0; i < 5; i++) {
    if (getStringFromIndexWithDelim(nb_npcdat2, ",", i + 1, buf1,
                                    sizeof(buf1))) {
      gelpet2 = atoi(buf1);
      if (gelpet2 > 0) {
        gelpet[i] = gelpet2;
      }
    }
  }

  //自定义人龙编号结束
  for (i = 0; i < 5; i++) {
    petid = CHAR_getInt(pindex[i], CHAR_PETID); // EMENYTEMPNO
    // print("\n宠物ID%d:%d-%d",i,delpet[i],gelpet[i]);
    if (petid != delpet[0] && petid != delpet[1] && petid != delpet[2] &&
        petid != delpet[3] && petid != delpet[4]) {
      sprintf(nobuf, "非人龙系宠物不能进化。");
      CHAR_talkToCli(toindex, -1, nobuf, CHAR_COLORYELLOW);
      return FALSE;
    }
    if (CHAR_getInt(pindex[i], CHAR_TRANSMIGRATION) < 2) {
      sprintf(nobuf, "进化的人龙必须达到2转77级以上");
      CHAR_talkToCli(toindex, -1, nobuf, CHAR_COLORYELLOW);
      return FALSE;
    }
    if (CHAR_getInt(pindex[i], CHAR_LV) < 77) {
      sprintf(nobuf, "进化的人龙必须达到2转77级以上");
      CHAR_talkToCli(toindex, -1, nobuf, CHAR_COLORYELLOW);
      return FALSE;
    }
  }
#ifdef _NB_DragonFunsionId
  if (getDragonFunsionRand() == 0) {
    for (i = 0; i < 5; i++) {
      if (CHAR_getInt(pindex[0], CHAR_PETID) == delpet[i]) {
        return getDragonFunsionId(i + 1);
      }
    }
  } else {
    int randid = RAND(1, 5);
    return getDragonFunsionId(randid);
  }
#else
  for (i = 0; i < 5; i++) {
    if (CHAR_getInt(pindex[0], CHAR_PETID) == delpet[i]) {
      return gelpet[i];
    }
  }
#endif
  return -1;
}

void getDragonFusionBase(int toindex, int *pindex, int *work) {
  int i;
  int plevel, levelup;
  int base[5][4];

  for (i = 0; i < 5; i++) {
    plevel = CHAR_getInt(pindex[i], CHAR_LV);
    levelup = CHAR_getInt(pindex[i], CHAR_ALLOCPOINT);
    if (plevel < 80) {
      base[i][0] = ((levelup >> 24) & 0xFF) * 0.8;
      base[i][1] = ((levelup >> 16) & 0xFF) * 0.8;
      base[i][2] = ((levelup >> 8) & 0xFF) * 0.8;
      base[i][3] = ((levelup >> 0) & 0xFF) * 0.8;
    } else {
      base[i][0] = ((levelup >> 24) & 0xFF);
      base[i][1] = ((levelup >> 16) & 0xFF);
      base[i][2] = ((levelup >> 8) & 0xFF);
      base[i][3] = ((levelup >> 0) & 0xFF);
    }

    print("\n 旧人龙%d成长率:%d,%d,%d,%d ", i + 1, base[i][0], base[i][1],
          base[i][2], base[i][3]);
  }
  work[0] = base[0][0] * 0.3 + base[1][0] * 0.7 + base[2][0] * 0.1 +
            base[3][0] * 0.1 + base[4][0] * 0.1;
  work[1] = base[0][1] * 0.3 + base[1][1] * 0.1 + base[2][1] * 0.55 +
            base[3][1] * 0.1 + base[4][1] * 0.1;
  work[2] = base[0][2] * 0.3 + base[1][2] * 0.1 + base[2][2] * 0.1 +
            base[3][2] * 0.6 + base[4][2] * 0.1;
  work[3] = base[0][3] * 0.3 + base[1][3] * 0.1 + base[2][3] * 0.1 +
            base[3][3] * 0.1 + base[4][3] * 0.6;

#define RAND(x, y)                                                             \
  ((x - 1) + 1 + (int)((double)(y - (x - 1)) * rand() / (RAND_MAX + 1.0)))
  work[0] += (RAND(0, 4) - 2);
  work[1] += (RAND(0, 4) - 2);
  work[2] += (RAND(0, 4) - 2);
  work[3] += (RAND(0, 4) - 2);

  for (i = 0; i < 15; i++) {
    int rnt = RAND(0, 3);
    if (rnt == 0)
      work[0]++;
    if (rnt == 1)
      work[1]++;
    if (rnt == 2)
      work[2]++;
    if (rnt == 3)
      work[3]++;
  }

  print("\n 新人龙成长率:%d,%d,%d,%d ", work[0], work[1], work[2], work[3]);
}

BOOL DRAGONFUSION_ReflashPet(int toindex, int *pindex) {
  int i;

  for (i = 0; i < 5; i++) {
    char szPet[256];

    if (!CHAR_CHECKINDEX(pindex[i]))
      continue;

    snprintf(szPet, sizeof(szPet), "K%d", i);
    CHAR_sendStatusString(toindex, szPet);
#ifdef _PET_ITEM
    snprintf(szPet, sizeof(szPet), "B%d", i);
    CHAR_sendStatusString(toindex, szPet);
#endif
  }

  return TRUE;
}

BOOL DRAGONFUSION_DelPet(int toindex, int *pindex) {
  int i;

  for (i = 0; i < 5; i++) {
    char msgbuf[256];

    if (!CHAR_CHECKINDEX(pindex[i]))
      continue;

    snprintf(msgbuf, sizeof(msgbuf), "交出%s。",
             CHAR_getChar(pindex[i], CHAR_NAME));
    CHAR_talkToCli(toindex, -1, msgbuf, CHAR_COLORYELLOW);

    LogPet(CHAR_getChar(toindex, CHAR_NAME), CHAR_getChar(toindex, CHAR_CDKEY),
           CHAR_getChar(pindex[i], CHAR_NAME), CHAR_getInt(pindex[i], CHAR_LV),
           "人龙进化删除", CHAR_getInt(toindex, CHAR_FLOOR),
           CHAR_getInt(toindex, CHAR_X), CHAR_getInt(toindex, CHAR_Y),
           CHAR_getChar(pindex[i], CHAR_UNIQUECODE), pindex[i], toindex);

    CHAR_endCharOneArray(pindex[i]);
  }

  return TRUE;
}

#endif
