#include "npc_manorsman.h"
#include "char.h"
#include "configfile.h"
#include "family.h"
#include "handletime.h"
#include "log.h"
#include "lssproto_serv.h"
#include "net.h"
#include "npc_scheduleman.h"
#include "npcutil.h"
#include "object.h"
#include "saacproto_cli.h"
#include "version.h"
#include <time.h>

/*
 * 庄园 PK 登记员
 *
 */
#ifdef _FAMILY_PK_FIX
int manorflag[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char manorname[10][32] = {"\O", "\O", "\O", "\O", "\O",
                          "\O", "\O", "\O", "\O", "\O"};
#endif

#define MAX_MANORSMAN 22
static int FMPK_ManorsmanList[MAX_MANORSMAN] = {-1, -1, -1, -1, -1, -1, -1, -1,
                                                -1, -1, -1, -1, -1, -1, -1, -1,
                                                -1, -1, -1, -1, -1, -1};

enum {
  NPC_WORK_ID = CHAR_NPCWORKINT1 // 登记员 ID, 从 0 开始
  ,
  NPC_WORK_MANORID = CHAR_NPCWORKINT2 // 庄园编号
  ,
  NPC_WORK_CHALLENGEWAIT = CHAR_NPCWORKINT3 // 挑战等待时间
  ,
  NPC_WORK_PEACEWAIT = CHAR_NPCWORKINT4 // 休战时间
  ,
  NPC_WORK_PREVLOOPTIME = CHAR_NPCWORKINT5 // 前一次处理 Loop 的时间
#ifdef _NEW_MANOR_LAW
  ,
  NPC_WORK_BETTLETIME = CHAR_NPCWORKINT6 // 记录开打时的日期
#endif
};

#define SCHEDULEFILEDIR "./Schedule/"

#ifdef _FIX_FAMILY_PK_LIMIT // WON ADD 将可挑战庄园的由前十大改为前三十大
#define PK_LIMIT 30
#endif

extern struct FM_POINTLIST fmpointlist;
extern void NPC_talkToFloor(int floor, int index1, int index2, char *data);
#ifdef _MANOR_PKRULE
extern struct FMS_DPTOP fmdptop;
#endif

void NPC_ManorLoadPKSchedule(int meindex); // Load schedule from disk
#ifdef _NEW_MANOR_LAW
void SortManorSchedule();
int SortManorScheduleMomentum(const void *indexa, const void *indexb);
extern int familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];
int g_iSortManor;
#endif

int NPC_getManorsmanListIndex(int ID) {
  if (ID < 0 || ID >= MAX_MANORSMAN) {
    return FMPK_ManorsmanList[ID];
  }
  return -1;
}

BOOL NPC_ManorSmanInit(int meindex) {
  static char argstr1[NPC_UTIL_GETARGSTR_BUFSIZE];
  char *argstr;
  int meid;
  int a;
  CHAR_setInt(meindex, CHAR_WHICHTYPE, CHAR_TYPEMANORSCHEDULEMAN);
  // 参数
  argstr = NPC_Util_GetArgStr(meindex, argstr1, sizeof(argstr1));
  meid = NPC_Util_GetNumFromStrWithDelim(argstr, "id");

  if ((meid < 0) || (meid >= MAX_SCHEDULEMAN)) {
    print("MANORSMAN init error: invalid ID(%d)\n", meid);
    meid = 0;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_ID, meid);
  a = NPC_Util_GetNumFromStrWithDelim(argstr, "loop");
  if ((a < 100) || (a > 10000))
    a = 1000;
  CHAR_setInt(meindex, CHAR_LOOPINTERVAL, a);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "manorid");
#ifdef FAMLYNUM_CF
  if ((a < 1) || (a > getfamlynum())) {
#else
  if ((a < 1) || (a > MANORNUM)) {
#endif
    print("MANORSMAN init error: invalid manor id(%d).", a);
    a = 1;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_MANORID, a);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "challengewait");
  if ((a < 1) || (a > 259200)) {
    print("MANORSMAN init error: invalid challengewait(%d).", a);
    a = 259200;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_CHALLENGEWAIT, a);
