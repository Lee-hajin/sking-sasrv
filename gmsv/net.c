#define __NET_C__
#include "version.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
// ttom+1
#include "buf.h"
#include "char.h"
#include "common.h"
#include "configfile.h"
#include "enemy.h"
#include "handletime.h"
#include "item_event.h"
#include "link.h"
#include "log.h"
#include "lssproto_serv.h"
#include "msignal.h"
#include "net.h"
#include "object.h"
#include "saacproto_cli.h"
#include "util.h"
#include <fcntl.h>
#include <sys/timeb.h>
// Arminius 7.31 cursed stone
#include "battle.h"
#include "char_talk.h"
#include "pet_event.h"
#include "petmail.h"
#include "version.h"
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif
#include "pet_event.h"
#ifdef _GMSV_DEBUG
extern char *DebugMainFunction;
#endif
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

char rbmess[1024 * 256];

#define MAXSIZE 64000
#define MAXEPS 256
//#define EVENTS            100
#define LISTENQ 32
#define SERV_PORT 8000

/*Server �������*/
#ifdef _NEW_SERVER_
BOOL bNewServer = TRUE;
#else
BOOL bNewServer = FALSE;
#endif
extern time_t initTime;
// Nuke +1 0901: For state monitor
int StateTable[WHILESAVEWAIT + 1];

int CHAR_players();
#define CONO_CHECK_LOGIN 0x001
#define CONO_CHECK_ITEM 0x010
#define CONO_CHECK_PET 0x100
int cono_check = 0x111;

int AC_WBSIZE = (1024 * 1024);
// ttom+1 for the performatce
static unsigned int MAX_item_use = 0;
int i_shutdown_time = 0;       // ttom
BOOL b_first_shutdown = FALSE; // ttom

int mfdfulll = 0;
int sendspeed = 0;
int recvspeed = 0;

extern char firstdesToken[100000][12];
extern char seconddesToken[100000][20];

extern int luaplayernum;

#ifdef _P_THREAD_RUN
extern int pthreadflg;
#endif

#ifdef _NB_NET
#define nb_checkfd(a, b)                                                       \
  if ((a) < 0 || (a) >= ConnectLen) {                                          \
    print("%s %d fd:%d err.\n", __FILE__, __LINE__, (a));                      \
    return (b);                                                                \
  }
#define nb_checkfdnull(a)                                                      \
  if ((a) < 0 || (a) >= ConnectLen) {                                          \
    print("%s %d fd:%d err.\n", __FILE__, __LINE__, (a));                      \
    return;                                                                    \
  }
#define nb_checkfdcontinue(a)                                                  \
  if ((a) < 0 || (a) >= ConnectLen) {                                          \
    print("%s %d fd:%d err.\n", __FILE__, __LINE__, (a));                      \
    continue;                                                                  \
  }
#endif
/*------------------------------------------------------------
* 扔□田及橇谪
------------------------------------------------------------*/
typedef struct tag_serverState {
	BOOL acceptmore;				/*  1分匀凶日｝accept 仄凶丐午
									切斤仁匹｝close 允月 */
	unsigned int fdid;				/*  fd 及骚曰袄 */
	unsigned int closeallsocketnum; /*   closeallsocket   及酸曰及
									醒*/
	int shutdown;					/*  扔□田毛shutdown允月乒□玉
									 *	0:骚橘 公木动陆:扑乓永玄扑乓永玄乒□玉
									 * 乒□玉卞卅匀凶凛棉互  匀化月［
									 */
	int dsptime;					/* shutdown 乒□玉及伐□弁  醒*/
	int limittime;					/* 仇木手 */
} ServerState;
typedef struct tagCONNECT {
  BOOL use;

  char *rb;
  int rbuse;
  char *wb;
  int wbuse;
  int check_rb_oneline_b;
  int check_rb_time;
  pthread_mutex_t mutex;

	struct sockaddr_in sin; /* 涛粮燮及失玉伊旦 */
	ConnectType ctype;		/* 戊生弁扑亦件及潘挀 */

	char cdkey[CDKEYLEN];	/* CDKEY */
	char passwd[PASSWDLEN]; /* 由旦伐□玉 */
	LoginType state;		/* 蜇箕及夫弘奶件橇谪 */
	int nstatecount;
	char charname[CHARNAMELEN]; /* 夫弘奶件醱及平乓仿抩 */
	int charaindex;				/* char?昫尺及奶件犯永弁旦﹝
								 * 夫弘奶件詨卞袄互涩烂今木月﹝-1互犯白巧伙玄
								 * ?昫卞卅中凛﹝
								 */
  char CAbuf[2048];			/*  CA() 毛做谅允月啃及田永白央 */
	int CAbufsiz;				/*  CAbuf 及扔奶术  */

	struct timeval lastCAsendtime; /*瘉詨卞CA毛霜匀凶凛棉 */

	char CDbuf[2048]; /*  CD() 毛做谅允月啃及田永白央 */
	int CDbufsiz;	  /*  CDbuf 及扔奶术  */

	struct timeval lastCDsendtime; /*瘉詨卞CD毛霜匀凶凛棉 */
	struct timeval lastCharSaveTime; /* 瘉詨卞平乓仿犯□正毛本□皮仄凶凛棉 */
	struct timeval lastprocesstime; /* 瘉詨卞皿夫玄戊伙毛质咥仄凶凛棉*/
	struct timeval lastreadtime; /* 瘉詨卞read仄凶凛棉﹝晓午反切互丹*/

  // Nuke start 08/27 : For acceleration avoidance
  // WALK_TOLERANCE: Permit n W messages in a second (3: is the most restricted)
#define WALK_TOLERANCE 4
#define WALK_SPOOL 5
#define WALK_RESTORE 100
  unsigned int Walktime;
  unsigned int lastWalktime;
  unsigned int Walkcount;
  int Walkspool; // For walk burst after release key F10
  int Walkrestore;
  // B3_TOLERANCE: Time distance between recently 3 B message (8: is the
  // latgest) BEO_TOLERANCE: Time distance between the lastmost B and EO (5: is
  // the largest)
#define B3_TOLERANCE 5
#define BEO_TOLERANCE 3
#define BEO_SPOOL 10
#define BEO_RESTORE 100
  unsigned int Btime;
  unsigned int lastBtime;
  unsigned int lastlastBtime;
  unsigned int EOtime;

#ifdef _BATTLE_TIMESPEED
  // unsigned int  DefBtime;
  int BDTime;
  int CBTime;
#endif

#ifdef _TYPE_TOXICATION
  int toxication;
#endif

#ifdef _ITEM_ADDEXP // vincent 经验提昇
  int EDTime;
#endif

  //    unsigned int      BEO;
  int BEOspool;
  int BEOrestore;

  int die;

  int credit;
  int fcold;
  // Nuke 0406: New Flow Control
  int nu;
  int nu_decrease;
  int ke;
  // Nuke 1213: Flow Control 2
  int packetin;

  // Nuke 0624: Avoid Null Connection
  unsigned int cotime;
  // Nuke 0626: For no enemy
  int noenemy;
  // Arminius 7.2: Ra's amulet
  int eqnoenemy;
#ifdef _Item_MoonAct
  int eqrandenemy;
#endif

#ifdef _CHIKULA_STONE
  int chistone;
#endif
  // Arminius 7.31: cursed stone
  int stayencount;

  int battlecharaindex[CONNECT_WINDOWBUFSIZE];
  int duelcharaindex[CONNECT_WINDOWBUFSIZE];
  int tradecardcharaindex[CONNECT_WINDOWBUFSIZE];
  int joinpartycharaindex[CONNECT_WINDOWBUFSIZE];

  // CoolFish: Trade 2001/4/18
  int tradecharaindex[CONNECT_WINDOWBUFSIZE];
  int errornum;
  int fdid;

  int close_request; // the second have this

  int appendwb_overflow_flag; /* 1荚匹手appendWb互撩?仄凶日1卞允月 */

  BOOL in_watch_mode;
  BOOL b_shut_up; // for avoid the user wash the screen
  BOOL b_pass;    // for avoid the unlimited area

  struct timeval Wtime;

  struct timeval WLtime;
  BOOL b_first_warp;
  int state_trans;

  // CoolFish: Trade 2001/4/18
  char TradeTmp[256];

#ifdef _ITEM_PILEFORTRADE
  int tradelist;
#endif
  // Shan Recvdata Time

  struct timeval lastrecvtime; // 'FM' Stream Control time

  struct timeval lastrecvtime_d; // DENGON Talk Control time

  // Arminius: 6.22 encounter
  int CEP; // Current Encounter Probability
  // Arminius 7.12 login announce
  int announced;

  // shan battle delay time 2001/12/26

  struct timeval battle_recvtime;

  BOOL confirm_key; // shan  trade(DoubleCheck)

#ifdef _NEWCLISETSERVID
  int servid;
#endif

#ifdef _NEWCLISETMAC
  char mac[128];
  char mac2[128];
  char mac3[128];
#endif

  int connecttime;
  char defaultkey[32];
  char defaultdeskey[32];

#ifdef _NB_DEFAULT_KEY
  char encryptDefaultKey[32];
#endif
  int forpigcom;
} CONNECT;

CONNECT *Connect; /*コネクション瘦ち脱*/

/* 簇眶の黎片につけてわかるようにするだけのマクロ */
#define SINGLETHREAD
#define MUTLITHREAD
#define ANYTHREAD

ServerState servstate;

pthread_mutex_t MTIO_servstate_m;
#define SERVSTATE_LOCK() pthread_mutex_lock(&MTIO_servstate_m);
#define SERVSTATE_UNLOCK() pthread_mutex_unlock(&MTIO_servstate_m);
#define CONNECT_LOCK_ARG2(i, j) pthread_mutex_lock(&Connect[i].mutex);
#define CONNECT_UNLOCK_ARG2(i, j) pthread_mutex_unlock(&Connect[i].mutex);
#define CONNECT_LOCK(i) pthread_mutex_lock(&Connect[i].mutex);
#define CONNECT_UNLOCK(i) pthread_mutex_unlock(&Connect[i].mutex);
/*
#define SERVSTATE_LOCK()
#define SERVSTATE_UNLOCK()
#define CONNECT_LOCK_ARG2(i,j)
#define CONNECT_UNLOCK_ARG2(i,j)
#define CONNECT_LOCK(i)
#define CONNECT_UNLOCK(i)
*/

void SetTcpBuf(int sockfd) {

  unsigned long param = 1;
  int nRecvBuf = getrecvbuffer() * 1024;
  int nSendBuf = getsendbuffer() * 1024;
  //  int nRecvlowatBuf = getrecvlowatbuffer();
	//	int nNetTimeout=0;//0秒
  //	BOOL bDontLinger = FALSE;
  //	BOOL bReuseaddr=TRUE;

  struct linger rLinger;
	rLinger.l_onoff = 1;  // 打开linegr开关
	rLinger.l_linger = 0; // 设置延迟时间为 0 秒, 注意 TCPIP立即关闭，但是有可能出现化身

  //?�����??
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&nRecvBuf,
                 sizeof(int)) < 0)
		print("\n设置SO_RCVBUF失败!!!!\n");

  //?��?��??
  if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&nSendBuf,
                 sizeof(int)) < 0)
		print("\n设置SO_SNDBUF失败!!!!\n");
	/*
	if(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(int)) < 0)
	print( "\n设置SO_SNDTIMEO失败!!!!\n" );

	if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int)) < 0)
	print( "\n设置SO_RCVTIMEO失败!!!!\n" );

	if(setsockopt(sockfd,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(BOOL)) < 0)
	print( "\n设置SO_DONTLINGER失败!!!!\n" );

	if(setsockopt(sockfd,SOL_SOCKET ,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(BOOL)) < 0)
	print( "\n设置SO_REUSEADDR失败!!!!\n" );
	*/
  if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char *)&rLinger,
                 sizeof(rLinger)) < 0)
		print("\n设置SO_LINGER失败!!!!\n");
	/*  //设置接收缓冲下限
	if (setsockopt( sockfd, SOL_SOCKET, SO_RCVLOWAT, (char*)&nRecvlowatBuf, sizeof(int) < 0)
	print( "\n设置接收SO_RCVLOWAT失败!!!!\n" );
	*/

	//	if(ioctl(sockfd, FIONBIO, &param))
	//		print( "\n设置接收FIONBIO失败!!!!\n" );

	//	int flags=fcntl(sockfd,F_GETFL,0);//通过 F_GETFL 获取文件描述标志给flags
	//	fcntl(sockfd,F_SETFL,flags|O_NONBLOCK|O_NDELAY);//此处只是把描述符寄存位制为O_NONBLOCK
}

