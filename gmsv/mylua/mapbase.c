#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "lauxlib.h"
#include "lssproto_serv.h"
#include "lua.h"
#include "lualib.h"
#include "map_deal.h"
#include "mylua/base.h"
#include "object.h"
#include "readmap.h"
#include <stdio.h>
#include <string.h>

#ifdef _ALLBLUES_LUA
#ifdef _ALLBLUES_LUA_1_1
static int RandMap(lua_State *L) {
  lua_pushinteger(L, MAP_getfloorId(rand() % MAP_getMapNum()));
  return 1;
}

static int RandXAndY(lua_State *L) {
  const int ff = luaL_checkint(L, 1);
  int fx, fy;
  int i;
  int count = 0;
  lua_newtable(L); //初始化

  for (i = 0; i < 50; i++) {
    fx = rand() % MAP_getfloorX(ff);
    fy = rand() % MAP_getfloorY(ff);
#ifdef _ITEM_PATH
    if (MAP_walkAbleFromPoint(ff, fx, fy, 0, -1) == TRUE) {
      lua_pushinteger(L, 1);
      lua_pushinteger(L, fx);
      lua_settable(L, -3);

      lua_pushinteger(L, 2);
      lua_pushinteger(L, fy);
      lua_settable(L, -3);
    }
#else
    if (MAP_walkAbleFromPoint(ff, fx, fy, 0) == TRUE) {
      lua_pushinteger(L, 1);
      lua_pushinteger(L, fx);
      lua_settable(L, -3);

      lua_pushinteger(L, 2);
      lua_pushinteger(L, fy);
      lua_settable(L, -3);
    }
#endif
  }
  return 1;
}

static int getX(lua_State *L) {
  const int XandY = luaL_checkint(L, 1);
  lua_pushinteger(L, (XandY >> 16) & 0xFFFF);
  return 1;
}

static int getY(lua_State *L) {
  const int XandY = luaL_checkint(L, 1);
  lua_pushinteger(L, XandY & 0xFFFF);
  return 1;
}
static int getfloorX(lua_State *L) {
  const int floorid = luaL_checkint(L, 1);
  lua_pushinteger(L, MAP_getfloorX(floorid));
  return 1;
}

static int getfloorY(lua_State *L) {
  const int floorid = luaL_checkint(L, 1);
  lua_pushinteger(L, MAP_getfloorY(floorid));
  return 1;
}

static int getFloorName(lua_State *L) {
  const int floorid = luaL_checkint(L, 1);

  char escapeshowstring[256];
  char *showstr = MAP_getfloorShowstring(floorid);

  getStringFromIndexWithDelim(showstr, "|", 1, escapeshowstring,
                              sizeof(escapeshowstring));

  lua_pushstring(L, escapeshowstring);
  return 1;
}

static int getCharaindex(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int distance = luaL_checkint(L, 2);
  const int type = luaL_checkint(L, 3);

  int fl, x, y;
  OBJECT object;
  fl = CHAR_getInt(charaindex, CHAR_FLOOR);
  int i;
  for (i = 1; i <= distance; i++) {
    CHAR_getCoordinationDir(CHAR_getInt(charaindex, CHAR_DIR),
                            CHAR_getInt(charaindex, CHAR_X),
                            CHAR_getInt(charaindex, CHAR_Y), i, &x, &y);

    for (object = MAP_getTopObj(fl, x, y); object;
         object = NEXT_OBJECT(object)) {
      int objindex = GET_OBJINDEX(object);
      if (OBJECT_getType(objindex) == OBJTYPE_CHARA) {
        if (CHAR_getInt(OBJECT_getIndex(objindex), CHAR_WHICHTYPE) == type) {
          lua_pushinteger(L, OBJECT_getIndex(objindex));
          return 1;
        }
      }
    }
  }
  lua_pushinteger(L, -1);
  return 1;
}

int getBattleIndex(lua_State *L) {
  const int floorid = luaL_checkint(L, 1);
  lua_assert(BattleArray != NULL);
  int battlenum = getBattlenum();
  int i = 0;
  int count = 0;
  lua_newtable(L); //初始化

  for (i = 0; i < battlenum; i++) {
    BATTLE *TM_Battle = &BattleArray[i];
    if (TM_Battle->use == TRUE) { //有效战斗
      int mapid = CHAR_getInt(TM_Battle->leaderindex, CHAR_FLOOR);
      //找寻到该地图有效战斗
      if (floorid == mapid) {
        count++;
        lua_pushinteger(L, count);
        lua_pushinteger(L, i);
        lua_settable(L, -3);
      }
    }
  }
  return 1;
}

int getFloorPlayer(lua_State *L) {
  const int floorid = luaL_checkint(L, 1);
  int playernum = CHAR_getPlayerMaxNum();
  int i = 0;
  int count = 0;
  lua_newtable(L); //初始化
  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) != FALSE) {
      if (CHAR_getInt(i, CHAR_FLOOR) == floorid) {
        count++;
        lua_pushinteger(L, count);
        lua_pushinteger(L, i);
        lua_settable(L, -3);
      }
    }
  }
  return 1;
}

static const luaL_Reg maplib[] = {{"RandMap", RandMap},
                                  {"RandXAndY", RandXAndY},
                                  {"getfloorX", getfloorX},
                                  {"getfloorY", getfloorY},
                                  {"getFloorName", getFloorName},
                                  {"getX", getX},
                                  {"getY", getY},
                                  {"getCharaindex", getCharaindex},
                                  {"getFloorPlayer", getFloorPlayer},
                                  {"getBattleIndex", getBattleIndex},
                                  {NULL, NULL}};

LUALIB_API int luaopen_Map(lua_State *L) {
  luaL_register(L, "map", maplib);
  return 1;
}

#endif
#endif
