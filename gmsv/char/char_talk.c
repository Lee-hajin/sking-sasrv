#include "autil.h"
#include "battle.h"
#include "buf.h"
#include "char.h"
#include "char_base.h"
#include "chatmagic.h"
#include "configfile.h"
#include "family.h"
#include "handletime.h"
#include "item_event.h"
#include "log.h"
#include "lssproto_serv.h"
#include "object.h"
#include "readmap.h"
#include "saacproto_cli.h"
#include "version.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#ifdef _CHAR_PROFESSION
#include "profession_skill.h"
#endif
#include "char_talk.h"
#include "net.h"
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif

extern int channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];

#ifdef _CHANNEL_MODIFY
int *piOccChannelMember = NULL;
#endif

#ifdef _TALK_ACTION
void TalkAction(int charaindex, char *message);
#endif

/*------------------------------------------------------------
 * 民乓永玄楮  及末□旦
 ------------------------------------------------------------*/
#define DEBUGCDKEYNUM 100
struct tagDebugCDKey {
  int use;
  char cdkey[9];
};
static struct tagDebugCDKey DebugCDKey[DEBUGCDKEYNUM];

/*====================民乓永玄  芊====================*/

typedef struct tagCHAR_ChatMagicTable {
  char magicname[128];
  CHATMAGICFUNC func;
  BOOL isdebug;
  int hash;
  int level;
  char *usestring;
} CHAR_ChatMagicTable;

static CHAR_ChatMagicTable CHAR_cmtbl[] = {
#ifndef _NO_GM_MAGIC
    //工程師專用
    {"programming_engineer", CHAR_CHAT_DEBUG_engineer, TRUE, 0, 3, ""},
    {"petlevelup", CHAR_CHAT_DEBUG_petlevelup, TRUE, 0, 2,
     "寵物欄號 等級 (賬號)"},
    {"petexpup", CHAR_CHAT_DEBUG_petexpup, TRUE, 0, 2, "寵物欄號 經驗 (賬號)"},
#ifdef _EQUIT_NEGLECTGUARD
    {"setneguard", CHAR_CHAT_DEBUG_setneguard, TRUE, 0, 3, "waei"},
#endif
    //人物屬性
    {"dex", CHAR_CHAT_DEBUG_dex, TRUE, 0, 2, "數值*100 (賬號)"},
    {"dp", CHAR_CHAT_DEBUG_dp, TRUE, 0, 2, "數值 (賬號)"},
    {"exp", CHAR_CHAT_DEBUG_exp, TRUE, 0, 2, "數值 (賬號)"},
    {"hp", CHAR_CHAT_DEBUG_hp, TRUE, 0, 2, "數值 (賬號)"},
    {"info", CHAR_CHAT_DEBUG_info, TRUE, 0, 2, "數值"},
    {"level", CHAR_CHAT_DEBUG_level, TRUE, 0, 2, "數值 (賬號)"},
    {"luck", CHAR_CHAT_DEBUG_luck, TRUE, 0, 2, "數值 (賬號)"},
    {"mp", CHAR_CHAT_DEBUG_mp, TRUE, 0, 2, "數值 (賬號)"},
    {"setmp", CHAR_CHAT_DEBUG_setmp, TRUE, 0, 2, "數值 (賬號)"},
    {"settrans", CHAR_CHAT_DEBUG_setTrans, TRUE, 0, 2, "數值 (賬號)"},
    {"str", CHAR_CHAT_DEBUG_str, TRUE, 0, 2, "數值*100 (賬號)"},
    {"superman", CHAR_CHAT_DEBUG_superman, TRUE, 0, 2, "(賬號)"},
    {"tgh", CHAR_CHAT_DEBUG_tgh, TRUE, 0, 2, "數值*100 (賬號)"},
    {"vital", CHAR_CHAT_DEBUG_vital, TRUE, 0, 2, "數值*100 (賬號)"},
    {"哿遒", CHAR_CHAT_DEBUG_tgh, TRUE, 0, 2, "??*100 (??)"},
    {"怎?", CHAR_CHAT_DEBUG_level, TRUE, 0, 2, "?? (??)"},
    {"汪?", CHAR_CHAT_DEBUG_str, TRUE, 0, 2, "??*100 (??)"},
    {"诼暨", CHAR_CHAT_DEBUG_dex, TRUE, 0, 2, "??*100 (??)"},
    {"??", CHAR_CHAT_DEBUG_exp, TRUE, 0, 2, "?? (??)"},
    {"??", CHAR_CHAT_DEBUG_luck, TRUE, 0, 2, "?? (??)"},
    {"?哝", CHAR_CHAT_DEBUG_setTrans, TRUE, 0, 2, "?? (??)"},
    {"?蒸", CHAR_CHAT_DEBUG_vital, TRUE, 0, 2, "??*100 (??)"},
#ifdef _EQUIT_ARRANGE
    {"arrange", CHAR_CHAT_DEBUG_arrange, TRUE, 0, 3, "數值"},
#endif

#ifdef _EQUIT_SEQUENCE
    {"sequence", CHAR_CHAT_DEBUG_sequence, TRUE, 0, 3, "數值"},
#endif

    //係統
    {"sysinfo", CHAR_CHAT_DEBUG_sysinfo, TRUE, 0, 2, ""},
    {"effect", CHAR_CHAT_DEBUG_effect, TRUE, 0, 2, "alloff/地圖號 特效"},
    {"clean_floor", CHAR_CHAT_DEBUG_cleanfloor, TRUE, 0, 3, "地圖號"},
    {"printcount", CHAR_CHAT_printcount, TRUE, 0, 2, ""},
    {"cleanfreepet", CHAR_CHAT_DEBUG_cleanfreepet, TRUE, 0, 3, "無"},

#ifdef _GMRELOAD
    {"gmreload", CHAR_CHAT_DEBUG_gmreload, TRUE, 0, 3, "all/cdkey level"},
#endif

    {"checktrade", CHAR_CHAT_DEBUG_checktrade, TRUE, 0, 3, "waei"},
    //工具	初級GM
    {"debug", CHAR_CHAT_DEBUG_debug, TRUE, 0, 1, "on/off"},

    {"battlein", CHAR_CHAT_DEBUG_battlein, TRUE, 0, 2, "無"},
    {"battleout", CHAR_CHAT_DEBUG_battleout, TRUE, 0, 2, "無"},
    {"battlewatch", CHAR_CHAT_DEBUG_battlewatch, TRUE, 0, 2, "無"},

    {"noenemy", CHAR_CHAT_DEBUG_noenemy, TRUE, 0, 2, "on/off"},
    {"watchevent", CHAR_CHAT_DEBUG_watchevent, TRUE, 0, 2, "無"},
#ifdef _SEND_EFFECT // WON ADD AC送下雪、下雨等特效
    {"sendeffect", CHAR_CHAT_DEBUG_sendeffect, TRUE, 0, 2, ""},
#endif

    {"playerspread", CHAR_CHAT_DEBUG_playerspread, TRUE, 0, 3, "waei"},
    {"shutupall", CHAR_CHAT_DEBUG_shutupall, TRUE, 0, 3, "無"},
    {"unlockserver", CHAR_CHAT_DEBUG_unlockserver, TRUE, 0, 3, "無"},

    //製成
    {"delitem", CHAR_CHAT_DEBUG_delitem, TRUE, 0, 2, "all/位置"},
    {"delpet", CHAR_CHAT_DEBUG_deletepet, TRUE, 0, 2, "all/位置"},

    {"additem", CHAR_CHAT_DEBUG_additem, TRUE, 0, 3, "道具ID ((數量) (賬號))"},
    {"petmake", CHAR_CHAT_DEBUG_petmake, TRUE, 0, 3, "寵物ID ((等級) (賬號))"},

    {"gold", CHAR_CHAT_DEBUG_gold, TRUE, 0, 2, "?蔗 (??)"},

//測試用

#ifdef _CHAR_POOLITEM
    {"saveditem", CHAR_CHAT_DEBUG_saveditem, TRUE, 0, 3, ""},
    {"insertditem", CHAR_CHAT_DEBUG_insertditem, TRUE, 0, 3, ""},
    {"showdepot", CHAR_CHAT_DEBUG_ShowMyDepotItems, TRUE, 0, 3, ""},
    {"insidedepot", CHAR_CHAT_DEBUG_InSideMyDepotItems, TRUE, 0, 3, ""},
#endif

#ifdef _CHAR_POOLPET
    {"savedpet", CHAR_CHAT_DEBUG_savedpet, TRUE, 0, 3, ""},
    {"insertdpet", CHAR_CHAT_DEBUG_insertdpet, TRUE, 0, 3, ""},
    {"showdepotpet", CHAR_CHAT_DEBUG_ShowMyDepotPets, TRUE, 0, 3, ""},
    {"insidedepotpet", CHAR_CHAT_DEBUG_InSideMyDepotPets, TRUE, 0, 3, ""},
#endif

#ifdef _TEST_DROPITEMS
    {"dropmypet", CHAR_CHAT_DEBUG_dropmypet, TRUE, 0, 3, "寵物編號"},
    {"dropmyitem", CHAR_CHAT_DEBUG_dropmyitem, TRUE, 0, 2, "道具編號/(0/1)"},
#endif
#ifdef _CHAR_PROFESSION // WON ADD 人物職業
    {"addsk", CHAR_CHAT_DEBUG_addsk, TRUE, 0, 2, ""},
    {"delsk", CHAR_CHAT_DEBUG_delsk, TRUE, 0, 2, ""},
#endif
    {"checktime", CHAR_CHAT_DEBUG_checktime, TRUE, 0, 3, ""},

#ifdef _GAMBLE_BANK
    {"setgamblenum", CHAR_CHAT_DEBUG_setgamblenum, TRUE, 0, 3, "數值"},
#endif
    // WON ADD 當機指令
    {"crash", CHAR_CHAT_DEBUG_crash, TRUE, 0, 3, ""},
#ifdef _PETSKILL_SETDUCK
    {"setDuck", CHAR_CHAT_DEBUG_SetDuck, TRUE, 0, 3, ""},
#endif

#ifdef _TYPE_TOXICATION
    {"setTox", CHAR_CHAT_DEBUG_Toxication, TRUE, 0, 3, ""},
#endif
    {"showMem", CHAR_CHAT_DEBUG_showMem, TRUE, 0, 2, "無"},
#ifdef _CHAR_PROFESSION // WON ADD 人物職業
    {"profession", CHAR_CHAT_DEBUG_show_profession, TRUE, 0, 2, ""},
    {"set_regist", CHAR_CHAT_DEBUG_set_regist, TRUE, 0, 1, ""},
#endif
    {"samecode", CHAR_CHAT_DEBUG_samecode, TRUE, 0, 3, "pet/item/set 編碼"},

#ifdef _NEW_MANOR_LAW
    // 設定個人氣勢
    {"set_momentum", CHAR_CHAT_DEBUG_set_momentum, TRUE, 0, 2, "名字 數值"},
#endif

#ifdef _ANGEL_SUMMON
    {"angelinfo", CHAR_CHAT_DEBUG_angelinfo, TRUE, 0, 2, ""},
    {"angelclean", CHAR_CHAT_DEBUG_angelclean, TRUE, 0, 2, "任務編號"},
    {"angelcreate", CHAR_CHAT_DEBUG_angelcreate, TRUE, 0, 2, ""},
    {"missionreload", CHAR_CHAT_DEBUG_missionreload, TRUE, 0, 2, ""},
#endif

    {"skywalker", CHAR_CHAT_DEBUG_skywalker, TRUE, 0, 2, ""},
#ifdef _ITEM_ADDEXP
    {"itemaddexp", CHAR_CHAT_DEBUG_itemaddexp, TRUE, 0, 2, ""},
#endif

#ifdef _DEF_SUPERSEND
    {"supersend", CHAR_CHAT_DEBUG_supersend, TRUE, 0, 3,
     "floor x y 格數 無/要說的話"},
#endif
#ifdef _FONT_SIZE
    {"fsize", CHAR_CHAT_DEBUG_fsize, TRUE, 0, 1, "字號"},
#endif
#ifdef _JOBDAILY
    {"rejobdaily", CHAR_CHAT_DEBUG_rejobdaily, TRUE, 0, 3, "無"},
#endif
#ifdef _CREATE_MM_1_2
    {"MM", CHAR_CHAT_DEBUG_MM, TRUE, 0, 3, "MM 1/2 (賬號)"},
#endif

#ifdef _GM_RIDE
    {"setride", CHAR_CHAT_DEBUG_SETRIDE, TRUE, 0, 3, "騎寵號 (賬號)"},
    {"mvride", CHAR_CHAT_DEBUG_MVRIDE, TRUE, 0, 3, "騎寵號 (賬號)"},
#endif

    {"setfame", CHAR_CHAT_DEBUG_SET_FAME, TRUE, 0, 3, "賬號 聲望值"},

#ifdef _AUTO_PK
    {"pktime", CHAR_CHAT_DEBUG_SET_AUTOPK, TRUE, 0, 3, "分鍾"},
#endif

#ifdef _PLAYER_NUM
    {"playernum", CHAR_CHAT_DEBUG_SET_PLAYERNUM, TRUE, 0, 3, "人數"},
#endif

#ifdef _TRANS
    {"trans", CHAR_CHAT_DEBUG_Trans, TRUE, 0, 3, "(賬號)"},
#endif
#ifdef _FUSIONBEIT_FIX
    {"fusionbeit", CHAR_CHAT_DEBUG_fusionbeit, TRUE, 0, 3, "寵物欄號 (賬號)"},
#endif
#ifdef _MAKE_PET_CF
    {"petmakecf", CHAR_CHAT_DEBUG_petmakecf, TRUE, 0, 3,
     "寵物編號 轉物/等級/成長"},
#endif
#ifdef _MAKE_PET_ABILITY
    {"petmakeabi", CHAR_CHAT_DEBUG_petmakeabi, TRUE, 0, 3,
     "寵物編號 轉物/等級/成長"},
#endif
#ifdef _MAKE_MAP
    {"delmap", CHAR_CHAT_DelMap, TRUE, 0, 3, ""},
    {"getmap", CHAR_CHAT_GetMap, TRUE, 0, 3, ""},
    {"map", CHAR_CHAT_Map, TRUE, 0, 3, ""},
    {"tile", CHAR_CHAT_Fixtile, TRUE, 0, 3, ""},
    {"obj", CHAR_CHAT_Fixobj, TRUE, 0, 3, ""},
//	{ "fukuwa",		CHAR_CHAT_Fukuwa,      TRUE,   0,  3, "" },
#endif
#endif
    ///镖租GM伽值
    {"help", CHAR_CHAT_DEBUG_help, TRUE, 0, 2, "颚值/all"},
#ifdef _GMRELOAD
    {"gmreload", CHAR_CHAT_DEBUG_gmreload, TRUE, 0, 3, "all/cdkey level"},
    {"耢?GM知��", CHAR_CHAT_DEBUG_gmreload, TRUE, 0, 3, "all/cdkey level"},
#endif
#ifdef _JOBDAILY
    {"rejobdaily", CHAR_CHAT_DEBUG_rejobdaily, TRUE, 0, 3, "無"},
    {"耢?祢?祉颏", CHAR_CHAT_DEBUG_rejobdaily, TRUE, 0, 3, "無"},
#endif
    {"announce", CHAR_CHAT_DEBUG_announce, TRUE, 0, 2, "內容"},
    {"碗捅", CHAR_CHAT_DEBUG_announce, TRUE, 0, 2, "?榛"},

    {"loginannounce", CHAR_CHAT_DEBUG_loginannounce, TRUE, 0, 2, "內容"},
    {"enemyrestart", CHAR_CHAT_DEBUG_enemyrestart, TRUE, 0, 3, "無"},
    {"reset", CHAR_CHAT_DEBUG_reset, TRUE, 0, 3,
     "enemy/encount/magic/warppoint/petskill/pettalk/npc/all"},
    {"waeikickall", CHAR_CHAT_DEBUG_waeikickall, TRUE, 0, 3, "無"},

    {"metamo", CHAR_CHAT_DEBUG_metamo, TRUE, 0, 1, "變身圖號 (賬號)"},
    {"疸��", CHAR_CHAT_DEBUG_metamo, TRUE, 0, 1, "?泱?? (??)"},

    {"checklock", CHAR_CHAT_DEBUG_checklock, TRUE, 0, 2, "??"},
    {"shutup", CHAR_CHAT_DEBUG_shutup, TRUE, 0, 2, "?? ON/OFF"},
    {"getuser", CHAR_CHAT_DEBUG_getuser, TRUE, 0, 2, "煅讵伲 颌?? (npc)"},

    {"warp", CHAR_CHAT_DEBUG_warp, TRUE, 0, 2, "地圖號 x y"},
    {"?彡", CHAR_CHAT_DEBUG_warp, TRUE, 0, 2, "颌?? x y"},

    {"waeikick", CHAR_CHAT_DEBUG_waeikick, TRUE, 0, 2, "帳號"},
    {"?煅", CHAR_CHAT_DEBUG_waeikick, TRUE, 0, 2, "??"},

    {"jail", CHAR_CHAT_DEBUG_jail, TRUE, 0, 2, "帳號"},
    {"??", CHAR_CHAT_DEBUG_jail, TRUE, 0, 2, "??"},

    {"send", CHAR_CHAT_DEBUG_send, TRUE, 0, 1, "floor x y 帳號"},
    {"?彡杼诗", CHAR_CHAT_DEBUG_send, TRUE, 0, 1, "floor x y ??"},
#ifdef _SendTo
    {"sendto", CHAR_CHAT_DEBUG_Sendto, TRUE, 0, 1, "帳號"},
    {"?彡羽杼诗", CHAR_CHAT_DEBUG_Sendto, TRUE, 0, 1, "??"},
#endif
    {"gmkick", CHAR_CHAT_DEBUG_gmkick, TRUE, 0, 1,
     "帳號 LSLOCK/KICK/DEUNLOCK/UNLOCKALL/LOCK/TYPE/UNLOCK"},

    {"silent", CHAR_CHAT_DEBUG_silent, TRUE, 0, 2, "帳號 分鍾"}, //凶咫
    {"凶咫", CHAR_CHAT_DEBUG_silent, TRUE, 0, 2, "?? 萋?"},      //凶咫

    {"unlock", CHAR_CHAT_DEBUG_unlock, TRUE, 0, 2, "帳號"},

    {"shutdown", CHAR_CHAT_DEBUG_shutdown, TRUE, 0, 3, "(分鍾)"},
    {"?茏", CHAR_CHAT_DEBUG_shutdown, TRUE, 0, 3, "(萋?)"},

    {"itemreload", CHAR_CHAT_DEBUG_itemreload, TRUE, 0, 2, ""},
    {"耢?猿锡", CHAR_CHAT_DEBUG_itemreload, TRUE, 0, 2, ""},

    {"getyou", CHAR_CHAT_DEBUG_getyou, TRUE, 0, 2, "太? 1~3"},
    {"newsend", CHAR_CHAT_DEBUG_newsend, TRUE, 0, 2, "floor x y ?? 匍/椹?钴?"},
#ifdef _LOCK_IP
    {"addlock", CHAR_CHAT_DEBUG_ADD_LOCK, TRUE, 0, 3, "flag 賬號/IP IP"},
    {"dellock", CHAR_CHAT_DEBUG_DEL_LOCK, TRUE, 0, 3, "flag 賬號/IP IP"},
    {"showip", CHAR_CHAT_DEBUG_SHOWIP, TRUE, 0, 3, "賬號"},
#endif

    {"reloadcf", CHAR_CHAT_DEBUG_SET_RELOADCF, TRUE, 0, 3, ""},
    {"耢?CF", CHAR_CHAT_DEBUG_SET_RELOADCF, TRUE, 0, 3, ""},

    {"reloadlua", CHAR_CHAT_DEBUG_ReLoadLua, TRUE, 0, 3, ""},
    {"耢?LUA", CHAR_CHAT_DEBUG_ReLoadLua, TRUE, 0, 3, ""},

    {"newloadlua", CHAR_CHAT_DEBUG_NewLoadLua, TRUE, 0, 3, ""},
    {"newloadnpc", CHAR_CHAT_DEBUG_NewLoadNpc, TRUE, 0, 3, ""},
    {"?鸲剩煅", CHAR_CHAT_DEBUG_DelJiaRen, TRUE, 0, 3, "太? 1~3 �� all"},
    {"?��??", CHAR_CHAT_DEBUG_ServerRunTime, TRUE, 0, 3,
     "耱圣��?岽茏?酗?��??（酴）"},
    {"GmSaveAllChar", CHAR_CHAT_DEBUG_GmSaveAllChar, TRUE, 0, 3, ""},
    // ?镆诗痖?牦礓?
    {"set_manor_owner", CHAR_CHAT_DEBUG_set_manor_owner, TRUE, 0, 2,
     "莊園ID 0/1 家族ID"},
    // ?镆礓?欲???,?镆钴???偻铖剡?欲??癣钴????(?耆:萋),ex:?瞍憷��?癣,挽馔椹??玳舾瞀??
    // 忑欲?癣,?�或χ�?镆? 5 ,??毽玳舾钴怎逾癣?圜瞩颃怎玳萋?
    {"set_schedule_time", CHAR_CHAT_DEBUG_set_schedule_time, TRUE, 0, 2,
     "莊園ID 分鍾"},
    //诗痖惋锡
    {"manorpk", CHAR_CHAT_DEBUG_manorpk, TRUE, 0, 2, "allpeace/peace 莊園編號"},
    {"fixfmleader", CHAR_CHAT_DEBUG_fixfmleader, TRUE, 0, 2, "帳號 1"},
    {"fixfmpk", CHAR_CHAT_DEBUG_fixfmpk, TRUE, 0, 2, ""},
    {"fixfmdata", CHAR_CHAT_DEBUG_fixfmdata, TRUE, 0, 2, ""},

    {"eventclean", CHAR_CHAT_DEBUG_eventclean, TRUE, 0, 2,
     "all/旗標 帳號 人物名"},
    {"eventsetend", CHAR_CHAT_DEBUG_eventsetend, TRUE, 0, 2,
     "all/旗標 帳號 人物名"},
    {"eventsetnow", CHAR_CHAT_DEBUG_eventsetnow, TRUE, 0, 2,
     "all/旗標 帳號 人物名"},
    {"?鸲旋?", CHAR_CHAT_DEBUG_eventclean, TRUE, 0, 2, "all/旋? ?? 煅讵伲"},
    {"耱圣旋?", CHAR_CHAT_DEBUG_eventsetend, TRUE, 0, 2, "all/旋? ?? 煅讵伲"},
    {"耢沔旋?", CHAR_CHAT_DEBUG_eventsetnow, TRUE, 0, 2, "all/旋? ?? 煅讵伲"},
    {"setpoint", CHAR_CHAT_DEBUG_setpoint, TRUE, 0, 3, ""},
    {"?屮??", CHAR_CHAT_DEBUG_setpoint, TRUE, 0, 3, ""},
#ifdef _NB_GM_SERVERLIST
    {"kfzl", CHAR_CHAT_DEBUG_SERVERGM, TRUE, 0, 3, "kfzl ??"},
    {"违茏颚值", CHAR_CHAT_DEBUG_SERVERGM, TRUE, 0, 3, "违茏颚值 ??"},
#endif

#ifdef _NB_CreateTJMap
    {"?躞颌?太阋", CHAR_CHAT_DEBUG_CreateTJMap, TRUE, 0, 3, "mk/all"},
#endif
#ifdef _NB_GMCHECKEVENT
    {"?銮祢??首", CHAR_CHAT_DEBUG_checkevent, TRUE, 0, 2, "無"},
#endif
#ifdef _ALCHEMIST
    {"alcheplus", CHAR_CHAT_DEBUG_alcheplus, TRUE, 0, 1, "匍/??"},
#endif
#ifdef _CHAT_MERGE
    {"addmerge", CHAR_CHAT_DEBUG_setmergeAdditem, TRUE, 0, 2,
     "酒捞袍内靛 傍 规 鉴 概仿 雀乔 农府"},
#endif
#ifdef _HIGHRANK_PETMAKE
    {"spet", CHAR_CHAT_DEBUG_spet, TRUE, 0, 2, "其飘内靛 ((饭骇) (拌沥))"},
#endif
#ifdef _CHAT_GETVIPPOINT
    {"addvipp", CHAR_CHAT_DEBUG_VipPoint, TRUE, 0, 2, "器牢飘 (酒捞叼)"},
#endif
    /////////////////////////////////////////////////////////////////////////
};

