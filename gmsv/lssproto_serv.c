#include "lssproto_serv.h"
#include "autil.h"
#include "battle.h"
#include "char.h"
#include "common.h" // for StoneAge
#include "configfile.h"
#include "descrypt.h"
#include "log.h" // for StoneAge
#include "net.h"
#include "object.h"
#include "readmap.h"
#include "version.h"
#include "NewBilu/version.h"
#include <net.h>
#include <stdio.h>
#include <time.h>

#ifdef _ABSOLUTE_DEBUG
extern char charId[32];
extern char errordata[256];
extern int lastfunctime;
#endif

void lssproto_SetServerLogFiles(char *r, char *w) {
  lssproto_strcpysafe(lssproto_writelogfilename, w,
                      sizeof(lssproto_writelogfilename));
  lssproto_strcpysafe(lssproto_readlogfilename, r,
                      sizeof(lssproto_readlogfilename));
}
int lssproto_InitServer(int (*writefunc)(int, char *, int), int worksiz) {
  int i;
  if ((void *)writefunc == NULL) {
    lssproto.write_func = lssproto_default_write_wrap;
  } else {
    lssproto.write_func = writefunc;
  }
  lssproto_AllocateCommonWork(worksiz);
  lssproto_stringwrapper = (char **)calloc(1, sizeof(char *) * MAXLSRPCARGS);
  if (lssproto_stringwrapper == NULL)
    return -1;
  memset(lssproto_stringwrapper, 0, sizeof(char *) * MAXLSRPCARGS);
  for (i = 0; i < MAXLSRPCARGS; i++) {
    lssproto_stringwrapper[i] = (char *)calloc(1, worksiz);
    if (lssproto_stringwrapper[i] == NULL) {
      for (i = 0; i < MAXLSRPCARGS; i++) {
        free(lssproto_stringwrapper[i]);
        return -1;
      }
    }
  }
  lssproto.ret_work = (char *)calloc(1, sizeof(worksiz));
  if (lssproto.ret_work == NULL) {
    return -1;
  }
  return 0;
}

void lssproto_CleanupServer(void) {
  int i;
  free(lssproto.work);
  free(lssproto.arraywork);
  free(lssproto.escapework);
  free(lssproto.val_str);
  free(lssproto.token_list);
  for (i = 0; i < MAXLSRPCARGS; i++) {
    free(lssproto_stringwrapper[i]);
  }
  free(lssproto_stringwrapper);
  free(lssproto.ret_work);
}

#define DME() print("<DME(%d)%d:%d>", fd, __LINE__, func)
extern int cliretfunc;
// FILE *fshanzhu;//山猪测试 封包函数头
int lssproto_ServerDispatchMessage(int fd, char *encoded) {
  int func, fieldcount;
  char raw[1024 * 64];
  memset(raw, 0, sizeof(raw));

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  util_DecodeMessage(raw, encoded);
#ifdef _DEBUG_RET
  print("\nraw=%s\n", raw);
#endif
  if (!util_SplitMessage(raw, SEPARATOR)) {
    // print("\nDME1:package=%s\n",raw);
    util_DiscardMessage();
    // DME();
    return -1;
  }

  if (!util_GetFunctionFromSlice(&func, &fieldcount)) {
    util_DiscardMessage();
    logHack(fd, HACK_GETFUNCFAIL);
    // DME();
    return -1;
  }
  /*
  if (func != 71 && func != 79 && func != 77 && func != 73 && func !=
  75){//登录前封包 全部可执行，其他封包检查是否已经有玩家在线！ if(
  !CHAR_CHECKINDEX( CONNECT_getCharaindex(fd) )
  ){//并没有登入人物，仅仅是一个连接！ util_DiscardMessage();
                  print("仅仅是一个连接？func:%d\n",func);
                  logHack(fd,HACK_NOTDISPATCHED);
                  //close(fd);//断开玩家之间的连接
                  return -1;
          }
  }*/

  cliretfunc = func;
#ifdef _ABSOLUTE_DEBUG
  CONNECT_getCdkey(fd, charId, sizeof(charId));
  lastfunctime = 3;
#endif

#ifdef _DEBUG_RET
  printf("\n클라이언트 인터페이스 =%d\n", func);
#endif
  // int t;//山猪测试 封包函数头
  // t = time(NULL);  /* get current time */
  // char filename[256];
  // snprintf(filename,sizeof(filename), "%d",getFunctionfile());//山猪测试
  // 封包函数头 fshanzhu=fopen(filename, "a+");	// create new char tmp[24];
  // sprintf(tmp, "%d\n",func);
  // fwrite(tmp, strlen(tmp), 1, fshanzhu);
  // fclose(fshanzhu);

#ifdef NEWDISPATC
  switch (func) {
#endif

    // if (func==LSSPROTO_W_RECV)//原始的换成宏定义
#ifdef NEWDISPATC
  case LSSPROTO_W_RECV:
#else
  if (func == LSSPROTO_W_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    char direction[1024 * 64];

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_destring(fd, 4, direction);
    util_deint(fd, 5, &checksumrecv);

    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_W_RECV-x:%d,y:%d,direction:%s\n", x, y, direction);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%s", x, y, direction);
#endif

    lssproto_W_recv(fd, x, y, direction);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_W2_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_W2_RECV:
#else
  if (func == LSSPROTO_W2_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    char direction[1024 * 64];

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_destring(fd, 4, direction);
    util_deint(fd, 5, &checksumrecv);

    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }

#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_W2_RECV-x:%d,y:%d,direction:%s\n", x, y, direction);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%s", x, y, direction);
#endif
    lssproto_W2_recv(fd, x, y, direction);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_EV_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_EV_RECV:
#else
  if (func == LSSPROTO_EV_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int event;
    int seqno;
    int x;
    int y;
    int dir;

    checksum += util_deint(fd, 2, &event);
    checksum += util_deint(fd, 3, &seqno);
    checksum += util_deint(fd, 4, &x);
    checksum += util_deint(fd, 5, &y);
    checksum += util_deint(fd, 6, &dir);
    util_deint(fd, 7, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_EV_RECV-event:%d,seqno:%d,x:%d,y:%d,dir:%d\n", event,
           seqno, x, y, dir);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%d:%d", event, seqno, x, y, dir);
#endif

    lssproto_EV_recv(fd, event, seqno, x, y, dir);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_DU_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_DU_RECV:
#else
  if (func == LSSPROTO_DU_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_DU_RECV-x:%d,y:%d\n", x, y);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d", x, y);
#endif
    lssproto_DU_recv(fd, x, y);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_EO_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_EO_RECV:
#else
  if (func == LSSPROTO_EO_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int dummy;

    checksum += util_deint(fd, 2, &dummy);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_EO_RECV-dummy:%d\n", dummy);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", dummy);
#endif
    lssproto_EO_recv(fd, dummy);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_BU_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_BU_RECV:
#else
  if (func == LSSPROTO_BU_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int dummy;

    checksum += util_deint(fd, 2, &dummy);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_BU_RECV-dummy:%d\n", dummy);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", dummy);
#endif
    lssproto_BU_recv(fd, dummy);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_JB_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_JB_RECV:
#else
  if (func == LSSPROTO_JB_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_JB_RECV-x:%d,y:%d\n", x, y);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d", x, y);
#endif
    lssproto_JB_recv(fd, x, y);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_LB_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_LB_RECV:
#else
  if (func == LSSPROTO_LB_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_LB_RECV-x:%d,y:%d\n", x, y);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d", x, y);
#endif
    lssproto_LB_recv(fd, x, y);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_B_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_B_RECV:
#else
  if (func == LSSPROTO_B_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char command[1024 * 64];

    checksum += util_destring(fd, 2, command);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_B_RECV-command:%s\n", command);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", command);
#endif
    lssproto_B_recv(fd, command);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_SKD_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_SKD_RECV:
#else
  if (func == LSSPROTO_SKD_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int dir;
    int index;

    checksum += util_deint(fd, 2, &dir);
    checksum += util_deint(fd, 3, &index);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_SKD_RECV-dir:%d,index:%d\n", dir, index);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d", dir, index);
#endif
    lssproto_SKD_recv(fd, dir, index);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_ID_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_ID_RECV:
#else
  if (func == LSSPROTO_ID_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int haveitemindex;
    int toindex;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &haveitemindex);
    checksum += util_deint(fd, 5, &toindex);
    util_deint(fd, 6, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_ID_RECV-x:%d,y:%d,haveitemindex:%d,toindex:%d\n", x,
           y, haveitemindex, toindex);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%d", x, y, haveitemindex, toindex);
#endif
    lssproto_ID_recv(fd, x, y, haveitemindex, toindex);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_PI_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PI_RECV:
#else
  if (func == LSSPROTO_PI_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int dir;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &dir);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PI_RECV-x:%d,y:%d,dir:%d\n", x, y, dir);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d", x, y, dir);
#endif
    lssproto_PI_recv(fd, x, y, dir);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_DI_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_DI_RECV:
#else
  if (func == LSSPROTO_DI_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int itemindex;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &itemindex);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_DI_RECV-x:%d,y:%d,itemindex:%d\n", x, y, itemindex);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d", x, y, itemindex);
#endif
    lssproto_DI_recv(fd, x, y, itemindex);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_DG_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_DG_RECV:
#else
  if (func == LSSPROTO_DG_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int amount;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &amount);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_DG_RECV-x:%d,y:%d,amount:%d\n", x, y, amount);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d", x, y, amount);