/*------------------------------------------------------------
* servstate毛赓渝祭允月﹝
* 娄醒﹜忒曰袄
*  卅仄
------------------------------------------------------------*/
ANYTHREAD static void SERVSTATE_initserverState(void) {
  SERVSTATE_LOCK();
  servstate.acceptmore = TRUE;
  servstate.fdid = 0;
  servstate.closeallsocketnum = -1;
  servstate.shutdown = 0;
  servstate.limittime = 0;
  servstate.dsptime = 0;
  SERVSTATE_UNLOCK();
}

ANYTHREAD int SERVSTATE_SetAcceptMore(int nvalue) {
  BOOL buf;
  SERVSTATE_LOCK();
  buf = servstate.acceptmore;
  servstate.acceptmore = nvalue;
  SERVSTATE_UNLOCK();
  return buf;
}
ANYTHREAD static int SERVSTATE_incrementFdid(void) {
  int ret;
  SERVSTATE_LOCK();
  ret = servstate.fdid++;
  SERVSTATE_UNLOCK();
  return ret;
}
ANYTHREAD static void SERVSTATE_setCloseallsocketnum(int a) {
  SERVSTATE_LOCK();
  servstate.closeallsocketnum = a;
  SERVSTATE_UNLOCK();
}
ANYTHREAD static void SERVSTATE_incrementCloseallsocketnum(void) {
  SERVSTATE_LOCK();
  servstate.closeallsocketnum++;
  SERVSTATE_UNLOCK();
}
ANYTHREAD void SERVSTATE_decrementCloseallsocketnum(void) {
  SERVSTATE_LOCK();
  servstate.closeallsocketnum--;
  SERVSTATE_UNLOCK();
}
ANYTHREAD int SERVSTATE_getCloseallsocketnum(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.closeallsocketnum;
  SERVSTATE_UNLOCK();
  return a;
}

ANYTHREAD static int SERVSTATE_getAcceptmore(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.acceptmore;
  SERVSTATE_UNLOCK();
  return a;
}
ANYTHREAD int SERVSTATE_getShutdown(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.shutdown;
  SERVSTATE_UNLOCK();
  return a;
}
ANYTHREAD void SERVSTATE_setShutdown(int a) {
  SERVSTATE_LOCK();
  servstate.shutdown = a;
  SERVSTATE_UNLOCK();
}
ANYTHREAD int SERVSTATE_getLimittime(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.limittime;
  SERVSTATE_UNLOCK();
  return a;
}
ANYTHREAD void SERVSTATE_setLimittime(int a) {
  SERVSTATE_LOCK();
  servstate.limittime = a;
  SERVSTATE_UNLOCK();
}
ANYTHREAD int SERVSTATE_getDsptime(void) {
  int a;
  SERVSTATE_LOCK();
  a = servstate.dsptime;
  SERVSTATE_UNLOCK();
  return a;
}
ANYTHREAD void SERVSTATE_setDsptime(int a) {
  SERVSTATE_LOCK();
  servstate.dsptime = a;
  SERVSTATE_UNLOCK();
}

static int appendWB(int fd, char *buf, int size) {
  if (fd != acfd) {
    if (Connect[fd].wbuse + size >= WBSIZE) {
      print("appendWB:err buffer over[%d]:%s \n", Connect[fd].wbuse + size,
            Connect[fd].cdkey);
      return -1;
    }
  } else {
    if (Connect[fd].wbuse + size >= AC_WBSIZE) {
      /*
                FILE * fp = NULL;

                if ( ( fp = fopen( "appendWBerr.log", "a+" ) ) == NULL ) return
         -1;

                fprintf( fp, "(SAAC) appendWB:err buffer over[%d+%d/%d]:\n",
         Connect[ fd ].wbuse, size, AC_WBSIZE );

                fclose( fp );
      */
      // print( "appendWB:err buffer over[%d+%d]:(SAAC) \n", Connect[ fd
      // ].wbuse, size );
      return -1;
    }
  }

  memcpy(Connect[fd].wb + Connect[fd].wbuse, buf, size);
  Connect[fd].wbuse += size;
  return size;
}
static int appendRB(int fd, char *buf, int size) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  if (fd != acfd) {
    if (Connect[fd].rbuse + size > RBSIZE) {
      // print( "appendRB:OTHER(%d) err buffer over \n", fd );
      return -1;
    }
  } else {
    if (strlen(buf) > size) {
      // print( "appendRB AC buffer len err : %d/%d=\n", strlen( buf ), size );
    }
    if (Connect[fd].rbuse + size > AC_RBSIZE) {
      // printf("%s\n", buf);
      // print( "appendRB AC err buffer over: len:%d - AC_RBSIZE:%d \n",Connect[
      // fd ].rbuse + size, AC_RBSIZE );
      return -1;
    }
  }
  memcpy(Connect[fd].rb + Connect[fd].rbuse, buf, size);
  Connect[fd].rbuse += size;
  return size;
}

static int shiftWB(int fd, int len) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  if (Connect[fd].wbuse < len) {
    print("shiftWB: err\n");
    return -1;
  }

  memmove(Connect[fd].wb, Connect[fd].wb + len, Connect[fd].wbuse - len);
  Connect[fd].wbuse -= len;

  if (Connect[fd].wbuse < 0) {
    print("shiftWB:wbuse err\n");
    Connect[fd].wbuse = 0;
  }

  return len;
}

static int shiftRB(int fd, int len) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  if (Connect[fd].rbuse < len) {
    print("shiftRB: err\n");
    return -1;
  }

  memmove(Connect[fd].rb, Connect[fd].rb + len, Connect[fd].rbuse - len);
  Connect[fd].rbuse -= len;

  if (Connect[fd].rbuse < 0) {
    print("shiftRB:rbuse err\n");
    Connect[fd].rbuse = 0;
  }

  return len;
}

SINGLETHREAD int lsrpcClientWriteFunc(int fd, char *buf, int size) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  int r;
  if (Connect[fd].use == FALSE) {
    return FALSE;
  }

  if (Connect[fd].appendwb_overflow_flag) {
    print("lsrpcClientWriteFunc: buffer overflow fd:%d\n", fd);
    return -1;
  }

  r = appendWB(fd, buf, size);
  if (r < 0) {
    int ret;
    if (fd == acfd) {
      alarm(1);
      ret = write(fd, Connect[fd].wb,
                  (Connect[fd].wbuse < getAcwriteSize()) ? Connect[fd].wbuse
                                                         : getAcwriteSize());
      alarm(0);
      if (ret == -1 && errno == EINTR) {
        ;
      } else if (ret == -1) {
        return -1;
      } else if (ret > 0) {
        shiftWB(fd, ret);
      }
      return lsrpcClientWriteFunc(fd, buf, size);
    }
  }
  // Nuke *1 0907: Ignore acfd from WB error
  if ((r < 0) && (fd != acfd)) {
    Connect[fd].appendwb_overflow_flag = 1;
    CONNECT_endOne_debug(fd);
    // Nuke + 1 0901: Why close
    //  print("closed in lsrpcClientWriteFunc");
  }
  return r;
}

static int logRBuseErr = 0;
SINGLETHREAD BOOL GetOneLine_fix(int fd, char *buf, int max) {
#ifdef _NB_NET
  nb_checkfd(fd, FALSE);
#endif
  int i;
  if (Connect[fd].rbuse == 0)
    return FALSE;
  if (Connect[fd].check_rb_oneline_b == 0 &&
      Connect[fd].check_rb_oneline_b == Connect[fd].rbuse) {
    return FALSE;
  }
  for (i = 0; i < Connect[fd].rbuse && i < (max - 1); i++) {
    if (Connect[fd].rb[i] == '\n') {
      memcpy(buf, Connect[fd].rb, i + 1);
      buf[i + 1] = '\0';
      shiftRB(fd, i + 1);
      logRBuseErr = 0;
      Connect[fd].check_rb_oneline_b = 0;
      Connect[fd].check_rb_time = 0;
      return TRUE;
    }
  }

  logRBuseErr++;
  // if( logRBuseErr >= 50 ) {
  // Connect[ fd ].rb[Connect[ fd ].rbuse] = '\n';
  // printf("rebuse err %d:%d\n", logRBuseErr, Connect[ fd ].rbuse);
  // logRBuseErr = 0;
  //}
  if (fd == acfd && strstr(Connect[fd].rb, "ACCharLoad") != NULL &&
      logRBuseErr >= 50) {
    char buf[AC_RBSIZE];
    memcpy(buf, Connect[fd].rb, Connect[fd].rbuse + 1);
    buf[Connect[fd].rbuse + 1] = 0;
    LogAcMess(fd, "RBUFFER", buf);
    logRBuseErr = 0;
  }

  Connect[fd].check_rb_oneline_b = Connect[fd].rbuse;

  return FALSE;
}

char *getRandStr(int num) {
  char str[num + 1];
  int i;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
  for (i = 0; i < num; i++) {
    int a = 0;
    a = (rand() % 2) ? (rand() % 26 + 65) : (rand() % 10 + 48);
    str[i] = a;
  }
  str[num] = '\0';
  return str;
}

ANYTHREAD BOOL initConnectOne(int sockfd, struct sockaddr_in *sin, int len) {
  CONNECT_LOCK(sockfd);

  Connect[sockfd].use = TRUE;
  Connect[sockfd].ctype = NOTDETECTED;
  Connect[sockfd].wbuse = Connect[sockfd].rbuse = 0;
  Connect[sockfd].check_rb_oneline_b = 0;
  Connect[sockfd].check_rb_time = 0;

  memset(Connect[sockfd].cdkey, 0, sizeof(Connect[sockfd].cdkey));
  memset(Connect[sockfd].passwd, 0, sizeof(Connect[sockfd].passwd));

  Connect[sockfd].state = NULLCONNECT;
  Connect[sockfd].nstatecount = 0;
  memset(Connect[sockfd].charname, 0, sizeof(Connect[sockfd].charname));
  Connect[sockfd].charaindex = -1;

  Connect[sockfd].CAbufsiz = 0;
  Connect[sockfd].CDbufsiz = 0;
  memset(Connect[sockfd].rb, 0, RBSIZE);
  memset(Connect[sockfd].wb, 0, WBSIZE);
  Connect[sockfd].rbuse = 0;
  Connect[sockfd].wbuse = 0;
  Connect[sockfd].check_rb_oneline_b = 0;
  Connect[sockfd].check_rb_time = 0;

  Connect[sockfd].close_request = 0;
  Connect[sockfd].Walktime = 0;
  Connect[sockfd].lastWalktime = 0;
  Connect[sockfd].Walkcount = 0;
  Connect[sockfd].Walkspool = WALK_SPOOL;
  Connect[sockfd].Walkrestore = WALK_RESTORE;
  Connect[sockfd].Btime = 0;
  Connect[sockfd].lastBtime = 0;
  Connect[sockfd].lastlastBtime = 0;
  Connect[sockfd].EOtime = 0;
  Connect[sockfd].nu_decrease = 0;
#ifdef _BATTLE_TIMESPEED
  // Connect[sockfd].DefBtime = 0;
  Connect[sockfd].BDTime = 0;
  Connect[sockfd].CBTime = 0;
#endif
#ifdef _TYPE_TOXICATION
  Connect[sockfd].toxication = 0;
#endif
#ifdef _ITEM_ADDEXP // vincent ??��
  Connect[sockfd].EDTime = 0;
#endif
  //      Connect[sockfd].BEO = 0;
  Connect[sockfd].BEOspool = BEO_SPOOL;
  Connect[sockfd].BEOrestore = BEO_RESTORE;
  // ttom
  Connect[sockfd].b_shut_up = FALSE;
  Connect[sockfd].Wtime.tv_sec = 0;
  Connect[sockfd].Wtime.tv_usec = 0;
  Connect[sockfd].WLtime.tv_sec = 0;
  Connect[sockfd].WLtime.tv_usec = 0;
  Connect[sockfd].b_first_warp = FALSE;
  Connect[sockfd].state_trans = 0;

  Connect[sockfd].credit = 3;
  Connect[sockfd].fcold = 0;
  Connect[sockfd].nu = 30;

  Connect[sockfd].packetin = 30;
  Connect[sockfd].cotime = 0;
  Connect[sockfd].noenemy = 0;
  Connect[sockfd].eqnoenemy = 0;

#ifdef _Item_MoonAct
  Connect[sockfd].eqrandenemy = 0;
#endif
#ifdef _CHIKULA_STONE
  Connect[sockfd].chistone = 0;
#endif
  Connect[sockfd].stayencount = 0;

  memset(&Connect[sockfd].TradeTmp, 0, sizeof(Connect[sockfd].TradeTmp));
#ifdef _ITEM_PILEFORTRADE
  Connect[sockfd].tradelist = -1;
#endif
  Connect[sockfd].CEP = 0;
  Connect[sockfd].announced = 0;

  Connect[sockfd].confirm_key = FALSE;

  if (sin != NULL)
    memcpy(&Connect[sockfd].sin, sin, len);

  memset(&Connect[sockfd].lastprocesstime, 0,
         sizeof(Connect[sockfd].lastprocesstime));
  memcpy(&Connect[sockfd].lastCAsendtime, &NowTime,
         sizeof(Connect[sockfd].lastCAsendtime));
  memcpy(&Connect[sockfd].lastCDsendtime, &NowTime,
         sizeof(Connect[sockfd].lastCDsendtime));
  memcpy(&Connect[sockfd].lastCharSaveTime, &NowTime,
         sizeof(Connect[sockfd].lastCharSaveTime));
  memcpy(&Connect[sockfd].lastrecvtime, &NowTime,
         sizeof(Connect[sockfd].lastrecvtime));
  memcpy(&Connect[sockfd].lastrecvtime_d, &NowTime,
         sizeof(Connect[sockfd].lastrecvtime_d));
  memcpy(&Connect[sockfd].battle_recvtime, &NowTime,
         sizeof(Connect[sockfd].battle_recvtime));
  memcpy(&Connect[sockfd].lastreadtime, &NowTime, sizeof(struct timeval));

  Connect[sockfd].lastreadtime.tv_sec -= DEBUG_ADJUSTTIME;
  Connect[sockfd].errornum = 0;
  Connect[sockfd].fdid = SERVSTATE_incrementFdid();
  CONNECT_UNLOCK(sockfd);
  Connect[sockfd].appendwb_overflow_flag = 0;
  Connect[sockfd].connecttime = time(NULL);

  memset(Connect[sockfd].mac, 0, sizeof(Connect[sockfd].mac));
  memset(Connect[sockfd].mac2, 0, sizeof(Connect[sockfd].mac2));
  memset(Connect[sockfd].mac3, 0, sizeof(Connect[sockfd].mac3));
  memset(Connect[sockfd].defaultkey, 0, sizeof(Connect[sockfd].defaultkey));
  memset(Connect[sockfd].defaultdeskey, 0,
         sizeof(Connect[sockfd].defaultdeskey));
  int deskeyrand = RAND(1, 100000);
  strcpy(Connect[sockfd].defaultkey, firstdesToken[deskeyrand - 1]);
  strcpy(Connect[sockfd].defaultdeskey, seconddesToken[deskeyrand - 1]);

  return TRUE;
}