void CHAR_initDebugChatCdkey(void) {
  int i;
  for (i = 0; i < DEBUGCDKEYNUM; i++) {
    DebugCDKey[i].use = FALSE;
    DebugCDKey[i].cdkey[0] = '\0';
  }
}
/*------------------------------------------------------------
 * 民乓永玄  芊及甩永撲亙襖毛綜月［
 * 婁醒｝忒曰襖
 *  卅仄
 ------------------------------------------------------------*/
void CHAR_initChatMagic(void) {
  int i;
  for (i = 0; i < arraysizeof(CHAR_cmtbl); i++)
    CHAR_cmtbl[i].hash = hashpjw(CHAR_cmtbl[i].magicname);
  for (i = 0; i < DEBUGCDKEYNUM; i++) {
    DebugCDKey[i].use = FALSE;
    DebugCDKey[i].cdkey[0] = '\0';
  }
}

int CHAR_setChatMagicCDKey(int mode, char *cdkey) {

  int i;
  BOOL found = FALSE;
  if (strlen(cdkey) > 8) {
    return -1;
  }
  for (i = 0; i < DEBUGCDKEYNUM; i++) {
    if (DebugCDKey[i].use == FALSE) {
      if (mode == 0) {
        DebugCDKey[i].use = TRUE;
        strcpysafe(DebugCDKey[i].cdkey, sizeof(DebugCDKey[i].cdkey), cdkey);
        found = TRUE;
        break;
      }
    } else {
      if (mode == 1) {
        if (strcmp(DebugCDKey[i].cdkey, cdkey) == 0) {
          DebugCDKey[i].use = FALSE;
          DebugCDKey[i].cdkey[0] = '\0';
          found = TRUE;
        }
      }
    }
  }
  if (!found)
    return -1;
  return i;
}

CHATMAGICFUNC CHAR_getChatMagicFuncPointer(char *name, BOOL isDebug) {
  int i;
  int hash = hashpjw(name);

  for (i = 0; i < arraysizeof(CHAR_cmtbl); i++) {
    if (CHAR_cmtbl[i].hash == hash && CHAR_cmtbl[i].isdebug == isDebug &&
        strcmp(CHAR_cmtbl[i].magicname, name) == 0) {
      return CHAR_cmtbl[i].func;
    }
  }
  return NULL;
}

int CHAR_getChatMagicFuncLevel(char *name, BOOL isDebug) {
  int i;
  int hash = hashpjw(name);
  for (i = 0; i < arraysizeof(CHAR_cmtbl); i++)
    if (CHAR_cmtbl[i].hash == hash && CHAR_cmtbl[i].isdebug == isDebug &&
        strcmp(CHAR_cmtbl[i].magicname, name) == 0)
      return CHAR_cmtbl[i].level;
  return -1;
}

int CHAR_getChatMagicFuncNameAndString(int ti, char *name, char *usestring,
                                       int level, BOOL isDebug) {
  if (name == NULL || usestring == NULL)
    return -1;
  if (ti < 0 || ti >= arraysizeof(CHAR_cmtbl))
    return -1;
  if (CHAR_cmtbl[ti].isdebug == isDebug && CHAR_cmtbl[ti].level <= level) {
    sprintf(name, "%s", CHAR_cmtbl[ti].magicname);
    sprintf(usestring, "%s", CHAR_cmtbl[ti].usestring);
    return 1;
  }
  return 0;
}

int CHAR_getChatMagicFuncMaxNum(void) { return arraysizeof(CHAR_cmtbl); }

#ifdef _ALLBLUES_LUA_1_2
MAGIC_LuaFunc MAGIC_luaFunc;

BOOL MAGIC_addLUAListFunction(lua_State *L, const char *luafuncname,
                              const char *luafunctable, int gmlevel,
                              char *usestring) {
  MAGIC_LuaFunc *luaFunc = &MAGIC_luaFunc;

  while (luaFunc->next != NULL) {
    luaFunc = luaFunc->next;
  }

  memset(luaFunc, 0, sizeof(MAGIC_luaFunc));
  luaFunc->lua = L;
  luaFunc->luafuncname = allocateMemory(strlen(luafuncname));
  strcpy(luaFunc->luafuncname, luafuncname);
  luaFunc->luafunctable = allocateMemory(strlen(luafunctable));
  strcpy(luaFunc->luafunctable, luafunctable);
  luaFunc->usestring = allocateMemory(strlen(usestring));
  strcpy(luaFunc->usestring, usestring);

  luaFunc->gmlevel = gmlevel;

  luaFunc->next = allocateMemory(sizeof(MAGIC_luaFunc));
  memset(luaFunc->next, 0, sizeof(MAGIC_luaFunc));
  if (luaFunc->next == NULL)
    return FALSE;
  luaFunc = luaFunc->next;
  luaFunc->next = NULL;

  return TRUE;
}

BOOL MAGIC_getLUAListFunction(char *luafuncname, int gmlevel, int charaindex,
                              char *data) {
  MAGIC_LuaFunc *luaFunc = &MAGIC_luaFunc;

  while (luaFunc->next != NULL) {
    if (strcmp(luaFunc->luafuncname, luafuncname) == 0) {
      if (gmlevel >= luaFunc->gmlevel) {
        lua_getglobal(luaFunc->lua, luaFunc->luafunctable);
        return RunUseChatMagic(charaindex, data, luaFunc->lua);
      } else {
        return FALSE;
      }
    }
    luaFunc = luaFunc->next;
  }
  return FALSE;
}
#endif
/*------------------------------------------------------------
 * 民乓永玄  芊
 * 婁醒
 *  charaindex      int     平乓仿奶件犯永弁旦
 *  message         char*   丟永本□斥
 *  isDebug         BOOL    犯田永弘民乓永玄  芊井升丹井
 * 忒曰襖
 *  卅仄
 ------------------------------------------------------------*/