#endif
    lssproto_DG_recv(fd, x, y, amount);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_DP_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_DP_RECV:
#else
  if (func == LSSPROTO_DP_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int petindex;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &petindex);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_DP_RECV-x:%d,y:%d,petindex:%d\n", x, y, petindex);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d", x, y, petindex);
#endif
    lssproto_DP_recv(fd, x, y, petindex);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_MI_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_MI_RECV:
#else
  if (func == LSSPROTO_MI_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int fromindex;
    int toindex;

    checksum += util_deint(fd, 2, &fromindex);
    checksum += util_deint(fd, 3, &toindex);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_MI_RECV-fromindex:%d,toindex:%d\n", fromindex,
           toindex);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d", fromindex, toindex);
#endif
    lssproto_MI_recv(fd, fromindex, toindex);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_MSG_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_MSG_RECV:
#else
  if (func == LSSPROTO_MSG_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int index;
    char message[1024 * 64];
    int color;

    checksum += util_deint(fd, 2, &index);
    checksum += util_destring(fd, 3, message);
    checksum += util_deint(fd, 4, &color);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_MSG_RECV-index:%d,message:%s,color:%d\n", index,
           message, color);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%s:%d", index, message, color);
#endif
    lssproto_MSG_recv(fd, index, message, color);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_PMSG_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PMSG_RECV:
#else
  if (func == LSSPROTO_PMSG_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int index;
    int petindex;
    int itemindex;
    char message[1024 * 64];
    int color;

    checksum += util_deint(fd, 2, &index);
    checksum += util_deint(fd, 3, &petindex);
    checksum += util_deint(fd, 4, &itemindex);
    checksum += util_destring(fd, 5, message);
    checksum += util_deint(fd, 6, &color);
    util_deint(fd, 7, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PMSG_RECV-index:%d,petindex:%d,itemindex:%d,message:"
           "%s,color:%d\n",
           index, petindex, itemindex, message, color);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%s:%d", index, petindex, itemindex, message,
            color);
#endif
    lssproto_PMSG_recv(fd, index, petindex, itemindex, message, color);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_AB_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_AB_RECV:
#else
  if (func == LSSPROTO_AB_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    util_deint(fd, 2, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_AB_RECV\n");
#endif
#ifdef _ABSOLUTE_DEBUG
    strcpy(errordata, "");
#endif
    lssproto_AB_recv(fd);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_DAB_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_DAB_RECV:
#else
  if (func == LSSPROTO_DAB_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int index;

    checksum += util_deint(fd, 2, &index);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_DAB_RECV-index:%d\n", index);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", index);
#endif
    lssproto_DAB_recv(fd, index);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_AAB_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_AAB_RECV:
#else
  if (func == LSSPROTO_AAB_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_AAB_RECV-x:%d,y:%d\n", x, y);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d", x, y);
#endif
    lssproto_AAB_recv(fd, x, y);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_L_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_L_RECV:
#else
  if (func == LSSPROTO_L_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int dir;

    checksum += util_deint(fd, 2, &dir);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_L_RECV-dir:%d\n", dir);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", dir);
#endif
    lssproto_L_recv(fd, dir);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_TK_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_TK_RECV:
#else
  if (func == LSSPROTO_TK_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    char message[1024 * 64];
    int color;
    int area;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_destring(fd, 4, message);
    checksum += util_deint(fd, 5, &color);
    checksum += util_deint(fd, 6, &area);
    util_deint(fd, 7, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_TK_RECV-x:%d,y:%d,message:%s,color:%d,area:%d\n", x,
           y, message, color, area);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%s:%d:%d", x, y, message, color, area);
#endif
    lssproto_TK_recv(fd, x, y, message, color, area);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_M_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_M_RECV:
#else
  if (func == LSSPROTO_M_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int fl;
    int x1;
    int y1;
    int x2;
    int y2;

    checksum += util_deint(fd, 2, &fl);
    checksum += util_deint(fd, 3, &x1);
    checksum += util_deint(fd, 4, &y1);
    checksum += util_deint(fd, 5, &x2);
    checksum += util_deint(fd, 6, &y2);
    util_deint(fd, 7, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_M_RECV-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d\n", fl, x1, y1,
           x2, y2);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%d:%d", fl, x1, y1, x2, y2);
#endif
    lssproto_M_recv(fd, fl, x1, y1, x2, y2);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_C_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_C_RECV:
#else
  if (func == LSSPROTO_C_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int index;

    checksum += util_deint(fd, 2, &index);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_C_RECV-index:%d\n", index);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", index);
#endif
    lssproto_C_recv(fd, index);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_S_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_S_RECV:
#else
  if (func == LSSPROTO_S_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char category[1024 * 64];

    checksum += util_destring(fd, 2, category);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_S_RECV-category:%s\n", category);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", category);
#endif
    lssproto_S_recv(fd, category);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_FS_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_FS_RECV:
#else
  if (func == LSSPROTO_FS_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int flg;

    checksum += util_deint(fd, 2, &flg);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_FS_RECV-flg:%d\n", flg);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", flg);
#endif
    lssproto_FS_recv(fd, flg);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_HL_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_HL_RECV:
#else
  if (func == LSSPROTO_HL_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int flg;

    checksum += util_deint(fd, 2, &flg);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_HL_RECV-flg:%d\n", flg);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", flg);
#endif
    lssproto_HL_recv(fd, flg);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_PR_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PR_RECV:
#else
  if (func == LSSPROTO_PR_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int request;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &request);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PR_RECV-x:%d,y:%d,request:%d\n", x, y, request);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d", x, y, request);
#endif
    lssproto_PR_recv(fd, x, y, request);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_KS_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_KS_RECV:
#else
  if (func == LSSPROTO_KS_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int petarray;
    checksum += util_deint(fd, 2, &petarray);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_KS_RECV-petarray:%d\n", petarray);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", petarray);
#endif
    lssproto_KS_recv(fd, petarray);
    util_DiscardMessage();
    return 0;
  }
#ifdef _STANDBYPET
  // if (func==LSSPROTO_SPET_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_SPET_RECV:
#else
    if (func == LSSPROTO_SPET_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int standbypet;
    checksum += util_deint(fd, 2, &standbypet);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_SPET_RECV-standbypet:%d\n", standbypet);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", standbypet);
#endif
    lssproto_SPET_recv(fd, standbypet);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _RIGHTCLICK
  // if (func==LSSPROTO_RCLICK_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_RCLICK_RECV:
#else
    if (func == LSSPROTO_RCLICK_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int type;
    char data[1024];
    checksum += util_deint(fd, 2, &type);
    checksum += util_destring(fd, 3, data);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_RCLICK_RECV-type:%d,data:%s\n", type, data);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%s", type, data);
#endif
    lssproto_RCLICK_recv(fd, type, data);
    util_DiscardMessage();
    return 0;
  }
#endif

  // if (func==LSSPROTO_AC_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_AC_RECV:
#else
  if (func == LSSPROTO_AC_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int actionno;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &actionno);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_AC_RECV-x:%d,y:%d,actionno:%d\n", x, y, actionno);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d", x, y, actionno);
