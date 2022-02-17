#include "npc_petfusion.h"
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
#include "npcutil.h"
#include "object.h"
#include "pet.h"
#include "readmap.h"
#include "saacproto_cli.h"
#include "version.h"
#include <string.h>

#ifdef _NPC_FUSION

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

static void NPC_PetFusion_selectWindow(int meindex, int toindex, int num,
                                       int select);
BOOL NPC_DoPetFusion(int meindex, int toindex, char *data, int sizes);

BOOL NPC_PetFusionManInit(int meindex) {
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL) {
    print("err NPC_PetFusionManInit: GetArgStrErr!!");
    return FALSE;
  }
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_PETFUSIONMAN);
  // CHAR_setInt( meindex, CHAR_LOOPINTERVAL, MAKEPAIR_STANDBY);
  // CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
  // CHAR_setWorkInt( meindex, NPC_WORK_PAIRTEAMS, 0);
  return TRUE;
}
void NPC_PetFusionManTalked(int meindex, int talkerindex, char *msg,
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
  NPC_PetFusion_selectWindow(meindex, talkerindex, WINDOW_START, 0);
}

static void NPC_PetFusion_selectWindow(int meindex, int toindex, int num,
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
      windowtype = WINDOWS_MESSAGETYPE_PETFUSION;
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
void NPC_PetFusionManWindowTalked(int meindex, int talkerindex, int seqno,
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
    NPC_PetFusion_selectWindow(meindex, talkerindex, WINDOW_SELECT, 0);
    break;
  case WINDOW_TALKSELECT: {
    if (NPC_DoPetFusion(meindex, talkerindex, data, sizeof(data)) == FALSE)
      return;
    return;
    NPC_PetFusion_selectWindow(meindex, talkerindex, WINDOW_FUSION, 0);
  } break;
  case WINDOW_RETURN:
    break;
  case WINDOW_TALKEND:
    break;
  }
}