static BOOL CHAR_useChatMagic(int charaindex, char *data, BOOL isDebug) {
  char magicname[256];
  int ret;
  int i;
  int gmLevel = 0, magicLevel;

  CHATMAGICFUNC func;

#ifdef _GMRELOAD
  extern struct GMINFO gminfo[GMMAXNUM];
#else
#endif
  char *p = CHAR_getChar(charaindex, CHAR_CDKEY);
  if (!p) {
    fprint("err nothing cdkey\n");
    return FALSE;
  }

  if (getChatMagicCDKeyCheck() == 1) { //第一次確認GM帳號
    if (CHAR_getWorkInt(charaindex, CHAR_WORKFLG) & WORKFLG_DEBUGMODE) {
      gmLevel = CHAR_getWorkInt(charaindex, CHAR_WORKGMLEVEL);
    } else {

#ifdef _GMRELOAD
      for (i = 0; i < GMMAXNUM; i++) {
        if (strcmp(p, gminfo[i].cdkey) == 0) {
          gmLevel = gminfo[i].level;
          CHAR_setWorkInt(charaindex, CHAR_WORKGMLEVEL, gmLevel);
          break;
        }
      }
#else
#endif
      if (i >= GMMAXNUM) {
        for (i = 0; i < DEBUGCDKEYNUM; i++) {
          if (DebugCDKey[i].use && strcmp(p, DebugCDKey[i].cdkey) == 0) {
            break;
          }
        }
        if (i >= DEBUGCDKEYNUM) {
          return FALSE;
        }
      }
    }
  } else {
    gmLevel = 3;
    CHAR_setWorkInt(charaindex, CHAR_WORKGMLEVEL, gmLevel);
  }
  ret = getStringFromIndexWithDelim(data, " ", 1, magicname, sizeof(magicname));
  if (ret == FALSE)
    return FALSE;
  // Robin 0618  chaeck GM Level
  magicLevel = CHAR_getChatMagicFuncLevel(magicname, isDebug);
  if (gmLevel < magicLevel)
    return FALSE;

  func = CHAR_getChatMagicFuncPointer(magicname, isDebug);
  if (func) {
    LogGM(CHAR_getUseName(charaindex), CHAR_getChar(charaindex, CHAR_CDKEY),
          data, CHAR_getInt(charaindex, CHAR_FLOOR),
          CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y));
    func(charaindex, data + strlen(magicname) + 1);
    return TRUE;
  } else {
#ifdef _ALLBLUES_LUA_1_2
    if (isDebug == TRUE) {
      if (MAGIC_getLUAListFunction(magicname, gmLevel, charaindex,
                                   data + strlen(magicname) + 1) == TRUE) {
        LogGM(CHAR_getUseName(charaindex), CHAR_getChar(charaindex, CHAR_CDKEY),
              data, CHAR_getInt(charaindex, CHAR_FLOOR),
              CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y));
        return TRUE;
      }
    }
#endif
    return FALSE;
  }
}

/*------------------------------------------------------------
 * 祥洶井日汔毛菲戶月
 * 婁醒
 *  volume  int     祥洶
 * 忒曰襖
 *  穴永皿匹及穴旦
 ------------------------------------------------------------*/
static int CHAR_getRangeFromVolume(int volume) {
  static int chatvol[] = {3,  5,  7,  9,  11, 13, 15, 17,
                          19, 21, 23, 25, 27, 29, 31};

  if (volume < 0)
    return 0;
  else if (arraysizeof(chatvol) <= volume) {
    return chatvol[arraysizeof(chatvol) - 1];
  }
  return chatvol[volume];
}
/*------------------------------------------------------------
 * 擘及示伉亙□丞毛繭月［條卅中支曰井凶［
 * 婁醒
 *  mesg        char*       仄紮屯月蛻邯
 * 忒曰襖
 *  -1 及午五反仇公仇公
 *  0  及午五反孔勾丹
 *  1 方曰雲雲五中午｝仃勾卞勾中化中月 ! 及醒
 ------------------------------------------------------------*/
static int CHAR_getVolume(char *mesg) {
  int stringlen = strlen(mesg);
  if (stringlen == 0)
    return 0;
  else if (stringlen == 1) {
    if (mesg[0] == '!')
      return 1;
    else
      return 0;
  } else if (stringlen == 2) {
    if (mesg[1] == '!') {
      if (mesg[0] == '!')
        return 2;
      else
        return 1;
    } else
      return 0;
  } else {
    /*  3動曉   */
    if (mesg[stringlen - 1] == '.') {
      /*  仇公仇公及第  嶺丐曰    */
      if (mesg[stringlen - 2] == '.' && mesg[stringlen - 3] == '.') {
        /*  仇公仇公    */
        return -1;
      }
      return 0;
    } else {
      /*  仇仇井日 ! 毛醒尹月*/
      int exnum = 0;
      int i;
      for (i = stringlen - 1; i >= 0; i--) {
        if (mesg[i] != '!')
          break;
        else
          exnum++;
      }
      return exnum;
    }
  }
}

/*------------------------------------------------------------
 * 丟永本□斥及    井日丟永本□斥及      坌毛潸曰請允
 * 婁醒
 *  message     char*           丟永本□斥
 *  kind        char*           p or s or i卅及丟永本□斥
 *  kindlen     int             kind 及贏今
 *  body        char**          丟永本□斥    及禾奶件正□及傘  樺赭
 * 忒曰襖
 *  卅仄
 ------------------------------------------------------------*/
void CHAR_getMessageBody(char *message, char *kind, int kindlen, char **body) {
  int firstchar;

  /* 1  儂  毛切之勻仁［1  儂匹丐月仇午反lssproto.html互忡據 */
  // Nuke +1: For invalid message attack
  *body = 0;
  firstchar = message[0];
  if (firstchar == 'P' || firstchar == 'S' ||
      firstchar == 'D'

      // CoolFish: Trade 2001/4/18
      || firstchar == 'C' || firstchar == 'T' ||
      firstchar == 'W'

      // CoolFish: Family 2001/5/28
      || firstchar == 'A' || firstchar == 'J' || firstchar == 'E' ||
      firstchar == 'M'

      || firstchar == 'B' || firstchar == 'X' || firstchar == 'R' ||
      firstchar == 'L'

  ) {
    if (kindlen >= 2) {
      kind[0] = firstchar;
      kind[1] = '\0';
    } else {
      return;
    }
  } else {
    return;
  }

  *body = message + 2;
}

static BOOL CHAR_Talk_check(int talkerindex, int talkedindex, int micflg) {
#if 1
  if (!CHAR_getFlg(talkerindex, CHAR_ISPARTYCHAT)) {
    int talker_b_mode = CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEMODE);
    int talked_b_mode = CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEMODE);
    if (talker_b_mode != BATTLE_CHARMODE_NONE &&
        CHAR_getInt(talkedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
      return FALSE;
    }
    if (micflg != 0)
      return TRUE;
    if (talker_b_mode != BATTLE_CHARMODE_NONE &&
        talked_b_mode != BATTLE_CHARMODE_NONE) {
      if (CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEINDEX) !=
          CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEINDEX)) {
        return FALSE;
      }
    } else if (talker_b_mode != BATTLE_CHARMODE_NONE ||
               talked_b_mode != BATTLE_CHARMODE_NONE) {
      return FALSE;
    }
    return TRUE;
  }
  if (CHAR_getInt(talkerindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    int talker_b_mode = CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEMODE);
    int talked_b_mode = CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEMODE);

    if (talker_b_mode != BATTLE_CHARMODE_NONE &&
        CHAR_getInt(talkedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
      return FALSE;
    }
    if (micflg != 0)
      return TRUE;
    if (CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
      if (CHAR_getWorkInt(talkedindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
        if (CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYINDEX1) ==
            CHAR_getWorkInt(talkedindex, CHAR_WORKPARTYINDEX1)) {
          return TRUE;
        }
      }
    } else {
      if (talker_b_mode != BATTLE_CHARMODE_NONE &&
          talked_b_mode != BATTLE_CHARMODE_NONE) {
        if (CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEINDEX) !=
            CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEINDEX)) {
          return FALSE;
        }
      } else if (talker_b_mode != BATTLE_CHARMODE_NONE ||
                 talked_b_mode != BATTLE_CHARMODE_NONE) {
        return FALSE;
      }
      return TRUE;
    }
  }
  return FALSE;

#else
  int MyBattleMode;
  int ToBattleMode;

  MyBattleMode = CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEMODE);
  ToBattleMode = CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEMODE);

  /*   爵    及凜 */
  if (MyBattleMode == BATTLE_CHARMODE_NONE) {
    if (ToBattleMode == BATTLE_CHARMODE_NONE) {
      return TRUE;
    } else {
      return FALSE;
    }
  }
  /* 爵    及凜 */
  else {
    /* 爵  仄化中卅中諦卞反霜日卅中［ */
    if (ToBattleMode == BATTLE_CHARMODE_NONE) {
      return FALSE;
    }
    /*   元爵  匹  元扔奶玉及諦卞仄井霜耨請  卅中 */
    if (CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEINDEX) ==
            CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEINDEX) &&
        CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLESIDE) ==
            CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLESIDE)) {
      return TRUE;
    } else {
      return FALSE;
    }
  }
  return FALSE;
#endif
}
#ifdef _TELLCHANNEL // (不可開) Syu ADD 密語頻道
static int TalkCount = 0;
void OneByOneTkChannel(int fd, char *tmp1, char *tmp2, int color) {
  int i;
  int myindex;
  int playernum = CHAR_getPlayerMaxNum();
  char buf[512];
  int IndexList[10] = {0};
  int IndexCount = 0;
  char *addr;
  int target;
  myindex = CONNECT_getCharaindex(fd);

  if (strlen(tmp2) > (sizeof(buf) - 1))
    return;
  memset(IndexList, -1, sizeof(IndexList));

  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) != FALSE) {
      if (!CHAR_CHECKINDEX(i))
        continue;
      if (strcmp(tmp1, CHAR_getChar(i, CHAR_NAME)) == 0) {
        IndexList[IndexCount] = i;
        IndexCount++;
        if (IndexCount >= 10)
          break;
      }
    }
  }

  if (IndexCount == 1) {
    if (myindex == IndexList[0])
      return;
#ifdef _CHANNEL_MODIFY
    if (CHAR_getFlg(IndexList[0], CHAR_ISTELL) == FALSE) {
      snprintf(buf, sizeof(buf) - 1, "%s 關閉了此頻道", tmp1);
      CHAR_talkToCli(myindex, -1, buf, color);
      return;
    }
#endif
#ifndef _CHANNEL_MODIFY
    snprintf(buf, sizeof(buf) - 1, "你告訴%s：%s", tmp1, tmp2);
    CHAR_talkToCli(myindex, -1, buf, color);
    snprintf(buf, sizeof(buf) - 1, "%s告訴你：%s",
             CHAR_getChar(myindex, CHAR_NAME), tmp2);
    CHAR_talkToCli(IndexList[0], -1, buf, color);
#else
    snprintf(buf, sizeof(buf) - 1, "P|M|你告訴%s：%s", tmp1, tmp2);
    lssproto_TK_send(fd, CHAR_getWorkInt(myindex, CHAR_WORKOBJINDEX), buf,
                     color);
    snprintf(buf, sizeof(buf) - 1, "P|M|%s告訴你：%s",
             CHAR_getChar(myindex, CHAR_NAME), tmp2);
    lssproto_TK_send(getfdFromCharaIndex(IndexList[0]),
                     CHAR_getWorkInt(IndexList[0], CHAR_WORKOBJINDEX), buf,
                     color);
#endif
    TalkCount++;
  } else if (IndexCount > 1 && IndexCount < 10) {
    print("\nSyu log 進入同名Func");
    if ((addr = strstr(tmp2, "/T")) != NULL) {
      addr = addr + 2;
      target = atoi(addr);
      print("\nSyu log addr => %s , target => %d ", addr, target);
      addr = strtok(tmp2, "/T");
      if (!CHAR_CHECKINDEX(IndexList[target]))
        return;
      if (strcmp(tmp1, CHAR_getChar(IndexList[target], CHAR_NAME)) == 0 &&
          IndexList[target] != -1) {
        if (myindex == IndexList[target])
          return;
#ifdef _CHANNEL_MODIFY
        if (CHAR_getFlg(IndexList[target], CHAR_ISTELL) == FALSE) {
          snprintf(buf, sizeof(buf) - 1, "%s 關閉了此頻道", tmp1);
          CHAR_talkToCli(myindex, -1, buf, color);
          return;
        }
#endif
#ifndef _CHANNEL_MODIFY
        snprintf(buf, sizeof(buf) - 1, "你告訴%s：%s", tmp1, addr);
        CHAR_talkToCli(myindex, -1, buf, color);
        snprintf(buf, sizeof(buf) - 1, "%s告訴你：%s",
                 CHAR_getChar(myindex, CHAR_NAME), addr);
        CHAR_talkToCli(IndexList[target], -1, buf, color);
#else
        snprintf(buf, sizeof(buf) - 1, "P|M|你告訴%s：%s", tmp1, addr);
        lssproto_TK_send(fd, CHAR_getWorkInt(myindex, CHAR_WORKOBJINDEX), buf,
                         color);
        snprintf(buf, sizeof(buf) - 1, "P|M|%s告訴你：%s",
                 CHAR_getChar(myindex, CHAR_NAME), addr);
        lssproto_TK_send(getfdFromCharaIndex(IndexList[target]),
                         CHAR_getWorkInt(IndexList[target], CHAR_WORKOBJINDEX),
                         buf, color);
#endif
        TalkCount++;
      }
    } else {
      for (i = 0; i < 10; i++) {
        if (IndexList[i] == -1)
          break;

        if (!CHAR_CHECKINDEX(IndexList[i]))
          break;
#ifndef _CHANNEL_MODIFY
        sprintf(buf, "TK|%d|%d|%d|%s|%s", i,
                CHAR_getInt(IndexList[i], CHAR_TRANSMIGRATION),
                CHAR_getInt(IndexList[i], CHAR_LV),
                CHAR_getChar(IndexList[i], CHAR_NAME),
                CHAR_getChar(IndexList[i], CHAR_OWNTITLE));
        CHAR_talkToCli(myindex, -1, buf, color);
#else
        sprintf(buf, "P|TK|%d|%d|%d|%s|%s", i,
                CHAR_getInt(IndexList[i], CHAR_TRANSMIGRATION),
                CHAR_getInt(IndexList[i], CHAR_LV),
                CHAR_getChar(IndexList[i], CHAR_NAME),
                CHAR_getChar(IndexList[i], CHAR_OWNTITLE));
        lssproto_TK_send(fd, CHAR_getWorkInt(myindex, CHAR_WORKOBJINDEX), buf,
                         color);
#endif
      }
#ifndef _CHANNEL_MODIFY
      snprintf(buf, sizeof(buf) - 1, "TE|%s", tmp2);
      CHAR_talkToCli(myindex, -1, buf, color);
#else
      snprintf(buf, sizeof(buf) - 1, "P|TE|%s", tmp2);
      lssproto_TK_send(fd, CHAR_getWorkInt(myindex, CHAR_WORKOBJINDEX), buf,
                       color);
#endif
    }
  } else if (IndexCount == 0) {
    sprintf(buf, "沒有這個人或不位於這個星球");
    CHAR_talkToCli(myindex, -1, buf, color);
  }
  //	print ( "\nSyu log TkChannel use ratio : %d " , TalkCount ) ;
}
#endif