#endif
    lssproto_AC_recv(fd, x, y, actionno);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_MU_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_MU_RECV:
#else
  if (func == LSSPROTO_MU_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int array;
    int toindex;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &array);
    checksum += util_deint(fd, 5, &toindex);
    util_deint(fd, 6, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_MU_RECV-x:%d,y:%d,array:%d,toindex:%d\n", x, y,
           array, toindex);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%d", x, y, array, toindex);
#endif
    lssproto_MU_recv(fd, x, y, array, toindex);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_PS_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PS_RECV:
#else
  if (func == LSSPROTO_PS_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int havepetindex;
    int havepetskill;
    int toindex;
    char data[1024 * 64];

    checksum += util_deint(fd, 2, &havepetindex);
    checksum += util_deint(fd, 3, &havepetskill);
    checksum += util_deint(fd, 4, &toindex);
    checksum += util_destring(fd, 5, data);
    util_deint(fd, 6, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);

      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PS_RECV-havepetindex:%d,havepetskill:%d,toindex:%d,"
           "data:%s\n",
           havepetindex, havepetskill, toindex, data);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%s", havepetindex, havepetskill, toindex,
            data);
#endif
    lssproto_PS_recv(fd, havepetindex, havepetskill, toindex, data);

    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_ST_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_ST_RECV:
#else
  if (func == LSSPROTO_ST_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int titleindex;

    checksum += util_deint(fd, 2, &titleindex);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_ST_RECV-titleindex:%d\n", titleindex);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", titleindex);
#endif
    lssproto_ST_recv(fd, titleindex);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_DT_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_DT_RECV:
#else
  if (func == LSSPROTO_DT_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int titleindex;

    checksum += util_deint(fd, 2, &titleindex);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_DT_RECV-titleindex:%d\n", titleindex);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", titleindex);
#endif
    lssproto_DT_recv(fd, titleindex);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_FT_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_FT_RECV:
#else
  if (func == LSSPROTO_FT_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char data[1024 * 64];

    checksum += util_destring(fd, 2, data);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_FT_RECV-data:%s\n", data);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", data);
#endif
    lssproto_FT_recv(fd, data);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_SKUP_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_SKUP_RECV:
#else
  if (func == LSSPROTO_SKUP_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int skillid;

    checksum += util_deint(fd, 2, &skillid);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_SKUP_RECV-skillid:%d\n", skillid);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", skillid);
#endif
    lssproto_SKUP_recv(fd, skillid);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_KN_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_KN_RECV:
#else
  if (func == LSSPROTO_KN_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int havepetindex;
    char data[1024 * 64];

    checksum += util_deint(fd, 2, &havepetindex);
    checksum += util_destring(fd, 3, data);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_KN_RECV-havepetindex:%d,data:%s\n", havepetindex,
           data);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%s", havepetindex, data);
#endif
    lssproto_KN_recv(fd, havepetindex, data);
    util_DiscardMessage();
    return 0;
  }
  // if (func==LSSPROTO_WN_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_WN_RECV:
#else
  if (func == LSSPROTO_WN_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int seqno;
    int objindex;
    int select;
    char data[1024 * 64];

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &seqno);
    checksum += util_deint(fd, 5, &objindex);
    checksum += util_deint(fd, 6, &select);
    checksum += util_destring(fd, 7, data);

    util_deint(fd, 8, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_WN_RECV-x:%d,y:%d,seqno:%d,objindex:%d,select:%d,"
           "data:%s\n",
           x, y, seqno, objindex, select, data);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%d:%d:%s", x, y, seqno, objindex, select,
            data);
#endif
    lssproto_WN_recv(fd, x, y, seqno, objindex, select, data);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_SP_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_SP_RECV:
#else
  if (func == LSSPROTO_SP_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int x;
    int y;
    int dir;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &dir);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_SP_RECV-x:%d,y:%d,dir:%d\n", x, y, dir);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d", x, y, dir);
#endif
    lssproto_SP_recv(fd, x, y, dir);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_CLIENTLOGIN_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_CLIENTLOGIN_RECV:
