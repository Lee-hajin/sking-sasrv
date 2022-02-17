#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "version.h"
#include "NewBilu/version.h"
#include "common.h"
#include "util.h"

enum
{
    E_T_SIZE_NORMAL,
    E_T_SIZE_BIG,
};

typedef enum
{
    E_T_TEMPNO,       //编号
    E_T_INITNUM,      //初始能力
    E_T_LVUPPOINT,    //成长率
    E_T_BASEVITAL,    //血基数
    E_T_BASESTR,      //攻基数
    E_T_BASETGH,      //防基数
    E_T_BASEDEX,      //敏基数
    E_T_MODAI,        //AI模式
    E_T_GET,          //捕获率
    E_T_EARTHAT,      //地属性
    E_T_WATERAT,      //水属性
    E_T_FIREAT,       //火属性
    E_T_WINDAT,       //风属性
    E_T_POISON,       //猛毒抗性
    E_T_PARALYSIS,    //麻痹抗性
    E_T_SLEEP,        //睡眠抗性
    E_T_STONE,        //石化抗性
    E_T_DRUNK,        //酒醉抗性
    E_T_CONFUSION,    //混乱抗性
    E_T_PETSKILL1,    //技能1
    E_T_PETSKILL2,    //技能2
    E_T_PETSKILL3,    //技能3
    E_T_PETSKILL4,    //技能4
    E_T_PETSKILL5,    //技能5
    E_T_PETSKILL6,    //技能6
    E_T_PETSKILL7,    //技能7
    E_T_RARE,         //珍稀度
    E_T_CRITICAL,     //会心率
    E_T_COUNTER,      //反击率
    E_T_SLOT,         //技能格
    E_T_IMGNUMBER,    //图形编号
    E_T_PETFLG,       //无用参数
    E_T_SIZE,         //体形大小
    E_T_ATOMBASEADD1, //素材1修正度
    E_T_ATOMFIXMIN1,  //素材1成分min
    E_T_ATOMFIXMAX1,  //素材1成分max
    E_T_ATOMBASEADD2, //素材2修正度
    E_T_ATOMFIXMIN2,  //素材2成分min
    E_T_ATOMFIXMAX2,  //素材2成分max
    E_T_ATOMBASEADD3, //素材3修正度
    E_T_ATOMFIXMIN3,  //素材3成分min
    E_T_ATOMFIXMAX3,  //素材3成分max
    E_T_ATOMBASEADD4, //素材4修正度
    E_T_ATOMFIXMIN4,  //素材4成分min
    E_T_ATOMFIXMAX4,  //素材4成分max
    E_T_ATOMBASEADD5, //素材5修正度
    E_T_ATOMFIXMIN5,  //素材5成分min
    E_T_ATOMFIXMAX5,  //素材5成分max
    E_T_LIMITLEVEL,   //级限
#ifdef _PET_FUSION
    E_T_FUSIONCODE, //融合码
#endif
    E_T_DATAINTNUM,
} ENEMYTEMP_DATAINT;

typedef enum
{
    E_T_NAME,         //宠物名字
    E_T_ATOMFIXNAME1, //素材1名字
    E_T_ATOMFIXNAME2, //素材2名字
    E_T_ATOMFIXNAME3, //素材3名字
    E_T_ATOMFIXNAME4, //素材4名字
    E_T_ATOMFIXNAME5, //素材5名字
    E_T_DATACHARNUM,

} ENEMYTEMP_DATACHAR;