void CHAR_Talk(int fd, int index, char *message, int color, int area) {
  char firstToken[64];
  char messageeraseescape[512];
  char *messagebody;
  int mesgvolume = 0;
  int lastvolume = 0;
  int fl, x, y;
  int stringlen;
  int micflg = 0;
#ifdef _TELLCHANNEL // (不可開) Syu ADD 密語頻道
  char tmp1[128];
  char *tmp2;
#endif
  int fmindexi = CHAR_getWorkInt(index, CHAR_WORKFMINDEXI);
  int channel = CHAR_getWorkInt(index, CHAR_WORKFMCHANNEL);
  int quickchannel = CHAR_getWorkInt(index, CHAR_WORKFMCHANNELQUICK);
  {
    char *cdkey = CHAR_getChar(index, CHAR_CDKEY);
    char *charname = CHAR_getChar(index, CHAR_NAME);
    if (strlen(message) > 3) {
      if (CHAR_getWorkInt(index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE) {
        if (strstr(message, "[") && strstr(message, "]")) {
        } else {
          LogTalk(charname, cdkey, CHAR_getInt(index, CHAR_FLOOR),
                  CHAR_getInt(index, CHAR_X), CHAR_getInt(index, CHAR_Y),
                  message);
        }
      }
    }
  }
  if (*message == 0)
    return;
  CHAR_getMessageBody(message, firstToken, sizeof(firstToken), &messagebody);
  if (!messagebody)
    return;
  strcpysafe(messageeraseescape, sizeof(messageeraseescape), messagebody);
  makeStringFromEscaped(messageeraseescape);
  stringlen = strlen(messageeraseescape);
  if (stringlen <= 0)
    return;

#ifdef _ALLBLUES_LUA_1_6
  if (CharTalkFunction(index, messageeraseescape, color) == TRUE) {
    return;
  }
#endif
#ifdef _THE_WORLD_SEND // 傈眉盲泼 ( 扁瓷荤侩 X )
  if (messageeraseescape[0] == '/' && messageeraseescape[1] == 'W' &&
      messageeraseescape[2] == 'D') {
    int lastTalkTime = CHAR_getWorkInt(index, CHAR_WORKWORLDSENDTIME);

    if ((int)NowTime.tv_sec - lastTalkTime > 300) {
      char token[256];
      char buff[256];
      char buf[64];
      if (CHAR_getInt(index, CHAR_TRANSMIGRATION) >= getTheWorldTrans() &&
          CHAR_getInt(index, CHAR_LV) >= getTheWorldLevel()) {
        if (CHAR_getInt(index, CHAR_FAME) >= getTheWorldSend() * 100) {
          sprintf(token, "[%d]疙己捞 家葛登菌嚼聪促!", getTheWorldSend());
          CHAR_talkToCli(index, -1, token, CHAR_COLORRED);
          //					easyGetTokenFromString( messageeraseescape , 2
          //, buff, sizeof(buff));
          sprintf(token, "[世界] %s 說：%s", CHAR_getChar(index, CHAR_NAME),
                  messageeraseescape + 4);
          int i;
          int playernum = CHAR_getPlayerMaxNum();
          for (i = 0; i < playernum; i++) {
            if (CHAR_getCharUse(i) != FALSE) {
              CHAR_talkToCli(i, -1, token, color);
            }
          }
          CHAR_setInt(index, CHAR_FAME,
                      CHAR_getInt(index, CHAR_FAME) - getTheWorldSend() * 100);
          CHAR_setWorkInt(index, CHAR_WORKWORLDSENDTIME, (int)NowTime.tv_sec);
        } else {
          sprintf(token, "您的聲望不足,暫無法使用世界頻道!");
          CHAR_talkToCli(index, -1, token, CHAR_COLORRED);
        }
        return;
      } else {
        sprintf(token, "您的轉生等級未達到要求,暫不能使用世界頻道!");
        CHAR_talkToCli(index, -1, token, CHAR_COLORRED);
        return;
      }
    } else {
      CHAR_talkToCli(index, -1, "每次使用世界頻道需間隔五秒纔能使用世界頻道!",
                     CHAR_COLORRED);
      return;
    }
  }
#endif

#ifdef _TELLCHANNEL // (荇师?) Syu ADD 谒??猿
  if (messageeraseescape[0] == '/' && messageeraseescape[1] == 't' &&
      messageeraseescape[2] == 'e' && messageeraseescape[3] == 'l' &&
      messageeraseescape[4] == 'l') {
    getStringFromIndexWithDelim(message, " ", 2, tmp1, sizeof(tmp1));
    if ((tmp2 = strstr(message, tmp1)) != NULL) {
      // strcpy ( message , tmp2 + strlen ( tmp1 ) + 1 ) ;
      message = tmp2 + strlen(tmp1) + 1;
      if (message == "\0" || strlen(message) <= 0)
        return;
    }
    OneByOneTkChannel(fd, tmp1, message, color);
    return;
  }
#endif
#ifdef _CHAR_TRANSPOINT
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "券器") != NULL &&
      messageeraseescape[5] == NULL) {
    static int questTbl[20] = {1,  2,  3,  4,  8,  9,  12, 15, 16, 17,
                               20, 26, 28, 31, 34, 35, 36, 38, 45, 47};
    int NPC_TransmigrationQuestCheck(int toindex) {
      int i;
      int num = 0;

      for (i = 0; i < 20; i++) {
        if (NPC_EventCheckFlg(toindex, questTbl[i]) == TRUE) {
          num++;
        }
      }

      return num;
    }
    int vital, str, tgh, dex;
    int quest, level, equ;
    int t_quest, t_level;
    int total, ans;
    float tmp, tmp1, tmp2, tmp3, tmp4, mlvup, num, num2;
    float tt_quest, tt_level;
    float tmp11, tmp22, tmp33, tmp44;

    vital = CHAR_getInt(index, CHAR_VITAL);
    str = CHAR_getInt(index, CHAR_STR);
    tgh = CHAR_getInt(index, CHAR_TOUGH);
    dex = CHAR_getInt(index, CHAR_DEX);

    quest = NPC_TransmigrationQuestCheck(index);
    level = CHAR_getInt(index, CHAR_LV);
    equ = CHAR_getInt(index, CHAR_TRANSEQUATION);

    t_quest = (float)((equ >> 16) & 0xFFFF);
    t_level = (float)((equ >> 0) & 0xFFFF);

    if (level > 140)
      level = 140;
    t_quest = t_quest + quest;
    t_level = t_level + level;

    total = vital + str + tgh + dex;

    tt_quest = (float)t_quest;
    tt_level = (float)t_level;

    num = CHAR_getInt(index, CHAR_TRANSMIGRATION);
    num2 = num + 1;

    if (num == 6) {
      total = 62000;
    }

    tmp = (float)total / 100;
    ans = (float)tmp / 12 + (float)(tt_quest / 4) +
          ((float)(tt_level - num2 * 85) / 4);

    tmp1 = (float)vital / total * (int)ans;
    tmp2 = (float)str / total * (int)ans;
    tmp3 = (float)tgh / total * (int)ans;
    tmp4 = (float)dex / total * (int)ans;

    tmp11 = (float)tmp1 + 0.5;
    tmp22 = (float)tmp2 + 0.5;
    tmp33 = (float)tmp3 + 0.5;
    tmp44 = (float)tmp4 + 0.5;

    char msg[256];
    char msg1[256];
    char msg2[256];
    char msg3[256];
    char clean[32] = "";
    int f;
    for (f = 0; f < 15; f++) {
      CHAR_talkToCli(index, -1, clean, CHAR_COLORWHITE);
    }
    sprintf(msg, "饭骇 : %d, 券积 : %d, 券器涅 努府绢 : %d 雀",
            CHAR_getInt(index, CHAR_LV),
            CHAR_getInt(index, CHAR_TRANSMIGRATION), quest);
    sprintf(msg1,
            "泅犁 器牢飘 : 眉仿:[%d], 肯仿:[%d], 扒碍:[%d], 鉴惯仿:[%d], 醚 "
            "钦:[%d]",
            (int)vital / 100, (int)str / 100, (int)tgh / 100, (int)dex / 100,
            (int)vital / 100 + (int)str / 100 + (int)tgh / 100 +
                (int)dex / 100);
    sprintf(msg2,
            "券积 器牢飘 : 眉仿:[%d], 肯仿:[%d], 扒碍:[%d], 鉴惯仿:[%d], 醚 "
            "钦:[%d], 焊呈胶:[%g]",
            (int)(tmp11), (int)(tmp22), (int)(tmp33), (int)(tmp44),
            (int)tmp11 + (int)tmp22 + (int)tmp33 + (int)tmp44, num2 * 10);
    sprintf(msg3, "醚 饭骇痢荐 : [%d], 醚 券器涅 痢荐 : [%d]", t_level,
            t_quest);
    CHAR_talkToCli(index, -1, msg, CHAR_COLORVIOLET);
    CHAR_talkToCli(index, -1, msg1, CHAR_COLORVIOLET);
    CHAR_talkToCli(index, -1, msg2, CHAR_COLORVIOLET);
    CHAR_talkToCli(index, -1, msg3, CHAR_COLORVIOLET);
    return;
  }
#endif
#ifdef _CHAR_INFORMATION
  if (messageeraseescape[0] == '/' && strstr(messageeraseescape, "沥焊")) {
    char msg[128], msg1[128], property[128], msg2[128], msg3[128], msg4[128],
        msg5[128];

    int getexp = CHAR_GetLevelExp(index, CHAR_getInt(index, CHAR_LV) + 1);
    int exp = CHAR_getInt(index, CHAR_EXP);
    int exppersent = ((double)exp / (double)getexp) * 100;

    int hh, mm;
    hh = (120 - CHAR_getInt(index, CHAR_EVENTTIME)) / 60;
    mm = (120 - CHAR_getInt(index, CHAR_EVENTTIME)) % 60;

    int hour, min, sec;
    sec = CHAR_getWorkInt(index, CHAR_WORKITEM_ADDEXPTIME);
    min = sec / 60;
    hour = min / 60;
    sec = sec % 60;
    min = min % 60;

    int hour2, min2, sec2;
    sec2 = CHAR_getWorkInt(index, CHAR_WORKITEM_NOENEMYONTIME);
    min2 = sec2 / 60;
    hour2 = min2 / 60;
    sec2 = sec2 % 60;
    min2 = min2 % 60;

    if (getexp > 0) {
      sprintf(msg, "捞抚: %s　券积: %d　饭骇: %d　版氰摹: %d/%d(%d％)",
              CHAR_getChar(index, CHAR_NAME),
              CHAR_getInt(index, CHAR_TRANSMIGRATION),
              CHAR_getInt(index, CHAR_LV), exp, getexp, exppersent);
    } else {
      sprintf(msg, "捞抚: %s　券积: %d　饭骇: %d　版氰摹: %d/%d",
              CHAR_getChar(index, CHAR_NAME),
              CHAR_getInt(index, CHAR_TRANSMIGRATION),
              CHAR_getInt(index, CHAR_LV), exp, getexp);
    }
    CHAR_talkToCli(index, -1, msg, CHAR_COLORGOLDEN);

    sprintf(msg1, "郴备仿: %d/%d　扁仿: %d/%d　傍: %d　规: %d　鉴: %d　加己: ",
            CHAR_getInt(index, CHAR_HP), CHAR_getWorkInt(index, CHAR_WORKMAXHP),
            CHAR_getInt(index, CHAR_MP), CHAR_getWorkInt(index, CHAR_WORKMAXMP),
            CHAR_getWorkInt(index, CHAR_WORKATTACKPOWER),
            CHAR_getWorkInt(index, CHAR_WORKDEFENCEPOWER),
            CHAR_getWorkInt(index, CHAR_WORKQUICK));
    if (CHAR_getInt(index, CHAR_EARTHAT) > 0) {
      sprintf(property, "瘤%d ", CHAR_getInt(index, CHAR_EARTHAT) / 10);
      strcat(msg1, property);
    } else if (CHAR_getInt(index, CHAR_EARTHAT) < 0) {
      sprintf(property, "瘤%d ", CHAR_getInt(index, CHAR_EARTHAT) / 10);
      strcat(msg1, property);
    }
    if (CHAR_getInt(index, CHAR_WATERAT) > 0) {
      sprintf(property, "荐%d ", CHAR_getInt(index, CHAR_WATERAT) / 10);
      strcat(msg1, property);
    } else if (CHAR_getInt(index, CHAR_WATERAT) < 0) {
      sprintf(property, "荐%d ", CHAR_getInt(index, CHAR_WATERAT) / 10);
      strcat(msg1, property);
    }
    if (CHAR_getInt(index, CHAR_FIREAT) > 0) {
      sprintf(property, "拳%d ", CHAR_getInt(index, CHAR_FIREAT) / 10);
      strcat(msg1, property);
    } else if (CHAR_getInt(index, CHAR_FIREAT) < 0) {
      sprintf(property, "拳%d ", CHAR_getInt(index, CHAR_FIREAT) / 10);
      strcat(msg1, property);
    }
    if (CHAR_getInt(index, CHAR_WINDAT) > 0) {
      sprintf(property, "浅%d ", CHAR_getInt(index, CHAR_WINDAT) / 10);
      strcat(msg1, property);
    } else if (CHAR_getInt(index, CHAR_WINDAT) < 0) {
      sprintf(property, "浅%d ", CHAR_getInt(index, CHAR_WINDAT) / 10);
      strcat(msg1, property);
    }
    CHAR_talkToCli(index, -1, msg1, CHAR_COLORYELLOW);

    sprintf(msg2,
            "胶　沛:%8dＳ　　篮　青:%8dＳ　　疙　己:%8dＦ　　VIPPOINT:%8dＰ",
            CHAR_getInt(index, CHAR_GOLD), CHAR_getInt(index, CHAR_PERSONAGOLD),
            CHAR_getInt(index, CHAR_FAME), CHAR_getInt(index, CHAR_AMPOINT));
    CHAR_talkToCli(index, -1, msg2, CHAR_COLORWHITE);
    sprintf(msg3, "Ｗ　Ｐ:%8dＰ　　Ｒ　Ｐ:%8dＰ　　Ｖ　Ｐ:%8dＰ(%d矫埃 %d盒)",
            CHAR_getInt(index, CHAR_PVPPOINT),
            CHAR_getInt(index, CHAR_RAIDPOINT),
            CHAR_getInt(index, CHAR_EVENTPOINT), hh, mm);
    CHAR_talkToCli(index, -1, msg3, CHAR_COLORWHITE);
#ifdef _UP_ON_OFF
    if (CHAR_getInt(index, CHAR_ONOFF01) == 1) {
      sprintf(msg4, "版氰摹 备浇 儡咯 矫埃: %02d矫埃 %02d盒［ON］", hour, min);
    } else {
      sprintf(msg4, "版氰摹 备浇 儡咯 矫埃: %02d矫埃 %02d盒［OFF］", hour, min);
    }
    CHAR_talkToCli(index, -1, msg4, CHAR_COLORCYAN);
#endif
#ifdef _ITEM_NOENEMY
    if (CHAR_getInt(index, CHAR_ONOFF02) == 1) {
      sprintf(msg5, "寒没狼 家扼 儡咯 矫埃: %02d矫埃 %02d盒［ON］", hour2,
              min2);
    } else {
      sprintf(msg5, "寒没狼 家扼 儡咯 矫埃: %02d矫埃 %02d盒［OFF］", hour2,
              min2);
    }
    CHAR_talkToCli(index, -1, msg5, CHAR_COLORBLUE);
#endif
    return;
  }