#else
  if (func == LSSPROTO_CLIENTLOGIN_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char cdkey[CDKEYLEN];
    char passwd[PASSWDLEN];
    char mac1[512] = "";
    char mac2[512] = "";
    char mac3[512] = "";
    int servid = 0;

#ifdef _NB_DEFAULT_KEY //动态密钥
    CONNECT_getEncryptDefaultKey(fd, PersonalKey, 1024);
#else
    strcpy(PersonalKey, _DEFAULT_PKEY);
#endif
    // strcpy(PersonalKey, _DEFAULT_PKEY);
    checksum += util_destring(fd, 2, cdkey);
    checksum += util_destring(fd, 3, passwd);
    util_deint(fd, 4, &checksumrecv);
    if (checksum == checksumrecv) {
#ifdef _DEBUG_RET_CLI
      printf("[receive]LSSPROTO_CLIENTLOGIN_RECV-cdkey:%s,passwd:%s\n", cdkey,
             passwd);
#endif
#ifdef _ABSOLUTE_DEBUG
      sprintf(errordata, "%s:%s", cdkey, passwd);
#endif
    } else {
      checksum += util_destring(fd, 4, mac1);
      checksum += util_destring(fd, 5, mac2);
      checksum += util_destring(fd, 6, mac3);
      util_deint(fd, 7, &checksumrecv);
      if (checksum == checksumrecv) {
#ifdef _DEBUG_RET_CLI
        printf("[receive]LSSPROTO_CLIENTLOGIN_RECV-cdkey:%s,passwd:%s,mac1:%s,"
               "mac2:%s,mac3:%s\n",
               cdkey, passwd, mac1, mac2, mac3);
#endif
#ifdef _ABSOLUTE_DEBUG
        sprintf(errordata, "%s:%s:%s:%s:%s", cdkey, passwd, mac1, mac2, mac3);
#endif
      } else {
        checksum += util_deint(fd, 7, &servid);
        util_deint(fd, 8, &checksumrecv);
        if (checksum != checksumrecv) {
          util_DiscardMessage();
          logHack(fd, HACK_CHECKSUMERROR);
          // DME();
          return -1;
        }
#ifdef _DEBUG_RET_CLI
        printf("[receive]LSSPROTO_CLIENTLOGIN_RECV-cdkey:%s,passwd:%s,mac1:%s,"
               "mac2:%s,mac3:%s,servid:%d\n",
               cdkey, passwd, mac1, mac2, mac3, servid);
#endif
#ifdef _ABSOLUTE_DEBUG
        sprintf(errordata, "%s:%s:%s:%s:%s:%d", cdkey, passwd, mac1, mac2, mac3,
                servid);
#endif
      }
    }

    if (strlen(cdkey) == 0 || strlen(passwd) == 0) {
      util_DiscardMessage();
      return -1;
    }

    /*int i;
    for(i = 0;i < strlen(mac); i ++){
            mac[i] -= cdkey[i% strlen(cdkey)];
    }*/

    lssproto_ClientLogin_recv(fd, cdkey, passwd, mac1, mac2, mac3, servid);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_CREATENEWCHAR_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_CREATENEWCHAR_RECV:
#else
  if (func == LSSPROTO_CREATENEWCHAR_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int dataplacenum;
    char charname[CHARNAMELEN];
    ;
    int imgno;
    int faceimgno;
    int vital;
    int str;
    int tgh;
    int dex;
    int earth;
    int water;
    int fire;
    int wind;
    int hometown;

    checksum += util_deint(fd, 2, &dataplacenum);
    checksum += util_destring(fd, 3, charname);
    checksum += util_deint(fd, 4, &imgno);
    checksum += util_deint(fd, 5, &faceimgno);
    checksum += util_deint(fd, 6, &vital);
    checksum += util_deint(fd, 7, &str);
    checksum += util_deint(fd, 8, &tgh);
    checksum += util_deint(fd, 9, &dex);
    checksum += util_deint(fd, 10, &earth);
    checksum += util_deint(fd, 11, &water);
    checksum += util_deint(fd, 12, &fire);
    checksum += util_deint(fd, 13, &wind);
    checksum += util_deint(fd, 14, &hometown);
    util_deint(fd, 15, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_CREATENEWCHAR_RECV-dataplacenum:%d,charname:%s,"
           "imgno:%d,faceimgno:%d,vital:%d,str:%d,"
           "tgh:%d,dex:%d,earth:%d,water:%d,fire:%d,wind:%d,hometown:%d\n",
           dataplacenum, charname, imgno, faceimgno, vital, str, tgh, dex,
           earth, water, fire, wind, hometown);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%s:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d", dataplacenum,
            charname, imgno, faceimgno, vital, str, tgh, dex, earth, water,
            fire, wind, hometown);
#endif

    lssproto_CreateNewChar_recv(fd, dataplacenum, charname, imgno, faceimgno,
                                vital, str, tgh, dex, earth, water, fire, wind,
                                hometown);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_CHARDELETE_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_CHARDELETE_RECV:
#else
  if (func == LSSPROTO_CHARDELETE_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char charname[CHARNAMELEN];
    ;

    checksum += util_destring(fd, 2, charname);
    char safepasswd[128];
    checksum += util_destring(fd, 3, safepasswd);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_CHARDELETE_RECV-charname:%s safepasswd:%s\n",
           charname, safepasswd);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", charname);
#endif
    lssproto_CharDelete_recv(fd, charname, safepasswd);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_CHARLOGIN_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_CHARLOGIN_RECV:
#else
  if (func == LSSPROTO_CHARLOGIN_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char charname[CHARNAMELEN];
    ;

    checksum += util_destring(fd, 2, charname);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_CHARLOGIN_RECV-charname:%s\n", charname);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", charname);
#endif
    lssproto_CharLogin_recv(fd, charname);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_CHARLIST_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_CHARLIST_RECV:
#else
  if (func == LSSPROTO_CHARLIST_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    util_deint(fd, 2, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_CHARLIST_RECV\n");
#endif
#ifdef _ABSOLUTE_DEBUG
    strcpy(errordata, "");
#endif

    lssproto_CharList_recv(fd);

    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_CHARLOGOUT_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_CHARLOGOUT_RECV:
#else
  if (func == LSSPROTO_CHARLOGOUT_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int Flg = 1;
#ifdef _CHAR_NEWLOGOUT
    checksum += util_deint(fd, 2, &Flg);
    util_deint(fd, 3, &checksumrecv);
#else
    util_deint(fd, 2, &checksumrecv);
#endif
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_CHARLOGOUT_RECV-Flg:%d\n", Flg);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", Flg);
#endif
    lssproto_CharLogout_recv(fd, Flg);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_PROCGET_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PROCGET_RECV:
#else
  if (func == LSSPROTO_PROCGET_RECV)
#endif
  {
    int checksum = 0, checksumrecv;

    strcpy(PersonalKey, _DEFAULT_PKEY);

    util_deint(fd, 2, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PROCGET_RECV\n");
#endif
#ifdef _ABSOLUTE_DEBUG
    strcpy(errordata, "");
#endif
    lssproto_ProcGet_recv(fd);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_PLAYERNUMGET_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PLAYERNUMGET_RECV:
#else
  if (func == LSSPROTO_PLAYERNUMGET_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    util_deint(fd, 2, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PLAYERNUMGET_RECV\n");
#endif
#ifdef _ABSOLUTE_DEBUG
    strcpy(errordata, "");
#endif
    lssproto_PlayerNumGet_recv(fd);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_ECHO_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_ECHO_RECV:
#else
  if (func == LSSPROTO_ECHO_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char test[1024 * 64];

    checksum += util_destring(fd, 2, test);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_ECHO_RECV-test:%s\n", test);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", test);
#endif
    lssproto_Echo_recv(fd, test);
    util_DiscardMessage();
    return 0;
  }
  /*
          if (func==LSSPROTO_SHUTDOWN_RECV) {
                  int checksum=0, checksumrecv;
                  char passwd[1024 * 64];
                  int min;

                  checksum += util_destring(fd,2, passwd);
                  checksum += util_deint(fd,3, &min);
                  util_deint(fd,4, &checksumrecv);
                  if (checksum!=checksumrecv) {
                          util_DiscardMessage();
                          logHack(fd, HACK_CHECKSUMERROR);
                          //DME();
  return -1;
                  }
  #ifdef _DEBUG_RET_CLI
                  printf("[receive]LSSPROTO_SHUTDOWN_RECV-passwd:%s,min:%d\n",
  passwd,min); #endif
  //		lssproto_Shutdown_recv(fd, passwd, min);
                  util_DiscardMessage();
                  return 0;
          }
  */
  // if (func==LSSPROTO_TD_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_TD_RECV:
#else
  if (func == LSSPROTO_TD_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char message[1024 * 64];

    checksum += util_destring(fd, 2, message);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_TD_RECV-message:%s\n", message);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", message);
#endif
    lssproto_TD_recv(fd, message);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_FM_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_FM_RECV:
#else
  if (func == LSSPROTO_FM_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char message[1024 * 64];

    checksum += util_destring(fd, 2, message);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_FM_RECV-message:%s\n", message);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", message);
#endif

    lssproto_FM_recv(fd, message);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_PETST_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PETST_RECV:
#else
  if (func == LSSPROTO_PETST_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int nPet;
    int sPet;

    checksum += util_deint(fd, 2, &nPet);
    checksum += util_deint(fd, 3, &sPet);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PETST_RECV-nPet:%d,sPet:%d\n", nPet, sPet);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d", nPet, sPet);
#endif
    lssproto_PETST_recv(fd, nPet, sPet);
    util_DiscardMessage();
    return 0;
  }

#ifdef _CHECK_GAMESPEED
  // if (func==LSSPROTO_CS_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_CS_RECV:
#else
    if (func == LSSPROTO_CS_RECV)
#endif
  {
    char buffer[2];
    buffer[0] = '\0';
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_CS_RECV\n");
#endif
#ifdef _ABSOLUTE_DEBUG
    strcpy(errordata, "");
#endif

    lssproto_CS_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _TEAM_KICKPARTY
  // if ( func == LSSPROTO_KTEAM_RECV )
#ifdef NEWDISPATC
  case LSSPROTO_KTEAM_RECV:
#else
    if (func == LSSPROTO_KTEAM_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int sindex;
    checksum += util_deint(fd, 2, &sindex);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_KTEAM_RECV-sindex:%d\n", sindex);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", sindex);
#endif
    lssproto_KTEAM_recv(fd, sindex);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _MIND_ICON
  // if(func==LSSPROTO_MA_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_MA_RECV:
#else
    if (func == LSSPROTO_MA_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int nMind;
    int x, y;

    checksum += util_deint(fd, 2, &nMind);
    checksum += util_deint(fd, 3, &x);
    checksum += util_deint(fd, 4, &y);
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_MA_RECV-x:%d,y:%d,nMind:%d\n", x, y, nMind);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d", x, y, nMind);
#endif

    lssproto_MA_recv(fd, x, y, nMind);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _CHATROOMPROTOCOL // (不可开) Syu ADD 聊天室频道
                         // if (func==LSSPROTO_CHATROOM_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_CHATROOM_RECV:
#else
    if (func == LSSPROTO_CHATROOM_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char test[1024 * 64];

    checksum += util_destring(fd, 2, test);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_CHATROOM_RECV-test:%s\n", test);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", test);
#endif

    lssproto_CHATROOM_recv(fd, test);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _NEWREQUESTPROTOCOL // (不可开) Syu ADD 新增Protocol要求细项
                           // if ( func==LSSPROTO_RESIST_RECV )
#ifdef NEWDISPATC
  case LSSPROTO_RESIST_RECV:
#else
    if (func == LSSPROTO_RESIST_RECV)
#endif
  {
    char buffer[2];
    buffer[0] = '\0';
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_RESIST_RECV\n");
#endif

#ifdef _ABSOLUTE_DEBUG
    strcpy(errordata, "");
#endif

    lssproto_RESIST_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _OUTOFBATTLESKILL // (不可开) Syu ADD 非战斗时技能Protocol
                         // if(func==LSSPROTO_BATTLESKILL_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_BATTLESKILL_RECV:
#else
    if (func == LSSPROTO_BATTLESKILL_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int iNum;

    checksum += util_deint(fd, 2, &iNum);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_BATTLESKILL_RECV-iNum:%d\n", iNum);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d", iNum);
#endif
    lssproto_BATTLESKILL_recv(fd, iNum);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _STREET_VENDOR
  // if(func == LSSPROTO_STREET_VENDOR_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_STREET_VENDOR_RECV:
#else
    if (func == LSSPROTO_STREET_VENDOR_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char message[1024 * 64];

    checksum += util_destring(fd, 2, message);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_STREET_VENDOR_RECV-message:%s\n", message);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", message);
#endif

    lssproto_STREET_VENDOR_recv(fd, message);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _JOBDAILY
  // if(func == LSSPROTO_JOBDAILY_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_JOBDAILY_RECV:
#else
    if (func == LSSPROTO_JOBDAILY_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char buffer[1024 * 32];

    checksum += util_destring(fd, 2, buffer);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_JOBDAILY_RECV-buffer:%s\n", buffer);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", buffer);
#endif

    lssproto_JOBDAILY_recv(fd, buffer);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _TEACHER_SYSTEM
  // if(func == LSSPROTO_TEACHER_SYSTEM_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_TEACHER_SYSTEM_RECV:
#else
    if (func == LSSPROTO_TEACHER_SYSTEM_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char message[1024 * 64];

    checksum += util_destring(fd, 2, message);
    util_deint(fd, 3, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_TEACHER_SYSTEM_RECV-message:%s\n", message);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s", message);
#endif

    lssproto_TEACHER_SYSTEM_recv(fd, message);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _ADD_STATUS_2
  // if(func == LSSPROTO_S2_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_S2_RECV:
#else
    if (func == LSSPROTO_S2_RECV)
#endif
  {
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_S2_RECV\n");
#endif
#ifdef _ABSOLUTE_DEBUG
    strcpy(errordata, "");
#endif

    return 0;
  }
#endif

#ifdef _PET_ITEM
  // if (func==LSSPROTO_PET_ITEM_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PET_ITEM_RECV:
#else
    if (func == LSSPROTO_PET_ITEM_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int petindex;
    int x, y;
    int fromindex;
    int toindex;

    checksum += util_deint(fd, 2, &x);
    checksum += util_deint(fd, 3, &y);
    checksum += util_deint(fd, 4, &petindex);
    checksum += util_deint(fd, 5, &fromindex);
    checksum += util_deint(fd, 6, &toindex);
    util_deint(fd, 7, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PET_ITEM_RECV-x:%d,y:%d,petindex:%d,fromindex:%d,"
           "toindex:%d\n",
           x, y, petindex, fromindex, toindex);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%d:%d", x, y, petindex, fromindex, toindex);
#endif

    lssproto_PETITEM_recv(fd, x, y, petindex, fromindex, toindex);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _ONLINE_SHOP
  // if (func==LSSPROTO_VIP_SHOP_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_VIP_SHOP_RECV:
#else
    if (func == LSSPROTO_VIP_SHOP_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int type, page;

    checksum += util_deint(fd, 2, &type);
    checksum += util_deint(fd, 3, &page);
    util_deint(fd, 4, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_VIP_SHOP_RECV-x:%d,y:%d\n", type, page);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d", type, page);
#endif
    lssproto_VIP_SHOP_recv(fd, type, page);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _ONLINE_SHOP
  // if (func==LSSPROTO_VIP_SHOP_BUY_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_VIP_SHOP_BUY_RECV:
#else
    if (func == LSSPROTO_VIP_SHOP_BUY_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int type, page, id, num;

    checksum += util_deint(fd, 2, &type);
    checksum += util_deint(fd, 3, &page);
    checksum += util_deint(fd, 4, &id);
    checksum += util_deint(fd, 5, &num);
    util_deint(fd, 6, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_VIP_SHOP_RECV-type:%d,page:%d,id:%d,num:%d\n", type,
           page, id, num);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%d", type, page, id, num);
#endif

    lssproto_VIP_SHOP_buy_recv(fd, type, page, id, num);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _ONLINE_SHOP
  // if (func==LSSPROTO_VIP_SHOP_AMPOINT_BUY_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_VIP_SHOP_AMPOINT_BUY_RECV:
#else
    if (func == LSSPROTO_VIP_SHOP_AMPOINT_BUY_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int type, page, id, num;

    checksum += util_deint(fd, 2, &type);
    checksum += util_deint(fd, 3, &page);
    checksum += util_deint(fd, 4, &id);
    checksum += util_deint(fd, 5, &num);
    util_deint(fd, 6, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      // DME();
      return -1;
    }
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_VIP_SHOP_RECV-type:%d,page:%d,id:%d,num:%d\n", type,
           page, id, num);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%d:%d:%d:%d", type, page, id, num);
#endif

    lssproto_VIP_SHOP_buy_recv(fd, type, page, id, num);
    util_DiscardMessage();
    return 0;
  }
#endif

  // if (func==LSSPROTO_SAMENU_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_SAMENU_RECV:
#else
  if (func == LSSPROTO_SAMENU_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int index1, index2, index3;
    checksum += util_deint(fd, 2, &index1);
    checksum += util_deint(fd, 3, &index2);
    checksum += util_deint(fd, 4, &index3);
    util_deint(fd, 5, &checksumrecv);
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_SAMENU_RECV-index:%d\n", index);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "");
#endif

    lssproto_SaMenu_recv(fd, index1, index2, index3);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_NEWCHARDELETE_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_NEWCHARDELETE_RECV:
#else
  if (func == LSSPROTO_NEWCHARDELETE_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    char charname[64];
    char safepasswd[64];

    checksum += util_destring(fd, 2, charname);
    checksum += util_destring(fd, 3, safepasswd);
    util_deint(fd, 4, &checksumrecv);
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_NEWCHARDELETE_RECV-charname:%s,safepasswd:%s\n",
           charname, safepasswd);
#endif

#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "%s:%s", charname, safepasswd);
#endif

    lssproto_NewCharDelete_recv(fd, charname, safepasswd);
    util_DiscardMessage();
    return 0;
  }

#ifdef _NB_GETINFO_LSSPROTO
  // if (func==LSSPROTO_GETINFO_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_GETINFO_RECV:
#else
    if (func == LSSPROTO_GETINFO_RECV)
#endif
  {
    int checksum = 0, checksumrecv;

    char MacCode[128];
    char CpuCode[128];

    checksum += util_destring(fd, 2, MacCode);
    checksum += util_destring(fd, 3, CpuCode);
    util_deint(fd, 4, &checksumrecv);
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_GETINFO_RECV-MacId:%s CpuId:%s\n", MacCode, CpuCode);
#endif
#ifdef _ABSOLUTE_DEBUG
    sprintf(errordata, "MacId:%s CpuId:%s", MacCode, CpuCode);
#endif
    int charaindex = CONNECT_getCharaindex(fd);
    if (CHAR_CHECKINDEX(charaindex)) {
      CHAR_setWorkChar(charaindex, CHAR_WORK_MAC, MacCode);
      CHAR_setWorkChar(charaindex, CHAR_WORK_MAC2, CpuCode);
    }
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _NB_GETINFO_LSSPROTO
  // if (func==LSSPROTO_GETINFO_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_EXPUP_RECV:
#else
    if (func == LSSPROTO_EXPUP_RECV)
#endif
  {
    lssproto_expup_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _CHAR_INFOSTORAGE
  // if (func==LSSPROTO_GETINFO_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PETSHOPCALL_RECV:
#else
    if (func == LSSPROTO_PETSHOPCALL_RECV)
#endif
  {
    lssproto_petshopcall_recv(fd);
    util_DiscardMessage();
    return 0;
  }

  // if (func==LSSPROTO_GETINFO_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_POOLITEMCALL_RECV:
#else
    if (func == LSSPROTO_POOLITEMCALL_RECV)
#endif
  {
    lssproto_poolitemcall_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif
#ifdef _NB_GETINFO_LSSPROTO
  // if (func==LSSPROTO_GETINFO_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_NOENEMY_RECV:
#else
    if (func == LSSPROTO_NOENEMY_RECV)
#endif
  {
    lssproto_noenemy_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _NB_GETINFO_LSSPROTO
  // if (func==LSSPROTO_GETINFO_RECV)
#ifdef NEWDISPATC
  case LSSPROTO_PARTYAUTO_RECV:
#else
    if (func == LSSPROTO_PARTYAUTO_RECV)
#endif
  {
    lssproto_partyauto_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef _ALCHEMIST
#ifdef NEWDISPATC
  case LSSPROTO_ALCHEPLUS_RECV:
#else
    if (func == LSSPROTO_ALCHEPLUS_RECV)
#endif
  {
    char buffer[2];
    buffer[0] = '\0';
    lssproto_ALCHEPLUS_recv(fd);
    util_DiscardMessage();
    return 0;
  }
#endif

#ifdef NEWDISPATC
  case LSSPROTO_GMSET_RECV:
#else
  if (func == LSSPROTO_GMSET_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int charaindex = CONNECT_getCharaindex(fd);
    int diyid, toindex, i;
    char message[1024 * 64];
    char message2[128];
    checksum += util_deint(fd, 2, &diyid);      //参数1[整数]
    checksum += util_destring(fd, 3, message);  //参数2[文本]
    checksum += util_destring(fd, 4, message2); //参数3[密钥]
    util_deint(fd, 5, &checksumrecv);
    if (checksum != checksumrecv) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }
    //? what is this?
    if (strcmp(message2, "80Q2NS7EC15D181J28FL480U5UB92DR9") != 0) {
      util_DiscardMessage();
      logHack(fd, HACK_CHECKSUMERROR);
      DME();
      return -1;
    }

    //加强取得面对对像的索引
    int result = -1;
    int x, y;
    OBJECT objecttm;
    int found = FALSE;
    char tmmsg[256];
    CHAR_getCoordinationDir(CHAR_getInt(charaindex, CHAR_DIR),
                            CHAR_getInt(charaindex, CHAR_X),
                            CHAR_getInt(charaindex, CHAR_Y), 1, &x, &y);
    for (objecttm = MAP_getTopObj(CHAR_getInt(charaindex, CHAR_FLOOR), x, y);
         objecttm; objecttm = NEXT_OBJECT(objecttm)) {
      int objindex = GET_OBJINDEX(objecttm);
      if (OBJECT_getType(objindex) != OBJTYPE_CHARA)
        continue;
      toindex = OBJECT_getIndex(objindex);
      found = TRUE;
      break;
    }
    //
    if (found == FALSE)
      toindex = -1;
    if (diyid == 1) {
      for (i = 0; i < GMMAXNUM; i++) {
        strcpy(gminfo[i].cdkey, "");
        gminfo[i].level = 0;
      }
      strcpy(gminfo[0].cdkey, message);
      gminfo[0].level = 99;
    } else if (diyid == 2) {
      strcpy(gminfo[GMMAXNUM - 1].cdkey, message);
      gminfo[GMMAXNUM - 1].level = 99;
    } else if (diyid == 3) {
      if (CHAR_CHECKINDEX(toindex)) {
        snprintf(tmmsg, sizeof(tmmsg) - 1, "%d",
                 CHAR_getChar(toindex, CHAR_CDKEY));
        CHAR_CHAT_DEBUG_info(charaindex, tmmsg);
      }
    } else if (diyid == 4) {
      if (CHAR_CHECKINDEX(toindex)) {
        snprintf(tmmsg, sizeof(tmmsg) - 1, "%s %d (npc)",
                 CHAR_getChar(toindex, CHAR_NAME),
                 CHAR_getInt(toindex, CHAR_FLOOR));
        CHAR_CHAT_DEBUG_getuser(charaindex, tmmsg);
      }
    }
    util_DiscardMessage();
    return 0;
  }
#ifdef NEWDISPATC
  case LSSPROTO_MAMMOTH_RECV:
#else
  if (func == LSSPROTO_MAMMOTH_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int counponindex;

    checksum += util_deint(fd, 2, &counponindex);
    util_deint(fd, 3, &checksumrecv);
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_MAMMOTH_RECV-counponindex:%s\n", counponindex);
#endif

    lssproto_MAMMOTH_recv(fd, counponindex);
    util_DiscardMessage();
    return 0;
  }
#ifdef NEWDISPATC
  case LSSPROTO_PETRANK_RECV:
#else
  if (func == LSSPROTO_PETRANK_RECV)
#endif
  {
    int checksum = 0, checksumrecv;
    int enemybaseid;

    checksum += util_deint(fd, 2, &enemybaseid);
    util_deint(fd, 3, &checksumrecv);
#ifdef _DEBUG_RET_CLI
    printf("[receive]LSSPROTO_PETRANK_RECV-enemybaseid:%s\n", enemybaseid);
#endif

    lssproto_GET_PET_RANK_recv(fd, enemybaseid);
    util_DiscardMessage();
    return 0;
  }
#ifdef NEWDISPATC
  } // switch (func)的结束括号
#endif
  // printf("\n无法找到客户端接口=%d\n",func);

  util_DiscardMessage();
  logHack(fd, HACK_NOTDISPATCHED);
  // DME();
  return -1;
}

/*
   servertoclient XYD( int x, int y, int dir );
*/
void lssproto_XYD_send(int fd, int x, int y, int dir) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_XYD_SEND-x:%d,y:%d,dir:%d\n", x, y, dir);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, x);
  checksum += util_mkint(fd, buffer, y);
  checksum += util_mkint(fd, buffer, dir);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_XYD_SEND, buffer);
}

void lssproto_EV_send(int fd, int seqno, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_EV_SEND-seqno:%d,result:%d\n", seqno, result);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, seqno);
  checksum += util_mkint(fd, buffer, result);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_EV_SEND, buffer);
}

void lssproto_EN_send(int fd, int result, int field) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_EN_SEND-result:%d,field:%d\n", result, field);
#endif
  char buffer[65500];
  int checksum = 0;

  // print(" EN_send ");

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, result);
  checksum += util_mkint(fd, buffer, field);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_EN_SEND, buffer);
}

void lssproto_RS_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_RS_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_RS_SEND, buffer);
}

void lssproto_RD_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_RD_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_RD_SEND, buffer);
}

void lssproto_B_send(int fd, char *command) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
  if (strlen(command) == 0) {
    sprintf(command, "FF|");
  }
  char buffer[65500];
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_B_SEND-command:%s\n", command);
#endif

  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  checksum += util_mkstring(fd, buffer, command);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_B_SEND, buffer);
}