typedef enum
{
    ENEMY_ID,           //宠编号
    ENEMY_TEMPNO,       //enemybase编号
    ENEMY_LV_MIN,       //最小等级
    ENEMY_LV_MAX,       //最大等级
    ENEMY_CREATEMAXNUM, //创建数量max
    ENEMY_CREATEMINNUM, //创建数量min
    ENEMY_TACTICS,      //战斗模式
    ENEMY_EXP,          //怪物经验
    ENEMY_DUELPOINT,    //怪物DP
    ENEMY_STYLE,        //掉物旗标
    ENEMY_PETFLG,       //可否捕抓

    ENEMY_ITEM1,      //掉落道具1
    ENEMY_ITEM2,      //掉落道具2
    ENEMY_ITEM3,      //掉落道具3
    ENEMY_ITEM4,      //掉落道具4
    ENEMY_ITEM5,      //掉落道具5
    ENEMY_ITEM6,      //掉落道具6
    ENEMY_ITEM7,      //掉落道具7
    ENEMY_ITEM8,      //掉落道具8
    ENEMY_ITEM9,      //掉落道具9
    ENEMY_ITEM10,     //掉落道具10
    ENEMY_ITEMPROB1,  //掉落几率1
    ENEMY_ITEMPROB2,  //掉落几率2
    ENEMY_ITEMPROB3,  //掉落几率3
    ENEMY_ITEMPROB4,  //掉落几率4
    ENEMY_ITEMPROB5,  //掉落几率5
    ENEMY_ITEMPROB6,  //掉落几率6
    ENEMY_ITEMPROB7,  //掉落几率7
    ENEMY_ITEMPROB8,  //掉落几率8
    ENEMY_ITEMPROB9,  //掉落几率9
    ENEMY_ITEMPROB10, //掉落几率10

    ENEMY_DATAINTNUM,

} ENEMY_DATAINT;

typedef enum
{
    ENEMY_NAME,          //名字
    ENEMY_TACTICSOPTION, //战斗策略
#ifdef _BATTLENPC_WARP_PLAYER
    ENEMY_ACT_CONDITION, //逃跑策略
#endif
    ENEMY_DATACHARNUM,
} ENEMY_DATACHAR;

typedef enum
{
    GROUP_ID,
    GROUP_APPEARBYITEMID,    /* 仇及失奶  丞毛  匀化中凶日请蜇允月 -1 反  骰*/
    GROUP_NOTAPPEARBYITEMID, /* 仇及失奶  丞毛  匀化中凶日请蜇仄卅中 -1 反  骰*/
    ENEMY_ID1,
    ENEMY_ID2,
    ENEMY_ID3,
    ENEMY_ID4,
    ENEMY_ID5,
    ENEMY_ID6,
    ENEMY_ID7,
    ENEMY_ID8,
    ENEMY_ID9,
    ENEMY_ID10,
    CREATEPROB1,
    CREATEPROB2,
    CREATEPROB3,
    CREATEPROB4,
    CREATEPROB5,
    CREATEPROB6,
    CREATEPROB7,
    CREATEPROB8,
    CREATEPROB9,
    CREATEPROB10,
    GROUP_DATAINTNUM,
} GROUP_DATAINT;

typedef enum
{
    GROUP_NAME,
    GROUP_DATACHARNUM,

} GROUP_DATACHAR;

typedef struct tagENEMY_EnemyTable
{
    int intdata[ENEMY_DATAINTNUM];
    STRING64 chardata[ENEMY_DATACHARNUM];
    int enemytemparray;
} ENEMY_EnemyTable;

typedef struct tagENEMYTEMP_Table
{
    int intdata[E_T_DATAINTNUM];
    //ANDY_EDIT
    STRING64 chardata[E_T_DATACHARNUM];

} ENEMYTEMP_Table;

typedef struct tagGROUP_Table
{
    int intdata[GROUP_DATAINTNUM];
    STRING32 chardata[GROUP_DATACHARNUM];
    int enemyarray[CREATEPROB1 - ENEMY_ID1];
} GROUP_Table;

BOOL ENEMY_CHECKINDEX(int index);
int ENEMY_setInt(int index, ENEMY_DATAINT element, int data);
int ENEMY_getInt(int index, ENEMY_DATAINT element);

int *ENEMY_getIntdata(int index);

