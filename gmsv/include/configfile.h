#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

#include "version.h"
#include "NewBilu/version.h"
#include "common.h"

#undef EXTERN
#ifdef __CONFIGFILE_C__
#define EXTERN
#else /*  __CONFIGFILE_C__  */
#define EXTERN extern
#endif /*  __CONFIGFILE_C__  */

// Arminius 7.12 login announce
extern char announcetext[8192];
void AnnounceToPlayer(int charaindex);
// Robin 0720
void AnnounceToPlayerWN(int fd);
void LoadAnnounce(void);

#ifdef _PET_TALKPRO
#define PETTALK_MAXID 8
typedef struct {
  int ID;
  char DATA[1024 * 64]; // 1M
} PTALK;
extern PTALK pettalktext[PETTALK_MAXID];
void LoadPetTalk(void);
#else

extern char pettalktext[4096];
void LoadPetTalk(void);
#endif

#ifdef _GAMBLE_BANK
#define GAMBLEBANK_ITEMSMAX 100
#define DEFEND_ITEMSMAX 40
typedef struct REGAMBLEBANKITEMS {
  char name[128];
  int Gnum;
  int ItemId;
  int type;
} GAMBLEBANK_ITEMS;

extern GAMBLEBANK_ITEMS GB_ITEMS[GAMBLEBANK_ITEMSMAX];

void Load_GambleBankItems(void);
#endif

#ifdef _CFREE_petskill
#define PETSKILL_CODE 500
typedef struct REPETSKILL_CODES {
  char name[128];
  int TempNo;
  int PetId;
  char Code[256];
} PETSKILL_CODES;

extern PETSKILL_CODES Code_skill[PETSKILL_CODE];
void Load_PetSkillCodes(void);
#endif

#ifdef _GMRELOAD
#define GMMAXNUM 100
struct GMINFO {
  char cdkey[24];
  int level;
};
extern struct GMINFO gminfo[GMMAXNUM];
#endif

BOOL luareadconfigfile(char *data);

BOOL readconfigfile(char *filename);
BOOL readDesKey(char *filename);
BOOL readDesRand(char *filename);

void defaultConfig(char *argv0);
char *getProgname(void);
char *getConfigfilename(void);
void setConfigfilename(char *newv);

unsigned int getDebuglevel(void);
unsigned int setDebuglevel(unsigned int newv);
unsigned int getMemoryunit(void);
unsigned int getMemoryunitnum(void);
char *getAccountservername(void);
unsigned short getAccountserverport(void);
char *getAccountserverpasswd(void);
char *getGameservername(void);

unsigned short getPortnumber(void);
int getBattleSpeed();
int getServernumber(void);
int getReuseaddr(void);
int getNodelay(void);
int getLogWriteTime(void);
int getLogIOTime(void);
int getLogGameTime(void);
int getLogNetloopFaster(void);
int getSaacwritenum(void);
void setSaacwritenum(int num);
int getSaacreadnum(void);
void setSaacreadnum(int num);
#ifdef _PETUP_GET_EXP
int getPetUpGetExp(void);
#endif
unsigned int getFdnum(void);
unsigned int setFdnum(int temp_fdnum);
unsigned int getPlayercharnum(void);
unsigned int getPetcharnum(void);
unsigned int getOtherscharnum(void);
unsigned int getObjnum(void);
unsigned int getItemnum(void);
unsigned int getBattlenum(void);

char *getTopdir(void);
char *getMapdir(void);
char *getMaptilefile(void);
char *getBattleMapfile(void);
char *getItemfile(void);
char *getInvfile(void);
char *getAppearfile(void);
char *getEffectfile(void);
char *getTitleNamefile(void);
char *getTitleConfigfile(void);
char *getLsgenlogfilename(void);
char *getStoredir(void);
#ifdef _STORECHAR
char *getStorechar(void);
#endif
char *getNpcdir(void);
char *getLogdir(void);
char *getLogconffile(void);
char *getChatMagicPasswd(void);
unsigned int getChatMagicCDKeyCheck(void);
void setChatMagicPasswd(void);
void setChatMagicCDKeyCheck(void);

