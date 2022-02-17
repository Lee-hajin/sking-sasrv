
#ifndef __ITEM_H__
#define __ITEM_H__

#include "char_base.h"
#include "common.h"
#include "util.h"
#include "version.h"
#include "NewBilu/version.h"
#ifdef _ALLBLUES_LUA
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#endif

#define NULLITEM "0"

#ifdef _SIMPLIFY_ITEMSTRING
typedef struct ITEM_tag_intDataSetting
{
	char *dumpchar;
	int defaults;
	int table;
	char *string;
} ITEM_intDataSetting;

typedef struct ITEM_tag_charDataSetting
{
	char *dumpchar;
	char *defaults;
	int table;
	char *string;
} ITEM_charDataSetting;
#endif

typedef enum
{
	ITEM_FIST = 0,	   //爪
	ITEM_AXE,		   //1斧
	ITEM_CLUB,		   //2棍
	ITEM_SPEAR,		   //3枪
	ITEM_BOW,		   //4弓
	ITEM_SHIELD,	   //5盾牌
	ITEM_HELM,		   //6头盔/帽子
	ITEM_ARMOUR,	   //7衣服/铠甲
	ITEM_BRACELET = 8, //8手环
	ITEM_MUSIC,		   //9乐器
	ITEM_NECKLACE,	   //10项链
	ITEM_RING,		   //11环?
	ITEM_BELT,		   //12带?
	ITEM_EARRING,	   //13耳环
	ITEM_NOSERING,	   //14鼻环
	ITEM_AMULET,	   //15护身符
	ITEM_OTHER = 16,   //16物品
	ITEM_BOOMERANG,	   //17 回旋标
	ITEM_BOUNDTHROW,   // 18投掷斧头
	ITEM_BREAKTHROW,   // 19投掷石
	ITEM_DISH = 20,	   //20物品
#ifdef _ITEM_INSLAY
	ITEM_METAL, //21金属
	ITEM_JEWEL, //22宝石
#endif
#ifdef _ITEM_CHECKWARES
	ITEM_WARES, //23货物
#endif
#ifdef _ITEM_EQUITSPACE
	ITEM_WBELT,	  //24腰带
	ITEM_WSHIELD, //25盾
	ITEM_WSHOES,  //26鞋子
#endif
#ifdef _EQUIT_NEWGLOVE
	ITEM_WGLOVE, //27手套
#endif
#ifdef _ALCHEMIST
	ITEM_ALCHEMIST = 28, //炼金
#endif
#ifdef _PET_ITEM
	ITEM_PET_HEAD,	 //29 宠物头套
	ITEM_PET_TOOTH,	 //30 宠物牙套
	ITEM_PET_CLAW,	 //31 宠物爪套
	ITEM_PET_BREAST, //32 宠物胸甲
	ITEM_PET_BACK,	 //33 宠物背甲
	ITEM_PET_WING,	 //34 宠物翅膀
	ITEM_PET_FEET,	 //35 宠物脚
#endif
#ifdef _ANGEL_SUMMON
//ITEM_ANGELTOKEN,
//ITEM_HEROTOKEN,
#endif
	ITEM_CATEGORYNUM,
} ITEM_CATEGORY;

typedef enum
{
	ITEM_FIELD_ALL,
	ITEM_FIELD_BATTLE,
	ITEM_FIELD_MAP,
} ITEM_FIELDTYPE;

typedef enum
{
	ITEM_TARGET_MYSELF,
	ITEM_TARGET_OTHER,
	ITEM_TARGET_ALLMYSIDE,	  //我方全体
	ITEM_TARGET_ALLOTHERSIDE, //敌方全体
	ITEM_TARGET_ALL,		  //全体
} ITEM_TARGETTYPE;