ANYTHREAD BOOL _CONNECT_endOne(char *file, int fromline, int sockfd, int line) {
  CONNECT_LOCK_ARG2(sockfd, line);

  if (Connect[sockfd].use == FALSE) {
    CONNECT_UNLOCK_ARG2(sockfd, line); //?���??
    return TRUE;
  }
  Connect[sockfd].use = FALSE;
  if (Connect[sockfd].ctype == CLI &&
      CHAR_CHECKINDEX(Connect[sockfd].charaindex) == TRUE) {
    CONNECT_UNLOCK_ARG2(sockfd, line);
#ifdef _OFFLINE_SYSTEM
    if (CHAR_getWorkInt(Connect[sockfd].charaindex, CHAR_WORK_OFFLINE) != 0) {
      CHAR_setWorkInt(Connect[sockfd].charaindex, CHAR_WORKFD, -1);
    }
#ifdef _OFFLINE_BATTLE_RETEUN //??��?
    else if (CHAR_getWorkInt(Connect[sockfd].charaindex, CHAR_WORKBATTLEINDEX) >
                 -1 &&
             (BattleArray[CHAR_getWorkInt(Connect[sockfd].charaindex,
                                          CHAR_WORKBATTLEINDEX)]
                      .type == BATTLE_TYPE_P_vs_P ||
              (CHAR_getInt(Connect[sockfd].charaindex, CHAR_FLOOR) >= 40000 &&
               CHAR_getInt(Connect[sockfd].charaindex, CHAR_FLOOR) <= 40100)) &&
             CHAR_getFlg(Connect[sockfd].charaindex, CHAR_ISDIE) != 1) {
      // print("\nPK����ت����?��");
      CHAR_setWorkInt(Connect[sockfd].charaindex, CHAR_WORK_OFFLINE, 1);
      CHAR_setWorkInt(Connect[sockfd].charaindex, CHAR_WORKFD, -1);
    }
#endif
    else
#endif
    {
      if (!CHAR_logout(Connect[sockfd].charaindex, TRUE)) {
        // print( "err %s:%d from %s:%d \n", __FILE__, __LINE__, file,
        // fromline);
      }
      print("����??=%s \n", Connect[sockfd].cdkey);
    }
    CONNECT_LOCK_ARG2(sockfd, line);
  } else {
    if (strlen(Connect[sockfd].cdkey) > 0) {
      if (sasql_queryonline(Connect[sockfd].cdkey) == getServernumber())
        sasql_online(Connect[sockfd].cdkey, NULL, NULL, NULL, NULL, NULL, 0);
    }
  }
  Connect[sockfd].charaindex = -1;
  Connect[sockfd].wbuse = 0;
  Connect[sockfd].rbuse = 0;
  Connect[sockfd].CAbufsiz = 0;
  Connect[sockfd].CDbufsiz = 0;
  memset(Connect[sockfd].mac, 0, sizeof(Connect[sockfd].mac));
  memset(Connect[sockfd].mac2, 0, sizeof(Connect[sockfd].mac2));
  memset(Connect[sockfd].mac3, 0, sizeof(Connect[sockfd].mac3));
  CONNECT_UNLOCK_ARG2(sockfd, line);

  // print( "??=%d %s ?������:%s ��?:%d\n",
  // sockfd,inet_ntoa(Connect[sockfd].sin.sin_addr ), file, fromline);
  close(sockfd);
  return TRUE;
}

SINGLETHREAD BOOL initConnect(int size) {
  int i, j;
  ConnectLen = size;
  Connect = calloc(1, sizeof(CONNECT) * size);

  if (Connect == NULL)
    return FALSE;

  for (i = 0; i < size; i++) {
    memset(&Connect[i], 0, sizeof(CONNECT));
    Connect[i].charaindex = -1;
    Connect[i].rb = calloc(1, RBSIZE);

    if (Connect[i].rb == NULL) {
      fprint("calloc err\n");
      for (j = 0; j < i; j++) {
        free(Connect[j].rb);
        free(Connect[j].wb);
      }
      return FALSE;
    }
    memset(Connect[i].rb, 0, RBSIZE);
    Connect[i].wb = calloc(1, WBSIZE);
    if (Connect[i].wb == NULL) {
      fprint("calloc err\n");
      for (j = 0; j < i; j++) {
        free(Connect[j].rb);
        free(Connect[j].wb);
      }
      free(Connect[j].rb);
      return FALSE;
    }
    memset(Connect[i].wb, 0, WBSIZE);
  }

	print("预约 %d 接连...分配 %.2f MB 空间...", size, (sizeof(CONNECT) * size + RBSIZE * size + WBSIZE * size) / 1024.0 / 1024.0);
  SERVSTATE_initserverState();
  MAX_item_use = getItemnum() * 0.98;
  return TRUE;
}
BOOL CONNECT_acfdInitRB(int fd) {
#ifdef _NB_NET
  nb_checkfd(fd, FALSE);
#endif
  if (fd != acfd)
    return FALSE;
  Connect[fd].rb = realloc(Connect[acfd].rb, AC_RBSIZE);
  if (Connect[acfd].rb == NULL) {
    fprint("realloc err\n");
    return FALSE;
  }
  memset(Connect[acfd].rb, 0, AC_RBSIZE);
  return TRUE;
}

BOOL CONNECT_acfdInitWB(int fd) {
#ifdef _NB_NET
  nb_checkfd(fd, FALSE);
#endif
  if (fd != acfd)
    return FALSE;
  Connect[fd].wb = realloc(Connect[acfd].wb, AC_WBSIZE);
  if (Connect[acfd].wb == NULL) {
    fprint("realloc err\n");
    return FALSE;
  }
  memset(Connect[acfd].wb, 0, AC_WBSIZE);
  return TRUE;
}

ANYTHREAD void endConnect(void) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    int lco;
    lco = close(i);
    if (lco == 0) {
      CONNECT_endOne_debug(i);
    }
    free(Connect[i].rb);
    free(Connect[i].wb);
  }
  free(Connect);
}

ANYTHREAD BOOL CONNECT_appendCAbuf(int fd, char *data, int size) {
#ifdef _NB_NET
  nb_checkfd(fd, FALSE);
#endif
#ifdef _MASK_ENCOUNTER //��?
  int charaindex = CONNECT_getCharaindex(fd);
  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
    if (getStayEncount(fd))
      return FALSE;
  } else {
    int oyaindex = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1);
    if (getStayEncount(getfdFromCharaIndex(oyaindex)))
      return FALSE;
  }
#endif
  CONNECT_LOCK(fd);
  if ((Connect[fd].CAbufsiz + size) >= sizeof(Connect[fd].CAbuf)) {
    CONNECT_UNLOCK(fd);
    return FALSE;
  }
  memcpy(Connect[fd].CAbuf + Connect[fd].CAbufsiz, data, size);
  Connect[fd].CAbuf[Connect[fd].CAbufsiz + size] = ',';
  Connect[fd].CAbufsiz += (size + 1);
  CONNECT_UNLOCK(fd);
  return TRUE;
}

ANYTHREAD static int CONNECT_getCAbuf(int fd, char *out, int outmax,
                                      int *outlen) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  CONNECT_LOCK(fd);
  if (Connect[fd].use == TRUE) {
    int cplen = MIN(outmax, Connect[fd].CAbufsiz);
    memcpy(out, Connect[fd].CAbuf, cplen);
    *outlen = cplen;
    CONNECT_UNLOCK(fd);
    return 0;
  } else {
    CONNECT_UNLOCK(fd);
    return -1;
  }
}
ANYTHREAD static int CONNECT_getCDbuf(int fd, char *out, int outmax,
                                      int *outlen) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  CONNECT_LOCK(fd);
  if (Connect[fd].use == TRUE) {
    int cplen = MIN(outmax, Connect[fd].CDbufsiz);
    memcpy(out, Connect[fd].CDbuf, cplen);
    *outlen = cplen;
    CONNECT_UNLOCK(fd);
    return 0;
  } else {
    CONNECT_UNLOCK(fd);
    return 0;
  }
}

ANYTHREAD static int CONNECT_setCAbufsiz(int fd, int len) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  CONNECT_LOCK(fd);

  if (Connect[fd].use == TRUE) {
    Connect[fd].CAbufsiz = len;
    CONNECT_UNLOCK(fd);
    return 0;
  } else {
    CONNECT_UNLOCK(fd);
    return -1;
  }
}
ANYTHREAD static int CONNECT_setCDbufsiz(int fd, int len) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  CONNECT_LOCK(fd);

  if (Connect[fd].use == TRUE) {
    Connect[fd].CDbufsiz = len;
    CONNECT_UNLOCK(fd);
    return 0;
  } else {
    CONNECT_UNLOCK(fd);
    return -1;
  }
}

ANYTHREAD static void CONNECT_setLastCAsendtime(int fd, struct timeval *t) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].lastCAsendtime = *t;
  CONNECT_UNLOCK(fd);
}

ANYTHREAD static void CONNECT_getLastCAsendtime(int fd, struct timeval *t) {
  if (fd < 0 || fd >= ConnectLen) {
    t->tv_sec = 0;
    t->tv_usec = 0;
    return;
  }
  CONNECT_LOCK(fd);
  *t = Connect[fd].lastCAsendtime;
  CONNECT_UNLOCK(fd);
}

ANYTHREAD static void CONNECT_setLastCDsendtime(int fd, struct timeval *t) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].lastCDsendtime = *t;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD static void CONNECT_getLastCDsendtime(int fd, struct timeval *t) {
  if (fd < 0 || fd >= ConnectLen) {
    t->tv_sec = 0;
    t->tv_usec = 0;
    return;
  }
  CONNECT_LOCK(fd);
  *t = Connect[fd].lastCDsendtime;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getUse_debug(int fd, int i) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  int a;
  if (0 > fd || fd >= ConnectLen) {
    return 0;
  }
  CONNECT_LOCK_ARG2(fd, i);
  a = Connect[fd].use;
  CONNECT_UNLOCK_ARG2(fd, i);
  return a;
}