BOOL ENEMY_setChar(int index, ENEMY_DATACHAR element, char *new);
char *ENEMY_getChar(int index, ENEMY_DATACHAR element);
int ENEMY_getEnemyNum(void);
BOOL ENEMY_initEnemy(char *filename);
BOOL ENEMY_reinitEnemy(void);
int ENEMY_createEnemy(int array, int baselevel);
int *ENEMY_getEnemy(int charaindex, int x, int y);
int ENEMY_createPetFromEnemyIndex(int charaindex, int array);
int ENEMY_createPet(int array, int vital, int str, int tgh, int dex);
int ENEMY_createPet2(int array, int vital, int str, int tgh, int dex);
int ENEMY_getEnemyArrayFromId(int EnemyId);
int ENEMY_getEnemyArrayFromTempNo(int EnemyTempNo);
int ENEMY_getEnemyIdFromTempNo(int EnemyTempNo);
int ENEMY_getEnemyTempNoFromId(int EnemyId);

#ifdef _TEST_DROPITEMS
int ENEMY_TEST_createPetIndex(int array);
#endif

int ENEMYTEMP_getEnemyNum(void); //krynn 2001/12/13
BOOL ENEMYTEMP_CHECKINDEX(int index);
int ENEMYTEMP_setInt(int index, ENEMYTEMP_DATAINT element, int data);
int ENEMYTEMP_getInt(int index, ENEMYTEMP_DATAINT element);
char *ENEMYTEMP_getChar(int index, ENEMYTEMP_DATACHAR element);
BOOL ENEMYTEMP_getInt_setChar(int index, ENEMYTEMP_DATACHAR element, char *new);
char *ENEMYTEMP_getInt_getChar(int index, ENEMYTEMP_DATACHAR element);
int ENEMYTEMP_getInt_getEnemyNum(void);
BOOL ENEMYTEMP_getInt_initEnemy(char *filename);
BOOL ENEMYTEMP_getInt_reinitEnemy(void);
int ENEMYTEMP_getEnemyTempArray(int enemyindex);
int ENEMYTEMP_getEnemyTempArrayFromTempNo(int EnemyTempNo);
int ENEMYTEMP_getEnemyTempArrayFromInitnum(int EnemyTempNo);

int GROUP_setInt(int index, GROUP_DATAINT element, int data);
int GROUP_getInt(int index, GROUP_DATAINT element);
BOOL GROUP_setChar(int index, GROUP_DATACHAR element, char *new);
char *GROUP_getChar(int index, GROUP_DATACHAR element);
int GROUP_getEnemyNum(void);

BOOL ENEMYTEMP_initEnemy(char *filename);
BOOL ENEMYTEMP_reinitEnemy(void);
BOOL GROUP_initGroup(char *filename);
BOOL GROUP_reinitGroup(void);

#ifdef _PET_EVOLUTION
int EVOLUTION_createPetFromEnemyIndex(int charaindex, int baseindex, int flg);
int PET_CheckIncubate(int charaindex);
BOOL PETFUSION_getIndexForChar(int toindex, int *MainIndex, int *Subindex1, int *Subindex2, char *data);
#ifdef _PET_FUSION_2
int NPC_getFusionTableForBase(int charaindex, int petindex1, int petindex2, int type);
#else
int NPC_getFusionTableForBase(int charaindex, int petindex1, int petindex2);
#endif
int NPC_getPetArrayForNo(int PetCode);
BOOL PET_getEvolutionAns(int petindex, int *base);
#endif

#ifdef _PET_FUSION_2
int PETFUSION_SetNewEgg(int toindex, int petindex, int array, int *work, int *skill1, int *skill2, int fusion_type);
#else
int PETFUSION_SetNewEgg(int toindex, int petindex, int array, int *work, int *skill1, int *skill2);
#endif
BOOL PETFUSION_AddEgg(int toindex, int petID, int PetCode);

#ifdef _PET_TRANS
int GetNewPet(int toindex, int petindex, int array, int *work);
#endif
int PETFUSION_LuaSetNewEgg(int toindex, int petindex, int petcode, int petwork);

#endif
