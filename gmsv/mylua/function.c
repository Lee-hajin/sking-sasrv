#include "NewBilu/version.h"
#include "battle.h"
#include "char_base.h"
#include "common.h"
#include "configfile.h"
#include "item.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "mylua/base.h"
#include "mylua/mylua.h"
#include <string.h>

#ifdef _ALLBLUES_LUA
extern MY_Lua MYLua;

lua_State *FindLua(char *filename) {
  MY_Lua *mylua = &MYLua;
  char newfilename[256];
  while (mylua->next != NULL) {
    sprintf(newfilename, "%s", filename);

    if (strcmp(newfilename, mylua->luapath) == 0) {
      return mylua->lua;
    }
    mylua = mylua->next;
  }

  return NULL;
}

#ifdef _ALLBLUES_LUA_1_9
BOOL BattleCommand(int charaindex, int battleindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/battlecommand.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "BattleCommand");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, battleindex);
  docall(lua, 2, 1);

  return TRUE;
}

int FreeItemInslay(int charaindex, int inslayindex, int itemindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/iteminslay.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeItemInslay");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, inslayindex);
  lua_pushnumber(lua, itemindex);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

#ifdef _LOCK_PET_ITEM
#ifdef _CTRL_TRANS_DEVELOP
int FreeCtrlTransDevelop(int toindex, int petindex, int type) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/ctrltransdevelop.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeCtrlTransDevelop");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, toindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, type);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#endif

int FreeStreetVendorPet(int charaindex, int petindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/streetvendor.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeStreetVendorPet");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeStreetVendorItem(int charaindex, int itemindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/streetvendor.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeStreetVendorItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeStreetVendorName(int charaindex, char *data) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/streetvendor.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeStreetVendorName");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushstring(lua, data);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeTradePet(int charaindex, int petindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/trade.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeTradePet");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeTradeItem(int charaindex, int itemindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/trade.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeTradeItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeCheckPetLog(int charaindex, int petindex, char *CharID,
                    char *uniquecode) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/CheckLog.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "CheckPetLog");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入5个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petindex);
  lua_pushstring(lua, CharID);
  lua_pushstring(lua, uniquecode);

  if (lua_pcall(lua, 4, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);
  return ret;
}

int FreeCheckItemLog(int charaindex, int itemindex, int itemID, char *CharID,
                     char *uniquecode) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/CheckLog.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "CheckItemLog");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入5个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);
  lua_pushnumber(lua, itemID);
  lua_pushstring(lua, CharID);
  lua_pushstring(lua, uniquecode);

  if (lua_pcall(lua, 5, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);
  return ret;
}

int FreeLockPetSave(int charaindex, int petindex, int pooltype) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/lockpoolsave.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeLockPetSave");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, pooltype);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeLockItemSave(int charaindex, int itemindex, int pooltype) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/lockpoolsave.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeLockItemSave");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);
  lua_pushnumber(lua, pooltype);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

#endif

#ifdef _FIRST_LOCK_ITEM
void FreeFirstLockPet(int charaindex, int petindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/firstlock.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeFirstLockPet");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petindex);

  docall(lua, 2, 1);
}
void FreeFirstLockItem(int charaindex, int itemindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/firstlock.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeFirstLockItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  docall(lua, 2, 1);
}
#endif

void FreeCharExpSave(int charindex, int getexp, int addexp) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/charexpsave.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeCharExpSave");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charindex);
  lua_pushnumber(lua, getexp);
  lua_pushnumber(lua, addexp);

  docall(lua, 3, 1);
}
#ifdef _PET_FUSION_LUA
int FreeCheckFusion(int MainIndex, int Subindex1, int Subindex2
#ifdef _PET_FUSION_2
                    ,
                    int fusion_type
#endif
) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/checkfusion.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeCheckFusion");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, MainIndex);
  lua_pushnumber(lua, Subindex1);
  lua_pushnumber(lua, Subindex2);
#ifdef _PET_FUSION_2
  lua_pushnumber(lua, fusion_type);
  if (lua_pcall(lua, 4, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));
#else
  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));
#endif

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeFusionSkill(int charaindex, int petskill) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/checkfusion.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeFusionSkill");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petskill);
  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#endif

BOOL FreeAction(int charaindex, int x, int y, int actionno) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/action.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeAction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, x);
  lua_pushnumber(lua, y);
  lua_pushnumber(lua, actionno);
  docall(lua, 4, 1);

  return TRUE;
}

int FreeModeExp(int charaindex, int getexp, int modexp) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/modeexp.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeModeExp");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入3个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, getexp);
  lua_pushnumber(lua, modexp);
  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