ANYTHREAD int CONNECT_getUse(int fd) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  int a;
  if (0 > fd || fd >= ConnectLen) {
    return 0;
  }
  CONNECT_LOCK(fd);
  a = Connect[fd].use;
  CONNECT_UNLOCK(fd);
  return a;
}
void CONNECT_setUse(int fd, int a) {
#ifdef _NB_NET
  nb_checkfdnull(fd);
#endif
  CONNECT_LOCK(fd);
  Connect[fd].use = a;
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_checkStatecount(int a) {
  int i;
  int count = 0;
  for (i = 0; i < ConnectLen; i++) {
    if (Connect[i].use == FALSE || Connect[i].state != a)
      continue;
    if (Connect[i].nstatecount <= 0) {
      Connect[i].nstatecount = (int)time(NULL) + 60;
    } else {
      if (Connect[i].nstatecount < (int)time(NULL)) {
        CONNECT_endOne_debug(i);
        count++;
      }
    }
  }

  {
    memset(StateTable, 0, sizeof(StateTable));
    for (i = 0; i < ConnectLen; i++)
      if (Connect[i].use == TRUE)
        StateTable[Connect[i].state]++;
  }
}

ANYTHREAD void CONNECT_setState(int fd, int a) {
#ifdef _NB_NET
  nb_checkfdnull(fd);
#endif
  CONNECT_LOCK(fd);
  Connect[fd].state = a;
  Connect[fd].nstatecount = 0;

  {
    char temp[128], buffer[256];
    int i;
    memset(StateTable, 0, sizeof(StateTable));

    for (i = 0; i < ConnectLen; i++) {
      if (Connect[i].use == TRUE) {
        if (Connect[i].state > WHILESAVEWAIT)
          continue;
        StateTable[Connect[i].state]++;
      }
    }

    buffer[0] = 0;

    for (i = 0; i <= WHILESAVEWAIT; i++) {
      sprintf(temp, "%-6d", StateTable[i]);
      strcat(buffer, temp);
    }

    // print( "\n{{%s}}", buffer );//������ʫ��?����
  }
  CONNECT_UNLOCK(fd);
}

ANYTHREAD int CONNECT_getState(int fd) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].state;
  CONNECT_UNLOCK(fd);
  return a;
}

ANYTHREAD void CONNECT_setCharaindex(int fd, int a) {
#ifdef _NB_NET
  nb_checkfdnull(fd);
#endif
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].charaindex = a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getCharaindex(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].charaindex;
  CONNECT_UNLOCK(fd);
  return a;
}
ANYTHREAD void CONNECT_getCdkey(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    out[0] = '\0';
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].cdkey);
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_setCdkey(int fd, char *cd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  snprintf(Connect[fd].cdkey, sizeof(Connect[fd].cdkey), "%s", cd);
  CONNECT_UNLOCK(fd);
}

#ifdef _NEWCLISETSERVID
ANYTHREAD void CONNECT_setServid(int fd, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].servid = a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getServid(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].servid;
  CONNECT_UNLOCK(fd);
  return a;
}
#endif

#ifdef _NEWCLISETMAC
ANYTHREAD void CONNECT_getMAC(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    out[0] = '\0';
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].mac);
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_getMAC2(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    out[0] = '\0';
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].mac2);
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_getMAC3(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    out[0] = '\0';
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].mac3);
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_setMAC(int fd, char *in) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].mac, sizeof(Connect[fd].mac), in);
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_setMAC2(int fd, char *in) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].mac2, sizeof(Connect[fd].mac2), in);
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_setMAC3(int fd, char *in) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].mac3, sizeof(Connect[fd].mac3), in);
  CONNECT_UNLOCK(fd);
}
#endif

ANYTHREAD void CONNECT_getPasswd(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    out[0] = '\0';
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].passwd);
  CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_setPasswd(int fd, char *in) {
#ifdef _NB_NET
  nb_checkfdnull(fd);
#endif
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].passwd, sizeof(Connect[fd].passwd), in);
  CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getCtype(int fd) {
#ifdef _NB_NET
  nb_checkfd(fd, -1);
#endif
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].ctype;
  CONNECT_UNLOCK(fd);
  return a;
}
ANYTHREAD void CONNECT_setCtype(int fd, int a) {
#ifdef _NB_NET
  nb_checkfdnull(fd);
#endif
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].ctype = a;
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_getCharname(int fd, char *out, int outlen) {
  if (fd < 0 || fd >= ConnectLen) {
    out[0] = '\0';
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(out, outlen, Connect[fd].charname);
  CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_setCharname(int fd, char *in) {
#ifdef _NB_NET
  nb_checkfdnull(fd);
#endif
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].charname, sizeof(Connect[fd].charname), in);
  CONNECT_UNLOCK(fd);
}

ANYTHREAD int CONNECT_getFdid(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].fdid;
  CONNECT_UNLOCK(fd);
  return a;
}
ANYTHREAD void CONNECT_setDuelcharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].duelcharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getDuelcharaindex(int fd, int i) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].duelcharaindex[i];
  CONNECT_UNLOCK(fd);
  return a;
}
ANYTHREAD void CONNECT_setBattlecharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].battlecharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getBattlecharaindex(int fd, int i) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].battlecharaindex[i];
  CONNECT_UNLOCK(fd);
  return a;
}
ANYTHREAD void CONNECT_setJoinpartycharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].joinpartycharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getJoinpartycharaindex(int fd, int i) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);

  a = Connect[fd].joinpartycharaindex[i];
  CONNECT_UNLOCK(fd);
  return a;
}

// CoolFish: Trade 2001/4/18
ANYTHREAD void CONNECT_setTradecharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].tradecharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}

// Shan Begin
ANYTHREAD void CONNECT_setLastrecvtime(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].lastrecvtime = *a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getLastrecvtime(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  *a = Connect[fd].lastrecvtime;
  CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_setLastrecvtime_D(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].lastrecvtime_d = *a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getLastrecvtime_D(int fd, struct timeval *a) {
  CONNECT_LOCK(fd);
  *a = Connect[fd].lastrecvtime_d;
  CONNECT_UNLOCK(fd);
}

// 2001/12/26
ANYTHREAD void CONNECT_SetBattleRecvTime(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].battle_recvtime = *a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_GetBattleRecvTime(int fd, struct timeval *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  *a = Connect[fd].battle_recvtime;
  CONNECT_UNLOCK(fd);
}
// Shan End

#ifdef _ITEM_PILEFORTRADE
ANYTHREAD void CONNECT_setTradeList(int fd, int num) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].tradelist = num;
}
ANYTHREAD int CONNECT_getTradeList(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].tradelist;
}
#endif

ANYTHREAD void CONNECT_setTradeTmp(int fd, char *a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(Connect[fd].TradeTmp, sizeof(Connect[fd].TradeTmp), a);

  CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getTradeTmp(int fd, char *trademsg, int trademsglen) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  strcpysafe(trademsg, trademsglen, Connect[fd].TradeTmp);
  CONNECT_UNLOCK(fd);
}

#ifdef _NB_DEFAULT_KEY
ANYTHREAD void CONNECT_getEncryptDefaultKey(int fd, char *buf, int buflen) {
  CONNECT_LOCK(fd);
  strcpysafe(buf, buflen, Connect[fd].encryptDefaultKey);
  CONNECT_UNLOCK(fd);
}
#endif

ANYTHREAD void CONNECT_setTradecardcharaindex(int fd, int i, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  CONNECT_LOCK(fd);
  Connect[fd].joinpartycharaindex[i] = a;
  CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getTradecardcharaindex(int fd, int i) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int a;
  CONNECT_LOCK(fd);
  a = Connect[fd].joinpartycharaindex[i];
  CONNECT_UNLOCK(fd);
  return a;
}

ANYTHREAD void CONNECT_setCloseRequest(int fd, int count) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  //		print("\n???ϴ?��? FILE:%s,LINE:%d ", file, line);
  CONNECT_LOCK(fd);
  Connect[fd].close_request = count;
  // Nuke
  //    print("\n???ϴ?��? %d ",fd);
  CONNECT_UNLOCK(fd);
}

/*------------------------------------------------------------
* CAcheck などに蝗われる簇眶。悸狠に流る。
* 苞眶
*  fd      int     ファイルディスクリプタ
* 手り猛
*  なし
------------------------------------------------------------*/
ANYTHREAD void CAsend(int fd) {
  char buf[sizeof(Connect[0].CAbuf)];
  int bufuse = 0;

  if (CONNECT_getCAbuf(fd, buf, sizeof(buf), &bufuse) < 0)
    return;
  if (bufuse == 0)
    return;
  buf[bufuse - 1] = '\0';
  lssproto_CA_send(fd, buf);

  CONNECT_setCAbufsiz(fd, 0);
}

/*------------------------------------------------------------
* CAを流る。
* 苞眶
* 手り猛
*  なし
------------------------------------------------------------*/
ANYTHREAD void CAcheck(void) {
  int i;
#ifdef _SYSTEM_SPEAD
  unsigned int interval_us = getCAsendinterval_ms() * 200;
#else
  unsigned int interval_us = getCAsendinterval_ms() * 1000;
#endif

  /* Connect��?�?��?��������������?�� */
  for (i = 0; i < ConnectLen; i++) {
    struct timeval t;
    if (!CONNECT_getUse_debug(i, 1008))
      continue;
    CONNECT_getLastCAsendtime(i, &t);
    if (time_diff_us(NowTime, t) > interval_us) {
      CAsend(i);
      CONNECT_setLastCAsendtime(i, &NowTime);
    }
  }
}
ANYTHREAD void CAflush(int charaindex) {
  int i;
  i = getfdFromCharaIndex(charaindex);
  if (i == -1)
    return;
  CAsend(i);
}

ANYTHREAD BOOL CONNECT_appendCDbuf(int fd, char *data, int size) {
  CONNECT_LOCK(fd);

  if ((Connect[fd].CDbufsiz + size) >= sizeof(Connect[fd].CDbuf)) {
    CONNECT_UNLOCK(fd);
    return FALSE;
  }
  memcpy(Connect[fd].CDbuf + Connect[fd].CDbufsiz, data, size);
  Connect[fd].CDbuf[Connect[fd].CDbufsiz + size] = ',';
  Connect[fd].CDbufsiz += (size + 1);
  CONNECT_UNLOCK(fd);
  return TRUE;
}

ANYTHREAD void CDsend(int fd) {
  char buf[sizeof(Connect[0].CAbuf)];
  int bufuse = 0;
  if (CONNECT_getCDbuf(fd, buf, sizeof(buf), &bufuse) < 0)
    return;
  if (bufuse == 0)
    return;
  buf[bufuse - 1] = '\0';
  lssproto_CD_send(fd, buf);
  CONNECT_setCDbufsiz(fd, 0);
}

ANYTHREAD void CDcheck(void) {
  int i;
  unsigned int interval_us = getCDsendinterval_ms() * 1000;
  for (i = 0; i < ConnectLen; i++) {
    struct timeval t;
    if (!CONNECT_getUse_debug(i, 1082))
      continue;
    CONNECT_getLastCDsendtime(i, &t);
    if (time_diff_us(NowTime, t) > interval_us) {
      CDsend(i);
      CONNECT_setLastCDsendtime(i, &NowTime);
    }
  }
}

ANYTHREAD void CDflush(int charaindex) {
  int i;
  i = getfdFromCharaIndex(charaindex);
  if (i == -1)
    return;
  CDsend(i);
}

void chardatasavecheck(void) {
  int i;
  int interval = getCharSavesendinterval();
  static struct timeval chardatasavecheck_store;
  if (NowTime.tv_sec > (chardatasavecheck_store.tv_sec + 10)) {
    chardatasavecheck_store = NowTime;
#ifdef _FIX_SAVE_CHAR
    int playernum = CHAR_getPlayerMaxNum();
    for (i = 0; i < playernum; i++) {
      if (CHAR_CHECKINDEX(i) == FALSE)
        continue;
      int tv_sec = NowTime.tv_sec;
      if (CHAR_getWorkInt(i, CHAR_WORK_SAVE_CHAR) == 0) {
        CHAR_setWorkInt(i, CHAR_WORK_SAVE_CHAR, tv_sec);
      } else {
        if (tv_sec - CHAR_getWorkInt(i, CHAR_WORK_SAVE_CHAR) > interval) {
          CHAR_charSaveFromConnect(i, FALSE);
          CHAR_setWorkInt(i, CHAR_WORK_SAVE_CHAR, tv_sec);
        }
      }
    }
#else
    for (i = 0; i < ConnectLen; i++) {
      CONNECT_LOCK(i);
      if (Connect[i].use == TRUE && Connect[i].state == LOGIN &&
          NowTime.tv_sec - Connect[i].lastCharSaveTime.tv_sec > interval) {
        Connect[i].lastCharSaveTime = NowTime;
        CONNECT_UNLOCK(i);
        CHAR_charSaveFromConnect(CONNECT_getCharaindex(i), FALSE);
      } else {
        CONNECT_UNLOCK(i);
      }
    }
#endif
  } else {
    ;
  }
}

ANYTHREAD int CONNECT_checkfd(int fd) {
  if (0 > fd || fd >= ConnectLen) {
    return FALSE;
  }
  CONNECT_LOCK(fd);
  if (Connect[fd].use == FALSE) {
    CONNECT_UNLOCK(fd);
    return FALSE;
  } else {
    CONNECT_UNLOCK(fd);
    return TRUE;
  }
}

