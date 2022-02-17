#include "char.h"
#include "char_base.h"
#include "enemy.h"
#include "lauxlib.h"
#include "lssproto_serv.h"
#include "lua.h"
#include "lualib.h"
#include "map_deal.h"
#include "mylua/base.h"
#include "npc_transmigration.h"
#include "readmap.h"
#include <stdio.h>
#include <string.h>

#ifdef _ALLBLUES_LUA
#ifdef _ALLBLUES_LUA_1_4

static CharBase EnemytempBaseInt[] = {{{"编号"}, E_T_TEMPNO},
                                      {{"初始值"}, E_T_INITNUM},
                                      {{"成长率"}, E_T_LVUPPOINT},
                                      {{"体力"}, E_T_BASEVITAL},
                                      {{"腕力"}, E_T_BASESTR},
                                      {{"耐力"}, E_T_BASETGH},
                                      {{"速度"}, E_T_BASEDEX},
                                      {{"智能"}, E_T_MODAI},
                                      {{"给"}, E_T_GET},
                                      {{"地"}, E_T_EARTHAT},
                                      {{"水"}, E_T_WATERAT},
                                      {{"火"}, E_T_FIREAT},
                                      {{"风"}, E_T_WINDAT},
                                      {{"毒抗"}, E_T_POISON},
                                      {{"麻抗"}, E_T_PARALYSIS},
                                      {{"睡抗"}, E_T_SLEEP},
                                      {{"石抗"}, E_T_STONE},
                                      {{"酒抗"}, E_T_DRUNK},
                                      {{"混抗"}, E_T_CONFUSION},
                                      {{"技能1"}, E_T_PETSKILL1},
                                      {{"技能2"}, E_T_PETSKILL2},
                                      {{"技能3"}, E_T_PETSKILL3},
                                      {{"技能4"}, E_T_PETSKILL4},
                                      {{"技能5"}, E_T_PETSKILL5},
                                      {{"技能6"}, E_T_PETSKILL6},
                                      {{"技能7"}, E_T_PETSKILL7},
                                      {{"罕见"}, E_T_RARE},
                                      {{"暴击率"}, E_T_CRITICAL},
                                      {{"反击率"}, E_T_COUNTER},
                                      {{"技能数"}, E_T_SLOT},
                                      {{"形象"}, E_T_IMGNUMBER},
                                      {{"等级限制"}, E_T_LIMITLEVEL}
#ifdef _PET_FUSION
                                      ,
                                      {{"融合宠编码"}, E_T_FUSIONCODE}
#endif
};

static CharBase EnemytempBaseChar[] = {{{"名字"}, E_T_NAME}};

static int getDataInt(lua_State *L) {
  const int enemybaseid = luaL_checkint(L, 1);
  const int array = ENEMYTEMP_getEnemyTempArrayFromTempNo(enemybaseid);
  const int element = getCharBaseValue(L, 2, EnemytempBaseInt,
                                       arraysizeof(EnemytempBaseInt), 1);

  lua_pushinteger(L, ENEMYTEMP_getInt(array, element));
  return 1;
}

static int getDataChar(lua_State *L) {
  const int enemybaseid = luaL_checkint(L, 1);
  const int array = ENEMYTEMP_getEnemyTempArrayFromTempNo(enemybaseid);
  const int element = getCharBaseValue(L, 2, EnemytempBaseChar,
                                       arraysizeof(EnemytempBaseChar), 1);
  lua_pushstring(L, ENEMYTEMP_getChar(array, element));
  return 1;
}

static int getInt(lua_State *L) {
  const int array = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, EnemytempBaseInt,
                                       arraysizeof(EnemytempBaseInt), 1);

  lua_pushinteger(L, ENEMYTEMP_getInt(array, element));
  return 1;
}

static int getChar(lua_State *L) {
  const int array = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, EnemytempBaseChar,
                                       arraysizeof(EnemytempBaseChar), 1);
  lua_pushstring(L, ENEMYTEMP_getChar(array, element));
  return 1;
}

static int getEnemyTempArray(lua_State *L) {
  const int enemyid = luaL_checkint(L, 1);
  lua_pushinteger(L, ENEMYTEMP_getEnemyTempArray(enemyid));
  return 1;
}

static int getEnemyTempArrayFromTempNo(lua_State *L) {
  const int array = luaL_checkint(L, 1);
  lua_pushinteger(L, ENEMYTEMP_getEnemyTempArrayFromTempNo(array));
  return 1;
}

static int getEnemyTempArrayFromInitnum(lua_State *L) {
  const int array = luaL_checkint(L, 1);
  lua_pushinteger(L, ENEMYTEMP_getEnemyTempArrayFromInitnum(array));
  return 1;
}

static int getEnemyTempNameFromEnemyID(lua_State *L) {
  const int enemyid = luaL_checkint(L, 1);
  int i;
  for (i = 0; i < ENEMY_getEnemyNum(); i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == enemyid) {
      break;
    }
  }
  if (i == ENEMY_getEnemyNum()) {
    lua_pushstring(L, "");
    return 1;
  }
  int tarray = ENEMYTEMP_getEnemyTempArray(i);

  if (!ENEMYTEMP_CHECKINDEX(tarray)) {
    lua_pushstring(L, "");
  } else {
    lua_pushstring(L, ENEMYTEMP_getChar(tarray, E_T_NAME));
  }
  return 1;
}

static int getEnemyTempIDFromEnemyID(lua_State *L) {
  const int enemyid = luaL_checkint(L, 1);
  int i;
  for (i = 0; i < ENEMY_getEnemyNum(); i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == enemyid) {
      break;
    }
  }
  if (i == ENEMY_getEnemyNum()) {
    lua_pushinteger(L, -1);
    return 1;
  }
  lua_pushinteger(L, ENEMY_getInt(i, ENEMY_TEMPNO));
  return 1;
}