int FreeModeFame(int charaindex, int exp2, int feedpoint) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/modefame.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeModeFame");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入3个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, exp2);
  lua_pushnumber(lua, feedpoint);
  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeSaMenu(int charaindex, int toindex, int index1, int index2,
                int index3) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/samenu.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeSaMenu");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, toindex);
  lua_pushnumber(lua, index1);
  lua_pushnumber(lua, index2);
  lua_pushnumber(lua, index3);
  docall(lua, 5, 1);

  return TRUE;
}

#ifdef _DATA_INFO_SHOW
BOOL FreeDataInfoShow(int charaindex) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "FreeFameShow");
    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    //依次放入二个参数
    lua_pushnumber(mylua->lua, charaindex);

    if (lua_pcall(mylua->lua, 1, 1, 0) != 0)
      error(mylua->lua, "error running function `f': %s",
            lua_tostring(mylua->lua, -1));

    if (!lua_isnumber(mylua->lua, -1))
      error(mylua->lua, "function `f' must return a number");

    int ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == FALSE) {
      return FALSE;
    }
    mylua = mylua->next;
  }
  return TRUE;
}
#endif

#ifdef _FAME_REG_TIME
BOOL FreeFameFeatures(int charaindex, int kind, int flg) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/famefeatures.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeFameFeatures");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, kind);
  lua_pushnumber(lua, flg);

  docall(lua, 3, 1);

  return TRUE;
}
#endif
BOOL FreeComplianceParameter(int charaindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/complianceparameter.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeComplianceParameter");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);

  docall(lua, 1, 1);

  return TRUE;
}

BOOL FreeLoginCheck(int fd, char *cdkey, char *passwd, char *ip, char *mac1,
                    char *mac2, char *mac3) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/logincheck.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeLoginCheck");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, fd);
  lua_pushstring(lua, cdkey);
  lua_pushstring(lua, passwd);
  lua_pushstring(lua, ip);
  lua_pushstring(lua, mac1);
  lua_pushstring(lua, mac2);
  lua_pushstring(lua, mac3);

  if (lua_pcall(lua, 7, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeCharLogout(int charaindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/charlogout.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeCharLogout");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  docall(lua, 1, 1);

  return TRUE;
}

BOOL EquipEffectFunction(int charaindex, int id) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/equipeffectfunction.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "EquipEffectFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, id);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

#ifdef _ITEM_OVER_LAP
BOOL ItemOverlapFunction(int charindex, int fromitemindex, int toitemindex,
                         int fromid, int toid) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/itemlvup.lua");
    if (lua == NULL)
      return FALSE;
  }
  lua_getglobal(lua, "ItemOverlapFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charindex);
  lua_pushnumber(lua, fromitemindex);
  lua_pushnumber(lua, toitemindex);
  lua_pushnumber(lua, fromid);
  lua_pushnumber(lua, toid);

  if (lua_pcall(lua, 5, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

BOOL BattleFinishFunction(int charaindex, int battletime, int battleturn,
                          int battletype) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/battlefinishfunction.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "BattleFinishFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, battletime);
  lua_pushnumber(lua, battleturn);
  lua_pushnumber(lua, battletype);

  docall(lua, 4, 1);

  return TRUE;
}

BOOL SetBattleEnmeyFunction(int meindex, int enemyindex, int id) {
  lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_BATTLESETFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, enemyindex);
  lua_pushnumber(lua, id);
  docall(lua, 3, 1);

  return TRUE;
}

#endif

#ifdef _ALLBLUES_LUA_1_8
BOOL CaptureOkFunction(int attackindex, int defindex) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "CaptureOkFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    //依次放入二个参数
    lua_pushnumber(mylua->lua, attackindex);
    lua_pushnumber(mylua->lua, defindex);

    docall(mylua->lua, 2, 1);
    mylua = mylua->next;
  }

  return TRUE;
}

BOOL CaptureCheckFunction(int attackindex, int defindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/capturefunction.lua");
    if (lua == NULL)
      return 1;
  }

  lua_getglobal(lua, "CaptureCheckFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return 1;
  }

  //依次放入二个参数
  lua_pushnumber(lua, attackindex);
  lua_pushnumber(lua, defindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _ALLBLUES_LUA_1_7
BOOL CharVsEnemyFunction(int charaindex) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "CharVsEnemyFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }

    lua_pushnumber(mylua->lua, charaindex);

    if (lua_pcall(mylua->lua, 1, 1, 0) != 0)
      error(mylua->lua, "error running function `f': %s",
            lua_tostring(mylua->lua, -1));

    if (!lua_isnumber(mylua->lua, -1))
      error(mylua->lua, "function `f' must return a number");

    int ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == TRUE) {
      return TRUE;
    }

    mylua = mylua->next;
  }

  return FALSE;
}
#endif