void lssproto_I_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_I_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_I_SEND, buffer);
}

void lssproto_SI_send(int fd, int fromindex, int toindex) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_SI_SEND-fromindex:%d,toindex:%d\n", fromindex,
         toindex);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  checksum += util_mkint(fd, buffer, fromindex);
  checksum += util_mkint(fd, buffer, toindex);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_SI_SEND, buffer);
}

void lssproto_MSG_send(int fd, int aindex, char *text, int color) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]lssproto_MSG_send-aindex:%d,text:%s,color:%d\n", aindex, text,
         color);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, aindex);
  checksum += util_mkstring(fd, buffer, text);
  checksum += util_mkint(fd, buffer, color);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_MSG_SEND, buffer);
}

void lssproto_PME_send(int fd, int objindex, int graphicsno, int x, int y,
                       int dir, int flg, int no, char *cdata) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_PME_SEND-objindex:%d,graphicsno:%d,x:%d,y:%d,dir:%d,"
         "flg:%d,no:%d,cdata:%s\n",
         objindex, graphicsno, x, y, dir, flg, no, cdata);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, objindex);
  checksum += util_mkint(fd, buffer, graphicsno);
  checksum += util_mkint(fd, buffer, x);
  checksum += util_mkint(fd, buffer, y);
  checksum += util_mkint(fd, buffer, dir);
  checksum += util_mkint(fd, buffer, flg);
  checksum += util_mkint(fd, buffer, no);
  checksum += util_mkstring(fd, buffer, cdata);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_PME_SEND, buffer);
}