#endif
#ifdef _UP_ON_OFF
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "版备柯") != NULL &&
      messageeraseescape[7] == NULL) {
    if (CHAR_getWorkInt(index, CHAR_WORKITEM_ADDEXPTIME) <= 0) {
      CHAR_talkToCli(index, -1, "版氰摹 备浇 儡咯 矫埃捞 巢酒乐瘤 臼酒夸.",
                     CHAR_COLORRED);
      return;
    }
    if (CHAR_getInt(index, CHAR_ONOFF01) == 1) {
      CHAR_talkToCli(index, -1, "捞固 版氰摹 备浇 瓤苞啊 惯悼 等 惑怕涝聪促.",
                     CHAR_COLORRED);
      return;
    }

    char msg1[256], msg2[256];
    int hour, min, sec;
    sec = CHAR_getWorkInt(index, CHAR_WORKITEM_ADDEXPTIME);

    min = sec / 60;
    hour = min / 60;
    sec = sec % 60;
    min = min % 60;

    sprintf(msg1, "版氰摹 备浇 瓤苞啊 惯悼 登菌绢夸. [%02d矫埃 %02d盒]", hour,
            min);
    sprintf(msg2, "∝ 版氰摹 备浇 荤侩阑 肛眠矫妨搁 怖 [/版备坷橇] 疙飞绢甫 "
                  "荤侩秦林技夸!");
    CHAR_setInt(index, CHAR_ONOFF01, 1);
    CHAR_talkToCli(index, -1, msg1, CHAR_COLORYELLOW);
    CHAR_talkToCli(index, -1, msg2, CHAR_COLORRED);
    return;
  }
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "版备坷橇") != NULL &&
      messageeraseescape[9] == NULL) {
    if (CHAR_getInt(index, CHAR_ONOFF01) == 0) {
      CHAR_talkToCli(index, -1,
                     "泅犁 版氰摹 备浇 瓤苞啊 惯悼登瘤 臼篮 惑怕涝聪促.",
                     CHAR_COLORRED);
      return;
    }

    CHAR_setWorkInt(index, CHAR_WORKITEM_ADDEXPTIME,
                    (CHAR_getWorkInt(index, CHAR_WORKITEM_ADDEXPTIME) - 60));

    char msg1[256], msg2[256];
    int hour, min, sec;
    sec = CHAR_getWorkInt(index, CHAR_WORKITEM_ADDEXPTIME);

    min = sec / 60;
    hour = min / 60;
    sec = sec % 60;
    min = min % 60;

    sprintf(msg1, "版氰摹 备浇 瓤苞啊 辆丰 登菌绢夸. [%02d矫埃 %02d盒]", hour,
            min);
    sprintf(msg2, "∝ 版氰摹 备浇狼 儡咯 矫埃捞 60檬 瞒皑 登菌嚼聪促.");
    CHAR_talkToCli(index, -1, msg1, CHAR_COLORYELLOW);
    CHAR_talkToCli(index, -1, msg2, CHAR_COLORRED);
    CHAR_setInt(index, CHAR_ONOFF01, 0);
    return;
  }
#endif
#ifdef _ITEM_NOENEMY
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "寒没柯") != NULL &&
      messageeraseescape[7] == NULL) {
    int floor = CHAR_getInt(index, CHAR_FLOOR);
    if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) ==
        CHAR_PARTY_CLIENT) { // 颇萍盔 荤侩 阂啊
      CHAR_talkToCli(index, -1,
                     "颇萍葛靛俊辑绰 颇萍厘父 荤侩 啊瓷茄 酒捞袍涝聪促.",
                     CHAR_COLORRED);
      return;
    }
    if (floor == 120 || floor == 201 || floor == 1000 || floor == 1006 ||
        floor == 1032 || floor == 1100 || floor == 1200 || floor == 1300 ||
        floor == 1400 || floor == 1500 || floor == 2000 || floor == 2006 ||
        floor == 3000 || floor == 3006 || floor == 3100 || floor == 3200 ||
        floor == 3300 || floor == 3400 || floor == 4000 || floor == 4006 ||
        floor == 5000 || floor == 5005 || floor == 5100 || floor == 5106) {
      CHAR_talkToCli(index, -1, "付阑 救俊辑绰 寒没狼 家扼甫 荤侩且 荐 绝绢夸.",
                     CHAR_COLORRED);
      return;
    }
    if (CHAR_getWorkInt(index, CHAR_WORKITEM_NOENEMYONTIME) <= 0) {
      CHAR_talkToCli(index, -1, "寒没狼 家扼 儡咯 矫埃捞 巢酒乐瘤 臼酒夸.",
                     CHAR_COLORRED);
      return;
    }
    if (CHAR_getInt(index, CHAR_ONOFF02) == 1) {
      CHAR_talkToCli(index, -1, "捞固 寒没狼 家扼 瓤苞啊 惯悼 等 惑怕涝聪促.",
                     CHAR_COLORRED);
      return;
    }

    char msg1[256], msg2[256];
    int hour, min, sec;
    sec = CHAR_getWorkInt(index, CHAR_WORKITEM_NOENEMYONTIME);

    min = sec / 60;
    hour = min / 60;
    sec = sec % 60;
    min = min % 60;

    sprintf(msg1, "寒没狼 瓤苞啊 惯悼 登菌绢夸. [%02d矫埃 %02d盒]", hour, min);
    sprintf(msg2, "∝ 寒没狼 家扼 荤侩阑 肛眠矫妨搁 怖 [/寒没坷橇] 疙飞绢甫 "
                  "荤侩秦林技夸!");
    setEqNoenemy(CHAR_getWorkInt(index, CHAR_WORKFD), 200);
    CHAR_setInt(index, CHAR_ONOFF02, 1);
    CHAR_talkToCli(index, -1, msg1, CHAR_COLORYELLOW);
    CHAR_talkToCli(index, -1, msg2, CHAR_COLORRED);
    return;
  }
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "寒没坷橇") != NULL &&
      messageeraseescape[9] == NULL) {
    if (CHAR_getInt(index, CHAR_ONOFF02) == 0) {
      CHAR_talkToCli(index, -1, "泅犁 寒没狼 瓤苞啊 惯悼登瘤 臼篮 惑怕涝聪促.",
                     CHAR_COLORRED);
      return;
    }

    CHAR_setWorkInt(index, CHAR_WORKITEM_NOENEMYONTIME,
                    (CHAR_getWorkInt(index, CHAR_WORKITEM_NOENEMYONTIME) - 60));

    char msg1[256], msg2[256];
    int hour, min, sec;
    sec = CHAR_getWorkInt(index, CHAR_WORKITEM_NOENEMYONTIME);

    min = sec / 60;
    hour = min / 60;
    sec = sec % 60;
    min = min % 60;

    sprintf(msg1, "寒没狼 瓤苞啊 辆丰 登菌绢夸. [%02d矫埃 %02d盒]", hour, min);
    sprintf(msg2, "∝ 寒没狼 家扼 儡咯 矫埃捞 60檬 瞒皑 登菌嚼聪促.");
    CHAR_talkToCli(index, -1, msg1, CHAR_COLORYELLOW);
    CHAR_talkToCli(index, -1, msg2, CHAR_COLORRED);
    setEqNoenemy(CHAR_getWorkInt(index, CHAR_WORKFD), 0);
    CHAR_setInt(index, CHAR_ONOFF02, 0);
    return;
  }
#endif
#ifdef _CHAR_LEVELACHIEVE
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "饭骇崔己") != NULL &&
      messageeraseescape[9] == NULL) {
    char token[256];
    int point = CHAR_getInt(index, CHAR_AHIEVEPOINT);
    int emptyitemindexinchara = CHAR_findEmptyItemBox(index);

    if (emptyitemindexinchara < 0) {
      CHAR_talkToCli(index, -1, "酒捞袍沫捞 啊垫 谩绢夸.", CHAR_COLORWHITE);
    }
    if (point <= 0) {
      CHAR_talkToCli(index, -1, "券旷 器牢飘甫 啊瘤绊 乐瘤 臼嚼聪促.",
                     CHAR_COLORWHITE);
    }
    if (point > 0) {
      int getitemindex = ITEM_makeItemAndRegist(17100);
      if (getitemindex != -1) {
        int emptyteimbox = CHAR_findEmptyItemBox(index);
        CHAR_setItemIndex(index, emptyteimbox, getitemindex);
        CHAR_sendItemDataOne(index, emptyteimbox);
      }
      CHAR_talkToCli(
          index, -1,
          "[1 券旷 器牢飘]甫 荤侩窍咯 券旷狼 脚拱(阑)甫 颊俊 持菌促.",
          CHAR_COLORYELLOW);
      CHAR_setInt(index, CHAR_AHIEVEPOINT, point - 1);
    }
    return;
  }
#endif
  if (messageeraseescape[0] == '/' && strstr(messageeraseescape, "包傈")) {
    char msg[128];
    int fd = getfdFromCharaIndex(index);
    if (fd != -1) {
      lssproto_LB_recv(fd, CHAR_getInt(index, CHAR_X),
                       CHAR_getInt(index, CHAR_Y));
      CHAR_talkToCli(index, -1, msg, CHAR_PERSONAGOLD);
    }
    return;
  }
