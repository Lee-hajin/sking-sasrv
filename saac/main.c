#define _MAIN_C_

#include "main.h"
#include "util.h"
#include "mail.h"
#include "db.h"
#include "saacproto_util.h"
#include "saacproto_serv.h"
// CoolFish: Family 2001/5/9
#include "acfamily.h"
#include "version.h"
#ifdef _SEND_EFFECT // WON ADD 送下雪、下雨等特效
#include "recv.h"
#endif
#include "md5.h"
#include "char.h"
#ifdef _SASQL
#include "sasql.h"
#endif
#include "vertime.h"

#include <stdio.h>
#include <time.h>

#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/wait.h>
#include <getopt.h>
#include <stdio.h>
#include <malloc.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ioctl.h>
#include <netinet/tcp.h>

#include "lock.h"
#define BACKLOGNUM 5

int worksockfd;

static int findregBlankMemBuf(void);
static int unregMemBuf(int index);
static int findregBlankCon(void);
static int getFreeMem(void);

static int appendReadBuffer(int index, char *data, int len);
static int appendWriteBuffer(int index, char *data, int len);
static int appendMemBufList(int top, char *data, int len);
static int consumeMemBufList(int top, char *out, int len, int flag, int copyflag);
static int getLineReadBuffer(int index, char *buf, int len);

void logerr(char *token);

struct membuf
{
    int use;
    char buf[512];
    //    char buf[1024*128];
    int len;
    int next;
};

struct connection
{
    int use;
    int fd;
    int mbtop_ri;
    int mbtop_wi;
    struct sockaddr_in remoteaddr;
    int closed_by_remote;
};

struct membuf *mb;
int mbsize;
int mbuse;

int cpuuse;

int mainsockfd;               /* accept 及  域娄醒卞中木月 */
struct sockaddr_in localaddr; /* bind 允月失玉伊旦 */

struct connection *con; /* 戊生弁扑亦件     */

static int mb_finder = 0; /* mb及坞五毛腹绸允月凶户及
                               腹绸玄永皿及匏  筏盛迕 */
// WON FIX
//char tmpbuf[65536];
char tmpbuf[1024 * 1024];
//char tmpbuf[65536*3];         /* read迕 */

struct timeval select_timeout;

time_t sys_time = 0; // Robin add

extern gmsv gs[MAXCONNECTION];

int tcpstruct_init(char *addr, int port, int timeout_ms, int mem_use, int deb);
int tcpstruct_accept1(void);
int tcpstruct_accept(int *tis, int ticount);
int tcpstruct_close(int ti);
int tcpstruct_read(int ti, char *buf, int len);
int tcpstruct_readline(int ti, char *buf, int len, int k, int r);
int tcpstruct_readline_chop(int ti, char *buf, int len);
int tcpstruct_write(int ti, char *buf, int len);
int tcpstruct_countmbuse(void);
int tcpstruct_connect(char *addr, int port);
void set_nodelay(int sock);

#define OK 0                      /* 岳   */
#define TCPSTRUCT_ENOMEM -1       /* malloc 撩   */
#define TCPSTRUCT_ESOCK -2        /* socket 撩   */
#define TCPSTRUCT_EBIND -3        /* bind 撩   */
#define TCPSTRUCT_ELISTEN -4      /* listen 撩   */
#define TCPSTRUCT_EBUG -6         /* 田弘匹丐月 */
#define TCPSTRUCT_EINVCIND -7     /* con尺及index互云井仄中方 */
#define TCPSTRUCT_EREADFIN -8     /* read 允月犯□正互卅仁化 closed by remote */
#define TCPSTRUCT_EHOST -9        /* gethostbyname 撩   */
#define TCPSTRUCT_ECONNECT -10    /* connect 撩   */
#define TCPSTRUCT_ECFULL -11      /* con 互中匀天中 */
#define TCPSTRUCT_ETOOLONG -12    /* 垫互卅互允亢 */
#define TCPSTRUCT_EMBFULL -13     /* mb 互中匀天中  */
#define TCPSTRUCT_ECLOSEAGAIN -14 /* close 互2荚今木凶 */

int port; /* 必□丞扔□田□互涛粮仄化仁月禾□玄 */
int Total_Charlist;
int Expired_mail;
int Del_Family_or_Member;
int Write_Family;

// Nuke start
char *chartime()
{
    static char buf[80];
    time_t t;
    t = time(0);
    strcpy(buf, ctime(&t));
    buf[strlen(buf) - 1] = 0;
    return (buf);
}
#ifdef _ANGEL_SUMMON
extern struct MissionTable missiontable[MAXMISSIONTABLE];
static int initMissionTable(void);
int saveMissionTable(void);
void checkMissionTimelimit(void);
#endif

/*
  sigaction白弁
 */

void sighandle(int a)
{
    if (a == SIGUSR1)
        log("sigusr1信号!\n");
    log("得到一个信号! 异常中断......\n");
    writeFamily(familydir);
    writeFMPoint(fmpointdir);
    writeFMSMemo(fmsmemodir);
#ifdef _ANGEL_SUMMON
    saveMissionTable();
#endif
    exit(1);
}

// Arminius 7.20 memory unlock
void sigusr1(int a)
{
    int i;
    FILE *f;
    char key[4096], buf[4096];

    signal(SIGUSR1, sigusr1);

    f = fopen("./unlock.arg", "r");

    if (f)
    {
        memset(key, 0, 4096);
        fread(key, 4096, 1, f);
        for (i = 0; i < strlen(key); i++)
            if (key[i] == '\n')
                key[i] = '\0';

        switch (key[0])
        {
        case 'P': // unlock player
            if (DeleteMemLock(getHash(&key[1]) & 0xff, &key[1], &i))
            {
                log("ADM: memunlock: %s success.\n", key);
            }
            else
            {
                log("ADM: memunlock: %s failed.\n", key);
            }
            break;
        case 'S': // unlock server
            DeleteMemLockServer(&key[1]);
            log("ADM: memunlock: %s\n", key);
            break;
        case 'C': // check player lock
            GetMemLockState(getHash(&key[1]) & 0xff, &key[1], buf);
            sprintf(key, "echo \"%s\" > ./sigusr1.result", buf);
            system(key);
            break;
#ifdef _SEND_EFFECT // WON ADD 送下雪、下雨等特效
        case 'E':
            log("\nAC 向 GS 发送下雪特效!!\n");
            SendEffect(&key[1]);
            break;
#endif

        case 'L': // Robin 列出所有Server连线
            log("\nList All Server Conncet!!!!!\n");
            for (i = 0; i < MAXCONNECTION; i++)
                if (gs[i].use)
                    log("\n gs[%d] fd:%d name:%s ", i, gs[i].fd, gs[i].name);
            break;
        }
        log(" sigusr1_over_1 ");
        fclose(f);
        log(" sigusr1_over_2 ");
    }
}

static int netWrite(int ti, char *buf, int len)
{
    return tcpstruct_write(ti, buf, len);
}