#ifdef _NOT_FMPK_WAIT
  CHAR_setWorkInt(meindex, NPC_WORK_PEACEWAIT, 0);
#else
  a = NPC_Util_GetNumFromStrWithDelim(argstr, "peacewait");
  if ((a < 1) || (a > 432000)) {
    print("MANORSMAN init error: invalid peacewait(%d).", a);
    a = 604800;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_PEACEWAIT, a);
#endif

  NPC_ManorLoadPKSchedule(meindex);

  CHAR_setWorkInt(meindex, NPC_WORK_PREVLOOPTIME, NowTime.tv_sec);
#ifdef _NEW_MANOR_LAW
  CHAR_setWorkInt(meindex, NPC_WORK_BETTLETIME, 0);
#endif

  if (CHAR_getWorkInt(meindex, NPC_WORK_ID) >= 0 &&
      CHAR_getWorkInt(meindex, NPC_WORK_ID) < MAX_MANORSMAN) {
    FMPK_ManorsmanList[CHAR_getWorkInt(meindex, NPC_WORK_ID)] = meindex;
  }

  return TRUE;
}

void NPC_ManorSmanTalked(int meindex, int talkerindex, char *msg, int color) {

  char buf[4096];
  int fd;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
  if (NPC_Util_CharDistance(meindex, talkerindex) > 2)
    return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2))
    return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1)
    return;

  int manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
#ifdef _NB_ALLOW_FMPK
  if (getAllowFMPK(manorid) == 0) {
    CHAR_talkToCli(talkerindex, meindex, getFMPKNoMsg(), CHAR_COLORRED);
    return;
  }
#else
  if (getAllowManorPK() == 0) {
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_YESNO,
                     -1, -1, "庄园所有权争夺战的挑战资格请到任务线下战书");
    return;
  }