#ifdef _ALLBLUES_LUA_1_6
BOOL CharTalkFunction2(int charaindex, char *message, int color) {
  MY_Lua *mylua = &MYLua;
  int ret = 1;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "CharTalkFunction2");
    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }

    //依次放入参数
    lua_pushnumber(mylua->lua, charaindex);
    lua_pushstring(mylua->lua, message);
    lua_pushnumber(mylua->lua, color);

    if (lua_pcall(mylua->lua, 3, 1, 0) != 0)
      error(mylua->lua, "error running function `f': %s",
            lua_tostring(mylua->lua, -1));

    if (!lua_isnumber(mylua->lua, -1))
      error(mylua->lua, "function `f' must return a number");

    ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == TRUE) {
      return ret;
    }
    mylua = mylua->next;
  }
  return ret;
}

BOOL CharTalkFunction(int charaindex, char *message, int color) {
  MY_Lua *mylua = &MYLua;
  int ret = 1;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "CharTalkFunction");
    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }

    //依次放入参数
    lua_pushnumber(mylua->lua, charaindex);
    lua_pushstring(mylua->lua, message);
    lua_pushnumber(mylua->lua, color);

    if (lua_pcall(mylua->lua, 3, 1, 0) != 0)
      error(mylua->lua, "error running function `f': %s",
            lua_tostring(mylua->lua, -1));

    if (!lua_isnumber(mylua->lua, -1))
      error(mylua->lua, "function `f' must return a number");

    ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == TRUE) {
      return ret;
    }
    mylua = mylua->next;
  }
  return ret;
}

BOOL BattleWatch(int charaindex, int toindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/battlewatch.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "BattleWatch");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, toindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

BOOL FamilyRideFunction(int meindex, int petindex, int petid) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/familyridefunction.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FamilyRideFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, petid);

  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _ALLBLUES_LUA_1_5
BOOL NetLoopFunction(void) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "NetLoopFunction");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    docall(mylua->lua, 0, 1);
    mylua = mylua->next;
  }

  return TRUE;
}

BOOL FreeCharCreate(int charaindex, int hometown, int clifd) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/charcreate.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeCharCreate");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, hometown);
  lua_pushnumber(lua, clifd);

  docall(lua, 3, 1);

  return TRUE;
}

BOOL FreeCharDelete(int fd, int fdid, char *cdkey) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/chardelete.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeCharDelete");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  int charaindex = CONNECT_getCharaindex(fd);
  //依次放入二个参数
  lua_pushnumber(lua, fd);
  lua_pushnumber(lua, fdid);
  lua_pushstring(lua, cdkey);

  docall(lua, 3, 1);

  return TRUE;
}

BOOL FreeCharLogin(int charaindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/charlogin.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeCharLogin");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入二个参数
  lua_pushnumber(lua, charaindex);

  docall(lua, 1, 1);

  return TRUE;
}

BOOL FreeVsPlayer(int charaindex, int toindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/vsplayer.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeVsPlayer");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, toindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

BOOL FreePartyJoin(int charaindex, int toindex) {
  MY_Lua *mylua = &MYLua;
  while (mylua->lua != NULL) {
    lua_getglobal(mylua->lua, "FreePartyJoin");

    if (!lua_isfunction(mylua->lua, -1)) {
      lua_pop(mylua->lua, 1);
      mylua = mylua->next;
      continue;
    }
    //依次放入二个参数
    lua_pushnumber(mylua->lua, charaindex);
    lua_pushnumber(mylua->lua, toindex);

    if (lua_pcall(mylua->lua, 2, 1, 0) != 0)
      error(mylua->lua, "error running function `f': %s",
            lua_tostring(mylua->lua, -1));

    if (!lua_isnumber(mylua->lua, -1))
      error(mylua->lua, "function `f' must return a number");

    int ret = lua_tonumber(mylua->lua, -1);
    lua_pop(mylua->lua, 1);
    if (ret == FALSE) {
      return FALSE;
    }

    mylua = mylua->next;
  }

  return TRUE;
}
#endif

#ifdef _ALLBLUES_LUA_1_4

BOOL RunCharLogOutEvent(int charaindex) {
  lua_State *lua = CHAR_getLUAFunction(charaindex, CHAR_LOGINOUTFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);

  docall(lua, 1, 1);

  return TRUE;
}

BOOL BattleFinish(int battleindex, int charaindex) {
  lua_State *lua = BATTLE_getLUAFunction(battleindex, BATTLE_FINISH);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, battleindex);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL BattleEscape(int battleindex, int charaindex) {
  lua_State *lua = BATTLE_getLUAFunction(battleindex, BATTLE_ESCAPE);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);

  docall(lua, 1, 1);

  return TRUE;
}
#endif

