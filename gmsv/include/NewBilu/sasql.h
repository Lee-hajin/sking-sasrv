#ifndef _SASQL_H_
#define _SASQL_H_

#include "version.h"
#ifdef _SASQL

#define BOOL int
#define FALSE 0
#define TRUE 1

void sasql_close(void);
BOOL sasql_init(void);

int sasql_query(char *nm, char *pas);
BOOL sasql_online(char *ID, char *NM, char *IP, char *MAC1, char *MAC2, char *MAC3, int flag);
int sasql_queryonline(char *ID);
BOOL sasql_register(char *id, char *ps);
BOOL sasql_chehk_lock(char *idip);
BOOL sasql_add_lock(char *idip);
BOOL sasql_del_lock(char *idip);
#ifdef _ITEM_PET_LOCKED
char *sasql_ItemPetLocked(char *id, char *safepasswd);
char *sasql_ItemPetLocked_Passwd(char *id, char *safepasswd);
int sasql_ItemPetLocked_Char(char *id, char *safepasswd);
#endif
int sasql_getVipPoint(int charaindex);
int sasql_setVipPoint(int charaindex, int point);
int sasql_getVipPointForCdkey(char *id);
int sasql_setVipPointForCdkey(char *id, int point);
BOOL sasql_addPauctionInfo(char *cdkey, char *name, char *effect, int cost, int type, char *info, char *string);
int sasql_getPauctionList(char *List, int start, int type, char *cdkey);
BOOL sasql_getPauctionSurvey(char *token, int id);
int sasql_getMyPauction(char *token, int id, char *cdkey);
BOOL sasql_delPauctionBuy(int id, char *cdkey);
int sasql_PauctionOK(int id, char *cdkey);

int sasql_getPayPoint(int charaindex);
int sasql_setPayPoint(int charaindex, int point);
int sasql_getPayPointForCdkey(char *id);
int sasql_setPayPointForCdkey(char *id, int point);

int sasql_getRmbPoint(int charaindex);
int sasql_setRmbPoint(int charaindex, int point);
int sasql_getRmbPointForCdkey(char *id);
int sasql_setRmbPointForCdkey(char *id, int point);
void sasql_updateCharLockMac(int charaindex, char *cdkey);
void sasql_updateFmPointData(int fmid, int fmtime, int fmnum, int flg);
int sasql_queryFmPointData(int fmid, int flg);
enum
{
	INSERT,
	SELECT,
	UPDATE,
	DELETE,
};

int sasql_getDateNum(char *tablename);
#ifdef _NB_SQL_Enemy
char *sasql_getEnemyData(int limit);
#endif
#ifdef _NB_SQL_EnemyBase
char *sasql_getEnemybaseData(int limit);
#endif

#ifdef _NB_CDKEYLOCK
int sasql_getLock(char *cdkey);
int sasql_setLock(char *cdkey, int Lock);
#endif

void SQL_UpdatePetRank(int petindex);
void SQL_DeletePetInRank(int petindex);
char *SQL_GetTopPetRank(int enemybaseid);

#endif
#endif