#endif

  if (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_CHALLENGE) {
    fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_NONE;
  }
  switch (fmpks[fmpks_pos + 1].flag) {
  case FMPKS_FLAG_NONE: // 没有约定战斗，此时可以下战书
    saacproto_ACFMPointList_send(acfd);
    sprintf(buf, "庄园所有权争夺战的挑战资格\n\n"
                 "一、没有拥有庄园的家族\n"
                 "二、家族排行必需为前三十大家族\n"
                 "三、踢馆时间：下午六点至晚上十点\n\n"
                 "请稍待，我将确认你的资格。");
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_YESNO,
                     CHAR_WINDOWTYPE_CHECKMAN_START,
                     CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
    break;
  case FMPKS_FLAG_MANOR_PREPARE: // 目前已经定好战斗，准备中
  {
    int timeleft = fmpks[fmpks_pos + 1].dueltime - NowTime.tv_sec;
    int dd, hh, mm;
    char buf2[4096];
    dd = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 86400;
    hh = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 3600 - dd * 24;
    mm = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 60 - dd * 24 * 60 - hh * 60;
    memset(buf2, 0, sizeof(buf2));
    if (dd > 0)
      sprintf(buf, " %d 天", dd);
    else
      strcpy(buf, "");
    strcat(buf2, buf);
    if (hh > 0)
      sprintf(buf, " %d 小时", hh);
    else
      strcpy(buf, "");
    strcat(buf2, buf);
    if (mm > 0)
      sprintf(buf, " %d 分钟", mm);
    else
      strcpy(buf, "");
    strcat(buf2, buf);
    if (strlen(buf2) == 0)
      sprintf(buf,
              "庄园所有权争夺战\n『%s ＶＳ %s』\n\n"
              "即将开始，请准备入场。",
              fmpks[fmpks_pos + 1].guest_name, fmpks[fmpks_pos + 1].host_name);
    else
      sprintf(buf,
              "庄园所有权争夺战\n『%s ＶＳ %s』\n\n"
              "预定将在%s后开始。",
              fmpks[fmpks_pos + 1].guest_name, fmpks[fmpks_pos + 1].host_name,
              buf2);
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                     CHAR_WINDOWTYPE_CHECKMAN_END,
                     CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
  } break;
  case FMPKS_FLAG_MANOR_PEACE: // 战斗已经结束的和平时期
  {
    int timeleft = fmpks[fmpks_pos + 1].dueltime - NowTime.tv_sec;
    int dd, hh, mm;
    char buf2[4096];
    dd = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 86400;          //天
    hh = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 3600 - dd * 24; //小时
    mm = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 60 - dd * 24 * 60 -
         hh * 60; //秒
    strcpy(buf2, "");
    if (dd > 0)
      sprintf(buf, " %d 天", dd);
    else
      strcpy(buf, "");
    strcat(buf2, buf);
    if (hh > 0)
      sprintf(buf, " %d 小时", hh);
    else
      strcpy(buf, "");
    strcat(buf2, buf);
    if (mm > 0)
      sprintf(buf, " %d 分钟", mm);
    else
      strcpy(buf, "");
    strcat(buf2, buf);

    if (strlen(buf2) == 0)
      strcpy(buf, "请稍等，让我准备一下申请踢馆的表格。");
    else
      sprintf(buf, "现在是休战时期，要踢馆的话\n请%s后再来申请。", buf2);
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                     CHAR_WINDOWTYPE_CHECKMAN_END,
                     CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
  } break;
  case FMPKS_FLAG_MANOR_BATTLEBEGIN: // 目前正在进行踢馆
    sprintf(buf,
            "庄园所有权争夺战\n『%s ＶＳ %s』\n\n"
            "开～打～罗～\n还没进场的人赶快进场吧。",
            fmpks[fmpks_pos + 1].guest_name, fmpks[fmpks_pos + 1].host_name);
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                     CHAR_WINDOWTYPE_CHECKMAN_END,
                     CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET: // 在别的星球进行战斗
    sprintf(buf,
            "庄园所有权争夺战\n『%s ＶＳ %s』\n\n"
            "决斗地点在 %s 。",
            fmpks[fmpks_pos + 1].guest_name, fmpks[fmpks_pos + 1].host_name,
            fmpks[fmpks_pos + 2].host_name);
    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                     CHAR_WINDOWTYPE_CHECKMAN_END,
                     CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
    break;
  }
}

void NPC_ManorSmanWindowTalked(int meindex, int talkerindex, int seqno,
                               int select, char *data) {
  int fd, fmpks_pos, manorid, tkfmindex, tkfmdp;
  char buf[4096], token[256];
  int hadfmindex;
  char hadfmname[256];
  int hadfmpopular;

  if (NPC_Util_CharDistance(meindex, talkerindex) > 2)
    return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2))
    return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1)
    return;

  fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
  manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
  tkfmindex = CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
  tkfmdp = CHAR_getWorkInt(talkerindex, CHAR_WORKFMDP);

#ifdef _NB_ALLOW_FMPK
  if (getAllowFMPK(manorid) == 0) {
    CHAR_talkToCli(talkerindex, meindex, getFMPKNoMsg(), CHAR_COLORRED);
    return;
  }