gmsv gs[MAXCONNECTION];
int login_game_server(int ti, char *svname, char *svpas, char *result, int resultlen, char *retdata, int retdatalen)
{
    if (strcmp(svpas, svpass) == 0)
    {
        log("서버 암호 올바른 %s\n", svname);
    }
    else
    {
        log("서버 암호 오류가 발생했습니다 %s\n", svname);
        snprintf(result, resultlen, "실패");
        snprintf(retdata, retdatalen, "잘못된 비밀번호");
        return 0;
    }
    {
        int i;
        for (i = 0; i < MAXCONNECTION; i++)
        {
            if (gs[i].use &&
                strcmp(gs[i].name, svname) == 0)
            {
                snprintf(result, resultlen, "실패");
                snprintf(retdata, retdatalen, "중복 로그인");
                return 0;
            }
        }
    }
    snprintf(gs[ti].name, sizeof(gs[ti].name), "%s", svname);
    gs[ti].fd = ti;
    snprintf(result, resultlen, SUCCESSFUL);
    snprintf(retdata, retdatalen, "공간 없음");
    DeleteMemLockServer(svname); // Arminius 7.31 unlock server
    return 0;
}

int logout_game_server(int ti)
{
    gs[ti].use = 0;
    gs[ti].fd = -1;
    gs[ti].name[0] = 0;
    tcpstruct_close(ti);
    printf("메모리 남음%f\n", (float)(((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION))));
    return 0;
}
int is_game_server_login(int ti)
{
    if (strlen(gs[ti].name) == 0)
    {
        return 0;
    }
    else
    {
        return gs[ti].use;
    }
}
static int readConfig(char *path)
{
    char buf[2048];
    FILE *fp;

    fp = fopen(path, "r");
    if (fp == NULL)
    {
        return -2;
    }

    while (fgets(buf, sizeof(buf), fp))
    {
        char command[128];
        char param[128];
        chop(buf);

        easyGetTokenFromString(buf, 1, command, sizeof(command));
        easyGetTokenFromString(buf, 2, param, sizeof(param));

        if (strcmp(command, "port") == 0)
        {
            port = atoi(param);
            log("포트:%d\n", port);
        }
        else if (strcmp(command, "logdir") == 0)
        {
            snprintf(logdir, sizeof(logdir), param);
            log("로그 카탈로그:%s\n", logdir);
        }
        else if (strcmp(command, "chardir") == 0)
        {
            snprintf(chardir, sizeof(chardir), param);
            log("파일 카탈로그:%s\n", chardir);
#ifdef _SLEEP_CHAR
            snprintf(sleepchardir, sizeof(sleepchardir), "%s_sleep", chardir);
            log("수면 카탈로그:%s\n", sleepchardir);
#endif
        }
        else if (strcmp(command, "pass") == 0)
        {
            snprintf(svpass, sizeof(svpass), param);
            log("비밀번호:%s\n", param);
        }
        else if (strcmp(command, "dbdir") == 0)
        {
            snprintf(dbdir, sizeof(dbdir), param);
            log("데이터 카탈로그:%s\n", dbdir);
        }
        else if (strcmp(command, "rotate_interval") == 0)
        {
            log_rotate_interval = atoi(param);
            log("로그주기 간격:%d\n", log_rotate_interval);
        }
        else if (strcmp(command, "maildir") == 0)
        {
            snprintf(maildir, sizeof(maildir), param);
            log("메일 카탈로그:%s\n", maildir);
        }
#ifdef _FAMILY
        else if (strcmp(command, "familydir") == 0)
        {
            snprintf(familydir, sizeof(familydir), param);
            log("가족 카탈로그:%s\n", familydir);
        }
        else if (strcmp(command, "fmpointdir") == 0)
        {
            snprintf(fmpointdir, sizeof(fmpointdir), param);
            log("매너 테이블:%s\n", fmpointdir);
        }
        else if (strcmp(command, "fmsmemodir") == 0)
        {
            snprintf(fmsmemodir, sizeof(fmsmemodir), param);
            log("가족 백업:%s\n", fmsmemodir);
        }
#endif
        else if (strcmp(command, "Total_Charlist") == 0)
        {
            Total_Charlist = atoi(param);
            log("사람들 포인트 간격을 업데이트하십시오:%d秒\n", Total_Charlist);
        }
        else if (strcmp(command, "Expired_mail") == 0)
        {
            Expired_mail = atoi(param);
            log("만료 된 메일 간격 업데이트:%d秒\n", Expired_mail);
        }
        else if (strcmp(command, "Del_Family_or_Member") == 0)
        {
            Del_Family_or_Member = atoi(param);
            log("가족 구성원 삭제:%d秒\n", Del_Family_or_Member);
        }
        else if (strcmp(command, "Write_Family") == 0)
        {
            Write_Family = atoi(param);
            log("가족 정보 간격 업데이트:%d秒\n", Write_Family);
        }
        else if (strcmp(command, "SameIpNum") == 0)
        {
            sameipnum = atoi(param);
            if (sameipnum > 0)
            {
                log("IP로 로그인 할 수 있습니다:%d次\n", sameipnum);
            }
            else
            {
                log("IP로 로그인 할 수 있습니다:无限制\n");
            }
        }
        else if (strcmp(command, "CPUUSE") == 0)
        {
            cpuuse = atoi(param);
            log("CPU 사용 빈도:%d秒\n", cpuuse);
        }
#ifdef _OLDPS_TO_MD5PS
        else if (strcmp(command, "USEMD5") == 0)
        {
            usemd5 = atoi(param);
            log("MD5 암호를 사용할지 여부:%d\n", usemd5);
        }
#endif
#ifdef _LOTTERY_SYSTEM
        else if (strcmp(command, "LOTTERYSYSTEM") == 0)
        {
            lotterysystem = atoi(param);
            log("모든 %d 복권을 수상하십시오!", lotterysystem);
        }
#endif
    }

    fclose(fp);
    return 0;
}

static void parseOpts(int argc, char **argv)
{
    int c, option_index;

    while (1)
    {
        static struct option long_options[] = {
            {"help", 0, 0, 'h'},
        };
        c = getopt_long(argc, argv, "n:bchilmdt", long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
        case 'h':
            fprintf(stderr,
                    "使用方法: saac [-h] [-w port] [-w port] ... \n"
                    "-h : 显示saac的帮助\n"
                    "-w port : 添加一个工作站进程端口\n"
                    "Copyright 2006 龙zoro工作室 "
                    "( Longzoro System Supply )\n");
            exit(0);
            break;
        default:
            log("옵션을 읽을 수 없습니다 %c\n", c);
            exit(0);
        }
    }
}
double time_diff(struct timeval subtrahend, struct timeval subtractor);

int passwd = 0;