ANYTHREAD int getfdFromCdkey(char *cd) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && strcmp(Connect[i].cdkey, cd) == 0) {
      CONNECT_UNLOCK(i);
      return i;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

ANYTHREAD int getfdFromCharaIndex(int charaindex) {
#if 1
  int ret;
  if (!CHAR_CHECKINDEX(charaindex))
    return -1;
  if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
    return -1;
  ret = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  if (ret < 0 || ret >= ConnectLen)
    return -1;
  return ret;
#else
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].charaindex == charaindex) {
      CONNECT_UNLOCK(i);
      return i;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
#endif
}

ANYTHREAD int getcdkeyFromCharaIndex(int charaindex, char *out, int outlen) {
  int i;

  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].charaindex == charaindex) {
      snprintf(out, outlen, "%s", Connect[i].cdkey);
      CONNECT_UNLOCK(i);
      return 0;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

ANYTHREAD int getfdFromFdid(int fdid) {
  int i;

  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].fdid == fdid) {
      CONNECT_UNLOCK(i);
      return i;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

ANYTHREAD int getCharindexFromFdid(int fdid) {
  int i;

  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].fdid == fdid &&
        Connect[i].charaindex >= 0) {
      int a = Connect[i].charaindex;
      CONNECT_UNLOCK(i);
      return a;
    }
    CONNECT_UNLOCK(i);
  }

  return -1;
}

ANYTHREAD int getFdidFromCharaIndex(int charind) {
  int i;

  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE && Connect[i].charaindex == charind) {
      int a = Connect[i].fdid;
      CONNECT_UNLOCK(i);
      return a;
    }
    CONNECT_UNLOCK(i);
  }

  return -1;
}

ANYTHREAD BOOL CONNECT_isCLI(int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].ctype == CLI ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

ANYTHREAD BOOL CONNECT_isAC(int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].ctype == AC ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

ANYTHREAD BOOL CONNECT_isUnderLogin(int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].state == LOGIN ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

ANYTHREAD BOOL CONNECT_isWhileLogin(int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].state == WHILELOGIN ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

ANYTHREAD BOOL CONNECT_isNOTLOGIN(int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].state == NOTLOGIN ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

ANYTHREAD BOOL CONNECT_isLOGIN(int fd) {
  int a;
  CONNECT_LOCK(fd);
  a = (Connect[fd].state == LOGIN ? TRUE : FALSE);
  CONNECT_UNLOCK(fd);
  return a;
}

void closeAllConnectionandSaveData(void) {
  int i;
  int num;
  SERVSTATE_setCloseallsocketnum(0);
  for (i = 0; i < ConnectLen; i++) {
    if (CONNECT_getUse_debug(i, 1413) == TRUE) {
      BOOL clilogin = FALSE;
      if (CONNECT_isAC(i))
        continue;
      if (CONNECT_isCLI(i) && CONNECT_isLOGIN(i))
        clilogin = TRUE;
      CONNECT_endOne_debug(i);
      if (clilogin) {
        CONNECT_setUse(i, TRUE);
        CONNECT_setState(i, WHILECLOSEALLSOCKETSSAVE);
        SERVSTATE_incrementCloseallsocketnum();
      }
    }
  }
  num = SERVSTATE_getCloseallsocketnum();
  if (num == 0) {
    SERVSTATE_SetAcceptMore(-1);
  } else {
    SERVSTATE_SetAcceptMore(0);
  }
	print("\n发送人物数据数目:%d\n", num);
}

// andy_add 2003/02/12
void CONNECT_SysEvent_Loop(void) {
  static time_t checkT = 0;
  static int chikulatime = 0;
  int NowTimes = time(NULL);
  if (checkT != NowTimes && (checkT + 10) <= NowTimes) { //ÿ10�����
    int i;
    checkT = time(NULL);
    chikulatime++;
    if (chikulatime > 10000)
      chikulatime = 0;
    int playernum = CHAR_getPlayerMaxNum();
    int charaindex;
    for (charaindex = 0; charaindex < playernum; charaindex++) {
      if (!CHAR_CHECKINDEX(charaindex))
        continue;

      if (CHAR_getInt(charaindex, CHAR_EVOLUTION_STATS) > 0) {
        if (NowTimes > CHAR_getInt(charaindex, CHAR_EVOLUTION_STATS)) {
          CHAR_talkToCli(charaindex, -1, "TIMES UP", CHAR_COLORRED);
          CHAR_warpToSpecificPoint(charaindex, 1006, 16, 22); //���̵� Ư���̵� ����
          CHAR_setInt(charaindex, CHAR_EVOLUTION_STATS, 0);
        }
      }
#ifdef _CANCEL_STREET_VENDOR
      char token[256];
      if (CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR) ==
          3) { //�鿴��̯���۵�״̬
        if (CHAR_getWorkInt(charaindex, CHAR_WORK_STREET_VENDOR_TIME) <
            time(NULL) - 90) { // 90��
          int toindex = CHAR_getWorkInt(charaindex, CHAR_WORKSTREETVENDOR_WHO);
          // �����趨Ϊ��̯,�����״̬
          CHAR_setWorkInt(charaindex, CHAR_WORKSTREETVENDOR, -1);
          CHAR_setWorkInt(toindex, CHAR_WORKSTREETVENDOR, 1);
          // �������ͼʾ
          CHAR_sendTradeEffect(charaindex, 0);
          CHAR_sendTradeEffect(toindex, 0);
          CHAR_setWorkInt(toindex, CHAR_WORKSTREETVENDOR_WHO, -1);
          CHAR_setWorkInt(charaindex, CHAR_WORKSTREETVENDOR_WHO, -1);
          lssproto_STREET_VENDOR_send(charaindex, "C|");
          CHAR_talkToCli(charaindex, -1,
                         "查看擺攤時間過長,賣傢自動取消你的查看",
                         CHAR_COLORYELLOW);
        }
      }
#endif
      if (chikulatime % 6 == 0) { // 每60秒
#ifdef _PETSKILL_BECOMEPIG
        if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1) { //處於烏力化狀態
          if ((CHAR_getInt(charaindex, CHAR_BECOMEPIG) - 1) <=
              0) { //烏力時間結束了
            CHAR_setInt(charaindex, CHAR_BECOMEPIG, 0);
            if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_NONE) { //不是在戰鬥狀態下
              CHAR_setInt(charaindex, CHAR_BECOMEPIG, -1); //結束烏力狀態
              CHAR_complianceParameter(charaindex);
              CHAR_sendCToArroundCharacter(
                  CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
              CHAR_send_P_StatusString(charaindex,
                                       CHAR_P_STRING_BASEBASEIMAGENUMBER);
              CHAR_talkToCli(charaindex, -1, "烏力化失效了。", CHAR_COLORWHITE);
            }
          } else {
            CHAR_setInt(charaindex, CHAR_BECOMEPIG,
                        CHAR_getInt(charaindex, CHAR_BECOMEPIG) - 10);
          }
        }
#endif
#ifdef _ITEM_TIME_LIMIT
        ITEM_TimeLimit(charaindex); // (可開放) shan time limit of item. code:shan
#endif
      }
      if (chikulatime % 6 == 0) { // 60��
#ifdef _ITEM_ADDEXP               // vincent ��������
#ifdef _PET_ADD_EXP
        if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _UP_ON_OFF
            && CHAR_getInt(charaindex, CHAR_ONOFF01) == 1
#endif
        ) {
          int exptime;
          if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP) > 0) {
            exptime =
                CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME) - 60;
            if (exptime <= 0) {
              CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXP, 0);
              CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME, 0);
              CHAR_talkToCli(charaindex, -1, "提升學習經驗的能力消失了!",
                             CHAR_COLORYELLOW);
            } else {
              CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME, exptime);
              if ((exptime % (60 * 60)) < 60 && exptime >= (60 * 60)) {
                char msg[1024];
                sprintf(msg, "您的提升學習經驗的能力剩大約 %d 小時。",
                        (int)(exptime / (60 * 60)));
                CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
              }
            }
          }
          int i;
          for (i = 0; i < CHAR_MAXPETHAVE; i++) {
            int petindex = CHAR_getCharPet(charaindex, i);
            if (!CHAR_CHECKINDEX(petindex))
              continue;
            if (CHAR_getWorkInt(petindex, CHAR_WORKITEM_ADDEXP) > 0) {
              exptime =
                  CHAR_getWorkInt(petindex, CHAR_WORKITEM_ADDEXPTIME) - 60;
              if (exptime <= 0) {
                CHAR_setWorkInt(petindex, CHAR_WORKITEM_ADDEXP, 0);
                CHAR_setWorkInt(petindex, CHAR_WORKITEM_ADDEXPTIME, 0);
                char msg[1024];
                sprintf(msg, "[%s]提升學習經驗的能力消失了!",
                        CHAR_getChar(petindex, CHAR_NAME));
                CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
              } else {
                CHAR_setWorkInt(petindex, CHAR_WORKITEM_ADDEXPTIME, exptime);
                if ((exptime % (60 * 60)) < 60 && exptime >= (60 * 60)) {
                  char msg[1024];
                  sprintf(msg, "寵物%s提升學習經驗的能力剩大約 %d 小時。",
                          CHAR_getChar(petindex, CHAR_NAME),
                          (int)(exptime / (60 * 60)));
                  CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
                }
              }
            }
          }
        }
#else
        if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP) > 0 &&
            CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _UP_ON_OFF
            && CHAR_getInt(charaindex, CHAR_ONOFF01) == 1
#endif
        ) {
          int exptime;
          exptime = CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME) - 60;
          if (exptime <= 0) {
            CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXP, 0);
            CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME, 0);
            CHAR_talkToCli(charaindex, -1, "提升學習經驗的能力消失了!",
                           CHAR_COLORYELLOW);
          } else {
            CHAR_setWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME, exptime);
            if ((exptime % (60 * 60)) < 60 && exptime >= (60 * 60)) {
              char msg[1024];
              sprintf(msg, "提升學習經驗的能力剩大約 %d 小時。",
                      (int)(exptime / (60 * 60)));
              CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
            }
          }
          CHAR_setInt(charaindex, CHAR_ADDEXPPOWER,
                      CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXP));
          CHAR_setInt(charaindex, CHAR_ADDEXPTIME,
                      CHAR_getWorkInt(charaindex, CHAR_WORKITEM_ADDEXPTIME));
        }
#endif
#endif
#ifdef _ITEM_NOENEMY
        if (CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME) > 0 &&
            CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
            CHAR_getInt(charaindex, CHAR_ONOFF02) == 1) {
          int time;
          time = CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME) - 60;

          if (time <= 0) {
            CHAR_setWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME, 0);
            CHAR_setInt(charaindex, CHAR_ONOFF02, 0);
            setEqNoenemy(CHAR_getWorkInt(charaindex, CHAR_WORKFD), 0);
            CHAR_talkToCli(charaindex, -1, "unknown message",
                           CHAR_COLORYELLOW);
          } else {
            CHAR_setWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME, time);
            if ((time % (60 * 60)) < 60 && time >= (60 * 60)) {
              char msg[1024];
              sprintf(msg, "unknown message %d",
                      (int)(time / (60 * 60)));
              CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORYELLOW);
            }
          }
          CHAR_setInt(charaindex, CHAR_NOENEMYONOFF,
                      CHAR_getWorkInt(charaindex, CHAR_WORKITEM_NOENEMYONTIME));
        }
#endif
#ifdef _CHAR_EVENTPOINT
        char msg[256];
        if (CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
          if (!CHAR_CHECKINDEX(charaindex))
            return -1;
          CHAR_setInt(charaindex, CHAR_EVENTTIME,
                      CHAR_getInt(charaindex, CHAR_EVENTTIME) + 1);
          if (CHAR_getInt(charaindex, CHAR_EVENTTIME) > 120) {
            CHAR_setInt(charaindex, CHAR_EVENTPOINT,
                        CHAR_getInt(charaindex, CHAR_EVENTPOINT) + 1);
            CHAR_setInt(charaindex, CHAR_EVENTTIME, 0);
            CHAR_sendCToArroundCharacter(
                CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
            sprintf(
                msg,
                "unknown message %s",
                CHAR_getChar(charaindex, CHAR_NAME));
            CHAR_talkToCli(charaindex, -1, msg, CHAR_COLORWHITE);
          }
        }
#endif
#ifdef _ITEM_METAMO
        if (CHAR_getWorkInt(charaindex, CHAR_WORKITEMMETAMO) < NowTime.tv_sec &&
            CHAR_getWorkInt(charaindex, CHAR_WORKITEMMETAMO) != 0) {
          CHAR_setWorkInt(charaindex, CHAR_WORKITEMMETAMO, 0);
          CHAR_setWorkInt(charaindex, CHAR_WORKNPCMETAMO,
                          0); //��npc�Ի���ı���ҲҪ�����
          CHAR_complianceParameter(charaindex);
          CHAR_sendCToArroundCharacter(
              CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
          CHAR_send_P_StatusString(charaindex,
                                   CHAR_P_STRING_BASEBASEIMAGENUMBER);
          CHAR_talkToCli(charaindex, -1, "실패로 바뀌었다.", CHAR_COLORWHITE);
        }
#endif
      }
#ifdef _PETSKILL_BECOMEPIG
      if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
          BATTLE_CHARMODE_NONE) { //������ս��״̬��
        if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1) { //���������״̬
          char temp[256];
          CHAR_setInt(charaindex, CHAR_BECOMEPIG,
                      CHAR_getInt(charaindex, CHAR_BECOMEPIG) - 1);
          sprintf(temp, "烏力時間:%d秒",
                  CHAR_getInt(charaindex, CHAR_BECOMEPIG));
          CHAR_talkToCli(charaindex, -1, temp, CHAR_COLORWHITE);
        }
      }