BOOL NPC_DoPetFusion(int meindex, int toindex, char *data, int sizes) {
  char buf[256];
  int i, flg, MainIndex = -1, Subindex1 = -1, Subindex2 = -1;
  int work[4], skill1[7], skill2[7];
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

  memset(buf, 0, sizeof(buf));
  sprintf(buf, "%s", data);
  if (!CHAR_CHECKINDEX(meindex))
    return FALSE;
  if (NPC_Util_GetArgStr(meindex, npcarg, sizeof(npcarg)) == NULL) {
    print("err NPC_MakePairMan: GetArgStrErr!!");
    return FALSE;
  }
//判定那种类型的融合，1:陆地，2：海洋
#ifdef _PET_FUSION_2
  int fusion_type = 1;
  char buf2[256];
  if (NPC_Util_GetStrFromStrWithDelim(npcarg, "FUSIONTYPE", buf2,
                                      sizeof(buf2)) == NULL) {
    // return FALSE;
    fusion_type = FUSIONTYPE_LAND;
  } else {
    fusion_type = atoi(buf2);
  }
  print(" FUSIONTYPE:%d ", fusion_type);
#endif
  for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {
    skill1[i] = -1;
    skill2[i] = -1;
  }

  if (PETFUSION_getIndexForChar(toindex, &MainIndex, &Subindex1, &Subindex2,
                                buf) == FALSE) {
    CHAR_talkToCli(toindex, -1, "宠物融合失败！[1]", CHAR_COLORYELLOW);
    return FALSE;
  }
  if ((flg = PETFUSION_FusionPetSub(toindex, Subindex1, Subindex2, work,
                                    skill2)) == 0) {
    CHAR_talkToCli(toindex, -1, "宠物融合失败！[2]", CHAR_COLORYELLOW);
    return FALSE;
  }
#ifdef _PET_ITEM
  char buf3[256];
  for (i = 0; i < CHAR_MAXPETITEMHAVE; i++) {
    if (CHAR_CHECKINDEX(MainIndex)) {
      if (CHAR_getItemIndex(MainIndex, i) != -1) {
        sprintf(buf3, "[%s]身上带有装备，不可参与融合！",
                CHAR_getChar(MainIndex, CHAR_NAME));
        CHAR_talkToCli(toindex, -1, buf3, CHAR_COLORYELLOW);
        return FALSE;
      }
    }
    if (CHAR_CHECKINDEX(Subindex1)) {
      if (CHAR_getItemIndex(Subindex1, i) != -1) {
        sprintf(buf3, "[%s]身上带有装备，不可参与融合！",
                CHAR_getChar(Subindex1, CHAR_NAME));
        CHAR_talkToCli(toindex, -1, buf3, CHAR_COLORYELLOW);
        return FALSE;
      }
    }
    if (CHAR_CHECKINDEX(Subindex2)) {
      if (CHAR_getItemIndex(Subindex2, i) != -1) {
        sprintf(buf3, "[%s]身上带有装备，不可参与融合！",
                CHAR_getChar(Subindex2, CHAR_NAME));
        CHAR_talkToCli(toindex, -1, buf3, CHAR_COLORYELLOW);
        return FALSE;
      }
    }
  }
#endif
  for (i = 0; i < 4; i++) {
    work[i] = work[i] / flg;
    work[i] = work[i] * 0.4;
  }

  if (PETFUSION_FusionPetMain(toindex, MainIndex, work, skill1) == FALSE) {
    CHAR_talkToCli(toindex, -1, "宠物融合失败！[3]", CHAR_COLORYELLOW);
    return FALSE;
  } else {
    int Eggindex = -1, petNo = 0, petID, newpetindex = -1;
    char buf1[256];
    int PetCode = -1;
    flg++;
#ifdef _PET_FUSION_2
    if ((PetCode = NPC_getFusionTableForBase(toindex, MainIndex, Subindex1,
                                             fusion_type)) < 0) {
#else
    if ((PetCode = NPC_getFusionTableForBase(toindex, MainIndex, Subindex1)) <
        0) {
#endif
      CHAR_talkToCli(toindex, -1, "宠物融合失败！[4]", CHAR_COLORYELLOW);
      return FALSE;
    }

#ifdef _PET_FUSION_LUA
#ifdef _PET_FUSION_2
    if (FreeCheckFusion(MainIndex, Subindex1, Subindex2, fusion_type) == 0)
#else
    if (FreeCheckFusion(MainIndex, Subindex1, Subindex2) == 0)
#endif
    {
      CHAR_talkToCli(toindex, -1, "宠物融合失败！[5]", CHAR_COLORYELLOW);
      return FALSE;
    }
#endif

    petNo = NPC_getPetArrayForNo(PetCode);

    if (PETFUSION_DelPet(toindex, MainIndex, Subindex1, Subindex2, flg) ==
        FALSE) {
      return FALSE;
    }
    if (NPC_Util_GetStrFromStrWithDelim(npcarg, "ADDEGGID", buf1,
                                        sizeof(buf1)) == NULL) {
      return FALSE;
    }
    petID = atoi(buf1);
    if ((Eggindex = PETFUSION_AddEgg(toindex, petID, PetCode)) < 0) {
      return FALSE;
    }
#ifdef _PET_FUSION_2
    newpetindex = PETFUSION_SetNewEgg(toindex, Eggindex, petNo, work, skill1,
                                      skill2, fusion_type);
#else
    newpetindex =
        PETFUSION_SetNewEgg(toindex, Eggindex, petNo, work, skill1, skill2);
#endif
    if (!CHAR_CHECKINDEX(newpetindex)) {
      print("ANDY err newpetindex=%d\n", newpetindex);
      return FALSE;
    }
#ifdef _PET_FUSION_2
    CHAR_setInt(newpetindex, CHAR_FUSIONTYPE, fusion_type);
#endif
    snprintf(buf1, sizeof(buf1), "W%d", petNo);
    CHAR_sendStatusString(toindex, buf1);
    CHAR_sendStatusString(toindex, "P");
  }

  return TRUE;
}

#endif