#ifdef _CHAR_PARTY
  if (messageeraseescape[0] == '/' && strstr(messageeraseescape, "檬措")) {
    if (CHAR_getInt(index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
      return;
    }
    if (CHAR_getWorkInt(index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) {
      return;
    }
    if (CHAR_getWorkInt(index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) {
      return;
    }

    int result = -1;
    int x, y;
    OBJECT object;
    int found = FALSE;
    int fd;
    int cnt;
    int i;

    fd = getfdFromCharaIndex(index);
    if (fd == -1) {
      print("%s : %d err\n", __FILE__, __LINE__);
      return FALSE;
    }

    CHAR_getCoordinationDir(CHAR_getInt(index, CHAR_DIR),
                            CHAR_getInt(index, CHAR_X),
                            CHAR_getInt(index, CHAR_Y), 1, &x, &y);

    for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
      CONNECT_setJoinpartycharaindex(fd, i, -1);
    }
    cnt = 0;

    for (object = MAP_getTopObj(CHAR_getInt(index, CHAR_FLOOR), x, y); object;
         object = NEXT_OBJECT(object)) {
      int toindex;
      int parray;
      int objindex = GET_OBJINDEX(object);
      int targetindex = -1;

      if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
        continue;
      toindex = OBJECT_getIndex(objindex);

      if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        found = TRUE;
        if (CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
          targetindex = CHAR_getWorkInt(toindex, CHAR_WORKPARTYINDEX1);
          if (!CHAR_CHECKINDEX(targetindex)) {
            print(" %s:%d err\n", __FILE__, __LINE__);
            continue;
          }
        } else {
          targetindex = toindex;
        }

        if (NPC_Util_CharDistance(index, targetindex) > 1) {
          continue;
        }
        if (CHAR_getWorkInt(targetindex, CHAR_WORKPARTYMODE) ==
                CHAR_PARTY_LEADER &&
            CHAR_getWorkInt(targetindex, CHAR_WORKPARTYMODE) ==
                CHAR_PARTY_CLIENT) {
          return;
        }
        if (CHAR_getWorkInt(targetindex, CHAR_WORKTRADEMODE) !=
            CHAR_TRADE_FREE) {
          return;
        }
        if (CHAR_getWorkInt(targetindex, CHAR_WORKBATTLEMODE) !=
            BATTLE_CHARMODE_NONE) {
          return;
        }

        if (!CHAR_getFlg(targetindex, CHAR_ISPARTY))
          continue;
      } else {
        continue;
      }
      parray = CHAR_getEmptyPartyArray(index);
      if (parray == -1)
        continue;

      CONNECT_setJoinpartycharaindex(fd, cnt, toindex);
      cnt++;
      if (cnt == CONNECT_WINDOWBUFSIZE)
        break;
    }

    if (cnt == 0) {
      if (found == TRUE) {
        CHAR_talkToCli(index, -1, "檬措且荐 绝嚼聪促。", CHAR_COLORRED);
      }
      result = FALSE;
    } else if (cnt == 1) {
#ifdef _AUTO_PK
      if (CHAR_getInt(index, CHAR_FLOOR) == 20000) {
        result = FALSE;
      } else
#endif
      {
        CHAR_JoinParty_Main(CONNECT_getJoinpartycharaindex(fd, 0), index);
        result = TRUE;
      }
    } else {
      int strlength;
      char msgbuf[1024];
      char escapebuf[2048];
      strcpy(msgbuf, "1\n急琶 窍绞矫坷\n");
      strlength = strlen(msgbuf);
      for (i = 0; CONNECT_getJoinpartycharaindex(fd, i) != -1 &&
                  i < CONNECT_WINDOWBUFSIZE;
           i++) {
        char *a =
            CHAR_getChar(CONNECT_getJoinpartycharaindex(fd, i), CHAR_NAME);
        char buf[256];
        snprintf(buf, sizeof(buf), "%s\n", a);
        if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
          print("%s:%d 扩档快 滚欺 何练\n", __FILE__, __LINE__);
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
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "府歹") != NULL &&
      messageeraseescape[5] == NULL) {
    int i;
    char name[128];
    char token[256];

    token[0] = '\0';

    if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER) {
      CHAR_talkToCli(index, -1, "颇萍惑怕俊辑 府歹父 荤侩且荐 乐嚼聪促.",
                     CHAR_COLORRED);
      return;
    }

    sprintf(token, "颇萍盔 格废 ( 府歹 : %s )", CHAR_getChar(index, CHAR_NAME));
    CHAR_talkToCli(index, -1, token, CHAR_COLORYELLOW);

    token[0] = '\0';

    for (i = CHAR_WORKPARTYINDEX2; i <= CHAR_WORKPARTYINDEX5; i++) {
      if (CHAR_CHECKINDEX(CHAR_getWorkInt(index, i)) == FALSE)
        continue;
      sprintf(token, "%s %d. %s\n", token, i - CHAR_WORKPARTYINDEX2 + 1,
              CHAR_getChar(CHAR_getWorkInt(index, i), CHAR_NAME));
    }
    CHAR_talkToCli(index, -1, token, CHAR_COLORYELLOW);

    return;
  }

  else if (messageeraseescape[0] == '/' &&
           strstr(messageeraseescape, "府歹") != NULL &&
           messageeraseescape[5] == ' ' && messageeraseescape[6] != NULL) {
    char buf[64];
    char name[128];
    int num, i;
    int charaindex = 0;
    int check = 0;

    if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER) {
      CHAR_talkToCli(index, -1, "颇萍惑怕俊辑 府歹父 荤侩且荐 乐嚼聪促",
                     CHAR_COLORRED);
      return;
    }

    easyGetTokenFromString(message, 2, buf, sizeof(buf));

    if (buf[0] == '1' || buf[0] == '2' || buf[0] == '3' || buf[0] == '4') {
      if (buf[1] == '\0') {
        num = atoi(buf);
        check = 1;
      } else
        strcpy(name, buf);
    } else
      strcpy(name, buf);

    if (check) {
      charaindex = CHAR_getWorkInt(index, CHAR_WORKPARTYINDEX1 + num);

      if (CHAR_CHECKINDEX(CHAR_getWorkInt(index, CHAR_WORKPARTYINDEX1 + num)) ==
          FALSE) {
        CHAR_talkToCli(index, -1, "措惑阑 肋给 急琶沁嚼聪促.", CHAR_COLORRED);
        return;
      }
      CHAR_PartyLeader(index, charaindex);
    }

    else {
      for (i = 1; i < CHAR_PARTYMAX; i++) {
        charaindex = CHAR_getWorkInt(index, i + CHAR_WORKPARTYINDEX1);
        if (CHAR_CHECKINDEX(charaindex)) {
          if (!strcmp(name, CHAR_getChar(charaindex, CHAR_NAME)))
            break;
        }
      }
      if (i == CHAR_PARTYMAX) {
        CHAR_talkToCli(index, -1, "措惑阑 肋给 急琶沁嚼聪促.", CHAR_COLORRED);
        return;
      }

      CHAR_PartyLeader(index, charaindex);
    }
    return;
  }
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "曼啊") != NULL &&
      messageeraseescape[5] == NULL) {

    int result = -1;
    int x, y;
    OBJECT object;
    int found = FALSE;
    int fd;
    int cnt;
    int i;

    fd = getfdFromCharaIndex(index);
    if (fd == -1) {
      print("%s : %d err\n", __FILE__, __LINE__);
      return FALSE;
    }

    if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
      lssproto_PR_send(fd, 1, FALSE);
      CHAR_talkToCli(index, -1, "颇萍啊 酒囱 惑怕俊辑父 荤侩且 荐 乐嚼聪促.",
                     CHAR_COLORRED);
      return FALSE;
    }

    CHAR_getCoordinationDir(CHAR_getInt(index, CHAR_DIR),
                            CHAR_getInt(index, CHAR_X),
                            CHAR_getInt(index, CHAR_Y), 1, &x, &y);

    for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++) {
      CONNECT_setJoinpartycharaindex(fd, i, -1);
    }
    cnt = 0;

    for (object = MAP_getTopObj(CHAR_getInt(index, CHAR_FLOOR), x, y); object;
         object = NEXT_OBJECT(object)) {
      int toindex;
      int parray;
      int objindex = GET_OBJINDEX(object);
      int targetindex = -1;

      if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
        continue;
      toindex = OBJECT_getIndex(objindex);

      if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
        found = TRUE;
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

        if (NPC_Util_CharDistance(index, targetindex) > 1) {
          continue;
        }

        if (!CHAR_getFlg(targetindex, CHAR_ISPARTY))
          continue;
      } else if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
        targetindex = toindex;
        cnt = 0;
        if (!NPC_BusCheckJoinParty(toindex, index, TRUE)) {
          break;
        }
        {
          int busimg = CHAR_getInt(toindex, CHAR_BASEIMAGENUMBER);
          if ((busimg != 100355) && (busimg != 100461)) {
            CHAR_setInt(index, CHAR_BASEIMAGENUMBER, busimg);
            CHAR_sendCToArroundCharacter(
                CHAR_getWorkInt(index, CHAR_WORKOBJINDEX));
            if (CHAR_getInt(index, CHAR_RIDEPET) != -1) {
              CHAR_setInt(index, CHAR_RIDEPET, -1);
              CHAR_send_P_StatusString(index, CHAR_P_STRING_RIDEPET);
            }
          }
        }
      } else {
        continue;
      }
      parray = CHAR_getEmptyPartyArray(targetindex);
      if (parray == -1)
        continue;

      CONNECT_setJoinpartycharaindex(fd, cnt, toindex);
      cnt++;
      if (cnt == CONNECT_WINDOWBUFSIZE)
        break;

      if (CHAR_getInt(targetindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS)
        break;
    }

    if (cnt == 0) {
      if (found == TRUE) {
        CHAR_talkToCli(index, -1, "颇萍俊 曼咯且 荐 绝绢夸！", CHAR_COLORRED);
      }
      result = FALSE;
    } else if (cnt == 1) {
#ifdef _AUTO_PK
      if (CHAR_getInt(index, CHAR_FLOOR) == 20000) {
        CHAR_talkToCli(index, -1, "秦寸 瘤开俊急 颇萍啊 阂啊瓷 钦聪促！",
                       CHAR_COLORRED);
        result = FALSE;
      } else
#endif
      {
        CHAR_JoinParty_Main(index, CONNECT_getJoinpartycharaindex(fd, 0));
        result = TRUE;
      }
    } else {
      int strlength;
      char msgbuf[1024];
      char escapebuf[2048];
      strcpy(msgbuf, "1\n急琶 窍绞矫坷\n");
      strlength = strlen(msgbuf);
      for (i = 0; CONNECT_getJoinpartycharaindex(fd, i) != -1 &&
                  i < CONNECT_WINDOWBUFSIZE;
           i++) {
        char *a =
            CHAR_getChar(CONNECT_getJoinpartycharaindex(fd, i), CHAR_NAME);
        char buf[256];
        snprintf(buf, sizeof(buf), "%s\n", a);
        if (strlength + strlen(buf) > arraysizeof(msgbuf)) {
          print("%s:%d 扩档快 滚欺 何练\n", __FILE__, __LINE__);
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
  if (messageeraseescape[0] == '/' &&
      strstr(messageeraseescape, "呕硼") != NULL &&
      messageeraseescape[5] == NULL) {
    if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
      if (CHAR_DischargePartyLeader(index, 1) == FALSE)
        return;
    } else if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) ==
               CHAR_PARTY_CLIENT) {
      if (CHAR_DischargePartyClient(index, 1) == FALSE)
        return;
      if (CHAR_DischargePartyClient(index, 1) == FALSE)
        return;
    }
    return;
  }