void dump()
{
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    printf("Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++)
    {
        logerr(strings[i]);
        printf("\n");
    }

    free(strings);
}

void sigshutdown(int number)
{
    char buff[256];

    if (number == 0)
    {
        printf("\n\n\nGMSV 일반적으로 닫습니다\n");
    }
    else if (number == 2)
    {
        printf("\n\n\nGMSV CTRL+C 수동 인터럽트\n");
    }
    else
    {
        sprintf(buff, "다음은 주요 실수이며, 우리에게 제출해야합니다.\n");
        logerr(buff);
        dump();
    }

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGILL, SIG_IGN);
    signal(SIGTRAP, SIG_IGN);
    signal(SIGIOT, SIG_IGN);
    signal(SIGBUS, SIG_IGN);
    signal(SIGFPE, SIG_IGN);
    signal(SIGKILL, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    log("신호를 얻으십시오! 비정상적인 중단.....\n");
    writeFamily(familydir);
    writeFMPoint(fmpointdir);
    writeFMSMemo(fmsmemodir);
#ifdef _ANGEL_SUMMON
    saveMissionTable();
#endif
    exit(1);
}

void signalset(void)
{
    // CoolFish: Test Signal 2001/10/26
    printf("\n 신호 받기 시작..\n");

    printf("SIGINT:%d\n", SIGINT);
    printf("SIGQUIT:%d\n", SIGQUIT);
    printf("SIGFPE:%d\n", SIGILL);
    printf("SIGTRAP:%d\n", SIGTRAP);
    printf("SIGIOT:%d\n", SIGIOT);
    printf("SIGBUS:%d\n", SIGBUS);
    printf("SIGFPE:%d\n", SIGFPE);
    printf("SIGKILL:%d\n", SIGKILL);
    printf("SIGSEGV:%d\n", SIGSEGV);
    printf("SIGPIPE:%d\n", SIGPIPE);
    printf("SIGTERM:%d\n", SIGTERM);

    signal(SIGINT, sigshutdown);
    signal(SIGQUIT, sigshutdown);
    signal(SIGILL, sigshutdown);
    signal(SIGTRAP, sigshutdown);
    signal(SIGIOT, sigshutdown);
    signal(SIGBUS, sigshutdown);
    signal(SIGFPE, sigshutdown);
    signal(SIGKILL, sigshutdown);
    signal(SIGSEGV, sigshutdown);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, sigshutdown);
}