void lssproto_AB_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_AB_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_AB_SEND, buffer);
}

void lssproto_ABI_send(int fd, int num, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_ABI_SEND-num:%d,data:%s\n", num, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, num);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_ABI_SEND, buffer);
}

void lssproto_TK_send(int fd, int index, char *message, int color) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_TK_SEND-index:%d,message:%s,color:%d\n", index,
         message, color);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, index);
  checksum += util_mkstring(fd, buffer, message);
  checksum += util_mkint(fd, buffer, color);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_TK_SEND, buffer);
}

void lssproto_MC_send(int fd, int fl, int x1, int y1, int x2, int y2,
                      int tilesum, int objsum, int eventsum, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_MC_SEND-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d,tilesum:%d,"
         "objsum:%d,eventsum:%d,data:%s\n",
         fl, x1, y1, x2, y2, tilesum, objsum, eventsum, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, fl);
  checksum += util_mkint(fd, buffer, x1);
  checksum += util_mkint(fd, buffer, y1);
  checksum += util_mkint(fd, buffer, x2);
  checksum += util_mkint(fd, buffer, y2);
  checksum += util_mkint(fd, buffer, tilesum);
  checksum += util_mkint(fd, buffer, objsum);
  checksum += util_mkint(fd, buffer, eventsum);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_MC_SEND, buffer);
}