static int getEnemyId(lua_State *L) {
  const int enemyindex = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(enemyindex)) {
    lua_pushinteger(L, -1);
    return 1;
  }
  int PetID = CHAR_getInt(enemyindex, CHAR_PETID);
  int enemynum = ENEMY_getEnemyNum();
  int i;
  for (i = 0; i < enemynum; i++) {
    if (ENEMY_getInt(i, ENEMY_TEMPNO) == PetID)
      break;
  }
  if (i == enemynum) {
    lua_pushinteger(L, -1);
    return 1;
  }
  lua_pushinteger(L, ENEMY_getInt(i, ENEMY_ID));
  return 1;
}

static int getEnemyBaseIdFromIndex(lua_State *L) {
  const int petindex = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(petindex)) {
    lua_pushinteger(L, -1);
    return 1;
  }
  lua_pushinteger(L, CHAR_getInt(petindex, CHAR_PETID));
  return 1;
}

static int getEnemyIdFromIndex(lua_State *L) {
  const int enemyindex = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(enemyindex)) {
    lua_pushinteger(L, -1);
    return 1;
  }

  lua_pushinteger(L, CHAR_getInt(enemyindex, CHAR_PETID));
  return 1;
}

static int getEnemyBaseIdFromEnemyId(lua_State *L) {
  const int enemyid = luaL_checkint(L, 1);
  int enemynum = ENEMY_getEnemyNum();
  int i;
  for (i = 0; i < enemynum; i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == enemyid)
      break;
  }
  if (i == enemynum) {
    lua_pushinteger(L, -1);
  }
  lua_pushinteger(L, ENEMY_getInt(i, ENEMY_TEMPNO));
  return 1;
}

static int GetEnemyId(lua_State *L) {
  const int petindex = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(petindex)) {
    lua_pushinteger(L, -1);
    return 0;
  }
  int PetID = CHAR_getInt(petindex, CHAR_PETID);
  int enemynum = ENEMY_getEnemyNum();
  int i;
  for (i = 0; i < enemynum; i++) {
    if (ENEMY_getInt(i, ENEMY_TEMPNO) == PetID)
      break;
  }
  if (i == enemynum) {
    lua_pushinteger(L, -1);
    return 0;
  }
  int ret = ENEMY_getInt(i, ENEMY_ID);
  lua_pushinteger(L, ret);
  return 1;
}

static int GetEnemyBaseId(lua_State *L) {
  const int petindex = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(petindex)) {
    lua_pushinteger(L, -1);
    return 0;
  }
  int ret = CHAR_getInt(petindex, CHAR_PETID);
  lua_pushinteger(L, ret);
  return 1;
}

static int PetTrans(lua_State *L) {
  const int meindex = luaL_checkint(L, 1);
  const int petindex = luaL_checkint(L, 2);
  const int att1 = luaL_checkint(L, 3);
  const int att2 = luaL_checkint(L, 4);
  const int att3 = luaL_checkint(L, 5);
  const int att4 = luaL_checkint(L, 6);
  if (!CHAR_CHECKINDEX(meindex)) {
    lua_pushinteger(L, -1);
    return 0;
  }
  if (!CHAR_CHECKINDEX(petindex)) {
    lua_pushinteger(L, -1);
    return 0;
  }
  int ret = -1;
  if (NPC_PetTransManStatus_Lua(meindex, petindex, att1, att2, att3, att4) ==
      TRUE) {
    ret = 1;
  } else {
    ret = -1;
  }
  lua_pushinteger(L, ret);
  return 1;
}

static int LuaSetNewEgg(lua_State *L) {
  const int meindex = luaL_checkint(L, 1);
  const int petindex = luaL_checkint(L, 2);
  const int petcode = luaL_checkint(L, 3);
  const int petwork = luaL_checkint(L, 4);
  if (!CHAR_CHECKINDEX(meindex)) {
    lua_pushinteger(L, -1);
    return 0;
  }
  if (!CHAR_CHECKINDEX(petindex)) {
    lua_pushinteger(L, -1);
    return 0;
  }
  int ret = 0;
  if (PETFUSION_LuaSetNewEgg(meindex, petindex, petcode, petwork) == -1) {
    ret = 0;
  } else {
    ret = 1;
  }
  lua_pushinteger(L, ret);
  return 1;
}

static const luaL_Reg enemytemplib[] = {
    {"getInt", getInt},
    {"getChar", getChar},
    {"getEnemyTempArray", getEnemyTempArray},
    {"getEnemyTempArrayFromTempNo", getEnemyTempArrayFromTempNo},
    {"getEnemyTempArrayFromInitnum", getEnemyTempArrayFromInitnum},
    {"getEnemyTempNameFromEnemyID", getEnemyTempNameFromEnemyID},
    {"getEnemyTempIDFromEnemyID", getEnemyTempIDFromEnemyID},

    {"getEnemyIDFromIndex", getEnemyIdFromIndex},
    {"getEnemyBaseIdFromIndex", getEnemyBaseIdFromIndex},
    {"getEnemyBaseIdFromEnemyId", getEnemyBaseIdFromEnemyId},
    {"getDataInt", getDataInt},
    {"getDataChar", getDataChar},
    {"GetEnemyId", GetEnemyId},
    {"GetEnemyBaseId", GetEnemyBaseId},
    {"PetTrans", PetTrans},
    {"SetNewEgg", LuaSetNewEgg},
    {NULL, NULL}};

LUALIB_API int luaopen_Enemytemp(lua_State *L) {
  luaL_register(L, "pet", enemytemplib);
  return 1;
}
#endif

#endif