#endif
  getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|", 5,
                              token, sizeof(token));
  hadfmindex = atoi(token);
  getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|", 6,
                              token, sizeof(token));
  strcpy(hadfmname, token);
  getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|", 7,
                              token, sizeof(token));
  hadfmpopular = atoi(token);

  switch (seqno) {
  case CHAR_WINDOWTYPE_CHECKMAN_START:
    if (select == WINDOW_BUTTONTYPE_YES) {
      if ((CHAR_getInt(talkerindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER) &&
          (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG) == 1)) {
        if (hadfmindex != -1) {
          if (hadfmindex - 1 != tkfmindex) {
            int check = 0, i;
#ifdef _FM_POINT_PK
            if (strcmp(getFmPointPK(), "是")) { //是否庄园互抢模式
#ifdef FAMLYNUM_CF
              for (i = 0; i < getfamlynum(); i++) { // 9个庄园
#else
              for (i = 0; i < MANORNUM; i++) { // 9个庄园
#endif
                getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|",
                                            5, token, sizeof(token));
                if (tkfmindex == atoi(token) - 1)
                  check = 1;
              }
            }
#else
#ifdef FAMLYNUM_CF
            for (i = 0; i < getfamlynum(); i++) { // 9个庄园
#else
            for (i = 0; i < MANORNUM; i++) { // 9个庄园
#endif
              getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5,
                                          token, sizeof(token));
              if (tkfmindex == atoi(token) - 1)
                check = 1;
            }
#endif
#ifdef FAMLYNUM_CF
            for (i = 1; i <= getfamlynum(); i++) { // 是否已经挑战其他庄园
#else
            for (i = 1; i <= MANORNUM; i++) { // 是否已经挑战其他庄园
#endif
              if ((fmpks[i * MAX_SCHEDULE + 1].guest_index == tkfmindex) &&
                  (strcmp(fmpks[i * MAX_SCHEDULE + 1].guest_name,
                          CHAR_getChar(talkerindex, CHAR_FMNAME)) == 0)) {
                check = 2;
              }

              if ((fmpks[i * MAX_SCHEDULE + 1].host_index == tkfmindex) &&
                  (strcmp(fmpks[i * MAX_SCHEDULE + 1].host_name,
                          CHAR_getChar(talkerindex, CHAR_FMNAME)) == 0)) {
                check = 3;
              }
            }
            if (check == 0) {
              int won1;
              //-----------------------------------------
              int index;
              won1 = 1;
              int mydp, todp;
              for (index = 0; index < FAMILY_MAXNUM;
                   index++) { // 找出挑战家族的索引
                if (fmdptop.fmtopid[index] == tkfmindex)
                  break;
              }

              if (index >= FAMILY_MAXNUM) {
                printf(
                    "\nNPC_ManorSmanWindowTalked():find tkfmIndex error (%d)",
                    tkfmindex);
              } else {
                // 如果挑战家族气势不足守庄家族气势的二成,不能挑战
                if (fmdptop.fmMomentum[index] < 500000)
                  won1 = 0;
                if (fmdptop.fmMomentum[index] <=
                    fmpointlist.fm_momentum[manorid - 1] * 0.2)
                  won1 = 0;
              }
              //-----------------------------------------
              if (won1 == 1) {
                sprintf(buf, "确定庄园挑战资格后会有一天的准备时间\n"
                             "你可以随时询问我以得知剩下的时间\n"
                             "\n"
                             "\n\n"
                             "你确定要挑战这个庄园吗？");
                lssproto_WN_send(
                    fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_YESNO,
                    CHAR_WINDOWTYPE_CHECKMAN_MAIN,
                    CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
              } else {
                sprintf(buf, "你的家族气势尚未达到5000或未达到挑战庄园的二成，"
                             "请再加油。[1]");
                lssproto_WN_send(
                    fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                    CHAR_WINDOWTYPE_CHECKMAN_END,
                    CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
              }
            } else if (check == 2) {
              sprintf(buf,
                      "你的家族正在挑战其他庄园\n请把机会留给其他家族吧！");
              lssproto_WN_send(
                  fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                  CHAR_WINDOWTYPE_CHECKMAN_END,
                  CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
            } else if (check == 3) {
              sprintf(buf,
                      "守庄的家族目前有约战！\n请等该家族族战过后再来约战吧！");
              lssproto_WN_send(
                  fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                  CHAR_WINDOWTYPE_CHECKMAN_END,
                  CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
            } else {
              sprintf(buf, "一个家族只能拥有一个庄园，\n"
                           "如果庄园家族之间需要切磋\n"
                           "请到家族ＰＫ场。");
              lssproto_WN_send(
                  fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                  CHAR_WINDOWTYPE_CHECKMAN_END,
                  CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
            }
          } else {
            sprintf(buf, "这个庄园已经是你的家族的喔。");
            lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
                             WINDOW_BUTTONTYPE_OK, CHAR_WINDOWTYPE_CHECKMAN_END,
                             CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
          }
        } else {
          sprintf(buf, "现在并没有庄园家族！\n"
                       "直接去申请迁入就可以了哟。");
          lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                           CHAR_WINDOWTYPE_CHECKMAN_END,
                           CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
        }
      } else {
        sprintf(buf, "只有族长可以下战书喔。");
        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                         CHAR_WINDOWTYPE_CHECKMAN_END,
                         CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
      }
    }
    break;
  case CHAR_WINDOWTYPE_CHECKMAN_MAIN:
    if (select == WINDOW_BUTTONTYPE_YES) {
      if ((CHAR_getInt(talkerindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)) {
        sprintf(buf, "族长才可进行下书！");
        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                         CHAR_WINDOWTYPE_CHECKMAN_END,
                         CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
        return;
      }
      if ((CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG) != 1)) {
        sprintf(buf, "家族需要正式成立了才可下书！");
        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                         CHAR_WINDOWTYPE_CHECKMAN_END,
                         CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
        return;
      }
      if ((hadfmindex == -1)) {
        sprintf(buf, "家族索引不存在？？？");
        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                         CHAR_WINDOWTYPE_CHECKMAN_END,
                         CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
        return;
      }
      if ((hadfmindex - 1 == tkfmindex)) {
        sprintf(buf, "自己不可下书自己的庄园！");
        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                         CHAR_WINDOWTYPE_CHECKMAN_END,
                         CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
        return;
      }

      int index;
      int mydp, todp;
      for (index = 0; index < FAMILY_MAXNUM; index++) { // 找出挑战家族的索引
        if (fmdptop.fmtopid[index] == tkfmindex)
          break;
      }
      if (index >= FAMILY_MAXNUM) {
        printf("\nNPC_ManorSmanWindowTalked():find tkfmIndex error (%d)",
               tkfmindex);
        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                         CHAR_WINDOWTYPE_CHECKMAN_END,
                         CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
                         "获取不到家族气势？");
        return;
      } else {
        // 如果挑战家族气势不足守庄家族气势的二成,不能挑战
        if (fmdptop.fmMomentum[index] < 500000) {
          lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                           CHAR_WINDOWTYPE_CHECKMAN_END,
                           CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
                           "你的家族气势尚未达到5000以上，请再加油。");
          return;
        }
        if (fmdptop.fmMomentum[index] <=
            fmpointlist.fm_momentum[manorid - 1] * 0.2) {
          lssproto_WN_send(
              fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
              CHAR_WINDOWTYPE_CHECKMAN_END,
              CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
              "你的家族气势尚未到达庄园家族气势的二成，请再加油。");
          return;
        }
      }
      /*
      if ((tkfmdp < hadfmpopular)){
              sprintf(buf, "未知错误:%d.%d",tkfmdp , hadfmpopular);
                              lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
                                               WINDOW_BUTTONTYPE_OK,
                                               CHAR_WINDOWTYPE_CHECKMAN_END,
                                               CHAR_getWorkInt(meindex,
      CHAR_WORKOBJINDEX), buf); return;
      }
                      */
      struct tm tm1;
      if (fmpks[fmpks_pos + 1].flag != FMPKS_FLAG_NONE) {
        sprintf(buf, "这个庄园已经有人约战了喔。");
        lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                         CHAR_WINDOWTYPE_CHECKMAN_END,
                         CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
        return;
      }
      sprintf(buf, "2\n请选择你要下族战的时间\n\n"
                   "                     ２０：００\n"
                   "                     ２０：３０\n"
                   "                     ２１：００\n"
                   "                     ２１：３０\n"
                   "                     ２２：００\n");
      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_OK,
                       CHAR_WINDOWTYPE_FAMILYMAN_PKTIME,
                       CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
      return;
    }
    break;
  case CHAR_WINDOWTYPE_FAMILYMAN_PKTIME:
    if (select == WINDOW_BUTTONTYPE_NONE) {
      int manorid, dueltime;
      char msg[1024], n1[256], n2[256], n3[256];
      char token[256], hadfmname[256];
      int hadfmindex = 0, hadfmpopular = 0, tkfmindex = 0;
      int PkFlg = 0;
      struct tm tm1;
      tkfmindex = CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
      fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
      manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
      memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
      // 以挑战时的时间为开打的时间,中间所经过的时间为准备时间
      switch (atoi(data)) {
      case 1:
        dueltime = NowTime.tv_sec - tm1.tm_min * 60 +
                   ((24 - tm1.tm_hour) * 3600) + 3600 * 20;
        break;
      case 2:
        dueltime = NowTime.tv_sec - tm1.tm_min * 60 +
                   ((24 - tm1.tm_hour) * 3600) + 3600 * 20 + 1800;
        break;
      case 3:
        dueltime = NowTime.tv_sec - tm1.tm_min * 60 +
                   ((24 - tm1.tm_hour) * 3600) + 3600 * 21;
        break;
      case 4:
        dueltime = NowTime.tv_sec - tm1.tm_min * 60 +
                   ((24 - tm1.tm_hour) * 3600) + 3600 * 21 + 1800;
        break;
      case 5:
        dueltime = NowTime.tv_sec - tm1.tm_min * 60 +
                   ((24 - tm1.tm_hour) * 3600) + 3600 * 22;
        break;
      default: //默认为21:00
        dueltime = NowTime.tv_sec - tm1.tm_min * 60 +
                   ((24 - tm1.tm_hour) * 3600) + 3600 * 21;
        break;
      }
      //				|-------------取整点-------------|
      //|--------加到0000时-------|  |挑战时的时间(算整点)|
      getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|",
                                  5, token, sizeof(token));
      hadfmindex = atoi(token);
      getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|",
                                  7, token, sizeof(token));
      hadfmpopular = atoi(token);
      getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|",
                                  6, token, sizeof(token));
      strcpy(hadfmname, token);
      makeEscapeString(hadfmname, n1, sizeof(n1));
      makeEscapeString(CHAR_getChar(talkerindex, CHAR_FMNAME), n2, sizeof(n2));
      makeEscapeString(getGameservername(), n3, sizeof(n3));
      sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s", dueltime, hadfmindex - 1, n1,
              tkfmindex, n2, 15, 50, FMPKS_FLAG_MANOR_OTHERPLANET, n3);
      PkFlg = 1;
      {
        char buf1[256];
        sprintf(buf1, "%d", CHAR_getWorkInt(meindex, NPC_WORK_ID));
        Logfmpk(n1, hadfmindex - 1, 0, n2, tkfmindex, 0, "", buf1, n3, 1);
      }
      saacproto_ACSendFmPk_send(acfd, talkerindex, 0,
                                CHAR_getWorkInt(meindex, NPC_WORK_ID), msg);
      sprintf(buf, "庄园挑战已经设定完成，请好好准备。");
      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OK,
                       CHAR_WINDOWTYPE_CHECKMAN_END,
                       CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
      //全服通知
      int i, iPlayerNum = CHAR_getPlayerMaxNum();
      char szMsg[256];
      char *fm_name[10] = {"萨姆吉尔", "玛丽娜斯", "加　　加", "卡鲁他那",
                           "伊　　甸", "塔 尔 塔", "尼 克 斯", "弗 列 顿",
                           "亚 伊 欧", "瑞尔亚斯"};
      struct tm *TM_temptime = NULL;
      time_t TM_t = dueltime;
      TM_temptime = localtime(&TM_t);
      char pktime[256];
      sprintf(pktime, "%2d月%2d日%02d:%02d", TM_temptime->tm_mon + 1,
              TM_temptime->tm_mday, TM_temptime->tm_hour, TM_temptime->tm_min);
      sprintf(szMsg, "%s 家族获得挑战 %s 庄园资格，开战时间为 %s", n2,
              fm_name[CHAR_getWorkInt(meindex, NPC_WORK_ID) - 1], pktime);
#ifdef _ALL_SERV_SEND
      saacproto_AllServSend_send(szMsg, 4);
#else
      for (i = 0; i < iPlayerNum; i++) {
        if (CHAR_getCharUse(i) != FALSE) {
          CHAR_talkToCli(i, -1, szMsg, CHAR_COLORBLUE2);
        }
      }
#endif
    }
    break;
  }
}