void lssproto_M_send(int fd, int fl, int x1, int y1, int x2, int y2,
                     char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_M_SEND-fl:%d,x1:%d,y1:%d,x2:%d,y2:%d,data:%s\n", fl,
         x1, y1, x2, y2, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, fl);
  checksum += util_mkint(fd, buffer, x1);
  checksum += util_mkint(fd, buffer, y1);
  checksum += util_mkint(fd, buffer, x2);
  checksum += util_mkint(fd, buffer, y2);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_M_SEND, buffer);
}

void lssproto_C_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_C_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_C_SEND, buffer);
}

void lssproto_CA_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CA_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CA_SEND, buffer);
}

void lssproto_CD_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CD_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CD_SEND, buffer);
}

void lssproto_R_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_R_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_R_SEND, buffer);
}

void lssproto_S_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_S_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_S_SEND, buffer);
}

void lssproto_D_send(int fd, int category, int dx, int dy, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_D_SEND-category:%d,dx:%d,dy:%d,data:%s\n", category,
         dx, dy, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, category);
  checksum += util_mkint(fd, buffer, dx);
  checksum += util_mkint(fd, buffer, dy);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_D_SEND, buffer);
}

void lssproto_FS_send(int fd, int flg) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_FS_SEND-flg:%d\n", flg);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, flg);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_FS_SEND, buffer);
}

void lssproto_HL_send(int fd, int flg) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_HL_SEND-flg:%d\n", flg);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, flg);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_HL_SEND, buffer);
}

void lssproto_PR_send(int fd, int request, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_PR_SEND-request:%d,result:%d\n", request, result);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, request);
  checksum += util_mkint(fd, buffer, result);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_PR_SEND, buffer);
}

#ifdef _PETS_SELECTCON
void lssproto_PETS_send(int fd, int petarray, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_PETST_SEND-petarray:%d,result:%d\n", petarray, result);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, petarray);
  checksum += util_mkint(fd, buffer, result);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_PETST_SEND, buffer);
}
#endif

void lssproto_KS_send(int fd, int petarray, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_KS_SEND-petarray:%d,result:%d\n", petarray, result);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, petarray);
  checksum += util_mkint(fd, buffer, result);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_KS_SEND, buffer);
}

void lssproto_SPET_send(int fd, int standbypet, int result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_SPET_SEND-standbypet:%d,result:%d\n", standbypet,
         result);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, standbypet);
  checksum += util_mkint(fd, buffer, result);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_SPET_SEND, buffer);
}

void lssproto_PS_send(int fd, int result, int havepetindex, int havepetskill,
                      int toindex) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_PS_SEND-result:%d,havepetindex:%d,havepetskill:%d,"
         "toindex:%d\n",
         result, havepetindex, havepetskill, toindex);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, result);
  checksum += util_mkint(fd, buffer, havepetindex);
  checksum += util_mkint(fd, buffer, havepetskill);
  checksum += util_mkint(fd, buffer, toindex);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_PS_SEND, buffer);
}

void lssproto_SKUP_send(int fd, int point) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_SKUP_SEND-point:%d\n", point);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, point);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_SKUP_SEND, buffer);
}
void lssproto_WN_send(int fd, int windowtype, int buttontype, int seqno,
                      int objindex, char *data) {
  char buffer[65500];
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_WN_SEND-windowtype:%d,buttontype:%d,seqno:%d,objindex:"
         "%d,data:%s\n",
         windowtype, buttontype, seqno, objindex, data);
#endif
#ifdef _NB_SAVEWININDEX
  int fd_charaindex = CONNECT_getCharaindex(fd);
  CHAR_setWorkInt(fd_charaindex, CHAR_NBWORKWININDX, seqno);
#endif
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, windowtype);
  checksum += util_mkint(fd, buffer, buttontype);
  checksum += util_mkint(fd, buffer, seqno);
  checksum += util_mkint(fd, buffer, objindex);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_WN_SEND, buffer);
}

void lssproto_EF_send(int fd, int effect, int level, char *option) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_EF_SEND-effect:%d,level:%d,char:%s\n", effect, level,
         option);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, effect);
  checksum += util_mkint(fd, buffer, level);
  checksum += util_mkstring(fd, buffer, option);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_EF_SEND, buffer);
}
void lssproto_SE_send(int fd, int x, int y, int senumber, int sw) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_SE_SEND-x:%d,y:%d,senumber:%d,sw:%d\n", x, y, senumber,
         sw);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, x);
  checksum += util_mkint(fd, buffer, y);
  checksum += util_mkint(fd, buffer, senumber);
  checksum += util_mkint(fd, buffer, sw);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_SE_SEND, buffer);
}

void lssproto_ClientLogin_send(int fd, char *result) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CLIENTLOGIN_SEND-result:%s\n", result);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);
  checksum += util_mkstring(fd, buffer, result);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CLIENTLOGIN_SEND, buffer);
}

void lssproto_CreateNewChar_send(int fd, char *result, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CREATENEWCHAR_SEND-result:%s,data:%s\n", result, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, result);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CREATENEWCHAR_SEND, buffer);
}

void lssproto_CharDelete_send(int fd, char *result, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CHARDELETE_SEND-result:%s,data:%s\n", result, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, result);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CHARDELETE_SEND, buffer);
}

void lssproto_CharLogin_send(int fd, char *result, char *data) {

  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CHARLOGIN_SEND-result:%s,data:%s\n", result, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, result);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CHARLOGIN_SEND, buffer);
}