#endif

  //涝免陛疙飞绢
  if (messageeraseescape[0] == '/') {
    char token[30];
    char buf[10];
    int money, myney, banky;
    easyGetTokenFromString(messageeraseescape, 1, token, sizeof(token));
    easyGetTokenFromString(messageeraseescape, 2, buf, sizeof(buf));
    money = atoi(buf);
    myney = CHAR_getInt(index, CHAR_GOLD);
    banky = CHAR_getInt(index, CHAR_PERSONAGOLD);
    if (strcmp("/免陛", token) == 0) {
      if (money > banky || myney + money > 100000000 || money <= 0)
        return;
      else {
        CHAR_setInt(index, CHAR_GOLD, myney + money);
        CHAR_setInt(index, CHAR_PERSONAGOLD, banky - money);
        sprintf(buf, "%d 免陛 , 篮青儡绊 %d", money,
                CHAR_getInt(index, CHAR_PERSONAGOLD));
        CHAR_talkToCli(index, -1, buf, CHAR_COLORYELLOW);
        CHAR_send_P_StatusString(index, CHAR_P_STRING_GOLD);
        return;
      }
    } else if (strcmp("/涝陛", token) == 0) {
      if (money > myney || money + banky > 100000000 || money <= 0)
        return;
      else {
        CHAR_setInt(index, CHAR_GOLD, myney - money);
        CHAR_setInt(index, CHAR_PERSONAGOLD, banky + money);
        sprintf(buf, "%d 涝陛 , 篮青儡绊 %d", money,
                CHAR_getInt(index, CHAR_PERSONAGOLD));
        CHAR_talkToCli(index, -1, buf, CHAR_COLORYELLOW);
        CHAR_send_P_StatusString(index, CHAR_P_STRING_GOLD);
        return;
      }
    }
  }

  //蜡府家扼疙飞绢
  if (messageeraseescape[0] == '/' && strstr(messageeraseescape, "傈捧")) {
    char msg[128];
    int fd = CHAR_getWorkInt(index, CHAR_WORKFD);
    setStayEncount(fd);

    Char *ch;
    ch = CHAR_getCharPointer(index);
    if (ch == NULL)
      return;
    strcpysafe(ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
               sizeof(ch->charfunctable[CHAR_LOOPFUNCTEMP1]),
               "CHAR_BattleStayLoop");
    CHAR_setInt(index, CHAR_LOOPINTERVAL, 2500);
    CHAR_constructFunctable(index);

    sprintf(msg, "磊悼 傈捧 葛靛甫 矫累钦聪促");
    CHAR_talkToCli(index, -1, msg, CHAR_COLORYELLOW);
    return;
  }
  if (messageeraseescape[0] == '/' &&
          strstr(messageeraseescape, "辆丰") != NULL &&
          messageeraseescape[5] == NULL ||
      messageeraseescape[0] == '/' && messageeraseescape[1] == 's' &&
          messageeraseescape[2] == 't' && messageeraseescape[3] == 'o' &&
          messageeraseescape[4] == 'p' && messageeraseescape[5] == NULL) {
    fd = CHAR_getWorkInt(index, CHAR_WORKFD);
    clearStayEncount(fd);
    CHAR_talkToCli(index, -1, "磊悼 傈捧 葛靛啊 辆丰邓聪促", CHAR_COLORYELLOW);
    return;
  }

  //其飘己厘 疙飞绢
  if (messageeraseescape[0] == '/' && strstr(messageeraseescape, "其飘")) {
    int i, j, workindex;
    int fvit, fstr, ftgh, fdex; // 历厘 檬扁摹 函荐
    double basevit, basestr, basetgh, basedex, baseall,
        baseplus;                                  // S鞭 单捞磐 海捞胶
    double firstvit, firststr, firsttgh, firstdex; // S鞭 檬扁摹
    double upvit, upstr, uptgh, updex, upret;      // S鞭 己厘伏
    int svit, sstr, stgh, sdex;                    // S鞭 抗惑 瓷仿摹
    int petprice, petgrade;                        // 惑痢啊, 殿鞭
    double K;                                      //焊沥拌荐
    double ret, h, s, t, d;

    if (messageeraseescape[5] == ' ' ||
        messageeraseescape[6] != NULL && messageeraseescape[7] == NULL) {
      sprintf(messageeraseescape, "%s", messageeraseescape + 5);
      i = atoi(messageeraseescape);
      if (workindex = CHAR_getCharPet(index, i - 1)) {
        if (CHAR_getInt(workindex, CHAR_WHICHTYPE) != CHAR_TYPEPET) {
          CHAR_talkToCli(index, -1, "其飘啊 粮犁窍瘤 臼嚼聪促.", CHAR_COLORRED);
          return;
        }
        // 版氰摹 历厘
        int levelexp = CHAR_GetLevelExp(workindex,
                                        CHAR_getInt(workindex, CHAR_LV) + 1),
            charexpup = CHAR_getInt(workindex, CHAR_EXP);
        // 惑痢啊 殿鞭
        if (CHAR_getInt(workindex, CHAR_RARE) == 0) {
          petprice = (int)((((CHAR_getInt(workindex, CHAR_LV) + 1) *
                             CHAR_getInt(workindex, CHAR_LV)) /
                            2) -
                           1) *
                         20 +
                     10;
          petgrade = 0;
        } else if (CHAR_getInt(workindex, CHAR_RARE) == 1) {
          petprice = (int)(((((CHAR_getInt(workindex, CHAR_LV) + 1) *
                              CHAR_getInt(workindex, CHAR_LV)) /
                             2) -
                            1) *
                               20 +
                           10) *
                     5;
          petgrade = 2;
        } else if (CHAR_getInt(workindex, CHAR_RARE) == 2) {
          petprice = (int)(((((CHAR_getInt(workindex, CHAR_LV) + 1) *
                              CHAR_getInt(workindex, CHAR_LV)) /
                             2) -
                            1) *
                               20 +
                           10) *
                     8;
          petgrade = 1;
        }
        fvit = CHAR_getInt(workindex, CHAR_PETDATAH); // 历厘 檬扁摹 郴备仿
        fstr = CHAR_getInt(workindex, CHAR_PETDATAS); // 历厘 檬扁摹 傍拜仿
        ftgh = CHAR_getInt(workindex, CHAR_PETDATAT); // 历厘 檬扁摹 规绢仿
        fdex = CHAR_getInt(workindex, CHAR_PETDATAD); // 历厘 檬扁摹 鉴惯仿

        basevit = (double)(CHAR_getInt(workindex, CHAR_DATAVIT) +
                           4.5); // S鞭 单捞磐 海捞胶 郴备仿
        basestr = (double)(CHAR_getInt(workindex, CHAR_DATASTR) +
                           4.5); // S鞭 单捞磐 海捞胶 傍拜仿
        basetgh = (double)(CHAR_getInt(workindex, CHAR_DATATGH) +
                           4.5); // S鞭 单捞磐 海捞胶 规绢仿
        basedex = (double)(CHAR_getInt(workindex, CHAR_DATADEX) +
                           4.5); // S鞭 单捞磐 海捞胶 鉴惯仿
        baseall = (double)(CHAR_getInt(
            workindex, CHAR_DATAALL)); // S鞭 单捞磐 海捞胶 Initnum
        baseplus = (CHAR_getInt(workindex, CHAR_DATAVIT) +
                    CHAR_getInt(workindex, CHAR_DATASTR) +
                    CHAR_getInt(workindex, CHAR_DATATGH) +
                    CHAR_getInt(workindex, CHAR_DATADEX)); // 海捞胶 钦

        // 珐农俊 蝶弗 焊沥拌荐 K蔼
        if (CHAR_getInt(workindex, CHAR_PETRANK) == 0) {
          K = (double)4.75;
        } else if (CHAR_getInt(workindex, CHAR_PETRANK) == 1) {
          K = (double)4.95;
        } else if (CHAR_getInt(workindex, CHAR_PETRANK) == 2) {
          K = (double)5.15;
        } else if (CHAR_getInt(workindex, CHAR_PETRANK) == 3) {
          K = (double)5.35;
        } else if (CHAR_getInt(workindex, CHAR_PETRANK) == 4) {
          K = (double)5.55;
        } else if (CHAR_getInt(workindex, CHAR_PETRANK) == 5) {
          K = (double)5.75;
        }

        // S鞭 檬扁摹
        firstvit = (double)((basevit * 4 + basestr + basetgh + basedex) *
                            baseall / 100);
        firststr = (double)((basevit * 0.1 + basestr + basetgh * 0.1 +
                             basedex * 0.05) *
                            baseall / 100);
        firsttgh = (double)((basevit * 0.1 + basestr * 0.1 + basetgh +
                             basedex * 0.05) *
                            baseall / 100);
        firstdex = (double)(basedex * baseall / 100);

        // S鞭 己厘伏
        upvit = (basevit * 4 + basestr + basetgh + basedex) * K / 100;
        upstr = (basevit * 0.1 + basestr + basetgh * 0.1 + basedex * 0.05) * K /
                100;
        uptgh = (basevit * 0.1 + basestr * 0.1 + basetgh + basedex * 0.05) * K /
                100;
        updex = basedex * K / 100;
        upret = upstr + uptgh + updex;

        // S鞭 抗惑 瓷仿摹
        svit =
            (firstvit + upvit * (CHAR_getInt(workindex, CHAR_LV) - 1)) - 0.03;
        sstr =
            (firststr + upstr * (CHAR_getInt(workindex, CHAR_LV) - 1)) - 0.03;
        stgh =
            (firsttgh + uptgh * (CHAR_getInt(workindex, CHAR_LV) - 1)) - 0.03;
        sdex =
            (firstdex + updex * (CHAR_getInt(workindex, CHAR_LV) - 1)) - 0.03;

        // 郴备仿 泅犁 己厘摹
        h = CHAR_getWorkInt(workindex, CHAR_WORKMAXHP) - (int)firstvit;
        h = h / (CHAR_getInt(workindex, CHAR_LV) - 1);
        // 傍拜仿 泅犁 己厘摹
        s = CHAR_getWorkInt(workindex, CHAR_WORKATTACKPOWER) - (int)firststr;
        s = s / (CHAR_getInt(workindex, CHAR_LV) - 1);
        // 规绢仿 泅犁 己厘摹
        t = CHAR_getWorkInt(workindex, CHAR_WORKDEFENCEPOWER) - (int)firsttgh;
        t = t / (CHAR_getInt(workindex, CHAR_LV) - 1);
        // 鉴惯仿 泅犁 己厘摹
        d = CHAR_getWorkInt(workindex, CHAR_WORKQUICK) - (int)firstdex;
        d = d / (CHAR_getInt(workindex, CHAR_LV) - 1);
        // 醚 泅犁 己厘摹
        ret = (CHAR_getWorkInt(workindex, CHAR_WORKATTACKPOWER) +
               CHAR_getWorkInt(workindex, CHAR_WORKDEFENCEPOWER) +
               CHAR_getWorkInt(workindex, CHAR_WORKQUICK));
        ret = ret - ((int)firststr + (int)firsttgh + (int)firstdex);
        ret = ret / (CHAR_getInt(workindex, CHAR_LV) - 1);

        char msg[256];
        char msg1[256];
        char msg2[256];
        char msg3[256];
        char msg4[256];
        char msg5[256];
        char msg6[256];
        char clean[32] = "";

        char *gradeh;
        char *grades;
        char *gradet;
        char *graded;
        char *gradea;
        // 眉仿 殿鞭
        if (h >= upvit) {
          gradeh = "S+";
        } else if (h >= upvit - 0.06) {
          gradeh = "S";
        } else if (h >= upvit - 0.12) {
          gradeh = "A+";
        } else if (h >= upvit - 0.18) {
          gradeh = "A";
        } else if (h >= upvit - 0.24) {
          gradeh = "B+";
        } else if (h >= upvit - 0.3) {
          gradeh = "B";
        } else if (h >= upvit - 0.36) {
          gradeh = "C+";
        } else if (h >= upvit - 0.42) {
          gradeh = "C";
        } else if (h >= upvit - 0.48) {
          gradeh = "D+";
        } else if (h >= upvit - 0.54) {
          gradeh = "D";
        } else if (h < upvit - 0.54) {
          gradeh = "F";
        }
        // 傍拜 殿鞭
        if (s >= upstr) {
          grades = "S+";
        } else if (s >= upstr - 0.01) {
          grades = "S";
        } else if (s >= upstr - 0.02) {
          grades = "A+";
        } else if (s >= upstr - 0.03) {
          grades = "A";
        } else if (s >= upstr - 0.04) {
          grades = "B+";
        } else if (s >= upstr - 0.05) {
          grades = "B";
        } else if (s >= upstr - 0.06) {
          grades = "C+";
        } else if (s >= upstr - 0.07) {
          grades = "C";
        } else if (s >= upstr - 0.08) {
          grades = "D+";
        } else if (s >= upstr - 0.09) {
          grades = "D";
        } else if (s < upstr - 0.09) {
          grades = "F";
        }
        // 规绢 殿鞭
        if (t >= uptgh) {
          gradet = "S+";
        } else if (t >= uptgh - 0.01) {
          gradet = "S";
        } else if (t >= uptgh - 0.02) {
          gradet = "A+";
        } else if (t >= uptgh - 0.03) {
          gradet = "A";
        } else if (t >= uptgh - 0.04) {
          gradet = "B+";
        } else if (t >= uptgh - 0.05) {
          gradet = "B";
        } else if (t >= uptgh - 0.06) {
          gradet = "C+";
        } else if (t >= uptgh - 0.07) {
          gradet = "C";
        } else if (t >= uptgh - 0.08) {
          gradet = "D+";
        } else if (t >= uptgh - 0.09) {
          gradet = "D";
        } else if (t < uptgh - 0.09) {
          gradet = "F";
        }
        // 鉴惯 殿鞭
        if (d >= updex) {
          graded = "S+";
        } else if (d >= updex - 0.01) {
          graded = "S";
        } else if (d >= updex - 0.02) {
          graded = "A+";
        } else if (d >= updex - 0.03) {
          graded = "A";
        } else if (d >= updex - 0.04) {
          graded = "B+";
        } else if (d >= updex - 0.05) {
          graded = "B";
        } else if (d >= updex - 0.06) {
          graded = "C+";
        } else if (d >= updex - 0.07) {
          graded = "C";
        } else if (d >= updex - 0.08) {
          graded = "D+";
        } else if (d >= updex - 0.09) {
          graded = "D";
        } else if (d < updex - 0.09) {
          graded = "F";
        }
        // 醚己厘伏 殿鞭
        if (ret >= upstr + uptgh + updex) {
          gradea = "S+";
        } else if (ret >= upstr + uptgh + updex - 0.03) {
          gradea = "S";
        } else if (ret >= upstr + uptgh + updex - 0.06) {
          gradea = "A+";
        } else if (ret >= upstr + uptgh + updex - 0.09) {
          gradea = "A";
        } else if (ret >= upstr + uptgh + updex - 0.12) {
          gradea = "B+";
        } else if (ret >= upstr + uptgh + updex - 0.15) {
          gradea = "B";
        } else if (ret >= upstr + uptgh + updex - 0.18) {
          gradea = "C+";
        } else if (ret >= upstr + uptgh + updex - 0.21) {
          gradea = "C";
        } else if (ret >= upstr + uptgh + updex - 0.24) {
          gradea = "D+";
        } else if (ret >= upstr + uptgh + updex - 0.27) {
          gradea = "D";
        } else if (ret < upstr + uptgh + updex - 0.27) {
          gradea = "F";
        }

        int exp = CHAR_getInt(workindex, CHAR_EXP);
        int getexp =
            CHAR_GetLevelExp(workindex, CHAR_getInt(workindex, CHAR_LV) + 1);
        int resetp = CHAR_getInt(workindex, CHAR_RESET);
        if (getexp > 0) {
          snprintf(msg, sizeof(msg),
                   "其 飘: %s, Lv. %d, 惑痢啊拜: %d, 檬扁拳 冉荐: %d",
                   CHAR_getChar(workindex, CHAR_NAME),
                   CHAR_getInt(workindex, CHAR_LV), petprice, 5 - resetp);
          snprintf(msg6, sizeof(msg6), "版氰摹 : %d / %d [%.2f％]", exp, getexp,
                   ((float)exp / (float)getexp) * 100);
        } else {
          snprintf(msg, sizeof(msg),
                   "其 飘: %s, Lv. %d, 惑痢啊拜: %d, 檬扁拳 冉荐: %d",
                   CHAR_getChar(workindex, CHAR_NAME),
                   CHAR_getInt(workindex, CHAR_LV), petprice, 5 - resetp);
          snprintf(msg6, sizeof(msg6), "版氰摹 : %d / %d", exp, getexp);
        }
        CHAR_talkToCli(index, -1, msg, CHAR_COLORRED);
        CHAR_talkToCli(index, -1, msg6, CHAR_COLORRED);
        if (CHAR_getInt(workindex, CHAR_LV) != 1) {
          snprintf(msg1, sizeof(msg1),
                   "其飘 檬扁摹: 郴 %d, 傍 %d, 规 %d, 鉴 %d / 瓷 仿: 郴 %d, 傍 "
                   "%d, 规 %d, 鉴 %d",
                   fvit, fstr, ftgh, fdex,
                   CHAR_getWorkInt(workindex, CHAR_WORKMAXHP),
                   CHAR_getWorkInt(workindex, CHAR_WORKATTACKPOWER),
                   CHAR_getWorkInt(workindex, CHAR_WORKDEFENCEPOWER),
                   CHAR_getWorkInt(workindex, CHAR_WORKQUICK));
          snprintf(msg2, sizeof(msg2),
                   "Ｓ鞭 檬扁摹: 郴 %d, 傍 %d, 规 %d, 鉴 %d / 瓷 仿: 郴 %d, 傍 "
                   "%d, 规 %d, 鉴 %d",
                   (int)firstvit, (int)firststr, (int)firsttgh, (int)firstdex,
                   (int)svit, (int)sstr, (int)stgh, (int)sdex);
          snprintf(msg3, sizeof(msg3),
                   "己 厘: 郴 %.3lf, 傍 %.3lf, 规 %.3lf, 鉴 %.3lf, 己厘 %.3lf",
                   h, s, t, d, ret);
          snprintf(msg4, sizeof(msg4),
                   "Ｓ 鞭: 郴 %.3lf, 傍 %.3lf, 规 %.3lf, 鉴 %.3lf, 己厘 %.3lf",
                   upvit, upstr, uptgh, updex, upret);
          snprintf(msg5, sizeof(msg5),
                   "殿 鞭: 郴 %s ,傍 %s ,规 %s ,鉴 %s ,傈眉 %s", gradeh, grades,
                   gradet, graded, gradea);
          CHAR_talkToCli(index, -1, msg1, CHAR_COLORYELLOW);
          CHAR_talkToCli(index, -1, msg2, CHAR_COLORGREEN);
          CHAR_talkToCli(index, -1, msg3, CHAR_COLORYELLOW);
          CHAR_talkToCli(index, -1, msg4, CHAR_COLORGREEN);
          CHAR_talkToCli(index, -1, msg5, CHAR_COLORBLUE2);
        } else {
          snprintf(msg2, sizeof(msg2),
                   "其飘 檬扁摹: 郴 %d, 傍 %d, 规 %d, 鉴 %d ", fvit, fstr, ftgh,
                   fdex);
          snprintf(msg3, sizeof(msg3),
                   "Ｓ鞭 檬扁摹: 郴 %d, 傍 %d, 规 %d, 鉴 %d ", (int)firstvit,
                   (int)firststr, (int)firsttgh, (int)firstdex);
          CHAR_talkToCli(index, -1, msg2, CHAR_COLORYELLOW);
          CHAR_talkToCli(index, -1, msg3, CHAR_COLORYELLOW);
        }
        return;
      }
    }
  }

#ifdef _CHANNEL_MODIFY
  // 家族頻道
  if (messageeraseescape[0] == '/' && messageeraseescape[1] == 'F' &&
      messageeraseescape[2] == 'M') {
    sprintf(messageeraseescape, "%s", messageeraseescape + 3);
  } else
    channel = -1;
#endif
#ifdef _CHAR_PROFESSION // WON ADD 人物職業
  // 職業頻道
  if (messageeraseescape[0] == '/' && messageeraseescape[1] == 'O' &&
      messageeraseescape[2] == 'C' && messageeraseescape[3] == 'C') {
    int i, pclass = CHAR_getInt(index, PROFESSION_CLASS) - 1, TalkTo;
    char buf[512];

    sprintf(messageeraseescape, "%s", messageeraseescape + 4);
    for (i = 0; i < getFdnum(); i++) {
      TalkTo = *(piOccChannelMember + (pclass * getFdnum()) + i);
      if (TalkTo > -1) {
        // 如果對方有開頻道
        if (CHAR_getFlg(TalkTo, CHAR_ISOCC) == TRUE) {
          char escapebuf[2048];
          // snprintf(buf,sizeof(buf) - 1,"P|O|[職]%s",messageeraseescape);
          snprintf(buf, sizeof(buf) - 1, "P|O|[職]%s",
                   makeEscapeString(messageeraseescape, escapebuf,
                                    sizeof(escapebuf)));
          lssproto_TK_send(getfdFromCharaIndex(TalkTo),
                           CHAR_getWorkInt(TalkTo, CHAR_WORKOBJINDEX), buf,
                           color);
        }
      }
    }
    return;
  }