unsigned int getFilesearchnum(void);
unsigned int getNpctemplatenum(void);
unsigned int getNpccreatenum(void);
unsigned int getWalksendinterval(void);
void setWalksendinterval(unsigned int);
unsigned int getCAsendinterval_ms(void);
void setCAsendinterval_ms(unsigned int);
unsigned int getCDsendinterval_ms(void);
void setCDsendinterval_ms(unsigned int);
unsigned int getOnelooptime_ms(void);
void setOnelooptime_ms(unsigned int);
unsigned int getCharSavesendinterval(void);
void setCharSavesendinterval(unsigned int interval);
unsigned int getAddressbookoffmsgnum(void);
unsigned int getProtocolreadfrequency(void);
unsigned int getAllowerrornum(void);
unsigned int getLogHour(void);
unsigned int getBattleDebugMsg(void);
void setBattleDebugMsg(unsigned int);

char *getEncountfile(void);
char *getEnemyfile(void);
char *getGroupfile(void);
char *getEnemyBasefile(void);
char *getMagicfile(void);
#ifdef _ATTACK_MAGIC

char *getAttMagicfileName(void);

#endif

char *getPetskillfile(void);
char *getItematomfile(void);
char *getQuizfile(void);

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
char *getProfession(void);
#endif

unsigned int getPetdeletetime(void);
void setPetdeletetime(unsigned int interval);

unsigned int getItemdeletetime(void);
unsigned int getFunctionfile(void); //获得函数

void setItemdeletetime(unsigned int interval);
// ttom add this because the second have this function
// unsigned int getAcwriteSize( void );

#ifdef _DEL_DROP_GOLD
unsigned int getGolddeletetime(void);
void setGolddeletetime(unsigned int interval);
#endif

unsigned int setEncodeKey(void);
unsigned int setAcWBSize(void);
// ttom end

// CoolFish: +2 2001/4/18
unsigned int getAcwriteSize(void);
unsigned int getErrUserDownFlg(void);

// Arminius 7.24 manor pk
char *getGameserverID(void);
char *getGameserverListID(void);
unsigned short getAllowManorPK(void);

// Terry 2001/10/03 service ap
char *getApID(void);
unsigned short getApPort(void);
int getLoopTime(void);
int getEnableService(void);

#ifdef _ANGEL_SUMMON
BOOL LoadMissionList(void);
#endif
#ifdef _JOBDAILY
BOOL LoadJobdailyfile(void);
#endif
#ifdef _GMRELOAD
char *getGMSetfile(void);
BOOL LoadGMSet(char *filename);
#endif

#ifdef _ITEM_QUITPARTY
char *getitemquitparty(void);
#endif

#ifdef _UNLAW_WARP_FLOOR
int getUnlawwarpfloor(unsigned int index);
#endif
#ifdef _NO_JOIN_FLOOR
int getNoJoinFloor(unsigned int index);
#endif
#ifdef _USER_EXP_CF
BOOL LoadEXP(char *filename);
char *getEXPfile(void);
int getNeedLevelUpTbls(int level);
#endif

#ifdef _UNREG_NEMA
char *getUnregname(int index);
#endif

#ifdef _TRANS_LEVEL_CF
int getChartrans(void);
int getPettrans(void);
int getYBLevel(void);
int getMaxLevel(void);
#endif

#ifdef _GET_BATTLE_EXP
unsigned int getBattleexp(void);
#endif

#ifdef _PLAYERMAXPOINT
char *getPoint(void);
int getTransPoint(int index);
#endif

#ifdef _PET_AND_ITEM_UP
char *getPetup(void);
char *getItemup(void);
#endif
#ifdef _WATCH_FLOOR
int getWatchFloor(unsigned int index);
char *getWatchFloorCF(void);
#endif

#ifdef _BATTLE_FLOOR
int getBattleFloor(unsigned int index);
char *getBattleFloorCF(void);
#endif

#ifdef _SKILLUPPOINT_CF
int getSkup(void);
#endif
#ifdef _RIDELEVEL
int getRideLevel(void);
int getRideTrans(void);
#endif
#ifdef _REVLEVEL
char *getRevLevel(void);
#endif
#ifdef _FIX_CHARLOOPS
int getCharloops(void);
#endif
#ifdef _PLAYER_MOVE
int getPMove(void);
#endif

