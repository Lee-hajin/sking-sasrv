#include "NewBilu/version.h"
#include "lauxlib.h"
#include "log.h"
#include "lssproto_serv.h"
#include "lua.h"
#include "lualib.h"
#include "net.h"
#include <string.h>

#ifdef _ALLBLUES_LUA

static int endOne(lua_State *L) {
  const int sockfd = luaL_checkint(L, 1);

  CONNECT_endOne_debug(sockfd);

  return 1;
}

static int userip(lua_State *L) {
  int charaindex = luaL_checkint(L, 1);

  int sockfd = getfdFromCharaIndex(charaindex);

  unsigned long ip;
  int a, b, c, d;
  char strIP[32];
  ip = CONNECT_get_userip(sockfd);

  a = (ip % 0x100);
  ip = ip / 0x100;
  b = (ip % 0x100);
  ip = ip / 0x100;
  c = (ip % 0x100);
  ip = ip / 0x100;
  d = (ip % 0x100);

  sprintf(strIP, "%d.%d.%d.%d", a, b, c, d);

  lua_pushstring(L, strIP);
  return 1;
}
#ifdef _NEWCLISETSERVID
static int getServid(lua_State *L) {
  int charaindex = luaL_checkint(L, 1);

  int sockfd = getfdFromCharaIndex(charaindex);

  lua_pushinteger(L, CONNECT_getServid(sockfd));
  return 1;
}
#endif
#ifdef _NEWCLISETMAC
static int getMac(lua_State *L) {
  int sockfd = luaL_checkint(L, 1);

  char mac[128];
  CONNECT_getMAC(sockfd, mac, sizeof(mac));

  lua_pushstring(L, mac);
  return 1;
}
static int getMac2(lua_State *L) {
  int sockfd = luaL_checkint(L, 1);

  char mac[128];
  CONNECT_getMAC2(sockfd, mac, sizeof(mac));

  lua_pushstring(L, mac);
  return 1;
}
static int getMac3(lua_State *L) {
  int sockfd = luaL_checkint(L, 1);

  char mac[128];
  CONNECT_getMAC3(sockfd, mac, sizeof(mac));

  lua_pushstring(L, mac);
  return 1;
}
#endif

static int getIP(lua_State *L) {
  int sockfd = luaL_checkint(L, 1);
  unsigned long ip;
  int a, b, c, d;
  char strIP[32];

  ip = CONNECT_get_userip(sockfd);

  a = (ip % 0x100);
  ip = ip / 0x100;
  b = (ip % 0x100);
  ip = ip / 0x100;
  c = (ip % 0x100);
  ip = ip / 0x100;
  d = (ip % 0x100);

  sprintf(strIP, "%d.%d.%d.%d", a, b, c, d);

  lua_pushstring(L, strIP);
  return 1;
}

static int getUse(lua_State *L) {
  int fd = luaL_checkint(L, 1);

  lua_pushinteger(L, CONNECT_getUse(fd));
  return 1;
}
static int getCharaindex(lua_State *L) {
  int fd = luaL_checkint(L, 1);

  lua_pushinteger(L, CONNECT_getCharaindex(fd));
  return 1;
}

static int getCdkey(lua_State *L) {
  int fd = luaL_checkint(L, 1);
  char cdkey[16];
  CONNECT_getCdkey(fd, cdkey, sizeof(cdkey));

  lua_pushstring(L, cdkey);
  return 1;
}

static int setCloseRequest(lua_State *L) {
  const int fd = luaL_checkint(L, 1);
  const int count = luaL_checkint(L, 2);
  CONNECT_setCloseRequest(fd, count);
  return 1;
}

static int echosend(lua_State *L) {
  const int fd = luaL_checkint(L, 1);
  lssproto_Echo_recv(fd, "!!!!");
  return 1;
}

static int getSocketType(lua_State *L) {
  int fd = luaL_checkint(L, 1);

  lua_pushinteger(L, CONNECT_getState(fd));
  return 1;
}
static const luaL_Reg netlib[] = {{"endOne", endOne},
                                  {"userip", userip},
#ifdef _NEWCLISETSERVID
                                  {"getServid", getServid},
#endif
#ifdef _NEWCLISETMAC
                                  {"getMac", getMac},
                                  {"getMac2", getMac2},
                                  {"getMac3", getMac3},
#endif
                                  {"getIP", getIP},
                                  {"getUse", getUse},
                                  {"getCharaindex", getCharaindex},
                                  {"getCdkey", getCdkey},
                                  {"getSocketType", getSocketType},
                                  {"setCloseRequest", setCloseRequest},
                                  {"echosend", echosend},
                                  {NULL, NULL}};

LUALIB_API int luaopen_Net(lua_State *L) {
  luaL_register(L, "net", netlib);
  return 1;
}

#endif