#endif
#ifdef _MAP_TIME
      if (CHAR_getWorkInt(charaindex, CHAR_WORK_MAP_TIME) > 0 &&
          CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
              BATTLE_CHARMODE_NONE) {
        CHAR_setWorkInt(charaindex, CHAR_WORK_MAP_TIME,
                        CHAR_getWorkInt(charaindex, CHAR_WORK_MAP_TIME) - 10);
        if (CHAR_getWorkInt(charaindex, CHAR_WORK_MAP_TIME) <= 0) {
          // 時間到了,傳迴入口
          CHAR_talkToCli(charaindex, -1,
                         "你因為受不了高熱而熱死！傳迴裂縫入口。",
                         CHAR_COLORRED);
          CHAR_warpToSpecificPoint(charaindex, 30008, 39, 38);
          CHAR_setInt(charaindex, CHAR_HP, 1);
          CHAR_AddCharm(charaindex, -3);
          CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_HP);
          CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_CHARM);
        } else {
          char szMsg[64];
          sprintf(szMsg, "在這高熱的環境下你隻能再待 %d 秒。",
                  CHAR_getWorkInt(charaindex, CHAR_WORK_MAP_TIME));
          CHAR_talkToCli(charaindex, -1, szMsg, CHAR_COLORRED);
        }
      }
#endif
    }

    for (i = 0; i < ConnectLen; i++) {
      if ((Connect[i].use) && (i != acfd))
        if (!CONNECT_getUse(i))
          continue;
      if (!CHAR_CHECKINDEX(Connect[i].charaindex))
        continue;
      {
#ifndef _USER_CHARLOOPS
        // here ԭ������
        if (Connect[i].stayencount) {
          if (Connect[i].BDTime < time(NULL)) {
            if (CHAR_getWorkInt(Connect[i].charaindex, CHAR_WORKBATTLEMODE) ==
                BATTLE_CHARMODE_NONE) {
              lssproto_EN_recv(i, CHAR_getInt(Connect[i].charaindex, CHAR_X),
                               CHAR_getInt(Connect[i].charaindex, CHAR_Y));
              Connect[i].BDTime = time(NULL);
            }
          }
        }
#endif
#ifdef _CHIKULA_STONE
        if (chikulatime % 3 == 0 && getChiStone(i) > 0) { //自動補血
          CHAR_AutoChikulaStone(Connect[i].charaindex, getChiStone(i));
        }
#endif
        if (chikulatime % 6 == 0) { //水世界狀態
#ifdef _STATUS_WATERWORD
          CHAR_CheckWaterStatus(Connect[i].charaindex);
#endif
          if (Connect[i].noenemy > 0) {
            Connect[i].noenemy--;
            if (Connect[i].noenemy == 0) {
              CHAR_talkToCli(CONNECT_getCharaindex(i), -1, "守護消失了。",
                             CHAR_COLORWHITE);
            }
          }
        }
        //每10秒
#ifdef _TYPE_TOXICATION //中毒
        if (Connect[i].toxication > 0) {
          CHAR_ComToxicationHp(Connect[i].charaindex);
        }

#endif
        if (Connect[i].state == NOTLOGIN) {
          Connect[i].cotime++;
          if (Connect[i].cotime > 30) {
            print("LATE");
            CONNECT_endOne_debug(i);
          }
        } else {
          Connect[i].cotime = 0;
        }

        if ((Connect[i].nu <= 22)) {
          if (Connect[i].nu <= 0) {
            Connect[i].nu_decrease++;
            if (Connect[i].nu_decrease >= 30)
              Connect[i].nu_decrease = 30;
            if (Connect[i].nu_decrease > 22)
              logSpeed(i);
          } else {
            Connect[i].nu_decrease -= 1;
            if (Connect[i].nu_decrease < 0)
              Connect[i].nu_decrease = 0;
          }
          lssproto_NU_send(i, 0);
        }
        Connect[i].packetin--;
        if (Connect[i].packetin <= 0) {
          // print( "Drop line: sd=%d\n", i );
          CONNECT_endOne_debug(i);
        }
      }
    }
  }
}

// Nuke 0126: Resource protection
int isThereThisIP(unsigned long ip) {
  int i;
  unsigned long ipa;
  for (i = 0; i < ConnectLen; i++)
    if (!Connect[i].use)
      continue;
  if (Connect[i].state == NOTLOGIN ||
      Connect[i].state == WHILEDOWNLOADCHARLIST) {
    memcpy(&ipa, &Connect[i].sin.sin_addr, 4);
    if (ipa == ip)
      return 1;
  }

  return 0;
}

int player_online = 0;
int player_maxonline = 0;

#ifdef _NB_FIX_SOCK_WRITE_OP
ssize_t writen(int fd, const void *ptr, size_t n) {
  size_t nleft;
  ssize_t nwritten;
  nleft = n;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) < 0) {
      if (nleft == n)
        return -1; // error, return -1
      else
        break; /* error, return amount written so far */
    } else if (nwritten == 0) {
      break;
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return (n - nleft); // return >= 0;
}
#endif

