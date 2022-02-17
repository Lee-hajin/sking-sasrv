#include "family.h"
#include "char.h"
#include "char_base.h"
#include "handletime.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "mylua/base.h"
#include "npc_fmwarpman.h"
#include "npc_scheduleman.h"
#include "saacproto_cli.h"
#include <string.h>
#ifdef _ALLBLUES_LUA

extern char familyListBuf[MAXFAMILYLIST];

static int ShowFamilyList(lua_State *L) {
  size_t l;
  const int index = luaL_checkint(L, 1);
  char subbuf[256];
  if (getStringFromIndexWithDelim(familyListBuf, "|", index, subbuf,
                                  sizeof(subbuf)) == TRUE) {
    lua_pushstring(L, subbuf);
  } else {
    lua_pushstring(L, "");
  }
  return 1;
}

static int ShowFamilyPkList(lua_State *L) {
  size_t l;
  const int index = luaL_checkint(L, 1);
  char NPC_sendbuf[128 * 10];
  memset(&NPC_sendbuf, 0, sizeof(NPC_sendbuf));
  char fmpointname[10][16] = {"萨姆吉尔", "玛丽娜丝", "加    加", "卡鲁它那",
                              "伊　　甸", "塔 尔 塔", "尼 克 斯", "弗 列 顿",
                              "亚 伊 欧", "瑞尔亚斯"};
  int i, fmpks_pos;
  char tmpbuf[1024];
  char fmtimebuff[64];
  char fmnumbuff[64];
  char fmtimebuff2[64];
  memset(tmpbuf, 0, sizeof(tmpbuf));
  memset(fmtimebuff, 0, sizeof(fmtimebuff));
  memset(fmnumbuff, 0, sizeof(fmnumbuff));
  strcpy(
      NPC_sendbuf,
      "[style c=4]                         庄 园 族 战 排 程 表[/style]\n\n");
  sprintf(tmpbuf, "[style c=5]%-8s　%-16s　%-16s  %-16s  %-16s[/style]\n",
          "庄    园" //庄园名字
          ,
          "攻方家族" //攻方家族
          ,
          "守方家族" //守方家族
          ,
          "族战时间" //族战时间
          ,
          "族战线路" //族战线路
  );
  strcat(NPC_sendbuf, tmpbuf);
#ifdef FAMLYNUM_CF
  for (i = 1; i <= getfamlynum(); i++) {
#else
  for (i = 1; i <= FMPOINTNUM; i++) {
#endif
    memset(tmpbuf, 0, sizeof(tmpbuf));
    fmpks_pos = i * MAX_SCHEDULE;
    char tmpbuf[256] = "";
    int fmtime = sasql_queryFmPointData(i, 0);
    int fmpkmum = sasql_queryFmPointData(i, 1);

    memset(fmtimebuff2, 0, sizeof(fmtimebuff2));
    memset(fmtimebuff, 0, sizeof(fmtimebuff));
    memset(fmnumbuff, 0, sizeof(fmnumbuff));

    if (fmtime > 0) {
      sprintf(fmtimebuff, "%d天", (NowTime.tv_sec - fmtime) / 86400);
    } else {
      sprintf(fmtimebuff, "无记录");
    }
    if (fmpkmum >= 0) {
      sprintf(fmnumbuff, "%d次", fmpkmum);
    } else {
      sprintf(fmnumbuff, "无记录");
    }
    switch (fmpks[fmpks_pos + 1].flag) {
    case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    case FMPKS_FLAG_MANOR_PREPARE:
    case FMPKS_FLAG_MANOR_OTHERPLANET: {
      time_t dueltime = (time_t)fmpks[fmpks_pos + 1].dueltime;
      struct tm *tm1 = localtime(&dueltime);
      sprintf(fmtimebuff2, "%02d.%02d %d:%d", tm1->tm_mon + 1, tm1->tm_mday,
              tm1->tm_hour, tm1->tm_min);

      if (dueltime > 0) {
        sprintf(tmpbuf, "[style c=6]%-8s　%-16s　%-16s  %-16s  %-16s[/style]\n",
                fmpointname[i - 1] //庄园名字
                ,
                fmpks[fmpks_pos + 1].guest_name //攻方家族
                ,
                fmpks[fmpks_pos + 1].host_name //守方家族
                ,
                fmtimebuff2 //族战时间
                ,
                fmpks[fmpks_pos + 2].host_name //族战线路
        );
      } else {
        sprintf(tmpbuf, "[style c=6]%-8s　%-16s　%-16s  %-16s  %-16s[/style]\n",
                fmpointname[i - 1] //庄园名字
                ,
                fmpks[fmpks_pos + 1].guest_name //攻方家族
                ,
                fmpks[fmpks_pos + 1].host_name //守方家族
                ,
                fmtimebuff2 //族战时间
                ,
                fmpks[fmpks_pos + 2].host_name //族战线路
        );
      }
    } break;
    default: {
      sprintf(tmpbuf, "[style c=0]%-8s　%-16s　%-16s  %-16s  %-16s[/style]\n",
              fmpointname[i - 1] //庄园名字
              ,
              "", "", "无排程", "");
    } break;
    }
    strcat(NPC_sendbuf, tmpbuf);
  }
  lua_pushstring(L, NPC_sendbuf);
  return 1;
}

static int ShowFamilyPkNum(lua_State *L) {
  size_t l;
  const int floorid = luaL_checkint(L, 1);
  const int fmpks_pos = luaL_checkint(L, 2) * MAX_SCHEDULE;
  char token[1024];
  int num1 = 0, num2 = 0;
  memset(token, 0, sizeof(token));
  NPC_GetPKFMNum(floorid, fmpks[fmpks_pos].host_index,
                 fmpks[fmpks_pos].guest_index, &num1, &num2);
  sprintf(token, "\n%s\n\n%s:%4d人\n\n%s:%4d人", "目前两个家族人数如下：",
          fmpks[fmpks_pos].host_name, num1, fmpks[fmpks_pos].guest_name, num2);
  lua_pushstring(L, token);
  return 1;
}

static const luaL_Reg Familylib[] = {{"ShowFamilyList", ShowFamilyList},
                                     {"ShowFamilyPkList", ShowFamilyPkList},
                                     {"ShowFamilyPkNum", ShowFamilyPkNum},
                                     {NULL, NULL}};

LUALIB_API int luaopen_Family(lua_State *L) {
  luaL_register(L, "family", Familylib);
  return 1;
}

#endif