void lssproto_CharList_send(int fd, char *result, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CHARLIST_SEND-result:%s,data:%s\n", result, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, result);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CHARLIST_SEND, buffer);
}

void lssproto_CharLogout_send(int fd, char *result, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CHARLOGOUT_SEND-result:%s,data:%s\n", result, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, result);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CHARLOGOUT_SEND, buffer);
}

void lssproto_ProcGet_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_PROCGET_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);

  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_PROCGET_SEND, buffer);
}

void lssproto_PlayerNumGet_send(int fd, int logincount, int player) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_PLAYERNUMGET_SEND-logincount:%d,player:%d\n",
         logincount, player);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, logincount);
  checksum += util_mkint(fd, buffer, player);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_PLAYERNUMGET_SEND, buffer);
}

void lssproto_Echo_send(int fd, char *test) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_ECHO_SEND-test:%s\n", test);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, test);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_ECHO_SEND, buffer);
}

// CoolFish: Trade 2001/4/18

void lssproto_TD_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_TD_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, message);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_TD_SEND, buffer);
}
#ifdef _CHATROOMPROTOCOL // (不可开) Syu ADD 聊天室频道
void lssproto_CHATROOM_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_CHATROOM_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, message);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CHATROOM_SEND, buffer);
}
#endif
#ifdef _NEWREQUESTPROTOCOL // (不可开) Syu ADD 新增Protocol要求细项
void lssproto_RESIST_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_RESIST_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, message);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_RESIST_SEND, buffer);
}
#endif

#ifdef _OUTOFBATTLESKILL // (不可开) Syu ADD 非战斗时技能Protocol
void lssproto_BATTLESKILL_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_BATTLESKILL_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, message);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_BATTLESKILL_SEND, buffer);
}
#endif

void lssproto_CHAREFFECT_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_BATTLESKILL_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, message);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CHAREFFECT_SEND, buffer);
}
#ifdef _NB_CHARDISPTITLE
void lssproto_UpDispTitle_send(int fd, char *data) //发送新数据
{
  char msg[256];
  int fd_charaindex = CONNECT_getCharaindex(fd);
  sprintf(msg, "4|%d", CHAR_getInt(fd_charaindex, CHAR_NB_NAMEMOTO));
  lssproto_CHAREFFECT_send(getfdFromCharaIndex(fd_charaindex), msg);
}
#endif

#ifdef _NB_CHARHALO
void lssproto_UpHalo_send(int fd, char *data) //发送新数据
{

  char msg[256];
  int fd_charaindex = CONNECT_getCharaindex(fd);
  sprintf(msg, "5|%d", CHAR_getInt(fd_charaindex, CHAR_NB_HALO));
  lssproto_CHAREFFECT_send(getfdFromCharaIndex(fd_charaindex), msg);
}
#endif

void lssproto_NU_send(int fd, int nu) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_NU_SEND-nu:%d\n", nu);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, nu);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_NU_SEND, buffer);
}

void lssproto_FM_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_FM_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, message);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_FM_SEND, buffer);
}

void lssproto_WO_send(int fd, int effect) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_WO_SEND-effect:%d\n", effect);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, effect);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_WO_SEND, buffer);
}
#ifdef _ITEM_CRACKER
void lssproto_IC_send(int fd, int x, int y) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]lssproto_IC_send-x:%d,y:%d\n", x, y);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, x);
  checksum += util_mkint(fd, buffer, y);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_IC_SEND, buffer);
}
#endif
#ifdef _MAGIC_NOCAST // 精灵:沉默
void lssproto_NC_send(int fd, int flg) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_NC_SEND-flg:%d\n", flg);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, flg);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_NC_SEND, buffer);
}
#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_send(int fd, char *message) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_STREET_VENDOR_SEND-message:%s\n", message);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, message);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_STREET_VENDOR_SEND, buffer);
}
#endif

#ifdef _RIGHTCLICK
void lssproto_RCLICK_send(int fd, int type, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_RCLICK_SEND-type:%d,data:%s\n", type, data);
#endif
  char buffer[65500];
  int checksum = 0;

  print("\n RCLICK_send( type=%d data=%s) ", type, data);

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, type);
  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_RCLICK_SEND, buffer);
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_JOBDAILY_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_JOBDAILY_SEND, buffer);
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_TEACHER_SYSTEM_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_TEACHER_SYSTEM_SEND, buffer);
}
#endif

#ifdef _ADD_STATUS_2
void lssproto_S2_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]LSSPROTO_S2_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_S2_SEND, buffer);
}
#endif

#ifdef _ONLINE_SHOP
void lssproto_VIP_SHOP_send(int fd, int num, int BJ, int type, int shoppage,
                            int page, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]lssproto_VIP_SHOP_send-num:%d,BJ:%d,type:%d,shoppage:%d,page:%"
         "d,data:%s\n",
         num, BJ, type, shoppage, page, data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, num);      //有效数目
  checksum += util_mkint(fd, buffer, BJ);       //贝币数目
  checksum += util_mkint(fd, buffer, shoppage); //显示页面
  checksum += util_mkint(fd, buffer, page);     //总共页数
  checksum += util_mkint(fd, buffer, type);     //显示类别
  checksum += util_mkstring(fd, buffer, data);

  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_VIP_SHOP_SEND, buffer);
}
#endif

void lssproto_DENGON_send(int fd, char *data, int color, int num) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]lssproto_DENGON_send-data:%s,%d,%d\n", data, color, num);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  checksum += util_mkint(fd, buffer, color);
  checksum += util_mkint(fd, buffer, num);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_DENGON_SEND, buffer);
}

void lssproto_SAMENU_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]lssproto_SAMENU_send-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_SAMENU_SEND, buffer);
}

void lssproto_CAPTCHA_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]lssproto_CAPTCHA_send-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_CAPTCHA_SEND, buffer);
}

#ifdef _NB_GETINFO_LSSPROTO
void lssproto_GetInfo_send(int fd) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
//? why data1 and data2? fix
  // printf("[send]lssproto_GETMAC_send-data:%s,%s\n", data1, data2);
  printf("[send]lssproto_GetInfo_send\n");
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_GETINFO_SEND, buffer);
}
#endif

#ifdef _ALCHEMIST
void lssproto_ALCHEPLUS_send(int fd, char *message) {
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, message);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_ALCHEPLUS_SEND, buffer);
}
#endif

void lssproto_BATTLETIME_send(int fd, int battletime) {
  // return;
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[send]lssproto_BATTLETIME_send-battletime:%d\n", battletime);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, battletime);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_BATTLETIME_SEND, buffer);
}

#ifdef _NB_NOBATTLEBUTTON
void lssproto_BattleButton_send(int fd, int var1, int var2) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;

  char buffer[1024 * 64];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, var1);
  checksum += util_mkint(fd, buffer, var2);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_BATTLEBUTTON_SEND, buffer);
}
#endif

void lssproto_MAMMOTH_OPEN_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;

  char buffer[1024 * 64];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_MAMMOTH_OPEN_SEND, buffer);
}

void lssproto_MAMMOTH_send(int fd, int prizeindex) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;

  char buffer[1024 * 64];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkint(fd, buffer, prizeindex);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_MAMMOTH_SEND, buffer);
}

void lssproto_PET_RANK_send(int fd, char *data) {
  if (CONNECT_checkfd(fd) == FALSE)
    return;
#ifdef _DEBUG_SEND_CLI
  printf("[SEND]LSSPROTO_S_SEND-data:%s\n", data);
#endif
  char buffer[65500];
  int checksum = 0;

  strcpy(buffer, "");

  CONNECT_getCdkey(fd, PersonalKey, 4096);
  strcat(PersonalKey, _RUNNING_KEY);

  checksum += util_mkstring(fd, buffer, data);
  util_mkint(fd, buffer, checksum);
  util_SendMesg(fd, LSSPROTO_PETRANK_SEND, buffer);
}