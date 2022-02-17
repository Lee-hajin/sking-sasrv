#include "version.h"
#include <dirent.h>
#include <stdio.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "autil.h"
#include "battle.h"
#include "buf.h"
#include "char.h"
#include "char_data.h"
#include "char_talk.h"
#include "configfile.h"
#include "encount.h"
#include "enemy.h"
#include "function.h"
#include "handletime.h"
#include "item.h"
#include "item_gen.h"
#include "log.h"
#include "lssproto_serv.h"
#include "magic_base.h"
#include "msignal.h"
#include "net.h"
#include "npc_quiz.h"
#include "object.h"
#include "pet_skill.h"
#include "petmail.h"
#include "readmap.h"
#include "readnpc.h"
#include "saacproto_cli.h"
#include "title.h"
#include "util.h"
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
#include "profession_skill.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/mylua.h"
#endif
#ifdef _ONLINE_SHOP
#include "NewBilu/newshop.h"
#endif

#ifdef _ITEM_QUITPARTY
#include "init.h"
// int itemquitparty_num = 0;
// static DisappearItem *Disappear_Item;
#endif

#include "NewBilu/sasql.h"

#define MESSAGEFILE "hoge.txt"

#define OPTIONSTRING "d:f:hcl"
#define usage() print("Usage: %s [" OPTIONSTRING "]\n", getProgname());

void printUsage(void) {
  usage();
  /*print( "Usage: %s ["OPTIONSTRING"]\n", progname );*/
  print("          [-d debuglevel]        default value is 0\n");
  print("          [-f configfilename]    default value is setup.cf\n");
}

/*
 *
 * 娄醒
 * 忒曰袄
 *      TRUE(1)     恳橘卅戊穴件玉仿奶件娄醒分匀凶日
 *      FALSE(0)    唱橘卅戊穴件玉仿奶件娄醒分匀凶日
 */
BOOL parseCommandLine(int argc, char **argv) {
  int c;               /* getopt 匹银丹 */
  extern char *optarg; /* getopt 匹银丹 */

  while ((c = getopt(argc, argv, OPTIONSTRING)) != -1) {
    switch (c) {
    case 'd': {
      int debuglevel;
      if (!strtolchecknum(optarg, (int *)&debuglevel, 10, INT)) {
        print("Specify digit number\n");
        return FALSE;
      }
      setDebuglevel(debuglevel);
      break;
    }
    case 'f':
      setConfigfilename(optarg);
      break;
    case 'h':
      printUsage();
      return FALSE;
      break;
    case 'c':
      break;
    default:
      printUsage();
      return FALSE;
      break;
    }
  }
  return TRUE;
}

/*
 * 娄醒
 *
 * 漆及赭窒手仄卅中
 */
BOOL parseEnvironment(char **env) {
  if (getDebuglevel() >= 3) {
    int index = 0;
    while (env[index] != NULL)
      print("%s ", env[index++]);
    print("\n");
  }
  return TRUE;
}

extern int backdoor;

#define GOTORETURNFALSEIFFALSE(x)                                              \
  if (!(x))                                                                    \
  goto RETURNFALSE
/*
 * 赓渝祭伙□民件
 * 娄醒
 *      argc    argv及醒
 *      argv    戊穴件玉仿奶件娄醒
 * 忒曰袄
 */