typedef enum
{
	ITEM_ID,			  //道具编号
	ITEM_BASEIMAGENUMBER, //道具图像
	ITEM_COST,			  //道具价格
	ITEM_TYPE,			  //道具类型
	ITEM_ABLEUSEFIELD,	  //无效参数
	ITEM_TARGET,		  //作用范围(0我方单体,1全场单个,2我方全体,3敌方全体,4全体)
	ITEM_LEVEL,			  //物品可使用等级
#ifdef _ITEM_MAXUSERNUM
	ITEM_DAMAGEBREAK, //物品使用次数
#endif

#ifdef _ITEMSET4_TXT
	ITEM_USEPILENUMS, //物品堆叠次数
	ITEM_CANBEPILE,	  //是否可堆叠

	ITEM_NEEDSTR,		 //物品装备需要攻击
	ITEM_NEEDDEX,		 //物品装备需要敏捷
	ITEM_NEEDTRANS,		 //物品装备需要转生
	ITEM_NEEDPROFESSION, //物品装备需要职业
#endif

	ITEM_DAMAGECRUSHE,	  //最小耐久度
	ITEM_MAXDAMAGECRUSHE, //最大耐久度

#ifdef _ADD_DEAMGEDEFC
	ITEM_OTHERDAMAGE, //伤害增加
	ITEM_OTHERDEFC,	  //吸收增加
#endif
#ifdef _SUIT_ITEM
	ITEM_SUITCODE, //套装编号
#endif
	ITEM_ATTACKNUM_MIN,		//攻击加成min
	ITEM_ATTACKNUM_MAX,		//攻击加成max
	ITEM_MODIFYATTACK,		//攻
	ITEM_MODIFYDEFENCE,		//防
	ITEM_MODIFYQUICK,		//敏
	ITEM_MODIFYHP,			//HP
	ITEM_MODIFYMP,			//MP
	ITEM_MODIFYLUCK,		//运气加成
	ITEM_MODIFYCHARM,		//魅力加成
	ITEM_MODIFYAVOID,		//回避加成
	ITEM_MODIFYATTRIB,		//装备属性(1地，2水，3火，4风)
	ITEM_MODIFYATTRIBVALUE, //属性加成
	ITEM_MAGICID,			//精灵编号
	ITEM_MAGICPROB,			//精灵命中率(无效)
	ITEM_MAGICUSEMP,		//精灵消耗MP
#ifdef _ITEMSET5_TXT
	ITEM_MODIFYARRANGE,	 //格挡率
	ITEM_MODIFYSEQUENCE, //攻击顺序
	ITEM_ATTACHPILE,	 //负重加成
	ITEM_HITRIGHT,		 //命中加成
#endif
#ifdef _ITEMSET6_TXT
	ITEM_NEGLECTGUARD, //无视防御
//	ITEM_BEMERGE,
#endif
	ITEM_POISON,	   //猛毒抗性
	ITEM_PARALYSIS,	   //麻痹抗性
	ITEM_SLEEP,		   //睡眠抗性
	ITEM_STONE,		   //石化抗性
	ITEM_DRUNK,		   //酒醉抗性
	ITEM_CONFUSION,	   //混乱抗性
	ITEM_CRITICAL,	   //会心一击几率
	ITEM_USEACTION,	   //使用后人物做出动作
	ITEM_DROPATLOGOUT, //登出消失(1为登出消失，0为正常)
	ITEM_VANISHATDROP, //丢弃消失(1为丢弃消失，0为正常)
	ITEM_ISOVERED,	   //是否挡路(1为挡路，0为正常)
	ITEM_CANPETMAIL,   //可否邮件(1为可邮寄，0为不可邮寄)
	ITEM_CANMERGEFROM, //可否被合成(1为可，0为不可)
	ITEM_CANMERGETO,   //可否被合成出(1为可，0为不可)
	ITEM_INGVALUE0,	   //材料成分1
	ITEM_INGVALUE1,	   //材料成分2
	ITEM_INGVALUE2,	   //材料成分3
	ITEM_INGVALUE3,	   //材料成分4
	ITEM_INGVALUE4,	   //材料成分15
	ITEM_PUTTIME,	   //物品时间
	ITEM_LEAKLEVEL,	   //强化等级
	ITEM_MERGEFLG,	   //合成标识
	ITEM_CRUSHLEVEL,   //修正记录
	ITEM_VAR1,		   //骰子变量1
	ITEM_VAR2,		   //骰子变量2
	ITEM_VAR3,		   //骰子变量3
	ITEM_VAR4,		   //骰子变量4
#ifdef _ITEM_COLOER
	ITEM_COLOER, //物品名字颜色
#endif
#ifdef _ITEM_USE_TIME
	ITEM_USETIME, //物品时限
#endif
#ifdef _LOCK_PET_ITEM
	ITEM_ItemLock, //绑定标志
#endif
#ifdef _ITEM_TIME_LIMIT
	ITEM_TIMELIMIT, //倒计时
#endif
#ifdef _ITEM_ATTUPDATA
	ITEM_ATTLv,	 //等级
	ITEM_ATTUP1, //血强
	ITEM_ATTUP2, //攻强
	ITEM_ATTUP3, //防强
	ITEM_ATTUP4, //敏强
#endif
#ifdef _ITEM_METAMOGET
	ITEM_FACEDATA,
#endif
#ifdef _ITEM_UPGRADE
	ITEM_UPGRADENO,
	ITEM_UPGRADENO1,
	ITEM_UPGRADENO2,
	ITEM_UPGRADENO3,
	ITEM_UPGRADENO4,
	ITEM_UPGRADENO5,
	ITEM_UPGRADENO6,
#endif
	ITEM_DATAINTNUM,
} ITEM_DATAINT;