int main(int argc, char **argv)
{
    /*
		#define cpuid(in,a,b,c,d)\
  	asm("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));
    unsigned long eax,ebx,ecx,edx;
    cpuid(0,eax,ebx,ecx,edx);
    printf("%08x %08lx %08lx %08lx %08lx\n",0,eax,ebx,ecx,edx);
*/

    srand((int)time(0));
    parseOpts(argc, argv);
    signalset();
    // Nuke +1 1012: Loop counter
    int counter1 = 0;

    int counter2 = 0;

    int counter3 = 0;

    int counter4 = 0;

    int counter5 = 0;

    int counter6 = 0;
    //signal(SIGUSR1, sigusr1);

    log_rotate_interval = 3600 * 24 * 7;

    Lock_Init(); // Arminius 7.17 memory lock

    UNlockM_Init();

    if (readConfig("acserv.cf") < 0)
    {
        log("현재 디렉토리에서 읽을 수 없습니다 acserv.cf .\n");
        exit(1);
    }

#ifdef _SASQL
    if (sasql_init() == FALSE)
    {
        exit(1);
    }
#ifdef _SQL_BACKGROUND
    sasql_online(NULL, NULL, NULL, NULL, NULL, NULL, 3);
#endif
#endif
    log("데이터 디렉토리를 읽으십시오\n");
    dbRead(dbdir);
#ifdef _FAMILY
    log("독서 가족 재산\n");
    readFMSMemo(fmsmemodir);
    log("가족 메시지를 읽는 것\n");
    readFMPoint(fmpointdir);
    log("패밀리 디렉토리를 읽으십시오\n");
    readFamily(familydir);
#endif
    log("준비 파일 카탈로그\n");
    prepareDirectories(chardir);
    log("로그 카탈로그를 준비하십시오\n");
    prepareDirectories(logdir);
    log("준비 메일 디렉토리\n");
    prepareDirectories(maildir);

#ifdef _SLEEP_CHAR
    prepareDirectories(sleepchardir);
    log("수면 파일 카탈로그 준비\n");
#endif

    /* 凶引匀化月丢□伙毛  心仇戈 */
    if (readMail(maildir) < 0)
    {
        log("메일을 초기화 할 수 없습니다\n");
        exit(1);
    }
    /* TCPSTRUCT 毛赓渝祭 */
    while (1)
    {
        int tcpr;
        if ((tcpr = tcpstruct_init(NULL, port, 0,
                                   CHARDATASIZE * 16 * MAXCONNECTION, 1 /* DEBUG */)) < 0)
        {
            log("TCP포트를 들을 수 없습니다. 에러 코드: %d, 기다리세요...\n", tcpr);
            sleep(10);
        }
        else
        {
            break;
        }
    }
    saacproto_InitServer(netWrite, CHARDATASIZE);
/*
    {
        struct sigaction s,os;
        
        bzero( &s, sizeof(s));
        s.sa_handler = sighandle;
        s.sa_flags = SA_NOMASK;
        sigaction( SIGTERM,  &s, &os );

        bzero( &s, sizeof(s));
        s.sa_handler = sighandle;
        s.sa_flags = SA_NOMASK;
        sigaction( SIGINT,  &s, &os );

        bzero( &s, sizeof( s ));
        s.sa_handler = SIG_IGN;
        s.sa_flags = SA_NOMASK;
        sigaction( SIGPIPE, &s, &os );

    }
*/
#ifdef _AC_SEND_FM_PK // WON ADD 庄园对战列表储存在AC
    load_fm_pk_list();
#endif

#ifdef _ACFMPK_LIST
    FMPK_LoadList();
#endif
#ifdef _ALLDOMAN
    LOAD_herolist(); // Syu ADD 排行榜NPC
#endif

#ifdef _ANGEL_SUMMON
    initMissionTable();
#endif

    log("\n\033[35m※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※");
    log("\n※ 컴파일 시간 :%-56s※", SERVER_VERTIME);
    log("\n※%-66s※", SERVER_VERSION);
#ifdef SERVER_VERSIONCF
    log("\n※%-66s※", SERVER_VERSION1);
    log("\n※%-66s※", SERVER_VERSION2);
    log("\n※%-66s※", SERVER_VERSION3);
#endif
    log("\n※\033[31m%-66s\033[35m※", "이 엔드는 기술적 교환으로 제한되며 상업적 목적으로 사용하지 마십시오. 모든 법적 책임을 만지면 저자와 아무 관련이 없습니다.");
    log("\n※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※\033[0m\n");
    log("\n시작...\n");

    // signal(SIGUSR1,sigusr1);	// Arminius 7.20 memory lock

    int itime = 0;
    while (1)
    {
        itime++;
        if (itime > cpuuse)
        {
            itime = 0;
            usleep(1);
        }
        int newti, i;
        static time_t main_loop_time;

        sys_time = time(NULL);

#ifdef _LOTTERY_SYSTEM
        char todayaward[256] = "-1,-1,-1,-1,-1,-1,-1";
        {
            if (lotterysystem > 0)
            {
                struct tm *p;
                p = localtime(&sys_time); /*取得当地时间*/
                static BOOL lottery = FALSE;
                if (lottery == FALSE)
                {
                    if ((p->tm_mday % lotterysystem) == 0)
                    {
                        if (p->tm_hour == 0)
                        {
                            int award[7];
                            int i, j;
                            for (i = 0; i < 7; i++)
                            {
                                award[i] = rand() % 36 + 1;
                                for (j = 0; j < i; j++)
                                {
                                    if (award[i] == award[j])
                                    {
                                        award[i] = rand() % 36 + 1;
                                        j = 0;
                                    }
                                }
                            }
                            sprintf(todayaward, "%d,%d,%d,%d,%d,%d,%d", award[0],
                                    award[1],
                                    award[2],
                                    award[3],
                                    award[4],
                                    award[5],
                                    award[6]);
                            FILE *f1 = fopen("todayaward.txt", "w+");
                            fputs(todayaward, f1);
                            fclose(f1);
                            for (i = 0; i < MAXCONNECTION; i++)
                            {
                                if (gs[i].use && gs[i].name[0])
                                {
                                    saacproto_LotterySystem_send(i, todayaward);
                                }
                            }
                            lottery = TRUE;
                        }
                    }
                }
                else
                {
                    if (p->tm_hour != 0)
                    {
                        lottery = FALSE;
                    }
                }
            }
        }
#endif

        if (main_loop_time != sys_time)
        {
            main_loop_time = time(NULL);
            counter1++;
            counter2++;
            counter3++;
            counter4++;
            counter5++;
            counter6++;
            if (counter6 > 600) // 300( -> 60)
            {
                readConfig("acserv.cf");
                counter6 = 0;
            }
            //andy add 2002/06/20
            UNlockM_UnlockPlayer();

#ifdef _ANGEL_SUMMON
            checkMissionTimelimit();
#endif
            // Nuke *1 1012
            if (counter1 > Total_Charlist)
            {
                counter1 = 0;
                char *c = ctime(&main_loop_time);
                if (c)
                {
                    struct timeval st, et;
                    log("\nTIME:%s\n", c);
                    gettimeofday(&st, NULL);
                    dbFlush(dbdir);
                    gettimeofday(&et, NULL);
                    log("Flushed db(%fsec)\n", time_diff(et, st));
                    log("전체 파일 목록 수:%d NG:%d\n",
                        total_ok_charlist, total_ng_charlist);
                }
            }
            // Nuke **1 1012
            //if( ( counter % 600 ) == 0 ){
            if (counter2 > Expired_mail)
            {
                counter2 = 0;
                struct timeval st, et;
                gettimeofday(&st, NULL);
                expireMail();
                gettimeofday(&et, NULL);
                log("만료(%fsec)\n", time_diff(et, st));
            }
#ifdef _FAMILY
#ifdef _DEATH_FAMILY_LOGIN_CHECK
            //if ((counter % 300) == 0) // 300( -> 60)
            if ((counter4 % 1800) == 0) // 3hr( -> 1min)
            {
                counter4 = 0;
                struct timeval st, et;
                time_t t1;
                gettimeofday(&st, NULL);
                time(&t1);
                delovertimeFMMem(t1);
                gettimeofday(&et, NULL);
                log("Del Family or Member (%fsec)\n", time_diff(et, st));
            }
#endif
            if (counter5 > Write_Family) // 300( -> 60)
            {
                counter5 = 0;
                struct timeval st, et;
                gettimeofday(&st, NULL);
                writeFamily(familydir);
                writeFMPoint(fmpointdir);
                writeFMSMemo(fmsmemodir);
                gettimeofday(&et, NULL);
                log("가족(%fsec)\n", time_diff(et, st));
            }
#endif
        }

        newti = tcpstruct_accept1();
        if (newti >= 0)
        {
            log("동의하다: %d\n", newti);
            gs[newti].use = 1;
        }

        for (i = 0; i < MAXCONNECTION; i++)
        {
            //      char buf[CHARDATASIZE * 16;
            char buf[CHARDATASIZE];
            int l;
            l = tcpstruct_readline_chop(i, buf, sizeof(buf) - 1);
            {
                if (!gs[i].use)
                    continue;
                if (l > 0)
                {
                    char debugfun[256];
                    buf[l] = 0;
                    if (saacproto_ServerDispatchMessage(i, buf, debugfun) < 0)
                    {
                        printf("buf:%s;%d\n", buf, strlen(buf));
                        char token[256];
                        char tmp[256];
                        struct tm now;
                        time_t timep;
                        time(&timep);
                        memcpy(&now, localtime(&timep), sizeof(now));

                        sprintf(tmp, "%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);

                        sprintf(token, "[%s]GMSV(%s) 정보:%s\n", tmp, gs[i].name, debugfun);
                        logerr(token);
                        // Nuke start
                        //if(strlen(debugfun)>0){
                        //logout_game_server(i);// avoid the shutdown the gmsv ttom
                        //}
                    }
                }
                else if (l == TCPSTRUCT_ETOOLONG)
                {
                    char token[256];
                    sprintf(token, "매우 긴:%d 서버 이름:%s\n", i, gs[i].name);
                    logerr(token);
                    logout_game_server(i);
                }
                else if (l < 0)
                {
                    log("폐쇄:%d 서버 이름:%s\n", i, gs[i].name);
                    logout_game_server(i);
                }
                else if (l == 0)
                {
                    ;
                }
            }
        }
    }

    return 0;
}
double
time_diff(struct timeval subtrahend,
          struct timeval subtractor)
{
    return ((subtrahend.tv_sec - subtractor.tv_sec) + (subtrahend.tv_usec - subtractor.tv_usec) / (double)1E6);
}

/*
  夫弘及夫□  □玄市它件玄毛  月［
  云卅元凛渝及手及反允屯化及夫弘白央奶伙互云卅元卞卅月方丹卞允月［

  仇及酷  反醒侬及瑕互云云仁卅月互｝    匹奶件正□田伙毛  凳仄化手
    暹屺分［
  
 */
int get_rotate_count(void)
{
    int a;
    unsigned int t = (unsigned int)time(NULL);

    a = (t / log_rotate_interval) * log_rotate_interval;

    return a;
}

int tcpstruct_init(char *addr, int p, int timeout_ms, int mem_use, int db)
{
    mbsize = mem_use / sizeof(struct membuf);
    mbuse = 0;
    mb_finder = 0;
    mb = (struct membuf *)calloc(1, mbsize * sizeof(struct membuf));

    if (mb == NULL)
        return TCPSTRUCT_ENOMEM;
    bzero(mb, mbsize * sizeof(struct membuf));

    con = (struct connection *)calloc(1, MAXCONNECTION * sizeof(struct connection));
    if (con == NULL)
    {
        free(mb);
        return TCPSTRUCT_ENOMEM;
    }
    else
    {
        int i;
        for (i = 0; i < MAXCONNECTION; i++)
        {
            con[i].use = 0;
            con[i].fd = -1;
        }
    }
    select_timeout.tv_sec = timeout_ms / 1000;
    select_timeout.tv_usec = (timeout_ms - (timeout_ms / 1000) * 1000) * 1000;

    /* socket */
    mainsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (mainsockfd < 0)
        return TCPSTRUCT_ESOCK;

    /* bind */
    bzero(&localaddr, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(p);

    if (addr)
    {
        localaddr.sin_addr.s_addr = inet_addr(addr);
    }
    else
    {
        localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    if (bind(mainsockfd, (struct sockaddr *)&localaddr,
             sizeof(localaddr)) < 0)
        return TCPSTRUCT_EBIND;

    /* listen */
    if (listen(mainsockfd, BACKLOGNUM) < 0)
        return TCPSTRUCT_ELISTEN;

    return OK;
}

int tcpstruct_accept1(void)
{
    int tis[BACKLOGNUM];
    int ret;

    ret = tcpstruct_accept(tis, 1);
    if (ret < 0)
    {
        return ret;
    }
    else if (ret == 1)
    {
        return tis[0];
    }
    else
    {
        return TCPSTRUCT_EBUG;
    }
}

int tcpstruct_accept(int *tis, int ticount)
{
    int i;
    int sret = 0;
    int accepted = 0;
    struct timeval t;
    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    for (i = 0; i < MAXCONNECTION; i++)
    {
        if (con[i].use &&
            con[i].fd >= 0 && con[i].closed_by_remote == 0)
        {
            FD_SET(con[i].fd, &rfds);
            FD_SET(con[i].fd, &wfds);
            FD_SET(con[i].fd, &efds);

            int j = 1, k;

            if ((float)(((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION))) > 0.10)
            {
                t = select_timeout;
                sret = select(con[i].fd + 1, &rfds, (fd_set *)NULL, &efds, &t);
                if (sret > 0)
                {
                    if ((con[i].fd >= 0) && FD_ISSET(con[i].fd, &rfds))
                    {
                        int fr = getFreeMem();
                        int rr, readsize;
                        if (fr <= 0)
                            continue;
                        memset(tmpbuf, 0, sizeof(tmpbuf));
                        if (fr > sizeof(tmpbuf))
                        {
                            readsize = sizeof(tmpbuf);
                        }
                        else
                        {
                            readsize = fr - 1;
                        }
                        rr = read(con[i].fd, tmpbuf, readsize);
                        if (rr <= 0)
                        {
                            con[i].closed_by_remote = 1;
                        }
                        else
                        {
                            appendReadBuffer(i, tmpbuf, rr);
#ifdef _DEBUG
                            printf("콘텐츠 읽기:%s\n", tmpbuf);
#endif
                        }
                    }
                }

                if ((float)(((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION))) > 0.50)
                {
                    j = 2;
                }
                else if ((float)(((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION))) > 0.40)
                {
                    j = 3;
                }
                else if ((float)(((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION))) > 0.30)
                {
                    j = 4;
                }
                else if ((float)(((float)getFreeMem() / (CHARDATASIZE * 16 * MAXCONNECTION))) > 0.20)
                {
                    j = 5;
                }
            }

            for (k = 0; k < j; k++)
            {
                t = select_timeout;
                sret = select(con[i].fd + 1, (fd_set *)NULL, &wfds, (fd_set *)NULL, &t);
                if (sret > 0)
                {
                    if ((con[i].fd >= 0) && FD_ISSET(con[i].fd, &wfds))
                    {
                        char send_buf[4096];
                        memset(send_buf, 0, sizeof(send_buf));
                        int l = consumeMemBufList(con[i].mbtop_wi, send_buf, sizeof(send_buf), 0, 1);
                        if (l > 0)
                        {
                            int rr = write(con[i].fd, send_buf, l);
                            if (rr < 0)
                            {
                                con[i].closed_by_remote = 1;
                            }
                            else
                            {
#ifdef _DEBUG
                                printf("콘텐츠 보내기:%s\n", send_buf);
#endif
                                consumeMemBufList(con[i].mbtop_wi, send_buf, l, 1, 0);
                            }
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < ticount; i++)
    {
        int asret;
        struct timeval t;
        t.tv_sec = 0;
        t.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);
        FD_SET(mainsockfd, &rfds);
        FD_SET(mainsockfd, &wfds);
        FD_SET(mainsockfd, &efds);
        asret = select(mainsockfd + 1, &rfds, &wfds, &efds, &t);
        // Nuke 20040610: add asret>0 to avoid signal interrupt in select
        if ((asret > 0) && FD_ISSET(mainsockfd, &rfds))
        {
            struct sockaddr_in c;
            int len, newsockfd;
            int newcon;
            bzero(&c, sizeof(c));
            len = sizeof(c);
            fprintf(stderr, "i can accept ");
            newcon = findregBlankCon();
            if (newcon < 0)
                continue;
            newsockfd = accept(mainsockfd, (struct sockaddr *)&c, &len);
            log("동의하다: %d\n", newsockfd);
            if (newsockfd < 0)
            {
                unregMemBuf(newcon);
                continue;
            }
            char ipbuf[64];
            char ipmsg[2048];
            FILE *ipfp;
            ipfp = fopen("ip.txt", "r");
            if (ipfp != NULL)
            {
                //log("111111111\n");
                while (fgets(ipbuf, sizeof(ipbuf), ipfp))
                {
                    sprintf(ipmsg, "%s%s", ipmsg, ipbuf);
                }
                fclose(ipfp);

                unsigned long sinip;
                memcpy(&sinip, &c.sin_addr, 4);
                int ipa, ipb, ipc, ipd;
                char ip[32];
                ipa = (sinip % 0x100);
                sinip = sinip / 0x100;
                ipb = (sinip % 0x100);
                sinip = sinip / 0x100;
                ipc = (sinip % 0x100);
                sinip = sinip / 0x100;
                ipd = (sinip % 0x100);
                sprintf(ip, ".%d.%d.%d.%d.", ipa, ipb, ipc, ipd);
                //log("ip=%s,ip1=%s\n",ip,ipmsg);
                if (strstr(ipmsg, ip) == NULL)
                {
                    close(newsockfd);
                    unregMemBuf(newcon);
                    continue;
                }
            }
            set_nodelay(newsockfd);
            con[newcon].fd = newsockfd;
            memcpy(&con[newcon].remoteaddr, &c, sizeof(c));
            tis[accepted] = newcon;
            accepted++;
        }
    }
    return accepted;
}

int tcpstruct_close(int ti)
{

    if (ti < 0 || ti >= MAXCONNECTION)
        return TCPSTRUCT_EINVCIND;
    if (con[ti].use == 0)
    {
        return TCPSTRUCT_ECLOSEAGAIN;
    }
    close(con[ti].fd);
    con[ti].use = 0;
    con[ti].fd = -1;

    /* 伉旦玄毛凶升匀化蝈  毛弁伉失允月 */
    consumeMemBufList(con[ti].mbtop_ri, NULL,
                      mbsize * sizeof(mb[0].buf), 1, 0);
    consumeMemBufList(con[ti].mbtop_wi, NULL,
                      mbsize * sizeof(mb[0].buf), 1, 0);

    unregMemBuf(con[ti].mbtop_ri);
    unregMemBuf(con[ti].mbtop_wi);
    con[ti].mbtop_ri = -1;
    con[ti].mbtop_wi = -1;
    return OK;
}

/*
    心仇戈手及互手丹卅仁化｝仄井手 remoteclose 分匀凶日 -1 毛井尹允
  
 */
int tcpstruct_read(int ti, char *buf, int len)
{
    int l;

    if (ti < 0 || ti >= MAXCONNECTION || con[ti].use == 0)
        return TCPSTRUCT_EINVCIND;
    l = consumeMemBufList(con[ti].mbtop_ri, buf, len, 1, 1);
    if (l == 0 && con[ti].closed_by_remote)
        return TCPSTRUCT_EREADFIN;

    return l;
}

/*
  1垫毛方心仇戈［
  int kend : 1卅日垫  及 \n 毛诮允
  int kend_r : 1卅日垫  及 \r 手诮允(丐木壬)

    心仇戈手及互手丹卅仁化｝井勾 remote closed 分匀凶日-1毛井尹允
  // Nuke
	Read 1 line
	if kend==1 then delete \n at the tail
	if kend_r== 1 then delete \r at the tail (if any)
	if no data read, it means 'remote closed' then return -1
 */
int tcpstruct_readline(int ti, char *buf, int len, int kend, int kend_r)
{
    int l;
    int minus = 0;

    if (ti < 0 || ti >= MAXCONNECTION || con[ti].use == 0)
        return TCPSTRUCT_EINVCIND;

    l = getLineReadBuffer(ti, buf, len);
    if (l == 0)
    {
        if (con[ti].closed_by_remote)
        {
            return TCPSTRUCT_EREADFIN;
        }
        else
        {
            return 0;
        }
    }

    if (kend)
    {
        if (buf[l - 1] == '\n')
        {
            buf[l - 1] = 0;
            minus = -1;
        }
    }
    if (kend_r)
    {
        if (buf[l - 1] == '\r')
        {
            buf[l - 1] = 0;
            minus = -1;
        }
        if (buf[l - 2] == '\r')
        {
            buf[l - 2] = 0;
            minus = -2;
        }
    }
    return l + minus;
}
int tcpstruct_readline_chop(int ti, char *buf, int len)
{
    return tcpstruct_readline(ti, buf, len, 1, 1);
}

int tcpstruct_write(int ti, char *buf, int len)
{
    if (ti < 0 || ti >= MAXCONNECTION || con[ti].use == 0)
        return TCPSTRUCT_EINVCIND;
    return appendWriteBuffer(ti, buf, len);
}

int tcpstruct_connect(char *addr, int port)
{
    int newti;
    int s, r;
    struct sockaddr_in svaddr;
    struct hostent *he;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        return -2;

    bzero(&svaddr, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(port);

    if (inet_aton(addr, &svaddr.sin_addr) == 0)
    {
        he = gethostbyname(addr);
        if (he == NULL)
        {
            return TCPSTRUCT_EHOST;
        }
        memcpy(&svaddr.sin_addr.s_addr, he->h_addr,
               sizeof(struct in_addr));
    }
    r = connect(s, (struct sockaddr *)&svaddr, sizeof(svaddr));
    if (r < 0)
    {
        return TCPSTRUCT_ECONNECT;
    }
    set_nodelay(s);
    newti = findregBlankCon();
    if (newti < 0)
    {
        fprintf(stderr, "연결에 실패: newti:%d\n", newti);
        return TCPSTRUCT_ECFULL;
    }
    con[newti].fd = s;
    memcpy(&con[newti].remoteaddr, &svaddr,
           sizeof(struct sockaddr_in));
    return newti;
}

static int appendReadBuffer(int index, char *data, int len)
{
    int top;

    top = con[index].mbtop_ri;
    for (;;)
    {
        int nextind = mb[top].next;

        if (nextind == -1)
            break;
        top = nextind;
    }
    return appendMemBufList(top, data, len);
}

static int appendWriteBuffer(int index, char *data, int len)
{
    int top;
    top = con[index].mbtop_wi;
    for (;;)
    {
        int nextind = mb[top].next;
        if (nextind == -1)
            break;
        top = nextind;
    }
    return appendMemBufList(top, data, len);
}
static int appendMemBufList(int top, char *data, int len)
{
    int fr = getFreeMem();
    int rest = len;
    int data_topaddr = 0;

    if (len >= fr)
    {
        /*
		FILE *fp;
		if( (fp=fopen( "badsysinfo.txt", "a+")) != NULL ){
			fprintf( fp, "appendMemBufList() len:%d / fr:%d err !! \n", len, fr);
			fclose( fp);
		}
*/
        //andy_log
        log("appendMemBufList() len:%d / fr:%d err !! \n", len, fr);
        return -1;
    }
    data[len] = 0;
    for (;;)
    {
        int blanksize = sizeof(mb[0].buf) - mb[top].len;
        int cpsize = (rest <= blanksize) ? rest : blanksize;
        memcpy(mb[top].buf + mb[top].len,
               data + data_topaddr, cpsize);
        mb[top].len += cpsize;
        if (rest <= blanksize)
        {
            return len;
        }
        else
        {
            int newmb;
            rest -= cpsize;
            data_topaddr += cpsize;
            if ((newmb = findregBlankMemBuf()) == TCPSTRUCT_EMBFULL)
            {
                FILE *fp;
                if ((fp = fopen("badsysinfo.txt", "a+")) != NULL)
                {
                    fprintf(fp, "find newmb == TCPSTRUCT_EMBFULL err data:%s !!\n", data);
                    fclose(fp);
                }
                log("find newmb == TCPSTRUCT_EMBFULL err data:%s !!\n", data);
            }
            mb[top].next = newmb;
            top = mb[top].next;
        }
    }
    return TCPSTRUCT_EBUG;
}

static int consumeMemBufList(int top, char *out, int len,
                             int consumeflag, int copyflag)
{
    int total = 0;
    int top_store = top;
    for (;;)
    {
        int cpsize;
        if (top == -1)
            break;
        cpsize = (mb[top].len <= (len - total)) ? mb[top].len : (len - total);

        if (copyflag)
            memcpy(out + total, mb[top].buf, cpsize);
        total += cpsize;

        if (consumeflag)
        {
            mb[top].len -= cpsize;
            if (mb[top].len > 0)
            {
                /* 勾井中反凶仄化卅中及匹memmove */
                memmove(mb[top].buf, mb[top].buf + cpsize,
                        sizeof(mb[top].buf) - cpsize);
            }
        }
        top = mb[top].next;
        if (total == len)
        {
            break;
        }
    }

    if (consumeflag)
    {
        /* 卅互今互0卞卅匀化月卅日荸  ［匹手  赓及支勾反荸  仄卅中冗 */
        top = mb[top_store].next;
        for (;;)
        {
            if (top == -1)
                break;
            if (mb[top].len == 0)
            {
                int prev;
                mb[top_store].next = mb[top].next;
                prev = top;
                top = mb[top].next;
                unregMemBuf(prev);
            }
            else
            {
                top = mb[top].next;
            }
        }
    }

    return total;
}

static int getLineReadBuffer(int index, char *buf, int len)
{

    int top = con[index].mbtop_ri;
    int ti = 0, breakflag = 0;

    for (;;)
    {
        int i;
        int l = mb[top].len;
        if (top == -1)
            break;
        for (i = 0; i < l; i++)
        {
            if (mb[top].buf[i] == '\n')
            {
                breakflag = 1;
                break;
            }
            ti++;
        }
        if (breakflag)
            break;
        top = mb[top].next;
    }
    if (ti > len)
    {
        /* 1垫互卅互允亢月［    卅巨仿□毛井尹六 */
        return TCPSTRUCT_ETOOLONG;
    }
    /* 垫互敦岳仄化卅中 */
    if (breakflag == 0)
    {
        return 0;
    }

    return consumeMemBufList(con[index].mbtop_ri, buf, ti + 1, 1, 1);
}

/*
    心仇户月    赢今毛忒允
  int index : con index

  return:
    反巨仿□
  0动晓及桦宁反 read 仄化手方中赢今［


  mbsize 井日mbuse 毛娄中化扔奶术毛井仃月分仃［
  仇引井中芴曰及坌反  骰允月［公氏卅称井中袄卞仇分歹日卅仁化手第［
  
 */
static int getFreeMem(void)
{
    return (mbsize - mbuse) * sizeof(mb[0].buf);
}

/*
  
  membuf 及坞五毛茧仄分允［

  return : 心勾井匀凶日 >=0 匹 index.
  心勾井日卅井匀凶日  

  腹绸及午五卞反 mb_finder 毛勾井丹［
  仇木匹腹绸仄化｝    卞reg允月［
 */

static int findregBlankMemBuf(void)
{
    int i;
    for (i = 0; i < mbsize; i++)
    {
        mb_finder++;
        if (mb_finder >= mbsize || mb_finder < 0)
            mb_finder = 0;

        if (mb[mb_finder].use == 0)
        {
            mb[mb_finder].use = 1;
            mb[mb_finder].len = 0;
            mb[mb_finder].next = -1;
            mbuse++;
            return mb_finder;
        }
    }
    return TCPSTRUCT_EMBFULL;
}

/*
  mb 毛荸  允月
  
 */
static int unregMemBuf(int index)
{
    mb[index].use = 0;
    mb[index].next = -1;
    mb[index].len = 0;
    mbuse--;
    return OK;
}

static int findregBlankCon(void)
{
    int i;
    // Nuke changed 0->1
    //for(i=0;i<MAXCONNECTION;i++){
    for (i = 1; i < MAXCONNECTION; i++)
    {
        if (con[i].use == 0)
        {
            con[i].use = 1;
            con[i].fd = -1;

            con[i].mbtop_ri = findregBlankMemBuf();
            if (con[i].mbtop_ri < 0)
            {
                fprintf(stderr, "EMBFULL\n");
                return TCPSTRUCT_EMBFULL;
            }

            con[i].mbtop_wi = findregBlankMemBuf();
            if (con[i].mbtop_wi < 0)
            {
                unregMemBuf(con[i].mbtop_ri);
                fprintf(stderr, "EMBFULL\n");
                return TCPSTRUCT_EMBFULL;
            }
            bzero(&con[i].remoteaddr, sizeof(struct sockaddr_in));
            con[i].closed_by_remote = 0;
            return i;
        }
    }
    return TCPSTRUCT_ECFULL;
}

int tcpstruct_countmbuse(void)
{
    int i, c = 0;
    for (i = 0; i < mbsize; i++)
    {
        if (mb[i].use)
            c++;
    }
    return c;
}

char *getGSName(int i)
{
    return gs[i].name;
}

void checkGSUCheck(char *id)
{
    int i;
    char gname[256];
    if (!id[0])
        return;
    memset(gname, 0, sizeof(gname));
    if (LockNode_getGname((getHash(id) & 0xff), id, gname) <= 0)
    {
        log("게임에서 계정을 찾을 수 없습니다:%x/%s !!\n", getHash(id), id);
        return;
    }
    log("\n");
    for (i = 0; i < MAXCONNECTION; i++)
    {
        if (gs[i].name[0] && strcmp(gs[i].name, gname) == 0)
        {
            log("잠금 해제 확인을 보내십시오[%s] 到 %d.%x/%s 服务器:%d !!\n", id, i, getHash(id), gname, gs[i].fd);
            saacproto_ACUCheck_send(gs[i].fd, id);
            return;
        }
    }
    //	log("Can't find gname:%s sending err !!\n", gname);

    int ret = -1;
    if (!isLocked(id))
    {
        log("메모리 정보를 삭제하십시오: 用户:%x/%s 没有锁定!!\n", getHash(id), id);
    }
    if (DeleteMemLock(getHash(id) & 0xff, id, &ret))
    {
    }
    else
    {
        log("잠금 해제 할 수 없습니다 %x:%s !\n", getHash(id), id);
    }
}

void set_nodelay(int sock)
{
    int flag = 1;
    int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                            (char *)&flag, sizeof(int));
    if (result < 0)
    {
        log("지연을 설정할 수 없습니다.\n");
    }
    else
    {
        log("지연을 설정합니다: fd:%d\n", sock);
    }
}

/*
  允屯化及必□丞扔□田□卞  霜［

  int flag : 1分匀凶日霜耨葭卞反云仁日卅中
  
 */
void gmsvBroadcast(int fd, char *p1, char *p2, char *p3, int flag)
{
    int i, c = 0;

    for (i = 0; i < MAXCONNECTION; i++)
    {
        if ((flag == 1) && (i == fd))
            continue;
        if (gs[i].use && gs[i].name[0])
        {
            saacproto_Broadcast_send(i, p1, p2, p3);
            c++;
        }
    }
    /*
#if 1
    {
        char fn[1000];
        FILE *fp;
        snprintf( fn, sizeof(fn), "%s/broadcast.log" , logdir );
        fp = fopen( fn,"a" );
        if(fp){
            fprintf( fp, "From %s(%d) Id:[%s] Char:[%s] Mesg:[%s]\n",
                     gs[fd].name, fd, p1,p2,p3 );
            fclose(fp);
        }
    }
#endif
*/
}

#ifdef _ANGEL_SUMMON

#define MISSIONFILE "db/missiontable.txt"

static int initMissionTable(void)
{
    FILE *fp;
    char onedata[1024];
    char buf[1024];
    int index = 0;

    memset(missiontable, 0, sizeof(missiontable));
    fp = fopen(MISSIONFILE, "r");
    if (!fp)
    {
        log("\nLoading Elf Summon Error.!!!! \n");
        //return false;
    }
    log("\n마법사 소환을로드합니다...");
    while (1)
    {
        //
        if (fgets(onedata, sizeof(onedata), fp) == NULL)
            break;
        if (onedata[0] == '\0' || onedata[0] == '#')
            continue;
        //easyGetTokenFromBuf( onedata, ",", 1, buf, sizeof( buf));
        //index = atoi( buf);
        easyGetTokenFromBuf(onedata, ",", 1, buf, sizeof(buf));
        if (buf[0] == '\0')
            continue;
        strcpy(missiontable[index].angelinfo, buf);
        easyGetTokenFromBuf(onedata, ",", 2, buf, sizeof(buf));
        if (buf[0] == '\0')
            continue;
        strcpy(missiontable[index].heroinfo, buf);
        easyGetTokenFromBuf(onedata, ",", 3, buf, sizeof(buf));
        if (buf[0] == '\0')
            continue;
        missiontable[index].mission = atoi(buf);
        easyGetTokenFromBuf(onedata, ",", 4, buf, sizeof(buf));
        if (buf[0] == '\0')
            continue;
        missiontable[index].flag = atoi(buf);
        easyGetTokenFromBuf(onedata, ",", 5, buf, sizeof(buf));
        if (buf[0] == '\0')
            continue;
        missiontable[index].time = atoi(buf);
        easyGetTokenFromBuf(onedata, ",", 6, buf, sizeof(buf));
        if (buf[0] == '\0')
            continue;
        missiontable[index].limittime = atoi(buf);

        log("%d=%s,%s,%d,%d,%d,%d \n", index,
            missiontable[index].angelinfo,
            missiontable[index].heroinfo,
            missiontable[index].mission,
            missiontable[index].flag,
            missiontable[index].time,
            missiontable[index].limittime);

        index++;
        if (index >= MAXMISSIONTABLE)
            break;
    }
    fclose(fp);
    log("..성공! \n");
    return TRUE;
}

int saveMissionTable(void)
{
    FILE *fp;
    char onedata[1024];
    int index = 0;

    fp = fopen(MISSIONFILE, "w");
    if (!fp)
    {
        log("\n엘프 소환 실수를 엽니 다 !!!! \n");
        //return false;
    }
    log("\n마법사 소환을 저장 ...");
    for (index = 0; index < MAXMISSIONTABLE; index++)
    {

        if (missiontable[index].angelinfo[0] == '\0')
            continue;
        sprintf(onedata, "%s,%s,%d,%d,%d,%d\n",
                missiontable[index].angelinfo,
                missiontable[index].heroinfo,
                missiontable[index].mission,
                missiontable[index].flag,
                missiontable[index].time,
                missiontable[index].limittime);
        fputs(onedata, fp);
    }
    fclose(fp);
    log("..成功! \n");
    return TRUE;
}

void delMissionTableOnedata(int index)
{
    int gi;

    log("\n삭제 마법사:%d:%s:%s \n", index, missiontable[index].angelinfo, missiontable[index].heroinfo);

    if (index < 0 || index >= MAXMISSIONTABLE)
        return;
    strcpy(missiontable[index].angelinfo, "");
    strcpy(missiontable[index].heroinfo, "");
    missiontable[index].mission = 0;
    missiontable[index].flag = MISSION_NONE;
    missiontable[index].time = 0;
    missiontable[index].limittime = 0;

    for (gi = 0; gi < MAXCONNECTION; gi++)
    {
        if (gs[gi].use && gs[gi].name[0])
        {
            saacproto_ACMissionTable_send(gi, index, 3, "", "");
        }
    }
}

#define ANSWERTIME 1 // 等待回答时间(小时)
//#define DOINGTIME 3*24 // 任务时间(小时)
#define BOUNDSTIME 1 * 24 // 保留时间(小时)

void checkMissionTimelimit(void)
{
    int index;
    static time_t lastcheck = 0;

    if (sys_time < lastcheck + 5 * 60)
        return;

    log("\nELF Summon 제한 시간 확인:%d \n", (int)sys_time);
    for (index = 0; index < MAXMISSIONTABLE; index++)
    {
        if (missiontable[index].flag == MISSION_NONE)
        {
            continue;
        }
        // 等待使者回应1小时
        else if (missiontable[index].flag == MISSION_WAIT_ANSWER && sys_time > missiontable[index].time + ANSWERTIME * 60 * 60)
        {

            delMissionTableOnedata(index); // 删
        }
        // 等待领奖完成 limittime小时
        else if ((missiontable[index].flag == MISSION_DOING || missiontable[index].flag == MISSION_HERO_COMPLETE) && (sys_time > (missiontable[index].time + missiontable[index].limittime * 60 * 60)))
        {

            char buf[1024];
            int gi;
            // 改TIMEOVER
            log("엘프 소환 및 수상 수상자:%d ", index);
            missiontable[index].flag = MISSION_TIMEOVER;
            missiontable[index].time = time(NULL);
            missiontable[index].limittime = BOUNDSTIME;

            sprintf(buf, "%d|%s|%s|%d|%d|%d|%d ", index,
                    missiontable[index].angelinfo,
                    missiontable[index].heroinfo,
                    missiontable[index].mission,
                    missiontable[index].flag,
                    missiontable[index].time,
                    missiontable[index].limittime);
            for (gi = 0; gi < MAXCONNECTION; gi++)
            {
                if (gs[gi].use && gs[gi].name[0])
                {
                    saacproto_ACMissionTable_send(gi, 1, 1, buf, "");
                }
            }

            continue;
        }
        //else if( missiontable[index].flag == MISSION_HERO_COMPLETE
        //		&& sys_time > missiontable[index].time + BOUNDSTIME*60*60 ) {
        //	log(" 领奖时间过:%d ", index);
        //	delMissionTableOnedata( index);// 删
        //}

        // 资料保留时间(BOUNDSTIME小时)
        else if (missiontable[index].flag == MISSION_TIMEOVER && sys_time > missiontable[index].time + BOUNDSTIME * 60 * 60)
        {
            log("시간을 지키십시오:%d ", index);
            delMissionTableOnedata(index); // 删
        }
    }
    saveMissionTable();

    lastcheck = sys_time;
}

#endif

void logerr(char *token)
{
    char tmp[256];
    struct tm now;
    time_t timep;
    time(&timep);
    memcpy(&now, localtime(&timep), sizeof(now));

    sprintf(tmp, "%04d-%02d-%02d.log", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);

    FILE *fp = fopen(tmp, "a+");
    fwrite(token, strlen(token), 1, fp);
    fclose(fp);
    printf(token);
}