BOOL init(int argc, char **argv, char **env) {
#ifdef _ITEM_QUITPARTY
  FILE *f;
  int i;
  char line[256];
#endif
  srand(getpid());
  print("This Program is compiled at %s %s by gcc %s\n", __DATE__, __TIME__,
        __VERSION__);

  defaultConfig(argv[0]);
  GOTORETURNFALSEIFFALSE(parseCommandLine(argc, argv));
  GOTORETURNFALSEIFFALSE(parseEnvironment(env));

  signalset();

  {
    Char aho;
    debug(sizeof(aho), d);
    debug(sizeof(aho.data), d);
    debug(sizeof(aho.string), d);
    debug(sizeof(aho.flg), d);
    debug(sizeof(aho.indexOfExistItems), d);
    debug(sizeof(aho.haveSkill), d);
    debug(sizeof(aho.indexOfHaveTitle), d);
    debug(sizeof(aho.addressBook), d);
    debug(sizeof(aho.workint), d);
    debug(sizeof(aho.workchar), d);
  }

  print("配置文件: %s\n", getConfigfilename());

  GOTORETURNFALSEIFFALSE(readconfigfile(getConfigfilename()));

  nice(getrunlevel());
  // ttom start
  {
    int iWork = setEncodeKey();
    if (iWork == 0) {
      // 巨件戊□玉平□毛涩烂
      printf("----------------------------------------\n");
      printf("-------------[编码] 无法设置 %s\n", getConfigfilename());
      printf("----------------------------------------\n");
      exit(1);
    } else {
      // 巨件戊□玉平□毛涩烂
      printf("编码 = %d\n", iWork);
    }
  }
  // AcWBuff毛涩烂
  {
    int iWork = setAcWBSize();
    if (iWork == 0) {
      printf("----------------------------------------\n");
      printf("-------------[AC缓冲] 无法设置 %s\n", getConfigfilename());
      printf("----------------------------------------\n");
      exit(1);
    } else {
      printf("AC缓冲 = %d\n", iWork);
    }
  }
  // ttom end

  if (getDebuglevel() >= 1) {
    //		print("ServerType: %d\n", getServerType() );
    print("디버그 등급: %d\n", getDebuglevel());
    print("실행 레벨: %d\n", getrunlevel());
    print("수신 버퍼: %d\n", getrecvbuffer() * 1024);
    print("버퍼를 보내십시오: %d\n", getsendbuffer() * 1024);
    print("버퍼 하한을 수신합니다: %d\n", getrecvlowatbuffer());
    print("메모리 단위 크기: %d\n", getMemoryunit());
    print("메모리 셀의 수: %d\n", getMemoryunitnum());

    print("계정 서버 주소: %s\n", getAccountservername());
    print("계정 서버 포트: %d\n", getAccountserverport());
    print("로그인 서버 이름: %s\n", getGameservername());
    print("로그인 서버 암호: %s\n", getAccountserverpasswd());

    print("연결 포트를 기다리고 있습니다: %d\n", getPortnumber());

    print("서버 일련 번호: %d\n", getServernumber());

    print("반복 주소: %d\n", getReuseaddr());

    print("최대 온라인 번호: %d\n", getFdnum());
    print("최대 온라인 애완 동물: %d\n", getPetcharnum());
    print("최대 수: %d\n", getOtherscharnum());
    print("가장 큰 물건의 수: %d\n", getObjnum());
    print("항목 수: %d\n", getItemnum());
    print("최대 전투 수: %d\n", getBattlenum());
    print("맨 위 파일 디렉토리: %s\n", getTopdir());
    print("지도 파일 디렉토리: %s\n", getMapdir());
    print("지도 로고 파일: %s\n", getMaptilefile());
    print("항목 구성 파일: %s\n", getItemfile());
    print("근거리 파일: %s\n", getInvfile());
    print("위치 파일 표시: %s\n", getAppearfile());
    print("적의 구성 파일: %s\n", getEffectfile());
    print("짹 타이틀 파일: %s\n", getTitleNamefile());
    print("제목 프로필: %s\n", getTitleConfigfile());
    print("적의 조정 문서: %s\n", getEncountfile());
    print("적 그룹 파일: %s\n", getGroupfile());
    print("애완 동물 기본 문서: %s\n", getEnemyBasefile());
    print("PET 파일을 만듭니다: %s\n", getEnemyfile());
    print("엘프 매직 문서: %s\n", getMagicfile());

#ifdef _ATTACK_MAGIC
    print("마술 파일을 공격하십시오: %s\n", getAttMagicfileName());
#endif

    print("애완 동물 기술 파일: %s\n", getPetskillfile());

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    print("직업 기술 파일: %s\n", getProfession());
#endif

    print("재료 파일: %s\n", getItematomfile());
    print("추측 문제 파일: %s\n", getQuizfile());
#ifdef _GMRELOAD
    print("G M 프로필: %s\n", getGMSetfile());
#endif
    print("로그 레코드 파일: %s\n", getLsgenlogfilename());
    print("데이터 카탈로그 복원: %s\n", getStoredir());
    print("NPC 구성 디렉토리: %s\n", getNpcdir());
    print("로그 레코드 파일: %s\n", getLogdir());
    print("로그 프로파일: %s\n", getLogconffile());
    print("GM의 지침 암호: %s\n", getChatMagicPasswd());
    print("GM 사용 권한을 사용하십시오: %d\n", getChatMagicCDKeyCheck());

    print("NPC 템플릿 수: %d\n", getNpctemplatenum());
    print("NPC 최대 수: %d\n", getNpccreatenum());

    print("걷는 시간 경과: %d\n", getWalksendinterval());
    print("모든 간격을 지우십시오: %d\n", getCAsendinterval_ms());
    print("목표 간격을 지우십시오: %d\n", getCDsendinterval_ms());
    print("실행 시간: %d\n", getOnelooptime_ms());
    print("애완 동물 허가 시간: %d\n", getPetdeletetime());
    print("소품 허가 시간: %d\n", getItemdeletetime());
#ifdef _DEL_DROP_GOLD
    print("돌 허가 시간: %d\n", getGolddeletetime());
#endif
    print("데이터 저장 간격: %d\n", getCharSavesendinterval());

    print("명함 최대 번호: %d\n", getAddressbookoffmsgnum());
    print("읽기 주파수 프로토콜: %d\n", getProtocolreadfrequency());

    print("연결 오류 상한값: %d\n", getAllowerrornum());
#ifdef _GET_BATTLE_EXP
    print("싸움 경험: %d倍\n", getBattleexp());
#endif
#ifdef _UNREG_NEMA
    print("금지 된 문자 이름: 이름1:%s 이름2:%s 이름3:%s4:%s 이름5:%s\n",
          getUnregname(0), getUnregname(1), getUnregname(2), getUnregname(3),
          getUnregname(4));
#endif

#ifdef _WATCH_FLOOR
    print("전체 그림이보다 낫 지 여부: %s\n", getWatchFloorCF());
    if (strcmp(getWatchFloorCF(), "是"))
      print("Watchmap을 허용하십시오: 지도1:%d 지도2:%d 지도3:%d 지도4:%d 지도5:%d\n",
            getWatchFloor(1), getWatchFloor(2), getWatchFloor(3),
            getWatchFloor(4), getWatchFloor(5));
#endif

#ifdef _BATTLE_FLOOR
    print("전투를 강요할지 여부: %s\n", getBattleFloorCF());
    if (strcmp(getBattleFloorCF(), "是"))
      print("필수 전투지도: 지도1:%d 지도2:%d 지도3:%d 지도4:%d 지도5:%d\n",
            getBattleFloor(1), getBattleFloor(2), getBattleFloor(3),
            getBattleFloor(4), getBattleFloor(5));
#endif

#ifdef _TRANS_LEVEL_CF
    print("캐릭터 순위 번호: %d级\n", getChartrans());
    print("애완 동물 등급 혁명: %d级\n", getPettrans());
#endif

#ifdef _PLAYERMAXPOINT
    print("상한의 금지: %s\n", getPoint());
    if (strcmp(getPoint(), "是"))
      print("최대 한도: 0턴:%d 1턴:%d 2턴:%d 3턴:%d 4턴:%d 5턴:%d 6턴:%d\n",
            getTransPoint(0), getTransPoint(1), getTransPoint(2),
            getTransPoint(3), getTransPoint(4), getTransPoint(5),
            getTransPoint(6));
#endif

#ifdef _PET_AND_ITEM_UP
    print("애완 동물은 붙잡을 수 있습니까?: %s\n", getPetup());
    print("소품이 붙잡을 수 있습니다: %s\n", getItemup());
#endif
#ifdef _SKILLUPPOINT_CF
    print("레벨 당 업그레이드 포인트: %d\n", getSkup());
#endif

#ifdef _RIDELEVEL
    print("승마 애완 동물 등급: %d级\n", getRideLevel());
#endif
#ifdef _REVLEVEL
    print("상한선 레벨을 복원하십시오: %s级\n", getRevLevel());
#endif
#ifdef _TRANS_LEVEL_CF
    print("일반 등급 상한값: %d级\n", getYBLevel());
    print("최대 상한값: %d级\n", getMaxLevel());
#endif
#ifdef _FIX_CHARLOOPS
    print("악마 시간: %d倍\n", getCharloops());
#endif
#ifdef _PLAYER_MOVE
    if (getPMove() == -1)
      print("쿠션 소비 지점: 폐쇄\n");
    else
      print("쿠션 소비 지점: %d点\n", getPMove());
#endif
#ifdef _BATTLE_GOLD
    print("싸움 돈을 얻으십시오: %d\n", getBattleGold());
#endif
#ifdef _ANGEL_TIME
    print("엘프 통화 시간: (%d人/在线人数)分\n", getAngelPlayerTime());
    print("엘프의 수는 소환되었습니다: %d人\n", getAngelPlayerMun());
#endif
#ifdef _RIDEMODE_20
    print("2.0 승마 PET 모드: %d\n", getRideMode());
#endif
#ifdef _FM_POINT_PK
    print("매너 상호 잡기 모드: %s\n", getFmPointPK());
#endif
  }
  {
    char *GameServerListName;
    GameServerListName = getGameserverListID();
    if (GameServerListName == NULL || strlen(GameServerListName) <= 0)
      return FALSE;
    print("\n게임 서버 행성 ID.: %s\n", GameServerListName);
  }

  { // andy_add 2003/05/05 check GameServer Name
    char *GameServerName;
    GameServerName = getGameserverID();
    if (GameServerName == NULL || strlen(GameServerName) <= 0)
      return FALSE;
    print("\n게임 서버 ID.: %s\n", GameServerName);
  }

  print("초기화를 시작하십시오\n");

  //#define DEBUG1( arg... ) if( getDebuglevel()>1 ){##arg}
  print("메모리 공간을 빌드하십시오...");
  GOTORETURNFALSEIFFALSE(configmem(getMemoryunit(), getMemoryunitnum()));
  print(" ...");
  GOTORETURNFALSEIFFALSE(memInit());
  print("마치다\n");

  print("항상 연결 공간을 연결하십시오...");
  if (!initConnect(getFdnum()))
    goto MEMEND;
  print("마치다\n");
  while (1) {
    print("로컬 포트를 묶으십시오 %d... ", getPortnumber());
    bindedfd = bindlocalhost(getPortnumber());
    if (bindedfd == -1)
      sleep(10);
    else
      break;
  }
  print("마치다\n");

  print("물체...");
  if (!initObjectArray(getObjnum()))
    goto CLOSEBIND;
  print("마치다\n");

  print("사람을 만드십시오...");
#ifdef _OFFLINE_SYSTEM
  if (!CHAR_initCharArray(getPlayercharnum(), getPetcharnum(),
                          getOtherscharnum()))
#else
  if (!CHAR_initCharArray(getFdnum(), getPetcharnum(), getOtherscharnum()))
#endif
    goto CLOSEBIND;
  print("마치다\n");

#ifdef _SASQL
  print("초기화 MySQL 데이터베이스...");
  if (sasql_init() == FALSE) {
    goto CLOSEBIND;
  }
  sasql_online(NULL, NULL, NULL, NULL, NULL, NULL, 3);
#endif

  print("항목을 짓는다...");
  if (!ITEM_readItemConfFile(getItemfile()))
    goto CLOSEBIND;
  if (!ITEM_initExistItemsArray(getItemnum()))
    goto CLOSEBIND;
  print("마치다\n");

  print("화이팅...");
  if (!BATTLE_initBattleArray(getBattlenum()))
    goto CLOSEBIND;
  print("마치다\n");

  print("기능 모듈을 설정하십시오...");
  if (!initFunctionTable())
    goto CLOSEBIND;
  print("마치다\n");

  print("초기화 메일...");
  if (!PETMAIL_initOffmsgBuffer(getAddressbookoffmsgnum()))
    goto CLOSEBIND;
  print("마치다\n");

  print("전투 파일에서 읽는 것...");
  if (!CHAR_initInvinciblePlace(getInvfile()))
    goto CLOSEBIND;
  print("마치다\n");

  print("디스플레이 위치 파일 읽기...");
  if (!CHAR_initAppearPosition(getAppearfile()))
    goto CLOSEBIND;
  print("마치다\n");

  print("제목 이름을 읽으십시오...");
  if (!TITLE_initTitleName(getTitleNamefile()))
    goto CLOSEBIND;
  print("마치다\n");

  print("제목 구성 파일을 읽으십시오...");
  if (!TITLE_initTitleConfig(getTitleConfigfile()))
    goto CLOSEBIND;
  print("마치다\n");

  print("적의 좌표 파일을 읽으십시오...");
  if (!ENCOUNT_initEncount(getEncountfile()))
    goto CLOSEBIND;
  print("마치다\n");

  print("애완 동물 기본 문서를 읽으십시오...");
  if (!ENEMYTEMP_initEnemy(getEnemyBasefile()))
    goto CLOSEBIND;
  print("마치다\n");

  print("PET 파일 만들기를 읽습니다...");
  if (!ENEMY_initEnemy(getEnemyfile()))
    goto CLOSEBIND;
  print("마치다\n");

  print("적 그룹 파일을 읽으십시오...");
  if (!GROUP_initGroup(getGroupfile()))
    goto CLOSEBIND;
  print("마치다\n");
  print("마술 파일을 읽으십시오...");
  if (!MAGIC_initMagic(getMagicfile()))
    goto CLOSEBIND;
  print("마치다\n");

#ifdef _ATTACK_MAGIC

  print("마법 공격 파일을 읽으십시오...");

  if (!ATTMAGIC_initMagic(getAttMagicfileName()))
    //		if( !ATTMAGIC_initMagic( getMagicfile() ) )
    goto CLOSEBIND;

  print("매직 공격 파일 -->%s...", getAttMagicfileName());
  print("마치다\n");

#endif

  print("애완 동물 스킬 파일을 읽으십시오...");
  if (!PETSKILL_initPetskill(getPetskillfile()))
    goto CLOSEBIND;
  print("마치다\n");

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
  print("전문 기술 파일을 읽으십시오...");
  if (!PROFESSION_initSkill(getProfession())) {
    goto CLOSEBIND;
  }
  print("마치다\n");
#endif

  /* 失奶  丞及笺    毛  戈 */
  print("항목 재료 파일 읽기...");
  if (!ITEM_initItemAtom(getItematomfile()))
    goto CLOSEBIND;
  print("마치다\n");

  print("초기화...");
  if (!ITEM_initItemIngCache())
    goto CLOSEBIND;
  print("마치다\n");

  print("초기 합성 임의 설정...");
  if (!ITEM_initRandTable())
    goto CLOSEBIND;
  print("마치다\n");

  print("적의 프로필을 읽으십시오...");
  if (!CHAR_initEffectSetting(getEffectfile()))
    goto CLOSEBIND;
  print("마치다\n");
  print("추측 문제 파일 읽기...");
  if (!QUIZ_initQuiz(getQuizfile()))
    goto CLOSEBIND;
  print("마치다\n");
#ifdef _GMRELOAD
  print("GM 구성 파일을 읽으십시오...");
  if (!LoadGMSet(getGMSetfile()))
    goto CLOSEBIND;
  print("마치다\n");
#endif

#ifdef _USER_EXP_CF
  print("읽기 경험 프로필...");
  if (!LoadEXP(getEXPfile()))
    goto CLOSEBIND;
  print("최고 수준: %d...", getMaxLevel());
  print("일반 등급: %d...", getYBLevel());
  print("마치다\n");
#endif

#ifdef _ANGEL_SUMMON
  print("마법사 소환 작업 목록 파일을 읽으십시오...");
  if (!LoadMissionList())
    goto CLOSEBIND;
  print("마치다\n");
#endif

#ifdef _JOBDAILY
  print("작업 로그 파일 읽기...");
  if (!LoadJobdailyfile())
    print("...실패하다\n");
  else
    print("마치다\n");
#endif
#ifdef _RIDE_CF
  print("맞춤 승마 애완 동물 파일을 읽으십시오...");
  if (!CHAR_Ride_CF_init())
    print("...실패하다\n");
  print("마치다\n");
#endif
#ifdef _FM_LEADER_RIDE
  print("애완 동물 파일을 타기 위해 하구의 강어귀를 읽으십시오....");
  if (!CHAR_FmLeaderRide_init())
    print("...실패하다\n");
  print("마치다\n");
#endif
#ifdef _RE_GM_COMMAND
  print("사용자 정의 GM 명령 파일의 이름을 바꿉니다...");
  if (!re_gm_command())
    print("...실패하다\n");
  print("마치다\n");
#endif
  print("지도를 수립하십시오...");
  if (!MAP_initReadMap(getMaptilefile(), getMapdir()))
    goto CLOSEBIND;
  print("마치다\n");
  print("NPC 파일 읽기...");
  if (!NPC_readNPCSettingFiles(getNpcdir(), getNpctemplatenum(),
                               getNpccreatenum()))
    goto CLOSEBIND;
  print("마치다\n");
#ifdef _ONLINE_SHOP
  print("온라인 쇼핑몰 구성 파일 읽기...");
  if (!OnlineShop_init())
    print("...실패하다\n");
  print("마치다\n");
#endif

#ifdef _FILTER_TALK
  print("음성 필터 파일을 읽으십시오...");
  if (!ReadFilterTalk())
    print("...실패하다\n");
  print("마치다\n");
#endif

  print("초기화 NPC 서버... ");
  if (lssproto_InitServer(lsrpcClientWriteFunc, LSGENWORKINGBUFFER) < 0)
    goto CLOSEBIND;
  print("마치다\n");
  print("계정 서버에 연결하십시오... ");
  acfd = connectHost(getAccountservername(), getAccountserverport());
  if (acfd == -1)
    goto CLOSEBIND;

  /*
          {
                  int errorcode;
                  int errorcodelen;
                  int qs;

                  errorcodelen = sizeof(errorcode);
                  qs = getsockopt( acfd, SOL_SOCKET, SO_RCVBUF , &errorcode,
     &errorcodelen);
                  //andy_log
                  print("\n\n GETSOCKOPT SO_RCVBUF: [ %d, %d, %d] \n", qs,
     errorcode, errorcodelen);
          }
  */

  print("마치다\n");
  initConnectOne(acfd, NULL, 0);
  if (!CONNECT_acfdInitRB(acfd))
    goto CLOSEAC;
  if (!CONNECT_acfdInitWB(acfd))
    goto CLOSEAC;
  CONNECT_setCtype(acfd, AC);

  print("初始化 账号 客户端 ... ");
  if (saacproto_InitClient(lsrpcClientWriteFunc, LSGENWORKINGBUFFER, acfd) < 0)
    goto CLOSEAC;
  print("마치다\n");

  print("向账号服务器发送登陆请求... ");
  /*  夫弘奶件邰菲毛请允  */
  {
    saacproto_ACServerLogin_send(acfd, getGameservername(),
                                 getAccountserverpasswd());
  }
  print("마치다\n");

  if (isExistFile(getLsgenlogfilename())) {
    lssproto_SetServerLogFiles(getLsgenlogfilename(), getLsgenlogfilename());
    saacproto_SetClientLogFiles(getLsgenlogfilename(), getLsgenlogfilename());
  }

  print("Init Bot\n");
  memset(&Bot, 0, sizeof(LeaderBot));
  print("Done Bot\n");
  print("初始化已完成\n");

  print("开始记寻新的日志 ... ");
  {
    char logconffile[512];
    snprintf(logconffile, sizeof(logconffile), "%s/%s", getLogdir(),
             getLogconffile());
    if (!initLog(logconffile))
      goto CLOSEAC;
  }
  print("마치다\n");
#ifdef _PET_ITEM
  restoreObjects(getStoredir());
#endif
#ifdef _ITEM_QUITPARTY
  print("读取队伍解散物品消失文件...");
  f = fopen(getitemquitparty(), "r");
  if (f != NULL) {
    while (fgets(line, sizeof(line), f)) {
      if (line[0] == '#')
        continue;
      if (line[0] == '\n')
        continue;
      chomp(line);
      itemquitparty_num++;
    }
    if (fseek(f, 0, SEEK_SET) == -1) {
      print("物品录找错误\n");
      fclose(f);
      goto CLOSEAC;
    }
    //配记忆体
    Disappear_Item =
        allocateMemory(sizeof(struct tagDisappearItem) * itemquitparty_num);
    if (Disappear_Item == NULL) {
      print("无法分配内存 %d\n",
            sizeof(struct tagDisappearItem) * itemquitparty_num);
      fclose(f);
      goto CLOSEAC;
    }
    i = 0;
    //将道具编号存入 Disappear_Item.string
    while (fgets(line, sizeof(line), f)) {
      if (line[0] == '#')
        continue;
      if (line[0] == '\n')
        continue;
      chomp(line);
      sprintf(Disappear_Item[i].string, "%s", line);
      print("\n道具编号:%s", Disappear_Item[i].string);
      i++;
    }
    fclose(f);
  }
#endif

  DEBUG_ADJUSTTIME = 0;
  print("\n");
  return TRUE;

CLOSEAC:
  close(acfd);
CLOSEBIND:
  close(bindedfd);
  endConnect();
MEMEND:
  memEnd();
RETURNFALSE:
  return FALSE;
}