typedef enum
{
	ITEM_NAME,		   //物品原名
	ITEM_SECRETNAME,   //物品显示名
	ITEM_EFFECTSTRING, //物品说明
	ITEM_ARGUMENT,	   //物品字段
#ifdef _ITEM_INSLAY
	ITEM_TYPECODE,	//镶嵌标志
	ITEM_INLAYCODE, //可否镶嵌
#endif
	ITEM_CDKEY, //主人帐号
#ifdef _ITEM_FORUSERNAMES
	ITEM_FORUSERNAME,  //物品主人名字
	ITEM_FORUSERCDKEY, //物品主人帐号
#endif
#ifdef _UNIQUE_P_I
	ITEM_UNIQUECODE, //物品唯一码
#endif

	ITEM_INGNAME0, //物品成分1
	ITEM_INGNAME1, //物品成分2
	ITEM_INGNAME2, //物品成分3
	ITEM_INGNAME3, //物品成分4
	ITEM_INGNAME4, //物品成分5

	ITEM_INITFUNC,						//函数开始标志
	ITEM_FIRSTFUNCTION = ITEM_INITFUNC, //函数开始标志
	ITEM_PREOVERFUNC,					//未知事件函数1
	ITEM_POSTOVERFUNC,					//未知事件函数2
	ITEM_WATCHFUNC,						//观看事件函数
	ITEM_USEFUNC,						//使用事件函数
	ITEM_ATTACHFUNC,					//装备事件函数
	ITEM_DETACHFUNC,					//卸下事件函数
	ITEM_DROPFUNC,						//丢弃事件函数
	ITEM_PICKUPFUNC,					//捡起事件函数
#ifdef _Item_ReLifeAct
	ITEM_DIERELIFEFUNC, //战斗复活事件函数
#endif
	ITEM_LASTFUNCTION,					  //函数结束标志
	ITEM_DATACHARNUM = ITEM_LASTFUNCTION, //函数结束标志

#ifdef _ANGEL_SUMMON
	ITEM_ANGELMISSION = ITEM_INGNAME0, //精灵召唤标志
	ITEM_ANGELINFO = ITEM_INGNAME1,	   //天使标志
	ITEM_HEROINFO = ITEM_INGNAME2,	   //英雄标志
#endif
#ifdef _ITEM_UPGRADE
	ITEM_UPGRADEARG,
#endif
} ITEM_DATACHAR;

typedef enum
{
	ITEM_WORKOBJINDEX,
	ITEM_WORKCHARAINDEX,
#ifdef _ITEM_ORNAMENTS
	ITEM_CANPICKUP,
#endif
#ifdef _ITEM_TIME_LIMIT
	ITEM_WORKTIMELIMIT,
#endif
#ifdef _PET_AND_ITEM_UP
	ITEM_WORKCDKEY,
#endif
	ITEM_WORKDATAINTNUM,
} ITEM_WORKDATAINT;

typedef struct tagItem
{
	int data[ITEM_DATAINTNUM];
	STRING64 string[ITEM_DATACHARNUM];
	int workint[ITEM_WORKDATAINTNUM];
	void *functable[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];
#ifdef _ALLBLUES_LUA_1_2
	lua_State *lua[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];
	char *luafunctable[ITEM_LASTFUNCTION - ITEM_FIRSTFUNCTION];
#endif
} ITEM_Item;

typedef struct tagITEM_table
{
	int use;
	ITEM_Item itm;
	int randomdata[ITEM_DATAINTNUM];
} ITEM_table;

typedef struct tagITEM_index
{
	BOOL use;
	int index;
} ITEM_index;

typedef struct tagITEM_exists
{
	BOOL use;
	ITEM_Item itm;
} ITEM_exists;

#define ITEM_CHECKINDEX(index) \
	_ITEM_CHECKINDEX(__FILE__, __LINE__, index)
BOOL _ITEM_CHECKINDEX(char *file, int line, int index);

BOOL ITEM_initExistItemsArray(int num);
BOOL ITEM_endExistItemsArray(ITEM_table *ITEM_item);
#define ITEM_initExistItemsOne(itm) \
	_ITEM_initExistItemsOne(__FILE__, __LINE__, itm)
int _ITEM_initExistItemsOne(char *file, int line, ITEM_Item *itm);

#define ITEM_endExistItemsOne(index) \
	_ITEM_endExistItemsOne(index, __FILE__, __LINE__)

void _ITEM_endExistItemsOne(int index, char *file, int line);

#define ITEM_getInt(Index, element) _ITEM_getInt(__FILE__, __LINE__, Index, element)
int _ITEM_getInt(char *file, int line, int index, ITEM_DATAINT element);