#ifdef _OFFLINE_SYSTEM
BOOL OffLineCommand(int battleindex, int charaindex, int side) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/offlinecommand.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "OffLineCommand");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, battleindex);
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, side);

  docall(lua, 3, 1);

  return TRUE;
}
#endif

#ifdef _PETSKILL_SHOP_LUA
BOOL FreePetSkillShop(int talkerindex, int petindex, int oldSkillID,
                      int newSkillID) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/petskillshop.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreePetSkillShop");

  if (!lua_isfunction(lua, -1)) {
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, talkerindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, oldSkillID);
  lua_pushnumber(lua, newSkillID);

  if (lua_pcall(lua, 4, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

BOOL FreeWarpMap(int charaindex, int floorid, int x, int y) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/warpmap.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeWarpMap");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, floorid);
  lua_pushnumber(lua, x);
  lua_pushnumber(lua, y);

  if (lua_pcall(lua, 4, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeDropItem(int charaindex, int itemindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/checkdrop.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeDropItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeDropPet(int charaindex, int havepetindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/checkdrop.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeDropPet");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, havepetindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

BOOL FreeDropGold(int charaindex, int amount) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/checkdrop.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeDropGold");
  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, amount);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

#ifdef _LUA_CHECKPETMAIL
BOOL FreePetMail(int charaindex, int toindex, int havepetindex,
                 int haveitemindex) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/checkpetmail.lua");
    if (lua == NULL)
      return TRUE;
  }
  lua_getglobal(lua, "FreePetMail");
  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入4个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, toindex);
  lua_pushnumber(lua, havepetindex);
  lua_pushnumber(lua, haveitemindex);

  if (lua_pcall(lua, 4, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1); //取回回调信息
  lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _LUA_CHECKPARTYOUT
BOOL FreeDischargeParty(int charaindex, int msgflg) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/checkdischargepartyl.lua");
    if (lua == NULL)
      return TRUE;
  }
  lua_getglobal(lua, "FreeDischargeParty");
  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入4个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, msgflg);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1); //取回回调信息
  lua_pop(lua, 1);
  return ret;
}
#endif

BOOL FreeDropPetFollow(int charaindex, int havepetindex) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/droppetfollow.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeDropPetFollow");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, havepetindex);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}

#ifdef _ALLBLUES_LUA_1_2
BOOL RunUseChatMagic(int charaindex, char *data, lua_State *lua) {
  if (lua == NULL)
    return FALSE;

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushstring(lua, data);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemUseEvent(int itemindex, int charaindex, int toindex,
                     int haveitemindex) {
  lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_USEFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入四个参数
  lua_pushnumber(lua, itemindex);
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, toindex);
  lua_pushnumber(lua, haveitemindex);

  docall(lua, 4, 1);

  return TRUE;
}

BOOL RunItemDieReLifeEvent(int charaindex, int itemindex, int haveitemindex) {
  lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_DIERELIFEFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);
  lua_pushnumber(lua, haveitemindex);

  docall(lua, 3, 1);

  return TRUE;
}

BOOL RunItemDetachEvent(int charaindex, int itemindex) {
  lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_DETACHFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemAttachEvent(int charaindex, int itemindex) {
  lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_ATTACHFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPickupEvent(int charaindex, int itemindex) {
  lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_PICKUPFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPostOverEvent(int itemindex, int charaindex) {
  lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_POSTOVERFUNC);
  if (lua == NULL) {
    return FALSE;
  }
  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPreOverEvent(int itemindex, int charaindex) {
  lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_PREOVERFUNC);
  if (lua == NULL) {
    return FALSE;
  }
  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemDropEvent(int charaindex, int itemindex) {
  lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_DROPFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, itemindex);

  docall(lua, 2, 1);

  return TRUE;
}

#endif

BOOL RunCharTalkedEvent(int meindex, int toindex, char *messageeraseescape,
                        int color, int channel) {
  lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_TALKEDFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入五个参数
  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, toindex);
  lua_pushstring(lua, messageeraseescape);
  lua_pushnumber(lua, color);
  lua_pushnumber(lua, channel);

  docall(lua, 5, 1);

  return TRUE;
}

BOOL RunCharLoopEvent(int meindex) {
  lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_LOOPFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入一个参数
  lua_pushnumber(lua, meindex);
  docall(lua, 1, 1);

  return TRUE;
}

BOOL RunCharOverlapEvent(int meindex, int toindex) {
  lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_OVERLAPEDFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, toindex);
  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunCharBattleOverEvent(int meindex, int battleindex, int iswin) {
  lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_BATTLEOVERDFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入三个参数
  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, battleindex);
  lua_pushnumber(lua, iswin);
  docall(lua, 3, 1);

  return TRUE;
}

