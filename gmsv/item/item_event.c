#include "item_event.h"
#include "anim_tbl.h"
#include "battle.h"
#include "battle_event.h"
#include "battle_item.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "char_talk.h"
#include "chatmagic.h"
#include "common.h"
#include "configfile.h"
#include "encount.h"
#include "enemy.h"
#include "family.h"
#include "handletime.h"
#include "item.h"
#include "log.h"
#include "lssproto_serv.h"
#include "magic.h"
#include "net.h"
#include "npcutil.h"
#include "object.h"
#include "pet.h"
#include "readmap.h"
#include "saacproto_cli.h"
#include "util.h"
#include "version.h"
#include <ctype.h>  /* isdigit, isalnum */
#include <string.h> /* strstr */
#include <sys/time.h>
#ifdef _Item_ReLifeAct
#include "battle_magic.h"
#endif
#ifdef _ITEM_WARP_FIX_BI
extern tagRidePetTable ridePetTable[296];
#endif
#include "npc_poolitemshop.h"
#include "pet_skill.h"
#ifdef _CFREE_petskill
#include "npc_freepetskillshop.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
#include "profession_skill.h"
#endif
#ifdef _ITEM_UNCONFIRMED
#include "magic_base.h"
#endif
#include "npc_petshop.h"
int ITEM_TimeDelCheck(int itemindex) {
  int icnt, jcnt, playernum;
  playernum = CHAR_getPlayerMaxNum();
  for (icnt = 0; icnt < playernum; icnt++) {
    if (CHAR_CHECKINDEX(icnt)) {
      for (jcnt = 0; jcnt < CHAR_getMaxInventory(icnt); jcnt++) {
        if (CHAR_getItemIndex(icnt, jcnt) == itemindex) {
          print("道具效力时间已到，进行奇怪道具处理(%s)(%s)\n",
                CHAR_getUseName(icnt), ITEM_getAppropriateName(itemindex));
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

int ITEM_eventDrop(int itemindex, int charaindex, int itemcharaindex) {
  typedef void (*DROPF)(int, int);
  char szBuffer[256] = "";
  DROPF dropfunc = NULL;

  if (ITEM_CHECKINDEX(itemindex) == FALSE)
    return -1;

  dropfunc = (DROPF)ITEM_getFunctionPointer(itemindex, ITEM_DROPFUNC);
  if (dropfunc) {
    dropfunc(charaindex, itemindex);
  }
#ifdef _ALLBLUES_LUA_1_2
  else {
    RunItemDropEvent(charaindex, itemindex);
  }
#endif
  if (ITEM_getInt(itemindex, ITEM_VANISHATDROP) != 1)
    return 0;
  snprintf(szBuffer, sizeof(szBuffer), "%s(篮)绰 家戈沁绢夸.",
           ITEM_getAppropriateName(itemindex));
  CHAR_talkToCli(charaindex, -1, szBuffer, CHAR_COLORWHITE);
#ifdef _ITEM_METAMOGET
  if (ITEM_getInt(itemindex, ITEM_FACEDATA) != 0) {
    if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) { // 啪铰 惑怕啊 酒匆 版快
      CHAR_setInt(
          charaindex, CHAR_BASEBASEIMAGENUMBER,
          CHAR_getInt(charaindex, CHAR_CHARMETAMO)); // 某腐磐 捞固瘤 函脚
      CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
                  CHAR_getInt(charaindex, CHAR_CHARMETAMO));
    } else { // 啪铰 惑怕老 版快
      CHAR_setInt(
          charaindex, CHAR_BASEIMAGENUMBER,
          CHAR_getInt(charaindex, CHAR_CHARMETAMORIDE)); // 啪铰 捞固瘤 函脚
    }

    CHAR_setInt(charaindex, CHAR_CHARMETAMONO, 0);
    CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER,
                ITEM_getInt(itemindex, ITEM_FACEDATA));
    CHAR_setInt(charaindex, CHAR_BATTLEHPUP,
                CHAR_getInt(charaindex, CHAR_BATTLEHP)); // 傈捧 郴备仿雀汗 府悸
    ITEM_setInt(itemindex, ITEM_FACEDATA, 0);

    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
    CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
    CHAR_complianceParameter(charaindex);
  }
#endif
  {
    LogItem(CHAR_getChar(charaindex, CHAR_NAME), /* 平乓仿   */
            CHAR_getChar(charaindex, CHAR_CDKEY), itemindex, "酒捞袍 家戈",
            CHAR_getInt(charaindex, CHAR_FLOOR),
            CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
            ITEM_getChar(itemindex, ITEM_UNIQUECODE),
            ITEM_getChar(itemindex, ITEM_NAME), ITEM_getInt(itemindex, ITEM_ID),
            charaindex);
  }
  CHAR_setItemIndex(charaindex, itemcharaindex, -1);
  ITEM_endExistItemsOne(itemindex);
  return 1;
}

#undef UNDEF
#define UNDEF (-1)

typedef struct {
  char *cmd;        /* 摹    侬   */
  char *onmessage;  /*  丢永本□斥  侬      */
  char *offmessage; /*  丢永本□斥  侬      */
  int element;      /* 覆擂卞卅月邰豳 */
  int maxElement; /* element匹隙烂今木凶邰豳及    袄毛手勾邰豳 */
} ITEM_EFFECTPARAM;
static ITEM_EFFECTPARAM ITEM_restorableParam[] = {
    {"hp", "HP回复了。", "HP降低了。", CHAR_HP, CHAR_WORKMAXHP},
    {"mp", "MP回复了。", "MP降低了。", CHAR_MP, CHAR_WORKMAXMP},
};
static ITEM_EFFECTPARAM ITEM_statusParam[] = {
    {"po", CHAR_POISONSTRING, CHAR_RECOVERPOISONSTRING, CHAR_POISON, UNDEF},

    {"pa", CHAR_PARALYSISSTRING, CHAR_RECOVERPARALYSISSTRING, CHAR_PARALYSIS,
     UNDEF},

    {"si", CHAR_SILENCESTRING, CHAR_RECOVERSILENCESTRING, CHAR_SLEEP, UNDEF},

    {"st", CHAR_STONESTRING, CHAR_RECOVERSTONESTRING, CHAR_STONE, UNDEF},

    {"da", CHAR_DARKNESSSTRING, CHAR_RECOVERDARKNESSSTRING, CHAR_DRUNK, UNDEF},

    {"co", CHAR_CONFUSIONSTRING, CHAR_RECOVERCONFUSIONSTRING, CHAR_CONFUSION,
     UNDEF},
};

/*#define LOCAL_DEBUG*/
#ifdef LOCAL_DEBUG
#define DOUTFILE "doutfile"
#include <stdarg.h>
#include <stdio.h>
static int eprintf(char *format, ...) {
  va_list arg;
  long len;
  FILE *fp;
  fp = fopen(DOUTFILE, "a");
  va_start(arg, format);
  len = vfprintf(stderr, format, arg);
  if (fp) {
    vfprintf(fp, format, arg);
    fclose(fp);
  }
  va_end(arg);
  return len;
}

#define fprint eprintf
#endif

static BOOL ITEM_isValidEffect(char *cmd, int value) {
  int i;

  for (i = 0; i < arraysizeof(ITEM_restorableParam); i++) {
    if (!strcmp(cmd, ITEM_restorableParam[i].cmd)) {
      return value <= 0 ? FALSE : TRUE;
    }
  }
  if (value < 0)
    return FALSE;
  for (i = 0; i < arraysizeof(ITEM_statusParam); i++) {
    if (!strcmp(cmd, ITEM_statusParam[i].cmd)) {
      return TRUE;
    }
  }
  return FALSE;
}

#define ID_BUF_LEN_MAX 20
#define SEPARATORI '|'
BOOL ITEM_MedicineInit(ITEM_Item *itm) {
  char cmd[ID_BUF_LEN_MAX], arg[ID_BUF_LEN_MAX];
  int value;
  int effectCount = 0;
  char *p, *q;
  char *effectarg;

  effectarg = itm->string[ITEM_ARGUMENT].string;
  if (*effectarg == '\0')
    return TRUE;
  for (p = effectarg; *p != '\0';) {
    int i;
    if (*p == SEPARATORI)
      p++;
    for (q = cmd, i = 0; isalnum(*p) && i < ID_BUF_LEN_MAX; i++) {
      *q++ = *p++;
    }
    *q = '\0';
    if (q == cmd || *p != SEPARATORI) {
      fprint("ITEM_medicineInit: error(c)? invalid:%s(%d)\n", effectarg,
             effectCount);
      return FALSE;
    }
    p++;
    for (q = arg, i = 0; isdigit(*p) && i < ID_BUF_LEN_MAX; i++) {
      *q++ = *p++;
    }
    *q = '\0';
    value = strtol(arg, &q, 10); /* strtol()反OK? */
    if (ITEM_isValidEffect(cmd, value)) {
      effectCount++;
    } else {
      fprint("ITEM_medicineInit: error(v)? invalid:%s(%d)\n", effectarg,
             effectCount);
      return FALSE;
    }
  }
  return (effectCount == 0) ? FALSE : TRUE;
}

static BOOL ITEM_medicineRaiseEffect(int charaindex, char *cmd, int value) {
  int i;
  char ansmsg[256];
  for (i = 0; i < arraysizeof(ITEM_restorableParam); i++) {
    if (!strcmp(cmd, ITEM_restorableParam[i].cmd)) {
#if 1
      int maxv, curv, amount;
      char *onoroff = NULL;
      maxv = CHAR_getWorkInt(charaindex, ITEM_restorableParam[i].maxElement);
      curv = CHAR_getInt(charaindex, ITEM_restorableParam[i].element);
      amount = value;
      if (curv + amount < 0)
        amount = -curv;
      CHAR_setInt(charaindex, ITEM_restorableParam[i].element,
                  min((curv + amount), maxv));
      if (amount > 0)
        onoroff = ITEM_restorableParam[i].onmessage;
      else if (amount < 0)
        onoroff = ITEM_restorableParam[i].offmessage;
      else
        return FALSE;
      strcpysafe(ansmsg, sizeof(ansmsg), onoroff);
      CHAR_talkToCli(charaindex, -1, ansmsg, CHAR_COLORWHITE);
      return TRUE;
#else
      if (value == ITEM_EFFECT_RESTORE_NORMAL) {
        int maxv, curv, amount;
        maxv = CHAR_getWorkInt(charaindex, ITEM_restorableParam[i].maxElement);
        curv = CHAR_getInt(charaindex, ITEM_restorableParam[i].element);
        amount = 30;
        if (curv + amount < 0)
          amount = -curv;
        CHAR_setInt(charaindex, ITEM_restorableParam[i].element,
                    min((curv + amount), maxv));
        snprintf(ansmsg, sizeof(ansmsg), "%s回复了。",
                 ITEM_restorableParam[i].onmessage);
        CHAR_talkToCli(charaindex, -1, ansmsg, CHAR_COLORWHITE);
        return TRUE;
      } else if (value == ITEM_EFFECT_RESTORE_COMPLETE) {
        int maxv;
        maxv = CHAR_getWorkInt(charaindex, ITEM_restorableParam[i].maxElement);
        CHAR_setInt(charaindex, ITEM_restorableParam[i].element, maxv);
        snprintf(ansmsg, sizeof(ansmsg), "%s已完全回复。",
                 ITEM_restorableParam[i].onmessage);
        CHAR_talkToCli(charaindex, -1, ansmsg, CHAR_COLORWHITE);
        return TRUE;
      } else
        return FALSE;
#endif
    }
  }
  if (value >= 0) {
    int found = 0;
    for (i = 0; i < arraysizeof(ITEM_statusParam); i++) {
      if (!strcmp(cmd, ITEM_statusParam[i].cmd)) {
        ansmsg[0] = '\0';
        if (value &&
            CHAR_getInt(charaindex, ITEM_statusParam[i].element) < value) {
          CHAR_setInt(charaindex, ITEM_statusParam[i].element, value);
          strcpysafe(ansmsg, sizeof(ansmsg), ITEM_statusParam[i].onmessage);
        } else {
          if (CHAR_getInt(charaindex, ITEM_statusParam[i].element)) {
            strcpysafe(ansmsg, sizeof(ansmsg), ITEM_statusParam[i].offmessage);
          }
          CHAR_setInt(charaindex, ITEM_statusParam[i].element, 0);
        }
        found = 1;
        if (ansmsg[0] != '\0')
          CHAR_talkToCli(charaindex, -1, ansmsg, CHAR_COLORWHITE);
      }
    }
    if (found) {
      CHAR_sendCToArroundCharacter(
          CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
      return TRUE;
    }
  }
#undef ITEM_STATUSCHANGEVALUE
  strcpysafe(ansmsg, sizeof(ansmsg), "酒公老档 老绢唱瘤 臼疽嚼聪促。");
  CHAR_talkToCli(charaindex, -1, ansmsg, CHAR_COLORWHITE);
  return FALSE;
}

void ITEM_MedicineUsed(int charaindex, int to_charaindex, int itemindex) {
  int itemid;
  int usedf = 0;
  char cmd[ID_BUF_LEN_MAX], arg[ID_BUF_LEN_MAX];
  int value;
  char *p, *q;
  char *effectarg;
  char ansmsg[256];
  itemid = CHAR_getItemIndex(charaindex, itemindex);
  if (!ITEM_CHECKINDEX(itemid))
    return;
  effectarg = ITEM_getChar(itemid, ITEM_ARGUMENT);
  snprintf(ansmsg, sizeof(ansmsg), "抓到了%s 。",
           ITEM_getChar(itemid, ITEM_NAME));
  CHAR_talkToCli(charaindex, -1, ansmsg, CHAR_COLORWHITE);
  for (p = effectarg; *p != '\0';) {
    int i;
    if (*p == SEPARATORI)
      p++;
    for (q = cmd, i = 0; isalnum(*p) && i < ID_BUF_LEN_MAX; i++) {
      *q++ = *p++;
    }
    *q = '\0';
    if (q == cmd || *p != SEPARATORI)
      return;
    p++;
    for (q = arg, i = 0; isdigit(*p) && i < ID_BUF_LEN_MAX; i++) {
      *q++ = *p++;
    }
    *q = '\0';
    value = strtol(arg, &q, 10);
    if (ITEM_medicineRaiseEffect(charaindex, cmd, value)) {
      usedf = 1;
    }
  }
  if (usedf) {
    CHAR_DelItem(charaindex, itemindex);
    CHAR_sendStatusString(charaindex, "P");

  } else
    fprint("ITEM_medicineUsed: error? cannot be used.\n");
#undef ID_BUF_LEN_MAX
#undef SEPARATORI
}

void ITEM_SandClockDetach(int charaindex, int itemid) {
  int i;
  if (!ITEM_CHECKINDEX(itemid))
    return;
  for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
    if (CHAR_getItemIndex(charaindex, i) == itemid) {
      CHAR_DelItem(charaindex, i);
      CHAR_talkToCli(charaindex, -1, "一卸下沙漏，竟忽然坏了！",
                     CHAR_COLORWHITE);
      print("deleted sand clock!\n");
      break;
    }
  }
}

void ITEM_SandClockLogin(int charaindex) {
  int i;
  int dTime;

  for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
    int itemindex = CHAR_getItemIndex(charaindex, i);
    if (ITEM_getInt(itemindex, ITEM_ID) != 29)
      continue;
    if (ITEM_getInt(itemindex, ITEM_VAR4) == 0)
      continue;
    dTime = NowTime.tv_sec - ITEM_getInt(itemindex, ITEM_VAR4);
    ITEM_setInt(itemindex, ITEM_VAR3,
                ITEM_getInt(itemindex, ITEM_VAR3) + dTime);
  }
}

void ITEM_SandClockLogout(int charaindex) {
  int i;
  for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
    int itemindex = CHAR_getItemIndex(charaindex, i);
    if (ITEM_getInt(itemindex, ITEM_ID) != 29)
      continue;
    ITEM_setInt(itemindex, ITEM_VAR4, NowTime.tv_sec);
  }
}

// Arminius 7.2: Ra's amulet , remove "static"
/*static*/
BOOL ITEM_getArgument(char *argument, char *entryname, char *buf, int buflen) {
  int i;
  char dividedbypipeline[512];
  for (i = 1;; i++) {
    BOOL ret;
    ret = getStringFromIndexWithDelim(argument, "|", i, dividedbypipeline,
                                      sizeof(dividedbypipeline));
    if (ret == TRUE) {
      int tworet = 1;
      char first[512];
      tworet &= getStringFromIndexWithDelim(dividedbypipeline, ":", 1, first,
                                            sizeof(first));
      tworet &=
          getStringFromIndexWithDelim(dividedbypipeline, ":", 2, buf, buflen);
      if (tworet != 0)
        if (strcasecmp(first, entryname) == 0)
          return TRUE;
    } else
      break;
  }

  return FALSE;
}

void ITEM_addTitleAttach(int charaindex, int itemindex) {
  char titlenumstring[256];
  int titleindex;

  if (ITEM_CHECKINDEX(itemindex) == FALSE)
    return;

  if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "addt",
                       titlenumstring, sizeof(titlenumstring)) == FALSE) {
    print("Can't find \"addt\" entry: %s\n",
          ITEM_getChar(itemindex, ITEM_ARGUMENT));
    return;
  }
  titleindex = atoi(titlenumstring);
  TITLE_addtitle(charaindex, titleindex);
  CHAR_sendStatusString(charaindex, "T");
}

void ITEM_delTitleDetach(int charaindex, int itemindex) {
  char titlenumstring[256];
  int titleindex;

  if (ITEM_CHECKINDEX(itemindex) == FALSE)
    return;

  if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "delt",
                       titlenumstring, sizeof(titlenumstring)) == FALSE) {
    print("Can't find \"delt\" entry: %s\n",
          ITEM_getChar(itemindex, ITEM_ARGUMENT));
    return;
  }
  titleindex = atoi(titlenumstring);
  TITLE_deltitle(charaindex, titleindex);
  CHAR_sendStatusString(charaindex, "T");
}

void ITEM_DeleteByWatched(int myobjindex, int moveobjindex, CHAR_ACTION act,
                          int x, int y, int dir, int *opt, int optlen) {

  int itemindex, moveindex;
  char szBuffer[256] = "";
  itemindex = OBJECT_getIndex(myobjindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (OBJECT_getType(moveobjindex) == OBJTYPE_CHARA) {
    moveindex = OBJECT_getIndex(moveobjindex);
    if (CHAR_getInt(moveindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      snprintf(szBuffer, sizeof(szBuffer), "%s",
               ITEM_getAppropriateName(itemindex));
      CHAR_talkToCli(moveindex, -1, "%s 消灭了。", CHAR_COLORWHITE);
    }
  }

  ITEM_endExistItemsOne(itemindex);
  CHAR_ObjectDelete(myobjindex);
}

void ITEM_DeleteTimeWatched(int objindex, int moveobjindex, CHAR_ACTION act,
                            int x, int y, int dir, int *opt, int optlen) {
  int itemindex;
  int itemputtime;

  if (!CHECKOBJECTUSE(objindex)) {
    return;
  }
  itemindex = OBJECT_getIndex(objindex);
  if (!ITEM_CHECKINDEX(itemindex)) {
    return;
  }
  itemputtime = ITEM_getInt(itemindex, ITEM_PUTTIME);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if ((int)NowTime.tv_sec > (int)(itemputtime + getItemdeletetime())) {
    if (ITEM_TimeDelCheck(itemindex) == FALSE) {
      return;
    }
    {
      LogItem("NULL", "NULL", itemindex, "TiemDelete",
              OBJECT_getFloor(objindex), OBJECT_getX(objindex),
              OBJECT_getY(objindex), ITEM_getChar(itemindex, ITEM_UNIQUECODE),
              ITEM_getChar(itemindex, ITEM_NAME),
              ITEM_getInt(itemindex, ITEM_ID), -1);
    }
    ITEM_endExistItemsOne(itemindex);
    CHAR_ObjectDelete(objindex);
  }
}

void ITEM_useEffectTohelos(int charaindex, int to_charaindex,
                           int haveitemindex) {
  char buf[64];
  char msgbuf[64];
  int ret;
  int itemindex;
  int cutrate, limitcount;
  int per;
  int sendcharaindex = charaindex;

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  CHAR_setItemIndex(charaindex, haveitemindex, -1);
  CHAR_sendItemDataOne(charaindex, haveitemindex);
  ret = getStringFromIndexWithDelim(ITEM_getChar(itemindex, ITEM_ARGUMENT), "|",
                                    1, buf, sizeof(buf));
  if (ret != TRUE) {
    {
      LogItem(CHAR_getChar(charaindex, CHAR_NAME),
              CHAR_getChar(charaindex, CHAR_CDKEY), itemindex, "FieldErrorUse",
              CHAR_getInt(charaindex, CHAR_FLOOR),
              CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
              ITEM_getChar(itemindex, ITEM_UNIQUECODE),
              ITEM_getChar(itemindex, ITEM_NAME),
              ITEM_getInt(itemindex, ITEM_ID), charaindex);
    }
    ITEM_endExistItemsOne(itemindex);
    return;
  }
  cutrate = atoi(buf);
  if (cutrate < 0)
    cutrate = 0;
  ret = getStringFromIndexWithDelim(ITEM_getChar(itemindex, ITEM_ARGUMENT), "|",
                                    2, buf, sizeof(buf));
  if (ret != TRUE) {
    {
      LogItem(CHAR_getChar(charaindex, CHAR_NAME),
              CHAR_getChar(charaindex, CHAR_CDKEY), itemindex, "FieldUse",
              CHAR_getInt(charaindex, CHAR_FLOOR),
              CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
              ITEM_getChar(itemindex, ITEM_UNIQUECODE),
              ITEM_getChar(itemindex, ITEM_NAME),
              ITEM_getInt(itemindex, ITEM_ID), charaindex);
    }
    ITEM_endExistItemsOne(itemindex);
    return;
  }
  limitcount = atoi(buf);
  if (limitcount < 0)
    limitcount = 0;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
    sendcharaindex = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
  }
  CHAR_setWorkInt(sendcharaindex, CHAR_WORK_TOHELOS_CUTRATE, cutrate);
  CHAR_setWorkInt(sendcharaindex, CHAR_WORK_TOHELOS_COUNT, limitcount);

  snprintf(msgbuf, sizeof(msgbuf), "抓到了%s 。",
           ITEM_getChar(itemindex, ITEM_NAME));
  CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORWHITE);

  if (sendcharaindex != charaindex) {
    snprintf(msgbuf, sizeof(msgbuf), "%s 抓到了 %s。 ",
             CHAR_getChar(charaindex, CHAR_NAME),
             ITEM_getChar(itemindex, ITEM_NAME));
    CHAR_talkToCli(sendcharaindex, -1, msgbuf, CHAR_COLORWHITE);
  }

  {
    LogItem(CHAR_getChar(charaindex, CHAR_NAME),
            CHAR_getChar(charaindex, CHAR_CDKEY), itemindex, "FieldUse",
            CHAR_getInt(charaindex, CHAR_FLOOR),
            CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
            ITEM_getChar(itemindex, ITEM_UNIQUECODE),
            ITEM_getChar(itemindex, ITEM_NAME), ITEM_getInt(itemindex, ITEM_ID),
            charaindex);
  }
  ITEM_endExistItemsOne(itemindex);
  per = ENCOUNT_getEncountPercentMin(
      sendcharaindex, CHAR_getInt(sendcharaindex, CHAR_FLOOR),
      CHAR_getInt(sendcharaindex, CHAR_X), CHAR_getInt(sendcharaindex, CHAR_Y));
  if (per != -1) {
    CHAR_setWorkInt(sendcharaindex, CHAR_WORKENCOUNTPROBABILITY_MIN, per);
  }
  per = ENCOUNT_getEncountPercentMax(
      sendcharaindex, CHAR_getInt(sendcharaindex, CHAR_FLOOR),
      CHAR_getInt(sendcharaindex, CHAR_X), CHAR_getInt(sendcharaindex, CHAR_Y));
  if (per != -1) {
    CHAR_setWorkInt(sendcharaindex, CHAR_WORKENCOUNTPROBABILITY_MAX, per);
  }
  CHAR_sendStatusString(sendcharaindex, "E");
}

void ITEM_dropMic(int charaindex, int itemindex) {
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  CHAR_setWorkInt(charaindex, CHAR_WORKFLG,
                  CHAR_getWorkInt(charaindex, CHAR_WORKFLG) & ~WORKFLG_MICMODE);
}

void ITEM_useMic_Field(int charaindex, int to_charaindex, int haveitemindex) {
  if (CHAR_getWorkInt(charaindex, CHAR_WORKFLG) & WORKFLG_MICMODE) {
    CHAR_setWorkInt(charaindex, CHAR_WORKFLG,
                    CHAR_getWorkInt(charaindex, CHAR_WORKFLG) &
                        ~WORKFLG_MICMODE);
    CHAR_talkToCli(charaindex, -1, "将麦克风设定为OFF。", CHAR_COLORWHITE);
  } else {
    CHAR_setWorkInt(charaindex, CHAR_WORKFLG,
                    CHAR_getWorkInt(charaindex, CHAR_WORKFLG) |
                        WORKFLG_MICMODE);
    CHAR_talkToCli(charaindex, -1, "将麦克风设定为ON。", CHAR_COLORWHITE);
  }
}
#if 1
char *aszHealStringByOwn[] = {
    "%s丛狼 郴备仿阑 %d 雀汗窍看绢夸", "%s丛狼 扁仿阑 %d 雀汗窍看绢夸",
    "%s丛狼 概仿捞 %d 惑铰窍看绢夸", "%s狼 面己档啊 %d 惑铰窍看绢夸", ""};

char *aszDownStringByOwn[] = {
    "%s丛狼 郴备仿阑 %d 雀汗窍看绢夸", "%s丛狼 扁仿阑 %d 雀汗窍看绢夸",
    "%s丛狼 概仿捞 %d 惑铰窍看绢夸", "%s狼 面己档啊 %d 惑铰窍看绢夸", ""};

char *aszHealStringByOther[] = {"%s(捞)啊 %s狼 郴备仿阑 %d 雀汗沁绢夸",
                                "%s(捞)啊 %s狼 扁仿阑 %d 雀汗沁绢夸",
                                "%s(捞)啊 %s狼 概仿阑 %d 雀汗沁绢夸",
                                "%s(捞)啊 %s狼 面己档甫 %d 棵啡绢夸", ""};

char *aszDownStringByOther[] = {"%s(捞)啊 %s狼 郴备仿阑 %d 雀汗沁绢夸",
                                "%s(捞)啊 %s狼 扁仿阑 %d 雀汗沁绢夸",
                                "%s(捞)啊 %s狼 概仿阑 %d 雀汗沁绢夸",
                                "%s(捞)啊 %s狼 面己档甫 %d 棵啡绢夸", ""};

char *aszKeyString[] = {"郴", "扁", "概", "面", ""};
int aHealInt[] = {CHAR_HP, CHAR_MP, CHAR_CHARM, CHAR_VARIABLEAI, -1};
int aHealMaxWork[] = {CHAR_WORKMAXHP, CHAR_WORKMAXMP, -1, -1, -1};

void ITEM_useRecovery_Field(int charaindex, int toindex, int haveitemindex) {
  int work, workmax, workmin;
  int power[BD_KIND_END] = {0, 0, 0}, prevhp = 0, workhp = 0,
      recovery[BD_KIND_END] = {0, 0, 0};
  int itemindex, kind = BD_KIND_HP, HealFlg = 0, j;
  char *p = NULL, *arg, msgbuf[256];
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (CHAR_CHECKINDEX(toindex) == FALSE)
    return;
  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (arg == "\0")
    return;

#ifdef _ITEM_UNBECOMEPIG
  if ((p = strstr(arg, "解猪")) != NULL) {
    if (CHAR_getInt(toindex, CHAR_BECOMEPIG) > -1) {
      CHAR_setInt(toindex, CHAR_BECOMEPIG, -1);
      CHAR_complianceParameter(toindex);
      CHAR_sendCToArroundCharacter(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX));
      CHAR_send_P_StatusString(toindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
      CHAR_talkToCli(toindex, -1, "乌力化失效了。", CHAR_COLORWHITE);
      CHAR_DelItemMess(charaindex, haveitemindex, 0);
    }
    return;
  }
#endif
#ifdef _ITEM_LVUPUP
  if ((p = strstr(arg, "LVUPUP")) != NULL) {
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      int pidx = 0, lvup = 0, lv = 0;
      sprintf(msgbuf, "%s", p + 7);
      pidx = atoi(strtok(msgbuf, " "));
      if (pidx != CHAR_getInt(toindex, CHAR_PETID) ||
          CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 0) {
        CHAR_talkToCli(charaindex, -1, "无法使用", CHAR_COLORWHITE);
        return;
      }
      lv = CHAR_getInt(toindex, CHAR_LV);
      if (lv < 10 || lv >= 140) {
        CHAR_talkToCli(charaindex, -1, "目前等级无法使用", CHAR_COLORWHITE);
        return;
      }
      if ((p = strstr(arg, "地")) != NULL)
        lvup = 2;
      if ((p = strstr(arg, "水")) != NULL)
        lvup = 3;
      if ((p = strstr(arg, "火")) != NULL)
        lvup = 0;
      if ((p = strstr(arg, "风")) != NULL)
        lvup = 1;
      if (lv % 4 != lvup ||
          CHAR_getInt(toindex, CHAR_LIMITLEVEL) - lv >= 1) { //
        CHAR_talkToCli(charaindex, -1, "破除封印失败", CHAR_COLORWHITE);
        CHAR_DelItemMess(charaindex, haveitemindex, 0);
        return;
      }
      CHAR_setInt(toindex, CHAR_LIMITLEVEL,
                  CHAR_getInt(toindex, CHAR_LIMITLEVEL) + 1);
      CHAR_talkToCli(charaindex, -1, "封印魔咒减弱，最高等级提升",
                     CHAR_COLORWHITE);
      if (CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 140) {
        CHAR_setInt(toindex, CHAR_LIMITLEVEL, 0);
        CHAR_talkToCli(charaindex, -1, "封印魔咒解除了", CHAR_COLORWHITE);
      }
      CHAR_DelItemMess(charaindex, haveitemindex, 0);
      CHAR_complianceParameter(toindex);
      CHAR_sendCToArroundCharacter(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX));
    }
    return;
  }
  if ((p = strstr(arg, "UPUPLV")) != NULL) {
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      int pidx = 0, lv = 0;
      sprintf(msgbuf, "%s", p + 7);
      pidx = atoi(strtok(msgbuf, " "));
      if (pidx != CHAR_getInt(toindex, CHAR_PETID) ||
          CHAR_getInt(toindex, CHAR_LIMITLEVEL) == 0) {
        CHAR_talkToCli(charaindex, -1, "无法使用", CHAR_COLORWHITE);
        return;
      }
      lv = CHAR_getInt(toindex, CHAR_LV);
      if (lv < 125 || lv > 140) {
        CHAR_talkToCli(charaindex, -1, "目前等级无法使用", CHAR_COLORWHITE);
        return;
      }
      CHAR_setInt(toindex, CHAR_LIMITLEVEL, 0);
      CHAR_talkToCli(charaindex, -1, "封印魔咒解除了", CHAR_COLORWHITE);
      CHAR_DelItemMess(charaindex, haveitemindex, 0);
      CHAR_complianceParameter(toindex);
      CHAR_sendCToArroundCharacter(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX));
    }
    return;
  }
#endif
#ifdef _ITEM_PROPERTY
  if ((p = strstr(arg, "PROPERTY")) != NULL) {
    // print("地水火风:%s", arg );
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
      return;
    if ((p = strstr(arg, "+")) != NULL) { //旋转属性 正转
      if (CHAR_getInt(toindex, CHAR_EARTHAT) == 100)
        CHAR_setInt(toindex, CHAR_EARTHAT,
                    CHAR_getInt(toindex, CHAR_EARTHAT) - 10),
            CHAR_setInt(toindex, CHAR_WATERAT,
                        CHAR_getInt(toindex, CHAR_WATERAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_WATERAT) == 100)
        CHAR_setInt(toindex, CHAR_WATERAT,
                    CHAR_getInt(toindex, CHAR_WATERAT) - 10),
            CHAR_setInt(toindex, CHAR_FIREAT,
                        CHAR_getInt(toindex, CHAR_FIREAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_FIREAT) == 100)
        CHAR_setInt(toindex, CHAR_FIREAT,
                    CHAR_getInt(toindex, CHAR_FIREAT) - 10),
            CHAR_setInt(toindex, CHAR_WINDAT,
                        CHAR_getInt(toindex, CHAR_WINDAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_WINDAT) == 100)
        CHAR_setInt(toindex, CHAR_WINDAT,
                    CHAR_getInt(toindex, CHAR_WINDAT) - 10),
            CHAR_setInt(toindex, CHAR_EARTHAT,
                        CHAR_getInt(toindex, CHAR_EARTHAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_EARTHAT) > 0 &&
               CHAR_getInt(toindex, CHAR_WATERAT) > 0)
        CHAR_setInt(toindex, CHAR_EARTHAT,
                    CHAR_getInt(toindex, CHAR_EARTHAT) - 10),
            CHAR_setInt(toindex, CHAR_WATERAT,
                        CHAR_getInt(toindex, CHAR_WATERAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_WATERAT) > 0 &&
               CHAR_getInt(toindex, CHAR_FIREAT) > 0)
        CHAR_setInt(toindex, CHAR_WATERAT,
                    CHAR_getInt(toindex, CHAR_WATERAT) - 10),
            CHAR_setInt(toindex, CHAR_FIREAT,
                        CHAR_getInt(toindex, CHAR_FIREAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_FIREAT) > 0 &&
               CHAR_getInt(toindex, CHAR_WINDAT) > 0)
        CHAR_setInt(toindex, CHAR_FIREAT,
                    CHAR_getInt(toindex, CHAR_FIREAT) - 10),
            CHAR_setInt(toindex, CHAR_WINDAT,
                        CHAR_getInt(toindex, CHAR_WINDAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_WINDAT) > 0 &&
               CHAR_getInt(toindex, CHAR_EARTHAT) > 0)
        CHAR_setInt(toindex, CHAR_WINDAT,
                    CHAR_getInt(toindex, CHAR_WINDAT) - 10),
            CHAR_setInt(toindex, CHAR_EARTHAT,
                        CHAR_getInt(toindex, CHAR_EARTHAT) + 10);
    }
    if ((p = strstr(arg, "-")) != NULL) { //旋转属性 反转
      if (CHAR_getInt(toindex, CHAR_EARTHAT) == 100)
        CHAR_setInt(toindex, CHAR_EARTHAT,
                    CHAR_getInt(toindex, CHAR_EARTHAT) - 10),
            CHAR_setInt(toindex, CHAR_WINDAT,
                        CHAR_getInt(toindex, CHAR_WINDAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_WATERAT) == 100)
        CHAR_setInt(toindex, CHAR_WATERAT,
                    CHAR_getInt(toindex, CHAR_WATERAT) - 10),
            CHAR_setInt(toindex, CHAR_EARTHAT,
                        CHAR_getInt(toindex, CHAR_EARTHAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_FIREAT) == 100)
        CHAR_setInt(toindex, CHAR_FIREAT,
                    CHAR_getInt(toindex, CHAR_FIREAT) - 10),
            CHAR_setInt(toindex, CHAR_WATERAT,
                        CHAR_getInt(toindex, CHAR_WATERAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_WINDAT) == 100)
        CHAR_setInt(toindex, CHAR_WINDAT,
                    CHAR_getInt(toindex, CHAR_WINDAT) - 10),
            CHAR_setInt(toindex, CHAR_FIREAT,
                        CHAR_getInt(toindex, CHAR_FIREAT) + 10);
      else if (CHAR_getInt(toindex, CHAR_EARTHAT) > 0 &&
               CHAR_getInt(toindex, CHAR_WATERAT) > 0)
        CHAR_setInt(toindex, CHAR_EARTHAT,
                    CHAR_getInt(toindex, CHAR_EARTHAT) + 10),
            CHAR_setInt(toindex, CHAR_WATERAT,
                        CHAR_getInt(toindex, CHAR_WATERAT) - 10);
      else if (CHAR_getInt(toindex, CHAR_WATERAT) > 0 &&
               CHAR_getInt(toindex, CHAR_FIREAT) > 0)
        CHAR_setInt(toindex, CHAR_WATERAT,
                    CHAR_getInt(toindex, CHAR_WATERAT) + 10),
            CHAR_setInt(toindex, CHAR_FIREAT,
                        CHAR_getInt(toindex, CHAR_FIREAT) - 10);
      else if (CHAR_getInt(toindex, CHAR_FIREAT) > 0 &&
               CHAR_getInt(toindex, CHAR_WINDAT) > 0)
        CHAR_setInt(toindex, CHAR_FIREAT,
                    CHAR_getInt(toindex, CHAR_FIREAT) + 10),
            CHAR_setInt(toindex, CHAR_WINDAT,
                        CHAR_getInt(toindex, CHAR_WINDAT) - 10);
      else if (CHAR_getInt(toindex, CHAR_WINDAT) > 0 &&
               CHAR_getInt(toindex, CHAR_EARTHAT) > 0)
        CHAR_setInt(toindex, CHAR_WINDAT,
                    CHAR_getInt(toindex, CHAR_WINDAT) + 10),
            CHAR_setInt(toindex, CHAR_EARTHAT,
                        CHAR_getInt(toindex, CHAR_EARTHAT) - 10);
    }
    CHAR_DelItemMess(charaindex, haveitemindex, 0);
    CHAR_complianceParameter(toindex);
    CHAR_sendCToArroundCharacter(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX));
    CHAR_send_P_StatusString(toindex,
                             CHAR_P_STRING_EARTH | CHAR_P_STRING_WATER |
                                 CHAR_P_STRING_FIRE | CHAR_P_STRING_WIND);
    return;
  }
#endif
#ifdef _ITEM_ADDPETEXP
  if ((p = strstr(arg, "GETEXP")) != NULL) {
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
      getStringFromIndexWithDelim(arg, "|", 2, msgbuf,
                                  sizeof(msgbuf)); //宠物编号
      if (atoi(msgbuf) == CHAR_getInt(toindex, CHAR_PETID)) {
        getStringFromIndexWithDelim(arg, "|", 3, msgbuf,
                                    sizeof(msgbuf)); //等级上限(等级超过才可用)
        if (CHAR_getInt(toindex, CHAR_LV) >= atoi(msgbuf)) {
          getStringFromIndexWithDelim(arg, "|", 4, msgbuf,
                                      sizeof(msgbuf)); //经验值增加
          if (CHAR_getInt(toindex, CHAR_LV) < CHAR_MAXUPLEVEL) {
            int UpLevel = 0;
            CHAR_setWorkInt(toindex, CHAR_WORKGETEXP,
                            atoi(msgbuf)); //回存CHAR_WORKGETEXP
            CHAR_AddMaxExp(toindex, CHAR_getWorkInt(toindex, CHAR_WORKGETEXP));
            sprintf(msgbuf, "%d EXP 甫 颊俊 持菌促.",
                    CHAR_getWorkInt(toindex, CHAR_WORKGETEXP));
            CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORWHITE);
            UpLevel = CHAR_LevelUpCheck(toindex, charaindex);
            if (UpLevel > 0) {
              if (getBattleDebugMsg() != 0) {
                snprintf(msgbuf, sizeof(msgbuf),
                         "%s(捞)啊 饭骇捞 %d(栏)肺 惑铰窍看嚼聪促.",
                         CHAR_getUseName(toindex),
                         CHAR_getInt(toindex, CHAR_LV));
                BATTLE_talkToCli(charaindex, msgbuf, CHAR_COLORYELLOW);
              }
            }
            for (j = 0; j < UpLevel; j++) {
              CHAR_PetLevelUp(toindex);
              CHAR_PetAddVariableAi(toindex, AI_FIX_PETLEVELUP);
            }
            CHAR_complianceParameter(toindex);
          } else {
            CHAR_talkToCli(charaindex, -1, "荤侩且荐 绝嚼聪促！",
                           CHAR_COLORWHITE);
          }
          /*CHAR_send_P_StatusString(  charindex, CHAR_P_STRING_DUELPOINT|
          CHAR_P_STRING_TRANSMIGRATION| CHAR_P_STRING_RIDEPET|
          CHAR_P_STRING_BASEBASEIMAGENUMBER| CHAR_P_STRING_GOLD|
          CHAR_P_STRING_EXP| CHAR_P_STRING_LV|
          CHAR_P_STRING_HP|CHAR_P_STRING_LEARNRIDE);
  */
        } else
          CHAR_talkToCli(charaindex, -1, "荤侩且荐 绝嚼聪促！",
                         CHAR_COLORWHITE);
      }
      CHAR_DelItemMess(charaindex, haveitemindex, 0);
      CHAR_complianceParameter(toindex);
      CHAR_sendCToArroundCharacter(CHAR_getWorkInt(toindex, CHAR_WORKOBJINDEX));
      return;
    } else {
      CHAR_talkToCli(charaindex, -1, "荤侩且荐 绝嚼聪促！", CHAR_COLORWHITE);
    }
  }
#endif
  for (j = 0; j < BD_KIND_END; j++) {
    power[j] = 0;
    recovery[j] = 0;
  }
  if ((p = strstr(arg, "葛电")) != NULL) {
    HealFlg = (1 << BD_KIND_HP);
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    } else {
      HealFlg |= (1 << BD_KIND_MP);
    }
    power[BD_KIND_HP] = 10000000;
    power[BD_KIND_MP] = 100;
  }
  kind = BD_KIND_HP;
  if ((p = strstr(arg, aszKeyString[kind])) != NULL) {
    HealFlg |= (1 << kind);
    if (sscanf(p + 2, "%d", &work) != 1) {
      power[kind] = 1;
    } else {
      power[kind] = RAND((work * 0.9), (work * 1.1));
    }
#ifndef _MAGIC_REHPAI
    power[kind] *= GetRecoveryRate(toindex);
#endif
  }
  kind = BD_KIND_MP;
  if ((p = strstr(arg, aszKeyString[kind])) != NULL &&
      CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    HealFlg |= (1 << kind);
    if (sscanf(p + 2, "%d", &work) != 1) {
      power[kind] = 1;
    } else {
      power[kind] = RAND((work * 0.9), (work * 1.1));
    }
  }
  kind = BD_KIND_AI;
  if ((p = strstr(arg, aszKeyString[kind])) != NULL &&
      CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    HealFlg |= (1 << kind);
    if (sscanf(p + 2, "%d", &work) != 1) {
      power[kind] = 1;
    } else {
      power[kind] = RAND((work * 0.9), (work * 1.1));
    }
    power[kind] *= 100;
  }
  kind = BD_KIND_CHARM;
  if ((p = strstr(arg, aszKeyString[kind])) != NULL &&
      CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    HealFlg |= (1 << kind);
    if (sscanf(p + 2, "%d", &work) != 1) {
      power[kind] = 1;
    } else {
      power[kind] = RAND((work * 0.9), (work * 1.1));
    }
  }
  if (HealFlg == 0)
    return;
#ifdef _TYPE_TOXICATION
  if (CHAR_CanCureFlg(toindex, "HP") == FALSE)
    return;
#endif

  for (j = 0; j < BD_KIND_END; j++) {
    if ((HealFlg & (1 << j)) == 0)
      continue;
    prevhp = CHAR_getInt(toindex, aHealInt[j]);
#ifdef _HP_HEAL
    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      workhp = prevhp + (int)power[j] +
               (((int)power[j] * (CHAR_getInt(toindex, CHAR_VITAL) / 100)) /
                100); //郴备仿 雀汗 眉仿 包拌
    } else {
      workhp =
          prevhp + (int)power[j] +
          (((int)power[j] * (CHAR_getInt(toindex, CHAR_VITAL) / 100)) / 400);
    }
#endif

    if (j == BD_KIND_CHARM) {
      workmax = 100;
      workmin = 0;
    } else if (j == BD_KIND_AI) {
      workmax = 10000;
      workmin = -10000;
    } else {
      workmax = CHAR_getWorkInt(toindex, aHealMaxWork[j]);
      workmin = 1;
    }
    workhp = min(workhp, workmax);
    workhp = max(workhp, workmin);
    CHAR_setInt(toindex, aHealInt[j], workhp);
    recovery[j] = workhp - prevhp;
    if (j == BD_KIND_AI) {
      recovery[j] *= 0.01;
    }
  }
  CHAR_complianceParameter(toindex);

  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (charaindex != toindex) {
      CHAR_send_P_StatusString(toindex, CHAR_P_STRING_HP | CHAR_P_STRING_MP |
                                            CHAR_P_STRING_CHARM);
    }
  }
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_HP | CHAR_P_STRING_MP |
                                           CHAR_P_STRING_CHARM);
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    CHAR_PartyUpdate(toindex, CHAR_N_STRING_HP | CHAR_N_STRING_MP);
  }
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    int i;
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      int workindex = CHAR_getCharPet(charaindex, i);
      if (workindex == toindex) {
        CHAR_send_K_StatusString(charaindex, i,
                                 CHAR_K_STRING_HP | CHAR_K_STRING_AI);
      }
    }
  }

  for (j = 0; j < BD_KIND_END; j++) {
    if ((HealFlg & (1 << j)) == 0)
      continue;
    if (charaindex != toindex) {
      if (power[j] >= 0) {
        snprintf(msgbuf, sizeof(msgbuf), aszHealStringByOwn[j],
                 CHAR_getUseName(toindex), recovery[j]);
      } else {
        snprintf(msgbuf, sizeof(msgbuf), aszDownStringByOwn[j],
                 CHAR_getUseName(toindex), -recovery[j]);
      }
      CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORWHITE);
      if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        if (power[j] >= 0) {
          snprintf(msgbuf, sizeof(msgbuf), aszHealStringByOther[j],
                   CHAR_getUseName(charaindex), CHAR_getUseName(toindex),
                   recovery[j]);
        } else {
          snprintf(msgbuf, sizeof(msgbuf), aszDownStringByOther[j],
                   CHAR_getUseName(charaindex), CHAR_getUseName(toindex),
                   -recovery[j]);
        }
        CHAR_talkToCli(toindex, -1, msgbuf, CHAR_COLORWHITE);
      }
    } else {
      if (power[j] >= 0) {
        snprintf(msgbuf, sizeof(msgbuf), aszHealStringByOwn[j],
                 CHAR_getUseName(charaindex), recovery[j]);
      } else {
        snprintf(msgbuf, sizeof(msgbuf), aszDownStringByOwn[j],
                 CHAR_getUseName(charaindex), -recovery[j]);
      }
      CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORWHITE);
    }
  }
  {
    LogItem(CHAR_getChar(charaindex, CHAR_NAME),
            CHAR_getChar(charaindex, CHAR_CDKEY), itemindex, "FieldUse",
            CHAR_getInt(charaindex, CHAR_FLOOR),
            CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
            ITEM_getChar(itemindex, ITEM_UNIQUECODE),
            ITEM_getChar(itemindex, ITEM_NAME), ITEM_getInt(itemindex, ITEM_ID),
            charaindex);
  }
  CHAR_DelItemMess(charaindex, haveitemindex, 0);
}

#endif

void ITEM_useRecovery(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useRecovery_Battle(charaindex, toindex, haveitemindex);
  } else {
    ITEM_useRecovery_Field(charaindex, toindex, haveitemindex);
  }
}

#ifdef _ITEM_MAGICRECOVERY
void ITEM_useMRecovery(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useMRecovery_Battle(charaindex, toindex, haveitemindex);
  } else {
    //			ITEM_useRecovery_Field(	charaindex, toindex, haveitemindex
    //);
  }
}

#endif

#ifdef _ITEM_USEMAGIC
void ITEM_useMagic(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useMagic_Battle(charaindex, toindex, haveitemindex);
  }
}
#endif

#ifdef _PET_LIMITLEVEL
void ITEM_useOtherEditBase(int charaindex, int toindex, int haveitemindex) {
  int itemindex, i;
  int work[4];
  int num = -1, type;
  int LevelUpPoint, petrank;
  char buf1[256];
  char buf2[][32] = {"腕力成长率", "耐久力成长率", "速度成长率", "体力成长率",
                     "能力"};
  char buf3[][32] = {"大幅提高", "略为提高", "略为减少"};
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
#define RAND(x, y)                                                             \
  ((x - 1) + 1 + (int)((double)(y - (x - 1)) * rand() / (RAND_MAX + 1.0)))
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (CHAR_getInt(toindex, CHAR_PETID) == 718
#ifdef _PET_2LIMITLEVEL
      || CHAR_getInt(toindex, CHAR_PETID) == 401
#endif
  ) {

    int maxnums = 50;

    if (CHAR_getInt(toindex, CHAR_LV) < 74) {
      sprintf(buf1, "给我的吗？好美丽的项链喔！〈能力起了变化〉");
      CHAR_talkToCli(charaindex, toindex, buf1, CHAR_COLORWHITE);
      num = ITEM_MODIFYATTACK;
      LevelUpPoint = CHAR_getInt(toindex, CHAR_ALLOCPOINT);
      petrank = CHAR_getInt(toindex, CHAR_PETRANK);
      work[3] = ((LevelUpPoint >> 24) & 0xFF);
      work[0] = ((LevelUpPoint >> 16) & 0xFF);
      work[1] = ((LevelUpPoint >> 8) & 0xFF);
      work[2] = ((LevelUpPoint >> 0) & 0xFF);
      for (i = 0; i < 4; i++) {
        type = ITEM_getInt(itemindex, (num + i));
        work[i] += type;
        strcpy(buf1, "\0");
        if (work[i] > maxnums) {
          sprintf(buf1, "%s 已经达到最高了。", buf2[i]);
          work[i] = maxnums;
        } else if (work[i] < 0) {
          sprintf(buf1, "%s 已经为零了。", buf2[i]);
          work[i] = 0;
        } else {
          if (type > 0) {
            if (type > 2)
              sprintf(buf1, "%s %s %s", buf2[i], buf3[0], "。");
            else
              sprintf(buf1, "%s %s %s", buf2[i], buf3[1], "。");
          } else if (type < 0) {
            sprintf(buf1, "%s %s %s", buf2[i], buf3[2], "。");
          }
        }
        if (strcmp(buf1, "\0")) {
          CHAR_talkToCli(charaindex, toindex, buf1, CHAR_COLORWHITE);
        }
      }

      LevelUpPoint =
          (work[3] << 24) + (work[0] << 16) + (work[1] << 8) + (work[2] << 0);
      CHAR_setInt(toindex, CHAR_ALLOCPOINT, LevelUpPoint);
      CHAR_setInt(toindex, CHAR_PETRANK, petrank);
      /*
      LogPetPointChange(
              CHAR_getChar( charaindex, CHAR_NAME ),
              CHAR_getChar( charaindex, CHAR_CDKEY ),
              CHAR_getChar( charaindex, CHAR_NAME),
              toindex, 4,
              CHAR_getInt( toindex, CHAR_LV),
              "item_use",
              CHAR_getInt( charaindex, CHAR_FLOOR),
              CHAR_getInt( charaindex, CHAR_X ),
              CHAR_getInt( charaindex, CHAR_Y )
              );*/
    }

  } else {
    sprintf(buf1, "？？？");
  }
  CHAR_DelItem(charaindex, haveitemindex);
  return;
}
#endif

void ITEM_useStatusChange(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return; //｛撩
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useStatusChange_Battle(charaindex, toindex, haveitemindex);
  } else {
  }
}

void ITEM_useStatusRecovery(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return; //｛撩
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useStatusRecovery_Battle(charaindex, toindex, haveitemindex);
  } else {
  }
}

void ITEM_useMagicDef(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useMagicDef_Battle(charaindex, toindex, haveitemindex);
  } else {
  }
}

void ITEM_useParamChange(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useParamChange_Battle(charaindex, toindex, haveitemindex);
  }
}

void ITEM_useFieldChange(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useFieldChange_Battle(charaindex, toindex, haveitemindex);
  }
}

void ITEM_useAttReverse(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useAttReverse_Battle(charaindex, toindex, haveitemindex);
  } else {
  }
}

void ITEM_useMic(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
  } else {
    ITEM_useMic_Field(charaindex, toindex, haveitemindex);
  }
}

void ITEM_useCaptureUp(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return; //｛撩

  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
    ITEM_useCaptureUp_Battle(charaindex, toindex, haveitemindex);
  } else {
  }
}

#ifdef _PETSKILL_CANNEDFOOD
static void ITEM_usePetSkillCanned_PrintWindow(int charaindex, int flg) {
  int fd;
  char message[256], buf[2048];
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  fd = getfdFromCharaIndex(charaindex);
  if (fd == -1)
    return;

  sprintf(message, "%d", flg);
  lssproto_WN_send(fd, WINDOWS_MESSAGETYPE_PETSKILLSHOW, WINDOW_BUTTONTYPE_NONE,
                   ITEM_WINDOWTYPE_SELECTPETSKILL_SELECT, -1,
                   makeEscapeString(message, buf, sizeof(buf)));
}

void ITEM_usePetSkillCanned_WindowResult(int charaindex, int seqno, int select,
                                         char *data) {
  int itemindex = -1, itemNo, petindex = -1, petNo;
  int SkillNo, SkillID;
  char buf1[256];
  char *skillarg = NULL;

  petNo = CHAR_getWorkInt(charaindex, CHAR_WORKRENAMEITEMNUM);
  itemNo = CHAR_getWorkInt(charaindex, CHAR_WORKRENAMEITEMINDEX);
  itemindex = CHAR_getItemIndex(charaindex, itemNo);

  int haveitemindex;
  haveitemindex = CHAR_getWorkInt(charaindex, CHAR_WORKHAVEITEMINDEX);
  if (itemindex != haveitemindex)
    return;

  if (!ITEM_CHECKINDEX(itemindex))
    return;
  petindex = CHAR_getCharPet(charaindex, petNo);
  if (!CHAR_CHECKINDEX(petindex))
    return;
  SkillNo = atoi(data);
  if (SkillNo < 0 || SkillNo >= CHAR_MAXPETSKILLHAVE)
    return;

  if (strcmp(ITEM_getChar(itemindex, ITEM_USEFUNC), "ITEM_useSkillCanned")) {
    CHAR_talkToCli(charaindex, -1, "禁止非法学习宠物技能！", CHAR_COLORRED);
    return;
  }

  skillarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  SkillID = atoi(skillarg);

#ifdef _PETSKILL_SHOP_LUA
  if (FreePetSkillShop(charaindex, petindex,
                       CHAR_getPetSkill(petindex, SkillNo), SkillID) == FALSE) {
    return;
  }
#endif
  /*
  #ifdef _CFREE_petskill
          if( NPC_CHECKFREEPETSKILL( charaindex, petindex, SkillID) == FALSE ){
                  CHAR_talkToCli( charaindex, -1, "该宠物无法学习此项技能！",
  CHAR_COLORYELLOW); return;
          }
  #endif
  */
  CHAR_setPetSkill(petindex, SkillNo, SkillID);
  snprintf(buf1, sizeof(buf1), "W%d", petNo);
  CHAR_sendStatusString(charaindex, buf1);
  CHAR_sendStatusString(charaindex, "P");
  {
    int skillarray = PETSKILL_getPetskillArray(SkillID);
    sprintf(buf1, "[%s](捞)啊 [%s]胶懦阑 硅奎绢夸，%s (篮)绰 家戈沁绢夸.",
            CHAR_getUseName(petindex),
            PETSKILL_getChar(skillarray, PETSKILL_NAME),
            ITEM_getChar(itemindex, ITEM_NAME));
  }

  CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
  CHAR_setItemIndex(charaindex, itemNo, -1);
  CHAR_sendItemDataOne(charaindex, itemNo);
  ITEM_endExistItemsOne(itemindex);
}

#endif

static void ITEM_useRenameItem_PrintWindow(int charaindex, int page) {
  int fd;
  int pos = 0;
  int i;
  int btntype = WINDOW_BUTTONTYPE_CANCEL;
  char message[1024];
  char msgwk[1024];
  char buf[2048];

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (page < 0 || page > 3) {
    print("%s:%d err\n", __FILE__, __LINE__);
    return;
  }
  fd = getfdFromCharaIndex(charaindex);
  if (fd == -1)
    return;

  snprintf(message, sizeof(message),
           "2\n   函版窍妨绰 酒捞袍阑 急琶窍绞矫坷\n"
           "                               其捞瘤:%d\n",
           page + 1);

  for (i = page * 5; i < page * 5 + 5; i++) {
    int itemindex = CHAR_getItemIndex(charaindex, i);
    BOOL flg = FALSE;
    while (1) {
      char *cdkey;
      if (!ITEM_CHECKINDEX(itemindex))
        break;
      cdkey = ITEM_getChar(itemindex, ITEM_CDKEY);
      if (!cdkey) {
        print("%s:%d err\n", __FILE__, __LINE__);
        break;
      }
      if (ITEM_getInt(itemindex, ITEM_MERGEFLG) != 1)
        break;
      if (ITEM_getInt(itemindex, ITEM_TYPE) == ITEM_DISH)
        break;
      if (ITEM_getInt(itemindex, ITEM_CRUSHLEVEL) != 0)
        break;
      if (strlen(cdkey) != 0) {
        if (strcmp(cdkey, CHAR_getChar(charaindex, CHAR_CDKEY)) != 0) {
          break;
        }
      }
      flg = TRUE;
      break;
    }
    if (flg) {
      char *nm = ITEM_getChar(itemindex, ITEM_SECRETNAME);
      char wk[256];
      if (pos + strlen(nm) + 1 > sizeof(msgwk)) {
        print("buffer over error %s:%d\n", __FILE__, __LINE__);
        break;
      }
      snprintf(wk, sizeof(wk), "%s\n", nm);
      strncpy(&msgwk[pos], wk, sizeof(msgwk) - pos - 1);
      pos += strlen(wk);
    } else {
      if (pos + 2 > sizeof(msgwk)) {
        print("buffer over error %s:%d\n", __FILE__, __LINE__);
        break;
      }
      strncpy(&msgwk[pos], "\n", sizeof(msgwk) - pos - 1);
      pos += 1;
    }
  }
  strcat(message, msgwk);
  switch (page) {
  case 0:
    btntype |= WINDOW_BUTTONTYPE_NEXT;
    break;
  case 3:
    btntype |= WINDOW_BUTTONTYPE_PREV;
    break;
  case 1:
  case 2:
    btntype |= WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;
    break;
  }
  lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, btntype,
                   CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1 + page, -1,
                   makeEscapeString(message, buf, sizeof(buf)));
}

void ITEM_useRenameItem(int charaindex, int toindex, int haveitemindex) {

  ITEM_useRenameItem_PrintWindow(charaindex, 0);

  CHAR_setWorkInt(charaindex, CHAR_WORKRENAMEITEMNUM, -1);
  CHAR_setWorkInt(charaindex, CHAR_WORKRENAMEITEMINDEX, haveitemindex);
  /*
          char buf[256];
          int itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
          if( !ITEM_CHECKINDEX( itemindex) ) return;
          sprintf( buf, "%s功能已被取消。", ITEM_getChar( itemindex,
     ITEM_NAME)); CHAR_talkToCli( charaindex, -1, "功能已被取消。",
     CHAR_COLORRED );

          {
                  LogItem(
                          CHAR_getChar( charaindex, CHAR_NAME ),
                          CHAR_getChar( charaindex, CHAR_CDKEY ),
                          itemindex,
                          "魔术笔DEL",
                          CHAR_getInt( charaindex, CHAR_FLOOR),
                          CHAR_getInt( charaindex, CHAR_X ),
              CHAR_getInt( charaindex, CHAR_Y ),
              ITEM_getChar( itemindex, ITEM_UNIQUECODE),
                          ITEM_getChar( itemindex, ITEM_NAME),
                          ITEM_getInt( itemindex, ITEM_ID),
                          charaindex
                  );
          }

          CHAR_setItemIndex( charaindex, haveitemindex, -1);
          CHAR_sendItemDataOne( charaindex, haveitemindex);
          ITEM_endExistItemsOne( itemindex );
  */
}

void ITEM_useRenameItem_WindowResult(int charaindex, int seqno, int select,
                                     char *data) {
  int page = 0;
  int fd;
  if (select == WINDOW_BUTTONTYPE_CANCEL)
    return;

  fd = getfdFromCharaIndex(charaindex);
  if (fd == -1)
    return;
  if (seqno != CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME) {
    if (select == WINDOW_BUTTONTYPE_NEXT)
      page = 1;
    else if (select == WINDOW_BUTTONTYPE_PREV)
      page = -1;
    if (select == WINDOW_BUTTONTYPE_NEXT || select == WINDOW_BUTTONTYPE_PREV) {
      int winno = seqno - CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1;
      winno += page;
      if (winno < 0)
        winno = 0;
      if (winno > 3)
        winno = 3;
      ITEM_useRenameItem_PrintWindow(charaindex, winno);
    } else {
      char message[1024];
      char buf[2048];
      char haveitemindex = CHAR_getWorkInt(charaindex, CHAR_WORKRENAMEITEMNUM);
      int itemindex;
      if (haveitemindex == -1) {
        haveitemindex = (seqno - CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1) * 5 +
                        (atoi(data) - 1);
        CHAR_setWorkInt(charaindex, CHAR_WORKRENAMEITEMNUM, haveitemindex);
      }
      itemindex = CHAR_getItemIndex(charaindex, haveitemindex);

      snprintf(message, sizeof(message),
               "%s 酒捞袍狼 函版且\n"
               "捞抚阑 涝仿秦林绞矫坷.\n"
               "13磊 捞郴肺 涝仿窍技夸.",
               ITEM_getChar(itemindex, ITEM_NAME));

      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,
                       WINDOW_BUTTONTYPE_OKCANCEL,
                       CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME, -1,
                       makeEscapeString(message, buf, sizeof(buf)));
    }
  } else {
    BOOL flg = FALSE;
    char message[1024];
    char buf[2048];
    while (1) {
      char *p;
      if (strlen(data) > 26 || strlen(data) < 1) {
        if (strlen(data) > 26) {
          strcpy(message, "13磊 捞郴肺 涝仿窍技夸！");
        } else {
          strcpy(message, "巩磊甫 涝仿秦林技夸！");
        }
        break;
      }

      // WON ADD 修正魔术笔改名问题
      flg = TRUE;

      for (p = data; *p; p++) {
        if (*p == ' ') {
          strcpy(message, "傍归篮 荤侩且荐 绝嚼聪促！");
          flg = FALSE;
          break;
        }
        if (strncmp(p, "　", 2) == 0) {
          strcpy(message, "傍归篮 荤侩且荐 绝嚼聪促！");
          flg = FALSE;
          break;
        }
        if (*p == '|') {
          strcpy(message, "巩磊绰 荤侩且荐 绝嚼聪促！");
          flg = FALSE;
          break;
        }
      }

      break;
    }

    if (!flg) {
      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                       CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME_ATTENTION, -1,
                       makeEscapeString(message, buf, sizeof(buf)));
    } else {
      char haveitemindex = CHAR_getWorkInt(charaindex, CHAR_WORKRENAMEITEMNUM);
      int itemindex;
      int renameitemindex;
      int renameitemhaveindex;
      int remain;
      char msgbuf[128];

      itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
      if (!ITEM_CHECKINDEX(itemindex)) {
        print("%s %d err\n", __FILE__, __LINE__);
        return;
      }
      ITEM_setChar(itemindex, ITEM_SECRETNAME, data);
#ifndef _PET_AND_ITEM_UP
      ITEM_setChar(itemindex, ITEM_CDKEY, CHAR_getChar(charaindex, CHAR_CDKEY));
#endif
      CHAR_sendItemDataOne(charaindex, haveitemindex);
      snprintf(msgbuf, sizeof(msgbuf), "%s(捞)啊 %s栏肺 函版登菌嚼聪促！",
               ITEM_getChar(itemindex, ITEM_NAME), data);
      CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORYELLOW);
      renameitemhaveindex =
          CHAR_getWorkInt(charaindex, CHAR_WORKRENAMEITEMINDEX);
      renameitemindex = CHAR_getItemIndex(charaindex, renameitemhaveindex);
      if (!ITEM_CHECKINDEX(renameitemindex)) {
        print("%s %d err\n", __FILE__, __LINE__);
        return;
      }
      remain = atoi(ITEM_getChar(renameitemindex, ITEM_ARGUMENT));
      if (remain != 0) {
        remain--;
        if (remain <= 0) {
          snprintf(msgbuf, sizeof(msgbuf), "%s(阑)甫 荤侩窍看嚼聪促！",
                   ITEM_getChar(renameitemindex, ITEM_NAME));
          CHAR_talkToCli(charaindex, -1, msgbuf, CHAR_COLORYELLOW);
          CHAR_setItemIndex(charaindex, renameitemhaveindex, -1);
          CHAR_sendItemDataOne(charaindex, renameitemhaveindex);
          ITEM_endExistItemsOne(renameitemindex);
        } else {
          char buf[32];
          snprintf(buf, sizeof(buf), "%d", remain);
          ITEM_setChar(renameitemindex, ITEM_ARGUMENT, buf);
        }
      }
    }
  }
}

//-------------------------------------------------------------------------
//	今中仇欠毛  蜇允月楮醒［
//	  午仄凶凛卞］仿件母丞卞ㄠ  ㄥ毛蓟太］  蟆午  飓  寞毛  凳允月［
//-------------------------------------------------------------------------
void ITEM_dropDice(int charaindex, int itemindex) {
  char *dicename[] = {"窍唱", "笛", "悸", "齿", "促几", "咯几"};
  int diceimagenumber[] = {24298, 24299, 24300, 24301, 24302, 24303};
  int r = RAND(0, 5);

  //   飓  寞毛谨
  ITEM_setInt(itemindex, ITEM_VAR1,
              ITEM_getInt(itemindex, ITEM_BASEIMAGENUMBER));
  //   飓  寞凳蕙
  ITEM_setInt(itemindex, ITEM_BASEIMAGENUMBER, diceimagenumber[r]);
  //   蟆凳蕙
  ITEM_setChar(itemindex, ITEM_SECRETNAME, dicename[r]);

  // 犯□正及弁仿奶失件玄尺及霜耨反晓匏楮醒匹垫丹及匹］仇仇匹反支日卅中［
}
//-------------------------------------------------------------------------
//	今中仇欠毛  蜇允月楮醒［
//  胶匀凶凛卞］  蟆午  飓  寞毛葭卞  允［
//-------------------------------------------------------------------------
void ITEM_pickupDice(int charaindex, int itemindex) {
  //   飓  寞毛葭卞  允［
  ITEM_setInt(itemindex, ITEM_BASEIMAGENUMBER,
              ITEM_getInt(itemindex, ITEM_VAR1));
  //   蟆手葭卞  允
  ITEM_setChar(itemindex, ITEM_SECRETNAME, ITEM_getChar(itemindex, ITEM_NAME));
}
enum {
  ITEM_LOTTERY_1ST, // 1羁
  ITEM_LOTTERY_2ND,
  ITEM_LOTTERY_3RD,
  ITEM_LOTTERY_4TH,
  ITEM_LOTTERY_5TH,  // 5羁
  ITEM_LOTTERY_6TH,  // 6羁
  ITEM_LOTTERY_NONE, // 陆木
  ITEM_LOTTERY_NUM,
};
//-------------------------------------------------------------------------
//	旦疋□玉仁元互综日木月凛及楮醒［
//  仇仇匹窒羁操卞卅月井毛瑁户月［
//  割  反动票及骚曰［

#define PRE_6 (10000)
#define PRE_5 (1300 + PRE_6)
#define PRE_4 (600 + PRE_5)
#define PRE_3 (300 + PRE_4)
#define PRE_2 (8 + PRE_3)
#define PRE_1 (1 + PRE_2)

//#define PRE_5		(13000)
//#define PRE_4		(2300 + PRE_5)
//#define PRE_3		(540 + PRE_4)
//#define PRE_2		(8 + PRE_3)
//#define PRE_1		(1 + PRE_2)

//#define PRE_5		(16000)
//#define PRE_4		(2500 + PRE_5)
//#define PRE_3		(400 + PRE_4)
//#define PRE_2		(10 + PRE_3)
//#define PRE_1		(1 + PRE_2)
//#define PRE_5		(27000)
//#define PRE_4		(4000 + PRE_5)
//#define PRE_3		(400 + PRE_4)
//#define PRE_2		(10 + PRE_3)
//#define PRE_1		(1 + PRE_2)
//
//-------------------------------------------------------------------------
BOOL ITEM_initLottery(ITEM_Item *itm) {
  int r = RAND(0, 49999);
  int hit = ITEM_LOTTERY_NONE; //
  char result[7];              // 请
  int countnum[6];
  int count;
  int i;
  int len;
  // 域荚综匀化月及匹［疯粟  仄凶凛午井卞综曰卅云今木卅中方丹卞［
  if (itm->data[ITEM_VAR3] == 1)
    return TRUE;

  // 铲蓟允月［
  if (r < PRE_6)
    hit = ITEM_LOTTERY_6TH;
  else if (r < PRE_5)
    hit = ITEM_LOTTERY_5TH;
  else if (r < PRE_4)
    hit = ITEM_LOTTERY_4TH;
  else if (r < PRE_3)
    hit = ITEM_LOTTERY_3RD;
  else if (r < PRE_2)
    hit = ITEM_LOTTERY_2ND;
  else if (r < PRE_1)
    hit = ITEM_LOTTERY_1ST;
  else
    hit = ITEM_LOTTERY_NONE;

  // 请  毛瑁烂允月［
  count = 0;
  if (hit != ITEM_LOTTERY_NONE) {
    // 癫曰毛本永玄
    result[0] = result[1] = result[2] = hit + 1;
    count = 3;
    countnum[hit] = 3;
  }
  while (count < 6) {
    int r = RAND(ITEM_LOTTERY_1ST, ITEM_LOTTERY_6TH);
    if (countnum[r] >= 2)
      continue;
    // 2/3仁日中及割  匹］职及陆木钓  毛伉□民今六化支月［
    // 升五升五［
    if ((hit != ITEM_LOTTERY_NONE && count == 3) ||
        (hit == ITEM_LOTTERY_NONE && count == 0)) {
      if (RAND(0, 2)) {
        result[count] = result[count + 1] = r + 1;
        countnum[r] += 2;
        count += 2;
        continue;
      }
    }
    countnum[r]++;
    result[count] = r + 1;
    count++;
  }
  // 请  毛扑乓永白伙允月［
  // 癫曰及凛反2/3及割  匹］癫曰醒侬毛域    欠卞裔烂允月［
  // 玉平玉平躲绊毛谎丹啃［
  len = sizeof(result) - 2;
  if (hit != ITEM_LOTTERY_NONE) {
    if (RAND(0, 2)) {
      // 癫曰醒侬毛域    欠卞［
      char s = result[0];
      result[0] = result[5];
      result[5] = s;
      len = sizeof(result) - 3;
    }
  }
  for (i = 0; i < 10; i++) {
    int x = RAND(0, len);
    int y = RAND(0, len);
    int s;
    s = result[x];
    result[x] = result[y];
    result[y] = s;
  }
  //   及凶户］  侬  健中卞仄化云仁［
  result[sizeof(result) - 1] = '\0';
  itm->data[ITEM_VAR1] = hit;
  // 户仁匀凶市它件玄
  itm->data[ITEM_VAR2] = 0;
  itm->data[ITEM_VAR3] = 1;
  memcpy(itm->string[ITEM_ARGUMENT].string, result, sizeof(result));

  return TRUE;
}
//-------------------------------------------------------------------------
//	旦疋□玉仁元毛银匀凶凛及楮醒［
//  ㄥ荚银丹午］窒羁操井及失奶  丞卞  祭允月［
//-------------------------------------------------------------------------
void ITEM_useLottery(int charaindex, int toindex, int haveitemindex) {
  int i, j;
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  int count = ITEM_getInt(itemindex, ITEM_VAR2);
  int hit = ITEM_getInt(itemindex, ITEM_VAR1);
  char buff[1024];
  char num[6][3] = {{"A"}, {"B"}, {"C"}, {"D"}, {"E"}, {"F"}};
  char numbuff[128];
  char *n;
  int result;
  BOOL flg;
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (count == 0) {
    ITEM_setChar(itemindex, ITEM_EFFECTSTRING, "");
  } else if (count == 6) {
    CHAR_setItemIndex(charaindex, haveitemindex, -1);
    CHAR_sendItemDataOne(charaindex, haveitemindex);
    ITEM_endExistItemsOne(itemindex);
    return;
  }
  n = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  result = (int)n[count] - 1;

  flg = FALSE;
  for (i = 0; i < count + 1 && flg == FALSE; i++) {
    for (j = i + 1; j < count + 1 && flg == FALSE; j++) {
      if (i != j) {
        if (n[i] == n[j]) {
          flg = TRUE;
        }
      }
    }
  }
  memcpy(numbuff, ITEM_getChar(itemindex, ITEM_EFFECTSTRING), (count)*2);
  snprintf(buff, sizeof(buff), "%s%s", numbuff, num[result]);
  count++;
  ITEM_setInt(itemindex, ITEM_VAR2, count);
  if (count >= 6) {
    if (hit != ITEM_LOTTERY_NONE) {
      int newitemindex;
      char strbuff[1024];
      char msgbuff[1024];
      CHAR_setItemIndex(charaindex, haveitemindex, -1);
      ITEM_endExistItemsOne(itemindex);
      newitemindex = ITEM_makeItemAndRegist(2729 + hit);
      CHAR_setItemIndex(charaindex, haveitemindex, newitemindex);
      snprintf(strbuff, sizeof(strbuff), "%s                %s", buff,
               ITEM_getChar(newitemindex, ITEM_EFFECTSTRING));
      ITEM_setChar(newitemindex, ITEM_EFFECTSTRING, strbuff);
      CHAR_sendItemDataOne(charaindex, haveitemindex);
      snprintf(msgbuff, sizeof(msgbuff), "中了第%d奖", hit + 1);
      CHAR_talkToCli(charaindex, -1, msgbuff, CHAR_COLORYELLOW);
    } else {
      //            CHAR_setItemIndex( charaindex , haveitemindex, -1 );
      char strbuff[1024];
      snprintf(strbuff, sizeof(strbuff),
               "%s                       没中,下次再来", buff);
      ITEM_setChar(itemindex, ITEM_EFFECTSTRING, strbuff);
      CHAR_sendItemDataOne(charaindex, haveitemindex);
      //            ITEM_endExistItemsOne( itemindex );
      //            CHAR_talkToCli( charaindex, -1,
      //                            "反内木″",
      //                            CHAR_COLORWHITE );
    }
  } else {
    char strbuff[1024];
    if (flg) {
      int spc = 16 + (6 - count) * 2;
      char space[17];
      space[spc] = '\0';
      snprintf(strbuff, sizeof(strbuff), "%s%s快中了", buff, space);
    } else {
      strcpy(strbuff, buff);
    }
    ITEM_setChar(itemindex, ITEM_EFFECTSTRING, strbuff);
    CHAR_sendItemDataOne(charaindex, haveitemindex);
  }
}

void ITEM_WarpDelErrorItem(int charaindex) {
  int j;
  for (j = 0; j < CHAR_getMaxInventory(charaindex); j++) {
    int itemindex = CHAR_getItemIndex(charaindex, j);
    if (ITEM_CHECKINDEX(itemindex)) {
      int id = ITEM_getInt(itemindex, ITEM_ID);
      if (id == 2609 || id == 2704) {
        CHAR_setItemIndex(charaindex, j, -1);
        ITEM_endExistItemsOne(itemindex);
        CHAR_sendItemDataOne(charaindex, j);
      }
    }
  }
#ifdef _ITEM_WARP_FIX_BI
  if (!CHAR_getWorkInt(charaindex, CHAR_WORKITEMMETAMO))
    recoverbi(charaindex);
#endif
}

BOOL ITEM_WarpForAny(int charaindex, int ff, int fx, int fy, int flg) {
  if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return FALSE;
  if (CHAR_getInt(charaindex, CHAR_FLOOR) == 117
#ifdef _ADD_DUNGEON //追加地牢
      || CHAR_getInt(charaindex, CHAR_FLOOR) == 8513
#endif
  ) {
    CHAR_talkToCli(charaindex, -1, "此处无法使用。", CHAR_COLORYELLOW);
    return FALSE;
  }

  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    int i;
    if (flg == 0) { //单体
      CHAR_talkToCli(charaindex, -1, "只能单人使用。", CHAR_COLORYELLOW);
      return FALSE;
    }
    for (i = 0; i < CHAR_PARTYMAX; i++) {
      int subindex = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1 + i);
      if (CHAR_CHECKINDEX(subindex) == FALSE)
        continue;
      CHAR_talkToCli(subindex, -1, "全体瞬间飞行．．．", CHAR_COLORWHITE);
      ITEM_WarpDelErrorItem(subindex);
      CHAR_warpToSpecificPoint(subindex, ff, fx, fy);
    }
  } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
             CHAR_PARTY_CLIENT) {
    CHAR_talkToCli(charaindex, -1, "队员无法使用。", CHAR_COLORYELLOW);
    return FALSE;
  } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
             CHAR_PARTY_NONE) {
    ITEM_WarpDelErrorItem(charaindex);
    CHAR_warpToSpecificPoint(charaindex, ff, fx, fy);
  }
  return TRUE;
}
// andy_end

// Robin 0523
void ITEM_useWarp(int charaindex, int toindex, int haveitemindex) {
  char *arg;
  int itemindex, warp_t, warp_fl, warp_x, warp_y;
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

#ifdef _ITEM_CHECKWARES
  if (CHAR_CheckInItemForWares(charaindex, 0) == FALSE) {
    CHAR_talkToCli(charaindex, -1, "携带货物无法使用。", CHAR_COLORYELLOW);
    return;
  }
#endif
  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (arg == "\0")
    return;
  if (sscanf(arg, "%d %d %d %d", &warp_t, &warp_fl, &warp_x, &warp_y) != 4)
    return;
  if (ITEM_WarpForAny(charaindex, warp_fl, warp_x, warp_y, warp_t) == FALSE)
    return;

  CHAR_DelItem(charaindex, haveitemindex);
  CHAR_sendStatusString(charaindex, "P");
}

#ifdef _USEWARP_FORNUM
void ITEM_useWarpForNum(int charaindex, int toindex, int haveitemindex) {
  char *arg;
  int flg, ff, fx, fy, itemindex, usenum = 0, i;
  int Mf, Mx, My;
  int MapPoint[12] = {100, 200, 300, 400, 700, 701,
                      702, 703, 704, 705, 707, 708};
  char MapString[12][256] = {"萨伊那斯",     "加鲁卡",       "吉鲁岛",
                             "沙姆岛",       "伊甸园区",     "伊甸大陆西方",
                             "尼克斯城区",   "伊甸大陆南方", "亚伊欧城区",
                             "伊甸大陆东方", "伊甸大陆北方", "塔耳塔城区"};

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (arg == "\0")
    return;

  if (sscanf(arg, "%d %d %d %d", &flg, &ff, &fx, &fy) != 4)
    return;
  Mf = CHAR_getInt(charaindex, CHAR_FLOOR);
  Mx = CHAR_getInt(charaindex, CHAR_X);
  My = CHAR_getInt(charaindex, CHAR_Y);

  usenum = ITEM_getInt(itemindex, ITEM_DAMAGEBREAK);
  for (i = 0; i < 12; i++) {
    if (Mf == MapPoint[i]) {
      break;
    }
  }
#ifdef _ITEM_CHECKWARES
  if (CHAR_CheckInItemForWares(charaindex, 0) == FALSE) {
    CHAR_talkToCli(charaindex, -1, "携带货物无法使用。", CHAR_COLORYELLOW);
    return;
  }
#endif
  if (--usenum <= 0) {
    CHAR_DelItem(charaindex, haveitemindex);
    CHAR_sendStatusString(charaindex, "P");
  } else {
    char buf[256];
    // WON ADD
    if (i >= 12) {
      CHAR_talkToCli(charaindex, -1, "此处无法使用。", CHAR_COLORYELLOW);
      return;
    }
    sprintf(buf, "%d %d %d %d", flg, Mf, Mx, My);
    ITEM_setChar(itemindex, ITEM_ARGUMENT, buf);
    ITEM_setInt(itemindex, ITEM_DAMAGEBREAK, usenum);
    sprintf(buf, "飞行纪录点(%s,%d,%d)", MapString[i], Mx, My);
    ITEM_setChar(itemindex, ITEM_EFFECTSTRING, buf);
    CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
    CHAR_sendItemDataOne(charaindex, haveitemindex);
  }

  // WON ADD
  if (ITEM_WarpForAny(charaindex, ff, fx, fy, flg) == FALSE)
    return;
}
#endif

// Robin 0707 petFollow
void ITEM_petFollow(int charaindex, int toindex, int haveitemindex) {
  char *arg;
  int itemindex, followLv, haveindex, i;
  // print(" PetFollow_toindex:%d ", toindex);
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPETFOLLOW) != -1) {
    if (CHAR_CHECKINDEX(CHAR_getWorkInt(charaindex, CHAR_WORKPETFOLLOW))) {
      CHAR_talkToCli(charaindex, -1, "请先收回放出的宠物！", CHAR_COLORWHITE);
      return;
    }
    CHAR_setWorkInt(charaindex, CHAR_WORKPETFOLLOW, -1);
  }
#ifdef _FIX_METAMORIDE
  if (CHAR_CHECKJOINENEMY(charaindex) == TRUE) {
    CHAR_talkToCli(charaindex, -1, "搭乘中无法使用！", CHAR_COLORWHITE);
    return;
  }
#endif

  if (CHAR_CHECKINDEX(toindex) == FALSE)
    return;
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (arg == "\0")
    return;
  if (arg == NULL)
    return;

  if (sscanf(arg, "%d", &followLv) != 1)
    return;
  if (CHAR_getInt(toindex, CHAR_LV) > followLv) {
    CHAR_talkToCli(charaindex, -1, "道具的等级不足！", CHAR_COLORWHITE);
    return;
  }
  if (CHAR_getWorkInt(toindex, CHAR_WORKFIXAI) < 80) {
    // CHAR_talkToCli( charaindex, -1, "宠物的忠诚度不足！", CHAR_COLORWHITE );
    // return;
  }

  haveindex = -1;
  for (i = 0; i < 5; i++) {
    if (CHAR_getCharPet(charaindex, i) == toindex) {
      haveindex = i;
      break;
    }
  }
  if (haveindex == -1)
    return;

  if (!PET_dropPetFollow(charaindex, haveindex, -1, -1, -1)) {
    // CHAR_talkToCli( charaindex, -1, "宠物跟随失败！", CHAR_COLORWHITE );
    return;
  }
}

// Nuke start 0624: Hero's bless
void ITEM_useSkup(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
#ifdef _SUPER
  if (CHAR_getInt(charaindex, CHAR_SUPER) >= 1) {
    CHAR_talkToCli(charaindex, -1, "已经是圆满人。不可以再吃祝福。",
                   CHAR_COLORYELLOW);
    return;
  }
#endif
  //计算未升级完点数
  int trn = CHAR_getInt(charaindex, CHAR_TRANSMIGRATION); //获取转生
  int lv = CHAR_getInt(charaindex, CHAR_LV);              //获取等级
  int maxlv = 140;
  if (trn >= getChartrans()) {
    maxlv = getMaxLevel();
  } else {
    maxlv = getYBLevel();
  }
  int point;
#ifdef _SKILLUPPOINT_CF
  point = (maxlv - CHAR_getInt(charaindex, CHAR_LV)) * getSkup();
#else
  point = (maxlv - CHAR_getInt(charaindex, CHAR_LV)) * 3;
#endif
  int vi = CHAR_getInt(charaindex, CHAR_VITAL);
  int str = CHAR_getInt(charaindex, CHAR_STR);
  int tou = CHAR_getInt(charaindex, CHAR_TOUGH);
  int dx = CHAR_getInt(charaindex, CHAR_DEX);
  int skup = CHAR_getInt(charaindex, CHAR_SKILLUPPOINT);
  int total = (vi + str + tou + dx) / 100 + skup + point;
  float table[] = {437, 480, 510, 537, 565, 595, 627}; //各转最高点数(减10)
  int getpoint = (trn + 1) * 10;                       //计算可超越点数
  if (getpoint > 30) {
    getpoint = 30;
  }
  if (trn == 6) {
    getpoint = getpoint + 5;
  }

  int maxpoint = table[trn] + getpoint;
  if (total > maxpoint) {
    CHAR_talkToCli(charaindex, -1, "使用后超过点数上限，无法使用祝福！",
                   CHAR_COLORYELLOW);
    return;
  }

  CHAR_setInt(charaindex, CHAR_SKILLUPPOINT,
              CHAR_getInt(charaindex, CHAR_SKILLUPPOINT) + 1);
  CHAR_Skillupsend(charaindex);
  CHAR_talkToCli(charaindex, -1, "你感受到自己的能力被提升了。",
                 CHAR_COLORWHITE);

  CHAR_DelItem(charaindex, haveitemindex);
}
// Nuke end
extern void setNoenemy();
// Nuke start 0626: Dragon's bless
void ITEM_useNoenemy(int charaindex, int toindex, int haveitemindex) {
  int itemindex, fd;
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  setNoenemy(fd);
  CHAR_talkToCli(charaindex, -1, "你感受到周边的杀气消失了。", CHAR_COLORWHITE);
  CHAR_DelItem(charaindex, haveitemindex);
}
// Nuke end

// Arminius 7.2: Ra's amulet
void ITEM_equipNoenemy(int charaindex, int itemindex) {
  char buf[4096];
  int evadelevel;
  int fl, fd;

  if (ITEM_CHECKINDEX(itemindex) == FALSE)
    return;

  if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "noen", buf,
                       sizeof(buf)) == FALSE) {
    return;
  }
  evadelevel = atoi(buf);
  fl = CHAR_getInt(charaindex, CHAR_FLOOR);
  fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  if (evadelevel >= 200) {
    setEqNoenemy(fd, 200);
    CHAR_talkToCli(charaindex, -1, "一道奇异的光芒隐藏了你的行踪。",
                   CHAR_COLORWHITE);
    return;
  } else if (evadelevel >= 120) {
    setEqNoenemy(fd, 120);

    if ((fl == 100) || (fl == 200) || (fl == 300) || (fl == 400) ||
        (fl == 500)) {
      CHAR_talkToCli(charaindex, -1, "一道奇异的光芒隐藏了你的行踪。",
                     CHAR_COLORWHITE);
      return;
    }
  } else if (evadelevel >= 80) {
    setEqNoenemy(fd, 80);
    if ((fl == 100) || (fl == 200) || (fl == 300) || (fl == 400)) {

      CHAR_talkToCli(charaindex, -1, "一道奇异的光芒隐藏了你的行踪。",
                     CHAR_COLORWHITE);
      return;
    }
  } else if (evadelevel >= 40) {
    setEqNoenemy(fd, 40);
    if ((fl == 100) || (fl == 200)) {
      CHAR_talkToCli(charaindex, -1, "一道奇异的光芒隐藏了你的行踪。",
                     CHAR_COLORWHITE);
      return;
    }
  }
  CHAR_talkToCli(charaindex, -1, "什麽事也没有发生。", CHAR_COLORWHITE);
}

#ifdef _Item_MoonAct
void ITEM_randEnemyEquipOne(int charaindex, int toindex, int haveitemindex) {
  int itemindex, RandNum = 0;
  char buf[256];

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "rand", buf,
                       sizeof(buf)) == FALSE) {
    return;
  }

  if ((RandNum = atoi(buf)) > 0) {
    int fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
    setEqRandenemy(fd, RandNum);
    CHAR_talkToCli(charaindex, -1, "遇敌率降低了。", CHAR_COLORWHITE);
    sprintf(buf, "道具 %s消失了。", ITEM_getChar(itemindex, ITEM_NAME));
    CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
    CHAR_DelItem(charaindex, haveitemindex);
    return;
  }
}

void ITEM_randEnemyEquip(int charaindex, int itemindex) {
  char buf[4096];
  int RandNum = 0;
  int fd;

  if (ITEM_CHECKINDEX(itemindex) == FALSE)
    return;
  if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "rand", buf,
                       sizeof(buf)) == FALSE) {
    return;
  }

  RandNum = atoi(buf);
  fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  if (RandNum > 0) {
    setEqRandenemy(fd, RandNum);
    CHAR_talkToCli(charaindex, -1, "遇敌率降低了。", CHAR_COLORWHITE);
    return;
  }
}
void ITEM_RerandEnemyEquip(int charaindex, int itemindex) {
  int RandNum = 0;
  int fd;

  if (ITEM_CHECKINDEX(itemindex) == FALSE)
    return;

  fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  RandNum = getEqRandenemy(fd);

  if (RandNum > 0) {
    clearEqRandenemy(fd);
    CHAR_talkToCli(charaindex, -1, "遇敌率回复。", CHAR_COLORWHITE);
    return;
  }
}
#endif

#ifdef _ITEM_WATERWORDSTATUS
void ITEM_WaterWordStatus(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  char itemarg[256];

  if (!CHAR_CHECKINDEX(charaindex)) {
    return;
  }
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex)) {
    return;
  }

  if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "time", itemarg,
                       sizeof(itemarg)) != FALSE) {
    char token[256];
    int nums = CHAR_getWorkInt(charaindex, CHAR_WORKSTATUSWATER);
    if (nums < 0)
      nums = 0;
    if (nums > 0) {
      CHAR_talkToCli(charaindex, -1, "水中呼吸时间不得累积。",
                     CHAR_COLORYELLOW);
      return;
    }
    CHAR_setWorkInt(charaindex, CHAR_WORKSTATUSWATER, nums + atoi(itemarg));
    CHAR_setItemIndex(charaindex, haveitemindex, -1);
    CHAR_sendItemDataOne(charaindex, haveitemindex);
    ITEM_endExistItemsOne(itemindex);

    sprintf(token, "水中呼吸时间增加%d分，总计%d分。", atoi(itemarg),
            CHAR_getWorkInt(charaindex, CHAR_WORKSTATUSWATER));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  } else {
    CHAR_talkToCli(charaindex, -1, "什麽事也没发生。", CHAR_COLORYELLOW);
  }
}
#endif

#ifdef _CHIKULA_STONE
void ITEM_ChikulaStone(int charaindex, int toindex, int haveitemindex) {
  int itemindex, fd;
  char itemarg[256];

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  CHAR_setWorkInt(charaindex, CHAR_WORKCHIKULAHP, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKCHIKULAMP, 0);
  setChiStone(fd, 0); // 1hp 2mp

  if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "hp", itemarg,
                       sizeof(itemarg)) != FALSE) {
    setChiStone(fd, 1);
    CHAR_setWorkInt(charaindex, CHAR_WORKCHIKULAHP, atoi(itemarg));
  } else if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "mp",
                              itemarg, sizeof(itemarg)) != FALSE) {
    setChiStone(fd, 2);
    CHAR_setWorkInt(charaindex, CHAR_WORKCHIKULAMP, atoi(itemarg));
  } else {
  }

  CHAR_talkToCli(charaindex, -1, "接受奇克拉的祝福。", CHAR_COLORWHITE);
  CHAR_setItemIndex(charaindex, haveitemindex, -1);
  CHAR_sendItemDataOne(charaindex, haveitemindex);
  ITEM_endExistItemsOne(itemindex);
}
#endif

#ifdef _ITEM_ORNAMENTS
void ITEM_PutOrnaments(int charaindex, int itemindex) {
  char *arg = NULL;
  char itemname[256];
  int bbnums = 0;
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (arg == "\0")
    return; // ITEM_BASEIMAGENUMBER
  bbnums = atoi(arg);
  ITEM_setInt(itemindex, ITEM_BASEIMAGENUMBER, bbnums);
  ITEM_setWorkInt(itemindex, ITEM_CANPICKUP, 1);

  sprintf(itemname, "%s%s%s", CHAR_getChar(charaindex, CHAR_NAME), "的",
          ITEM_getChar(itemindex, ITEM_SECRETNAME));
  ITEM_setChar(itemindex, ITEM_SECRETNAME, itemname);
}
#endif

#ifdef _SUIT_ITEM

/*
#ifdef _SUIT_TWFWENDUM
        #define MAX_SUITTYPE 18
#else
        #ifdef _SUIT_ADDENDUM
                #define MAX_SUITTYPE 14
        #else
                #ifdef _MAGIC_RESIST_EQUIT			// WON ADD
职业抗性装备 #define MAX_SUITTYPE 11 #else #define MAX_SUITTYPE 8
                #endif//_MAGIC_RESIST_EQUIT
        #endif//_SUIT_ADDENDUM
#endif//_SUIT_TWFWENDUM
*/
void ITEM_CheckSuitEquip(int charaindex) {
  int i, j, itemindex, defCode = -1, same = 0;
  int nItem[CHAR_STARTITEMARRAY];
  int maxitem;
  struct tagIntSuit {
    char fun[256];
    int intfun; // CHAR_getInt
  };
  struct tagIntSuit ListSuit[] = {
      {"VIT", CHAR_WORKSUITVIT}, // HP
      {"FSTR", CHAR_WORKSUITMODSTR},
      {"MSTR", CHAR_WORKSUITSTR}, //攻
      {"MTGH", CHAR_WORKSUITTGH}, //防
      {"MDEX", CHAR_WORKSUITDEX}, //敏
#ifdef _STATUS_WATERWORD
      {"WAST", CHAR_WORKSTATUSWATER},
#endif
      {"HP", CHAR_WORKROUNDHP},
      {"MP", CHAR_WORKROUNDMP}
#ifdef _MAGIC_RESIST_EQUIT // WON ADD 职业抗性装备
      ,
      {"FRES", CHAR_WORK_F_SUIT},
      {"IRES", CHAR_WORK_I_SUIT},
      {"TRES", CHAR_WORK_T_SUIT}
#endif
#ifdef _SUIT_ADDENDUM
      ,
      {"RESIST", CHAR_WORKRESIST},   //异常抗性率
      {"COUNTER", CHAR_WORKCOUNTER}, //反击率
      {"M_POW", CHAR_WORKMPOWER}     //加强法师的魔法
#endif
#ifdef _SUIT_TWFWENDUM
      ,
      {"EARTH", CHAR_WORK_EA},  //地
      {"WRITER", CHAR_WORK_WR}, //水
      {"FIRE", CHAR_WORK_FI},   //火
      {"WIND", CHAR_WORK_WI}    //风
#endif
#ifdef _SUIT_ADDPART3
      ,
      {"WDUCKPOWER", CHAR_WORKDUCKPOWER}, //套装回避
      {"RENOCASE", CHAR_WORKRENOCAST},    //沉默抗性率
      {"SUITSTRP", CHAR_WORKSUITSTR_P},   //攻提升 单位为%
      {"SUITTGH_P", CHAR_WORKSUITTGH_P},  //防提升 单位为%
      {"SUITDEXP", CHAR_WORKSUITDEX_P}    //敏提升 单位为%
#endif
#ifdef _SUIT_ADDPART4
      ,
      {"SUITPOISON", CHAR_SUITPOISON}, //带毒装备
      {"M2_POW", CHAR_WORKMPOWER2}, //加强法师的魔法,与M_POW功能相同(机率30%)
      {"UN_POW_M", CHAR_WORKUNMPOWER} //抵抗法师的魔法
#endif
  };
  maxitem = sizeof(ListSuit) / sizeof(ListSuit[0]);
  CHAR_setWorkInt(charaindex, CHAR_WORKSUITITEM, 0);
  for (i = 0; i < maxitem /*MAX_SUITTYPE*/; i++) {
    CHAR_setWorkInt(charaindex, ListSuit[i].intfun, 0);
  }
  j = 0;
  for (i = 0; i < CHAR_STARTITEMARRAY; i++) {
    nItem[i] = -1;
    itemindex = CHAR_getItemIndex(charaindex, i);
    if (!ITEM_CHECKINDEX(itemindex))
      continue;
    nItem[j++] = ITEM_getInt(itemindex, ITEM_SUITCODE);
  }
  for (i = 0; i < j && defCode == -1; i++) {
    int k;
    same = 0;
    if (nItem[i] <= 0)
      continue;
    for (k = (j - 1); k >= 0; k--) {
      if (nItem[i] == nItem[k])
        same++;
    }
    if (same >= 3 && nItem[i] != 0)
      defCode = nItem[i];
  }
  if (defCode == -1)
    return;
  CHAR_setWorkInt(charaindex, CHAR_WORKSUITITEM, defCode);
  for (i = 0; i < CHAR_STARTITEMARRAY; i++) {
    char *buf, buf1[256];
    itemindex = CHAR_getItemIndex(charaindex, i);
    if (!ITEM_CHECKINDEX(itemindex))
      continue;
    if (ITEM_getInt(itemindex, ITEM_SUITCODE) == defCode) {
      for (j = 0; j < maxitem /*MAX_SUITTYPE*/; j++) {
        buf = ITEM_getChar(itemindex, ITEM_ARGUMENT);
        if (strstr(buf, ListSuit[j].fun) == NULL)
          continue;
        if (NPC_Util_GetStrFromStrWithDelim(buf, ListSuit[j].fun, buf1,
                                            sizeof(buf1)) == NULL)
          continue;
        CHAR_setWorkInt(charaindex, ListSuit[j].intfun, atoi(buf1));
      }
    }
  }
  CHAR_complianceParameter(charaindex);
}

void ITEM_suitEquip(int charaindex, int itemindex) {
  ITEM_CheckSuitEquip(charaindex);
}

void ITEM_ResuitEquip(int charaindex, int itemindex) {
  ITEM_CheckSuitEquip(charaindex);
}
#endif //_SUIT_ITEM

void ITEM_remNoenemy(int charaindex, int itemindex) {
  int fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  int el = getEqNoenemy(fd);
  int fl = CHAR_getInt(charaindex, CHAR_FLOOR);

  if (ITEM_CHECKINDEX(itemindex) == FALSE)
    return;

  clearEqNoenemy(CHAR_getWorkInt(charaindex, CHAR_WORKFD));
  if (el >= 200) {
    CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
    return;
  } else if (el >= 120) {
    if ((fl == 100) || (fl == 200) || (fl == 300) || (fl == 400) ||
        (fl == 500)) {
      CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
      return;
    }
  } else if (el >= 80) {
    if ((fl == 100) || (fl == 200) || (fl == 300) || (fl == 400)) {
      CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
      return;
    }
  } else if (el >= 40) {
    if ((fl == 100) || (fl == 200)) {
      CHAR_talkToCli(charaindex, -1, "环绕着你的光芒消失了。", CHAR_COLORWHITE);
      return;
    }
  }
  CHAR_talkToCli(charaindex, -1, "什麽事也没有发生。", CHAR_COLORWHITE);
}

extern void setStayEncount(int fd);
void ITEM_useEncounter(int charaindex, int toindex, int haveitemindex) {
  int itemindex, fd;
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  setStayEncount(fd);
#ifdef _USER_CHARLOOPS
  {
    Char *ch;
    ch = CHAR_getCharPointer(charaindex);
    if (ch == NULL)
      return;
    strcpysafe(ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
               sizeof(ch->charfunctable[CHAR_LOOPFUNCTEMP1]),
               "CHAR_BattleStayLoop"); //战斗
    CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, getBattleSpeed());
    CHAR_constructFunctable(charaindex);
  }
#endif
  CHAR_talkToCli(charaindex, -1, "你感受到周边突然充满杀气！",
                 CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}

#ifdef _Item_DeathAct
void ITEM_UseDeathCounter(int charaindex, int toindex, int haveitemindex) {
  int itemindex, fd;
  int itemmaxuse = -1;
  char buf1[256];
  char *itemarg;
  char itemnumstr[32];
  int okfloor = 0;
  BOOL Useflag = FALSE;
  int i = 1;

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

#ifdef _ITEM_STONE
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  while (1) {
    if (getStringFromIndexWithDelim(itemarg, "|", i, itemnumstr,
                                    sizeof(itemnumstr)) == FALSE)
      break;
    okfloor = atoi(itemnumstr);
    if (CHAR_getInt(charaindex, CHAR_FLOOR) == okfloor) {
      Useflag = TRUE;
      break;
    }
    i++;
  }
  if (okfloor != 0) { // 奇怪的石头
    itemmaxuse = ITEM_getInt(itemindex, ITEM_DAMAGEBREAK);
    if (itemmaxuse != -1) {
      itemmaxuse--;
      ITEM_setInt(itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
      if (itemmaxuse < 1) {
        sprintf(buf1, "%s消失了。", ITEM_getChar(itemindex, ITEM_NAME));
        CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
        CHAR_DelItem(charaindex, haveitemindex);
        if (Useflag == FALSE) {
          CHAR_talkToCli(charaindex, -1, "没有发生任何事情！",
                         CHAR_COLORYELLOW);
          return;
        }
      } else {
        sprintf(buf1, "原地遇敌，可使用次数剩余%d次。", itemmaxuse);
        ITEM_setChar(itemindex, ITEM_EFFECTSTRING, buf1);
        CHAR_sendItemDataOne(charaindex, haveitemindex);
        if (Useflag == FALSE) {
          CHAR_talkToCli(charaindex, -1, "没有发生任何事情！",
                         CHAR_COLORYELLOW);
          return;
        }
      }
      fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
      setStayEncount(fd);
      CHAR_talkToCli(charaindex, -1, "你身边笼罩阴暗气息！", CHAR_COLORYELLOW);
    } else {
      sprintf(buf1, "%s消失了。", ITEM_getChar(itemindex, ITEM_NAME));
      CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
      CHAR_DelItem(charaindex, haveitemindex);
      if (Useflag == FALSE) {
        CHAR_talkToCli(charaindex, -1, "没有发生任何事情！", CHAR_COLORYELLOW);
        return;
      }
      fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
      setStayEncount(fd);
      CHAR_talkToCli(charaindex, -1, "你身边笼罩阴暗气息！", CHAR_COLORYELLOW);
    }
  } else { //恶魔宝石
#endif
#ifdef _ITEM_MAXUSERNUM
    itemmaxuse = ITEM_getInt(itemindex, ITEM_DAMAGEBREAK);
    if (itemmaxuse != -1) {
      itemmaxuse--;
      ITEM_setInt(itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
      if (itemmaxuse < 1) {
        sprintf(buf1, "道具 %s消失了。", ITEM_getChar(itemindex, ITEM_NAME));
        CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
        CHAR_DelItem(charaindex, haveitemindex);
      } else {
        sprintf(buf1, "原地遇敌，可使用次数剩余%d次。", itemmaxuse);
        ITEM_setChar(itemindex, ITEM_EFFECTSTRING, buf1);
        CHAR_sendItemDataOne(charaindex, haveitemindex);
      }
      fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
      setStayEncount(fd);
      CHAR_talkToCli(charaindex, -1, "你感受到周边突然充满杀气！",
                     CHAR_COLORYELLOW);
    } else {
      CHAR_DelItem(charaindex, haveitemindex);
      return;
    }
#else
  fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  setStayEncount(fd);
  sprintf(buf1, "道具 %s消失了。", ITEM_getChar(itemindex, ITEM_NAME));
  CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
#endif
#ifdef _ITEM_STONE
  }
#endif
#ifdef _USER_CHARLOOPS
  {
    Char *ch;
    ch = CHAR_getCharPointer(charaindex);
    if (ch == NULL)
      return;
    strcpysafe(ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
               sizeof(ch->charfunctable[CHAR_LOOPFUNCTEMP1]),
               "CHAR_BattleStayLoop"); //战斗
    CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, getBattleSpeed());
    CHAR_constructFunctable(charaindex);
  }
#endif
}
#endif

#ifdef _CHRISTMAS_REDSOCKS
void ITEM_useMaxRedSocks(int charaindex, int toindex, int haveitemindex) {
  int itemtimes = -1, itemindex;
  char *itemarg = NULL;
  int present[13] = {13061, 13062, 13063, 13064, 13088,
                     13089, 13090, 13091, // 1.
                     14756, 17256, 13092, 19692, 20594};
  int nowtimes = time(NULL);

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0") {
    CHAR_talkToCli(charaindex, -1, "道具无效!", CHAR_COLORYELLOW);
    return;
  }
  itemtimes = atoi(itemarg);
  if (nowtimes >= itemtimes &&
      nowtimes <= itemtimes + (60 * 60 * 24)) { //限制时间内可换礼物
    int si = 0, ret;
    char token[256];
    //删除
    CHAR_setItemIndex(charaindex, haveitemindex, -1);
    CHAR_sendItemDataOne(charaindex, haveitemindex);
    ITEM_endExistItemsOne(itemindex);
    itemindex = -1;
    //新增
    si = rand() % 100;
    if (si > 70) {
      si = rand() % 3 + 10;
    } else if (si > 60) {
      si = rand() % 2 + 8;
    } else {
      si = rand() % 8;
    }
    itemindex = ITEM_makeItemAndRegist(present[si]);
    if (!ITEM_CHECKINDEX(itemindex)) {
      CHAR_talkToCli(charaindex, -1, "酒捞袍捞 家戈登菌嚼聪促!",
                     CHAR_COLORYELLOW);
      return;
    }
    ret = CHAR_addItemSpecificItemIndex(charaindex, itemindex);
    if (ret < 0 || ret >= CHAR_getMaxInventory(charaindex)) {
      ITEM_endExistItemsOne(itemindex);
      CHAR_talkToCli(charaindex, -1, "礼物置放栏位错误!", CHAR_COLORYELLOW);
      return;
    }
    sprintf(token, "裙垫窍促 %s", ITEM_getChar(itemindex, ITEM_NAME));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    CHAR_sendItemDataOne(charaindex, ret);
  } else {
    char token[256];
    if (nowtimes < itemtimes) {
      int days, hours, minute, second;
      int defTimes = itemtimes - nowtimes;

      days = defTimes / (24 * 60 * 60);
      defTimes = defTimes - (days * (24 * 60 * 60));
      hours = defTimes / (60 * 60);
      defTimes = defTimes - (hours * (60 * 60));
      minute = defTimes / 60;
      defTimes = defTimes - (minute * 60);
      second = defTimes;
      sprintf(token, "%s还差%d天%d小时%d分%d秒才可使用!",
              ITEM_getChar(itemindex, ITEM_NAME), days, hours, minute, second);
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    } else if (nowtimes > itemtimes + (60 * 60 * 24)) {
      sprintf(token, "%s使用期限已过!", ITEM_getChar(itemindex, ITEM_NAME));
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    }
  }
}
#endif

#ifdef _CHRISTMAS_REDSOCKS_NEW
void ITEM_useMaxRedSocksNew(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  char *itemarg = NULL;
  char itemnumstr[32];
  int itemnum = 0;
  int present[20];
  int si = 0, ret, i;
  char token[256];

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0") {
    CHAR_talkToCli(charaindex, -1, "这是个芭乐圣诞袜!", CHAR_COLORYELLOW);
    return;
  }

  //道具数量
  if (getStringFromIndexWithDelim(itemarg, "|", 1, itemnumstr,
                                  sizeof(itemnumstr)) == FALSE)
    return;
  itemnum = atoi(itemnumstr);
  if (itemnum > 20)
    itemnum = 20;
  for (i = 0; i < itemnum; i++) {
    if (getStringFromIndexWithDelim(itemarg, "|", 2 + i, itemnumstr,
                                    sizeof(itemnumstr)))
      present[i] = atoi(itemnumstr);
  }

  //删除
  CHAR_setItemIndex(charaindex, haveitemindex, -1);
  CHAR_sendItemDataOne(charaindex, haveitemindex);
  ITEM_endExistItemsOne(itemindex);
  itemindex = -1;
  //新增
  si = rand() % itemnum;

  itemindex = ITEM_makeItemAndRegist(present[si]);
  if (!ITEM_CHECKINDEX(itemindex)) {
    CHAR_talkToCli(charaindex, -1, "酒捞袍捞 家戈登菌嚼聪促!",
                   CHAR_COLORYELLOW);
    return;
  }
  ret = CHAR_addItemSpecificItemIndex(charaindex, itemindex);
  if (ret < 0 || ret >= CHAR_getMaxInventory(charaindex)) {
    ITEM_endExistItemsOne(itemindex);
    CHAR_talkToCli(charaindex, -1, "礼物置放栏位错误!", CHAR_COLORYELLOW);
    return;
  }
  sprintf(token, "裙垫窍促 %s", ITEM_getChar(itemindex, ITEM_NAME));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_sendItemDataOne(charaindex, ret);
}
#endif

#ifdef _PETSKILL_CANNEDFOOD
void ITEM_useSkillCanned(int charaindex, int toindex, int itemNo) {
  int itemindex;
  char buf1[256];
  itemindex = CHAR_getItemIndex(charaindex, itemNo);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    int i, petNo = -1;
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      if (toindex == CHAR_getCharPet(charaindex, i)) {
        petNo = i;
        break;
      }
    }
    if (petNo == -1) {
      sprintf(buf1, "%s不在人物身上。", CHAR_getChar(toindex, CHAR_NAME));
      CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
      return;
    }
    ITEM_usePetSkillCanned_PrintWindow(charaindex, petNo);
    CHAR_setWorkInt(charaindex, CHAR_WORKRENAMEITEMNUM, petNo);
    CHAR_setWorkInt(charaindex, CHAR_WORKRENAMEITEMINDEX, itemNo);
    CHAR_setWorkInt(charaindex, CHAR_WORKHAVEITEMINDEX, itemindex);
  } else {
    sprintf(buf1, "道具 %s仅限宠物使用。", ITEM_getChar(itemindex, ITEM_NAME));
    CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
    return;
  }
}
#endif

#ifdef _ITEM_METAMO
void ITEM_metamo(int charaindex, int toindex, int haveitemindex) {

  char *arg, msg[128];
  int itemindex, metamoTime, haveindex, battlemode, i;

  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  // print(" PetMetamo_toindex:%d ", toindex);

  if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
    CHAR_talkToCli(charaindex, -1, "无法变身，骑乘中不能变身！",
                   CHAR_COLORYELLOW);
    return;
  }
#ifdef _FIX_METAMORIDE
  if (CHAR_CHECKJOINENEMY(charaindex) == TRUE) {
    CHAR_talkToCli(charaindex, -1, "无法变身，搭乘中不能变身！",
                   CHAR_COLORYELLOW);
    return;
  }
#else
  if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 100259) {
    CHAR_talkToCli(charaindex, -1, "无法变身，搭乘中不能变身！",
                   CHAR_COLORYELLOW);
    return;
  }
#endif
#ifdef _PETSKILL_BECOMEPIG
  if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1) { //变成乌力了
    CHAR_talkToCli(charaindex, -1, "无法变身，乌力化中不能变身！",
                   CHAR_COLORYELLOW);
    return;
  }
#endif

#ifdef _FIXBUG_ATTACKBOW
  {
    int armindex = CHAR_getItemIndex(charaindex, CHAR_ARM);
    if (ITEM_CHECKINDEX(armindex) == TRUE) {
      int armtype = BATTLE_GetWepon(charaindex);
      if (armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
          armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG) {
        CHAR_talkToCli(charaindex, -1, "使用远距离武器无法变身！",
                       CHAR_COLORYELLOW);
        return;
      }
    }
  }
#endif

  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  // 爵  钒铵凛反  骰允月
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return;
  }
  // 爵    分匀凶日
  if (IsBATTLING(charaindex) == TRUE) {
    toindex = BATTLE_No2Index(CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX),
                              toindex);
  }

  if (CHAR_CHECKINDEX(toindex) == FALSE)
    return;

  haveindex = -1;
  for (i = 0; i < 5; i++) {
    if (CHAR_getCharPet(charaindex, i) == toindex) {
      haveindex = i;
      break;
    }
  }
  if (haveindex == -1 && charaindex != toindex) {
    CHAR_talkToCli(charaindex, -1, "无法变身，只能变成自己的宠物！",
                   CHAR_COLORYELLOW);
    return;
  }

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (arg == "\0")
    return;
  if (sscanf(arg, "%d", &metamoTime) != 1)
    return;

  if (toindex != charaindex) {
    CHAR_setWorkInt(charaindex, CHAR_WORKITEMMETAMO,
                    NowTime.tv_sec + metamoTime);
    sprintf(msg, "变身成%s！", CHAR_getChar(toindex, CHAR_NAME));
  } else {
    CHAR_setWorkInt(charaindex, CHAR_WORKITEMMETAMO, 0);
    sprintf(msg, "变回自己！");
  }
  CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);

  CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
              CHAR_getInt(toindex, CHAR_BASEBASEIMAGENUMBER));
  CHAR_complianceParameter(charaindex);
  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif
#ifdef _ITEM_CRACKER
void ITEM_Cracker(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  // 检查玩家是否有效
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return; //失败
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  // 战斗中使用无效
  if (!battlemode)
    ITEM_useCracker_Effect(charaindex, toindex, haveitemindex);
  else
    CHAR_talkToCli(charaindex, -1, "酒公老档 老绢唱瘤 臼疽嚼聪促。",
                   CHAR_COLORWHITE);
}
#endif

void ITEM_AddPRSkillPercent(int charaindex, int toindex, int haveitemindex) {
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
  int level;
  char token[64];
  int MySKPercent = 0, itemindex = -1, i;

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getInt(charaindex, PROFESSION_CLASS) == 0) {
    CHAR_talkToCli(charaindex, -1, "您尚未就职，无法使用！", CHAR_COLORYELLOW);
    return;
  }

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  level = atoi(ITEM_getChar(itemindex, ITEM_ARGUMENT));

  for (i = 0; i < PROFESSION_MAX_LEVEL; i++) {
    if (CHAR_getIntPSkill(charaindex, i, SKILL_IDENTITY) == -1) {
      continue;
    }
    MySKPercent = CHAR_getIntPSkill(charaindex, i, SKILL_LEVEL) + level * 100;
    if (MySKPercent > 10000)
      MySKPercent = 10000;
    CHAR_setIntPSkill(charaindex, i, SKILL_LEVEL, MySKPercent);
  }

  CHAR_setItemIndex(charaindex, haveitemindex, -1);
  CHAR_sendItemDataOne(charaindex, haveitemindex);
  ITEM_endExistItemsOne(itemindex);
  CHAR_sendStatusString(charaindex, "S");
  // PROFESSION_LEVEL_CHECK_UP( charaindex );
  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  sprintf(token, "所有职业技能熟练度上升%d％。", level);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
#endif
}

void ITEM_AddPRSkillPoint(int charaindex, int toindex, int haveitemindex) {
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
  int point;
  char token[64];
  int MyPRpoint = 0, itemindex = -1;
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getInt(charaindex, PROFESSION_CLASS) == 0) {
    CHAR_talkToCli(charaindex, -1, "您尚未就职，无法使用！", CHAR_COLORYELLOW);
    return;
  }

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  point = atoi(ITEM_getChar(itemindex, ITEM_ARGUMENT));

  MyPRpoint = CHAR_getInt(charaindex, PROFESSION_SKILL_POINT);
  CHAR_setInt(charaindex, PROFESSION_SKILL_POINT, MyPRpoint + point);

  CHAR_setItemIndex(charaindex, haveitemindex, -1);
  CHAR_sendItemDataOne(charaindex, haveitemindex);
  ITEM_endExistItemsOne(itemindex);

  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));

  // CHAR_talkToCli( charaindex, -1, "增加一点职业技能点数。",
  // CHAR_COLORYELLOW);
  sprintf(token, "增加%d点职业技能点数。", point);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
#endif
}

#ifdef _ITEM_ADDEXP // vincent 经验提升
void ITEM_Addexp(int charaindex, int toindex, int haveitemindex) {
  // 检查玩家是否有效
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return; //失败
  if (CHAR_CHECKINDEX(toindex) == FALSE)
    return; //失败
  int i;
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    if (CHAR_getCharPet(charaindex, i) == toindex) {
      break;
    }
  }
  if (i == CHAR_MAXPETHAVE) {
    if (charaindex != toindex) {
      CHAR_talkToCli(charaindex, -1, "只能为自己或自己的宠物使用！",
                     CHAR_COLORYELLOW);
      return;
    }
  }
#if 1
  ITEM_useAddexp_Effect(charaindex, toindex, haveitemindex);
#else
  if (!CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP)) {
    ITEM_useAddexp_Effect(charaindex, toindex, haveitemindex);
  } else {
    CHAR_talkToCli(charaindex, -1, "先前使用之药效依然存在", CHAR_COLORYELLOW);
  }
#endif
}
#endif

#ifdef _ITEM_REFRESH // vincent 解除异常状态道具
void ITEM_Refresh(int charaindex, int toindex, int haveitemindex) {
  int battlemode, itemindex;
  print("\nvincent--ITEM_Refresh");
  // 检查玩家是否有效
  if (CHAR_CHECKINDEX(charaindex) == FALSE) {
    print("\nvincent--(charaindex) == FALSE");
    return; //失败
  }
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);

  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode) {
    print("\nvincent--enter ITEM_useAddexp_Effect");
    print("\nvincent-->charaindex:%d,toindex:%d", charaindex, toindex);
    ITEM_useRefresh_Effect(charaindex, toindex, haveitemindex);
  } else
    CHAR_talkToCli(charaindex, -1, "酒公老档 老绢唱瘤 臼疽嚼聪促。",
                   CHAR_COLORWHITE);

  /* 平乓仿弁正□及赭    伉旦玄井日壅蛔 */
  CHAR_setItemIndex(charaindex, haveitemindex, -1);
  CHAR_sendItemDataOne(charaindex, haveitemindex); /* 失奶  丞凳蕙 */
  /* 壅允 */
  ITEM_endExistItemsOne(itemindex);
}
#endif
// Terry 2001/12/21
#ifdef _ITEM_FIRECRACKER
void ITEM_firecracker(int charaindex, int toindex, int haveitemindex) {
  int battlemode;

  // 检查玩家是否有效
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return; //失败

  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);

  if (battlemode // 检查是否在战斗中
#ifdef _PETSKILL_BECOMEPIG
      && CHAR_getInt(charaindex, CHAR_BECOMEPIG) == -1
#endif
  )
    ITEM_useFirecracker_Battle(charaindex, toindex, haveitemindex);
  else
    CHAR_talkToCli(charaindex, -1, "酒公老档 老绢唱瘤 臼疽嚼聪促。",
                   CHAR_COLORWHITE);
}
#endif
// Terry end

void ITEM_WearEquip(int charaindex, int itemindex) {
  // WON ADD
  //	if( ITEM_getInt(itemindex,ITEM_ID) == 20130 ){
  CHAR_setWorkInt(charaindex, CHAR_PickAllPet, TRUE);
  ///	}
  return;
}
void ITEM_ReWearEquip(int charaindex, int itemindex) {
  CHAR_setWorkInt(charaindex, CHAR_PickAllPet, FALSE);
  return;
}

#ifdef _Item_ReLifeAct
void ITEM_DIErelife(int charaindex, int itemindex, int eqw) {
  int ReceveEffect = -1;
  int toNo;
  int battleindex = -1;
  int attackNo = -1;
  int WORK_HP = 1;
  char buf[256];

  battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
  if (BATTLE_CHECKINDEX(battleindex) == FALSE) {
    print("\n battleindex =%d return", battleindex);
    return;
  }

  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  if (ITEM_CHECKINDEX(itemindex) == FALSE)
    return;
#ifdef _DUMMYDIE
  if (CHAR_getFlg(charaindex, CHAR_ISDUMMYDIE) == FALSE)
#else
  if (CHAR_getFlg(charaindex, CHAR_ISDIE) == FALSE)
#endif
  {
    print("\n [ %d, CHAR_ISDIE DUMMY FALSE] return !", charaindex);
    return;
  }

  if (ITEM_getArgument(ITEM_getChar(itemindex, ITEM_ARGUMENT), "HP", buf,
                       sizeof(buf)) == FALSE) {
    WORK_HP = 1;
  } else {
    if (!strcmp(buf, "FULL")) {
      WORK_HP = CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP);
    } else {
      WORK_HP = atoi(buf);
    }
  }

  ReceveEffect = SPR_fukkatu3;
  toNo = BATTLE_Index2No(battleindex, charaindex);
  attackNo = -1;

  BATTLE_MultiReLife(battleindex, attackNo, toNo, WORK_HP, ReceveEffect);
  CHAR_setItemIndex(charaindex, eqw, -1);
  ITEM_endExistItemsOne(itemindex);
  CHAR_sendItemDataOne(charaindex, eqw);
  return;
}
#endif

#ifdef _EQUIT_DEFMAGIC
void ITEM_MagicEquitWear(int charaindex, int itemindex) {
  char buf[256];
  char *itemarg;
  char Free[][128] = {"EA", "WA", "FI", "WI", "QU"};
  int index = 0;
  int dMagic = 0;
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  while (index < arraysizeof(Free)) {
    if (ITEM_getArgument(itemarg, Free[index], buf, sizeof(buf)) == TRUE) {
      dMagic = atoi(buf);
      if (dMagic <= 100 && dMagic >= -100) {
        int def_magic =
            CHAR_getWorkInt(charaindex, CHAR_EQUITDEFMAGIC_E + index);
        CHAR_setWorkInt(charaindex, CHAR_EQUITDEFMAGIC_E + index,
                        def_magic + dMagic);
      }
      dMagic = 0;
    }
    index++;
    if ((CHAR_EQUITDEFMAGIC_E + index) > CHAR_EQUITQUIMAGIC)
      break;
  }

  return;
}

void ITEM_MagicEquitReWear(int charaindex, int itemindex) {
  char buf[256];
  char *itemarg;
  char Free[][128] = {"EA", "WA", "FI", "WI", "QU"};
  int index = 0;
  int dMagic = 0;

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  while (index < arraysizeof(Free)) {
    if (ITEM_getArgument(itemarg, Free[index], buf, sizeof(buf)) == TRUE) {
      dMagic = atoi(buf);
      if (dMagic <= 100 && dMagic >= -100) {
        int def_magic =
            CHAR_getWorkInt(charaindex, CHAR_EQUITDEFMAGIC_E + index);
        CHAR_setWorkInt(charaindex, CHAR_EQUITDEFMAGIC_E + index,
                        def_magic - dMagic);
      }
      dMagic = 0;
    }
    index++;
    if ((CHAR_EQUITDEFMAGIC_E + index) > CHAR_EQUITQUIMAGIC)
      break;
  }

  return;
}
#endif

#ifdef _EQUIT_RESIST
void ITEM_MagicResist(int charaindex, int itemindex) {
  char *itemarg, *p = NULL;
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  if (strstr(itemarg, "抗火")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFIRE, atoi(p + 4));
  } else if (strstr(itemarg, "抗雷")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITTHUNDER, atoi(p + 4));
  } else if (strstr(itemarg, "抗冰")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITICE, atoi(p + 4));
  } else if (strstr(itemarg, "虚弱")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITWEAKEN, atoi(p + 4));
  } else if (strstr(itemarg, "魔障")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITBARRIER, atoi(p + 4));
  } else if (strstr(itemarg, "沉默")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITNOCAST, atoi(p + 4));
  } else if (strstr(itemarg, "落马")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFALLRIDE, atoi(p + 4));
  }
}

void ITEM_MagicReResist(int charaindex, int itemindex) {
  char *itemarg, *p = NULL;
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  if (strstr(itemarg, "抗火")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFIRE, 0);
  } else if (strstr(itemarg, "抗雷")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFIRE, 0);
  } else if (strstr(itemarg, "抗冰")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFIRE, 0);
  } else if (strstr(itemarg, "虚弱")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFIRE, 0);
  } else if (strstr(itemarg, "魔障")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFIRE, 0);
  } else if (strstr(itemarg, "沉默")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFIRE, 0);
  } else if (strstr(itemarg, "落马")) {
    p = itemarg;
    CHAR_setWorkInt(charaindex, CHAR_WORKEQUITFIRE, 0);
  }
}
#endif

#ifdef _MAGIC_RESIST_EQUIT // WON ADD 职业抗性装备
void ITEM_P_MagicEquitWear(int charaindex, int itemindex) {
  char buf[256] = {0};
  char *itemarg;
  char Free[][128] = {"FR", "IR", "TR"};
  int dMagic = 0;
  int i;

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;

  //	print("\n won test 10.0 ==> itemarg(%s)", itemarg );

  for (i = 0; i < 3; i++) {
    if (ITEM_getArgument(itemarg, Free[i], buf, sizeof(buf)) == TRUE) {
      dMagic = atoi(buf);
      //			print("\n won test 10.1 ==> i(%d) dMagic(%d)",
      //i, dMagic );

      if (dMagic <= 100 && dMagic >= -100) {
        int def_magic = CHAR_getWorkInt(charaindex, CHAR_WORK_F_SUIT + i);
        CHAR_setWorkInt(charaindex, CHAR_WORK_F_SUIT + i, def_magic + dMagic);

        //				print("\n won test 10.2 ==> i(%d)(%d)", i,
        //CHAR_getWorkInt( charaindex, CHAR_WORK_F_SUIT+i ) );
      }
      dMagic = 0;
    }
  }

  return;
}

void ITEM_P_MagicEquitReWear(int charaindex, int itemindex) {
  char buf[256] = {0};
  char *itemarg;
  char Free[][128] = {"FR", "IR", "TR"};
  int dMagic = 0;
  int i;

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;

  //	print("\n won test 10.0 ==> itemarg(%s)", itemarg );

  for (i = 0; i < 3; i++) {
    if (ITEM_getArgument(itemarg, Free[i], buf, sizeof(buf)) == TRUE) {
      dMagic = atoi(buf);
      //			print("\n won test 10.1 ==> i(%d) dMagic(%d)",
      //i, dMagic );

      if (dMagic <= 100 && dMagic >= -100) {
        int def_magic = CHAR_getWorkInt(charaindex, CHAR_WORK_F_SUIT + i);
        CHAR_setWorkInt(charaindex, CHAR_WORK_F_SUIT + i, def_magic - dMagic);

        //				print("\n won test 10.2 ==> i(%d)(%d)", i,
        //CHAR_getWorkInt( charaindex, CHAR_WORK_F_SUIT+i ) );
      }
      dMagic = 0;
    }
  }

  return;
}
#endif

#ifdef _ITEM_WARP_FIX_BI
void recoverbi(int index) {
  int eBbi = -1, eArm = -1, eNum = -1, eBi = -1, bi = -1;

  bi = CHAR_getInt(index, CHAR_BASEIMAGENUMBER);
  eBbi = CHAR_getInt(index, CHAR_BASEBASEIMAGENUMBER);
  eArm = CHAR_getItemIndex(index, CHAR_ARM);
  eNum = ITEM_FIST;
  if (ITEM_CHECKINDEX(eArm))
    eNum = ITEM_getInt(eArm, ITEM_TYPE);

  eBi = CHAR_getNewImagenumberFromEquip(eBbi, eNum);

  if (CHAR_getInt(index, CHAR_RIDEPET) != -1) //骑宠
    CHAR_complianceParameter(index);
  else //非骑宠
    if ((eBi != -1) && (eBi != bi))
      CHAR_setInt(index, CHAR_BASEIMAGENUMBER, eBi);
}
#endif

#ifdef _ITEM_TIME_LIMIT
void ITEM_TimeLimit(int charaindex) {
  int i = 0;
  int iid = 0;
  long lTime = 0;
  char buff[256];
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
    int itemindex = CHAR_getItemIndex(charaindex, i);
    if (!ITEM_CHECKINDEX(itemindex))
      continue;
    lTime = ITEM_getInt(itemindex, ITEM_TIMELIMIT);
    // if( ITEM_getInt( itemindex, ITEM_ID) == 20173 //燃烧火把
    //|| ITEM_getInt( itemindex, ITEM_ID) == 20704 ){
    if (lTime > 0 && NowTime.tv_sec > lTime) {
      print("进入检查:%s,%d", ITEM_getChar(itemindex, ITEM_NAME), lTime);
      //确认是否为 Time时间限制道具
      char *arg, *p = NULL;
      arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
      if (arg && (p = strstr(arg, "Time")) == NULL) {
        continue;
      }
      char token[128];
      if (getStringFromIndexWithDelim(arg, "|", 2, token, sizeof(token)) !=
          NULL) {
        iid = atoi(token);
      } else {
        iid = ITEM_getInt(itemindex, ITEM_ID) + 1;
      }
      CHAR_DelItemMess(charaindex, i, 0);
      itemindex = ITEM_makeItemAndRegist(iid);
      if (itemindex != -1) {
        snprintf(buff, sizeof(buff), "%s的属性已蜕变..",
                 ITEM_getChar(itemindex, ITEM_NAME));
        CHAR_talkToCli(charaindex, -1, buff, CHAR_COLORGREEN);
      } else {
        snprintf(buff, sizeof(buff), "%s的效果已消失..",
                 ITEM_getChar(itemindex, ITEM_NAME));
        CHAR_talkToCli(charaindex, -1, buff, CHAR_COLORGREEN);
      }
      if (itemindex != -1) {
        CHAR_setItemIndex(charaindex, i, itemindex);
        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, charaindex);
        CHAR_sendItemDataOne(charaindex, i);
      }
    }
    //}
  }
}
#endif

#ifdef _ITEM_CONSTITUTION
void ITEM_Constitution(int charaindex, int toindex, int haveitemindex) {
  char buf[256];
  char *itemarg;
  char Free[][128] = {"VI", "ST", "TG", "DE"};
  int index = 0, FixPoint = 0, itemindex;
  BOOL FIXs = FALSE;
  int AllPoint = 0;

  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return;
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;

  AllPoint = CHAR_getInt(charaindex, CHAR_VITAL) +
             CHAR_getInt(charaindex, CHAR_VITAL + 1) +
             CHAR_getInt(charaindex, CHAR_VITAL + 2) +
             CHAR_getInt(charaindex, CHAR_VITAL + 3);

  while (index < arraysizeof(Free)) {
    if (ITEM_getArgument(itemarg, Free[index], buf, sizeof(buf)) == TRUE) {
      FixPoint = atoi(buf);
      if (FixPoint <= 100 && FixPoint > 0) {
        int points = CHAR_getInt(charaindex, CHAR_VITAL + index);
        // Change Fix
        // if( points < (FixPoint*100) ) break;
        if (points < (FixPoint * 100) || AllPoint - (FixPoint * 100) <= 0) {
          CHAR_talkToCli(charaindex, -1,
                         "无法使用，修改后属性点不能小于或等于0。",
                         CHAR_COLORYELLOW);
          return;
        }
        CHAR_setInt(charaindex, CHAR_VITAL + index,
                    (points - (FixPoint * 100)));
        CHAR_setInt(charaindex, CHAR_SKILLUPPOINT,
                    CHAR_getInt(charaindex, CHAR_SKILLUPPOINT) + FixPoint);
        FIXs = TRUE;
      }
      FixPoint = 0;
    }
    index++;
    if ((CHAR_VITAL + index) > CHAR_DEX)
      break;
  }
  CHAR_complianceParameter(charaindex);
  CHAR_send_P_StatusString(
      charaindex, CHAR_P_STRING_MAXHP | CHAR_P_STRING_HP | CHAR_P_STRING_LV |
                      CHAR_P_STRING_EXP | CHAR_P_STRING_ATK |
                      CHAR_P_STRING_DEF | CHAR_P_STRING_QUICK |
                      CHAR_P_STRING_VITAL | CHAR_P_STRING_STR |
                      CHAR_P_STRING_TOUGH | CHAR_P_STRING_DEX);

  CHAR_Skillupsend(charaindex);
  if (FIXs == TRUE) {
    sprintf(buf, "%s", "你的身体感到变化。");
  } else {
    sprintf(buf, "%s", "物品无效。");
  }
  CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _NEW_RIDEPETS
void ITEM_useLearnRideCode(int charaindex, int toindex,
                           int haveitemindex) { // CHAR_LOWRIDEPETS
#ifdef _RIDEMODE_20
  if (getRideMode() == 2 || getRideMode() == 4) {
    CHAR_talkToCli(charaindex, -1,
                   "服务端为2.0非骑证模式，所以你无法使用该物品",
                   CHAR_COLORYELLOW);
    return;
  }
#endif
  int itemindex, i;
  char buf1[256];
  char *itemarg = NULL;
  int ridetrans;
  typedef struct {
    char arg[32];
    int Code;
  } tagNewRideCode;
  tagNewRideCode NewRides[] = {
      {"RIDE_PET0", RIDE_PET0},   {"RIDE_PET1", RIDE_PET1},
      {"RIDE_PET2", RIDE_PET2},   {"RIDE_PET3", RIDE_PET3},
      {"RIDE_PET4", RIDE_PET4},   {"RIDE_PET5", RIDE_PET5},
      {"RIDE_PET6", RIDE_PET6},   {"RIDE_PET7", RIDE_PET7},
      {"RIDE_PET8", RIDE_PET8},   {"RIDE_PET9", RIDE_PET9},
      {"RIDE_PET10", RIDE_PET10}, {"RIDE_PET11", RIDE_PET11},
      {"RIDE_PET12", RIDE_PET12}, {"RIDE_PET13", RIDE_PET13},
      {"RIDE_PET14", RIDE_PET14},
#ifdef _RIDE_CF
      {"RIDE_PET15", RIDE_PET15}, {"RIDE_PET16", RIDE_PET16},
      {"RIDE_PET17", RIDE_PET17}, {"RIDE_PET18", RIDE_PET18},
      {"RIDE_PET19", RIDE_PET19}, {"RIDE_PET20", RIDE_PET20},
      {"RIDE_PET21", RIDE_PET21}, {"RIDE_PET22", RIDE_PET22},
      {"RIDE_PET23", RIDE_PET23}, {"RIDE_PET24", RIDE_PET24},
      {"RIDE_PET25", RIDE_PET25}, {"RIDE_PET26", RIDE_PET26},
      {"RIDE_PET27", RIDE_PET27}, {"RIDE_PET28", RIDE_PET28},
      {"RIDE_PET29", RIDE_PET29}, {"RIDE_PET30", RIDE_PET30},
      {"RIDE_PET31", RIDE_PET31}, {"RIDE_PET32", RIDE_PET0},
      {"RIDE_PET33", RIDE_PET1},  {"RIDE_PET34", RIDE_PET2},
      {"RIDE_PET35", RIDE_PET3},  {"RIDE_PET36", RIDE_PET4},
      {"RIDE_PET37", RIDE_PET5},  {"RIDE_PET38", RIDE_PET6},
      {"RIDE_PET39", RIDE_PET7},  {"RIDE_PET40", RIDE_PET8},
      {"RIDE_PET41", RIDE_PET9},  {"RIDE_PET42", RIDE_PET10},
      {"RIDE_PET43", RIDE_PET11}, {"RIDE_PET44", RIDE_PET12},
      {"RIDE_PET45", RIDE_PET13}, {"RIDE_PET46", RIDE_PET14},
      {"RIDE_PET47", RIDE_PET15}, {"RIDE_PET48", RIDE_PET16},
      {"RIDE_PET49", RIDE_PET17}, {"RIDE_PET50", RIDE_PET18},
      {"RIDE_PET51", RIDE_PET19}, {"RIDE_PET52", RIDE_PET20},
      {"RIDE_PET53", RIDE_PET21}, {"RIDE_PET54", RIDE_PET22},
      {"RIDE_PET55", RIDE_PET23}, {"RIDE_PET56", RIDE_PET24},
      {"RIDE_PET57", RIDE_PET25}, {"RIDE_PET58", RIDE_PET26},
      {"RIDE_PET59", RIDE_PET27}, {"RIDE_PET60", RIDE_PET28},
      {"RIDE_PET61", RIDE_PET29}, {"RIDE_PET62", RIDE_PET30},
      {"RIDE_PET63", RIDE_PET31},
#ifdef _ADD_RIDE_CF
      {"RIDE_PET64", RIDE_PET0},  {"RIDE_PET65", RIDE_PET1},
      {"RIDE_PET66", RIDE_PET2},  {"RIDE_PET67", RIDE_PET3},
      {"RIDE_PET68", RIDE_PET4},  {"RIDE_PET69", RIDE_PET5},
      {"RIDE_PET70", RIDE_PET6},  {"RIDE_PET71", RIDE_PET7},
      {"RIDE_PET72", RIDE_PET8},  {"RIDE_PET73", RIDE_PET9},
      {"RIDE_PET74", RIDE_PET10}, {"RIDE_PET75", RIDE_PET11},
      {"RIDE_PET76", RIDE_PET12}, {"RIDE_PET77", RIDE_PET13},
      {"RIDE_PET78", RIDE_PET14}, {"RIDE_PET79", RIDE_PET15},
      {"RIDE_PET80", RIDE_PET16}, {"RIDE_PET81", RIDE_PET17},
      {"RIDE_PET82", RIDE_PET18}, {"RIDE_PET83", RIDE_PET19},
      {"RIDE_PET84", RIDE_PET20}, {"RIDE_PET85", RIDE_PET21},
      {"RIDE_PET86", RIDE_PET22}, {"RIDE_PET87", RIDE_PET23},
      {"RIDE_PET88", RIDE_PET24}, {"RIDE_PET89", RIDE_PET25},
      {"RIDE_PET90", RIDE_PET26}, {"RIDE_PET91", RIDE_PET27},
      {"RIDE_PET92", RIDE_PET28}, {"RIDE_PET93", RIDE_PET29},
      {"RIDE_PET94", RIDE_PET30}, {"RIDE_PET95", RIDE_PET31},
#endif
#endif
  };

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;

  if (getStringFromIndexWithDelim(itemarg, "|", 3, buf1, sizeof(buf1)) == FALSE)
    ridetrans = 0;
  else
    ridetrans = atoi(buf1);
  if (getStringFromIndexWithDelim(itemarg, "|", 1, buf1, sizeof(buf1)) == FALSE)
    return;
  for (i = 0; i < MAXNOINDEX; i++) {
    if (!strcmp(NewRides[i].arg, buf1)) {
      int LRCode;
      if (CHAR_getInt(charaindex, CHAR_TRANSMIGRATION) < ridetrans) {
        char token[256];
        if (getStringFromIndexWithDelim(itemarg, "|", 2, buf1, sizeof(buf1)) !=
            FALSE) {
          sprintf(token, "必须%d转人以上才能学习骑%s。", ridetrans, buf1);
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
          return;
        }
      }
#ifdef _RIDE_CF
#ifdef _ADD_RIDE_CF
      if (i < 32) {
        LRCode = CHAR_getInt(charaindex, CHAR_LOWRIDEPETS);
        LRCode = LRCode | NewRides[i].Code;
        CHAR_setInt(charaindex, CHAR_LOWRIDEPETS, LRCode);
      } else if (i < 64) {
        LRCode = CHAR_getInt(charaindex, CHAR_LOWRIDEPETS1);
        LRCode = LRCode | NewRides[i].Code;
        CHAR_setInt(charaindex, CHAR_LOWRIDEPETS1, LRCode);
      } else if (i < 96) {
        LRCode = CHAR_getInt(charaindex, CHAR_HIGHRIDEPET2);
        LRCode = LRCode | NewRides[i].Code;
        CHAR_setInt(charaindex, CHAR_HIGHRIDEPET2, LRCode);
      }
#else
      if (i < 32) {
        LRCode = CHAR_getInt(charaindex, CHAR_LOWRIDEPETS);
        LRCode = LRCode | NewRides[i].Code;
        CHAR_setInt(charaindex, CHAR_LOWRIDEPETS, LRCode);
      } else if (i < 64) {
        LRCode = CHAR_getInt(charaindex, CHAR_LOWRIDEPETS1);
        LRCode = LRCode | NewRides[i].Code;
        CHAR_setInt(charaindex, CHAR_LOWRIDEPETS1, LRCode);
      }
#endif
#else
      LRCode = CHAR_getInt(charaindex, CHAR_LOWRIDEPETS);
      LRCode = LRCode | NewRides[i].Code;
      CHAR_setInt(charaindex, CHAR_LOWRIDEPETS, LRCode);
#endif
      if (getStringFromIndexWithDelim(itemarg, "|", 2, buf1, sizeof(buf1)) !=
          FALSE) {
        char token[256];

        sprintf(token, "啪铰阑 硅奎嚼聪促 (%s)。", buf1);
        CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
        CHAR_DelItem(charaindex, haveitemindex);
        CHAR_sendStatusString(charaindex, "x");
      }
      break;
    }
  }
}

#endif

#ifdef _ITEM_EDITBASES
void ITEM_useFusionEditBase(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  int work[4] = {0, 0, 0, 0};
  int anhour = PETFEEDTIME; //宠物  食时间(单位：秒)

#ifdef _PET_FUSION_2
  int fusion_type = FUSIONTYPE_ALL;
  int raise_type;
  char *itemarg = NULL;
  char petname[32];
  strcpy(petname, CHAR_getChar(toindex, CHAR_USERPETNAME));
  if (strlen(petname) == 0) {
    strcpy(petname, CHAR_getChar(toindex, CHAR_NAME));
  }
#endif
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
    CHAR_talkToCli(charaindex, -1, "物品仅限宠物使用。", CHAR_COLORYELLOW);
    return;
  }
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (CHAR_getInt(toindex, CHAR_FUSIONBEIT) >= 1 &&
      CHAR_getInt(toindex, CHAR_FUSIONRAISE) > 0) { //检查是否为融合宠
    int time_l;
    int nowTime; // Robin fix

#ifdef _PET_FUSION_2 // 必须喂食专用饲料
    fusion_type = CHAR_getInt(toindex, CHAR_FUSIONTYPE);
    // raise_type = atoi( ITEM_getChar( itemindex, ITEM_ARGUMENT));
    itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
    if (strstr(itemarg, "ALL")) {
      raise_type = FUSIONTYPE_ALL;
    } else if (strstr(itemarg, "LAND")) {
      raise_type = FUSIONTYPE_LAND;
    } else if (strstr(itemarg, "OCEAN")) {
      raise_type = FUSIONTYPE_OCEAN;
    } else {
      raise_type = FUSIONTYPE_ALL;
    }
    if (fusion_type != raise_type && raise_type != FUSIONTYPE_ALL &&
        fusion_type != FUSIONTYPE_ALL) {
      char msg[1024];
      sprintf(msg, "（%s 似乎不能吃这种食物）", petname);
      CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
      return;
    }
#endif
    time_l = CHAR_getInt(toindex, CHAR_FUSIONTIMELIMIT);
    nowTime = (int)time(NULL);
#if 1 // 消化药
    {
      char *arg = NULL;
      char deltime[8];
      char msg[1024];
      arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
      if (arg != "\0" && !strncmp(arg, "消", 2)) {
        //				sscanf( arg, "消 %d", &deltime);
        getStringFromIndexWithDelim(arg, "|", 2, deltime, sizeof(deltime));
        time_l -= (atoi(deltime) * 60);
        CHAR_setInt(toindex, CHAR_FUSIONTIMELIMIT, time_l);
        CHAR_DelItem(charaindex, haveitemindex);
#ifdef _PET_FUSION_2
        if (fusion_type == FUSIONTYPE_OCEAN) {
          sprintf(msg, "加快海宠蛋的消化速度 %s 分钟。", deltime);
        } else
#endif
        {
          sprintf(msg, "加快宠物蛋的消化速度 %s 分钟。", deltime);
        }
        CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
        if ((time_l + anhour) <= nowTime) {
          int min, sec, deftime;
          deftime = nowTime - (time_l + anhour);
          min = deftime / 60;
          sec = deftime % 60;
#ifdef _PET_FUSION_2
          if (fusion_type == FUSIONTYPE_OCEAN) {
            sprintf(msg, "海宠蛋已经可以喂食了，超过喂食时间%d分%d秒。", min,
                    sec);
          } else
#endif
          {
            sprintf(msg, "宠物蛋已经可以喂食了，超过喂食时间%d分%d秒。", min,
                    sec);
          }
          CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
          return;
        }
      }
    }
#endif
    if ((time_l + anhour) >
        nowTime) { //现在的时间要超过(time_l+anhour)才可以  食
      char Mess1[256];
      int min, sec, deftime;
      // int nowTime = (int)time(NULL);
      deftime = (time_l + anhour) - nowTime;
      min = deftime / 60;
      sec = deftime % 60;
#ifdef _PET_FUSION_2
      if (fusion_type == FUSIONTYPE_OCEAN) {
        sprintf(Mess1, "海宠蛋需过%d分%d秒才可喂食。", min, sec);
      } else
#endif
      {
        sprintf(Mess1, "宠物蛋需过%d分%d秒才可喂食。", min, sec);
      }
      CHAR_talkToCli(charaindex, -1, Mess1, CHAR_COLORYELLOW);
      return;
    }
#ifdef _PET_FUSION_2 // 海宠蛋检查喂养地图
    if (CHAR_getInt(toindex, CHAR_FUSIONRAISE) % 3 == 0 &&
        fusion_type == FUSIONTYPE_OCEAN &&
        CHAR_getInt(charaindex, CHAR_FLOOR) != 777) {
      int raise = CHAR_getInt(toindex, CHAR_FUSIONRAISE);
      int floor = CHAR_getInt(charaindex, CHAR_FLOOR);
      char msg[1024];

      if (raise == 18 && floor != 8009) {
        sprintf(msg,
                "%s：好想念大海唷，可以带我去沙己夫换气道吗？我想在那边进食。",
                petname);
        CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
        return;
      }
      if (raise == 15 && floor != 8000) {
        sprintf(msg,
                "%s：主人，我还是不太习惯陆地的生活，可以带我去史东尔换气道透透"
                "气吗？",
                petname);
        CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
        return;
      }
      if (raise == 12 && floor != 21015) {
        sprintf(msg,
                "%s：主人，我的体力似乎有点衰落，可以让我接受大地女神的祝福吗？"
                "(琉璃洞窟15楼)",
                petname);
        CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
        return;
      }
      if (raise == 9 && floor != 21215) {
        sprintf(msg,
                "%s：主人，我一直觉得自己快要窒息了，可以让我接受水之女神的祝福"
                "吗？(碧青洞窟15F)",
                petname);
        CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
        return;
      }
      if (raise == 6 && floor != 30016) {
        sprintf(msg,
                "%s：咳咳咳～健康状况似乎有点差，可以让我接受火之女神的祝福吗？"
                "(深红洞窟地下9楼)",
                petname);
        CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
        return;
      }
      if (raise == 3 && floor != 11215) {
        sprintf(msg,
                "%s：主人，我的反应老是很迟钝，可以让我接受风之女神的祝福吗？("
                "玄黄洞窟15F)",
                petname);
        CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
        return;
      }
    }
#endif
    //---------------------------------------------
    work[3] = CHAR_getInt(toindex, CHAR_EVOLUTIONBASEVTL);
    work[0] = CHAR_getInt(toindex, CHAR_EVOLUTIONBASESTR);
    work[1] = CHAR_getInt(toindex, CHAR_EVOLUTIONBASETGH);
    work[2] = CHAR_getInt(toindex, CHAR_EVOLUTIONBASEDEX);
    PET_showEditBaseMsg(charaindex, toindex, itemindex, work);
    CHAR_setInt(toindex, CHAR_EVOLUTIONBASEVTL, work[3]);
    CHAR_setInt(toindex, CHAR_EVOLUTIONBASESTR, work[0]);
    CHAR_setInt(toindex, CHAR_EVOLUTIONBASETGH, work[1]);
    CHAR_setInt(toindex, CHAR_EVOLUTIONBASEDEX, work[2]);
    char nbmsg[1024];
    sprintf(nbmsg,
            "[%s]剩余喂养次数：%d，血能力%d 攻能力：%d 防能力：%d 敏能力：%d",
            petname, CHAR_getInt(toindex, CHAR_FUSIONRAISE) - 1, work[3],
            work[0], work[1], work[2]);
    CHAR_talkToCli(charaindex, -1, nbmsg, CHAR_COLORYELLOW);

    //---------------------------------------------
    /*
    LogPetPointChange(
            CHAR_getChar( charaindex, CHAR_NAME ),
            CHAR_getChar( charaindex, CHAR_CDKEY ),
            CHAR_getChar( charaindex, CHAR_NAME),
            toindex, 4,
            CHAR_getInt( toindex, CHAR_LV),
            "item_use",
            CHAR_getInt( charaindex, CHAR_FLOOR),
            CHAR_getInt( charaindex, CHAR_X ),
            CHAR_getInt( charaindex, CHAR_Y )
    );*/
    /*
    LogPetFeed(
            CHAR_getChar( charaindex, CHAR_NAME),
            CHAR_getChar( charaindex, CHAR_CDKEY),
            CHAR_getChar( toindex, CHAR_NAME),
            toindex,
            CHAR_getInt( toindex, CHAR_LV),
            ITEM_getChar( itemindex, CHAR_NAME), // Key
            CHAR_getInt( charaindex, CHAR_FLOOR),
            CHAR_getInt( charaindex, CHAR_X),
            CHAR_getInt( charaindex, CHAR_Y),
            CHAR_getChar( toindex, CHAR_UNIQUECODE) );
    */
#ifdef _PET_EVOLUTION
    {
      int raise = CHAR_getInt(toindex, CHAR_FUSIONRAISE);
      CHAR_setInt(toindex, CHAR_FUSIONRAISE, --raise);

      CHAR_setInt(toindex, CHAR_FUSIONTIMELIMIT, nowTime);
      if (CHAR_getInt(toindex, CHAR_FUSIONRAISE) <= 0) { //孵化
        char buf[256], buf1[256];
        int newindex;
        sprintf(buf, "蛋〈%s〉孵化成", CHAR_getUseName(toindex));

        newindex = EVOLUTION_createPetFromEnemyIndex(charaindex, toindex, 1);
        if (!CHAR_CHECKINDEX(newindex)) {
          CHAR_talkToCli(charaindex, -1, "宠物孵化发生错误。",
                         CHAR_COLORYELLOW);
          return;
        }
        sprintf(buf1, "〈%s〉。", CHAR_getChar(newindex, CHAR_NAME));
        strcat(buf, buf1);
        CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
        /*
        LogPetFeed(
                CHAR_getChar( charaindex, CHAR_NAME),
                CHAR_getChar( charaindex, CHAR_CDKEY),
                CHAR_getChar( newindex, CHAR_NAME),
                toindex,
                CHAR_getInt( newindex, CHAR_LV),
                buf, // Key
                CHAR_getInt( charaindex, CHAR_FLOOR),
                CHAR_getInt( charaindex, CHAR_X),
                CHAR_getInt( charaindex, CHAR_Y),
                CHAR_getChar( newindex, CHAR_UNIQUECODE) );
        */
      }
    }
#endif
  } else {
    CHAR_talkToCli(charaindex, -1, "？？？", CHAR_COLORYELLOW);
  }
  CHAR_DelItem(charaindex, haveitemindex);
  return;
}
#endif

#ifdef _THROWITEM_ITEMS
static int Niceitem = 0;
int golds[3] = {10000, 20000, 50000};
int items1[18] = {13092, 13091, 20439, 20417, 1284,  20172, 18210, 19014, 18360,
                  18362, 18364, 18359, 18356, 18357, 18510, 20418, 20419, 1452};
int items2[11] = {15842, 16136, 14334, 14034, 14634, 14934,
                  15534, 14934, 16432, 17057, 19695};

int items3[10] = {16014, 16314, 14515, 14215, 14815,
                  15115, 15715, 15295, 16552, 17157};

int items4[18] = {14516, 14513, 14216, 14213, 14816, 14813,
                  15116, 15716, 15415, 17360, 20279, 20282,
                  20276, 20270, 20288, 20290, 20291, 20289};
int items5[5] = {20280, 20283, 20277, 20271, 20274};
int items6[5] = {20284, 20272, 20275, 20281, 20278};
void ITEM_ThrowItemBox(int charaindex, int toindex, int haveitemindex) {
  int i, ret, Iindex, ItemID = -1, itemindex;
  char token[256];

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  if (Niceitem > 10)
    ret = rand() % 920;
  else
    ret = rand() % 1000;

  for (i = CHAR_STARTITEMARRAY; i < CHAR_getMaxInventory(charaindex); i++) {
    Iindex = CHAR_getItemIndex(charaindex, i);
    if (!ITEM_CHECKINDEX(itemindex))
      continue;
    if (itemindex == Iindex) {
      CHAR_DelItem(charaindex, i);

      LogItem(CHAR_getChar(charaindex, CHAR_NAME),
              CHAR_getChar(charaindex, CHAR_CDKEY), itemindex, "WarpManDelItem",
              CHAR_getInt(charaindex, CHAR_FLOOR),
              CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
              ITEM_getChar(itemindex, ITEM_UNIQUECODE),
              ITEM_getChar(itemindex, ITEM_NAME),
              ITEM_getInt(itemindex, ITEM_ID), charaindex);
      break;
    }
  }
  if (i >= CHAR_getMaxInventory(charaindex)) {
    return;
  }

  if (ret <= 774) {
    int Golds = 0;
    Golds = golds[RAND(0, 2)];
    CHAR_AddGold(charaindex, Golds);
  } else {
    if (ret < 924) {
      ItemID = items1[RAND(0, 17)];
    } else if (ret < 964) {
      ItemID = items2[RAND(0, 10)];
    } else if (ret < 984) {
      ItemID = items3[RAND(0, 9)];
      Niceitem++;
    } else if (ret < 994) {
      ItemID = items4[RAND(0, 17)];
      Niceitem++;
    } else if (ret < 999) {
      ItemID = items5[RAND(0, 4)];
      Niceitem++;
    } else {
      ItemID = items6[RAND(0, 4)];
      Niceitem++;
    }

    Iindex = ITEM_makeItemAndRegist(ItemID);
    if (!ITEM_CHECKINDEX(Iindex)) {
      return;
    }
    ret = CHAR_addItemSpecificItemIndex(charaindex, Iindex);
    if (ret < 0 || ret >= CHAR_getMaxInventory(charaindex)) {
      ITEM_endExistItemsOne(Iindex);
      return;
    }
    sprintf(token, "裙垫窍促 %s", ITEM_getChar(Iindex, ITEM_NAME));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    CHAR_sendItemDataOne(charaindex, ret);
  }
}
#endif

#ifdef _ITEM_LOVERPARTY
void ITEM_LoverSelectUser(int charaindex, int toindex, int haveitemindex) {
  int itemindex, i;
  int playernum = CHAR_getPlayerMaxNum();

  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    CHAR_talkToCli(charaindex, -1, "物品仅限人物使用。", CHAR_COLORYELLOW);
    return;
  }
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (ITEM_getInt(itemindex, ITEM_TYPE) != 16) {
    ITEM_setInt(itemindex, ITEM_TYPE, 16);
    return;
  }
  if (!strcmp(ITEM_getChar(itemindex, ITEM_FORUSERNAME), "") ||
      !strcmp(ITEM_getChar(itemindex, ITEM_FORUSERCDKEY), "")) { //寻找设定对象
    if (charaindex == toindex) {
      CHAR_talkToCli(charaindex, -1, "不能选择自己使用。", CHAR_COLORYELLOW);
      return;
    }
    ITEM_setChar(itemindex, ITEM_FORUSERNAME, CHAR_getChar(toindex, CHAR_NAME));
    ITEM_setChar(itemindex, ITEM_FORUSERCDKEY,
                 CHAR_getChar(toindex, CHAR_CDKEY));
    ITEM_setInt(itemindex, ITEM_TARGET, 0);
    {
      char token[256];
      sprintf(token, "%s(%s)", ITEM_getChar(itemindex, ITEM_SECRETNAME),
              CHAR_getChar(toindex, CHAR_NAME));
      ITEM_setChar(itemindex, ITEM_SECRETNAME, token);
      sprintf(token, "传送对象设定为%s。", CHAR_getChar(toindex, CHAR_NAME));
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    }
    CHAR_sendItemDataOne(charaindex, haveitemindex);
  } else {
    char buf1[256];
    // if( CHAR_getInt( charaindex, CHAR_FLOOR) == 117 || CHAR_getInt(
    // charaindex, CHAR_FLOOR) == 887 || CHAR_getInt( charaindex, CHAR_FLOOR) ==
    // 1042 || CHAR_getInt( charaindex, CHAR_FLOOR) == 2032 || CHAR_getInt(
    // charaindex, CHAR_FLOOR) == 3032 || CHAR_getInt( charaindex, CHAR_FLOOR)
    // == 4032 || CHAR_getInt( charaindex, CHAR_FLOOR) == 5032 || CHAR_getInt(
    // charaindex, CHAR_FLOOR) == 6032 || CHAR_getInt( charaindex, CHAR_FLOOR)
    // == 7032 || CHAR_getInt( charaindex, CHAR_FLOOR) == 8032 || CHAR_getInt(
    // charaindex, CHAR_FLOOR) == 9032 ){
    if (checkUnlawWarpFloor(CHAR_getInt(charaindex, CHAR_FLOOR))) {
      CHAR_talkToCli(charaindex, -1, "你的所在位置无法传送。",
                     CHAR_COLORYELLOW);
      return;
    }

    for (i = 0; i < playernum; i++) {
      int itemmaxuse = 0;
      if (!CHAR_CHECKINDEX(i))
        continue;
      if (!strcmp(ITEM_getChar(itemindex, ITEM_FORUSERNAME),
                  CHAR_getChar(i, CHAR_NAME)) &&
          !strcmp(ITEM_getChar(itemindex, ITEM_FORUSERCDKEY),
                  CHAR_getChar(i, CHAR_CDKEY))) {
        int floor, x, y;
        char token[256];
        floor = CHAR_getInt(i, CHAR_FLOOR);
        x = CHAR_getInt(i, CHAR_X);
        y = CHAR_getInt(i, CHAR_Y);

        // if( floor == 887 || floor == 117 ||
        //	floor == 1042 || floor == 2032 || floor == 3032 || floor == 4032
        //|| 	floor == 5032 || floor == 6032 ||floor == 7032 || floor == 8032 ||
        //floor == 9032 ){
        if (checkUnlawWarpFloor(floor)) {
          CHAR_talkToCli(charaindex, -1, "对象所在地方无法传送。",
                         CHAR_COLORYELLOW);
          return;
        }

        CHAR_warpToSpecificPoint(charaindex, floor, x, y);
        sprintf(token, "%s藉由戒指传送到你身边来。",
                CHAR_getChar(charaindex, CHAR_NAME));
        CHAR_talkToCli(i, -1, token, CHAR_COLORYELLOW);
        sprintf(token, "藉由戒指传送到%s身边。", CHAR_getChar(i, CHAR_NAME));
        CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
        CHAR_DischargePartyNoMsg(charaindex); //解散团队
#ifdef _ITEM_MAXUSERNUM
        itemmaxuse = ITEM_getInt(itemindex, ITEM_DAMAGEBREAK);
#endif
        if (itemmaxuse != -1) {
          itemmaxuse--;
#ifdef _ITEM_MAXUSERNUM
          ITEM_setInt(itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
#endif
          if (itemmaxuse < 1) {
            sprintf(buf1, "道具 %s消失了。",
                    ITEM_getChar(itemindex, ITEM_NAME));
            CHAR_talkToCli(charaindex, -1, buf1, CHAR_COLORYELLOW);
            CHAR_DelItem(charaindex, haveitemindex);
            return;
          } else {
            sprintf(buf1, "传送至目标对象所在位置，可使用次数剩馀%d次。",
                    itemmaxuse);
            ITEM_setChar(itemindex, ITEM_EFFECTSTRING, buf1);
          }
        } else {
          itemmaxuse = 10;
          ITEM_setInt(itemindex, ITEM_DAMAGEBREAK, itemmaxuse);
          sprintf(buf1, "传送至目标对象所在位置，可使用次数剩余%d次。",
                  itemmaxuse);
          ITEM_setChar(itemindex, ITEM_EFFECTSTRING, buf1);
        }

        CHAR_DischargePartyNoMsg(charaindex);
        CHAR_complianceParameter(charaindex);
        CHAR_sendItemDataOne(charaindex, haveitemindex);
        return;
      }
    }
    CHAR_talkToCli(charaindex, -1, "对象不在此伺服器或不在线上。",
                   CHAR_COLORYELLOW);
  }
}
#endif

#ifdef _ANGEL_SUMMON
void ITEM_AngelToken(int charaindex, int toindex, int haveitemindex) {

  Use_AngelToken(charaindex, toindex, haveitemindex);
}

void ITEM_HeroToken(int charaindex, int toindex, int haveitemindex) {

  Use_HeroToken(charaindex, toindex, haveitemindex);
}
#endif

#ifdef _HALLOWEEN_EFFECT
void ITEM_MapEffect(int charaindex, int toindex, int haveitemindex) {
  int itemindex, floor;
  char *pActionNumber, szMsg[128];

  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  // 找出玩家所在的地图编号
  floor = CHAR_getInt(charaindex, CHAR_FLOOR);
  // 找出道具要放的特效的编号
  pActionNumber = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  sprintf(szMsg, "%d 8 %s", floor, pActionNumber);
  // 执行
  CHAR_CHAT_DEBUG_effect(charaindex, szMsg);
  CHAR_DelItemMess(charaindex, haveitemindex, 0);
}
#endif

void ITEM_changePetOwner(int charaindex, int toindex, int haveitemindex) {
  int itemindex, i;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  if (CHAR_CHECKINDEX(toindex) == FALSE)
    return;

  // ITEM_useRecovery_Field(	charaindex, toindex, haveitemindex );

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
    CHAR_talkToCli(charaindex, -1, "这只能用在宠物身上喔。", CHAR_COLORYELLOW);
    return;
  }

  if (!strcmp(CHAR_getChar(toindex, CHAR_NPCARGUMENT), "") ||
      !strcmp(CHAR_getChar(toindex, CHAR_NPCARGUMENT),
              CHAR_getChar(charaindex, CHAR_CDKEY))) {
    CHAR_talkToCli(charaindex, -1, "这只宠物本来就是你的，并不需要使用这个呀。",
                   CHAR_COLORYELLOW);
    return;
  }

  CHAR_setChar(toindex, CHAR_NPCARGUMENT, "");

  for (i = 0; i < CHAR_MAXPETHAVE; i++)
    if (CHAR_getCharPet(charaindex, i) == toindex)
      CHAR_send_K_StatusString(
          charaindex, i, CHAR_K_STRING_NAME | CHAR_K_STRING_CHANGENAMEFLG);

  CHAR_talkToCli(charaindex, -1, "现在你可以帮这只宠物改名字了。",
                 CHAR_COLORYELLOW);

  CHAR_DelItemMess(charaindex, haveitemindex, 0);
}

#ifdef _DEL_DROP_GOLD
void GOLD_DeleteTimeCheckLoop(void) {
  int amount = 0;
  int objindex;
  int objmaxnum;

  objmaxnum = OBJECT_getNum();
  for (objindex = 0; objindex < objmaxnum; objindex++) {
    if (CHECKOBJECT(objindex) == FALSE)
      continue;
    if (OBJECT_getType(objindex) != OBJTYPE_GOLD)
      continue;

    //		amount = OBJECT_getIndex( objindex);
    //		if( amount >= 10000) continue;

    if ((int)NowTime.tv_sec >
        (int)(OBJECT_getTime(objindex) + getGolddeletetime())) {

      LogStone(-1, "NULL", "NULL", amount, 0, "Del(删除过时金钱)",
               OBJECT_getFloor(objindex), OBJECT_getX(objindex),
               OBJECT_getY(objindex));

      CHAR_ObjectDelete(objindex);
    }
  }
}

void GOLD_DeleteTimeCheckOne(int objindex) {
  int amount = 0;

  if (CHECKOBJECT(objindex) == FALSE)
    return;
  if (OBJECT_getType(objindex) != OBJTYPE_GOLD)
    return;

  //	amount = OBJECT_getIndex( objindex);
  //	if( amount >= 10000) return;

  if ((int)NowTime.tv_sec >
      (int)(OBJECT_getTime(objindex) + getGolddeletetime())) {

    LogStone(-1, "NULL", "NULL", amount, 0, "Del(删除过时金钱)",
             OBJECT_getFloor(objindex), OBJECT_getX(objindex),
             OBJECT_getY(objindex));

    CHAR_ObjectDelete(objindex);
  }
}

#endif

#ifdef _TIME_TICKET
void ITEM_timeticketEx(int charaindex, int toindex, int haveitemindex,
                       int flag);
void ITEM_timeticket(int charaindex, int toindex, int haveitemindex) {
  if (check_TimeTicketMap(CHAR_getInt(charaindex, CHAR_FLOOR)) == FALSE) {
    CHAR_setWorkInt(charaindex, CHAR_WORKTICKETTIME, 0);
    CHAR_setWorkInt(charaindex, CHAR_WORKTICKETTIMESTART, 0);
    CHAR_warpToSpecificPoint(charaindex, 7025, 143, 134);
  }
  ITEM_timeticketEx(charaindex, toindex, haveitemindex, 0);
}

void ITEM_timeticketEx(int charaindex, int toindex, int haveitemindex,
                       int flag) {
  int itemindex;
  int addtime;
  int nowtime = time(NULL);
  int tickettime;
  int lefttime;
  char msg[1024];

  if (!CHAR_CHECKINDEX(charaindex))
    return;

  if (check_TimeTicketMap(CHAR_getInt(charaindex, CHAR_FLOOR)) == FALSE &&
      flag == 0) {
    CHAR_talkToCli(charaindex, -1, "这个地点不可使用。", CHAR_COLORYELLOW);
    return;
  }

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  tickettime = CHAR_getWorkInt(charaindex, CHAR_WORKTICKETTIME);
  if (tickettime > nowtime + 20) {
    CHAR_talkToCli(charaindex, -1, "时间必须剩下２０秒以内才可使用。",
                   CHAR_COLORYELLOW);
    return;
  }
  // 第一次使用的话
  if (tickettime == 0) {
    tickettime = nowtime;
    CHAR_setWorkInt(charaindex, CHAR_WORKTICKETTIMESTART, nowtime);
  }
  // 战斗中且超过时限时使用
  // if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) !=
  // BATTLE_CHARMODE_NONE
  //		&& tickettime < nowtime ) {
  // tickettime = nowtime;
  //	sprintf(msg, "时间不足%d秒。", nowtime - tickettime );
  //	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
  //}

  addtime = atoi(ITEM_getChar(itemindex, ITEM_ARGUMENT));
  tickettime += addtime;
  CHAR_setWorkInt(charaindex, CHAR_WORKTICKETTIME, tickettime);
  lefttime = tickettime - nowtime;
  if (lefttime > 0)
    sprintf(msg, "时间增加%d秒，还剩下%d分%d秒。", addtime, lefttime / 60,
            lefttime % 60);
  else
    sprintf(msg, "时间增加%d秒，还不足%d分%d秒。", addtime, (-lefttime) / 60,
            (-lefttime) % 60);
  CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif
#ifdef _ITEM_SETLOVER
void ITEM_SetLoverUser(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  int playernum = CHAR_getPlayerMaxNum();
  char token[256], szMsg[128];
  int floor = CHAR_getInt(charaindex, CHAR_FLOOR);

  if (!strcmp(CHAR_getChar(charaindex, CHAR_LOVE), "YES")) {
    CHAR_talkToCli(charaindex, -1, "重婚是违法的！您已经结婚了哦~",
                   CHAR_COLORYELLOW);
    return;
  }
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    CHAR_talkToCli(charaindex, -1, "只能和玩家结婚哦", CHAR_COLORYELLOW);
    return;
  }
  if (CHAR_getInt(charaindex, CHAR_TRANSMIGRATION) < 3 ||
      CHAR_getInt(toindex, CHAR_TRANSMIGRATION) < 3) {
    CHAR_talkToCli(charaindex, -1, "如果想结婚的话，双方必须3转以上哦~",
                   CHAR_COLORYELLOW);
    return;
  }
  if (charaindex == toindex) {
    CHAR_talkToCli(charaindex, -1, "难道您想和自己结婚吗？", CHAR_COLORYELLOW);
    return;
  }
  if (IsMale(charaindex) == IsMale(toindex)) {
    CHAR_talkToCli(charaindex, -1, "同性怎么结婚呢？", CHAR_COLORYELLOW);
    return;
  }
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  int id = ITEM_getInt(itemindex, ITEM_ID);
  itemindex = ITEM_makeItemAndRegist(id);

  if (strcmp(CHAR_getChar(charaindex, CHAR_LOVE), "YES") &&
      strcmp(CHAR_getChar(toindex, CHAR_LOVE),
             "YES")) //第二次判断双方是否已经结婚
  {
    if (itemindex != -1) {
      CHAR_setChar(charaindex, CHAR_LOVERID, CHAR_getChar(toindex, CHAR_CDKEY));
      CHAR_setChar(charaindex, CHAR_LOVERNAME,
                   CHAR_getChar(toindex, CHAR_NAME));
      if (strcmp(CHAR_getChar(toindex, CHAR_LOVERID),
                 CHAR_getChar(charaindex, CHAR_CDKEY)) == 0 &&
          strcmp(CHAR_getChar(toindex, CHAR_LOVERNAME),
                 CHAR_getChar(charaindex, CHAR_NAME)) == 0) {
        int i;
        sprintf(token, "已经答应了%s的求婚", CHAR_getChar(toindex, CHAR_NAME));
        CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
        sprintf(token, "%s已经响应了你的求婚",
                CHAR_getChar(charaindex, CHAR_NAME));
        CHAR_talkToCli(toindex, -1, token, CHAR_COLORYELLOW);
        CHAR_setChar(toindex, CHAR_LOVE, "YES");
        CHAR_setChar(charaindex, CHAR_LOVE, "YES");
        CHAR_setChar(charaindex, CHAR_LOVERID,
                     CHAR_getChar(toindex, CHAR_CDKEY));
        CHAR_setChar(charaindex, CHAR_LOVERNAME,
                     CHAR_getChar(toindex, CHAR_NAME));
        CHAR_setChar(toindex, CHAR_LOVERID,
                     CHAR_getChar(charaindex, CHAR_CDKEY));
        CHAR_setChar(toindex, CHAR_LOVERNAME,
                     CHAR_getChar(charaindex, CHAR_NAME));
        sprintf(szMsg, "%d 8 101883", floor);
        CHAR_CHAT_DEBUG_effect(charaindex, szMsg);
        CHAR_DelItem(charaindex, haveitemindex);
        for (i = 0; i < playernum; i++) {
          if (CHAR_CHECKINDEX(i) == FALSE)
            continue;
          sprintf(token, "恭喜%s和%s新婚之喜，恭祝他们百年好合.白头到老.",
                  CHAR_getChar(charaindex, CHAR_NAME),
                  CHAR_getChar(toindex, CHAR_NAME));
          CHAR_talkToCli(i, -1, token, CHAR_COLORRED);
        }
        return;
      } else {
        int emptyitemindexinchara = CHAR_findEmptyItemBox(toindex);

        if (emptyitemindexinchara < 0) {
          sprintf(token, "%s物品栏已满。求婚失败",
                  CHAR_getChar(toindex, CHAR_NAME));
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
          sprintf(token, "%s向您发起求婚，但您的物品栏已满。",
                  CHAR_getChar(charaindex, CHAR_NAME));
          CHAR_talkToCli(toindex, -1, token, CHAR_COLORYELLOW);
        } else {
          CHAR_setItemIndex(toindex, emptyitemindexinchara, itemindex);
          sprintf(token, "%s向%s求婚的戒指!同意求婚请使用!",
                  CHAR_getChar(charaindex, CHAR_NAME),
                  CHAR_getChar(toindex, CHAR_NAME));
          ITEM_setChar(itemindex, ITEM_EFFECTSTRING, token);
          CHAR_sendItemDataOne(toindex, emptyitemindexinchara);
          ITEM_endExistItemsOne(itemindex);
        }

        sprintf(token, "%s向您求婚，获得对方的%s",
                CHAR_getChar(charaindex, CHAR_NAME),
                ITEM_getChar(itemindex, ITEM_NAME));
        CHAR_talkToCli(toindex, -1, token, CHAR_COLORYELLOW);
        sprintf(token, "已经向%s发起求婚", CHAR_getChar(toindex, CHAR_NAME));
        CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
        CHAR_DelItem(charaindex, haveitemindex);
      }
      CHAR_talkToCli(charaindex, -1, "请耐心等待对方回应！", CHAR_COLORYELLOW);
    } else {
      sprintf(token, "求婚失败，戒指无法创建!");
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
      return;
    }
  }
}

void ITEM_LoverWarp(int charaindex, int toindex, int haveitemindex) {
  int itemindex, i;
  int playernum = CHAR_getPlayerMaxNum();
  //检查是否结婚
  if (!strcmp(CHAR_getChar(charaindex, CHAR_LOVE), "YES")) {
    for (i = 0; i < playernum; i++) {
      if (CHAR_CHECKINDEX(i) == FALSE)
        continue;
      if (strcmp(CHAR_getChar(i, CHAR_LOVE), "YES") == 0 &&
          strcmp(CHAR_getChar(charaindex, CHAR_LOVERID),
                 CHAR_getChar(i, CHAR_CDKEY)) == 0 &&
          strcmp(CHAR_getChar(charaindex, CHAR_LOVERNAME),
                 CHAR_getChar(i, CHAR_NAME)) == 0) {
        break;
      }
    } //从index中对比LOVERNAME
    if (i >= playernum) {
      CHAR_talkToCli(charaindex, -1, "很抱歉，您的爱人不在线哦~",
                     CHAR_COLORYELLOW);
      return;
    }

    if (!CHAR_CHECKINDEX(charaindex))
      return;
    if (!CHAR_CHECKINDEX(i))
      return;

    itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
    if (!ITEM_CHECKINDEX(itemindex))
      return;

    if (checkUnlawWarpFloor(CHAR_getInt(charaindex, CHAR_FLOOR))) {
      CHAR_talkToCli(charaindex, -1,
                     "很抱歉，您的爱人在神秘遥远地方，无法传送。",
                     CHAR_COLORYELLOW);
      return;
    } // UNWARP地图禁止使用
    if (checkUnlawWarpFloor(CHAR_getInt(i, CHAR_FLOOR))) {
      CHAR_talkToCli(charaindex, -1,
                     "很抱歉，您的爱人在神秘遥远地方，无法传送。",
                     CHAR_COLORYELLOW);
      return;
    } // UNWARP地图禁止使用
    if (strcmp(CHAR_getChar(i, CHAR_LOVERID),
               CHAR_getChar(charaindex, CHAR_CDKEY)) == 0 &&
        strcmp(CHAR_getChar(i, CHAR_LOVERNAME),
               CHAR_getChar(charaindex, CHAR_NAME)) == 0) {
      CHAR_DischargePartyNoMsg(charaindex); //解散团队
      int floor, x, y;
      char token[256];
      floor = CHAR_getInt(i, CHAR_FLOOR);
      x = CHAR_getInt(i, CHAR_X);
      y = CHAR_getInt(i, CHAR_Y);

      CHAR_warpToSpecificPoint(charaindex, floor, x, y);
      sprintf(token, "%s:亲爱的，我来了！",
              CHAR_getChar(charaindex, CHAR_NAME));
      CHAR_talkToCli(i, -1, token, CHAR_COLORYELLOW);
      sprintf(token, "已传送到爱人%s身边！", CHAR_getChar(i, CHAR_NAME));
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
      print("\n玩家%s 传送到玩家%s身边", CHAR_getChar(charaindex, CHAR_NAME),
            CHAR_getChar(i, CHAR_NAME));
      return;
    }
  } else
    CHAR_talkToCli(charaindex, -1, "您没有结婚哦~", CHAR_COLORYELLOW);
}

void ITEM_LoverUnmarry(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  int i;
  char token[256];
  int playernum = CHAR_getPlayerMaxNum();
  //检查是否结婚
  if (!strcmp(CHAR_getChar(charaindex, CHAR_LOVE), "YES")) {
    for (i = 0; i < playernum; i++) {
      if (CHAR_CHECKINDEX(i) == FALSE)
        continue;
      if (strcmp(CHAR_getChar(i, CHAR_LOVE), "YES") == 0 &&
          strcmp(CHAR_getChar(charaindex, CHAR_LOVERID),
                 CHAR_getChar(i, CHAR_CDKEY)) == 0 &&
          strcmp(CHAR_getChar(charaindex, CHAR_LOVERNAME),
                 CHAR_getChar(i, CHAR_NAME)) == 0) {
        break;
      }
    } //从index中对比LOVERNAME
    if (i >= playernum) {
      CHAR_talkToCli(charaindex, -1, "很抱歉，您的爱人不在线哦~",
                     CHAR_COLORYELLOW);
      return;
    }

    if (!CHAR_CHECKINDEX(charaindex))
      return;
    if (!CHAR_CHECKINDEX(i))
      return;
    if (!ITEM_CHECKINDEX(itemindex))
      return;

    if (strcmp(ITEM_getChar(itemindex, ITEM_SECRETNAME), "同意离婚戒指")) {
      int id = ITEM_getInt(itemindex, ITEM_ID);
      itemindex = ITEM_makeItemAndRegist(id);

      if (itemindex != -1) {

        int emptyitemindexinchara = CHAR_findEmptyItemBox(i);

        if (emptyitemindexinchara < 0) {
          sprintf(token, "%s物品栏已满。离婚失败", CHAR_getChar(i, CHAR_NAME));
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
          sprintf(token, "%s向您发起离婚，但您的物品栏已满。",
                  CHAR_getChar(charaindex, CHAR_NAME));
          CHAR_talkToCli(i, -1, token, CHAR_COLORYELLOW);
        } else {
          CHAR_setItemIndex(i, emptyitemindexinchara, itemindex);
          ITEM_setChar(itemindex, ITEM_NAME, "同意离婚戒指");
          ITEM_setChar(itemindex, ITEM_SECRETNAME, "同意离婚戒指");
          sprintf(token, "%s向你申请离婚，如果同意请使用该戒指!",
                  CHAR_getChar(charaindex, CHAR_NAME));
          ITEM_setChar(itemindex, ITEM_EFFECTSTRING, token);
          CHAR_sendItemDataOne(i, emptyitemindexinchara);
          ITEM_endExistItemsOne(itemindex);
          sprintf(token, " %s向您提出离婚并把 %s 给你！",
                  CHAR_getChar(charaindex, CHAR_NAME),
                  ITEM_getChar(itemindex, ITEM_NAME));
          CHAR_talkToCli(i, -1, token, CHAR_COLORYELLOW);
          sprintf(token, "已经向%s发起离婚", CHAR_getChar(i, CHAR_NAME));
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
          CHAR_DelItem(charaindex, haveitemindex);
          return;
        }
      }
    } else {
      CHAR_setChar(i, CHAR_LOVE, "");
      CHAR_setChar(i, CHAR_LOVERID, "");
      CHAR_setChar(i, CHAR_LOVERNAME, "");
      CHAR_setChar(charaindex, CHAR_LOVE, "");
      CHAR_setChar(charaindex, CHAR_LOVERID, "");
      CHAR_setChar(charaindex, CHAR_LOVERNAME, "");
      CHAR_talkToCli(i, -1, "双方离婚成功！", CHAR_COLORYELLOW);
      CHAR_talkToCli(charaindex, -1, "双方离婚成功！", CHAR_COLORYELLOW);
    }
    CHAR_DelItem(charaindex, haveitemindex);
  } else
    CHAR_talkToCli(charaindex, -1, "您没有结婚哦~", CHAR_COLORYELLOW);
}

#endif

#ifdef _ITEM_METAMO
void ITEM_ColorMetamo(int charaindex, int toindex, int haveitemindex) {
  if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
    CHAR_talkToCli(charaindex, -1, "骑乘中无法使用该道具~", CHAR_COLORYELLOW);
    return;
  }
  int MetamoList[13][7] = {
      /*{  造型 ,   红  ,   绿  ,   黄  ,   灰  ,   白  ,   黑  },
         //此行为说明行*/
      {100000, 100000, 100005, 100010, 100015, 100700, 100705}, //小矮子
      {100020, 100025, 100030, 100035, 100020, 100710, 100715}, //赛亚人
      {100040, 100055, 100050, 100045, 100040, 100720, 100725}, //辫子男孩
      {100060, 100060, 100065, 100070, 100075, 100730, 100735}, //酷哥
      {100080, 100095, 100085, 100090, 100080, 100740, 100745}, //熊皮男
      {100100, 100100, 100115, 100110, 100105, 100750, 100755}, //大个
      {100120, 100135, 100120, 100125, 100130, 100760, 100765}, //小矮妹
      {100140, 100145, 100140, 100150, 100155, 100770, 100775}, //熊皮妹
      {100160, 100165, 100170, 100160, 100175, 100780, 100785}, //帽子妹
      {100180, 100190, 100195, 100185, 100180, 100790, 100795}, //短发夹妹
      {100200, 100200, 100210, 100210, 100205, 100800, 100805}, //手套女
      {100220, 100230, 100225, 100220, 100235, 100810, 100815}, //辣妹
      {100240, 0, 0, 0, 0, 100820, 0},                          //
  };
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *itemarg = "\0";
  int OldMetamoId, NewMetamoId, i;
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  OldMetamoId = CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER);
  for (i = 0; i < 12; i++)
    if ((OldMetamoId >= MetamoList[i][0] &&
         OldMetamoId < MetamoList[i + 1][0]) ||
        (OldMetamoId >= MetamoList[i][5] && OldMetamoId < MetamoList[i + 1][5]))
      break;
  if (i == 12) {
    CHAR_talkToCli(charaindex, -1, "变色失败，你造型并不是人物造型",
                   CHAR_COLORYELLOW);
    return;
  }
  if (strstr(itemarg, "红")) {
    CHAR_talkToCli(charaindex, -1, "变成可骑红虎的角色！", CHAR_COLORYELLOW);
    NewMetamoId = MetamoList[i][1];
  } else if (strstr(itemarg, "绿")) {
    CHAR_talkToCli(charaindex, -1, "变成可骑绿虎的角色！", CHAR_COLORYELLOW);
    NewMetamoId = MetamoList[i][2];
  } else if (strstr(itemarg, "黄")) {
    CHAR_talkToCli(charaindex, -1, "变成可骑金虎的角色!", CHAR_COLORYELLOW);
    NewMetamoId = MetamoList[i][3];
  } else if (strstr(itemarg, "灰")) {
    CHAR_talkToCli(charaindex, -1, "变成可骑黄虎的角色!", CHAR_COLORYELLOW);
    NewMetamoId = MetamoList[i][4];
  } else if (strstr(itemarg, "白")) {
    CHAR_talkToCli(charaindex, -1, "变成白色!", CHAR_COLORYELLOW);
    NewMetamoId = MetamoList[i][5];
  } else if (strstr(itemarg, "黑")) {
    CHAR_talkToCli(charaindex, -1, "变成黑色!", CHAR_COLORYELLOW);
    NewMetamoId = MetamoList[i][6];
  } else if (strstr(itemarg, "随")) {
    srand(time(0));
    do
      NewMetamoId = MetamoList[i][rand() % 4 + 1];
    while (NewMetamoId == OldMetamoId);
    CHAR_talkToCli(charaindex, -1, "随机变色成功", CHAR_COLORYELLOW);
  } else {
    CHAR_talkToCli(charaindex, -1, "变色失败", CHAR_COLORYELLOW);
    return;
  }
  if (NewMetamoId == 0)
    return;
  CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, NewMetamoId);
  CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, NewMetamoId);

  CHAR_complianceParameter(charaindex);
  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  CHAR_DelItem(charaindex, haveitemindex);
}

void ITEM_CharaMetamo(int charaindex, int toindex, int haveitemindex) {
  if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
    CHAR_talkToCli(charaindex, -1, "骑乘中无法使用该道具~", CHAR_COLORYELLOW);
    return;
  }

  int MetamoList[12][7] = {
      /*{  造型 ,   红  ,   绿  ,   黄  ,   灰  ,   白  ,   黑  },
         //此行为说明行*/
      {100000, 100000, 100005, 100010, 100015, 100700, 100705}, //小矮子
      {100020, 100025, 100030, 100035, 100020, 100710, 100715}, //赛亚人
      {100040, 100055, 100050, 100045, 100040, 100720, 100725}, //辫子男孩
      {100060, 100060, 100065, 100070, 100075, 100730, 100735}, //酷哥
      {100080, 100095, 100085, 100090, 100080, 100740, 100745}, //熊皮男
      {100100, 100100, 100115, 100110, 100105, 100750, 100755}, //大个
      {100120, 100135, 100120, 100125, 100130, 100760, 100765}, //小矮妹
      {100140, 100145, 100140, 100150, 100155, 100770, 100775}, //熊皮妹
      {100160, 100165, 100170, 100160, 100175, 100780, 100785}, //帽子妹
      {100180, 100190, 100195, 100185, 100180, 100790, 100795}, //短发夹妹
      {100200, 100200, 100210, 100215, 100205, 100800, 100805}, //手套女
      {100220, 100230, 100225, 100220, 100235, 100810, 100815}, //辣妹
  };
  int OldMetamoId = 0, NewMetamoId = 0;
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *itemarg = "\0";
  srand(time(0));
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  OldMetamoId = CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER);
  if ((OldMetamoId >= 100000 && OldMetamoId < 100240) ||
      (OldMetamoId >= 100700 && OldMetamoId < 100820)) {
    if (strstr(itemarg, "豆丁囝") && OldMetamoId >= 100000 &&
        OldMetamoId < 100120) {
      CHAR_talkToCli(charaindex, -1, "更改造型为豆囝", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[0][rand() % 4 + 1];
    } else if (strstr(itemarg, "赛亚人") && OldMetamoId >= 100000 &&
               OldMetamoId < 100120) {
      CHAR_talkToCli(charaindex, -1, "更改造型为赛亚人", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[1][rand() % 4 + 1];
    } else if (strstr(itemarg, "辫子男孩") && OldMetamoId >= 100000 &&
               OldMetamoId < 100120) {
      CHAR_talkToCli(charaindex, -1, "更改造型为辫子男孩", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[2][rand() % 4 + 1];
    } else if (strstr(itemarg, "酷哥") && OldMetamoId >= 100000 &&
               OldMetamoId < 100120) {
      CHAR_talkToCli(charaindex, -1, "更改造型为酷哥", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[3][rand() % 4 + 1];
    } else if (strstr(itemarg, "熊皮男") && OldMetamoId >= 100000 &&
               OldMetamoId < 100120) {
      CHAR_talkToCli(charaindex, -1, "更改造型为熊皮男", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[4][rand() % 4 + 1];
    } else if (strstr(itemarg, "大只佬") && OldMetamoId >= 100000 &&
               OldMetamoId < 100120) {
      CHAR_talkToCli(charaindex, -1, "更改造型为大只佬", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[5][rand() % 4 + 1];
    } else if (strstr(itemarg, "豆丁囡") && OldMetamoId >= 100120 &&
               OldMetamoId < 100240) {
      CHAR_talkToCli(charaindex, -1, "更改造型为豆丁囡", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[6][rand() % 4 + 1];
    } else if (strstr(itemarg, "熊皮妹") && OldMetamoId >= 100120 &&
               OldMetamoId < 100240) {
      CHAR_talkToCli(charaindex, -1, "更改造型为熊皮妹", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[7][rand() % 4 + 1];
    } else if (strstr(itemarg, "帽子妹") && OldMetamoId >= 100120 &&
               OldMetamoId < 100240) {
      CHAR_talkToCli(charaindex, -1, "更改造型为帽子妹", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[8][rand() % 4 + 1];
    } else if (strstr(itemarg, "短发夹妹") && OldMetamoId >= 100120 &&
               OldMetamoId < 100240) {
      CHAR_talkToCli(charaindex, -1, "更改造型为短发夹妹", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[9][rand() % 4 + 1];
    } else if (strstr(itemarg, "手套女") && OldMetamoId >= 100120 &&
               OldMetamoId < 100240) {
      CHAR_talkToCli(charaindex, -1, "更改造型为手套女", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[10][rand() % 4 + 1];
    } else if (strstr(itemarg, "辣妹") && OldMetamoId >= 100120 &&
               OldMetamoId < 100240) {
      CHAR_talkToCli(charaindex, -1, "更改造型为辣妹", CHAR_COLORYELLOW);
      NewMetamoId = MetamoList[11][rand() % 4 + 1];
    } else if (strstr(itemarg, "豆丁囝") && OldMetamoId >= 100700 &&
               OldMetamoId < 100760) {
      CHAR_talkToCli(charaindex, -1, "更改造型为豆囝", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[0][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[0][6];
    } else if (strstr(itemarg, "赛亚人") && OldMetamoId >= 100700 &&
               OldMetamoId < 100760) {
      CHAR_talkToCli(charaindex, -1, "更改造型为赛亚人", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[1][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[1][6];
    } else if (strstr(itemarg, "辫子男孩") && OldMetamoId >= 100700 &&
               OldMetamoId < 100760) {
      CHAR_talkToCli(charaindex, -1, "更改造型为辫子男孩", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[2][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[2][6];
    } else if (strstr(itemarg, "酷哥") && OldMetamoId >= 100700 &&
               OldMetamoId < 100760) {
      CHAR_talkToCli(charaindex, -1, "更改造型为酷哥", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[3][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[3][6];
    } else if (strstr(itemarg, "熊皮男") && OldMetamoId >= 100700 &&
               OldMetamoId < 100760) {
      CHAR_talkToCli(charaindex, -1, "更改造型为熊皮男", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[4][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[4][6];
    } else if (strstr(itemarg, "大只佬") && OldMetamoId >= 100700 &&
               OldMetamoId < 100760) {
      CHAR_talkToCli(charaindex, -1, "更改造型为大只佬", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[5][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[5][6];
    } else if (strstr(itemarg, "豆丁囡") && OldMetamoId >= 100760 &&
               OldMetamoId < 100820) {
      CHAR_talkToCli(charaindex, -1, "更改造型为豆丁囡", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[6][6];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[6][6];
    } else if (strstr(itemarg, "熊皮妹") && OldMetamoId >= 100760 &&
               OldMetamoId < 100820) {
      CHAR_talkToCli(charaindex, -1, "更改造型为熊皮妹", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[7][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[7][6];
    } else if (strstr(itemarg, "帽子妹") && OldMetamoId >= 100760 &&
               OldMetamoId < 100820) {
      CHAR_talkToCli(charaindex, -1, "更改造型为帽子妹", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[8][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[8][6];
    } else if (strstr(itemarg, "短发夹妹") && OldMetamoId >= 100760 &&
               OldMetamoId < 100820) {
      CHAR_talkToCli(charaindex, -1, "更改造型为短发夹妹", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[9][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[9][6];
    } else if (strstr(itemarg, "手套女") && OldMetamoId >= 100760 &&
               OldMetamoId < 100820) {
      CHAR_talkToCli(charaindex, -1, "更改造型为手套女", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[10][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[10][6];
    } else if (strstr(itemarg, "辣妹") && OldMetamoId >= 100760 &&
               OldMetamoId < 100820) {
      CHAR_talkToCli(charaindex, -1, "更改造型为辣妹", CHAR_COLORYELLOW);
      if ((OldMetamoId - 100700) % 10 == 0)
        NewMetamoId = MetamoList[11][5];
      else if ((OldMetamoId - 100700) % 10 == 5)
        NewMetamoId = MetamoList[11][6];
    } else if (strstr(itemarg, "随机") && OldMetamoId >= 100120 &&
               OldMetamoId < 100240) {
      do
        if (OldMetamoId >= 100000 && OldMetamoId < 100120) {
          NewMetamoId = MetamoList[rand() % 6][rand() % 4 + 1];
        } else if (OldMetamoId >= 100120 && OldMetamoId < 100240) {
          NewMetamoId = MetamoList[rand() % 6 + 6][rand() % 4 + 1];
        } else if (OldMetamoId >= 100700 && OldMetamoId < 100760) {
          if ((OldMetamoId - 100700) % 10 == 0)
            NewMetamoId = MetamoList[rand() % 6][5];
          else if ((OldMetamoId - 100700) % 10 == 5)
            NewMetamoId = MetamoList[rand() % 6][6];
        } else if (OldMetamoId >= 100760 && OldMetamoId < 100820) {
          if ((OldMetamoId - 100700) % 10 == 0)
            NewMetamoId = MetamoList[rand() % 6 + 6][5];
          else if ((OldMetamoId - 100700) % 10 == 5)
            NewMetamoId = MetamoList[rand() % 6 + 6][6];
        }
      while (NewMetamoId == OldMetamoId);
      CHAR_talkToCli(charaindex, -1, "随机更改造型成功", CHAR_COLORYELLOW);
    }
  } else {
    CHAR_talkToCli(charaindex, -1, "你并不是人物造型!", CHAR_COLORYELLOW);
    return;
  }
  if (NewMetamoId == 0) {
    CHAR_talkToCli(charaindex, -1, "你的性别不同，无没更改此造型!",
                   CHAR_COLORYELLOW);
    return;
  }
  CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, NewMetamoId);
  CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, NewMetamoId);

  CHAR_complianceParameter(charaindex);
  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  CHAR_DelItem(charaindex, haveitemindex);
}

void ITEM_SexMetamo(int charaindex, int toindex, int haveitemindex) {
  if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
    CHAR_talkToCli(charaindex, -1, "骑乘中无法使用该道具~", CHAR_COLORYELLOW);
    return;
  }
  int OldMetamoId, NewMetamoId;
  OldMetamoId = CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER);
  if (OldMetamoId >= 100000 && OldMetamoId < 100020) { //豆丁囝
    NewMetamoId = OldMetamoId + 120;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成豆丁囡!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100020 && OldMetamoId < 100040) { //赛亚人
    NewMetamoId = OldMetamoId + 160;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成短发夹妹!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100040 && OldMetamoId < 100060) { //辫子男孩
    NewMetamoId = OldMetamoId + 120;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成帽子妹!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100060 && OldMetamoId < 100080) { //酷哥
    NewMetamoId = OldMetamoId + 160;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成辣妹!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100080 && OldMetamoId < 100100) { //熊皮男
    NewMetamoId = OldMetamoId + 60;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成熊皮妹!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100100 && OldMetamoId < 100120) { //大只佬
    NewMetamoId = OldMetamoId + 100;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成手套女!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100120 && OldMetamoId < 100140) { //豆丁囡
    NewMetamoId = OldMetamoId - 120;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成豆丁囝!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100140 && OldMetamoId < 100160) { //熊皮妹
    NewMetamoId = OldMetamoId - 60;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成熊皮男!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100160 && OldMetamoId < 100180) { //帽子妹
    NewMetamoId = OldMetamoId - 120;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成辫子男孩!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100180 && OldMetamoId < 100200) { //短发夹妹
    NewMetamoId = OldMetamoId - 160;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成赛亚人!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100200 && OldMetamoId < 100220) { //手套女
    NewMetamoId = OldMetamoId - 100;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成大只佬!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100220 && OldMetamoId < 100240) { //辣妹
    NewMetamoId = OldMetamoId - 160;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成酷哥!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100700 && OldMetamoId < 100710) { //豆丁囝
    NewMetamoId = OldMetamoId + 60;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成豆丁囡!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100710 && OldMetamoId < 100720) { //赛亚人
    NewMetamoId = OldMetamoId + 80;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成短发夹妹!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100720 && OldMetamoId < 100730) { //辫子男孩
    NewMetamoId = OldMetamoId + 60;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成帽子妹!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100730 && OldMetamoId < 100740) { //酷哥
    NewMetamoId = OldMetamoId + 80;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成辣妹!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100740 && OldMetamoId < 100750) { //熊皮男
    NewMetamoId = OldMetamoId + 30;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成熊皮妹!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100750 && OldMetamoId < 100760) { //大只佬
    NewMetamoId = OldMetamoId + 50;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成手套女!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100760 && OldMetamoId < 100770) { //豆丁囡
    NewMetamoId = OldMetamoId - 60;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成豆丁囝!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100770 && OldMetamoId < 100780) { //熊皮妹
    NewMetamoId = OldMetamoId - 30;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成熊皮男!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100780 && OldMetamoId < 100790) { //帽子妹
    NewMetamoId = OldMetamoId - 60;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成辫子男孩!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100790 && OldMetamoId < 100800) { //短发夹妹
    NewMetamoId = OldMetamoId - 80;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成赛亚人!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100810 && OldMetamoId < 100820) { //手套女
    NewMetamoId = OldMetamoId - 50;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成大只佬!", CHAR_COLORYELLOW);
  } else if (OldMetamoId >= 100820 && OldMetamoId < 100830) { //辣妹
    NewMetamoId = OldMetamoId - 80;
    CHAR_talkToCli(charaindex, -1, "恭喜你变性成酷哥!", CHAR_COLORYELLOW);
  } else {
    CHAR_talkToCli(charaindex, -1, "你并不是人物造型，无法变性!",
                   CHAR_COLORYELLOW);
    return;
  }
  if (NewMetamoId == 0)
    return;
  CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, NewMetamoId);
  CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, NewMetamoId);

  CHAR_complianceParameter(charaindex);
  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _POINT_AMPOINT
void ITEM_AddMemberPoint(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *itemarg = "\0";
  char token[64];
  int point = CHAR_getInt(charaindex, CHAR_AMPOINT);
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  CHAR_setInt(charaindex, CHAR_AMPOINT, point + atoi(itemarg));
#ifdef _AMPOINT_LOG
  if (atoi(itemarg) > 0) {
    LogAmPoint(CHAR_getChar(charaindex, CHAR_NAME),
               CHAR_getChar(charaindex, CHAR_CDKEY), atoi(itemarg),
               CHAR_getInt(charaindex, CHAR_AMPOINT), "(积分道具)",
               CHAR_getInt(charaindex, CHAR_FLOOR),
               CHAR_getInt(charaindex, CHAR_X),
               CHAR_getInt(charaindex, CHAR_Y));
  }
#endif

  sprintf(token,
          "款康瘤盔 器牢飘 %d 刘啊 窍看嚼聪促. 泅犁 寸脚狼 款康瘤盔 器牢飘 : "
          "%d 器牢飘",
          atoi(itemarg), point + atoi(itemarg));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _FM_ITEM
void ITEM_AddFame(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *itemarg = "\0";
  char token[64];
  int fame = CHAR_getInt(charaindex, CHAR_FAME);
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  CHAR_setInt(charaindex, CHAR_FAME,
              min(MAX_PERSONALFAME, fame + atoi(itemarg) * 100));
  sprintf(token, "疙己器牢飘 [%d]甫 裙垫窍看嚼聪促. 泅犁 疙己器牢飘 : [%d]",
          atoi(itemarg), fame / 100 + atoi(itemarg));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _LUCK_ITEM
void ITEM_Luck(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  int i;
  char *itemarg = "\0";
  char token[64];
  char luck[][5] = {"凶", "一般", "小吉", "中吉", "大吉"};
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  for (i = 0; i < 5; i++)
    if (strstr(itemarg, luck[i]))
      break;
  if (i == 6)
    i = 0;
  CHAR_setInt(charaindex, CHAR_LUCK, i + 1);
  sprintf(token, "你现在的运气为%s", luck[i]);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _ITEM_METAMO_TIME
void ITEM_MetamoTime(int charaindex, int toindex, int haveitemindex) {
  char *itemarg = "\0", msg[128], buff[32];
  int itemindex, metamoTime = 0, battlemode, metamoNo = 0;

  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  // print(" PetMetamo_toindex:%d ", toindex);

  if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
    CHAR_talkToCli(charaindex, -1, "无法变身，骑乘中不能变身！",
                   CHAR_COLORYELLOW);
    return;
  }
#ifdef _FIX_METAMORIDE
  if (CHAR_CHECKJOINENEMY(charaindex) == TRUE) {
    CHAR_talkToCli(charaindex, -1, "无法变身，搭乘中不能变身！",
                   CHAR_COLORYELLOW);
    return;
  }
#else
  if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 100259) {
    CHAR_talkToCli(charaindex, -1, "无法变身，搭乘中不能变身！",
                   CHAR_COLORYELLOW);
    return;
  }
#endif
#ifdef _PETSKILL_BECOMEPIG
  if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1) { //变成乌力了
    CHAR_talkToCli(charaindex, -1, "无法变身，乌力化中不能变身！",
                   CHAR_COLORYELLOW);
    return;
  }
#endif

#ifdef _FIXBUG_ATTACKBOW
  {
    int armindex = CHAR_getItemIndex(charaindex, CHAR_ARM);
    if (ITEM_CHECKINDEX(armindex) == TRUE) {
      int armtype = BATTLE_GetWepon(charaindex);
      if (armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
          armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG) {
        CHAR_talkToCli(charaindex, -1, "使用远距离武器无法变身！",
                       CHAR_COLORYELLOW);
        return;
      }
    }
  }
#endif

  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  // 爵  钒铵凛反  骰允月
  if (battlemode == BATTLE_CHARMODE_INIT) {
    return;
  }
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  if (getStringFromIndexWithDelim(itemarg, "|", 1, buff, sizeof(buff)))
    metamoNo = atoi(buff);
  if (getStringFromIndexWithDelim(itemarg, "|", 2, buff, sizeof(buff)))
    metamoTime = atoi(buff);
  getStringFromIndexWithDelim(itemarg, "|", 3, buff, sizeof(buff));

  CHAR_setWorkInt(charaindex, CHAR_WORKITEMMETAMO, NowTime.tv_sec + metamoTime);
  if (metamoTime > 60)
    sprintf(msg, "变身成%s，可持续%d小时！", buff, metamoTime / 60);
  else
    sprintf(msg, "变身成%s，可持续%d分钟！", buff, metamoTime);

  CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);

  CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, metamoNo);
  CHAR_complianceParameter(charaindex);
  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _ITEM_GOLD
void ITEM_Gold(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  int MyGold, MaxGold;
  char itemarg[10];
  char token[64];
  MyGold = CHAR_getInt(charaindex, CHAR_GOLD);
  MaxGold = CHAR_getMaxHaveGold(charaindex);
  strcpy(itemarg, ITEM_getChar(itemindex, ITEM_ARGUMENT));
  MyGold += atoi(itemarg);

  if (MyGold > MaxGold) {
    int reGolds = 0;
    int MyGetGold = CHAR_getInt(charaindex, CHAR_GOLD);
    int MyGBGold = CHAR_getInt(charaindex, CHAR_BANKGOLD);
    reGolds = MyGold - MaxGold;
    MyGetGold = MaxGold - MyGetGold;
    MyGold = MaxGold;

    if (MyGetGold > 0) {
      CHAR_setInt(charaindex, CHAR_GOLD, MyGold);
      CHAR_complianceParameter(charaindex);
      CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_GOLD);
      sprintf(token, "%d 胶沛捞 刘啊沁嚼聪促。", MyGetGold);
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    }

    MyGBGold += reGolds;
    MyGBGold =
        (MyGBGold > CHAR_MAXPERSONAGOLD) ? CHAR_MAXPERSONAGOLD : MyGBGold;
    CHAR_setInt(charaindex, CHAR_BANKGOLD, MyGBGold);
    sprintf(token, "胶沛 家瘤茄档 檬苞肺 %d 胶沛阑 篮青俊 涝陛沁嚼聪促。",
            reGolds);
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    sprintf(token, "涝陛 饶 儡咀：%d", CHAR_getInt(charaindex, CHAR_BANKGOLD));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    LogBankStone(CHAR_getChar(charaindex, CHAR_NAME),
                 CHAR_getChar(charaindex, CHAR_CDKEY), charaindex, reGolds,
                 "GB_Bank_save(宝箱)", CHAR_getInt(charaindex, CHAR_FLOOR),
                 CHAR_getInt(charaindex, CHAR_X),
                 CHAR_getInt(charaindex, CHAR_Y),
                 CHAR_getInt(charaindex, CHAR_GOLD),
                 CHAR_getInt(charaindex, CHAR_BANKGOLD));
    CHAR_DelItem(charaindex, haveitemindex);
    return;
  }
  CHAR_setInt(charaindex, CHAR_GOLD, MyGold);
  CHAR_complianceParameter(charaindex);
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_GOLD);
  sprintf(token, "%d 胶沛捞 刘啊沁嚼聪促。 ", atoi(itemarg));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _MYSTERIOUS_GIFT
void ITEM_MysteriousGift(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  char *itemarg = NULL;
  char petnumstr[32];
  int petnum = 0;
  int present[20] = {0};
  int si = 0, ret, i;
  char token[256];

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "") {
    CHAR_talkToCli(charaindex, -1, "这是个神秘礼物!", CHAR_COLORYELLOW);
    return;
  }

  if (getStringFromIndexWithDelim(itemarg, "|", 1, petnumstr,
                                  sizeof(petnumstr)) == FALSE)
    return;
  petnum = atoi(petnumstr);
  if (petnum > 20)
    petnum = 20;
  for (i = 0; i < petnum; i++) {
    if (getStringFromIndexWithDelim(itemarg, "|", 2 + i, petnumstr,
                                    sizeof(petnumstr)))
      present[i] = atoi(petnumstr);
  }

  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
  si = rand() % petnum;

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    if (CHAR_getCharPet(charaindex, i) == -1)
      break;
  }

  if (i == CHAR_MAXPETHAVE) {
    snprintf(token, sizeof(token), "宠物已满！！");
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    return;
  }

  int enemynum = ENEMY_getEnemyNum();
  for (i = 0; i < enemynum; i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == present[si]) {
      break;
    }
  }

  if (i == enemynum)
    return;

  ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
  if (!CHAR_CHECKINDEX(ret))
    return;

  sprintf(token, "裙垫窍促 %s", ENEMY_getChar(i, ENEMY_NAME));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    if (CHAR_getCharPet(charaindex, i) == ret)
      break;
  }
  if (i == CHAR_MAXPETHAVE)
    i = 0;

  snprintf(token, sizeof(token), "K%d", i);
  CHAR_sendStatusString(charaindex, token);
  snprintf(token, sizeof(token), "W%d", i);
  CHAR_sendStatusString(charaindex, token);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _BATTLE_PK
void ITEM_BattlePK(int charaindex, int toindex, int haveitemindex) {
  char token[256];
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    CHAR_talkToCli(charaindex, -1, "只能挑战玩家哦！", CHAR_COLORYELLOW);
    return;
  }
  if (charaindex == toindex) {
    CHAR_talkToCli(charaindex, -1, "难道有自己挑战自己的吗？",
                   CHAR_COLORYELLOW);
    return;
  }
  CHAR_DischargePartyNoMsg(charaindex); //解散团队
  CHAR_DischargePartyNoMsg(toindex);    //解散团队
  BATTLE_CreateVsPlayer(charaindex, toindex);
  CHAR_setWorkInt(charaindex, CHAR_WORK_BATTLEPK, TRUE);
  CHAR_setWorkInt(toindex, CHAR_WORK_BATTLEPK, TRUE);
  sprintf(token, "成功向对方%s挑战!", CHAR_getChar(toindex, CHAR_NAME));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  sprintf(token, "对方%s正式向你挑战!", CHAR_getChar(charaindex, CHAR_NAME));
  CHAR_talkToCli(toindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _SILENTZERO
void ITEM_SetSilentZero(int charaindex, int toindex, int haveitemindex) {
  CHAR_setInt(charaindex, CHAR_SILENT, 0);
  CHAR_setWorkInt(charaindex, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _PET_LEVEL_ITEM
void ITEM_PetLevelItem(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_talkToCli(charaindex, -1, "该物品只能给宠物使用！", CHAR_COLORYELLOW);
    return;
  }
  if (CHAR_getInt(toindex, CHAR_PETID) == 718 ||
      CHAR_getInt(toindex, CHAR_PETID) == 401) {
    CHAR_talkToCli(charaindex, -1, "该物品不能给MM使用！", CHAR_COLORYELLOW);
    return;
  }
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  char *itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  char low[12], hight[12];
  getStringFromIndexWithDelim(itemarg, "|", 1, low, sizeof(low));
  getStringFromIndexWithDelim(itemarg, "|", 2, hight, sizeof(hight));
  if (CHAR_getInt(toindex, CHAR_LV) < atoi(low) &&
      (CHAR_getInt(toindex, CHAR_LIMITLEVEL) > 0 &&
       CHAR_getInt(toindex, CHAR_LIMITLEVEL) < atoi(low))) {
    CHAR_talkToCli(charaindex, -1, "目前你的宠物无法使用该物品！",
                   CHAR_COLORYELLOW);
    return;
  } else if (CHAR_getInt(toindex, CHAR_LV) >= atoi(hight) &&
             CHAR_getInt(toindex, CHAR_LIMITLEVEL) >= atoi(hight)) {
    CHAR_talkToCli(charaindex, -1, "目前你的宠物不必再使用该物品！",
                   CHAR_COLORYELLOW);
    return;
  }
  CHAR_setInt(toindex, CHAR_LIMITLEVEL, atoi(hight));
  if (CHAR_getInt(toindex, CHAR_EXP) < 0) {
    int nextexp = CHAR_GetLevelExp(toindex, CHAR_getInt(toindex, CHAR_LV));
    CHAR_setInt(charaindex, CHAR_EXP, nextexp);
  }

  char token[256];
  sprintf(token, "宠物%s等级已突破%d级！", CHAR_getChar(toindex, CHAR_NAME),
          atoi(hight));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _ITEM_EFMETAMO
void ITEM_efMetamo(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  char *itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  char charmetamo[16];
  char face[16];

  if (itemarg == "") {
    CHAR_talkToCli(charaindex, -1, "酒公老档 老绢唱瘤 臼疽嚼聪促.",
                   CHAR_COLORRED);
    return;
  }
  if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
    CHAR_talkToCli(charaindex, -1, "啪铰吝俊绰 荤侩窍角荐 绝嚼聪促.",
                   CHAR_COLORRED);
    return;
  }

  getStringFromIndexWithDelim(itemarg, "|", 1, charmetamo,
                              sizeof(charmetamo)); // itemarg 捞固瘤内靛 历厘
  getStringFromIndexWithDelim(itemarg, "|", 2, face, sizeof(face));

  CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, atoi(charmetamo));
  CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, atoi(charmetamo));
  CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, atoi(face));

  CHAR_complianceParameter(charaindex);
  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  CHAR_DelItem(charaindex, haveitemindex);
  CHAR_talkToCli(charaindex, -1, "某腐磐 函版捞 肯丰登菌嚼聪促.",
                 CHAR_COLORYELLOW);
}
#endif

#ifdef _PET_BEATITUDE
void PET_BEATITUDE(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_talkToCli(charaindex, -1, "该物品只能给宠物使用！", CHAR_COLORYELLOW);
    return;
  }
  if (CHAR_getInt(toindex, CHAR_PETID) == 718 ||
      CHAR_getInt(toindex, CHAR_PETID) == 401) {
    CHAR_talkToCli(charaindex, -1, "该物品不能给MM使用！", CHAR_COLORYELLOW);
    return;
  }
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  char *itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  char beatitude[12], mun[12];
  getStringFromIndexWithDelim(itemarg, "|", 1, beatitude, sizeof(beatitude));
  getStringFromIndexWithDelim(itemarg, "|", 2, mun, sizeof(mun));
  char token[256];
  int beat = CHAR_getInt(toindex, CHAR_BEATITUDE);
  int nbx1a = ((beat >> 24) & 0xff); //取出血祝福数据
  int nbx2a = ((beat >> 16) & 0xff); //取出攻祝福数据
  int nbx3a = ((beat >> 8) & 0xff);  //取出防祝福数据
  int nbx4a = ((beat >> 0) & 0xff);  //取出敏祝福数据

  int beattype = CHAR_getInt(toindex, CHAR_BEATITUDETYPE); //取出品质值
  int nbx1b = ((beattype >> 24) & 0xff);
  int nbx2b = ((beattype >> 16) & 0xff);
  int nbx3b = ((beattype >> 8) & 0xff);
  int nbx4b = ((beattype >> 0) & 0xff);

  if (strcmp(beatitude, "体") == 0) {
    if (nbx1b > 0) {
      CHAR_talkToCli(charaindex, -1, "您的宠物该项属性已提升过了！",
                     CHAR_COLORYELLOW);
      return;
    } else {
      sprintf(token, "%s体力上升%d", CHAR_getChar(toindex, CHAR_NAME),
              atoi(mun));
      CHAR_setInt(toindex, CHAR_VITAL,
                  (CHAR_getInt(toindex, CHAR_VITAL) + atoi(mun)));
      nbx1b = 1;
      nbx1a = atoi(mun);
    }
  } else if (strcmp(beatitude, "攻") == 0) {
    if (nbx2b > 0) {
      CHAR_talkToCli(charaindex, -1, "您的宠物该项属性已提升过了！",
                     CHAR_COLORYELLOW);
      return;
    } else {
      sprintf(token, "%s攻击力上升%d", CHAR_getChar(toindex, CHAR_NAME),
              atoi(mun));
      CHAR_setInt(toindex, CHAR_STR,
                  (CHAR_getInt(toindex, CHAR_STR) + atoi(mun)));
      nbx2b = 1;
      nbx2a = atoi(mun);
    }
  } else if (strcmp(beatitude, "防") == 0) {
    if (nbx3b > 0) {
      CHAR_talkToCli(charaindex, -1, "您的宠物该项属性已提升过了！",
                     CHAR_COLORYELLOW);
      return;
    } else {
      sprintf(token, "%s防御力上升%d", CHAR_getChar(toindex, CHAR_NAME),
              atoi(mun));
      CHAR_setInt(toindex, CHAR_TOUGH,
                  (CHAR_getInt(toindex, CHAR_TOUGH) + atoi(mun)));
      nbx3b = 1;
      nbx3a = atoi(mun);
    }
  } else if (strcmp(beatitude, "敏") == 0) {
    if (nbx4b > 0) {
      CHAR_talkToCli(charaindex, -1, "您的宠物该项属性已提升过了！",
                     CHAR_COLORYELLOW);
      return;
    } else {
      sprintf(token, "%s敏捷力上升%d", CHAR_getChar(toindex, CHAR_NAME),
              atoi(mun));
      CHAR_setInt(toindex, CHAR_DEX,
                  (CHAR_getInt(toindex, CHAR_DEX) + atoi(mun)));
      nbx4b = 1;
      nbx4a = atoi(mun);
    }
  } else {
    sprintf(token, "%s当前属性为体:%d,攻:%d,防:%d,敏:%d",
            CHAR_getChar(toindex, CHAR_NAME), CHAR_getInt(toindex, CHAR_VITAL),
            CHAR_getInt(toindex, CHAR_STR), CHAR_getInt(toindex, CHAR_TOUGH),
            CHAR_getInt(toindex, CHAR_DEX));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    return;
  }
  CHAR_talkToCli(charaindex, -1, "主人，这东西太棒了，我已充满力量了！",
                 CHAR_COLORYELLOW);
  beat = (nbx1a << 24) + (nbx2a << 16) + (nbx3a << 8) + (nbx4a << 0);
  beattype = (nbx1b << 24) + (nbx2b << 16) + (nbx3b << 8) + (nbx4b << 0);
  CHAR_setInt(toindex, CHAR_BEATITUDE, beat);         //设定祝福值
  CHAR_setInt(toindex, CHAR_BEATITUDETYPE, beattype); //设定品质值
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  int i;
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    if (CHAR_getCharPet(charaindex, i) == toindex)
      break;
  }
  if (i == CHAR_MAXPETHAVE)
    return;
  CHAR_complianceParameter(toindex);
  snprintf(token, sizeof(token), "K%d", i);
  CHAR_sendStatusString(charaindex, token);
  snprintf(token, sizeof(token), "W%d", i);
  CHAR_sendStatusString(charaindex, token);

  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _GET_MULTI_ITEM
void ITEM_GetMultiItem(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  char *itemarg = NULL;
  char buf[32];
  int itemnum = 0;
  int itemid = {-1};
  char itemname[512] = "";
  int i;
  char token[256];

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "") {
    CHAR_talkToCli(charaindex, -1, "这是个空盒子!", CHAR_COLORYELLOW);
    return;
  }

  if (getStringFromIndexWithDelim(itemarg, "|", 1, buf, sizeof(buf)) == FALSE) {
    CHAR_talkToCli(charaindex, -1, "不明东西!", CHAR_COLORYELLOW);
    return;
  }
  itemnum = atoi(buf);
  if (itemnum > 10) {
    itemnum = 10;
  } else if (itemnum < 0) {
    CHAR_talkToCli(charaindex, -1, "这是个空盒子!", CHAR_COLORYELLOW);
    return;
  }
  if ((itemnum - 1) <= CHAR_findSurplusItemBox(charaindex)) {
    CHAR_DelItem(charaindex, haveitemindex);
  } else {
    sprintf(token, "盒子里有%d件物品，你的物品栏位不足。", itemnum);
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
    return;
  }

  for (i = 0; i < itemnum; i++) {
    if (getStringFromIndexWithDelim(itemarg, "|", 2 + i, buf, sizeof(buf)) ==
        FALSE) {
      CHAR_talkToCli(charaindex, -1, "不明东西!", CHAR_COLORYELLOW);
      return;
    }
    itemid = atoi(buf);
    itemindex = ITEM_makeItemAndRegist(itemid);
    if (itemindex != -1) {
      int emptyteimbox = CHAR_findEmptyItemBox(charaindex);
      CHAR_setItemIndex(charaindex, emptyteimbox, itemindex);
      ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
      ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, charaindex);
      CHAR_sendItemDataOne(charaindex, emptyteimbox);
      strcat(itemname, ITEM_getChar(itemindex, ITEM_NAME));
      strcat(itemname, "|");
    }
  }
  sprintf(token, "裙垫茄 酒捞袍:|%s", itemname);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
}
#endif

#ifdef _SUPER_FLOOR_MIC
void ITEM_useSuperMic(int charaindex, int toindex, int haveitemindex) {
  int battlemode;
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return;
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else if (battlemode) {
  } else {
    if (CHAR_getWorkInt(charaindex, CHAR_WORKFLG) & WORKFLG_SUPERMICMODE) {
      CHAR_setWorkInt(charaindex, CHAR_WORKFLG,
                      CHAR_getWorkInt(charaindex, CHAR_WORKFLG) &
                          ~WORKFLG_SUPERMICMODE);
      CHAR_talkToCli(charaindex, -1, "将超级麦克风设定为OFF。",
                     CHAR_COLORWHITE);
    } else {
      CHAR_setWorkInt(charaindex, CHAR_WORKFLG,
                      CHAR_getWorkInt(charaindex, CHAR_WORKFLG) |
                          WORKFLG_SUPERMICMODE);
      CHAR_talkToCli(charaindex, -1, "将超级麦克风设定为ON。", CHAR_COLORWHITE);
    }
  }
}
#endif

#ifdef _ITEM_PET_LOCKED
void ITEM_ItemPetLocked(int charaindex, int toindex, int haveitemindex) {
  if (CHAR_getInt(charaindex, CHAR_LOCKED) == 0) {
    CHAR_setInt(charaindex, CHAR_LOCKED, 2);
    CHAR_talkToCli(charaindex, -1, "安全锁已永久解除！", CHAR_COLORYELLOW);
  } else if (CHAR_getInt(charaindex, CHAR_LOCKED) == 1) {
    char buf[256];
    int fd = getfdFromCharaIndex(charaindex);
    if (fd == -1)
      return;

    lssproto_WN_send(
        fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, WINDOW_BUTTONTYPE_OKCANCEL,
        CHAR_WINDOWTYPE_ITEM_PET_LOCKED, -1,
        makeEscapeString("您正在进行 「永久解除安全锁」 "
                         "操作。\n请输入您的安全密码进行确认。\n解锁后彻底打开"
                         "安全锁，无需输入安全码！\n\n此为高危操作，敬请慎重！",
                         buf, sizeof(buf)));
  } else {
    CHAR_setInt(charaindex, CHAR_LOCKED, 1);
    CHAR_talkToCli(charaindex, -1, "安全锁已经锁定！", CHAR_COLORYELLOW);
  }
}
#endif

#ifdef _ONLINE_COST
void ITEM_OnlineCost(int charaindex, int toindex, int haveitemindex) {
  char buf[256];
  int fd = getfdFromCharaIndex(charaindex);
  if (fd == -1)
    return;

  lssproto_WN_send(
      fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, WINDOW_BUTTONTYPE_OKCANCEL,
      CHAR_WINDOWTYPE_ONLINE_COST, -1,
      makeEscapeString("请输入你的充值卡密码串！\n", buf, sizeof(buf)));

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _VIPPOINT_OLD_TO_NEW
void ITEM_OldToNew(int charaindex, int toindex, int haveitemindex) {
  if (CHAR_getInt(charaindex, CHAR_AMPOINT) > 0) {
    saacproto_OldToNew_send(acfd, getfdFromCharaIndex(charaindex),
                            CHAR_getChar(charaindex, CHAR_CDKEY),
                            CHAR_getInt(charaindex, CHAR_AMPOINT));
    CHAR_talkToCli(charaindex, -1, "正在转换，请稍后", CHAR_COLORYELLOW);
#ifdef _SQL_VIPPOINT_LOG
    LogSqlVipPoint(CHAR_getChar(charaindex, CHAR_NAME),
                   CHAR_getChar(charaindex, CHAR_CDKEY), "(积分转换)",
                   CHAR_getInt(charaindex, CHAR_AMPOINT),
                   CHAR_getInt(charaindex, CHAR_FLOOR),
                   CHAR_getInt(charaindex, CHAR_X),
                   CHAR_getInt(charaindex, CHAR_Y));
#endif
    CHAR_setInt(charaindex, CHAR_AMPOINT, 0);

  } else {
    CHAR_talkToCli(charaindex, -1, "你身上都没积分点，无需转换",
                   CHAR_COLORYELLOW);
  }

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _TALK_SIZE_ITEM
void ITEM_TalkSize(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  char *itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  char token[256];

  CHAR_setWorkInt(charaindex, CHAR_WORKFONTSIZE, atoi(itemarg));
  sprintf(token, "您的字号设为%d！", atoi(itemarg));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _FORMULATE_AUTO_PK
void ITEM_PointToSQLPkPoint(int charaindex, int toindex, int haveitemindex) {
  if (CHAR_getInt(charaindex, CHAR_AMPOINT) > 0) {
    CHAR_talkToCli(charaindex, -1, "正在转换，请稍后", CHAR_COLORYELLOW);
    saacproto_FormulateAutoPk_send(getfdFromCharaIndex(charaindex),
                                   CHAR_getChar(charaindex, CHAR_CDKEY),
                                   CHAR_getInt(charaindex, CHAR_AMPOINT));
#ifdef _SQL_VIPPOINT_LOG
    LogSqlVipPoint(CHAR_getChar(charaindex, CHAR_NAME),
                   CHAR_getChar(charaindex, CHAR_CDKEY), "(积分转换)",
                   CHAR_getInt(charaindex, CHAR_AMPOINT),
                   CHAR_getInt(charaindex, CHAR_FLOOR),
                   CHAR_getInt(charaindex, CHAR_X),
                   CHAR_getInt(charaindex, CHAR_Y));
#endif

    CHAR_setInt(charaindex, CHAR_AMPOINT, 0);
  } else {
    CHAR_talkToCli(charaindex, -1, "你身上都没积分点，无需转换",
                   CHAR_COLORYELLOW);
  }
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _SUPER_MAN_ITEM
void ITEM_SuperManItem(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  char *itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  char token[256];
  float table[] = {437, 490, 521, 550, 578, 620, 700}; //各转最高点数(减10)
  int trans, lv, point;
  int enemyid = -1;
  int fd = getfdFromCharaIndex(charaindex);

  getStringFromIndexWithDelim(itemarg, "|", 1, token, sizeof(token));
  trans = atoi(token);
  getStringFromIndexWithDelim(itemarg, "|", 2, token, sizeof(token));
  lv = atoi(token);
  getStringFromIndexWithDelim(itemarg, "|", 3, token, sizeof(token));
  point = atoi(token);
  if (getStringFromIndexWithDelim(itemarg, "|", 4, token, sizeof(token)) ==
      FALSE) {
    enemyid = -1;
  } else {
    enemyid = atoi(token);
  }
  int totalPoint = CHAR_getInt(charaindex, CHAR_VITAL) / 100 +
                   CHAR_getInt(charaindex, CHAR_STR) / 100 +
                   CHAR_getInt(charaindex, CHAR_TOUGH) / 100 +
                   CHAR_getInt(charaindex, CHAR_DEX) / 100 +
                   CHAR_getInt(charaindex, CHAR_SKILLUPPOINT);
  if (totalPoint > point) {
    CHAR_talkToCli(charaindex, -1, "无法使用，身体能力已经够强了。",
                   CHAR_COLORYELLOW);
    return;
  }
  if (enemyid > -1) {
    if (CHAR_getCharPetElement(charaindex) < 0) {
      CHAR_talkToCli(charaindex, -1, "身上宠物栏位已满！", CHAR_COLORYELLOW);
      return;
    }
    int i, j, enemynum, ret;
    enemynum = ENEMY_getEnemyNum();
    for (i = 0; i < enemynum; i++) {
      if (ENEMY_getInt(i, ENEMY_ID) == enemyid) {
        break;
      }
    }
    if (i == enemynum)
      return;

    ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
    if (!CHAR_CHECKINDEX(ret))
      return;
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      if (CHAR_getCharPet(charaindex, i) == ret)
        break;
    }
    if (i == CHAR_MAXPETHAVE)
      return;

    if (CHAR_CHECKINDEX(ret) == TRUE) {
      CHAR_setMaxExpFromLevel(ret, CHAR_getInt(ret, CHAR_LV));
      CHAR_setInt(ret, CHAR_VARIABLEAI, 10000);
    }
    CHAR_setInt(ret, CHAR_HP, CHAR_getWorkInt(ret, CHAR_WORKMAXHP));
    CHAR_complianceParameter(ret);
    snprintf(token, sizeof(token), "K%d", i);
    CHAR_sendStatusString(charaindex, token);
    snprintf(token, sizeof(token), "W%d", i);
    CHAR_sendStatusString(charaindex, token);
  }

  int quest = (0xFFFF0000 >> 16) & 0xFF;

  float jxds = point - (getTransPoint(trans) + (lv - 1) * getSkup() +
                        table[trans - 1] / 12.0 + quest / 4.0 +
                        getTransPoint(trans) + trans * 10);

  int teq = jxds * 4.0 + trans * 85;

  CHAR_setInt(charaindex, CHAR_TRANSEQUATION, teq | 0xFFFF0000);

  CHAR_setInt(charaindex, CHAR_TRANSMIGRATION, trans);
  CHAR_setInt(charaindex, CHAR_LV, lv);
  CHAR_setInt(charaindex, CHAR_VITAL, 1000);
  CHAR_setInt(charaindex, CHAR_STR, 0);
  CHAR_setInt(charaindex, CHAR_TOUGH, 0);
  CHAR_setInt(charaindex, CHAR_DEX, 0);
  CHAR_setInt(charaindex, CHAR_SKILLUPPOINT, point - 10);
#ifdef _SUPER
  CHAR_setInt(charaindex, CHAR_SUPER, 1);
#endif
  NPC_EventSetFlg(charaindex, 4);
  NPC_EventSetFlg(charaindex, 39);
  NPC_EventSetFlg(charaindex, 40);
  NPC_EventSetFlg(charaindex, 41);
  NPC_EventSetFlg(charaindex, 42);
  NPC_EventSetFlg(charaindex, 46);
  // CHAR_Skillupsend( charaindex);
  lssproto_SKUP_send(fd, point - 10);
  CHAR_complianceParameter(charaindex);
  CHAR_sendStatusString(charaindex, "P");
  CHAR_send_P_StatusString(charaindex,
                           CHAR_P_STRING_DUELPOINT | CHAR_P_STRING_VITAL |
                               CHAR_P_STRING_STR | CHAR_P_STRING_TOUGH |
                               CHAR_P_STRING_DEX);

  char token2[256];
  sprintf(token2, "您已经成为%d转%d级%d属性点的完美人了！", trans, lv, point);
  CHAR_talkToCli(charaindex, -1, token2, CHAR_COLORYELLOW);
  CHAR_talkToCli(charaindex, -1, "您已完成【成人任务】和【四大洞穴任务】！",
                 CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _COST_ITEM
void ITEM_CostItem(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *itemarg = "\0";
  char token[64];
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
#ifdef _SQL_VIPPOINT_LOG
  LogSqlVipPoint(
      CHAR_getChar(charaindex, CHAR_NAME), CHAR_getChar(charaindex, CHAR_CDKEY),
      "(换兑道具)", atoi(itemarg), CHAR_getInt(charaindex, CHAR_FLOOR),
      CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y));
#endif
  saacproto_CostItem_send(acfd, getfdFromCharaIndex(charaindex),
                          CHAR_getChar(charaindex, CHAR_CDKEY), atoi(itemarg));
  sprintf(token, "获得金币数%d，正读取您当前金币数据...", atoi(itemarg));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _ITEM_EXP
void ITEM_EXP(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  char token[256];

  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  char *exp = ITEM_getChar(itemindex, ITEM_ARGUMENT);

  CHAR_setMaxExp(toindex, CHAR_getInt(toindex, CHAR_EXP) + atoi(exp));

  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_sendStatusString(toindex, "P");
  } else {
    int i;
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      CHAR_send_K_StatusString(charaindex, i, CHAR_K_STRING_EXP);
    }
  }
  sprintf(token, "%s 增加 %d 经验", CHAR_getChar(toindex, CHAR_NAME),
          atoi(exp));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _MEMORY_ITEM
void ITEM_WarpItem(int charaindex, int toindex, int haveitemindex) {
  char *arg;
  int flg = 0, ff = 0, fx = 0, fy = 0, itemindex = 0, usenum = 0;
  int Mf, Mx, My;
  char buf[256];

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (arg == "\0")
    return;

  if (sscanf(arg, "%d %d %d %d", &flg, &ff, &fx, &fy) != 4) {
    Mf = CHAR_getInt(charaindex, CHAR_FLOOR);
    Mx = CHAR_getInt(charaindex, CHAR_X);
    My = CHAR_getInt(charaindex, CHAR_Y);

    // WON ADD
    if (checkUnlawWarpFloor(Mf)) {
      CHAR_talkToCli(charaindex, -1, "此处无法使用。", CHAR_COLORYELLOW);
      return;
    }
    char escapeshowstring[64];
    char *showstr = MAP_getfloorShowstring(CHAR_getInt(charaindex, CHAR_FLOOR));

    getStringFromIndexWithDelim(showstr, "|", 1, escapeshowstring,
                                sizeof(escapeshowstring));

    if (sscanf(arg, "%d %d", &usenum, &flg) != 2) {
      ITEM_setInt(itemindex, ITEM_DAMAGEBREAK, usenum);
    } else {
      ITEM_setInt(itemindex, ITEM_DAMAGEBREAK, atoi(arg));
    }
    sprintf(buf, "%d %d %d %d", flg, Mf, Mx, My);
    ITEM_setChar(itemindex, ITEM_ARGUMENT, buf);

    sprintf(buf, "坐标记录点位于(%s,%d,%d)", escapeshowstring, Mx, My);
    ITEM_setChar(itemindex, ITEM_EFFECTSTRING, buf);
    CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
    CHAR_sendItemDataOne(charaindex, haveitemindex);
  } else {
    usenum = ITEM_getInt(itemindex, ITEM_DAMAGEBREAK);

#ifdef _ITEM_CHECKWARES
    if (CHAR_CheckInItemForWares(charaindex, 0) == FALSE) {
      CHAR_talkToCli(charaindex, -1, "携带货物无法使用。", CHAR_COLORYELLOW);
      return;
    }
#endif

    if (--usenum <= 0) {
      CHAR_talkToCli(charaindex, -1, "该道具已经没有使用次数了，永久消失！",
                     CHAR_COLORYELLOW);
      CHAR_DelItem(charaindex, haveitemindex);
    } else {
      sprintf(buf, "该道具使用次数剩余%d次", usenum);
      CHAR_talkToCli(charaindex, -1, buf, CHAR_COLORYELLOW);
    }
    ITEM_setInt(itemindex, ITEM_DAMAGEBREAK, usenum);
    if (ITEM_WarpForAny(charaindex, ff, fx, fy, flg) == FALSE)
      return;
  }
}
#endif

#ifdef _MANOR_EQUIP
void ITEM_CheckManorEquip(int charaindex, int itemindex) {
  int floor = CHAR_getWorkInt(charaindex, CHAR_WORKFMFLOOR);

  if (floor == ITEM_getInt(itemindex, ITEM_SUITCODE)) {
    char *arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
    ITEM_setInt(itemindex, ITEM_MAGICID, atoi(arg));
  } else {
    ITEM_setInt(itemindex, ITEM_MAGICID, -1);
  }
  int i;
  for (i = 0; i < CHAR_EQUIPPLACENUM; i++) {
    if (CHAR_getItemIndex(charaindex, i) == itemindex) {
      char buf[64];
      snprintf(buf, sizeof(buf), "J%d", i);
      CHAR_sendStatusString(charaindex, buf);
      break;
    }
  }
}

void ITEM_ManorEquip(int charaindex, int itemindex) {
  ITEM_CheckManorEquip(charaindex, itemindex);
}

void ITEM_ReManorEquip(int charaindex, int itemindex) {
  ITEM_CheckManorEquip(charaindex, itemindex);
}
#endif

#ifdef _ITEM_POOLITEM
extern int poolitemhanlde;
void ITEM_PoolItem(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (CHAR_getInt(charaindex, CHAR_AMPOINT) < atoi(arg)) {
    CHAR_talkToCli(charaindex, -1, "很抱歉，由于你身上不足金币使用该物品！",
                   CHAR_COLORRED);
    return;
  }
  NPC_PoolItemShop_printWindow_Start(poolitemhanlde, charaindex);
  CHAR_setInt(charaindex, CHAR_AMPOINT,
              CHAR_getInt(charaindex, CHAR_AMPOINT) - atoi(arg));
#ifdef _AMPOINT_LOG
  if (atoi(arg) > 0) {
    LogAmPoint(CHAR_getChar(charaindex, CHAR_NAME),
               CHAR_getChar(charaindex, CHAR_CDKEY), -atoi(arg),
               CHAR_getInt(charaindex, CHAR_AMPOINT), "(道具仓库)",
               CHAR_getInt(charaindex, CHAR_FLOOR),
               CHAR_getInt(charaindex, CHAR_X),
               CHAR_getInt(charaindex, CHAR_Y));
  }
#endif
}
#endif

#ifdef _ITEM_POOLPET
extern int petshophanlde;
void ITEM_PoolPet(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (CHAR_getInt(charaindex, CHAR_AMPOINT) < atoi(arg)) {
    CHAR_talkToCli(charaindex, -1, "很抱歉，由于你身上不足金币使用该物品！",
                   CHAR_COLORRED);
    return;
  }
  NPC_PetShop_selectWindow(petshophanlde, charaindex, 0, -1);
  CHAR_setInt(charaindex, CHAR_AMPOINT,
              CHAR_getInt(charaindex, CHAR_AMPOINT) - atoi(arg));
#ifdef _AMPOINT_LOG
  if (atoi(arg) > 0) {
    LogAmPoint(CHAR_getChar(charaindex, CHAR_NAME),
               CHAR_getChar(charaindex, CHAR_CDKEY), -atoi(arg),
               CHAR_getInt(charaindex, CHAR_AMPOINT), "(宠物仓库)",
               CHAR_getInt(charaindex, CHAR_FLOOR),
               CHAR_getInt(charaindex, CHAR_X),
               CHAR_getInt(charaindex, CHAR_Y));
  }
#endif
}
#endif

#ifdef _SHOW_PET_ABL
void ITEM_ShowPetAbl(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
    CHAR_talkToCli(charaindex, -1, "请选择你的宠物吧!", CHAR_COLORRED);
    return;
  }
  int work[4];
  int LevelUpPoint;
  char token[128];
  char buf[][32] = {"腕力成长率", "耐久力成长率", "速度成长率", "体力成长率"};

  LevelUpPoint = CHAR_getInt(toindex, CHAR_ALLOCPOINT);
  work[3] = ((LevelUpPoint >> 24) & 0xFF);
  work[0] = ((LevelUpPoint >> 16) & 0xFF);
  work[1] = ((LevelUpPoint >> 8) & 0xFF);
  work[2] = ((LevelUpPoint >> 0) & 0xFF);

  sprintf(token, "%s 各项成长率如下:", CHAR_getChar(toindex, CHAR_NAME));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
  sprintf(token, "%s %d", buf[0], work[0]);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  sprintf(token, "%s %d", buf[1], work[1]);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  sprintf(token, "%s %d", buf[2], work[2]);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  sprintf(token, "%s %d", buf[3], work[3]);
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _NEWEVENT_ITEM
void ITEM_NeweventItem(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;

  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  char *arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  char token[128];
  int i = 0;
  while (getStringFromIndexWithDelim(arg, "|", i, token, sizeof(token))) {
    NPC_NowEndEventSetFlgCls(charaindex, atoi(token));
    NPC_EventSetFlg(charaindex, atoi(token));
    i++;
    if (i > 30)
      break;
  }
  sprintf(token, "%s(阑)甫 荤侩窍看绢夸。", ITEM_getChar(itemindex, ITEM_NAME));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
  CHAR_DelItem(charaindex, haveitemindex);
}
void ITEM_transpointeventItem(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;

  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  char token[128];

  NPC_EventSetFlg(charaindex, 1);
  NPC_EventSetFlg(charaindex, 2);
  NPC_EventSetFlg(charaindex, 3);
  NPC_EventSetFlg(charaindex, 4);
  NPC_EventSetFlg(charaindex, 5);
  NPC_EventSetFlg(charaindex, 7);
  NPC_EventSetFlg(charaindex, 8);
  NPC_EventSetFlg(charaindex, 9);
  NPC_EventSetFlg(charaindex, 10);
  NPC_EventSetFlg(charaindex, 11);
  NPC_EventSetFlg(charaindex, 12);
  NPC_EventSetFlg(charaindex, 15);
  NPC_EventSetFlg(charaindex, 16);
  NPC_EventSetFlg(charaindex, 17);
  NPC_EventSetFlg(charaindex, 18);
  NPC_EventSetFlg(charaindex, 19);
  NPC_EventSetFlg(charaindex, 20);
  NPC_EventSetFlg(charaindex, 25);
  NPC_EventSetFlg(charaindex, 26);
  NPC_EventSetFlg(charaindex, 27);
  NPC_EventSetFlg(charaindex, 28);
  NPC_EventSetFlg(charaindex, 29);
  NPC_EventSetFlg(charaindex, 30);
  NPC_EventSetFlg(charaindex, 31);
  NPC_EventSetFlg(charaindex, 32);
  NPC_EventSetFlg(charaindex, 33);
  NPC_EventSetFlg(charaindex, 34);
  NPC_EventSetFlg(charaindex, 35);
  NPC_EventSetFlg(charaindex, 36);
  NPC_EventSetFlg(charaindex, 37);
  NPC_EventSetFlg(charaindex, 38);
  NPC_EventSetFlg(charaindex, 44);
  NPC_EventSetFlg(charaindex, 45);
  NPC_EventSetFlg(charaindex, 47);
  sprintf(token, "%s(阑)甫 荤侩窍看绢夸。", ITEM_getChar(itemindex, ITEM_NAME));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _NEW_PET_BEATITUDE
void ITEM_NewPetBeatitude(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
    CHAR_talkToCli(charaindex, -1, "请选择你的宠物吧!", CHAR_COLORRED);
    return;
  }
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  char *arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  char token[128];
  int abi[4];
  int flg;
  int i;
  int work[4];
  int LevelUpPoint;
  char buf[][32] = {"腕力成长率", "耐久力成长率", "速度成长率", "体力成长率"};
  if (getStringFromIndexWithDelim(arg, "|", 1, token, sizeof(token)) == TRUE) {
    if (CHAR_getInt(toindex, CHAR_PETID) != atoi(token)) {
      CHAR_talkToCli(charaindex, -1, "您选择的宠物不对!", CHAR_COLORRED);
      return;
    }
  } else {
    return;
  }

  if (getStringFromIndexWithDelim(arg, "|", 2, token, sizeof(token)) == TRUE) {
    flg = atoi(token);
    if (NPC_EventCheckFlg(toindex, flg) == TRUE) {
      CHAR_talkToCli(charaindex, -1, "该宠物已经无法再次使用该物品了!",
                     CHAR_COLORRED);
      return;
    }
  } else {
    return;
  }

  if (getStringFromIndexWithDelim(arg, "|", 3, token, sizeof(token)) == TRUE) {
    char tmp[16];
    if (getStringFromIndexWithDelim(token, ":", 1, tmp, sizeof(tmp)) == TRUE) {
      abi[3] = atoi(tmp);
    } else {
      abi[3] = 0;
    }
    if (getStringFromIndexWithDelim(token, ":", 2, tmp, sizeof(tmp)) == TRUE) {
      abi[0] = atoi(tmp);
    } else {
      abi[0] = 0;
    }
    if (getStringFromIndexWithDelim(token, ":", 3, tmp, sizeof(tmp)) == TRUE) {
      abi[1] = atoi(tmp);
    } else {
      abi[1] = 0;
    }
    if (getStringFromIndexWithDelim(token, ":", 4, tmp, sizeof(tmp)) == TRUE) {
      abi[2] = atoi(tmp);
    } else {
      abi[2] = 0;
    }
  } else {
    return;
  }

  LevelUpPoint = CHAR_getInt(toindex, CHAR_ALLOCPOINT);
  work[3] = ((LevelUpPoint >> 24) & 0xFF);
  work[0] = ((LevelUpPoint >> 16) & 0xFF);
  work[1] = ((LevelUpPoint >> 8) & 0xFF);
  work[2] = ((LevelUpPoint >> 0) & 0xFF);

  sprintf(token, "%s 各项成长率变化如下:", CHAR_getChar(toindex, CHAR_NAME));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
  for (i = 0; i < 4; i++) {
    work[i] += abi[i];
    if (abi[i] > 0) {
      sprintf(token, "%s 上升 %d", buf[i], abi[i]);
    } else if (abi[i] < 0) {
      sprintf(token, "%s 下降 %d", buf[i], abi[i]);
    } else {
      sprintf(token, "%s 未发生变化", buf[i]);
    }
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  }

  LevelUpPoint =
      (work[3] << 24) + (work[0] << 16) + (work[1] << 8) + (work[2] << 0);
  CHAR_setInt(toindex, CHAR_ALLOCPOINT, LevelUpPoint);

  NPC_EventSetFlg(toindex, flg);

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

#ifdef _OLYMPIC_TORCH
void ITEM_OlympicTorck(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char token[128];
  int i;
  int findindex;
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (strlen(ITEM_getChar(itemindex, ITEM_FORUSERNAME)) == 0 ||
      strlen(ITEM_getChar(itemindex, ITEM_FORUSERCDKEY)) == 0) {
    for (i = 0; i < 3; i++) {
      findindex = rand() % CHAR_getPlayerMaxNum();
      if (CHAR_CHECKINDEX(findindex) == TRUE) {
        if (findindex != charaindex) {
          ITEM_setChar(itemindex, ITEM_FORUSERNAME,
                       CHAR_getChar(findindex, CHAR_NAME));
          ITEM_setChar(itemindex, ITEM_FORUSERCDKEY,
                       CHAR_getChar(findindex, CHAR_CDKEY));

          sprintf(token,
                  "寻找到合适的接棒人 %s,当前所在地图号:%d 坐标X:%d 坐标Y:%d",
                  CHAR_getChar(findindex, CHAR_NAME),
                  CHAR_getInt(findindex, CHAR_FLOOR),
                  CHAR_getInt(findindex, CHAR_X),
                  CHAR_getInt(findindex, CHAR_Y));
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
          return;
        }
      }
    }
    CHAR_talkToCli(charaindex, -1, "很抱歉,找不到合适的接棒人!",
                   CHAR_COLORYELLOW);
  } else {
    int playernum = CHAR_getPlayerMaxNum();
    for (findindex = 0; findindex < playernum; findindex++) {
      if (CHAR_CHECKINDEX(findindex) == TRUE) {
        if (strcmp(CHAR_getChar(findindex, CHAR_CDKEY),
                   ITEM_getChar(itemindex, ITEM_FORUSERCDKEY)) == 0) {
          sprintf(token, "接棒人 %s,现位于地图号:%d 坐标X:%d 坐标Y:%d",
                  CHAR_getChar(findindex, CHAR_NAME),
                  CHAR_getInt(findindex, CHAR_FLOOR),
                  CHAR_getInt(findindex, CHAR_X),
                  CHAR_getInt(findindex, CHAR_Y));
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
          break;
        }
      }
    }
  }
}
#endif

#ifdef _SHOW_ITEM
void ITEM_ShowItem(int charaindex, int toindex, int haveitemindex) {

  int x, y;
  OBJECT object;
  int fd;

  fd = getfdFromCharaIndex(charaindex);
  if (fd == -1) {
    print("%s : %d err\n", __FILE__, __LINE__);
    return;
  }
  CHAR_getCoordinationDir(CHAR_getInt(charaindex, CHAR_DIR),
                          CHAR_getInt(charaindex, CHAR_X),
                          CHAR_getInt(charaindex, CHAR_Y), 1, &x, &y);

  for (object = MAP_getTopObj(CHAR_getInt(charaindex, CHAR_FLOOR), x, y);
       object; object = NEXT_OBJECT(object)) {
    int showtoindex;

    int objindex = GET_OBJINDEX(object);

    if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
      continue;
    showtoindex = OBJECT_getIndex(objindex);

    if (CHAR_getInt(showtoindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      int i;
      char token[512] = "";
      int itemindex = -1;

      char *arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
      if (atoi(arg) == 0) {
        char show[CHAR_EQUIPPLACENUM][32] = {"头盔", "盔甲", "左武",
                                             "左饰", "右饰", "腰带",
                                             "右武", "鞋子", "手套"};
        for (i = 0; i < CHAR_EQUIPPLACENUM; i++) {
          itemindex = CHAR_getItemIndex(showtoindex, i);
          if (!ITEM_CHECKINDEX(itemindex))
            continue;

          sprintf(token, "%s:%s:%s", show[i],
                  ITEM_getChar(itemindex, ITEM_NAME),
                  ITEM_getChar(itemindex, ITEM_EFFECTSTRING));
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
        }

        CHAR_DelItem(charaindex, haveitemindex);
        return;
      }
#ifdef _PET_ITEM
      else {
        char show[CHAR_EQUIPPLACENUM][32] = {"头", "牙", "爪", "胸",
                                             "背", "翅", "脚"};
        int showtopetindex = CHAR_getCharPet(showtoindex, atoi(arg));
        for (i = 0; i < CHAR_PETITEMNUM; i++) {
          itemindex = CHAR_getItemIndex(showtopetindex, i);
          if (!ITEM_CHECKINDEX(itemindex))
            continue;

          sprintf(token, "%s:%s:%s", show[i],
                  ITEM_getChar(itemindex, ITEM_NAME),
                  ITEM_getChar(itemindex, ITEM_EFFECTSTRING));
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
        }

        CHAR_DelItem(charaindex, haveitemindex);
        return;
      }
#endif
    }
  }

  CHAR_talkToCli(charaindex, -1, "前方没有玩家", CHAR_COLORRED);
}
#endif

#ifdef _ITEM_EXP_LV
void ITEM_ExpLvBase(int charaindex, int exp) {
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
      CHAR_getInt(charaindex, CHAR_LV) >= 140) {
    char token[256];
    int toitemindex = -1;
    toitemindex = CHAR_getItemIndex(charaindex, 9);
    if (ITEM_CHECKINDEX(toitemindex)) {
      if (strlen(ITEM_getChar(toitemindex, ITEM_USEFUNC)) > 0 &&
          strcmp(ITEM_getChar(toitemindex, ITEM_USEFUNC), "ITEM_useExpLv") ==
              0) {
        char *itemarg = ITEM_getChar(toitemindex, ITEM_ARGUMENT);
        char buftest[128];
        int itemlv = 0;
        int itemexp = 0;
        int itembase = 0;
        int itemlvmax = 0;
        if (getStringFromIndexWithDelim(itemarg, "|", 1, buftest,
                                        sizeof(buftest)) == FALSE)
          return;
        itemlv = atoi(buftest);
        if (getStringFromIndexWithDelim(itemarg, "|", 2, buftest,
                                        sizeof(buftest)) == FALSE)
          return;
        itemexp = atoi(buftest);
        if (getStringFromIndexWithDelim(itemarg, "|", 3, buftest,
                                        sizeof(buftest)) == FALSE)
          return;
        itembase = atoi(buftest);
        if (getStringFromIndexWithDelim(itemarg, "|", 4, buftest,
                                        sizeof(buftest)) == FALSE)
          return;
        itemlvmax = atoi(buftest);
        if (itemlv >= itemlvmax) {
          if (RAND(1, 10) == 1) {
            sprintf(token, "当前经验丹等级：Lv%d，已满级，无法续存经验。",
                    itemlv);
            CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
          }
          return;
        }
        itemexp = itemexp + exp / 100 * itembase;
        int nextexp = 0;
        int uplvtype = 0;
        while (1) {
          if (itemlv >= itemlvmax) {
            break;
          }
          nextexp = getNeedLevelUpTbls(itemlv + 1);
          if (itemexp >= nextexp) {
            itemlv = itemlv + 1;
            itemexp = itemexp - nextexp;
            uplvtype = 1;
          } else {
            break;
          }
        }
        char newitembuf[256];
        sprintf(newitembuf, "%d|%d|%d|%d", itemlv, itemexp, itembase,
                itemlvmax);
        ITEM_setChar(toitemindex, ITEM_ARGUMENT, newitembuf);
        sprintf(newitembuf, "%s[Lv%d]", ITEM_getChar(toitemindex, ITEM_NAME),
                itemlv);
        ITEM_setChar(toitemindex, ITEM_SECRETNAME, newitembuf);
        CHAR_sendItemDataOne(charaindex, 9);
        if (RAND(1, 10) == 1) {
          sprintf(token, "当前经验丹等级：Lv%d，还欠经验：%d", itemlv,
                  nextexp - itemexp);
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
        }
        if (uplvtype == 1) {
          sprintf(token, "经验丹升级啦，目前的等级Lv%d，再接再厉哦！", itemlv);
          CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
        }
      }
    }
  }
}

void ITEM_useExpLv(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  char *itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  char buftest[128];
  int itemlevel;
  char token[256];
  if (getStringFromIndexWithDelim(itemarg, "|", 1, buftest, sizeof(buftest)) ==
      FALSE)
    return;
  itemlevel = atoi(buftest);
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    int myupcnt = itemlevel - CHAR_getInt(toindex, CHAR_LV);
    if (myupcnt <= 0) {
      CHAR_talkToCli(charaindex, -1, "此经验丹的等级还没你高啦，不需要用的哦。",
                     CHAR_COLORYELLOW);
      return;
    }
    if (CHAR_GetLevelExp(toindex, itemlevel) < 0) {
      sprintf(token, "目前你不能修炼到%d等级，该经验丹无法使用。", itemlevel);
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
      return;
    }
    CHAR_setInt(toindex, CHAR_LV, itemlevel);
    CHAR_setInt(toindex, CHAR_SKILLUPPOINT,
                CHAR_getInt(toindex, CHAR_SKILLUPPOINT) + myupcnt * getSkup());
    CHAR_setInt(toindex, CHAR_EXP, RAND(100, 20000));
    CHAR_complianceParameter(toindex);
    CHAR_send_P_StatusString(toindex, CHAR_P_STRING_LV | CHAR_P_STRING_NEXTEXP |
                                          CHAR_P_STRING_DUELPOINT);
    CHAR_Skillupsend(toindex);
    sprintf(token, "恭喜您的人物已经通过经验丹[Lv%d]迅速升级到了Lv%d",
            itemlevel, CHAR_getInt(toindex, CHAR_LV));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  } else {
    if (CHAR_getInt(toindex, CHAR_PETID) == 718 ||
        CHAR_getInt(toindex, CHAR_PETID) == 401) {
      CHAR_talkToCli(charaindex, -1, "该物品不能给MM使用！", CHAR_COLORYELLOW);
      return;
    }
    int myupcnt = itemlevel - CHAR_getInt(toindex, CHAR_LV);
    if (myupcnt <= 0) {
      CHAR_talkToCli(charaindex, -1, "此经验丹的等级还没你高啦，不需要用的哦。",
                     CHAR_COLORYELLOW);
      return;
    }
    if (CHAR_GetLevelExp(toindex, itemlevel) < 0) {
      sprintf(token, "目前你的宠物不能修炼到%d等级，该经验丹无法使用。",
              itemlevel);
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
      return;
    }
    int j;
    for (j = 1; j <= myupcnt; j++) { //升级
      CHAR_setWorkInt(toindex, CHAR_WORKITEMEXP, 1);
      CHAR_PetLevelUp(toindex);
      CHAR_PetAddVariableAi(toindex, AI_FIX_PETLEVELUP);
      CHAR_setInt(toindex, CHAR_LV, CHAR_getInt(toindex, CHAR_LV) + 1);
    }
    CHAR_setWorkInt(toindex, CHAR_WORKITEMEXP, 0);
    CHAR_setInt(toindex, CHAR_EXP, RAND(100, 20000));
    int i;
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      int workindex = CHAR_getCharPet(charaindex, i);
      if (workindex == toindex) {
        char token[64];
        CHAR_complianceParameter(toindex);
        snprintf(token, sizeof(token), "K%d", i);
        CHAR_sendStatusString(charaindex, token);
        snprintf(token, sizeof(token), "W%d", i);
        CHAR_sendStatusString(charaindex, token);
        CHAR_setInt(toindex, CHAR_HP, CHAR_getWorkInt(toindex, CHAR_WORKMAXHP));
        CHAR_setInt(toindex, CHAR_MP, CHAR_getWorkInt(toindex, CHAR_WORKMAXMP));
        CHAR_complianceParameter(toindex);
        snprintf(token, sizeof(token), "K%d", i);
        CHAR_sendStatusString(charaindex, token);
        snprintf(token, sizeof(token), "W%d", i);
        CHAR_sendStatusString(charaindex, token);
        break;
      }
    }
    sprintf(token, "恭喜您的宠物[%s]已经通过经验丹[Lv%d]迅速升级到了Lv%d",
            CHAR_getChar(toindex, CHAR_NAME), itemlevel,
            CHAR_getInt(toindex, CHAR_LV));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  }

  CHAR_DelItem(charaindex, haveitemindex);
}
#endif

void ITEM_UpPointResetItem(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char buf[32];
  int fd = getfdFromCharaIndex(charaindex);
  int vital, str, tough, dex, uppoint;
  CHAR_DelItem(charaindex, haveitemindex);
  vital = CHAR_getInt(charaindex, CHAR_VITAL) / 100;
  str = CHAR_getInt(charaindex, CHAR_STR) / 100;
  tough = CHAR_getInt(charaindex, CHAR_TOUGH) / 100;
  dex = CHAR_getInt(charaindex, CHAR_DEX) / 100;
  uppoint =
      CHAR_getInt(charaindex, CHAR_SKILLUPPOINT) + vital + str + tough + dex;
  CHAR_setInt(charaindex, CHAR_VITAL,
              CHAR_getInt(charaindex, CHAR_VITAL) - vital * 100);
  CHAR_setInt(charaindex, CHAR_STR,
              CHAR_getInt(charaindex, CHAR_STR) - str * 100);
  CHAR_setInt(charaindex, CHAR_TOUGH,
              CHAR_getInt(charaindex, CHAR_TOUGH) - tough * 100);
  CHAR_setInt(charaindex, CHAR_DEX,
              CHAR_getInt(charaindex, CHAR_DEX) - dex * 100);
  if (uppoint >= 10) {
    CHAR_setInt(charaindex, CHAR_VITAL,
                CHAR_getInt(charaindex, CHAR_VITAL) + 1000);
    uppoint = uppoint - 10;
  } else {
    CHAR_setInt(charaindex, CHAR_VITAL,
                CHAR_getInt(charaindex, CHAR_VITAL) + uppoint * 100);
    uppoint = 0;
  }
  CHAR_setInt(charaindex, CHAR_SKILLUPPOINT, uppoint);
  lssproto_SKUP_send(fd, uppoint);
  CHAR_complianceParameter(charaindex);
  CHAR_send_P_StatusString(charaindex,
                           CHAR_P_STRING_DUELPOINT | CHAR_P_STRING_VITAL |
                               CHAR_P_STRING_STR | CHAR_P_STRING_TOUGH |
                               CHAR_P_STRING_DEX);
  CHAR_talkToCli(charaindex, -1, "洗点完成，请重新加点！", CHAR_COLORYELLOW);
  CHAR_charSaveFromConnect(charaindex, FALSE);
  CHAR_talkToCli(charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
}

#ifdef _ALCHEMIST // 学习鉴定的道具
extern char alchetest_list[25][16];
void ITEM_useAlchemistPlus(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  char *arg = NULL;
  int alch_plus;
  char msg[1024];
  int i;

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    CHAR_talkToCli(charaindex, -1, "似乎没有效果．．．", CHAR_COLORYELLOW);
    return;
  }

  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  print(" Use:%s ", arg);
  for (i = 0; i < 25; i++) {
    if (strncmp(arg, alchetest_list[i], strlen(alchetest_list[i])) == NULL &&
        arg[strlen(alchetest_list[i])] == ' ') {
      int alch_old;
      char buf[64];
      sscanf(arg, "%s %d", buf, &alch_plus);
      alch_old = CHAR_getInt(toindex, CHAR_ITEMCHECK01 + i);
      CHAR_setInt(toindex, CHAR_ITEMCHECK01 + i, alch_old + alch_plus);
      if (alch_old + alch_plus < 0)
        CHAR_setInt(toindex, CHAR_ITEMCHECK01 + i, 0);
      if (alch_old + alch_plus > 100 * 1000)
        CHAR_setInt(toindex, CHAR_ITEMCHECK01 + i, 100 * 1000);
      print(" old:%d plus:%d ", alch_old, alch_plus);
      sprintf(msg, "增加鉴定 %s 的能力了！", alchetest_list[i]);
      CHAR_talkToCli(toindex, -1, msg, CHAR_COLORYELLOW);
      sprintf(msg, "%s 增加鉴定 %s 的能力了！",
              CHAR_getChar(toindex, CHAR_NAME), alchetest_list[i]);
      CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);

      CHAR_DelItem(charaindex, haveitemindex);
      lssproto_ALCHEPLUS_recv(CHAR_getWorkInt(toindex, CHAR_WORKFD));

      return;
    }
  }
  CHAR_talkToCli(charaindex, -1, "这个道具无法使用", CHAR_COLORYELLOW);
}
#endif
#ifdef _ITEM_RESETPET
void ITEM_ResetPet(int charaindex, int toindex, int haveitemindex) {
  char msg[256], buf[2048];
  char *petname;

  petname = CHAR_getUseName(toindex);
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
    CHAR_talkToCli(charaindex, -1, "其飘俊霸父 荤侩窍角 荐 乐嚼聪促.",
                   CHAR_COLORRED);
    return;
  }

  if (CHAR_getInt(toindex, CHAR_RESET) == 5) {
    CHAR_talkToCli(charaindex, -1, "秦寸其飘绰 歹捞惑 檬扁拳啊 阂啊瓷钦聪促！",
                   CHAR_COLORRED);
    return;
  }

  if (strcmp(petname, "檬扁拳") != 0) {
    CHAR_talkToCli(charaindex, -1, "其飘捞抚阑 [檬扁拳]肺 函版秦林技夸.",
                   CHAR_COLORRED);
    return;
  }

  if (CHAR_getInt(charaindex, CHAR_RIDEPET) != -1) {
    CHAR_talkToCli(charaindex, -1, "啪铰惑怕俊急 荤侩窍角 荐 绝嚼聪促.",
                   CHAR_COLORRED);
    return;
  }

  snprintf(msg, sizeof(msg),
           "\n急琶茄 其飘 : %s [ LV : %d ]"
           "\n急琶茄 其飘甫 檬扁拳 矫诺聪促."
           "\n檬扁拳啊 肯丰登菌阑锭"
           "\n其飘绰 捞傈栏肺 登倒副 荐 绝嚼聪促."
           "\n\n急琶茄 其飘甫 檬扁拳 窍矫摆嚼聪鳖？",
           CHAR_getChar(toindex, CHAR_NAME), CHAR_getInt(toindex, CHAR_LV));

  lssproto_WN_send(getfdFromCharaIndex(charaindex), WINDOW_MESSAGETYPE_MESSAGE,
                   WINDOW_BUTTONTYPE_OKCANCEL, CHAR_WINDOWTYPE_RESETPET, -1,
                   makeEscapeString(msg, buf, sizeof(buf)));
  CHAR_setWorkInt(charaindex, CHAR_WORKRESETITEMINDEX,
                  haveitemindex); //酒捞袍 困摹 历厘
  CHAR_setWorkInt(charaindex, CHAR_WORKRESETPETINDEX,
                  toindex); //其飘 牢郸胶 历厘
}

void ITEM_ResetPetResult(int charaindex, int seqno, int select, char *data) {
  if (select == WINDOW_BUTTONTYPE_OK) {
    char category[12], msg[256], buf[2048];
    int toindex = CHAR_getWorkInt(charaindex, CHAR_WORKRESETPETINDEX);
    int haveitemindex = CHAR_getWorkInt(charaindex, CHAR_WORKRESETITEMINDEX);
    int enemynum = ENEMY_getEnemyNum();
    int i, petID, r = 1;
    int level, vital, str, tgh, dex, resetp;

    for (i = 0; i < CHAR_MAXPETHAVE; i++) { //昏力且 其飘八荤
      int pindex = CHAR_getCharPet(charaindex, i);

      if (pindex == toindex) {
        petID = CHAR_getInt(toindex, CHAR_PETID);
        level = CHAR_getInt(toindex, CHAR_LV);
        vital = CHAR_getWorkInt(toindex, CHAR_WORKMAXHP);
        str = CHAR_getWorkInt(toindex, CHAR_WORKATTACKPOWER);
        tgh = CHAR_getWorkInt(toindex, CHAR_WORKDEFENCEPOWER);
        dex = CHAR_getWorkInt(toindex, CHAR_WORKQUICK);
        resetp = CHAR_getInt(toindex, CHAR_RESET);

        CHAR_endCharOneArray(toindex);
        CHAR_setCharPet(charaindex, i, -1);
        snprintf(category, sizeof(category), "K%d", i);
        CHAR_sendStatusString(charaindex, category);
        snprintf(category, sizeof(category), "W%d", i);
        CHAR_sendStatusString(charaindex, category);
      }
    }

    for (i = 0; i < enemynum; i++) {
      if (ENEMY_getInt(i, ENEMY_TEMPNO) == petID)
        break;
    }

    if (i == enemynum)
      return;

    int ret = ENEMY_createPetFromEnemyIndex(charaindex, i); // 其飘 牢郸胶 汲沥
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {                 // 其飘芒 犬牢
      if (CHAR_getCharPet(charaindex, i) == ret)            // 其飘 积己
        break;
    }

    if (i == CHAR_MAXPETHAVE)
      i = 0;
    if (CHAR_CHECKINDEX(ret) == TRUE) { // 犬牢
      CHAR_setInt(ret, CHAR_HP,
                  CHAR_getWorkInt(ret, CHAR_WORKMAXHP)); // HP 雀汗
      CHAR_setInt(ret, CHAR_WORKFIXAI, 100);             // 面己档
      CHAR_setInt(ret, CHAR_RESET, resetp + (int)r);
    }

    snprintf(msg, sizeof(msg),
             "\n沥惑利栏肺 檬扁拳啊 肯丰登菌嚼聪促 "
             "\n其飘 : %s"
             "\n傈 : Lv %d, 郴 %d, 傍 %d, 规 %d, 鉴 %d"
             "\n饶 : Lv %d, 郴 %d, 傍 %d, 规 %d, 鉴 %d"
             "\n\n巢篮 檬扁拳 冉荐 [%d] 雀",
             CHAR_getChar(ret, CHAR_NAME), level, vital, str, tgh, dex,
             CHAR_getInt(ret, CHAR_LV), CHAR_getWorkInt(ret, CHAR_WORKMAXHP),
             CHAR_getWorkInt(ret, CHAR_WORKATTACKPOWER),
             CHAR_getWorkInt(ret, CHAR_WORKDEFENCEPOWER),
             CHAR_getWorkInt(ret, CHAR_WORKQUICK)
             //檬扁拳 茄档 沥窍绰 箭磊
             ,
             5 - CHAR_getInt(ret, CHAR_RESET));

    lssproto_WN_send(getfdFromCharaIndex(charaindex),
                     WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                     CHAR_WINDOWTYPE_RESETPETNOMAL, -1,
                     makeEscapeString(msg, buf, sizeof(buf)));

    snprintf(category, sizeof(category), "K%d", i);
    CHAR_sendStatusString(charaindex, category);
    snprintf(category, sizeof(category), "W%d", i);
    CHAR_sendStatusString(charaindex, category);

    CHAR_DelItem(charaindex, haveitemindex); // 酒捞袍 力芭
    CHAR_setWorkInt(charaindex, CHAR_WORKRESETITEMINDEX, -1); //酒捞袍 困摹 力芭
    CHAR_setWorkInt(charaindex, CHAR_WORKRESETPETINDEX, -1); //其飘 牢郸胶 力芭
  } else {
    CHAR_setWorkInt(charaindex, CHAR_WORKRESETITEMINDEX, -1); //酒捞袍 困摹 力芭
    CHAR_setWorkInt(charaindex, CHAR_WORKRESETPETINDEX, -1); //其飘 牢郸胶 力芭
  }
}
#endif
#ifdef _ITEM_NOENEMY // vincent 经验提升
void ITEM_NoEnemyOnOff(charaindex, toindex, haveitemindex) {
  int itemindex, vtime;
  char *p = NULL, *arg;
  char szBuffer[1024] = "";
  char msg[1024];
  char test[256];
  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);

  if (!ITEM_CHECKINDEX(itemindex))
    return;
  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);

  if ((p = strstr(arg, "盒")) != NULL) {
    sscanf(p + 2, "%d", &vtime);
  }
  if (p == NULL)
    return;
  {
    if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME) + vtime * 60 >=
        86459) {
      CHAR_talkToCli(charaindex, -1,
                     "寒没狼 家扼 穿利 矫埃篮 24矫埃阑 逞扁瘤 给钦聪促.",
                     CHAR_COLORRED);
      return;
    }
  }
  /*vtime += CHAR_getWorkInt( charaindex, CHAR_WORKITEM_NOENEMYONTIME)/60;

  CHAR_setWorkInt( charaindex, CHAR_WORKITEM_NOENEMYONTIME, vtime*60 );

  CHAR_setInt( charaindex, CHAR_NOENEMYONOFF ,CHAR_getWorkInt( charaindex,
  CHAR_WORKITEM_NOENEMYONTIME) );*/
  CHAR_setWorkInt(
      charaindex, CHAR_WORKITEM_NOENEMYONTIME,
      (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME) + vtime * 60));

  sprintf(szBuffer, "寒没狼 家扼 矫埃捞 穿利 登菌嚼聪促. 荤侩矫埃 %d盒", vtime);
  CHAR_talkToCli(charaindex, -1, szBuffer, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex); // 酒捞袍 力芭
  CHAR_sendItemDataOne(charaindex, haveitemindex);
}
#endif
#ifdef _ITEM_UNCONFIRMED
void ITEM_Unconfirmed(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  char *itemarg = NULL;
  char buf[32];
  int itemnum = 0;
  int itemid = {-1};
  int si = 0, ret, i;
  char token[256];
  char effectStr[256];
  int emptyitemindexinchara = CHAR_findEmptyItemBox(charaindex);

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  if (emptyitemindexinchara <= 0) {
    CHAR_talkToCli(charaindex, -1, "酒捞袍 沫捞 啊垫 谩绢夸", CHAR_COLORWHITE);
    return;
  }

  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "") {
    CHAR_talkToCli(charaindex, -1, "固犬牢 酒捞袍[%s]狼 皑沥俊 角菩 沁绢夸.2",
                   CHAR_COLORRED);
    return;
  }

  if (getStringFromIndexWithDelim(itemarg, "|", 1, buf, sizeof(buf)) == FALSE) {
    CHAR_talkToCli(charaindex, -1, "固犬牢 酒捞袍[%s]狼 皑沥俊 角菩 沁绢夸.3",
                   CHAR_COLORRED);
    return;
  }
  itemnum = atoi(buf);

  if (itemnum < 0) {
    sprintf(token, "固犬牢 酒捞袍[%s]狼 皑沥俊 角菩 沁绢夸.",
            ITEM_getChar(itemindex, ITEM_NAME));
    CHAR_talkToCli(charaindex, -1, token, CHAR_COLORRED);
  }

  for (i = 0; i < itemnum; i++) {
    if (getStringFromIndexWithDelim(itemarg, "|", 2 + i, buf, sizeof(buf)) ==
        FALSE) {
      CHAR_talkToCli(charaindex, -1, "固犬牢 酒捞袍[%s]狼 皑沥俊 角菩 沁绢夸.",
                     CHAR_COLORRED);
    }
    itemid = atoi(buf);
    itemindex = ITEM_makeItemAndRegist(itemid);
    if (itemindex != -1) {

      CHAR_setItemIndex(charaindex, haveitemindex, -1);
      CHAR_sendItemDataOne(charaindex, haveitemindex);

      int i, ret;
      int num = 0;
      int itemid, Str, Tgh, Dex, Charm, Avoid, Critical;
      char buf[64];
      char msgbuf[128];
      char tempStr[256];
      char *magicname;

      itemid = ITEM_getInt(itemindex, ITEM_ID);
      Str = ITEM_getInt(itemindex, ITEM_MODIFYATTACK);
      Tgh = ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE);
      Dex = ITEM_getInt(itemindex, ITEM_MODIFYQUICK);
      Charm = ITEM_getInt(itemindex, ITEM_MODIFYCHARM);
      Avoid = ITEM_getInt(itemindex, ITEM_MODIFYAVOID);
      Critical = ITEM_getInt(itemindex, ITEM_CRITICAL);

      ret = getStringFromIndexWithDelim(
          ITEM_getChar(itemindex, ITEM_EFFECTSTRING), "[", 2, buf, sizeof(buf));
      if (ret)
        getStringFromIndexWithDelim(buf, "]", 1, buf, sizeof(buf));

      memset(effectStr, 0, sizeof(effectStr));

      if (Str > 0) {
        sprintf(tempStr, "傍+%d ", Str);
        strcat(effectStr, tempStr);
      } else if (Str < 0) {
        sprintf(tempStr, "傍%d ", Str);
        strcat(effectStr, tempStr);
      }
      if (Tgh > 0) {
        sprintf(tempStr, "规+%d ", Tgh);
        strcat(effectStr, tempStr);
      } else if (Tgh < 0) {
        sprintf(tempStr, "规%d ", Tgh);
        strcat(effectStr, tempStr);
      }
      if (Dex > 0) {
        sprintf(tempStr, "鉴+%d ", Dex);
        strcat(effectStr, tempStr);
      } else if (Dex < 0) {
        sprintf(tempStr, "鉴%d ", Dex);
        strcat(effectStr, tempStr);
      }
      if (Charm > 0) {
        sprintf(tempStr, "概+%d ", Charm);
        strcat(effectStr, tempStr);
      } else if (Charm < 0) {
        sprintf(tempStr, "概+%d ", Charm);
        strcat(effectStr, tempStr);
      }
      if (ret) {
        sprintf(tempStr, "[%s] ", buf);
        strcat(effectStr, tempStr);
        if (ITEM_getInt(itemindex, ITEM_MAGICID) <= 0) { //林贱 内靛啊 乐阑 版快
          for (i = 0; i < 20 - strlen(tempStr); i++) {
            strcat(effectStr, " ");
          }
        }
      } else {
        strcat(effectStr, "");
      }
      if (Avoid > 0) {
        sprintf(tempStr, "雀乔+%d ", Avoid);
        strcat(effectStr, tempStr);
      } else if (Avoid < 0) {
        sprintf(tempStr, "雀乔+%d ", Avoid);
        strcat(effectStr, tempStr);
      }
      if (Critical > 0) {
        sprintf(tempStr, "农府+%d ", Critical);
        strcat(effectStr, tempStr);
      } else if (Critical < 0) {
        sprintf(tempStr, "农府+%d ", Critical);
        strcat(effectStr, tempStr);
      }

      if (ITEM_getInt(itemindex, ITEM_MAGICID) > 0) { //林贱 内靛啊 乐阑 版快
        int marray = MAGIC_getMagicArray(
            ITEM_getInt(itemindex, ITEM_MAGICID)); //林贱 牢郸胶 历厘
        if (marray != -1) { //林贱 牢郸胶啊 -1捞 酒匆版快
          if ((magicname = MAGIC_getChar(marray, MAGIC_NAME)) !=
              NULL) { //林贱捞抚 NULL捞 酒匆 版快
            strcat(effectStr, magicname);
          }
        }
      } //固犬牢 场
      ITEM_setChar(itemindex, ITEM_EFFECTSTRING, effectStr);
      int emptyteimbox = CHAR_findEmptyItemBox(charaindex);
      CHAR_setItemIndex(charaindex, emptyteimbox, itemindex);
      CHAR_sendItemDataOne(charaindex, emptyteimbox);
      sprintf(token, "固犬牢 酒捞袍[%s]狼 皑沥俊 己傍 沁绢夸.",
              ITEM_getChar(itemindex, ITEM_NAME));
      CHAR_talkToCli(charaindex, -1, token, CHAR_COLORGREEN);
    }
  }
}
#endif
#ifdef _ITEM_METAMOGET
void ITEM_TotemInstallation(int charaindex, int itemindex) {
  int TotemList[25][17] = {
      //   {  公捞 ,  归龋 ,档扼厚胶,绊福捣, 扼绊绊, 肺单坷,   榜榜,  骇仟 ,
      //   单喉 , 促农沸, 哎仟胶, 匡仟胶,  酒父 , 促捞畴,  榜厚,  捞备 ,
      //   龋福捻},   格废*//
      //        0       1       2       3       4       5
      /*0*/ {121180, 121252, 121198, 121216, 121234, 121270, -1, -1, 121292, -1,
             -1, -1, -1, -1, -1, -1, -1}, //巢部竿捞
                                          /*1*/
      {121181, 121253, 121199, 121217, 121235, 121271, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1, 100420}, //籍扁固巢
                                /*2*/
      {121182, 121254, 121200, 121218, 121236, 121272, -1, -1, -1, -1, -1, -1,
       -1, -1, 121302, -1, -1}, //绢府滚府
                                /*3*/
      {121183, 121255, 121201, 121219, 121237, 121273, -1, -1, -1, -1, 121296,
       -1, -1, -1, -1, -1, -1}, //阂樊家斥
                                /*4*/
      {121184, 121256, 121202, 121220, 121238, 121274, -1, -1, -1, -1, -1, -1,
       121298, -1, -1, -1, -1}, //磅酒历揪
                                /*5*/
      {121185, 121257, 121203, 121221, 121239, 121275, -1, -1, -1, -1, -1, -1,
       -1, 121300, -1, -1, -1}, //牢叼攫倒艰
                                /*6*/
      {121186, 121258, 121204, 121222, 121240, 121276, -1, -1, 121293, -1, -1,
       -1, -1, -1, -1, -1, -1}, //咯部竿捞
                                /*7*/
      {121187, 121259, 121205, 121223, 121241, 121277, -1, -1, -1, -1, -1,
       121297, -1, -1, -1, -1, -1}, //拱俺家赤
                                    /*8*/
      {121188, 121260, 121206, 121224, 121242, 121278, -1, -1, -1, -1, -1, -1,
       -1, 121301, -1, -1, -1}, //郴件家赤
                                /*9*/
      {121189, 121261, 121207, 121225, 121243, 121279, -1, 121290, -1, -1, -1,
       -1, -1, -1, -1, -1, -1}, //蓖堪嫡捞
                                /*10*/
      {121190, 121262, 121208, 121226, 121244, 121280, -1, 121291, -1, -1, -1,
       -1, -1, -1, -1, -1, -1}, //磐橇家赤
                                /*11*/
      {121191, 121263, 121209, 121227, 121245, 121281, -1, -1, -1, -1, -1, -1,
       121299, -1, -1, -1, -1}, //个炉酒淋付
                                /*12*/
      {121196, 121264, 121214, 121233, 121250, 121282, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, 121882, -1}, //坯啊
                                /*13*/
      {121197, 121265, 121215, 121232, 121251, 121283, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, 121883, -1}, //牢叼攫
                                /*14*/
      {121187, 121259, 121205, 121223, 121241, 121277, -1, -1, -1, -1, -1,
       121297, -1, -1, -1, -1, -1}, //饭绢1拱俺
                                    /*15*/
      {121190, 121262, 121208, 121226, 121244, 121280, -1, 121291, -1, -1, -1,
       -1, -1, -1, -1, -1, -1}, //饭绢1磐橇
                                /*16*/
      {121181, 121253, 121199, 121217, 121235, 121271, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1, 100420}, //饭绢2籍固
                                /*17*/
      {121187, 121259, 121205, 121223, 121241, 121277, -1, -1, -1, -1, -1,
       121297, -1, -1, -1, -1, -1}, //饭绢2拱俺
                                    /*18*/
      {121186, 121258, 121204, 121222, 121240, 121276, -1, -1, 121293, -1, -1,
       -1, -1, -1, -1, -1, -1}, //饭绢2咯部
                                /*19*/
      {121183, 121255, 121201, 121219, 121237, 121273, -1, -1, -1, -1, 121296,
       -1, -1, -1, -1, -1, -1}, //饭绢2阂樊
                                /*20*/
      {121192, 121269, 121210, 121228, 121246, 121287, -1, -1, -1, 121295, -1,
       -1, -1, -1, -1, -1, -1}, //荤磊家斥
                                /*21*/
      {121193, 121268, 121211, 121229, 121247, 121286, -1, -1, -1, 121294, -1,
       -1, -1, -1, -1, -1, -1}, //匡焊家赤
                                /*22*/
      {121194, 121266, 121212, 121230, 121248, 121284, 121288, -1, -1, -1, -1,
       -1, -1, -1, -1, -1, -1}, //沥臂家赤
                                /*23*/
      {121195, 121267, 121213, 121231, 121249, 121285, 121289, -1, -1, -1, -1,
       -1, -1, -1, -1, -1, -1}, //官恩家斥
                                /*24*/
      {121196, 121264, 121214, 121233, 121250, 121282, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, 121882, -1}, //沥厘坯啊
  };
  int FaceList[25][17] = {
      //   { 公捞 , 归龋 , 倒厚 ,绊福捣,扼绊绊,肺单坷, 榜榜 , 骇仟 , 单喉
      //   ,促农沸,哎仟胶,匡仟胶, 酒父 ,促捞畴, 榜厚 , 捞备 ,龋福捻},    格废*//
      //       0      1      2      3      4      5      6      7      8      9
      //       10     11     12     13     14     15     16
      /*0*/ {51180, 51252, 51198, 51216, 51234, 51270, -1, -1, 51292, -1, -1,
             -1, -1, -1, -1, -1, -1}, //巢部竿捞
                                      /*1*/
      {51181, 51253, 51199, 51217, 51235, 51271, -1, -1, -1, -1, -1, -1, -1, -1,
       -1, -1, 60068}, //籍扁固巢
                       /*2*/
      {51182, 51254, 51200, 51218, 51236, 51272, -1, -1, -1, -1, -1, -1, -1, -1,
       51302, -1, -1}, //绢府滚府
                       /*3*/
      {51183, 51255, 51201, 51219, 51237, 51273, -1, -1, -1, -1, 51296, -1, -1,
       -1, -1, -1, -1}, //阂樊家斥
                        /*4*/
      {51184, 51256, 51202, 51220, 51238, 51274, -1, -1, -1, -1, -1, -1, 51298,
       -1, -1, -1, -1}, //磅酒历揪
                        /*5*/
      {51185, 51257, 51203, 51221, 51239, 51275, -1, -1, -1, -1, -1, -1, -1,
       51300, -1, -1, -1}, //牢叼攫倒艰
                           /*6*/
      {51186, 51258, 51204, 51222, 51240, 51276, -1, -1, 51293, -1, -1, -1, -1,
       -1, -1, -1, -1}, //咯部竿捞
                        /*7*/
      {51187, 51259, 51205, 51223, 51241, 51277, -1, -1, -1, -1, -1, 51297, -1,
       -1, -1, -1, -1}, //拱俺家赤
                        /*8*/
      {51188, 51260, 51206, 51224, 51242, 51278, -1, -1, -1, -1, -1, -1, -1,
       51301, -1, -1, -1}, //郴件家赤
                           /*9*/
      {51189, 51261, 51207, 51225, 51243, 51279, -1, 51290, -1, -1, -1, -1, -1,
       -1, -1, -1, -1}, //蓖堪嫡捞
                        /*10*/
      {51190, 51262, 51208, 51226, 51244, 51280, -1, 51291, -1, -1, -1, -1, -1,
       -1, -1, -1, -1}, //磐橇家赤
                        /*11*/
      {51191, 51263, 51209, 51227, 51245, 51281, -1, -1, -1, -1, -1, -1, 51299,
       -1, -1, -1, -1}, //个炉酒淋付
                        /*12*/
      {51196, 51264, 51214, 51233, 51250, 51282, -1, -1, -1, -1, -1, -1, -1, -1,
       -1, 51882, -1}, //坯啊
                       /*13*/
      {51197, 51265, 51215, 51232, 51251, 51283, -1, -1, -1, -1, -1, -1, -1, -1,
       -1, 51883, -1}, //牢叼攫
                       /*14*/
      {51187, 51259, 51205, 51223, 51241, 51277, -1, -1, -1, -1, -1, 51297, -1,
       -1, -1, -1, -1}, //饭绢1拱俺
                        /*15*/
      {51190, 51262, 51208, 51226, 51244, 51280, -1, 51291, -1, -1, -1, -1, -1,
       -1, -1, -1, -1}, //饭绢1磐橇
                        /*16*/
      {51181, 51253, 51199, 51217, 51235, 51271, -1, -1, -1, -1, -1, -1, -1, -1,
       -1, -1, 60068}, //饭绢2籍固
                       /*17*/
      {51187, 51259, 51205, 51223, 51241, 51277, -1, -1, -1, -1, -1, 51297, -1,
       -1, -1, -1, -1}, //饭绢2拱俺
                        /*18*/
      {51186, 51258, 51204, 51222, 51240, 51276, -1, -1, 51292, -1, -1, -1, -1,
       -1, -1, -1, -1}, //饭绢2咯部
                        /*19*/
      {51183, 51255, 51201, 51219, 51237, 51273, -1, -1, -1, -1, 51296, -1, -1,
       -1, -1, -1, -1}, //饭绢2阂樊
                        /*20*/
      {51192, 51269, 51210, 51228, 51246, 51287, -1, -1, -1, 51295, -1, -1, -1,
       -1, -1, -1, -1}, //荤磊家斥
                        /*21*/
      {51193, 51268, 51211, 51229, 51247, 51286, -1, -1, -1, 51294, -1, -1, -1,
       -1, -1, -1, -1}, //匡焊家赤
                        /*22*/
      {51194, 51266, 51212, 51230, 51248, 51284, 51288, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1}, //沥臂家赤
                        /*23*/
      {51195, 51267, 51213, 51231, 51249, 51285, 51289, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1}, //官恩家斥
                        /*24*/
      {51196, 51264, 51214, 51233, 51250, 51282, -1, -1, -1, -1, -1, -1, -1, -1,
       -1, 51882, -1}, //沥厘坯啊
  };
  int image;
  char bttleahp[256];
  char *itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  image = CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER);
  int i = -1;

  if (CHAR_getInt(charaindex, CHAR_CHARMETAMONO) == 0) {
    if (ITEM_getInt(itemindex, ITEM_FACEDATA) == 0) {
      ITEM_setInt(itemindex, ITEM_FACEDATA,
                  CHAR_getInt(charaindex, CHAR_FACEIMAGENUMBER));
    }
  }
  if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) { // 啪铰 惑怕啊 酒匆 版快
    CHAR_setInt(charaindex, CHAR_CHARMETAMO,
                CHAR_getInt(charaindex,
                            CHAR_BASEBASEIMAGENUMBER)); // 某腐磐 捞固瘤 历厘
  } else { // 啪铰 惑怕老 版快
    CHAR_setInt(charaindex, CHAR_CHARMETAMO,
                CHAR_getInt(charaindex,
                            CHAR_BASEBASEIMAGENUMBER)); // 某腐磐 捞固瘤 历厘
    CHAR_setInt(
        charaindex, CHAR_CHARMETAMORIDE,
        CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER)); // 啪铰 捞固瘤 历厘
  }
  CHAR_setInt(charaindex, CHAR_CHARMETAMONO, 1); // 逞滚 历厘

  if (strstr(itemarg, "公捞")) {
    i = 0;
  }
  if (strstr(itemarg, "归龋")) {
    i = 1;
  }
  if (strstr(itemarg, "档扼厚胶")) {
    i = 2;
  }
  if (strstr(itemarg, "绊福捣")) {
    i = 3;
  }
  if (strstr(itemarg, "扼绊绊")) {
    i = 4;
  }
  if (strstr(itemarg, "肺单坷")) {
    i = 5;
  }
  if (strstr(itemarg, "绊福绊福")) {
    i = 6;
  }
  if (strstr(itemarg, "海福仟胶")) {
    i = 7;
  }
  if (strstr(itemarg, "单喉肺胶")) {
    i = 8;
  }
  if (strstr(itemarg, "促农沸")) {
    i = 9;
  }
  if (strstr(itemarg, "哎仟胶")) {
    i = 10;
  }
  if (strstr(itemarg, "匡仟胶")) {
    i = 11;
  }
  if (strstr(itemarg, "酒捞胶父葛")) {
    i = 12;
  }
  if (strstr(itemarg, "促捞畴")) {
    i = 13;
  }
  if (strstr(itemarg, "榜靛肺厚")) {
    i = 14;
  }
  if (strstr(itemarg, "捞备肺胶")) {
    i = 15;
  }
  if (strstr(itemarg, "龋福捻")) {
    i = 16;
  }

  if (image >= 100000 && image < 100020) {
    if (TotemList[0][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[0][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[0][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[0][i]);
      }
    }
  } else if (image >= 100020 && image < 100040) {
    if (TotemList[1][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[1][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[1][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[1][i]);
      }
    }
  } else if (image >= 100040 && image < 100060) {
    if (TotemList[2][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[2][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[2][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[2][i]);
      }
    }
  } else if (image >= 100060 && image < 100080) {
    if (TotemList[3][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[3][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[3][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[3][i]);
      }
    }
  } else if (image >= 100080 && image < 100100) {
    if (TotemList[4][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[4][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[4][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[4][i]);
      }
    }
  } else if (image >= 100100 && image < 100120) {
    if (TotemList[5][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[5][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[5][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[5][i]);
      }
    }
  } else if (image >= 100120 && image < 100140) {
    if (TotemList[6][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[6][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[6][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[6][i]);
      }
    }
  } else if (image >= 100140 && image < 100160) {
    if (TotemList[7][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[7][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[7][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[7][i]);
      }
    }
  } else if (image >= 100160 && image < 100180) {
    if (TotemList[8][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[8][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[8][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[8][i]);
      }
    }
  } else if (image >= 100180 && image < 100200) {
    if (TotemList[9][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[9][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[9][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[9][i]);
      }
    }
  } else if (image >= 100200 && image < 100220) {
    if (TotemList[10][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[10][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[10][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[10][i]);
      }
    }
  } else if (image >= 100220 && image < 100240) {
    if (TotemList[11][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[11][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[11][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[11][i]);
      }
    }
  } else if (image >= 100780 && image < 100790) {
    if (TotemList[12][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[12][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[12][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[12][i]);
      }
    }
  } else if (image >= 100790 && image < 100800) {
    if (TotemList[13][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[13][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[13][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[13][i]);
      }
    }
  } else if (image >= 100800 && image < 100810) {
    if (TotemList[14][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[14][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[14][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[14][i]);
      }
    }
  } else if (image >= 100810 && image < 100820) {
    if (TotemList[15][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[15][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[15][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[15][i]);
      }
    }
  } else if (image >= 101157 && image < 101162) {
    if (TotemList[16][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[16][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[16][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[16][i]);
      }
    }
  } else if (image >= 101162 && image < 101167) {
    if (TotemList[17][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[17][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[17][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[17][i]);
      }
    }
  } else if (image >= 101167 && image < 101172) {
    if (TotemList[18][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[18][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[18][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[18][i]);
      }
    }
  } else if (image >= 101172 && image < 101177) {
    if (TotemList[19][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[19][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[19][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[19][i]);
      }
    }
  } else if (image >= 100700 && image < 100720) {
    if (TotemList[20][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[20][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[20][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[20][i]);
      }
    }
  } else if (image >= 100720 && image < 100740) {
    if (TotemList[21][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[21][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[21][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[21][i]);
      }
    }
  } else if (image >= 100740 && image < 100760) {
    if (TotemList[22][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[22][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[22][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[22][i]);
      }
    }
  } else if (image >= 100760 && image < 100780) {
    if (TotemList[23][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[23][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[23][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[23][i]);
      }
    }
  } else if (image >= 130026 && image < 130031) {
    if (TotemList[24][i] != -1) {
      CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER, FaceList[24][i]);
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) {
        CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER, TotemList[24][i]);
      } else {
        CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER, TotemList[24][i]);
      }
    }
  }

  char msg[256];
  sprintf(msg, "%d", CHAR_getInt(charaindex, CHAR_BASEBASEIMAGENUMBER),
          CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER));
  CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORWHITE);

  getStringFromIndexWithDelim(itemarg, "|", 2, bttleahp, sizeof(bttleahp));
  CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX),
                      CHAR_ACTHAND, NULL, 0, TRUE);
  CHAR_setWorkInt(charaindex, CHAR_WORKACTION, CHAR_ACTHAND);

  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  CHAR_setInt(
      charaindex, CHAR_BATTLEHP,
      CHAR_getInt(charaindex, CHAR_BATTLEHPUP)); // 泅犁 傈捧 郴备仿 雀汗 历厘
  CHAR_setInt(charaindex, CHAR_BATTLEHPUP,
              atoi(bttleahp)); // 傈捧 郴备仿雀汗 悸泼
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  CHAR_complianceParameter(charaindex);
}
void ITEM_TotemOut(int charaindex, int itemindex) {
  CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX),
                      CHAR_ACTACTIONSTAND, NULL, 0, TRUE);
  CHAR_setWorkInt(charaindex, CHAR_WORKACTION, CHAR_ACTACTIONSTAND);

  if (CHAR_getInt(charaindex, CHAR_RIDEPET) == -1) { // 啪铰 惑怕啊 酒匆 版快
    CHAR_setInt(charaindex, CHAR_BASEBASEIMAGENUMBER,
                CHAR_getInt(charaindex, CHAR_CHARMETAMO)); // 某腐磐 捞固瘤 函脚
    CHAR_setInt(charaindex, CHAR_BASEIMAGENUMBER,
                CHAR_getInt(charaindex, CHAR_CHARMETAMO));
  } else { // 啪铰 惑怕老 版快
    CHAR_setInt(
        charaindex, CHAR_BASEIMAGENUMBER,
        CHAR_getInt(charaindex, CHAR_CHARMETAMORIDE)); // 啪铰 捞固瘤 函脚
  }

  CHAR_setInt(charaindex, CHAR_CHARMETAMONO, 0);
  CHAR_setInt(charaindex, CHAR_FACEIMAGENUMBER,
              ITEM_getInt(itemindex, ITEM_FACEDATA));
  CHAR_setInt(charaindex, CHAR_BATTLEHPUP,
              CHAR_getInt(charaindex, CHAR_BATTLEHP)); // 傈捧 郴备仿雀汗 府悸
  ITEM_setInt(itemindex, ITEM_FACEDATA, 0);

  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
  CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_BASEBASEIMAGENUMBER);
  CHAR_complianceParameter(charaindex);
}
#endif
void ITEM_FamilyBadge(int charaindex, int toindex, int haveitemindex) {
  int itemindex;
  char *arg;
  char buffer[256];

  itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (!ITEM_CHECKINDEX(itemindex))
    return;

  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);

  if (arg == "\0")
    return;
  if (*arg == 0)
    return;
  if (CHAR_getInt(charaindex, CHAR_FMINDEX) < 0)
    return;

  saacproto_ACFixFMData_send(acfd, CHAR_getChar(charaindex, CHAR_FMNAME),
                             CHAR_getInt(charaindex, CHAR_FMINDEX),
                             CHAR_getWorkInt(charaindex, CHAR_WORKFMINDEXI),
                             FM_FIX_BADGE, arg, "",
                             CHAR_getWorkInt(charaindex, CHAR_WORKFMCHARINDEX),
                             CONNECT_getFdid(getfdFromCharaIndex(charaindex)));
  CHAR_talkToCli(charaindex, -1, arg, CHAR_COLORWHITE);
  CHAR_talkToCli(charaindex, -1, "SENT BADGE OK!", CHAR_COLORWHITE);

  // int  fmindex_wk = CHAR_getWorkInt(charaindex, CHAR_WORKFMINDEXI);
  // memberlist[fmindex_wk].badge = atoi(arg);
  // CHAR_complianceParameter(charaindex);
  // CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex,
  // CHAR_WORKOBJINDEX));
}

void ITEM_BagSlotExtend(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif
  int battlemode;
  char szTemp[64];
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else {
    if (battlemode) {

    } else {
      if (haveitemindex < CHAR_STARTITEMARRAY ||
          haveitemindex > CHAR_getMaxInventory(charaindex) - 1) {
        CHAR_talkToCli(charaindex, -1, "INvALID!", CHAR_COLORWHITE);
        return;
      }

      int itemIdx2 = CHAR_getItemIndex(charaindex, haveitemindex);
      if (itemIdx2 == -1)
        return;
      if (CHAR_getInt(charaindex, CHAR_EXTRA_INVENTORY) >= 2) {
        CHAR_talkToCli(charaindex, -1, "MAX BAG LIMIT", CHAR_COLORWHITE);
        return;
      }

      CHAR_DelItem(charaindex, haveitemindex);
      CHAR_setInt(charaindex, CHAR_EXTRA_INVENTORY,
                  CHAR_getInt(charaindex, CHAR_EXTRA_INVENTORY) + 1);
      CHAR_sendStatusString(charaindex, "V");
    }
  }
}
#ifdef _ITEM_RANDOMBOX
void ITEM_RandBox(int charaindex, int toindex,
                  int haveitemindex) { // 犬伏 弥历蔼篮 0.001 涝聪促.
  int i, j, itemindex, defaultItemindex, newitemindex = -1, ret;
  int newItemindex[10] = {
      0,
  };
  double newItemPersent[10] = {
      0,
  };
  char *itemarg = NULL;
  char token[256];
  char tempStr[256];
  char itemnumStr[256];
  char itemindexStr[256];
  char itempersentStr[256];
  int itemnum, val;

  itemindex = CHAR_getItemIndex(charaindex,
                                haveitemindex); // 荤侩茄 酒捞袍 牢郸胶 啊廉咳
  if (!ITEM_CHECKINDEX(itemindex))
    return; // 粮犁窍瘤 臼绰 酒捞袍牢郸胶老 版快 辆丰

  if (CHAR_findEmptyItemBox(charaindex) < 0) {
    CHAR_talkToCli(charaindex, -1, "酒捞袍芒捞 啊垫 谩嚼聪促.", CHAR_COLORRED);
    return;
  }

  itemarg = ITEM_getChar(itemindex,
                         ITEM_ARGUMENT); // 酒捞袍 arg何盒狼 巩磊凯阑 啊廉咳
  if (itemarg == "\0") { // 巩磊凯捞 厚绢 乐阑 版快 辆丰
    return;
  }

  if (getStringFromIndexWithDelim(itemarg, "|", 1, itemnumStr,
                                  sizeof(itemnumStr)) ==
      FALSE) { // 力老 菊俊 箭磊甫 啊廉柯促.
    return;
  }
  itemnum = atoi(itemnumStr); // 第俊 利腮 酒捞袍 肮荐捞促.

  if (itemnum > 10)
    itemnum = 10; // 弥措 肮荐绰 10俺肺 力茄茄促. 10俺 檬苞茄 何盒篮 久塞

  if (getStringFromIndexWithDelim(itemarg, "|", 2, tempStr, sizeof(tempStr)) ==
      FALSE) { // 力老 菊俊 箭磊甫 啊廉柯促.
    return;
  }
  defaultItemindex = atoi(tempStr); // 参

  for (i = 0; i < itemnum; i++) {
    if (getStringFromIndexWithDelim(itemarg, "|", i + 3, tempStr,
                                    sizeof(tempStr)) ==
        FALSE) { // tempStr肺 | 付促 磊抚
      break;
    }
    if (getStringFromIndexWithDelim(tempStr, "%", 1, itemindexStr,
                                    sizeof(itemindexStr)) ==
        FALSE) { // item内靛 啊廉咳
      printf("\nerr ITEM_RAND 酒捞袍内靛 何盒 坷幅\n");
      return;
    }
    newItemindex[i] = atoi(itemindexStr); // 酒捞袍 内靛
    if (getStringFromIndexWithDelim(tempStr, "%", 2, itempersentStr,
                                    sizeof(itempersentStr)) ==
        FALSE) { // item犬伏 啊廉咳
      printf("\nerr ITEM_RAND 犬伏 何盒 坷幅\n");
      return;
    }
    if (0.001 > atof(itempersentStr)) {
      printf("\nerr ITEM_RAND 弥窍 犬伏篮 0.001涝聪促.\n");
      return;
    }
    newItemPersent[i] = atof(itempersentStr); // 酒捞袍 犬伏
  }

  for (i = 1; i < itemnum; i++)
    newItemPersent[i] += newItemPersent[i - 1]; // 欺季飘蔼 鉴瞒利栏肺 歹秦初扁
  if (newItemPersent[i - 1] > 100) {
    printf("\nerr ITEM_RAND 欺季飘 醚钦捞 100%啊 逞嚼聪促.\n");
    return;
  }
  for (i = 0; i < itemnum; i++)
    newItemPersent[i] *= 1000; // 葛电蔼俊 1000阑 蚌秦淋

  srand(time(NULL) + clock());
  val = rand() %
        100000; // 犬伏 100%甫 唱串 0.001 = 1 // 100000焊促 撤篮 蔼 唱柯促.

  for (i = 0; i < itemnum; i++) {
    if (newItemPersent[i] > val)
      break;
  }

  // 肋登瘤父 困俊膊 歹 炒促!
  /*val = RAND(1, 100000);
  int accum = 0;

  for (i = 0; i < itemnum; i++) {
          if ((val > accum) && (val <= newItemPersent[i])) {
                  break;
          }
          else {
                  if (i == itemnum - 1) { // 辆丰
                          i = itemnum;
                          break;
                  }
                  accum = newItemPersent[i];
          }
  }*/

  CHAR_DelItem(charaindex, haveitemindex); // 傈府前 惑磊 昏力

  if (i == itemnum) { // 参
    newitemindex = ITEM_makeItemAndRegist(defaultItemindex);
    sprintf(token, "%s阑(甫) 裙垫 窍看嚼聪促。",
            ITEM_getChar(newitemindex, ITEM_NAME));
  } else {
    newitemindex = ITEM_makeItemAndRegist(newItemindex[i]);
    sprintf(token, "%s阑(甫) 裙垫 窍看嚼聪促。",
            ITEM_getChar(newitemindex, ITEM_NAME));
  }
  if (!ITEM_CHECKINDEX(newitemindex)) {
    CHAR_talkToCli(charaindex, -1, "Error", CHAR_COLORRED);
    return;
  }
  ret = CHAR_addItemSpecificItemIndex(charaindex, newitemindex);
  if (ret < 0 || ret >= CHAR_MAXITEMHAVE) {
    ITEM_endExistItemsOne(newitemindex);
    CHAR_talkToCli(charaindex, -1, "酒捞袍芒捞 啊垫 谩嚼聪促。",
                   CHAR_COLORWHITE);
    return;
  }
  CHAR_sendItemDataOne(charaindex, ret);
}
#endif
#ifdef _ITEM_ADDPOINT
void ITEM_AddRP(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *itemarg = "\0";
  char token[64];
  int point = CHAR_getInt(charaindex, CHAR_RAIDPOINT);
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  CHAR_setInt(charaindex, CHAR_RAIDPOINT, point + atoi(itemarg));
  sprintf(token, "[%d]饭捞靛 器牢飘甫 裙垫 窍看促。 醚 饭捞靛 器牢飘: %d",
          atoi(itemarg), point + atoi(itemarg));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
void ITEM_AddEvP(int charaindex, int toindex, int haveitemindex) {
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  char *itemarg = "\0";
  char token[64];
  int point = CHAR_getInt(charaindex, CHAR_EVENTPOINT);
  itemarg = ITEM_getChar(itemindex, ITEM_ARGUMENT);
  if (itemarg == "\0")
    return;
  CHAR_setInt(charaindex, CHAR_EVENTPOINT, point + atoi(itemarg));
  sprintf(token, "[%d]ＶＰ甫 裙垫 窍看促。 醚 ＶＰ: %d", atoi(itemarg),
          point + atoi(itemarg));
  CHAR_talkToCli(charaindex, -1, token, CHAR_COLORYELLOW);
  CHAR_DelItem(charaindex, haveitemindex);
}
#endif
#ifdef _ITEM_UPGRADE
static void ITEM_useUpgradeItem_PrintWindow(int charaindex, int page) {
  int fd;
  int pos = 0;
  int i;
  int btntype = WINDOW_BUTTONTYPE_CANCEL;
  int upgrade_percent = 0;
  char message[1024];
  char msgwk[1024];
  char buf[2048];

  if (!CHAR_CHECKINDEX(charaindex))
    return;                   //酒捞袍 牢郸胶 八荤
  if (page < 0 || page > 3) { //其捞瘤啊 沥惑利牢 箭磊啊 酒匆版快
    print("%s:%d err\n", __FILE__, __LINE__);
    return; //捞亥飘 辆丰
  }
  fd = getfdFromCharaIndex(charaindex);
  if (fd == -1)
    return;

  snprintf(message, sizeof(message),
           "2\n 碍拳甫 矫档且 酒捞袍阑 急琶 秦林技夸.\n\n");

  for (i = page * 5; i < page * 5 + 5; i++) {             //其捞瘤 八荤
    int itemindex = CHAR_getItemIndex(charaindex, i + 9); //八荤且 酒捞袍 牢郸胶
    BOOL flg = FALSE;                                     //八荤傈 妻胶
    while (1) {
      if (!ITEM_CHECKINDEX(itemindex))
        break; //酒捞袍 牢郸胶 八荤
      if (ITEM_getInt(itemindex, ITEM_TYPE) == 16 ||
          ITEM_getInt(itemindex, ITEM_TYPE) >= 20 &&
              ITEM_getInt(itemindex, ITEM_TYPE) <= 23 ||
          ITEM_getInt(itemindex, ITEM_TYPE) >= 28 ||
          ITEM_getInt(itemindex, ITEM_TYPE) == 5)
        break;    //八荤 措惑 力寇
      flg = TRUE; //八荤啊 场唱搁 飘风
      break;
    }
    if (flg) {                                       //八荤啊 场抄 饶
      char *nm = ITEM_getChar(itemindex, ITEM_NAME); //酒捞袍 捞抚 函荐 历厘
      int poitemindex = CHAR_getWorkInt(
          charaindex, CHAR_WORKUPGRADEITEMINDEX); //碍拳籍 酒捞袍 牢郸胶
      char *itemarg = ITEM_getChar(poitemindex, ITEM_ARGUMENT);
      char str[32], tgh[32], dex[32], vital[32], critical[32], yold[32],
          hitright[32];
      sprintf(str, "%d", ITEM_getInt(itemindex, ITEM_MODIFYATTACK));
      sprintf(tgh, "%d", ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE));
      sprintf(dex, "%d", ITEM_getInt(itemindex, ITEM_MODIFYQUICK));
      sprintf(vital, "%d", ITEM_getInt(itemindex, ITEM_MODIFYHP));
      sprintf(critical, "%d", ITEM_getInt(itemindex, ITEM_CRITICAL));
      sprintf(yold, "%d", ITEM_getInt(itemindex, ITEM_MODIFYAVOID));
      sprintf(hitright, "%d", ITEM_getInt(itemindex, ITEM_HITRIGHT));

      int upgrade = ITEM_getInt(itemindex, ITEM_UPGRADENO) + 1;
      if (upgrade == 1)
        upgrade_percent = 80; //酒捞袍 碍拳荐摹啊 1 老锭 己傍犬伏
      else if (upgrade >= 2 && upgrade <= 3)
        upgrade_percent = 60; //酒捞袍 碍拳荐摹啊 2~3 老锭 己傍犬伏
      else if (upgrade >= 4 && upgrade <= 5)
        upgrade_percent = 50; //酒捞袍 碍拳荐摹啊 4~5 老锭 己傍犬伏
      else if (upgrade >= 6 && upgrade <= 7)
        upgrade_percent = 30; //酒捞袍 碍拳荐摹啊 6~7 老锭 己傍犬伏
      else if (upgrade >= 8 && upgrade <= 9)
        upgrade_percent = 10; //酒捞袍 碍拳荐摹啊 8~9 老锭 己傍犬伏
      else if (upgrade >= 10 && upgrade <= 11)
        upgrade_percent = 5; //酒捞袍 碍拳荐摹啊 10~11 老锭 己傍犬伏
      else if (upgrade == 12)
        upgrade_percent = 5; //酒捞袍 碍拳荐摹啊 12 老锭 己傍犬伏

      if (itemarg != NULL) {
        upgrade_percent += atoi(itemarg);
      }
      char wk[256];
      if (pos + strlen(nm) + 1 > sizeof(msgwk)) {
        print("buffer over error %s:%d\n", __FILE__, __LINE__);
        break;
      }
      snprintf(wk, sizeof(wk), "%s[%s%s%s%s%s%s%s%s%s%s%s][%d%]\n", nm,
               ITEM_getInt(itemindex, ITEM_MODIFYATTACK) != 0 ? "傍" : "",
               ITEM_getInt(itemindex, ITEM_MODIFYATTACK) > 0 ? "+" : "",
               ITEM_getInt(itemindex, ITEM_MODIFYATTACK) != 0 ? str : "",
               ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0 ? " " : "",
               ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0 ? "规" : "",
               ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) > 0 ? "+" : "",
               ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0 ? tgh : "",
               ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0 ? " " : "",
               ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0 ? "鉴" : "",
               ITEM_getInt(itemindex, ITEM_MODIFYQUICK) > 0 ? "+" : "",
               ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0 ? dex : "",
               upgrade_percent); //钎扁且 格废 wk俊 历厘
      strncpy(&msgwk[pos], wk, sizeof(msgwk) - pos - 1);
      pos += strlen(wk);
    } else {
      if (pos + 2 > sizeof(msgwk)) {
        print("buffer over error %s:%d\n", __FILE__, __LINE__);
        break;
      }
      strncpy(&msgwk[pos], "\n", sizeof(msgwk) - pos - 1); //绝阑 版快 \n 钎扁
      pos += 1;
    }
  }
  strcat(message, msgwk);
  switch (page) {
  case 0:
    btntype |= WINDOW_BUTTONTYPE_NEXT; //促澜 滚瓢 (0其捞瘤)
    break;
  case 2:
    btntype |= WINDOW_BUTTONTYPE_PREV; //捞傈 滚瓢 (3其捞瘤)
    break;
  case 1:
    btntype |= WINDOW_BUTTONTYPE_PREV |
               WINDOW_BUTTONTYPE_NEXT; //促澜, 捞傈 滚瓢 (1, 2其捞瘤)
    break;
  }
  lssproto_WN_send(
      fd, WINDOW_MESSAGETYPE_SELECT, btntype,
      CHAR_WINDOWTYPE_UPGRADEITEM_PAGE1 + page, -1,
      makeEscapeString(message, buf, sizeof(buf))); //扩档快 皋矫瘤 芒
}

void ITEM_useUpgrade(int charaindex, int toindex, int haveitemindex) {
  ITEM_useUpgradeItem_PrintWindow(
      charaindex, 0); // ITEM_useUpgradeItem_PrintWindow 窃荐 角青

  CHAR_setWorkInt(charaindex, CHAR_WORKUPGRADEINDEX, -1);

  CHAR_setWorkInt(charaindex, CHAR_WORKUPGRADEITEMINDEX,
                  CHAR_getItemIndex(charaindex, haveitemindex));
}

void ITEM_useUpgradeItem_WindowResult(int charaindex, int seqno, int select,
                                      char *data) {
  int page = 0;
  int fd;
  if (select == WINDOW_BUTTONTYPE_CANCEL)
    return; //乃浇 滚瓢 努腐矫 捞亥飘 辆丰

  fd = getfdFromCharaIndex(charaindex);
  if (fd == -1)
    return;
  if (seqno == CHAR_WINDOWTYPE_UPGRADEITEM_UPGADE) {

  } else {
    if (select == WINDOW_BUTTONTYPE_NEXT)
      page = 1; //促澜 滚瓢 努腐矫 其捞瘤 函荐 1
    else if (select == WINDOW_BUTTONTYPE_PREV)
      page = -1; //捞傈 滚瓢 努腐矫 其捞瘤 函荐 -1
    if (select == WINDOW_BUTTONTYPE_NEXT ||
        select == WINDOW_BUTTONTYPE_PREV) { //促澜, 捞傈 吝 窍唱甫 努腐沁阑 版快
      int winno =
          seqno - CHAR_WINDOWTYPE_UPGRADEITEM_PAGE1; // seqno -
                                                     // CHAR_WINDOWTYPE_UPGRADEITEM_PAGE1
                                                     // (箭磊 15)甫 函荐俊 历厘
      winno += page; //困 if巩 八荤茄 其捞瘤 函荐狼 荐摹甫 歹秦淋
      if (winno < 0)
        winno = 0; //函荐啊 0焊促 累阑版快 0栏肺 瘤沥
      if (winno > 3)
        winno = 3; //函荐啊 2焊促 努版快 3栏肺 瘤沥
      ITEM_useUpgradeItem_PrintWindow(
          charaindex, winno); // ITEM_useUpgradeItem_PrintWindow 窃荐龋免
    } else {                  //酒捞袍阑 急琶 沁阑 锭
      char msg[256], buf[2048], itmename[64];
      char haveitemindex = CHAR_getWorkInt(charaindex, CHAR_WORKUPGRADEINDEX);
      int itemindex;
      if (haveitemindex == -1) { //酒捞袍 牢郸胶 困摹(have) 八荤
        haveitemindex = (seqno - CHAR_WINDOWTYPE_UPGRADEITEM_PAGE1) * 5 +
                        (atoi(data) - 1) + 9;
      }
      itemindex =
          CHAR_getItemIndex(charaindex, haveitemindex); //碍拳 酒捞袍 牢郸胶

      // int renameitemhaveindex = CHAR_getWorkInt(charaindex,
      // CHAR_WORKUPGRADEITEMINDEX);      //碍拳籍 酒捞袍 庆宏 牢郸胶
      int renameitemindex = CHAR_getWorkInt(
          charaindex,
          CHAR_WORKUPGRADEITEMINDEX); //碍拳籍 酒捞袍 牢郸胶
                                      //int percent = rand() % 100; //欺季飘
                                      //(1~100) 蔼 吝 罚待 荐摹 历厘
      int percent = rand() % 100;
      int upgrade_percent = 0; //诀弊饭捞靛 欺季飘(窜拌喊 犬伏) 历厘
      int upgrade =
          ITEM_getInt(itemindex, ITEM_UPGRADENO) + 1; //诀弊饭捞靛 函荐 历厘
      char *itemarg = ITEM_getChar(renameitemindex, ITEM_ARGUMENT);
      if (upgrade >= 13) { //酒捞袍 碍拳荐摹啊 13 捞惑 老锭
        snprintf(msg, sizeof(msg),
                 "\n秦寸 酒捞袍篮 歹捞惑 碍拳且 荐 绝嚼聪促."
                 "\n\n泅犁 酒捞袍狼 碍拳 荐摹绰 弥措摹 涝聪促."
                 "\n"
                 "\n酒捞袍 惑怕绰 扁粮栏肺 蜡瘤 邓聪促.");
        lssproto_WN_send(
            getfdFromCharaIndex(charaindex), WINDOW_MESSAGETYPE_MESSAGE,
            WINDOW_BUTTONTYPE_OK, -1, -1,
            makeEscapeString(msg, buf, sizeof(buf))); //扩档快 皋矫瘤 芒
        return;
      }
      if (upgrade == 1)
        upgrade_percent = 80; //酒捞袍 碍拳荐摹啊 1 老锭 己傍犬伏
      else if (upgrade >= 2 && upgrade <= 3)
        upgrade_percent = 60; //酒捞袍 碍拳荐摹啊 2~3 老锭 己傍犬伏
      else if (upgrade >= 4 && upgrade <= 5)
        upgrade_percent = 50; //酒捞袍 碍拳荐摹啊 4~5 老锭 己傍犬伏
      else if (upgrade >= 6 && upgrade <= 7)
        upgrade_percent = 30; //酒捞袍 碍拳荐摹啊 6~7 老锭 己傍犬伏
      else if (upgrade >= 8 && upgrade <= 9)
        upgrade_percent = 10; //酒捞袍 碍拳荐摹啊 8~9 老锭 己傍犬伏
      else if (upgrade >= 10 && upgrade <= 11)
        upgrade_percent = 5; //酒捞袍 碍拳荐摹啊 10~11 老锭 己傍犬伏
      else if (upgrade == 12)
        upgrade_percent = 5; //酒捞袍 碍拳荐摹啊 12 老锭 己傍犬伏

      if (itemarg != NULL) {
        upgrade_percent += atoi(itemarg);
      }

      if (percent <= upgrade_percent) { //碍拳 犬伏 己傍 沁阑 版快
        char *magicname;
        char *upgradearg = ITEM_getChar(itemindex, ITEM_UPGRADEARG);
        char firstvit[64], firststr[64], firsttgh[64], firstdex[64],
            firstcriti[64], firstyoid[64], firsthitright[64];
        char nowstr[64], nowtgh[64], nowdex[64], nowvit[64], nowcharm[64],
            nowcriti[64], nowyoid[64], Mname[64], set[256], data[64],
            nowpenetration[64], nowshield[64], nowhitright[64];

        int vit = 0, str = 0, tgh = 0, dex = 0, i = 0, penetration = 0,
            shield = 0, hitright = 0;
        // 1碍 = 1, 2碍 = 2, 3碍 =3, 4碍=4, 5碍=6, 6碍=7, 7碍 = 8, 8碍 = 9, 9碍
        // = 10, 10碍 = 13, 11碍 = 16, 12碍 = 19
        if (ITEM_getInt(itemindex, ITEM_TYPE) >= 0 &&
                ITEM_getInt(itemindex, ITEM_TYPE) <= 4 ||
            ITEM_getInt(itemindex, ITEM_TYPE) >= 17 &&
                ITEM_getInt(itemindex, ITEM_TYPE) <= 19) {
          if (percent <= 5) {
            str = 20;
            dex = 20;
          } else if (percent <= 30) {
            str = 15;
            dex = 15;
          } else {
            str = 10;
            dex = 10;
          }
        } //公扁 辆幅 傍拜仿 郴备仿 0~3 罚待
        else if (ITEM_getInt(itemindex, ITEM_TYPE) >= 6 &&
                 ITEM_getInt(itemindex, ITEM_TYPE) <= 7) {
          if (percent <= 10) {
            tgh = 20;
            vit = 40;
          } else if (percent <= 30) {
            tgh = 15;
            vit = 30;
          } else {
            tgh = 10;
            vit = 20;
          }
        } //规绢备 辆幅 傍拜仿 郴备仿 0~3 罚待
        else if (ITEM_getInt(itemindex, ITEM_TYPE) >= 8 &&
                     ITEM_getInt(itemindex, ITEM_TYPE) <= 15 ||
                 ITEM_getInt(itemindex, ITEM_TYPE) == 24 ||
                 ITEM_getInt(itemindex, ITEM_TYPE) >= 26 &&
                     ITEM_getInt(itemindex, ITEM_TYPE) <= 27) {
          /*if (upgrade == 5) { //5碍 焊呈胶 胶泡 + 2
                  str = 10;
                  dex = 10;
          }
          else if (upgrade == 10 || upgrade == 11 || upgrade == 12) { //10碍
          捞惑 焊呈胶 胶泡 + 3 str = 10; dex = 10;
          }*/
          if (percent <= 10) {
            str = 20;
            dex = 20;
          } else if (percent <= 30) {
            str = 15;
            dex = 15;
          } else { //弊寇 焊呈胶 胶泡 +1
            str = 10;
            dex = 10;
          }
        } //档备 辆幅 郴备仿 + 3
        else if (ITEM_getInt(itemindex, ITEM_TYPE) == 25) {
          if (percent <= 10) {
            str = 15;
            tgh = 15;
            dex = 15;
            vit = 15;
          } else if (percent <= 30) {
            str = 10;
            tgh = 10;
            dex = 10;
            vit = 10;
          } else {
            str = 5;
            tgh = 5;
            dex = 5;
            vit = 5;
          }
        }
        if (upgrade == 1) { //诀弊饭捞靛啊 0老 版快
          sprintf(data, "(%d)|(%d)|(%d)|(%d)|(%d)|(%d)",
                  ITEM_getInt(itemindex, ITEM_MODIFYATTACK),
                  ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE),
                  ITEM_getInt(itemindex, ITEM_MODIFYQUICK),
                  ITEM_getInt(itemindex, ITEM_MODIFYHP),
                  ITEM_getInt(itemindex, ITEM_CRITICAL),
                  ITEM_getInt(itemindex, ITEM_MODIFYAVOID));
          ITEM_setChar(itemindex, ITEM_UPGRADEARG,
                       data); //扁粮 瓷仿摹甫 ITEM_UPGRADEARG 历厘
          ITEM_setInt(
              itemindex, ITEM_UPGRADENO1,
              ITEM_getInt(
                  itemindex,
                  ITEM_MODIFYATTACK)); //扁粮 傍拜仿阑 ITEM_UPGRADEAR1 历厘
          ITEM_setInt(
              itemindex, ITEM_UPGRADENO2,
              ITEM_getInt(
                  itemindex,
                  ITEM_MODIFYDEFENCE)); //扁粮 规绢仿阑 ITEM_UPGRADENO2 历厘
          ITEM_setInt(
              itemindex, ITEM_UPGRADENO3,
              ITEM_getInt(
                  itemindex,
                  ITEM_MODIFYQUICK)); //扁粮 鉴惯仿阑 ITEM_UPGRADENO3 历厘
          ITEM_setInt(
              itemindex, ITEM_UPGRADENO4,
              ITEM_getInt(itemindex,
                          ITEM_MODIFYHP)); //扁粮 郴备仿阑 ITEM_UPGRADENO4 历厘
          ITEM_setInt(
              itemindex, ITEM_UPGRADENO5,
              ITEM_getInt(
                  itemindex,
                  ITEM_CRITICAL)); //扁粮 农府萍拿阑 ITEM_UPGRADENO5 历厘
          ITEM_setInt(
              itemindex, ITEM_UPGRADENO6,
              ITEM_getInt(
                  itemindex,
                  ITEM_MODIFYAVOID)); //扁粮 雀乔啦阑 ITEM_UPGRADENO6 历厘

          if (ITEM_getInt(itemindex, ITEM_BASEIMAGENUMBER) == 37736) {
            ITEM_setInt(itemindex, ITEM_BASEIMAGENUMBER, 37737); //酒捞袍 捞固瘤
          } else if (ITEM_getInt(itemindex, ITEM_BASEIMAGENUMBER) == 37738) {
            ITEM_setInt(itemindex, ITEM_BASEIMAGENUMBER, 37739); //酒捞袍 捞固瘤
          }
        }
        getStringFromIndexWithDelim(
            upgradearg, "|", 1, firststr,
            sizeof(firststr)); //檬扁 傍拜仿阑 函荐俊 历厘
        getStringFromIndexWithDelim(
            upgradearg, "|", 2, firsttgh,
            sizeof(firsttgh)); //檬扁 规绢仿阑 函荐俊 历厘
        getStringFromIndexWithDelim(
            upgradearg, "|", 3, firstdex,
            sizeof(firstdex)); //檬扁 鉴惯仿阑 函荐俊 历厘
        getStringFromIndexWithDelim(
            upgradearg, "|", 4, firstvit,
            sizeof(firstvit)); //檬扁 郴备仿阑 函荐俊 历厘
        getStringFromIndexWithDelim(
            upgradearg, "|", 5, firstcriti,
            sizeof(firstcriti)); //檬扁 农府萍拿阑 函荐俊 历厘
        getStringFromIndexWithDelim(
            upgradearg, "|", 6, firstyoid,
            sizeof(firstyoid)); //檬扁 雀乔啦阑 函荐俊 历厘
        getStringFromIndexWithDelim(
            upgradearg, "|", 7, firsthitright,
            sizeof(firsthitright)); //檬扁 疙吝伏阑 函荐俊 历厘

        ITEM_setInt(itemindex, ITEM_MODIFYATTACK,
                    ITEM_getInt(itemindex, ITEM_MODIFYATTACK) +
                        str); //酒捞袍 傍拜仿 眠啊
        ITEM_setInt(itemindex, ITEM_MODIFYDEFENCE,
                    ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) +
                        tgh); //酒捞袍 规绢仿 眠啊
        ITEM_setInt(itemindex, ITEM_MODIFYQUICK,
                    ITEM_getInt(itemindex, ITEM_MODIFYQUICK) +
                        dex); //酒捞袍 鉴惯仿 眠啊
        ITEM_setInt(itemindex, ITEM_MODIFYHP,
                    ITEM_getInt(itemindex, ITEM_MODIFYHP) +
                        vit); //酒捞袍 郴备仿 眠啊
        ITEM_setInt(itemindex, ITEM_COLOER, 6);
        if (penetration > 0) {
          ITEM_setInt(itemindex, ITEM_OTHERDAMAGE,
                      ITEM_getInt(itemindex, ITEM_OTHERDAMAGE) +
                          penetration); //酒捞袍 包悼 单固瘤
        }
        if (shield > 0) {
          ITEM_setInt(itemindex, ITEM_OTHERDEFC,
                      ITEM_getInt(itemindex, ITEM_OTHERDEFC) +
                          shield); //酒捞袍 蒋靛 规绢仿
        }
        if (hitright > 0) {
          ITEM_setInt(itemindex, ITEM_HITRIGHT,
                      ITEM_getInt(itemindex, ITEM_HITRIGHT) +
                          hitright); //酒捞袍 疙吝伏
        }

        ITEM_setInt(itemindex, ITEM_UPGRADENO, upgrade); //酒捞袍 诀弊饭捞靛

        sprintf(
            itmename, "+%d %s", ITEM_getInt(itemindex, ITEM_UPGRADENO),
            ITEM_getChar(itemindex, ITEM_NAME)); //酒捞袍 碍拳 捞抚 函荐俊 历厘
        ITEM_setChar(itemindex, ITEM_SECRETNAME, itmename); //酒捞袍 捞抚 函版

        sprintf(nowstr, "%d",
                ITEM_getInt(itemindex,
                            ITEM_MODIFYATTACK)); //巩磊凯 泅犁 傍拜仿 历厘
        sprintf(nowtgh, "%d",
                ITEM_getInt(itemindex,
                            ITEM_MODIFYDEFENCE)); //巩磊凯 泅犁 规绢仿 历厘
        sprintf(
            nowdex, "%d",
            ITEM_getInt(itemindex, ITEM_MODIFYQUICK)); //巩磊凯 泅犁 鉴惯仿 历厘
        sprintf(
            nowvit, "%d",
            ITEM_getInt(itemindex, ITEM_MODIFYHP)); //巩磊凯 泅犁 郴备仿 历厘
        sprintf(
            nowcharm, "%d",
            ITEM_getInt(itemindex, ITEM_MODIFYCHARM)); //巩磊凯 泅犁 概仿 历厘
        sprintf(
            nowcriti, "%d",
            ITEM_getInt(itemindex, ITEM_CRITICAL)); //巩磊凯 泅犁 农府萍拿 历厘
        sprintf(
            nowyoid, "%d",
            ITEM_getInt(itemindex, ITEM_MODIFYAVOID)); //巩磊凯 泅犁 雀乔啦 历厘
        sprintf(
            nowhitright, "%d",
            ITEM_getInt(itemindex, ITEM_HITRIGHT)); //巩磊凯 泅犁 疙吝伏 历厘
        sprintf(nowpenetration, "%d",
                ITEM_getInt(itemindex,
                            ITEM_OTHERDAMAGE)); //巩磊凯 泅犁 包烹 单固瘤 历厘
        sprintf(nowshield, "%d",
                ITEM_getInt(itemindex,
                            ITEM_OTHERDEFC)); //巩磊凯 泅犁 蒋靛 单固瘤 历厘

        memset(Mname, 0, sizeof(Mname));
        if (ITEM_getInt(itemindex, ITEM_MAGICID) > 0) { //林贱 内靛啊 乐阑 版快
          int marray = MAGIC_getMagicArray(
              ITEM_getInt(itemindex, ITEM_MAGICID)); //林贱 牢郸胶 历厘
          if (marray != -1) { //林贱 牢郸胶啊 -1捞 酒匆版快
            if ((magicname = MAGIC_getChar(marray, MAGIC_NAME)) !=
                NULL) { //林贱捞抚 NULL捞 酒匆 版快
              sprintf(Mname, "%s", magicname); // Mname 函荐俊 林贱 捞抚 历厘
            }
          }
        }

        sprintf(set,
                "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%"
                "s%s%s%s%s%s%s%s%s%s%s %s",
                (ITEM_getInt(itemindex, ITEM_MODIFYATTACK) != 0) ? "傍" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYATTACK) > 0) ? "+" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYATTACK) != 0) ? nowstr : "",
                ITEM_getInt(itemindex, ITEM_MODIFYATTACK) !=
                        ITEM_getInt(itemindex, ITEM_UPGRADENO1)
                    ? firststr
                    : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0) ? " " : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0) ? "规" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) > 0) ? "+" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0) ? nowtgh : "",
                ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) !=
                        ITEM_getInt(itemindex, ITEM_UPGRADENO2)
                    ? firsttgh
                    : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0) ? " " : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0) ? "刮" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYQUICK) > 0) ? "+" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0) ? nowdex : "",
                ITEM_getInt(itemindex, ITEM_MODIFYQUICK) !=
                        ITEM_getInt(itemindex, ITEM_UPGRADENO3)
                    ? firstdex
                    : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYHP) != 0) ? " " : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYHP) != 0) ? "郴" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYHP) > 0) ? "+" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYHP) != 0) ? nowvit : "",
                ITEM_getInt(itemindex, ITEM_MODIFYHP) !=
                        ITEM_getInt(itemindex, ITEM_UPGRADENO4)
                    ? firstvit
                    : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYCHARM) != 0) ? " " : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYCHARM) != 0) ? "概" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYCHARM) > 0) ? "+" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYCHARM) != 0) ? nowcharm : "",
                (ITEM_getInt(itemindex, ITEM_CRITICAL) != 0) ? " " : "",
                (ITEM_getInt(itemindex, ITEM_CRITICAL) != 0) ? "农府" : "",
                (ITEM_getInt(itemindex, ITEM_CRITICAL) > 0) ? "+" : "",
                (ITEM_getInt(itemindex, ITEM_CRITICAL) != 0) ? nowcriti : "",
                ITEM_getInt(itemindex, ITEM_CRITICAL) !=
                        ITEM_getInt(itemindex, ITEM_UPGRADENO5)
                    ? firstcriti
                    : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYAVOID) != 0) ? " " : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYAVOID) != 0) ? "雀乔" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYAVOID) > 0) ? "+" : "",
                (ITEM_getInt(itemindex, ITEM_MODIFYAVOID) != 0) ? nowyoid : "",
                ITEM_getInt(itemindex, ITEM_MODIFYAVOID) !=
                        ITEM_getInt(itemindex, ITEM_UPGRADENO6)
                    ? firstyoid
                    : "",
                ITEM_getInt(itemindex, ITEM_OTHERDAMAGE) != 0 ? " " : "",
                (ITEM_getInt(itemindex, ITEM_OTHERDAMAGE) > 0) ? "包烹+" : "",
                (ITEM_getInt(itemindex, ITEM_OTHERDAMAGE) > 0) ? nowpenetration
                                                               : " ",
                (ITEM_getInt(itemindex, ITEM_OTHERDEFC) > 0) ? "蒋靛+" : "",
                (ITEM_getInt(itemindex, ITEM_OTHERDEFC) > 0) ? nowshield : "",
                ITEM_getInt(itemindex, ITEM_HITRIGHT) != 0 ? " " : "",
                (ITEM_getInt(itemindex, ITEM_HITRIGHT) > 0) ? "疙吝+" : "",
                (ITEM_getInt(itemindex, ITEM_HITRIGHT) > 0) ? nowhitright : "",
                Mname); //酒捞袍 郴侩 八荤
        ITEM_setChar(itemindex, ITEM_EFFECTSTRING, set); //酒捞袍 郴侩 悸泼

        snprintf(msg, sizeof(msg),
                 "\n酒捞袍 碍拳俊 己傍窍看嚼聪促."
                 "\n\n碍拳 犬伏 %d 欺季飘 己傍!"
                 "\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
                 "\n碍拳 己傍俊 绵窍靛赋聪促!",
                 upgrade_percent,
                 ITEM_getInt(itemindex, ITEM_MODIFYATTACK) != 0 ? "傍" : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYATTACK) > 0 ? " + " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYATTACK) != 0 ? nowstr : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYATTACK) !=
                         ITEM_getInt(itemindex, ITEM_UPGRADENO1)
                     ? firststr
                     : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0 ? ", " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0 ? "规" : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) > 0 ? " + " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) != 0 ? nowtgh : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYDEFENCE) !=
                         ITEM_getInt(itemindex, ITEM_UPGRADENO2)
                     ? firsttgh
                     : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0 ? ", " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0 ? "刮" : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYQUICK) > 0 ? " + " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYQUICK) != 0 ? nowdex : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYQUICK) !=
                         ITEM_getInt(itemindex, ITEM_UPGRADENO3)
                     ? firstdex
                     : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYHP) != 0 ? ", " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYHP) != 0 ? "郴" : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYHP) > 0 ? " + " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYHP) != 0 ? nowvit : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYHP) !=
                         ITEM_getInt(itemindex, ITEM_UPGRADENO4)
                     ? firstvit
                     : "",
                 ITEM_getInt(itemindex, ITEM_CRITICAL) != 0 ? ", " : "",
                 ITEM_getInt(itemindex, ITEM_CRITICAL) != 0 ? "农府" : "",
                 ITEM_getInt(itemindex, ITEM_CRITICAL) > 0 ? " + " : "",
                 ITEM_getInt(itemindex, ITEM_CRITICAL) != 0 ? nowcriti : "",
                 ITEM_getInt(itemindex, ITEM_CRITICAL) !=
                         ITEM_getInt(itemindex, ITEM_UPGRADENO5)
                     ? firstcriti
                     : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYAVOID) != 0 ? ", " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYAVOID) != 0 ? "雀乔" : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYAVOID) > 0 ? " + " : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYAVOID) != 0 ? nowyoid : "",
                 ITEM_getInt(itemindex, ITEM_MODIFYAVOID) !=
                         ITEM_getInt(itemindex, ITEM_UPGRADENO6)
                     ? firstyoid
                     : "",
                 ITEM_getInt(itemindex, ITEM_OTHERDAMAGE) != 0 ? ", " : "",
                 ITEM_getInt(itemindex, ITEM_OTHERDAMAGE) > 0 ? "包烹+" : "",
                 ITEM_getInt(itemindex, ITEM_OTHERDAMAGE) > 0 ? nowpenetration
                                                              : "",
                 ITEM_getInt(itemindex, ITEM_OTHERDEFC) > 0 ? "蒋靛+" : "",
                 ITEM_getInt(itemindex, ITEM_OTHERDEFC) > 0 ? nowshield : "",
                 ITEM_getInt(itemindex, ITEM_HITRIGHT) != 0 ? ", " : "",
                 ITEM_getInt(itemindex, ITEM_HITRIGHT) > 0 ? "疙吝+" : "",
                 ITEM_getInt(itemindex, ITEM_HITRIGHT) > 0 ? nowhitright : "");

        lssproto_WN_send(
            getfdFromCharaIndex(charaindex), WINDOW_MESSAGETYPE_MESSAGE,
            WINDOW_BUTTONTYPE_OK, -1, -1,
            makeEscapeString(msg, buf, sizeof(buf))); //扩档快 皋矫瘤 芒

        if (ITEM_getInt(itemindex, ITEM_UPGRADENO) >= 8) { //
          int i;
          int playernum = CHAR_getPlayerMaxNum();

          for (i = 0; i < playernum; i++) {
            if (CHAR_getCharUse(i) != FALSE) {
              char token[256];
              sprintf(token,
                      "[绵窍皋技瘤] [%s](捞)啊 %s[+%d] 碍拳俊 己傍沁绢夸！",
                      CHAR_getChar(charaindex, CHAR_NAME),
                      ITEM_getChar(itemindex, ITEM_NAME),
                      ITEM_getInt(itemindex, ITEM_UPGRADENO));
              CHAR_talkToCli(i, -1, token, CHAR_COLORBLUE2);
            }
          }
        }

      } else {
        if (upgrade <= 5) { // 诀弊饭捞靛 4 捞窍 角菩矫 碍拳 酒捞袍 力芭
          snprintf(msg, sizeof(msg),
                   "\n酒捞袍 碍拳俊 角菩 窍看嚼聪促."
                   "\n\n+%d 碍拳狼 己傍犬伏篮 [%d] 欺季飘 涝聪促."
                   "\n[%d] 欺季飘 犬伏肺 角菩 窍看嚼聪促."
                   "\n酒捞袍 惑怕绰 扁粮栏肺 蜡瘤 邓聪促.",
                   ITEM_getInt(itemindex, ITEM_UPGRADENO) + 1, upgrade_percent,
                   100 - percent);

          lssproto_WN_send(
              getfdFromCharaIndex(charaindex), WINDOW_MESSAGETYPE_MESSAGE,
              WINDOW_BUTTONTYPE_OK, -1, -1,
              makeEscapeString(msg, buf, sizeof(buf))); //扩档快 皋矫瘤 芒
        } else { // 诀弊饭捞靛 5 捞惑 角菩矫 颇鲍
          snprintf(msg, sizeof(msg),
                   "\n酒捞袍 碍拳俊 角菩 窍看嚼聪促."
                   "\n\n+%d 碍拳 己傍犬伏篮 [%d] 欺季飘 涝聪促."
                   "\n[%d] 欺季飘 犬伏肺 角菩 窍看嚼聪促."
                   "\n[%s] 酒捞袍篮 颇鲍 邓聪促.",
                   ITEM_getInt(itemindex, ITEM_UPGRADENO) + 1, upgrade_percent,
                   100 - percent, ITEM_getChar(itemindex, ITEM_NAME));

          lssproto_WN_send(
              getfdFromCharaIndex(charaindex), WINDOW_MESSAGETYPE_MESSAGE,
              WINDOW_BUTTONTYPE_OK, -1, -1,
              makeEscapeString(msg, buf, sizeof(buf))); //扩档快 皋矫瘤 芒

          CHAR_setItemIndex(charaindex, haveitemindex, -1); //酒捞袍 牢郸胶 力芭
          CHAR_sendItemDataOne(charaindex,
                               haveitemindex); //酒捞袍沫 单捞磐 肺靛
        }
      }
      int i;
      for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
        int itemtempindex = CHAR_getItemIndex(charaindex, i);
        if (renameitemindex == itemtempindex) {
          CHAR_DelItem(charaindex, i);
          break;
        }
      }

      for (i = 0; i < CHAR_getMaxInventory(charaindex); i++) {
        CHAR_sendItemDataOne(charaindex, i);
      }
    }
  }
}
#endif

void ITEM_Mammoth(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
#ifdef _STREET_VENDOR
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
#endif
  int battlemode;
  char szTemp[64];
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else {
    if (battlemode) {

    } else {
      if (haveitemindex < CHAR_STARTITEMARRAY ||
          haveitemindex > CHAR_getMaxInventory(charaindex) - 1) {
        CHAR_talkToCli(charaindex, -1, "INvALID!", CHAR_COLORWHITE);
        return;
      }
      extern int MAMMOTH_REWARDS[][2];
      char buffer_[1024 * 4];
      int i;
      int ret = 0;
      *buffer_ = 0;
      for (i = 0; MAMMOTH_REWARDS[i][0] != -1; i++) {
        ret += snprintf(
            buffer_ + ret, sizeof(buffer_) - ret, "%d|",
            ITEMTBL_getInt(MAMMOTH_REWARDS[i][1], ITEM_BASEIMAGENUMBER));
      }
      lssproto_MAMMOTH_OPEN_send(getfdFromCharaIndex(charaindex), buffer_);
    }
  }
}

#ifdef ITEM_AURA_
void ITEM_AttachAura(int charaindex, int itemindex) {
  char *arg;
  int image;
  int layer;

  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);

  print("arg:%s\n", arg);

  if (arg == "\0")
    return;
  if (*arg == 0)
    return;

  if (strstr(arg, "AURA") == NULL)
    return;

  sscanf(arg + 4, "%d %d", &layer, &image);

  print("layer:%d\n", layer);
  print("image:%d\n", image);
  switch (layer) {
  case 1:
    CHAR_setInt(charaindex, CHAR_ITEM_AURA1, image);
    break;
  case 2:
    CHAR_setInt(charaindex, CHAR_ITEM_AURA2, image);
    break;
  default:
    break;
  }

  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
}

void ITEM_DetachAura(int charaindex, int itemindex) {
  char *arg;
  int image;
  int layer;

  arg = ITEM_getChar(itemindex, ITEM_ARGUMENT);

  if (arg == "\0")
    return;
  if (*arg == 0)
    return;

  if (strstr(arg, "AURA") == NULL)
    return;

  sscanf(arg + 4, "%d %d", &layer, &image);

  switch (layer) {
  case 1:
    CHAR_setInt(charaindex, CHAR_ITEM_AURA1, 0);
    break;
  case 2:
    CHAR_setInt(charaindex, CHAR_ITEM_AURA2, 0);
    break;
  default:
    break;
  }

  CHAR_sendCToArroundCharacter(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
}
#endif

void ITEM_EvolutionType1(int charaindex, int toindex, int haveitemindex) {
  if (!CHAR_CHECKINDEX(charaindex))
    return;
  if (!CHAR_CHECKINDEX(toindex))
    return;
  if (charaindex == toindex)
    return;
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET)
    return;

  if (CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    return;
  if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) != -1)
    return;
  int battlemode;
  char szTemp[64];
  battlemode = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE);
  if (battlemode == BATTLE_CHARMODE_INIT) {
  } else {
    if (battlemode) {
    } else {
      char token[256];
      int itemindex;
      int petid;
      int i, ret;
      int enemynum = ENEMY_getEnemyNum();

      itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
      if (!ITEM_CHECKINDEX(itemindex))
        return;
      if (strcmp(CHAR_getChar(toindex, CHAR_USERPETNAME), "柳拳") != 0)
        return;
      if (strstr(CHAR_getChar(toindex, CHAR_NAME), "(券)") != NULL)
        return;
      if (CHAR_getInt(toindex, CHAR_LV) != 199)
        return;
      if (CHAR_getInt(toindex, CHAR_EVOLUTION_STATS) != 0)
        return;

      petid = GetEvolutionPet(charaindex, CHAR_getInt(toindex, CHAR_PETID));
      print("petid:%d\n", petid);
      if (petid == -1)
        return;

      for (i = 0; i < enemynum; i++) {
        if (ENEMY_getInt(i, ENEMY_TEMPNO) == petid) {
          break;
        }
      }
      if (i == enemynum)
        return;

      ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
      if (!CHAR_CHECKINDEX(ret))
        return;

      SQL_DeletePetInRank(toindex);
      CHAR_DelPetForIndex(charaindex, toindex);
      CHAR_setInt(toindex, CHAR_EVOLUTION_STATS, 1);
      char *new_name = strcat(CHAR_getChar(ret, CHAR_NAME), " (券)");
      CHAR_setChar(ret, CHAR_NAME, new_name);

      for (i = 0; i < CHAR_MAXPETHAVE; i++) {
        if (CHAR_getCharPet(charaindex, i) == ret)
          break;
      }
      if (i == CHAR_MAXPETHAVE)
        return;

      if (CHAR_CHECKINDEX(ret) == TRUE) {
        CHAR_setMaxExpFromLevel(ret, CHAR_getInt(ret, CHAR_LV));
        CHAR_setInt(ret, CHAR_VARIABLEAI, 10000);
      }

      CHAR_complianceParameter(ret);
      CHAR_setInt(ret, CHAR_HP, CHAR_getWorkInt(ret, CHAR_WORKMAXHP));
      SQL_UpdatePetRank(ret);
      snprintf(token, sizeof(token), "K%d", i);
      CHAR_sendStatusString(charaindex, token);
      snprintf(token, sizeof(token), "W%d", i);
      CHAR_sendStatusString(charaindex, token);
      CHAR_DelItem(charaindex, haveitemindex);
    }
  }
}