#define ITEM_setInt(Index, element, data) _ITEM_setInt(__FILE__, __LINE__, Index, element, data)
int _ITEM_setInt(char *file, int line, int index, ITEM_DATAINT element, int data);

char *ITEM_getChar(int index, ITEM_DATACHAR element);
BOOL ITEM_setChar(int index, ITEM_DATACHAR element, char *new);

int ITEM_getWorkInt(int index, ITEM_WORKDATAINT element);
int ITEM_setWorkInt(int index, ITEM_WORKDATAINT element, int data);
int ITEM_getITEM_itemnum(void);
int ITEM_getITEM_UseItemnum(void);
BOOL ITEM_getITEM_use(int index);
void ITEM_constructFunctable(int itemindex);
#define ITEM_getFunctionPointer(itemindex, functype) _ITEM_getFunctionPointer(itemindex, functype, __FILE__, __LINE__)
void *_ITEM_getFunctionPointer(int itemindex, int functype, char *file, int line);
#ifdef _ALLBLUES_LUA_1_2
typedef struct tagITEM_LuaFunc
{
	lua_State *lua;
	char luafuncname[128];
	char luafunctable[128];
	struct tagITEM_LuaFunc *next;
} ITEM_LuaFunc;

BOOL ITEM_setLUAFunction(int itemindex, int functype, const char *luafuncname);
lua_State *ITEM_getLUAFunction(int itemindex, int functype);

BOOL ITEM_addLUAListFunction(lua_State *L, const char *luafuncname, const char *luafunctable);
#endif
ITEM_Item *ITEM_getItemPointer(int index);
int ITEM_getItemMaxIdNum(void);

char *ITEM_makeStringFromItemData(ITEM_Item *one, int mode);
char *ITEM_makeStringFromItemIndex(int index, int mode);

BOOL ITEM_makeExistItemsFromStringToArg(char *src, ITEM_Item *item, int mode);
void ITEM_getDefaultItemSetting(ITEM_Item *itm);

BOOL ITEM_CHECKITEMTABLE(int number);
BOOL ITEM_readItemConfFile(char *filename);

CHAR_EquipPlace ITEM_getEquipPlace(int charaindex, int itmid);

char *ITEM_makeItemStatusString(int haveitemindex, int itemindex);
char *ITEM_makeItemFalseString(void);
char *ITEM_makeItemFalseStringWithNum(int haveitemindex);

BOOL ITEM_makeItem(ITEM_Item *itm, int number);
int ITEM_makeItemAndRegist(int number);

void ITEM_equipEffect(int index);

void Other_DefcharWorkInt(int index);

char *ITEM_getAppropriateName(int itemindex);
char *ITEM_getEffectString(int itemindex);

int ITEM_getcostFromITEMtabl(int itemid);

#define ITEM_getNameFromNumber(id) _ITEM_getNameFromNumber(__FILE__, __LINE__, id)
char *_ITEM_getNameFromNumber(char *file, int line, int itemid);

#define ITEM_getSecretNameFromNumber(id) _ITEM_getSecretNameFromNumber(__FILE__, __LINE__, id)
char *_ITEM_getSecretNameFromNumber(char *file, int line, int itemid);

int ITEM_getlevelFromITEMtabl(int itemid);
int ITEM_getgraNoFromITEMtabl(int itemid);
char *ITEM_getItemInfoFromNumber(int itemid);

int ITEM_getdropatlogoutFromITEMtabl(int itemid);
int ITEM_getvanishatdropFromITEMtabl(int itemid);
int ITEM_getcanpetmailFromITEMtabl(int itemid);
int ITEM_getmergeItemFromFromITEMtabl(int itemid);

#ifdef _ITEM_CHECKWARES
BOOL CHAR_CheckInItemForWares(int charaindex, int flg);
#endif

BOOL ITEM_canuseMagic(int itemindex);
// Nuke +1 08/23 : For checking the validity of item target
int ITEM_isTargetValid(int charaindex, int itemindex, int toindex);

int ITEMTBL_getInt(int ItemID, ITEM_DATAINT datatype);
char *ITEMTBL_getChar(int ItemID, ITEM_DATACHAR datatype);

int ITEM_getItemDamageCrusheED(int itemindex);
void ITEM_RsetEquit(int charaindex); //自动卸除装备位置错误之物品
void ITEM_reChangeItemToPile(int itemindex);
void ITEM_reChangeItemName(int itemindex);

#ifdef _SIMPLIFY_ITEMSTRING
void ITEM_getDefaultItemData(int itemID, ITEM_Item *itm);
#endif

#ifdef _PET_ITEM
char *ITEM_petmakeItemStatusString(int petindex, int itemid);
#endif

#endif