BOOL RunCharWindowTalked(int meindex, int talkerindex, int seqno, int select,
                         char *data) {
  lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_WINDOWTALKEDFUNC);
  if (lua == NULL) {
    return FALSE;
  }

  //依次放入五个参数
  lua_pushnumber(lua, meindex);
  lua_pushnumber(lua, talkerindex);
  lua_pushnumber(lua, seqno);
  lua_pushnumber(lua, select);
  lua_pushstring(lua, data);

  docall(lua, 5, 1);

  return TRUE;
}

#ifdef _LUA_BATTLEOVERCALL
BOOL FreeBattleOverCall(int charaindex, int alive, int battletime,
                        int battleturn, int battletype) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/battleover.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "FreeBattleOver");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, alive);
  lua_pushnumber(lua, battletime);
  lua_pushnumber(lua, battleturn);
  lua_pushnumber(lua, battletype);
  docall(lua, 5, 1);

  return TRUE;
}
#endif

#ifdef _LUA_PROFESSIONSKILLCALL
int professionCallLua(int charaindex, int skillid, int Pskillid) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/professionskill.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeCheckProfessionSkill");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, skillid);
  lua_pushnumber(lua, Pskillid);

  if (lua_pcall(lua, 3, 1, 0) != 0) {
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));
  }

  if (!lua_isnumber(lua, -1)) {
    error(lua, "function `f' must return a number");
  }

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#endif

#ifdef _NB_LUA_MovePetItem
BOOL FreeMovePetItem(int charaindex, int petindex, int charitemindex,
                     int petitemindex, int toindex, int fromindex) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/movepetitem.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeMovePetItem");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, charitemindex);
  lua_pushnumber(lua, petitemindex);
  lua_pushnumber(lua, toindex);
  lua_pushnumber(lua, fromindex);

  if (lua_pcall(lua, 6, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
#endif

#ifdef _NB_LUA_BattlePetSkill
BOOL FreeBattleSkillMode(int charaindex, int petindex, int petskillindex,
                         int petskillid) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/BattleSkillMode.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "FreeBattleSkillMode");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petindex);
  lua_pushnumber(lua, petskillindex);
  lua_pushnumber(lua, petskillid);

  if (lua_pcall(lua, 4, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = 0;
  ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);

  return ret;
}
#endif

#ifdef _NB_LUA_MAGIC_MP_RATE
BOOL FreeMagicMpRate(int charaindex, int marray, int mp) {
  static lua_State *lua;
  if (lua == NULL) {
    lua = FindLua("mylua/function/FreeMagicMpRate.lua");
    if (lua == NULL)
      return TRUE;
  }
  lua_getglobal(lua, "FreeMagicMpRate");
  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入3个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, marray);
  lua_pushnumber(lua, mp);
  if (lua_pcall(lua, 3, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}
#endif

BOOL TransOverFunction(int meindex) //转生后处理回调
{
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/function/TransOverFunction.lua");
    if (lua == NULL)
      return FALSE;
  }

  lua_getglobal(lua, "TransOverFunction");

  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, meindex);

  if (lua_pcall(lua, 1, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);
  lua_pop(lua, 1);
  return ret;
}

#endif

int GetEvolutionPet(int charaindex, int petid) {
  static lua_State *lua;

  if (lua == NULL) {
    lua = FindLua("mylua/chaos/evolution.lua");
    if (lua == NULL)
      return TRUE;
  }

  lua_getglobal(lua, "GetEvolution");
  if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入一个参数
  lua_pushnumber(lua, charaindex);
  lua_pushnumber(lua, petid);

  if (lua_pcall(lua, 2, 1, 0) != 0)
    error(lua, "error running function `f': %s", lua_tostring(lua, -1));

  if (!lua_isnumber(lua, -1))
    error(lua, "function `f' must return a number");

  int ret = lua_tonumber(lua, -1);

  lua_pop(lua, 1);

  return ret;
}