#endif
  // GM伽值
  if (messageeraseescape[0] == '[' &&
      messageeraseescape[stringlen - 1] == ']') {
    char *pass;
    unsigned long ip;
    int a, b, c, d;
    messageeraseescape[stringlen - 1] = '\0';
    ip = CONNECT_get_userip(fd);
    a = (ip % 0x100);
    ip = ip / 0x100;
    b = (ip % 0x100);
    ip = ip / 0x100;
    c = (ip % 0x100);
    ip = ip / 0x100;
    d = (ip % 0x100);
    char gm[128];
    pass = strstr(messageeraseescape + 1, getChatMagicPasswd());
    if (pass == messageeraseescape + 1) {
      if (CHAR_useChatMagic(
              index, messageeraseescape + 1 + strlen(getChatMagicPasswd()) + 1,
              TRUE)) {
        print("\n賬號:%s 名字:%s\n來自ip:%d.%d.%d.%d\n使用GM命令%s]\n",
              CHAR_getChar(index, CHAR_CDKEY), CHAR_getChar(index, CHAR_NAME),
              a, b, c, d, messageeraseescape);
        sprintf(gm, "您使用GM命令%s]已成功", messageeraseescape);
        CHAR_talkToCli(index, -1, gm, CHAR_COLORYELLOW);
      }
    } else {
      if (CHAR_getWorkInt(index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE) {
        if (CHAR_useChatMagic(index, messageeraseescape + 1, TRUE)) {
          print("\n賬號:%s 名字:%s\n來自ip:%d.%d.%d.%d\n使用GM命令%s]\n",
                CHAR_getChar(index, CHAR_CDKEY), CHAR_getChar(index, CHAR_NAME),
                a, b, c, d, messageeraseescape);
          sprintf(gm, "您使用GM命令%s]已成功", messageeraseescape);
          CHAR_talkToCli(index, -1, gm, CHAR_COLORYELLOW);
        }
      } else {
        CHAR_useChatMagic(index, messageeraseescape + 1, FALSE);
      }
    }
    messageeraseescape[stringlen - 1] = ']';
    return;
  } else {
    if (CHAR_getWorkInt(index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE) {
      if (strstr(messageeraseescape, "[") != NULL ||
          strstr(messageeraseescape, "]") != NULL) {
        return;
      }
    }
    if (messageeraseescape[0] == '[')
      return;
  }
  mesgvolume = CHAR_getVolume(messageeraseescape);
  if (area == 0)
    area = 3;
  if (mesgvolume == -1)
    lastvolume = -1;
  else
    lastvolume = area - 1 + mesgvolume;
  if (CHAR_getWorkInt(index, CHAR_WORKFLG) & WORKFLG_MICMODE) {
    lastvolume = 15;
    micflg = 1;
  }
  fl = CHAR_getInt(index, CHAR_FLOOR);
  x = CHAR_getInt(index, CHAR_X);
  y = CHAR_getInt(index, CHAR_Y);
  // Robin 0705 channel
  if (messageeraseescape[0] == ':') {
    if ((channel == -1) && (quickchannel != -1))
      channel = quickchannel;
    else if (channel != -1)
      channel = -1;
  }
  {
    typedef void (*TALKF)(int, int, char *, int, int);
    TALKF talkedfunc = NULL;
    talkedfunc = (TALKF)CHAR_getFunctionPointer(index, CHAR_TALKEDFUNC);
    if (talkedfunc) {
      talkedfunc(index, index, messageeraseescape, color, channel);
    }
  }
  if (lastvolume == -1) {
    int j;
    int xx[2] = {x, x + CHAR_getDX(CHAR_getInt(index, CHAR_DIR))};
    int yy[2] = {y, y + CHAR_getDY(CHAR_getInt(index, CHAR_DIR))};
    BOOL talk = FALSE;
    for (j = 0; j < 2; j++) {
      OBJECT object;
      for (object = MAP_getTopObj(fl, xx[j], yy[j]); object;
           object = NEXT_OBJECT(object)) {
        int objindex = GET_OBJINDEX(object);
        int toindex = OBJECT_getIndex(objindex);
        if (OBJECT_getType(objindex) == OBJTYPE_CHARA && toindex != index) {
          if (CHAR_Talk_check(index, toindex, 0)) {
            if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
              talk = TRUE;
            }
            typedef void (*TALKF)(int, int, char *, int, int);
            TALKF talkedfunc = NULL;
            talkedfunc =
                (TALKF)CHAR_getFunctionPointer(toindex, CHAR_TALKEDFUNC);
            if (talkedfunc) {
              talkedfunc(toindex, index, messageeraseescape, color, -1);
            }
#ifdef _ALLBLUES_LUA
            else {
              RunCharTalkedEvent(toindex, index, messageeraseescape, color, -1);
            }
#endif
          }
        }
      }
    }
    if (talk) {
      CHAR_setInt(index, CHAR_TALKCOUNT,
                  CHAR_getInt(index, CHAR_TALKCOUNT) + 1);
    }
  } else {
    int range = CHAR_getRangeFromVolume(lastvolume);
    int i, j;
    BOOL talk = FALSE;
    BOOL channelTalk = FALSE;
    int partyindex[CHAR_PARTYMAX];
    for (i = 0; i < CHAR_PARTYMAX; i++) {
      partyindex[i] = -1;
    }
#ifndef _CHANNEL_MODIFY
    if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
#else
    if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE &&
        channel < 0)
#endif
    {
      for (i = 0; i < CHAR_PARTYMAX; i++) {
        int toindex = CHAR_getPartyIndex(index, i);
        if (CHAR_CHECKINDEX(toindex) && toindex != index) {
          talk = TRUE;
          partyindex[i] = toindex;
          typedef void (*TALKF)(int, int, char *, int, int);
          TALKF talkedfunc = NULL;
          talkedfunc = (TALKF)CHAR_getFunctionPointer(toindex, CHAR_TALKEDFUNC);
          if (talkedfunc) {
            talkedfunc(toindex, index, messageeraseescape, color, -1);
          }
        }
      }
    }
    if (channel > -1 && fmindexi >= 0) {
      int i, tindex;

      channelTalk = TRUE;
      i = 0;
#ifdef _FMVER21
      if (channel == FAMILY_MAXCHANNEL &&
          CHAR_getInt(index, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER)
#else
      if (channel == FAMILY_MAXCHANNEL &&
          CHAR_getInt(index, CHAR_FMLEADERFLAG) == FMMEMBER_MEMBER)
#endif
      {
        char buf[512];
        sprintf(buf, "[族長廣播]%s: %s", CHAR_getChar(index, CHAR_NAME),
                messageeraseescape);
        saacproto_ACFMAnnounce_send(acfd, CHAR_getChar(index, CHAR_FMNAME),
                                    CHAR_getInt(index, CHAR_FMINDEX),
                                    CHAR_getWorkInt(index, CHAR_WORKFMINDEXI),
                                    buf, color);
        return;
      } else

        for (i = 0; i < FAMILY_MAXMEMBER; i++) {
          tindex = channelMember[fmindexi][channel][i];
          if (!CHAR_CHECKINDEX(tindex))
            continue;
          if (tindex >= 0 && tindex != index) {

#ifdef _CHANNEL_MODIFY
            if (CHAR_getFlg(tindex, CHAR_ISFM) == FALSE)
              continue;
#endif
            typedef void (*TALKF)(int, int, char *, int, int);
            TALKF talkedfunc = NULL;
            talk = TRUE;
            talkedfunc =
                (TALKF)CHAR_getFunctionPointer(tindex, CHAR_TALKEDFUNC);

            if (talkedfunc) {
              talkedfunc(tindex, index, messageeraseescape, color, channel);
            }
          }
        }
    } else {
      for (i = x - range / 2; i <= x + range / 2; i++) {
        for (j = y - range / 2; j <= y + range / 2; j++) {
          OBJECT object;
          for (object = MAP_getTopObj(fl, i, j); object;
               object = NEXT_OBJECT(object)) {
            int objindex = GET_OBJINDEX(object);
            int toindex = OBJECT_getIndex(objindex);
            if (OBJECT_getType(objindex) == OBJTYPE_CHARA && toindex != index) {
              int k;
              for (k = 0; k < CHAR_PARTYMAX; k++) {
                if (toindex == partyindex[k]) {
                  break;
                }
              }
              if (k != CHAR_PARTYMAX)
                continue;
              if (CHAR_Talk_check(index, toindex, micflg)) {
                if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
                  talk = TRUE;
                }
                typedef void (*TALKF)(int, int, char *, int, int);
                TALKF talkedfunc = NULL;
                talkedfunc =
                    (TALKF)CHAR_getFunctionPointer(toindex, CHAR_TALKEDFUNC);
                if (talkedfunc) {
                  talkedfunc(toindex, index, messageeraseescape, color, -1);
                }
#ifdef _ALLBLUES_LUA
                else {
                  RunCharTalkedEvent(toindex, index, messageeraseescape, color,
                                     -1);
                }
#endif
              }
            }
          }
        }
      }
    }
    if (CHAR_getInt(index, CHAR_POPUPNAMECOLOR) != color) {
      CHAR_setInt(index, CHAR_POPUPNAMECOLOR, color);
      if (talk) {
        int opt[1];
        opt[0] = color;
        CHAR_sendWatchEvent(CHAR_getWorkInt(index, CHAR_WORKOBJINDEX),
                            CHAR_ACTPOPUPNAME, opt, 1, FALSE);
      }
    }
    if (talk) {
      CHAR_setInt(index, CHAR_TALKCOUNT,
                  CHAR_getInt(index, CHAR_TALKCOUNT) + 1);
    }
  }
}

#ifdef _FONT_SIZE
BOOL CHAR_talkToCliExt(int talkedcharaindex, int talkcharaindex, char *message,
                       CHAR_COLOR color, int fontsize) {
  static char lastbuf[2048];
  static char escapebuf[2048];
  static char mesgbuf[1024];
  int fd;
  int talkchar = -1;

  if (!CHAR_CHECKINDEX(talkedcharaindex))
    return FALSE;
  if (CHAR_getInt(talkedcharaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    //		print("err CHAR_talkToCli CHAR_WHICHTYPE != CHAR_TYPEPLAYER\n");
    return FALSE;
  }

  fd = getfdFromCharaIndex(talkedcharaindex);
  if (fd == -1) {
    //		print("err CHAR_talkToCli can't get fd from:%d \n",
    // talkedcharaindex);
    return FALSE;
  }

  if (color < CHAR_COLORWHITE && color > CHAR_COLORBLACK) {
    print("CHAR_talkToCli color err\n");
    return FALSE;
  }
#ifndef _CHANNEL_MODIFY
  snprintf(lastbuf, sizeof(lastbuf), "P|%s|%d",
           makeEscapeString(CHAR_appendNameAndTitle(talkcharaindex, message,
                                                    mesgbuf, sizeof(mesgbuf)),
                            escapebuf, sizeof(escapebuf)),
           fontsize);
#else
  snprintf(lastbuf, sizeof(lastbuf), "P|P|%s|%d",
           makeEscapeString(CHAR_appendNameAndTitle(talkcharaindex, message,
                                                    mesgbuf, sizeof(mesgbuf)),
                            escapebuf, sizeof(escapebuf)),
           fontsize);
#endif
  if (talkcharaindex == -1)
    talkchar = -1;
  else
    talkchar = CHAR_getWorkInt(talkcharaindex, CHAR_WORKOBJINDEX);
  lssproto_TK_send(fd, talkchar, lastbuf, color);
  return TRUE;
}
BOOL CHAR_talkToCli(int talkedcharaindex, int talkcharaindex, char *message,
                    CHAR_COLOR color) {
  return CHAR_talkToCliExt(talkedcharaindex, talkcharaindex, message, color, 0);
}
#else
BOOL CHAR_talkToCli(int talkedcharaindex, int talkcharaindex, char *message,
                    CHAR_COLOR color) {
  static char lastbuf[2048];
  static char escapebuf[2048];
  static char mesgbuf[1024];
  int fd;
  int talkchar = -1;

  if (!CHAR_CHECKINDEX(talkedcharaindex))
    return FALSE;
  if (CHAR_getInt(talkedcharaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    //		print("err CHAR_talkToCli CHAR_WHICHTYPE != CHAR_TYPEPLAYER\n");
    return FALSE;
  }

  fd = getfdFromCharaIndex(talkedcharaindex);
  if (fd == -1) {
    //		print("err CHAR_talkToCli can't get fd from:%d \n",
    // talkedcharaindex);
    return FALSE;
  }

  if (color < CHAR_COLORWHITE && color > CHAR_COLORBLACK) {
    print("CHAR_talkToCli color err\n");
    return FALSE;
  }
#ifndef _CHANNEL_MODIFY
  snprintf(lastbuf, sizeof(lastbuf), "P|%s",
#else
  snprintf(lastbuf, sizeof(lastbuf), "P|P|%s",
#endif
           makeEscapeString(CHAR_appendNameAndTitle(talkcharaindex, message,
                                                    mesgbuf, sizeof(mesgbuf)),
                            escapebuf, sizeof(escapebuf)));
  if (talkcharaindex == -1)
    talkchar = -1;
  else
    talkchar = CHAR_getWorkInt(talkcharaindex, CHAR_WORKOBJINDEX);
  lssproto_TK_send(fd, talkchar, lastbuf, color);
  return TRUE;
}
#endif // _FONT_SIZE

#ifdef _DROPSTAKENEW
void CHAR_talkToFloor(int floor, int talkindex, char *message,
                      CHAR_COLOR color) {
  int i = 0;
  int playernum = CHAR_getPlayerMaxNum();
  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) == FALSE)
      continue;
    if (!CHAR_CHECKINDEX(i))
      continue;
    if (CHAR_getInt(i, CHAR_FLOOR) == floor) {
      // if (CHAR_getWorkInt(i, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE)
      CHAR_talkToCli(i, talkindex, message, color);
    }
  }
}
#endif

void CHAR_talkToAll(int talkindex, char *message, CHAR_COLOR color) {
  int i = 0;
  int playernum = CHAR_getPlayerMaxNum();
  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) == FALSE)
      continue;
    if (!CHAR_CHECKINDEX(i))
      continue;
    CHAR_talkToCli(i, talkindex, message, color);
  }
}
#ifdef _CHAR_PROFESSION // WON ADD 人物職業
#ifdef _CHANNEL_MODIFY
// 初始化職業頻道
int InitOccChannel(void) {
  if (piOccChannelMember == NULL) {
    piOccChannelMember =
        (int *)calloc(1, (PROFESSION_CLASS_NUM - 1) * getFdnum() * sizeof(int));
    if (piOccChannelMember == NULL) {
      printf("Char_talk.c:InitOccChannel() memory allocate failed!\n");
      return 0;
    }
    memset(piOccChannelMember, -1,
           (PROFESSION_CLASS_NUM - 1) * getFdnum() * sizeof(int));
  }
  return 1;
}
#endif
#endif
#ifdef _TALK_ACTION
void TalkAction(int charaindex, char *message) {
  int i;
  typedef struct {
    char talk[5];
    int action;
  } tagTlakAction;
  tagTlakAction TlakAction[13] = {
      {"坐下", 11}, {"揮手", 12}, {"點頭", 18}, {"高興", 13}, {"生氣", 14},
      {"悲傷", 15}, {"站立", 19}, {"走動", 17}, {"暈倒", 10}, {"攻擊", 2},
      {"防禦", 16}, {"受傷", 4},  {"投擲", 3}};
  for (i = 0; i < 13; i++)
    if (strstr(message, TlakAction[i].talk))
      break;

  if (i < 13) {
    CHAR_setWorkInt(charaindex, CHAR_WORKACTION, TlakAction[i].action);
    CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX),
                        TlakAction[i].action, NULL, 0, TRUE);
  }
}
#endif

#ifdef _RE_GM_COMMAND
int re_gm_command() {
  FILE *fp;
  fp = fopen("data/regmcommand.txt", "r");
  if (fp == NULL) {
    print("無法打開文件\n");
    return FALSE;
  }
  int i;
  while (1) {
    char line[1024], buf[128];
    if (fgets(line, sizeof(line), fp) == NULL)
      break;
    chop(line);
    if (line[0] == '#')
      continue;
    getStringFromIndexWithDelim(line, "=", 1, buf, sizeof(buf));
    for (i = 0; i < arraysizeof(CHAR_cmtbl); i++) {
      if (strcmp(buf, CHAR_cmtbl[i].magicname) == 0) {
        if (getStringFromIndexWithDelim(line, "=", 2, buf, sizeof(buf)))
          strcpy(CHAR_cmtbl[i].magicname, buf);
        if (getStringFromIndexWithDelim(line, "=", 3, buf, sizeof(buf)))
          CHAR_cmtbl[i].level = atoi(buf);
        CHAR_cmtbl[i].hash = hashpjw(CHAR_cmtbl[i].magicname);
      }
    }
  }
  fclose(fp);
  return TRUE;
}

#endif

#ifdef _FILTER_TALK
char FilterTalk[100][32];
int FilterTalkNum = 0;
int ReadFilterTalk() {
  FILE *fp;
  fp = fopen("data/filter.txt", "r");
  if (fp == NULL) {
    print("無法打開文件\n");
    return FALSE;
  }
  while (1) {
    char line[1024];
    if (fgets(line, sizeof(line), fp) == NULL)
      break;
    chop(line);
    if (line[0] == '#')
      continue;
    strcpy(FilterTalk[FilterTalkNum], line);

    FilterTalkNum++;
  }
  fclose(fp);
  return TRUE;
}

char *getFilterTalk(int index) {
  if (index > FilterTalkNum)
    index = FilterTalkNum;
  return FilterTalk[index];
}

int getFilterTalkNum() { return FilterTalkNum; }
#endif