void NPC_CleanPkList(int ti) {
  // Terry fix 要清为-1不能清为0
  /*fmpks[ ti+1 ].host_index = 0;
          fmpks[ ti+1].guest_index=0;
          fmpks[ ti].host_index=0;
          fmpks[ ti].guest_index=0;*/
  fmpks[ti + 1].host_index = -1;
  fmpks[ti + 1].guest_index = -1;
  fmpks[ti].host_index = -1;
  fmpks[ti].guest_index = -1;

  strcpy(fmpks[ti + 1].host_name, "");
  strcpy(fmpks[ti + 1].guest_name, "");
  strcpy(fmpks[ti].host_name, "");
  strcpy(fmpks[ti].guest_name, "");
  strcpy(fmpks[ti + 2].host_name, "");
}

void NPC_ManorSmanLoop(int meindex) {
  struct tm tm1;
  int fmpks_pos;
  int iOffsetTime;

  if (CHAR_CHECKINDEX(meindex) == FALSE) {
    printf("\nNPC_ManorSmanLoop error!(meindex:%d)", meindex);
    return;
  }

  fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
  memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));

  switch (fmpks[fmpks_pos + 1].flag) {
  case FMPKS_FLAG_NONE:
    break;
  case FMPKS_FLAG_MANOR_PREPARE:
    if (fmpks[fmpks_pos + 1].dueltime <= NowTime.tv_sec) {
      memcpy(&fmpks[fmpks_pos], &fmpks[fmpks_pos + 1],
             sizeof(FamilyPKSchedule));
      fmpks[fmpks_pos].dueltime =
          tm1.tm_hour * 100 + tm1.tm_min; //山猪修改挑战时间
      fmpks[fmpks_pos].flag = FMPKS_FLAG_SCHEDULED;
      fmpks[fmpks_pos + 1].dueltime = 0;
      fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_MANOR_BATTLEBEGIN;
      NPC_talkToFloor(CHAR_getInt(meindex, CHAR_FLOOR),
                      fmpks[fmpks_pos].host_index, fmpks[fmpks_pos].guest_index,
                      "庄园争夺战已经开始，请尽快入场！");
      char token[256];
      sprintf(token, "『%s ＶＳ %s』庄园所有权争夺战即将开始～",
              fmpks[fmpks_pos + 1].guest_name, fmpks[fmpks_pos + 1].host_name);
#ifdef _ALL_SERV_SEND
      saacproto_AllServSend_send(token, 4);
#else
      int i, iPlayerNum = CHAR_getPlayerMaxNum();
      for (i = 0; i < iPlayerNum; i++) {
        if (CHAR_getCharUse(i) != FALSE) {
          CHAR_talkToCli(i, -1, szMsg, CHAR_COLORBLUE2);
        }
      }
#endif
    }
    break;
  case FMPKS_FLAG_MANOR_PEACE:
#ifdef _FAMILY_PK_FIX
    if (manorflag[CHAR_getWorkInt(meindex, NPC_WORK_MANORID) - 1] == 1)
      break;
    saacproto_ACFmPkOnlineSn_send(
        acfd, CHAR_getWorkInt(meindex, NPC_WORK_MANORID) - 1, 0);
#endif
    if (fmpks[fmpks_pos + 1].dueltime <= NowTime.tv_sec) {
      fmpks[fmpks_pos + 1].dueltime = 0;
      fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_NONE;
      NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_NONE,
                              fmpks[fmpks_pos + 1].dueltime, tm1);
    }
    break;
  case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    break;
  case FMPKS_FLAG_MANOR_BATTLEEND:
    fmpks[fmpks_pos].flag = FMPKS_FLAG_NONE;
    fmpks[fmpks_pos + 1].dueltime =
        CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
    fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_MANOR_PEACE_SAVE;
    NPC_CleanPkList(fmpks_pos);
    NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_MANOR_PEACE_SAVE,
                            fmpks[fmpks_pos + 1].dueltime, tm1);
    break;
  case FMPKS_FLAG_MANOR_PEACE_SAVE:
    fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_MANOR_PEACE;
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET:
    break;
  case FMPKS_FLAG_MANOR_READYTOFIGHT:
  case FMPKS_FLAG_MANOR_CLEANFLAG:
#ifdef _FAMILY_PK_FIX
    if (manorflag[CHAR_getWorkInt(meindex, NPC_WORK_MANORID) - 1] == 1)
      break;
    saacproto_ACFmPkOnlineSn_send(
        acfd, CHAR_getWorkInt(meindex, NPC_WORK_MANORID) - 1, 0);
#endif
    fmpks[fmpks_pos].flag = FMPKS_FLAG_NONE;
    fmpks[fmpks_pos + 1].dueltime =
        CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
    fmpks[fmpks_pos + 1].flag = FMPKS_FLAG_NONE;
    NPC_CleanPkList(fmpks_pos);
    NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_NONE,
                            fmpks[fmpks_pos + 1].dueltime, tm1);
    break;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_PREVLOOPTIME, NowTime.tv_sec);
}

void NPC_ManorLoadPKSchedule(int meindex) {
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID);
  saacproto_ACLoadFmPk_send(acfd, fmpks_pos);
}

void NPC_ManorSavePKSchedule(int meindex, int toindex, int flg, int setTime,
                             struct tm tm2) {
  int fmpks_pos, manorid, dueltime;
  char msg[1024], n1[256], n2[256], n3[256];
  char token[256], hadfmname[256];
  int hadfmindex = 0, hadfmpopular = 0, tkfmindex = 0;
  int PkFlg = 0;

  if (toindex == -1) {
    dueltime = CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
    sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s", dueltime, -1, "", -1, "", 0, 0,
            flg, "");
    PkFlg = 0;
  } else {
    struct tm tm1;
    tkfmindex = CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI);

    fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
    manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
    memcpy(&tm1, localtime((time_t *)&NowTime.tv_sec), sizeof(tm1));
    dueltime = NowTime.tv_sec - tm1.tm_min * 60 + ((24 - tm1.tm_hour) * 3600) +
               3600 * tm1.tm_hour;

    //				|-------------取整点-------------|  |--------加到0000时-------|
    //|挑战时的时间(算整点)|
    getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|", 5,
                                token, sizeof(token));
    hadfmindex = atoi(token);
    getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|", 7,
                                token, sizeof(token));
    hadfmpopular = atoi(token);
    getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid - 1], "|", 6,
                                token, sizeof(token));
    strcpy(hadfmname, token);
    makeEscapeString(hadfmname, n1, sizeof(n1));
    makeEscapeString(CHAR_getChar(toindex, CHAR_FMNAME), n2, sizeof(n2));
    makeEscapeString(getGameservername(), n3, sizeof(n3));
    if (strlen(n2) < 1) {
      sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s", setTime, -1, "", -1, "", 0, 0,
              flg, "");
      PkFlg = 0;
    } else {
      sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s", dueltime, hadfmindex - 1, n1,
              tkfmindex, n2, 15, 50, FMPKS_FLAG_MANOR_OTHERPLANET, n3);
      PkFlg = 1;
    }
    {
      char buf1[256];
      sprintf(buf1, "%d", CHAR_getWorkInt(meindex, NPC_WORK_ID));
      Logfmpk(n1, hadfmindex - 1, 0, n2, tkfmindex, 0, "", buf1, n3, 1);
    }
  }
  saacproto_ACSendFmPk_send(acfd, toindex, PkFlg,
                            CHAR_getWorkInt(meindex, NPC_WORK_ID), msg);
}