int getrecvbuffer(void);
int getsendbuffer(void);
int getrecvlowatbuffer(void);
int getrunlevel(void);

#ifdef _PLAYER_NUM
int getPlayerNum(void);
void setPlayerNum(int num);
#endif
#ifdef _BATTLE_GOLD
int getBattleGold(void);
#endif

#ifdef _ANGEL_TIME
int getAngelPlayerTime(void);
int getAngelPlayerMun(void);
#endif

#ifdef _RIDEMODE_20
int getRideMode(void);
#endif

#ifdef _FM_POINT_PK
char *getFmPointPK(void);
#endif

#ifdef _ENEMY_ACTION
int getEnemyAction(void);
#endif

#ifdef _FUSIONBEIT_TRANS
int getFusionbeitTrans(void);
#endif

char *getMacCheck(void);
int getCpuUse(void);

#ifdef _CHECK_PEPEAT
int getCheckRepeat(void);
#endif

#ifdef _FM_JOINLIMIT
int getJoinFamilyTime(void);
#endif

#ifdef _THE_WORLD_SEND
int getTheWorldTrans();
int getTheWorldLevel();
int getTheWorldSend();
#endif

#ifdef _LOGIN_DISPLAY
int getLoginDisplay();
#endif

#ifdef _VIP_POINT_PK
int getVipPointPK(int index);
float getVipPointPKCost(void);
#endif

#ifdef _AUTO_DEL_PET
#define AUTODELPETNUM 30
int getAutoDelPet(int index);
#endif

#ifdef _AUTO_DEL_ITEM
#define AUTODELITEMNUM 30
int getAutoDelItem(int index);
#endif

#ifdef _UNLAW_THIS_LOGOUT
int getUnlawThisLogout(int index);
#endif
#ifdef _TRANS_POINT_UP
int getTransPoinUP(int index);
#endif

#ifdef _NO_STW_ENEMY
int getNoSTWNenemy(void);
int getNoSTWNenemyPoint(void);
#endif
#ifdef _ITEM_PET_LOCKED
int getItemPetLocked(void);
#endif

#ifdef _PET_TRANS_ABILITY
int getPetTransAbility();
int getPetTransAbility1();
int getPetTransAbility2();
#endif
#ifdef _NEED_ITEM_ENEMY
int getDelNeedItem();
#endif
#ifdef _PLAYER_OVERLAP_PK
int getPlayerOverlapPk(int index);
#endif
#ifdef _FIMALY_PK_TIME
int getFimalyPkTime();
#endif

#ifdef _CANCEL_ANGLE_TRANS
int getCancelAngleTrans();
#endif

#ifdef _NO_HELP_MAP
int getNoHelpMap(int index);
#endif

#ifdef _BATTLE_TIME
int getBattleTime();
#endif
#ifdef _STREET_VENDOR_TRANS
int getStreetVendorTrans();
#endif
#ifdef _CHECK_SEVER_IP
int checkServerIp(unsigned int ip);
int checkServerIpChar(char *ip);
#endif

#endif

#ifdef _NO_ENEMYID_GROUP
int checkNoEnemyIdGroup(int enemyid);
#endif

#ifdef _NEW_MAP_NO_FAME
int getNoFameMap(void);
#endif

#ifdef _TALK_AREA_CF
int getTalkMin(void);
int getTalkMax(void);
#endif

int getNoSellPetId(int index);
#ifdef _BATTLE_RAND_DEX
int getBattleRandDex(void);
#endif

#ifdef FAMLYNUM_CF
int getfamlynum(void);
#endif

#ifdef _NB_ALLOW_FMPK
int getAllowFMPK(int index);
char *getFMPKNoMsg(void);
void initAllowFMPK(void);
#endif

#ifdef _NB_CF_SHARE_EXP
int getExpShare(void);
#endif

#ifdef _NB_SILENT_SWITCH
int getsilentswitch(void);
#endif

#ifdef _NB_MAGICNOEXP
int getMagicNoExp1(void);
int getMagicNoExp2(void);
#endif

#ifdef _NB_DragonFunsionId
int getDragonFunsionRand(void);
void initDragonFunsionId(void);
int getDragonFunsionId(int index);
int getDragonTrans(void);
#endif