struct timeval speedst, speedet;
SINGLETHREAD BOOL netloop_faster(void) {
  static unsigned int total_item_use = 0;
  static int petcnt = 0;
  struct timeval st, et;
  unsigned int looptime_us;
  looptime_us = getOnelooptime_ms() * 1000;
  int ret, loop_num;
  struct timeval tmv; /*timeval*/
  static int fdremember = 0;
  int acceptmore = SERVSTATE_getAcceptmore();

  //    static unsigned int nu_time=0;
  unsigned int casend_interval_us, cdsend_interval_us;
  fd_set rfds, wfds, efds;
  int allowerrornum = getAllowerrornum();
  int acwritesize = getAcwriteSize();
#ifdef _AC_PIORITY
  static int flag_ac = 1;
  static int fdremembercopy = 0;
  static int totalloop = 0;
  static int totalfd = 0;
  static int totalacfd = 0;
  static int counter = 0;
#endif

#ifdef _SYSTEM_SPEAD
  casend_interval_us = getCAsendinterval_ms() * 200;
  cdsend_interval_us = getCDsendinterval_ms() * 200;
#else
  casend_interval_us = getCAsendinterval_ms() * 1000;
  cdsend_interval_us = getCDsendinterval_ms() * 1000;
#endif
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&efds);
  FD_SET(bindedfd, &rfds);
  FD_SET(bindedfd, &wfds);
  FD_SET(bindedfd, &efds);
  tmv.tv_sec = tmv.tv_usec = 0;
  ret = select(bindedfd + 1, &rfds, &wfds, &efds, &tmv);
  if (ret < 0 && (errno != EINTR)) {
    print("accept select() error:%s\n", strerror(errno));
  }
  if (ret > 0 && FD_ISSET(bindedfd, &rfds)) {
    struct sockaddr_in sin;
    int addrlen = sizeof(struct sockaddr_in);
    int sockfd;
    int nonblock_err = 0;
    sockfd = accept(bindedfd, (struct sockaddr *)&sin, &addrlen);
    // SetTcpBuf(sockfd, &rfds);
    if (sockfd == -1 && errno == EINTR) {
      print("accept err:%s\n", strerror(errno));
    } else if (sockfd != -1) {
      int __flags = fcntl(sockfd, F_GETFL, 0);
      if (fcntl(sockfd, F_SETFL, __flags | O_NONBLOCK) < 0) {
        close(sockfd);
        print("Set sockfd Nonblock error.\n");
        nonblock_err = 1;
      }
      unsigned long sinip;
      int cono = 1, from_acsv = 0;
      if (cono_check & CONO_CHECK_LOGIN) {
        if (StateTable[WHILELOGIN] + StateTable[WHILELOGOUTSAVE] >
                QUEUE_LENGTH1 ||
            StateTable[WHILEDOWNLOADCHARLIST] > QUEUE_LENGTH2) {
          print("err State[%d,%d,%d]!!\n", StateTable[WHILELOGIN],
                StateTable[WHILELOGOUTSAVE], StateTable[WHILEDOWNLOADCHARLIST]);
          CONNECT_checkStatecount(WHILEDOWNLOADCHARLIST);
          cono = 0;
        }
      }
      if (cono_check & CONO_CHECK_ITEM) {
        if (total_item_use >= MAX_item_use) {
          print("可使用物品數已滿!!");
          cono = 0;
        }
      }
      if (cono_check & CONO_CHECK_PET) {
        if (petcnt >= CHAR_getPetMaxNum()) {
          print("可使用寵物數已滿!!");
          cono = 0;
        }
      }
      {
        float fs = 0.0;
        if ((fs = ((float)Connect[acfd].rbuse / AC_RBSIZE)) > 0.6) {
          print("andy AC rbuse: %3.2f [%4d]\n", fs, Connect[acfd].rbuse);
          if (fs > 0.78)
            cono = 0;
        }
      }
      memcpy(&sinip, &sin.sin_addr, 4);
      if (nonblock_err) {
        ;
      } else if ((cono == 0) || (acceptmore <= 0) || isThereThisIP(sinip)) {
        char mess[64] = "E伺服器忙綫中，請稍候再試。";
        if (!from_acsv)
          write(sockfd, mess, strlen(mess) + 1);
        print("accept but drop[cono:%d,acceptmore:%d]\n", cono, acceptmore);
        close(sockfd);
      } else if (sockfd < ConnectLen) {
        char mess[64] = "A";
        if (bNewServer) {
          mess[0] = _SA_VERSION;
        } else {
          mess[0] = '$';
        }
        // initConnectOne(sockfd,&sin,addrlen);
#ifdef _NB_DEFAULT_KEY
#define RANDOM_KEY_LEN 128
#define RANDOM_KEY_NUM 10
        unsigned char tmp[RANDOM_KEY_LEN + 1];
        char tmp2[RANDOM_KEY_LEN + 1];
        *tmp2 = '\0';
#endif
        if (!from_acsv) {
#ifdef _NB_DEFAULT_KEY
          int j, k;
          for (j = 0; j < RANDOM_KEY_NUM; j++) {
            int r = rand() % 52;
            if (r < 26) {
              tmp[j] = 'A' + r;
            } else {
              tmp[j] = 'a' + r - 26;
            }
          }
          tmp[RANDOM_KEY_NUM] = '\0';
          unsigned char xorArr[] = {'\x88', '\x9A', '\xCC',
                                    '\xAB', '\x03', '\x20'}; // Length = 6
          int encLen = 0;
          for (j = 0; j < RANDOM_KEY_NUM; j++) {
            int count = (rand() % 3) + 1;
            unsigned char enc = count ^ xorArr[j % 6];
            char tmpString[32];
            sprintf(tmpString, "%02X", enc);
            strcat(tmp2, tmpString);
            encLen++;
            int sum = 0;
            for (k = 0; k < count; k++) {
              int partialSum;
              if (k == count - 1) {
                partialSum = tmp[j] - sum;
              } else {
                partialSum = tmp[j] / count;
              }
              sum += partialSum;
              enc = partialSum ^ xorArr[j % 6];
              sprintf(tmpString, "%02X", enc);
              strcat(tmp2, tmpString);
              encLen++;
            }
          }
          for (j = encLen * 2; j < RANDOM_KEY_LEN; j = j + 2) {
            int ran = rand() % 256;
            char charTmp[256];
            sprintf(charTmp, "%02X", ran & 0xFF);
            strcat(tmp2, charTmp);
          }
          strcat(mess, tmp2);
#endif
          send(sockfd, mess, strlen(mess) + 1, 0);
        }
        initConnectOne(sockfd, &sin, addrlen);
#ifdef _NB_DEFAULT_KEY
        // print("??��?��%s\n",tmp);
        strcpy(Connect[sockfd].encryptDefaultKey, (char *)tmp);
#else
        strcat(mess, Connect[sockfd].defaultdeskey);
#endif
        SetTcpBuf(sockfd);
        if (getNodelay()) {
          int flag = 1;
          int result = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
                                  (char *)&flag, sizeof(int));
          if (result < 0) {
            CONNECT_endOne_debug(sockfd);
            print("setsockopt TCP_NODELAY failed:%s\n", strerror(errno));
          } else {
            print("NO");
          }
        }
      } else {
        int i;
        for (i = 0; i < ConnectLen; i++) {
          if (Connect[i].use == FALSE)
            continue;
          if (i == acfd)
            continue;
          if (Connect[i].charaindex != -1)
            continue;
          char mess[64] = "E伺服器繁忙，請稍候再試。";
          if (!from_acsv)
            write(i, mess, strlen(mess) + 1);
          CONNECT_endOne_debug(i);
        }
      }
    }
  }
  loop_num = 0;
  gettimeofday(&st, NULL);
  while (1) {
    static char buf[1024 * 64 * 8];
    int j;
    static int i_tto = 0;
    static int i_timeNu = 0;
    gettimeofday(&et, NULL);
    if (time_diff_us(et, st) >= looptime_us) { //?������?0.1��?����������
#define LOOP_NUM_ADD_CREDIT 5
#define CREDIT_SPOOL 3
      switch (acceptmore) {
      case -1:
        print("#");
        if (Connect[acfd].wbuse > 0)
          goto SendAC;
        shutdownProgram();
        exit(0);
        break;
      case 0:
        print("$");
        if (!b_first_shutdown) {
          b_first_shutdown = TRUE;
          i_shutdown_time = SERVSTATE_getLimittime();
          print("\n 關閉服務器時間=%d", i_shutdown_time);
        }
        break;
      default:
      SendAC : {
        static int i_counter = 0; // Syu ADD ��??��Announce
        static int j_counter = 0; // Syu ADD ���?�����������??����۴?��
        static int h_counter = 0; // ��??����??��
        static long total_count = 0;
        int i;
        int item_max;
        if (i_counter > 10) { // 10��
          player_online = 0;  // looptime_us
#ifdef _AC_PIORITY
          totalloop = 0;
          totalfd = 0;
          totalacfd = 0;
#endif
          i_counter = 0;
          item_max = ITEM_getITEM_itemnum();
          total_item_use = ITEM_getITEM_UseItemnum();
          for (i = 0; i < ConnectLen; i++) {
            if ((Connect[i].use) && (i != acfd)) {
              if (CHAR_CHECKINDEX(Connect[i].charaindex))
                player_online++;
            }
          }
          if (player_online > player_maxonline) {
            player_maxonline = player_online;
          }
          {
            int max, min; // int MaxItemNums;
            char buff1[512];
            char szBuff1[256];
#ifdef _ASSESS_SYSEFFICACY
            {
              float TVsec;
              ASSESS_getSysEfficacy(&TVsec);
              sprintf(szBuff1, "Sys:[%2.4f] \n", TVsec);
            }
#endif
            // MaxItemNums = ITEM_getITEM_itemnum();
            // MaxItemNums = getItemnum();
            memset(buff1, 0, sizeof(buff1));
            CHAR_getCharOnArrayPercentage(1, &max, &min, &petcnt);
            sprintf(buff1,
                    "\nOnline player=%d Offline player=%d pet=%d thing=%d "
                    "mail:%d fighting:%d %s",
                    player_online, luaplayernum, petcnt, total_item_use,
                    PETMAIL_getPetMailTotalnums(), Battle_getTotalBattleNum(),
                    szBuff1);
            buff1[strlen(buff1) + 1] = 0;
            print("%s", buff1); //?����ʫ??����

            gettimeofday(&speedet, NULL);
            float speedtime = time_diff_us(speedet, speedst) / 1000000.0;
#ifdef _ASSESS_SYSDOWNUPNEWS
            sprintf(buff1,
                    "Upload:[%2.3f K]average[%2.3f K]"
                    "download:[%2.3f K]average[%2.3f K]"
                    "Intimate%2.3fSecond\n",
                    sendspeed / 1024.0, sendspeed / speedtime / 1024.0,
                    recvspeed / 1024.0, recvspeed / speedtime / 1024.0,
                    speedtime);
#endif
            gettimeofday(&speedst, NULL);
            sendspeed = 0;
            recvspeed = 0;

#ifdef _ASSESS_SYSDOWNUPNEWS
            buff1[strlen(buff1) + 1] = 0;
            print("%s", buff1);
#endif

#ifdef _ASSESS_SYSEFFICACY_SUB
            {
              float TVsec;
              ASSESS_getSysEfficacy_sub(&TVsec, 1);
              sprintf(szBuff1, "NT:[%2.4f] ", TVsec);
              strcpy(buff1, szBuff1);

              ASSESS_getSysEfficacy_sub(&TVsec, 2);
              sprintf(szBuff1, "NG:[%2.4f] ", TVsec);
              strcat(buff1, szBuff1);

              ASSESS_getSysEfficacy_sub(&TVsec, 3);
              sprintf(szBuff1, "BT:[%2.4f] ", TVsec);
              strcat(buff1, szBuff1);

              ASSESS_getSysEfficacy_sub(&TVsec, 4);
              sprintf(szBuff1, "CH:[%2.4f] \n", TVsec);
              strcat(buff1, szBuff1);

              buff1[strlen(buff1) + 1] = 0;
#ifdef _ASSESS_SYSDOWNUPNEWS
              print("%s.", buff1);
#endif
            }
#endif
          }
#ifdef _TIME_TICKET
          check_TimeTicket();
#endif
        }
        if (i_timeNu != time(NULL)) { // ������?�����
          i_timeNu = time(NULL);
#ifdef _DEL_DROP_PET
          if (total_count % 60 == 0) { //����??��
#ifdef _DEL_DROP_GOLD
            GOLD_DeleteTimeCheckLoop();
#endif
#ifdef _ALLBLUES_LUA_1_5
            NetLoopFunction();
#endif
            int objindex;
            int objmaxnum = OBJECT_getNum();

            for (objindex = 0; objindex < objmaxnum; objindex++) {
              if (CHECKOBJECT(objindex) == FALSE)
                continue;
              if (OBJECT_getType(objindex) == OBJTYPE_CHARA) {
                int petindex;
                petindex = OBJECT_getIndex(objindex);
                if (CHAR_CHECKINDEX(petindex) == FALSE)
                  continue;
                if (CHECKOBJECT(petindex) == FALSE)
                  continue;
                if (CHAR_getInt(petindex, CHAR_WHICHTYPE) != CHAR_TYPEPET)
                  continue;
                if (CHAR_getInt(petindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE)
                  continue;
                if (CHAR_getWorkInt(petindex, CHAR_WORKPETFOLLOWMODE) ==
                    CHAR_PETFOLLOW_NOW)
                  continue;
                PET_CHECKFreePetIsIt(petindex);
              }
            }
          }
#endif
          i_counter++;
          j_counter++;
          h_counter++;
          total_count++;
        }
      }
        if ((i_tto % 60) == 0) {
          i_tto = 0;
          print(".");
        }
        i_tto++;
        CONNECT_SysEvent_Loop();
      } // switch()

#ifdef _AC_PIORITY
      if (flag_ac == 2)
        fdremember = fdremembercopy;
      flag_ac = 1;
      totalloop++;
#endif
      break; // Break while
    }        // if(>0.1sec)

    loop_num++;

#ifdef _AC_PIORITY
    switch (flag_ac) {
    case 1:
      fdremembercopy = fdremember;
      fdremember = acfd;
      flag_ac = 2;
      break;
    case 2:
      counter++;
      if (counter >= 3) {
        counter = 0;
        fdremember = fdremembercopy + 1;
        flag_ac = 0;
      }

      break;
    default:
      fdremember++;
      break;
    }
#else
    fdremember++;
#endif

    if (fdremember == ConnectLen)
      fdremember = 0;
    // shen_checkfdcontinue(fdremember);
    if (Connect[fdremember].use == FALSE)
      continue;
    if (Connect[fdremember].state == WHILECLOSEALLSOCKETSSAVE)
      continue;
    if (Connect[fdremember].state == NULLCONNECT) {
      time_t new_t;
      time(&new_t);
      new_t -= initTime;
      if (new_t > 60) {
        if (fdremember != acfd) {
          if (Connect[fdremember].connecttime + 15 < time(NULL)) {
            CONNECT_endOne_debug(fdremember);
            continue;
          }
        }
      }
    }
#ifdef _AC_PIORITY
    totalfd++;
    if (fdremember == acfd)
      totalacfd++;
#endif
    /* read select */
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    FD_SET(fdremember, &rfds);
    FD_SET(fdremember, &wfds);
    FD_SET(fdremember, &efds);
    tmv.tv_sec = tmv.tv_usec = 0;
    ret = select(fdremember + 1, &rfds, &wfds, &efds, &tmv);

    if (ret > 0 && FD_ISSET(fdremember, &rfds)) {
      errno = 0;
      char buf[1024 * 128];
      memset(buf, 0, sizeof(buf));
      ret = read(fdremember, buf, sizeof(buf));
      if (ret > 0 && sizeof(buf) <= ret) {
        print("讀取(%s)緩衝長度:%d - %d !!\n",
              (fdremember == acfd) ? "SAAC" : "其它", ret, sizeof(buf));
      }
      if ((ret == -1 && errno != EINTR) || ret == 0) {
        if (fdremember == acfd) {
          print("讀取返迴:%d %s\n", ret, strerror(errno));
          print("gmsv與acsv失去連接! 異常終止...\n");
          sigshutdown(-1);
          exit(1);
        } else {
          if (ret == -1) {
            if (errno == 113 && errno == 104) {
              continue;
            } else {
              print("讀取返迴: %d %s \n", errno, strerror(errno));
            }
          }
          // print( "\nRCL " );
          CONNECT_endOne_debug(fdremember);
          continue;
        }
      } else {
        if (appendRB(fdremember, buf, ret) == -2 && getErrUserDownFlg() == 1) {
          CONNECT_endOne_debug(fdremember);
          continue;
        } else {
          if (fdremember != acfd) {
            recvspeed += ret;
          }
          Connect[fdremember].lastreadtime = NowTime;
          Connect[fdremember].lastreadtime.tv_sec -= DEBUG_ADJUSTTIME;
          Connect[fdremember].packetin = 30;
        }
      }
    } else if (ret < 0 && errno != EINTR) {
      if (fdremember != acfd) {
        // print( "\n?��?��??:%d %s\n", errno, strerror( errno ));
        CONNECT_endOne_debug(fdremember);
        continue;
      }
    }

    for (j = 0; j < 3; j++) {
      char rbmess[65535 * 2];
      memset(rbmess, 0, sizeof(rbmess));
      if (GetOneLine_fix(fdremember, rbmess, sizeof(rbmess)) == FALSE)
        continue;
      if (!((rbmess[0] == '\r' && rbmess[1] == '\n') || rbmess[0] == '\n')) {
        if (fdremember == acfd) {
#ifdef _DEBUG
          printf("讀取SAAC數據:%s\n", rbmess);
#endif
          if (saacproto_ClientDispatchMessage(fdremember, rbmess) < 0) {
            print("\nSAAC服務器數據出錯!!!\n");
          }
        } else {
          if (lssproto_ServerDispatchMessage(fdremember, rbmess) < 0) {
            // print("\nLSSP:DispatchMsg_Error!!! \n");
            if (++Connect[fdremember].errornum > allowerrornum)
              break;
          }
        }
      }
    }
    if (Connect[fdremember].errornum > allowerrornum) {
      print("用戶:%s發生太多錯誤了，所以強制關閉\n",
            inet_ntoa(Connect[fdremember].sin.sin_addr));
      CONNECT_endOne_debug(fdremember);
      continue;
    }
    if (Connect[fdremember].CAbufsiz > 0 &&
        time_diff_us(et, Connect[fdremember].lastCAsendtime) >
            casend_interval_us) {
      CAsend(fdremember);
      Connect[fdremember].lastCAsendtime = et;
    }

    if (Connect[fdremember].CDbufsiz > 0 &&
        time_diff_us(et, Connect[fdremember].lastCDsendtime) >
            cdsend_interval_us) {
      CDsend(fdremember);
      Connect[fdremember].lastCDsendtime = et;
    }

    if (Connect[fdremember].wbuse > 0) {
      FD_ZERO(&rfds);
      FD_ZERO(&wfds);
      FD_ZERO(&efds);

      FD_SET(fdremember, &rfds);
      FD_SET(fdremember, &wfds);
      FD_SET(fdremember, &efds);
      tmv.tv_sec = tmv.tv_usec = 0;
      ret = select(fdremember + 1, &rfds, &wfds, &efds, &tmv);

      if (ret > 0 && FD_ISSET(fdremember, &wfds)) {
        if (fdremember == acfd) {
          ret = write(fdremember, Connect[fdremember].wb,
                      (Connect[fdremember].wbuse < acwritesize)
                          ? Connect[fdremember].wbuse
                          : acwritesize); //���� ?������
        } else {
#ifdef _NB_FIX_SOCK_WRITE_OP
          ret = writen(fdremember, Connect[fdremember].wb,
                       (Connect[fdremember].wbuse < 1024 * 64)
                           ? Connect[fdremember].wbuse
                           : 1024 * 64); //���� ?������
#else
          ret = write(fdremember, Connect[fdremember].wb,
                      (Connect[fdremember].wbuse < 1024 * 64)
                          ? Connect[fdremember].wbuse
                          : 1024 * 64); //���� ?������
#endif
          sendspeed += ret;
        }
        if (ret == -1 && errno != EINTR) {
          // print( "?������: %d %s \n", errno, strerror( errno));
          CONNECT_endOne_debug(fdremember);
          continue;
        } else if (ret > 0) {
          shiftWB(fdremember, ret);
        }
      } else if (ret < 0 && errno != EINTR) {
        // print( "\n?��?��??:%d %s\n",errno, strerror( errno ));
        CONNECT_endOne_debug(fdremember);
      }
    }
    if (fdremember == acfd)
      continue;

    if (Connect[fdremember].close_request) {
      // print( "˭��??:%s \n",inet_ntoa(Connect[fdremember].sin.sin_addr ));
      CONNECT_endOne_debug(fdremember);
      continue;
    }
  }
#ifdef _P_THREAD_RUN
  pthreadflg = 0;
  // pthread_exit(NULL);
#endif
  return TRUE;
}

ANYTHREAD void outputNetProcLog(int fd, int mode) {
  int i;
  int c_use = 0, c_notdetect = 0;
  int c_ac = 0, c_cli = 0, c_adm = 0, c_max = 0;
  int login = 0;
  char buffer[4096];
  char buffer2[4096];
  strcpysafe(buffer, sizeof(buffer), "Server Status\n");
  c_max = ConnectLen;
  for (i = 0; i < c_max; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use) {
      c_use++;
      switch (Connect[i].ctype) {
      case NOTDETECTED:
        c_notdetect++;
        break;
      case AC:
        c_ac++;
        break;
      case CLI:
        c_cli++;
        break;
      case ADM:
        c_adm++;
        break;
      }

      if (Connect[i].charaindex >= 0) {
        login++;
      }
    }
    CONNECT_UNLOCK(i);
  }
  snprintf(buffer2, sizeof(buffer2),
           "connect_use=%d\n"
           "connect_notdetect=%d\n"
           "connect_ac=%d\n"
           "connect_cli=%d\n"
           "connect_adm=%d\n"
           "connect_max=%d\n"
           "login=%d\n",
           (c_use * getConnectnum() / 100), c_notdetect, c_ac,
           (c_cli * getConnectnum() / 100), c_adm, c_max,
           (login * getConnectnum() / 100));
  strcatsafe(buffer, sizeof(buffer), buffer2);
  {
    int char_max = CHAR_getCharNum();
    int char_use = 0;
    int pet_use = 0;

    for (i = 0; i < char_max; i++) {
      if (CHAR_getCharUse(i)) {
        char_use++;
        if (CHAR_getInt(i, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
          pet_use++;
        }
      }
    }
    snprintf(buffer2, sizeof(buffer2),
             "char_use=%d\n"
             "char_max=%d\n"
             "pet_use=%d\n",
             char_use, char_max, pet_use);
    strcatsafe(buffer, sizeof(buffer), buffer2);
  }

  {
    int i;
    int item_max = ITEM_getITEM_itemnum();
    int item_use = 0;
    for (i = 0; i < item_max; i++) {
      if (ITEM_getITEM_use(i)) {
        item_use++;
      }
    }
    snprintf(buffer2, sizeof(buffer2),
             "item_use=%d\n"
             "item_max=%d\n",
             item_use, item_max);
    strcatsafe(buffer, sizeof(buffer), buffer2);
  }

  {
    int i, obj_use = 0;
    int obj_max = OBJECT_getNum();
    for (i = 0; i < obj_max; i++) {
      if (OBJECT_getType(i) != OBJTYPE_NOUSE) {
        obj_use++;
      }
    }
    snprintf(buffer2, sizeof(buffer2),
             "object_use=%d\n"
             "object_max=%d\n",
             obj_use, obj_max);
    strcatsafe(buffer, sizeof(buffer), buffer2);
  }

  if (mode == 0) {
    printl(LOG_PROC, buffer);
  } else if (mode == 1) {
    lssproto_ProcGet_send(fd, buffer);
  }
}

ANYTHREAD int getfdFromCdkeyWithLogin(char *cd) {
  int i;
  for (i = 0; i < ConnectLen; i++) {
    CONNECT_LOCK(i);
    if (Connect[i].use == TRUE &&
        Connect[i].state != NOTLOGIN // Nuke 0514: Avoid duplicated login
        && strcmp(Connect[i].cdkey, cd) == 0) {
      CONNECT_UNLOCK(i);
      return i;
    }
    CONNECT_UNLOCK(i);
  }
  return -1;
}

#define m_cktime 500
// static float m_cktime=0;
int CHAR_players() {
  int i;
  int chars = 0;
  int players = 0, pets = 0, others = 0;
  int whichtype = -1;
  int objnum = OBJECT_getNum();
  for (i = 0; i < objnum; i++) {
    switch (OBJECT_getType(i)) {
    case OBJTYPE_CHARA:
      chars++;
      whichtype = CHAR_getInt(OBJECT_getIndex(i), CHAR_WHICHTYPE);

      if (whichtype == CHAR_TYPEPLAYER)
        players++;
      else if (whichtype == CHAR_TYPEPET)
        pets++;
      else
        others++;

      break;

    default:
      break;
    }
  }

  return players;
}

void sigusr1(int i) {
  signal(SIGUSR1, sigusr1);
  cono_check = (cono_check + 1) % 4;
  print("Cono Check is login:%d item:%d", cono_check & 1, cono_check & 2);
}

void sigusr2(int i) {
  signal(SIGUSR2, sigusr2);
  print("\nReceived Shutdown signal...\n\n");
  lssproto_Shutdown_recv(0, "hogehoge", 5); // 5��?��?��
}

unsigned long CONNECT_get_userip(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  unsigned long ip;
  memcpy(&ip, &Connect[fd].sin.sin_addr, sizeof(long));
  return ip;
}
void CONNECT_set_pass(int fd, BOOL b_ps) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].b_pass = b_ps;
}
BOOL CONNECT_get_pass(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  BOOL B_ret;
  B_ret = Connect[fd].b_pass;
  return B_ret;
}
void CONNECT_set_first_warp(int fd, BOOL b_ps) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].b_first_warp = b_ps;
}
BOOL CONNECT_get_first_warp(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  BOOL B_ret;
  B_ret = Connect[fd].b_first_warp;
  return B_ret;
}
void CONNECT_set_state_trans(int fd, int a) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].state_trans = a;
}
int CONNECT_get_state_trans(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  int i_ret;
  i_ret = Connect[fd].state_trans;
  return i_ret;
}
// ttom end

// Arminius 6.22 encounter
int CONNECT_get_CEP(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].CEP;
}

void CONNECT_set_CEP(int fd, int cep) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].CEP = cep;
}

int CONNECT_get_confirm(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].confirm_key;
}
void CONNECT_set_confirm(int fd, BOOL b) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].confirm_key = b;
}

int checkNu(fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  Connect[fd].nu--;
  // print("NU=%d\n",Connect[fd].nu);
  if (Connect[fd].nu < 0)
    return -1;

  return 0;
}

// Nuke start 0626: For no enemy function
void setNoenemy(fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].noenemy = 6;
}
void clearNoenemy(fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].noenemy = 0;
}
int getNoenemy(fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  return Connect[fd].noenemy;
}
// Nuke end

// Arminius 7/2: Ra's amulet
void setEqNoenemy(int fd, int level) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].eqnoenemy = level;
}

void clearEqNoenemy(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].eqnoenemy = 0;
}

int getEqNoenemy(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  return Connect[fd].eqnoenemy;
}

#ifdef _Item_MoonAct
void setEqRandenemy(int fd, int level) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].eqrandenemy = level;
}

void clearEqRandenemy(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].eqrandenemy = 0;
}

int getEqRandenemy(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  return Connect[fd].eqrandenemy;
}

#endif

#ifdef _CHIKULA_STONE
void setChiStone(int fd, int nums) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].chistone = nums;
}
int getChiStone(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].chistone;
}
#endif

// Arminius 7.31 cursed stone
void setStayEncount(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  if (ENCOUNT_getEncountAreaArray(
          CHAR_getInt(Connect[fd].charaindex, CHAR_FLOOR),
          CHAR_getInt(Connect[fd].charaindex, CHAR_X),
          CHAR_getInt(Connect[fd].charaindex, CHAR_Y)) == -1)
    return;
  // CHAR_talkToCli(Connect[ fd ].charaindex, -1,
  // "��?����ơ�??��??������???�������??ʦ������?��", CHAR_COLORYELLOW);
  Connect[fd].stayencount = 1;
}

void clearStayEncount(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  if (Connect[fd].stayencount == 1)
    // CHAR_talkToCli(Connect[ fd ].charaindex, -1, "��?����ơ�?�?��������???��",
    // CHAR_COLORYELLOW);
    Connect[fd].stayencount = 0;
}

int getStayEncount(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return 0;
  }
  return Connect[fd].stayencount;
}

#ifdef _BATTLE_TIMESPEED
void CONNECT_setBDTime(int fd, int nums) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].BDTime = nums;
}

int CONNECT_getBDTime(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].BDTime;
}
#endif
#ifdef _TYPE_TOXICATION
void setToxication(int fd, int flg) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  Connect[fd].toxication = flg;
}
int getToxication(int fd) {
  if (fd < 0 || fd >= ConnectLen) {
    return -1;
  }
  return Connect[fd].toxication;
}
#endif

#ifdef _BATTLE_TIMESPEED
void RescueEntryBTime(int charaindex, int fd, unsigned int lowTime,
                      unsigned int battletime) {
  if (fd < 0 || fd >= ConnectLen) {
    return;
  }
  int NowTime = (int)time(NULL);

  Connect[fd].CBTime = NowTime;
  // Connect[fd].CBTime+battletime
}

BOOL CheckDefBTime(int charaindex, int fd, unsigned int lowTime,
                   unsigned int battletime,
                   unsigned int addTime) { // lowTime��???
  if (fd < 0 || fd >= ConnectLen) {
    return TRUE;
  }
  unsigned int NowTime = (unsigned int)time(NULL);
  // print(" NowTime=%d lowTime=%d battleTime=%d CBTime=%d", NowTime, lowTime,
  // battletime, Connect[fd].CBTime);
  lowTime += battletime;

  if ((Connect[fd].CBTime + battletime) > lowTime)
    lowTime = Connect[fd].CBTime + battletime;
  /*
    if ( NowTime < lowTime ) { //lowTime??��?��?��??
          int r = 0;
          delayTime = lowTime - NowTime;
          delayTime = ( delayTime <= 0 ) ? 1 : delayTime;
          r = ( -4 ) * ( delayTime + 2 );
          lssproto_NU_send( fd, r );
          Connect[ fd ].nu += r;
    }
  */
  lssproto_NU_send(fd, 0);
  // Connect[fd].BDTime = (NowTime+20)+delayTime;
#ifdef _FIX_CHARLOOPS
  if (getCharloops() > 0)
    Connect[fd].BDTime = NowTime + rand() % getCharloops(); // ??����??
  else
    Connect[fd].BDTime = NowTime;
#else
  Connect[fd].BDTime = (NowTime + rand() % 5) + delayTime + addTime; // ??����??
#endif
  // print(" BDTime=%d ", Connect[fd].BDTime);
  return TRUE;
}
#endif

BOOL MSBUF_CHECKbuflen(int size, float defp) { return TRUE; }

void saveforsaac() {
  int acwritesize = getAcwriteSize();
  while (Connect[acfd].wbuse > 0) {
    struct timeval tmv; /*timeval*/
    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    FD_SET(acfd, &rfds);
    FD_SET(acfd, &wfds);
    FD_SET(acfd, &efds);
    tmv.tv_sec = tmv.tv_usec = 0;
    int ret = select(acfd + 1, &rfds, &wfds, &efds, &tmv);
    if (ret > 0 && FD_ISSET(acfd, &wfds)) {
      // Nuke start 0907: Protect gmsv
#ifdef _DEBUG
      printf("發送SAAC內容:%s\n", Connect[acfd].wb);
#endif
      ret = write(acfd, Connect[acfd].wb,
                  (Connect[acfd].wbuse < acwritesize)
                      ? Connect[acfd].wbuse
                      : acwritesize); //���� ?������

      if (ret == -1 && errno != EINTR) {
        // print( "?������: %d %s \n", errno, strerror( errno));
        CONNECT_endOne_debug(acfd);
        continue;
      } else if (ret > 0) {
        shiftWB(acfd, ret);
      }
    } else if (ret < 0 && errno != EINTR) {
      print("\n寫入返迴:%d %s\n", errno, strerror(errno));
      CONNECT_endOne_debug(acfd);
    }
  }
}
