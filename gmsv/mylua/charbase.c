#include "NewBilu/version.h"
#include "battle.h"
#include "char.h"
#include "char_base.h"
#include "char_talk.h"
#include "configfile.h"
#include "enemy.h"
#include "handletime.h"
#include "item_event.h"
#include "lauxlib.h"
#include "log.h"
#include "lssproto_serv.h"
#include "lua.h"
#include "lualib.h"
#include "mylua/base.h"
#include "npc_healer.h"
#include "npcutil.h"
#include "object.h"
#include "pet.h"
#include "pet_skill.h"
#include "readmap.h"
#include <string.h>
#ifdef _ALLBLUES_LUA
#include "mylua/mylua.h"
extern MY_Lua MYLua;

static CharBase CharBaseWorkInt[] = {
    {{"对象"}, CHAR_WORKOBJINDEX}
#ifdef _FONT_SIZE
    ,
    {{"字号"}, CHAR_WORKFONTSIZE}
#endif
    ,
    {{"战斗"}, CHAR_WORKBATTLEMODE},
    {{"组队"}, CHAR_WORKPARTYMODE},
    {{"战斗索引"}, CHAR_WORKBATTLEINDEX},
    {{"NPC临时1"}, CHAR_NPCWORKINT1},
    {{"NPC临时2"}, CHAR_NPCWORKINT2},
    {{"NPC临时3"}, CHAR_NPCWORKINT3},
    {{"NPC临时4"}, CHAR_NPCWORKINT4},
    {{"NPC临时5"}, CHAR_NPCWORKINT5},
    {{"NPC临时6"}, CHAR_NPCWORKINT6},
    {{"NPC临时7"}, CHAR_NPCWORKINT7},
    {{"NPC临时8"}, CHAR_NPCWORKINT8},
    {{"NPC临时9"}, CHAR_NPCWORKINT9},
    {{"NPC临时10"}, CHAR_NPCWORKINT10},
    {{"NPC临时11"}, CHAR_NPCWORKINT11},
    {{"NPC临时12"}, CHAR_NPCWORKINT12},
    {{"NPC临时13"}, CHAR_NPCWORKINT13},
    {{"捡起模式"}, CHAR_WORKPETFOLLOWMODE},
    {{"队员1"}, CHAR_WORKPARTYINDEX1},
    {{"队员2"}, CHAR_WORKPARTYINDEX2},
    {{"队员3"}, CHAR_WORKPARTYINDEX3},
    {{"队员4"}, CHAR_WORKPARTYINDEX4},
    {{"队员5"}, CHAR_WORKPARTYINDEX5},
    {{"最大HP"}, CHAR_WORKMAXHP},
    {{"最大MP"}, CHAR_WORKMAXMP},
    {{"攻击力"}, CHAR_WORKATTACKPOWER},
    {{"防御力"}, CHAR_WORKDEFENCEPOWER},
    {{"敏捷力"}, CHAR_WORKQUICK},
    {{"修正体力"}, CHAR_WORKFIXVITAL},
    {{"修正腕力"}, CHAR_WORKFIXSTR},
    {{"修正耐力"}, CHAR_WORKFIXTOUGH},
    {{"修正速度"}, CHAR_WORKFIXDEX},
    {{"家族地图"}, CHAR_WORKFMFLOOR},
    {{"攻击"}, CHAR_WORKATTACKPOWER},
    {{"防御"}, CHAR_WORKDEFENCEPOWER},
    {{"敏捷"}, CHAR_WORKQUICK},
    {{"逃跑"}, CHAR_WORKDBATTLEESCAPE},
    {{"经验时间"}, CHAR_WORKITEM_ADDEXPTIME},
    {{"经验加成"}, CHAR_WORKITEM_ADDEXP}
#ifdef _OFFLINE_SYSTEM
    ,
    {{"离线"}, CHAR_WORK_OFFLINE}
#endif
    ,
    {{"获得经验"}, CHAR_WORKGETEXP}
#ifdef _AUTO_PK
    ,
    {{"自动PK点"}, CHAR_WORK_AUTOPK},
    {{"自动PK死亡"}, CHAR_WORK_AUTOPK_DEAD}
#endif
#ifdef _ALL_SERV_SEND
    ,
    {{"星球频道时"}, CHAR_WORKINTALLSERTALKTIME}
#endif
    ,
    {{"计时器"}, CHAR_WORK_TIMER}
#ifdef _STREET_VENDOR
    ,
    {{"摆摊"}, CHAR_WORKSTREETVENDOR}
#endif
#ifdef _RECORD_IP
    ,
    {{"IP"}, CHAR_WORK_RECORD_IP}
#endif
#ifdef _ONLINE_TIME
    ,
    {{"登陆时间"}, CHAR_WORK_LOGIN_TIME}
#endif
#ifdef _NEWCLISETSERVID
    ,
    {{"服务器ID"}, CHAR_WORK_SERVID}
#endif
    ,
    {{"PK时间"}, CHAR_WORKDBATTLETIME},
    {{"使用道具索引"}, CHAR_WORKUSEINDEX},
    {{"被使用道具索引"}, CHAR_WORKTOPPINDEX},
    {{"宠物主人索引"}, CHAR_WORKPLAYERINDEX}
#ifdef _NB_TMWORKVER
    ,
    {{"临时整数1"}, CHAR_NBWORKINT1},
    {{"临时整数2"}, CHAR_NBWORKINT2},
    {{"临时整数3"}, CHAR_NBWORKINT3},
    {{"临时整数4"}, CHAR_NBWORKINT4},
    {{"临时整数5"}, CHAR_NBWORKINT5},
    {{"临时整数6"}, CHAR_NBWORKINT6},
    {{"临时整数7"}, CHAR_NBWORKINT7},
    {{"临时整数8"}, CHAR_NBWORKINT8},
    {{"临时整数9"}, CHAR_NBWORKINT9},
    {{"临时整数10"}, CHAR_NBWORKINT10}
#endif

    ,
    {{"丛林时间"}, CHAR_WORKZOOROOMTIME}
#ifdef _NB_SAVEWININDEX
    ,
    {{"窗口索引"}, CHAR_NBWORKWININDX}
#endif
#ifdef _NB_NOBATTLEBUTTON
    ,
    {{"禁止求救"}, CHAR_WORK_NOHELP},
    {{"禁止逃跑"}, CHAR_WORK_NOESCAPE}
#endif

    ,
    {{"WORKBATTLEMODE"}, CHAR_WORKBATTLEMODE},
    {{"WORKPARTYMODE"}, CHAR_WORKPARTYMODE},
    {{"WORKBATTLEINDEX"}, CHAR_WORKBATTLEINDEX},
    {{"WORKPARTYINDEX1"}, CHAR_WORKPARTYINDEX1},
    {{"WORKPARTYINDEX2"}, CHAR_WORKPARTYINDEX2},
    {{"WORKPARTYINDEX3"}, CHAR_WORKPARTYINDEX3},
    {{"WORKPARTYINDEX4"}, CHAR_WORKPARTYINDEX4},
    {{"WORKPARTYINDEX5"}, CHAR_WORKPARTYINDEX5},
    {{"WORKMAXHP"}, CHAR_WORKMAXHP}

    ,
    {{"WORKLOWSPEED"}, CHAR_WORKPARTYLOWDEX},
    {{"WORKHIGHSPEED"}, CHAR_WORKPARTYHIGHDEX},
    {{"WORKBATTLEMODE"}, CHAR_WORKBATTLEMODE},
    {{"NBWORKINT1"}, CHAR_NBWORKINT1},
    {{"WORKOBJINDEX"}, CHAR_WORKOBJINDEX}};

static CharBase CharBaseWorkChar[] = {{{"NPC临时1"}, CHAR_NPCWORKCHAR1},
                                      {{"NPC临时2"}, CHAR_NPCWORKCHAR2},
                                      {{"NPC临时3"}, CHAR_NPCWORKCHAR3},
                                      {{"NPC临时4"}, CHAR_NPCWORKCHAR4},
                                      {{"NPC临时5"}, CHAR_NPCWORKCHAR5},
                                      {{"NPC临时6"}, CHAR_NPCWORKCHAR6}
#ifdef _STREET_VENDOR
                                      ,
                                      {{"摆摊"}, CHAR_STREETVENDOR_NAME}
#endif
#ifdef _NEWCLISETMAC
                                      ,
                                      {{"MAC"}, CHAR_WORK_MAC},
                                      {{"CPU"}, CHAR_WORK_MAC2}
#endif
#ifdef _NB_TMWORKVER
                                      ,
                                      {{"临时字串1"}, CHAR_NBWORKCHAR1},
                                      {{"NBWORKCHAR1"}, CHAR_NBWORKCHAR1},
                                      {{"临时字串2"}, CHAR_NBWORKCHAR2},
                                      {{"临时字串3"}, CHAR_NBWORKCHAR3},
                                      {{"临时字串4"}, CHAR_NBWORKCHAR4},
                                      {{"临时字串5"}, CHAR_NBWORKCHAR5},
                                      {{"临时字串6"}, CHAR_NBWORKCHAR6},
                                      {{"临时字串7"}, CHAR_NBWORKCHAR7},
                                      {{"临时字串8"}, CHAR_NBWORKCHAR8},
                                      {{"临时字串9"}, CHAR_NBWORKCHAR9},
                                      {{"临时字串10"}, CHAR_NBWORKCHAR10}
#endif
};

static CharBase CharBaseValue[] = {{{"北"}, 0},
                                   {{"东北"}, 1},
                                   {{"东"}, 2},
                                   {{"东南"}, 3},
                                   {{"南"}, 4},
                                   {{"西南"}, 5},
                                   {{"西"}, 6},
                                   {{"西北"}, 7},
                                   {{"无类型"}, CHAR_TYPENONE},
                                   {{"玩家"}, CHAR_TYPEPLAYER},
                                   {{"敌人"}, CHAR_TYPEENEMY},
                                   {{"其飘"}, CHAR_TYPEPET}
#ifdef _ALLBLUES_LUA
                                   ,
                                   {{"LUA"}, CHAR_TYPELUANPC}
#endif
#ifdef _PLAYER_NPC
                                   ,
                                   {{"帮托"}, CHAR_TYPEPLAYERNPC},
                                   {{"帮宠"}, CHAR_TYPEPLAYERPETNPC}
#endif
};

static CharBase CharBaseWorkValue[] = {
    {{"无"}, CHAR_PARTY_NONE},
    {{"队长"}, CHAR_PARTY_LEADER},
    {{"队员"}, CHAR_PARTY_CLIENT},
    {{"无战斗"}, BATTLE_CHARMODE_NONE},
    {{"战斗初始化"}, BATTLE_CHARMODE_INIT},
    {{"等待战斗"}, BATTLE_CHARMODE_C_WAIT},
    {{"确认战斗"}, BATTLE_CHARMODE_C_OK},
    {{"帮助战斗"}, BATTLE_CHARMODE_RESCUE},
    {{"结束战斗"}, BATTLE_CHARMODE_FINAL},
    {{"观战初始化"}, BATTLE_CHARMODE_WATCHINIT},
    {{"没有家族"}, FMMEMBER_NONE},
    {{"申请入族"}, FMMEMBER_APPLY},
    {{"族长"}, FMMEMBER_LEADER},
    {{"成员"}, FMMEMBER_MEMBER},
    {{"长老"}, FMMEMBER_ELDER}};

static CharBase CharBaseInt[] = {
    {{"图像号"}, CHAR_BASEIMAGENUMBER},
    {{"BASEIMAGENUMBER"}, CHAR_BASEIMAGENUMBER},
    {{"原图像号"}, CHAR_BASEBASEIMAGENUMBER},
    {{"存档"}, CHAR_SAVEINDEXNUMBER},
    {{"地图号"}, CHAR_FLOOR},
    {{"坐标X"}, CHAR_X},
    {{"坐标Y"}, CHAR_Y},
    {{"方向"}, CHAR_DIR},
    {{"等级"}, CHAR_LV},
    {{"石币"}, CHAR_GOLD},
    {{"HP"}, CHAR_HP},
    {{"MP"}, CHAR_MP},
    {{"最大MP"}, CHAR_MAXMP},
    {{"体力"}, CHAR_VITAL},
    {{"腕力"}, CHAR_STR},
    {{"耐力"}, CHAR_TOUGH},
    {{"速度"}, CHAR_DEX},
    {{"地"}, CHAR_EARTHAT},
    {{"水"}, CHAR_WATERAT},
    {{"火"}, CHAR_FIREAT},
    {{"风"}, CHAR_WINDAT},
    {{"战宠"}, CHAR_DEFAULTPET},
    {{"魅力"}, CHAR_CHARM},
    {{"运气"}, CHAR_LUCK},
    {{"死亡次数"}, CHAR_DEADCOUNT},
    {{"走路步数"}, CHAR_WALKCOUNT},
    {{"说话次数"}, CHAR_TALKCOUNT},
    {{"损坏次数"}, CHAR_DAMAGECOUNT},
    {{"给宠次数"}, CHAR_GETPETCOUNT},
    {{"杀宠次数"}, CHAR_KILLPETCOUNT},
    {{"死宠次数"}, CHAR_DEADPETCOUNT},
    {{"发邮件数"}, CHAR_SENDMAILCOUNT},
    {{"合成次数"}, CHAR_MERGEITEMCOUNT},
    {{"PK次数"}, CHAR_DUELBATTLECOUNT},
    {{"PK赢数"}, CHAR_DUELWINCOUNT},
    {{"PK败数"}, CHAR_DUELLOSECOUNT},
    {{"PK连胜"}, CHAR_DUELSTWINCOUNT},
    {{"PK连败"}, CHAR_DUELSTLOSECOUNT},
    {{"PK最高连胜"}, CHAR_DUELMAXSTWINCOUNT},
    {{"辆幅"}, CHAR_WHICHTYPE},
    {{"循环事件时间"}, CHAR_LOOPINTERVAL}
#ifdef _NEWOPEN_MAXEXP
    ,
    {{"总经验"}, CHAR_OLDEXP}
#endif
    ,
    {{"当前经验"}, CHAR_EXP},
    {{"技能点"}, CHAR_SKILLUPPOINT},
    {{"升级点"}, CHAR_LEVELUPPOINT},
    {{"DP"}, CHAR_DUELPOINT},
    {{"经验"}, CHAR_EXP},
    {{"出生地"}, CHAR_LASTTALKELDER},
    {{"转数"}, CHAR_TRANSMIGRATION},
    {{"禁言"}, CHAR_SILENT},
    {{"家族索引"}, CHAR_FMINDEX},
    {{"家族地位"}, CHAR_FMLEADERFLAG},
    {{"FMLEADERFLAG"}, CHAR_FMLEADERFLAG},
    {{"家族类型"}, CHAR_FMSPRITE},
    {{"FMSPRITE"}, CHAR_FMSPRITE}
#ifdef _NEW_RIDEPETS
    ,
    {{"证书骑宠"}, CHAR_LOWRIDEPETS},
    {{"LOWRIDEPETS"}, CHAR_LOWRIDEPETS}
#endif
#ifdef _RIDE_CF
    ,
    {{"证书骑宠1"}, CHAR_LOWRIDEPETS1},
    {{"LOWRIDEPETS1"}, CHAR_LOWRIDEPETS1}
#ifdef _ADD_RIDE_CF
    ,
    {{"证书骑宠2"}, CHAR_HIGHRIDEPET2},
    {{"HIGHRIDEPET2"}, CHAR_HIGHRIDEPET2}
#endif
    ,
    {{"骑乘等级"}, CHAR_LEARNRIDE}
#endif
    ,
    {{"个人银行"}, CHAR_BANKGOLD},
    {{"骑宠"}, CHAR_RIDEPET},
    {{"暴击"}, CHAR_CRITIAL},
    {{"反击"}, CHAR_COUNTER},
    {{"下线时间"}, CHAR_LASTLEAVETIME}
#ifdef _GAMBLE_BANK
//	,{{"赌场银行"}, 	CHAR_RIDEPET}
#endif
#ifdef _DROPSTAKENEW
    ,
    {{"赌场积分"}, CHAR_GAMBLENUM}
#endif
#ifdef _PERSONAL_FAME // Arminius 8.30: 家族个人声望
    ,
    {{"声望"}, CHAR_FAME}
#endif
#ifdef _ATTACK_MAGIC
    ,
    {{"地魔法熟练度"}, CHAR_EARTH_EXP},
    {{"水魔法熟练度"}, CHAR_WATER_EXP},
    {{"火魔法熟练度"}, CHAR_FIRE_EXP},
    {{"风魔法熟练度"}, CHAR_WIND_EXP},
    {{"地魔法抗性"}, CHAR_EARTH_RESIST},
    {{"水魔法抗性"}, CHAR_WATER_RESIST},
    {{"火魔法抗性"}, CHAR_FIRE_RESIST},
    {{"风魔法抗性"}, CHAR_WIND_RESIST},
    {{"地魔法熟练度经验"}, CHAR_EARTH_ATTMAGIC_EXP},
    {{"水魔法熟练度经验"}, CHAR_WATER_ATTMAGIC_EXP},
    {{"火魔法熟练度经验"}, CHAR_FIRE_ATTMAGIC_EXP},
    {{"风魔法熟练度经验"}, CHAR_WIND_ATTMAGIC_EXP},
    {{"地魔法抗性经验值"}, CHAR_EARTH_DEFMAGIC_EXP},
    {{"水魔法抗性经验值"}, CHAR_WATER_DEFMAGIC_EXP},
    {{"火魔法抗性经验值"}, CHAR_FIRE_DEFMAGIC_EXP},
    {{"风魔法抗性经验值"}, CHAR_WIND_DEFMAGIC_EXP}
#endif
#ifdef _CHAR_PROFESSION // WON ADD 人物职业
    ,
    {{"职业别"}, PROFESSION_CLASS},
    {{"职业等级"}, PROFESSION_LEVEL},
    {{"技能点数"}, PROFESSION_SKILL_POINT},
    {{"增加堆叠"}, ATTACHPILE},
    {{"火熟练度"}, PROFESSION_FIRE_P},
    {{"冰熟练度"}, PROFESSION_ICE_P},
    {{"雷熟练度"}, PROFESSION_THUNDER_P},
    {{"火抗性"}, PROFESSION_FIRE_R},
    {{"冰抗性"}, PROFESSION_ICE_R},
    {{"雷抗性"}, PROFESSION_THUNDER_R}
#endif
#ifdef _NEW_MANOR_LAW
    ,
    {{"气势"}, CHAR_MOMENTUM}
#endif
#ifdef _POINT_AMPOINT
    ,
    {{"积分"}, CHAR_AMPOINT}
#endif
#ifdef _ITEM_PET_LOCKED
    ,
    {{"安全锁"}, CHAR_LOCKED}
#endif
#ifdef _ONLINE_SHOP
    ,
    {{"贝币"}, CHAR_BJ}
#endif
    ,
    {{"模式AI"}, CHAR_MODAI},
    {{"可变AI"}, CHAR_VARIABLEAI},
    {{"宠技位"}, CHAR_SLOT},
    {{"宠ID"}, CHAR_PETID},
    {{"守护兽"}, CHAR_PETFAMILY},
    {{"能力值"}, CHAR_ALLOCPOINT},
    {{"成长区间"}, CHAR_PETRANK},
    {{"限制等级"}, CHAR_LIMITLEVEL}
#ifdef _PET_BEATITUDE
    ,
    {{"祝福值"}, CHAR_BEATITUDE},
    {{"祝福品"}, CHAR_BEATITUDETYPE}
#endif
#ifdef _PET_PETZUOBI
    ,
    {{"宠作弊值"}, CHAR_PETZUOBI}
#endif
#ifdef _MISSION_TRAIN
    ,
    {{"跑环链"}, CHAR_MISSIONTRAIN_NUM},
    {{"跑环链时间"}, CHAR_MISSIONTRAIN_TIME}
#endif
#ifdef _PET_EVOLVE
    ,
    {{"进化"}, CHAR_EVOLVE}
#endif
#ifdef _ONLINE_TIME
    ,
    {{"在线时间"}, CHAR_ONLINE_TIME}
#endif
#ifdef _FLOOR_DIFI_FIELD
    ,
    {{"楼层时间"}, CHAR_FLOORDIFI_TIME}
#endif
#ifdef _FLOOR_FUBEN
    ,
    {{"副本时间"}, CHAR_FLOORFB_TIME}
#endif
#ifdef _ACTIVE_GAME
    ,
    {{"活力"}, CHAR_ACTIVE}
#endif
#ifdef _SUPER
    ,
    {{"极品"}, CHAR_SUPER}
#endif
#ifdef _ALLDOMAN
    ,
    {{"英雄称号"}, CHAR_HEROFLOOR}
#endif

#ifdef _PLAYER_TITLE
    ,
    {{"称号等级"}, CHAR_TITLE_LV},
    {{"称号时间"}, CHAR_TITLE_TIME}
#endif
#ifdef _MISSION_TIME
    ,
    {{"任务计时"}, CHAR_MISSION_TIME}
#endif
#ifdef _PET_FUSION
    ,
    {{"融合宠"}, CHAR_FUSIONBEIT},
    {{"融合编码"}, CHAR_FUSIONCODE},
    {{"喂养次数"}, CHAR_FUSIONRAISE},
    {{"体力药"}, CHAR_EVOLUTIONBASEVTL},
    {{"攻击药"}, CHAR_EVOLUTIONBASESTR},
    {{"防御药"}, CHAR_EVOLUTIONBASETGH},
    {{"速度药"}, CHAR_EVOLUTIONBASEDEX},
    {{"喂养时间"}, CHAR_FUSIONTIMELIMIT},
    {{"孵化ID"}, CHAR_FUSIONINDEX}
#ifdef _PET_FUSION_2
    ,
    {{"融合类型"}, CHAR_FUSIONTYPE}
#endif

#endif
#ifdef _REGISTRATION_TIME
    ,
    {{"签到时间"}, CHAR_REGISTRATION_TIME},
    {{"签到次数"}, CHAR_REGISTRATION_COUNT},
    {{"签到在线时间"}, CHAR_REGISTRATION_ONLINE_TIME}
#endif
#ifdef _OFFLINE_TIME
    ,
    {{"离线时间"}, CHAR_OFFLINE_TIME}
#endif
#ifdef _FAME_REG_TIME
    ,
    {{"家族签到时间"}, CHAR_FAME_REG_TIME}
#endif
#ifdef _TENSE_FIELD
    ,
    {{"限时字段"}, CHAR_TENSE_FIELD}
#endif
    ,
    {{"刷楼积分"}, CHAR_SHUALOU_POINT}
#ifdef _NEW_MAP_NO_FAME
    ,
    {{"无声望模式"}, CHAR_NOFAME}
#endif
#ifdef _LOCK_PET_ITEM
    ,
    {{"绑定"}, CHAR_PETLOCK}
#endif
#ifdef _NB_PETINITBILITY
    ,
    {{"宠初等级"}, CHAR_PETINITLV},
    {{"宠初耐久"}, CHAR_PETATT1},
    {{"宠初攻击"}, CHAR_PETATT2},
    {{"宠初防御"}, CHAR_PETATT3},
    {{"宠初敏捷"}, CHAR_PETATT4}
#endif
#ifdef _NB_PETREPOINT
    ,
    {{"宠回炉次"}, CHAR_PETINIREPOINT}
#endif
#ifdef _AUTO_PK
    ,
    {{"乱舞积分"}, CHAR_AUTOPKPOINT}
#endif
#ifdef _NB_TLPOINT
    ,
    {{"体能值"}, CHAR_TLPOINT}
#endif
#ifdef _NB_ITEM_ADDEXP
    ,
    {{"天数经人倍"}, CHAR_DAYEXPNUM1},
    {{"天数经宠倍"}, CHAR_DAYEXPNUM2},
    {{"天数经骑倍"}, CHAR_DAYEXPNUM3},
    {{"天数经人时"}, CHAR_DAYEXPTIME1},
    {{"天数经宠时"}, CHAR_DAYEXPTIME2},
    {{"天数经骑时"}, CHAR_DAYEXPTIME3}
#endif
    ,
    {{"骑宠名字模式"}, CHAR_RIDENAMEFLG}
#ifdef _NB_SKIN
    ,
    {{"皮肤属性"}, CHAR_NB_SKIN}
#endif
#ifdef _NB_CHARSTRTITLE
    ,
    {{"现称号颜色"}, CHAR_NB_NAMECOLOR},
    {{"现称号属性"}, CHAR_NB_NAMESX}
#endif
#ifdef _NB_CHARHALO
    ,
    {{"现光环图像"}, CHAR_NB_HALO},
    {{"现光环属性"}, CHAR_NB_HALOSX}
#endif
#ifdef _NB_CHARDISPTITLE
    ,
    {{"现称号图像"}, CHAR_NB_NAMEMOTO},
    {{"现称号属性"}, CHAR_NB_NAMEMOTOSX}
#endif
#ifdef _NB_EVENTDAYTIME
    ,
    {{"每日任务时"}, CHAR_EVENTDAYTIM}
#endif
#ifdef _NB_NEWDAYEVENT
    ,
    {{"春节任务时"}, CHAR_NEWDAYEVENT}
#endif
#ifdef _NB_EVENTVER
    ,
    {{"任_数量"}, CHAR_NBEVENTNUM},
    {{"任_状态"}, CHAR_NBEVENTSTART},
    {{"任_类型"}, CHAR_NBEVENTTYPE},
    {{"任_需求"}, CHAR_NBEVENTNEED},
    {{"任_时间"}, CHAR_NBEVENTFLG}
#endif
#ifdef _NB_EVENTVER
    ,
    {{"存储整数1"}, CHAR_NBSAVEINT0},
    {{"存储整数2"}, CHAR_NBSAVEINT1},
    {{"存储整数3"}, CHAR_NBSAVEINT2},
    {{"存储整数4"}, CHAR_NBSAVEINT3},
    {{"存储整数5"}, CHAR_NBSAVEINT4},
    {{"存储整数6"}, CHAR_NBSAVEINT5},
    {{"存储整数7"}, CHAR_NBSAVEINT6},
    {{"存储整数8"}, CHAR_NBSAVEINT7},
    {{"存储整数9"}, CHAR_NBSAVEINT8},
    {{"存储整数10"}, CHAR_NBSAVEINT9}
#endif
    ,
    {{"HP"}, CHAR_HP},
    {{"EXP"}, CHAR_EXP},
    {{"LOOPINTERVAL"}, CHAR_LOOPINTERVAL},
    {{"WHICHTYPE"}, CHAR_WHICHTYPE},
    {{"FLOOR"}, CHAR_FLOOR},
    {{"X"}, CHAR_X},
    {{"Y"}, CHAR_Y},
    {{"GOLD"}, CHAR_GOLD},
    {{"LV"}, CHAR_LV},
    {{"AMPOINT"}, CHAR_AMPOINT},
    {{"GOLD"}, CHAR_GOLD},
    {{"BASEBASEIMAGENUMBER"}, CHAR_BASEBASEIMAGENUMBER},
    {{"RAIDPOINT"}, CHAR_RAIDPOINT},
    {{"EVENTPOINT"}, CHAR_EVENTPOINT}};

static CharBase CharBaseChar[] = {{{"名字"}, CHAR_NAME},
                                  {{"昵称"}, CHAR_OWNTITLE},
                                  {{"主人"}, CHAR_USERPETNAME},
                                  {{"账号"}, CHAR_CDKEY},
                                  {{"帐号"}, CHAR_CDKEY},
                                  {{"家族"}, CHAR_FMNAME}
#ifdef _TEACHER_SYSTEM
                                  ,
                                  {{"导师帐号"}, CHAR_TEACHER_ID},
                                  {{"导师名字"}, CHAR_TEACHER_NAME}
#endif
#ifdef _ITEM_SETLOVER
                                  ,
                                  {{"爱人账号"}, CHAR_LOVERID},
                                  {{"爱人名字"}, CHAR_LOVERNAME}
#endif
                                  ,
                                  {{"主人账号"}, CHAR_OWNERCDKEY},
                                  {{"主人名字"}, CHAR_OWNERCHARANAME},
                                  {{"唯一编号"}, CHAR_UNIQUECODE}
#ifdef _NB_CHARSTRTITLE
                                  ,
                                  {{"现称号文字"}, CHAR_NB_NAME}
#endif
};

static CharBase CharBaseEvent[] = {
    {{"初始化事件"}, CHAR_INITFUNC}
    //,{{"行走事件"}, 		CHAR_WALKPREFUNC}
    //,{{"行走后事件"},		CHAR_WALKPOSTFUNC}
    //,{{"未知事件"}, 		CHAR_PREOVERFUNC}
    //,{{"未知事件"}, 		CHAR_POSTOVERFUNC}
    //,{{"观看事件"}, 		CHAR_WATCHFUNC}
    ,
    {{"循环事件"}, CHAR_LOOPFUNC}
    //,{{"死亡事件"}, 		CHAR_DYINGFUNC}
    ,
    {{"对话事件"}, CHAR_TALKEDFUNC}
    //,{{"攻击事件"},			CHAR_PREATTACKEDFUNC}
    //,{{"攻击后事件"}, 	CHAR_POSTATTACKEDFUNC}
    //,{{"离开事件"}, 		CHAR_OFFFUNC}
    //,{{"看事件"}, 			CHAR_LOOKEDFUNC}
    //,{{"丢弃事件"},			CHAR_ITEMPUTFUNC}
    //,{{"特殊对话事件"}, CHAR_SPECIALTALKEDFUNC}
    ,
    {{"窗口事件"}, CHAR_WINDOWTALKEDFUNC}
#ifdef _USER_CHARLOOPS
//,{{"循环事件1"}, 		CHAR_LOOPFUNCTEMP1}
//,{{"循环事件2"}, 		CHAR_LOOPFUNCTEMP2}
//,{{"未知事件"}, 		CHAR_BATTLEPROPERTY}
#endif
    ,
    {{"重叠事件"}, CHAR_OVERLAPEDFUNC},
    {{"战后事件"}, CHAR_BATTLEOVERDFUNC}
#ifdef _ALLBLUES_LUA_1_4
    ,
    {{"登出事件"}, CHAR_LOGINOUTFUNC}
#endif
#ifdef _ALLBLUES_LUA_1_9
    ,
    {{"战斗设置事件"}, CHAR_BATTLESETFUNC}
#endif
    ,
    {{"LOOPFUNC"}, CHAR_LOOPFUNC},
    {{"TALKEDFUNC"}, CHAR_TALKEDFUNC},
    {{"WINDOWTALKEDFUNC"}, CHAR_WINDOWTALKEDFUNC},
    {{"BATTLEOVERDFUNC"}, CHAR_BATTLEOVERDFUNC}};

// static CharBase CharBaseColor[] = {
//	{{"白色"}, 			CHAR_COLORWHITE}
//	,{{"青色"}, 		CHAR_COLORCYAN}
//	,{{"粉色"},			CHAR_COLORPURPLE}
//	,{{"蓝色"}, 		CHAR_COLORBLUE}
//	,{{"黄色"}, 		CHAR_COLORYELLOW}
//	,{{"绿色"}, 		CHAR_COLORGREEN}
//	,{{"红色"}, 		CHAR_COLORRED}
//	,{{"灰白色"}, 	CHAR_COLORGRAY}
//	,{{"灰蓝色"}, 	CHAR_COLORBLUE2}
//	,{{"灰绿色"}, 	CHAR_COLORGREEN2}
//
//
//#ifdef _NEW_FONT // <<<<<<-------
//	,{{"紫色"}, 	  CHAR_COLORVIOLET}
//	,{{"橙色"}, 	  CHAR_COLORORANGE}
//	,{{"暗?色"}, 	CHAR_COLORYELLOW2}
//	,{{"金色"}, 	  CHAR_COLORGOLDEN}
//	,{{"暗橙色"}, 	CHAR_COLORORANGE2}
//#endif
//	,{{"黑色"}, 	CHAR_COLORBLACK}
//	,{{"随机色"},   CHAR_COLORNUM}
//
// };

static CharBase CharBaseColor[] = {{{"白色"}, CHAR_COLORWHITE},
                                   {{"青色"}, CHAR_COLORCYAN},
                                   {{"粉色"}, CHAR_COLORPURPLE},
                                   {{"蓝色"}, CHAR_COLORBLUE},
                                   {{"黄色"}, CHAR_COLORYELLOW},
                                   {{"绿色"}, CHAR_COLORGREEN},
                                   {{"红色"}, CHAR_COLORRED},
                                   {{"灰白色"}, CHAR_COLORGRAY},
                                   {{"灰蓝色"}, CHAR_COLORBLUE2},
                                   {{"灰绿色"}, CHAR_COLORGREEN2}

#ifdef _NEW_FONT // <<<<<<-------
                                   ,
                                   {{"紫色"}, CHAR_COLORVIOLET},
                                   {{"橙色"}, CHAR_COLORORANGE},
                                   {{"暗?色"}, CHAR_COLORYELLOW2},
                                   {{"金色"}, CHAR_COLORGOLDEN},
                                   {{"暗橙色"}, CHAR_COLORORANGE2}
#endif
                                   ,
                                   {{"黑色"}, CHAR_COLORBLACK},
                                   {{"随机色"}, CHAR_COLORNUM}

                                   ,
                                   {{"WHITE"}, CHAR_COLORWHITE},
                                   {{"CYAN"}, CHAR_COLORCYAN},
                                   {{"PURPLE"}, CHAR_COLORPURPLE},
                                   {{"BLUE"}, CHAR_COLORBLUE},
                                   {{"YELLOW"}, CHAR_COLORYELLOW},
                                   {{"GREEN"}, CHAR_COLORGREEN},
                                   {{"RED"}, CHAR_COLORRED},
                                   {{"GRAY"}, CHAR_COLORGRAY},
                                   {{"BLUE2"}, CHAR_COLORBLUE2},
                                   {{"GREEN2"}, CHAR_COLORGREEN2}

#ifdef _NEW_FONT // <<<<<<-------
                                   ,
                                   {{"VIOLET"}, CHAR_COLORVIOLET},
                                   {{"ORANGE"}, CHAR_COLORORANGE},
                                   {{"YELLOW2"}, CHAR_COLORYELLOW2},
                                   {{"GOLDEN"}, CHAR_COLORGOLDEN},
                                   {{"ORANGE2"}, CHAR_COLORORANGE2}
#endif
                                   ,
                                   {{"BLACK"}, CHAR_COLORBLACK},
                                   {{"NUMCOL"}, CHAR_COLORNUM}};

static CharBase CharBaseUpdata[] = {
    {{"HP"}, CHAR_P_STRING_HP},
    {{"MAXHP"}, CHAR_P_STRING_MAXHP},
    {{"MP"}, CHAR_P_STRING_MP},
    {{"MAXMP"}, CHAR_P_STRING_MAXMP},
    {{"体力"}, CHAR_P_STRING_VITAL},
    {{"腕力"}, CHAR_P_STRING_STR},
    {{"耐力"}, CHAR_P_STRING_TOUGH},
    {{"速度"}, CHAR_P_STRING_DEX},
    {{"经验"}, CHAR_P_STRING_EXP},
    {{"下一级经验"}, CHAR_P_STRING_NEXTEXP},
    {{"等级"}, CHAR_P_STRING_LV},
    {{"攻击力"}, CHAR_P_STRING_ATK},
    {{"防御力"}, CHAR_P_STRING_DEF},
    {{"敏捷力"}, CHAR_P_STRING_QUICK},
    {{"魅力"}, CHAR_P_STRING_CHARM},
    {{"运气"}, CHAR_P_STRING_LUCK},
    {{"地"}, CHAR_P_STRING_EARTH},
    {{"水"}, CHAR_P_STRING_WATER},
    {{"火"}, CHAR_P_STRING_FIRE},
    {{"风"}, CHAR_P_STRING_WIND},
    {{"石币"}, CHAR_P_STRING_GOLD},
    {{"标题"}, CHAR_P_STRING_TITLE},
    {{"DP"}, CHAR_P_STRING_DUELPOINT},
    {{"转数"}, CHAR_P_STRING_TRANSMIGRATION},
    {{"名字"}, CHAR_P_STRING_NAME},
    {{"昵称"}, CHAR_P_STRING_OWNTITLE},
    {{"骑宠"}, CHAR_P_STRING_RIDEPET},
    {{"学骑"}, CHAR_P_STRING_LEARNRIDE},
    {{"图像"}, CHAR_P_STRING_BASEBASEIMAGENUMBER},
    {{"天行者"}, CHAR_P_STRING_SKYWALKER},
    {{"调试模式"}, CHAR_P_STRING_DEBUGMODE},
    {{"GOLD"}, CHAR_P_STRING_GOLD}};

static CharBase CharBaseFlg[] = {{{"可见"}, CHAR_ISVISIBLE},
                                 {{"透明"}, CHAR_ISTRANSPARENT},
                                 {{"飞行"}, CHAR_ISFLYING},
                                 {{"死亡"}, CHAR_ISDIE},
                                 {{"组队"}, CHAR_ISPARTY},
                                 {{"决斗"}, CHAR_ISDUEL},
                                 {{"名片"}, CHAR_ISTRADECARD},
                                 {{"交易"}, CHAR_ISTRADE}
#ifdef _CHANNEL_MODIFY
                                 ,
                                 {{"密语频道"}, CHAR_ISTELL},
                                 {{"家族频道"}, CHAR_ISFM},
                                 {{"职业频道"}, CHAR_ISOCC},
                                 {{"对话储存"}, CHAR_ISSAVE},
                                 {{"聊天室"}, CHAR_ISCHAT}
#ifdef _CHATROOMPROTOCOL
#ifdef _THE_WORLD_SEND
                                 ,
                                 {{"世界频道"}, CHAR_ISWORLD}
#endif
#endif
#endif
};

static CharBase CharBaseAction[] = {{{"站立"}, CHAR_ACTSTAND},
                                    {{"走动"}, CHAR_ACTWALK},
                                    {{"攻击"}, CHAR_ACTATTACK},
                                    {{"投掷"}, CHAR_ACTTHROW},
                                    {{"受伤"}, CHAR_ACTDAMAGE},
                                    {{"死亡"}, CHAR_ACTDEAD},
                                    {{"魔法"}, CHAR_ACTMAGIC},
                                    {{"酒捞袍"}, CHAR_ACTITEM},
                                    {{"效果"}, CHAR_ACTEFFECT},
                                    {{"下"}, CHAR_ACTDOWN},
                                    {{"坐"}, CHAR_ACTSIT},
                                    {{"挥手"}, CHAR_ACTHAND},
                                    {{"高兴"}, CHAR_ACTPLEASURE},
                                    {{"发怒"}, CHAR_ACTANGRY},
                                    {{"悲哀"}, CHAR_ACTSAD},
                                    {{"防守"}, CHAR_ACTGUARD},
                                    {{"走路"}, CHAR_ACTACTIONWALK},
                                    {{"点头"}, CHAR_ACTNOD},
                                    {{"站立"}, CHAR_ACTACTIONSTAND},
                                    {{"战斗"}, CHAR_ACTBATTLE},
                                    {{"带队"}, CHAR_ACTLEADER},
                                    {{"观战"}, CHAR_ACTBATTLEWATCH},
                                    {{"未知"}, CHAR_ACTPOPUPNAME},
                                    {{"旋转"}, CHAR_ACTTURN},
                                    {{"变形"}, CHAR_ACTWARP},
                                    {{"交易"}, CHAR_ACTTRADE}
#ifdef _ANGEL_SUMMON
                                    ,
                                    {{"天使"}, CHAR_ACTANGEL}
#endif

#ifdef _MIND_ICON
                                    ,
                                    {{"心思"}, CHAR_MIND}
#endif
#ifdef _STREET_VENDOR
                                    ,
                                    {{"打开摊滩"}, CHAR_STREETVENDOR_OPEN},
                                    {{"关闭摊滩"}, CHAR_STREETVENDOR_CLOSE}
#endif
};

static int getCharNum(lua_State *L) {
  lua_pushinteger(L, CHAR_getCharNum());

  return 1;
}

static int getPlayerMaxNum(lua_State *L) {
  lua_pushinteger(L, CHAR_getPlayerMaxNum());

  return 1;
}

static int getPetMaxNum(lua_State *L) {
  lua_pushinteger(L, CHAR_getPetMaxNum());

  return 1;
}

static int getOthersMaxNum(lua_State *L) {
  lua_pushinteger(L, CHAR_getOthersMaxNum());

  return 1;
}

static int check(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  lua_pushinteger(L, CHAR_CHECKINDEX(index));

  return 1;
}

static int setFlg(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int element =
      getCharBaseValue(L, 2, CharBaseFlg, arraysizeof(CharBaseFlg), 1);
  const int data = luaL_checkint(L, 3);
  if (CHAR_setFlg(index, element, data) == -1) {
    return 0;
  } else {
    return 1;
  }
}

static int getFlg(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int element =
      getCharBaseValue(L, 2, CharBaseFlg, arraysizeof(CharBaseFlg), 1);

  lua_pushinteger(L, CHAR_getFlg(index, element));
  return 1;
}

static int setWorkChar(lua_State *L) {
  size_t l;
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseWorkChar,
                                       arraysizeof(CharBaseWorkChar), 1);
  char *data = luaL_checklstring(L, 3, &l);
  if (CHAR_setWorkChar(index, element, data) == -1) {
    return 0;
  } else {
    return 1;
  }
}

static int getWorkChar(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseWorkChar,
                                       arraysizeof(CharBaseWorkChar), 1);
  lua_pushstring(L, CHAR_getWorkChar(index, element));
  return 1;
}

static int setWorkInt(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int element =
      getCharBaseValue(L, 2, CharBaseWorkInt, arraysizeof(CharBaseWorkInt), 1);
  const int data = getCharBaseValue(L, 3, CharBaseWorkValue,
                                    arraysizeof(CharBaseWorkValue), 1);

#ifdef _NB_NOBATTLEBUTTON
  if (element == CHAR_WORK_NOHELP) {
    lssproto_BattleButton_send(getfdFromCharaIndex(index),
                               CHAR_getWorkInt(index, CHAR_WORK_NOHELP),
                               CHAR_getWorkInt(index, CHAR_WORK_NOESCAPE));
  }
  if (element == CHAR_WORK_NOESCAPE) {
    lssproto_BattleButton_send(getfdFromCharaIndex(index),
                               CHAR_getWorkInt(index, CHAR_WORK_NOHELP),
                               CHAR_getWorkInt(index, CHAR_WORK_NOESCAPE));
  }
#endif
  if (CHAR_setWorkInt(index, element, data) == -1) {
    return 0;
  } else {
    return 1;
  }
}

static int getWorkInt(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int element =
      getCharBaseValue(L, 2, CharBaseWorkInt, arraysizeof(CharBaseWorkInt), 1);
  lua_pushinteger(L, CHAR_getWorkInt(index, element));
  return 1;
}

static int setChar(lua_State *L) {
  size_t l;
  const int index = luaL_checkint(L, 1);
  const int element =
      getCharBaseValue(L, 2, CharBaseChar, arraysizeof(CharBaseChar), 1);
  char *data = luaL_checklstring(L, 3, &l);
  if (CHAR_setChar(index, element, data) == -1) {
    return 0;
  } else {
    return 1;
  }
}

static int getData(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  int element = -1;
  element = getCharBaseValue(L, 2, CharBaseInt, arraysizeof(CharBaseInt), 0);
  if (element >= 0) {
    lua_pushinteger(L, CHAR_getInt(index, element));
    return 1;
  }

  element = getCharBaseValue(L, 2, CharBaseChar, arraysizeof(CharBaseChar), 0);
  if (element >= 0) {
    lua_pushstring(L, CHAR_getChar(index, element));
    return 1;
  }

  element = getCharBaseValue(L, 2, CharBaseWorkChar,
                             arraysizeof(CharBaseWorkChar), 0);
  if (element >= 0) {
    lua_pushstring(L, CHAR_getWorkChar(index, element));
    return 1;
  }

  element =
      getCharBaseValue(L, 2, CharBaseWorkInt, arraysizeof(CharBaseWorkInt), 0);
  if (element >= 0) {
    lua_pushinteger(L, CHAR_getWorkInt(index, element));
    return 1;
  }
  return 1;
}

static int getChar(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int element =
      getCharBaseValue(L, 2, CharBaseChar, arraysizeof(CharBaseChar), 1);
  lua_pushstring(L, CHAR_getChar(index, element));
  return 1;
}

static int setInt(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int element =
      getCharBaseValue(L, 2, CharBaseInt, arraysizeof(CharBaseInt), 1);
  const int data =
      getCharBaseValue(L, 3, CharBaseValue, arraysizeof(CharBaseValue), 1);
  if (CHAR_setInt(index, element, data) == -1) {
    return 0;
  } else {
    return 1;
  }
}

static int getInt(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int element =
      getCharBaseValue(L, 2, CharBaseInt, arraysizeof(CharBaseInt), 1);
  lua_pushinteger(L, CHAR_getInt(index, element));
  return 1;
}

static int setFunctionPointer(lua_State *L) {
  size_t l;
  const int index = luaL_checkint(L, 1);
  const int functype =
      getCharBaseValue(L, 2, CharBaseEvent, arraysizeof(CharBaseEvent), 1);
  char *luafunctable = luaL_checklstring(L, 3, &l);
  char *luafunctablepath = luaL_checklstring(L, 4, &l);
  if (strlen(luafunctablepath) > 0) {
    MY_Lua *mylua = &MYLua;
    while (mylua->next != NULL) {
      if (strcmp(mylua->luapath, luafunctablepath) == 0) {
        return CHAR_setLUAFunction(index, functype, mylua->lua, luafunctable);
      }
      mylua = mylua->next;
    }
  } else {
    return CHAR_setLUAFunction(index, functype, L, luafunctable);
  }
  return 1;
}

static int delFunctionPointer(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int functype =
      getCharBaseValue(L, 2, CharBaseEvent, arraysizeof(CharBaseEvent), 1);

  return CHAR_delLUAFunction(index, functype);
}

static int TalkToCli(lua_State *L) {
  size_t l;
  const int talkedcharaindex = luaL_checkint(L, 1);
  const int talkcharaindex = luaL_checkint(L, 2);
  char *message = luaL_checklstring(L, 3, &l);
  const int color =
      getCharBaseValue(L, 4, CharBaseColor, arraysizeof(CharBaseColor), 1);
  CHAR_talkToCli(talkedcharaindex, talkcharaindex, message, color);
  return 1;
}

static int TalkToRound(lua_State *L) {
  size_t l;
  const int talkedcharaindex = luaL_checkint(L, 1);
  char *message = luaL_checklstring(L, 2, &l);
  const int color =
      getCharBaseValue(L, 3, CharBaseColor, arraysizeof(CharBaseColor), 1);

  int fl = CHAR_getInt(talkedcharaindex, CHAR_FLOOR);
  int x = CHAR_getInt(talkedcharaindex, CHAR_X);
  int y = CHAR_getInt(talkedcharaindex, CHAR_Y);

  int i, j;

  int range = 8;

  for (i = x - range; i <= x + range; i++) {
    for (j = y - range; j <= y + range; j++) {
      OBJECT object;
      for (object = MAP_getTopObj(fl, i, j); object;
           object = NEXT_OBJECT(object)) {
        int objindex = GET_OBJINDEX(object);
        int toindex = OBJECT_getIndex(objindex);
        if (OBJECT_getType(objindex) == OBJTYPE_CHARA &&
            toindex != talkedcharaindex) {
          if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
              CHAR_getWorkInt(toindex, CHAR_WORKBATTLEINDEX) < 0) {
            typedef void (*TALKF)(int, int, char *, int, int);
            TALKF talkedfunc = NULL;
            talkedfunc =
                (TALKF)CHAR_getFunctionPointer(toindex, CHAR_TALKEDFUNC);
            if (talkedfunc) {
              talkedfunc(toindex, talkedcharaindex, message, color, -1);
            }
          }
        }
      }
    }
  }

  return 1;
}

static int WalkPoint(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  if (CHAR_getWorkInt(index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return 0;

  const int x = luaL_checkint(L, 2);
  const int y = luaL_checkint(L, 3);

  POINT start, end;

  start.x = CHAR_getInt(index, CHAR_X);
  start.y = CHAR_getInt(index, CHAR_Y);

  end.x = x;
  end.y = y;

  int pos = NPC_Util_getDirFromTwoPoint(&start, &end);
  if (pos != -1) {
    CHAR_walk(index, pos, 0);
  }
  return 1;
}

static int Walk(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  if (CHAR_getWorkInt(index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return 0;

  const int dir = luaL_checkint(L, 2);
  const int step = luaL_checkint(L, 3);

  POINT start, end;

  start.x = CHAR_getInt(index, CHAR_X);
  start.y = CHAR_getInt(index, CHAR_Y);

  end.x = start.x;
  end.y = start.y;
  switch (dir) {
  case 0: {
    end.x = start.x - step;
    break;
  }
  case 1: {
    end.x = start.x + step;
    end.y = start.y - step;
    break;
  }
  case 2: {
    end.x = start.x + step;
    break;
  }
  case 3: {
    end.x = start.x + step;
    end.y = start.y + step;

    break;
  }
  case 4: {
    end.y = start.y + step;
    break;
  }
  case 5: {
    end.x = start.x - step;
    end.y = start.y + step;
    break;
  }
  case 6: {
    end.y = start.y - step;
    break;
  }
  case 7: {
    end.x = start.x - step;
    end.y = start.y - step;
    break;
  }
  }

  int pos = NPC_Util_getDirFromTwoPoint(&start, &end);
  if (dir != -1) {
    CHAR_walk(index, pos, 0);
  }
  return 1;
}

static int BoundRandWalk(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  if (CHAR_getWorkInt(index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return 0;

  const int minX = min(luaL_checkint(L, 2), luaL_checkint(L, 4));
  const int minY = min(luaL_checkint(L, 3), luaL_checkint(L, 5));
  const int maxX = max(luaL_checkint(L, 2), luaL_checkint(L, 4));
  const int maxY = max(luaL_checkint(L, 3), luaL_checkint(L, 5));

  POINT start, end;
  int stepX = (rand() % 10) - 5;
  int stepY = (rand() % 10) - 5;

  start.x = CHAR_getInt(index, CHAR_X);
  start.y = CHAR_getInt(index, CHAR_Y);

  end.x = start.x + stepX;
  if (end.x > maxX)
    end.x = maxX;
  else if (end.x < minX)
    end.x = minX;

  end.y = start.y + stepY;
  if (end.y > maxY)
    end.y = maxY;
  else if (end.y < minY)
    end.y = minY;

  int dir = NPC_Util_getDirFromTwoPoint(&start, &end);

  if (dir != -1) {
    CHAR_walk(index, dir, 0);
  }
  return 1;
}

static int RandRandWalk(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  if (CHAR_getWorkInt(index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return 0;

  POINT start, end;
  int stepX = (rand() % 8) - 4;
  int stepY = (rand() % 8) - 4;

  start.x = CHAR_getInt(index, CHAR_X);
  start.y = CHAR_getInt(index, CHAR_Y);

  end.x = start.x + stepX;
  end.y = start.y + stepY;

  int dir = NPC_Util_getDirFromTwoPoint(&start, &end);
  if (dir != -1) {
    CHAR_walk(index, dir, 0);
  }
  return 1;
}

static int ToAroundChar(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  int objindex = CHAR_getWorkInt(index, CHAR_WORKOBJINDEX);
  CHAR_sendCToArroundCharacter(objindex);
  return 1;
}

static int talkToFloor(lua_State *L) {
  size_t l;
  const int floor = luaL_checkint(L, 1);
  const int talkindex = luaL_checkint(L, 2);
  char *message = luaL_checklstring(L, 3, &l);
  const int color =
      getCharBaseValue(L, 4, CharBaseColor, arraysizeof(CharBaseColor), 1);
  CHAR_talkToFloor(floor, talkindex, message, color);
  return 1;
}

static int talkToParty(lua_State *L) {
  size_t l;
  const int talkedcharaindex = luaL_checkint(L, 1);
  const int talkcharaindex = luaL_checkint(L, 2);
  char *message = luaL_checklstring(L, 3, &l);
  const int color =
      getCharBaseValue(L, 4, CharBaseColor, arraysizeof(CharBaseColor), 1);
  CHAR_talkToCliAndParty(talkedcharaindex, talkcharaindex, message, color);
  return 1;
}

static int talkToServer(lua_State *L) {
  size_t l;
  const int talkcharaindex = luaL_checkint(L, 1);
  char *message = luaL_checklstring(L, 2, &l);
  const int color =
      getCharBaseValue(L, 3, CharBaseColor, arraysizeof(CharBaseColor), 1);

  CHAR_talkToAll(talkcharaindex, message, color);
  return 1;
}
#ifdef _ALLBLUES_LUA_1_1
static int WarpToSpecificPoint(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int floor = luaL_checkint(L, 2);
  const int x = luaL_checkint(L, 3);
  const int y = luaL_checkint(L, 4);

  lua_pushinteger(L, CHAR_warpToSpecificPoint(charaindex, floor, x, y));
  return 1;
}
static int MapAllWarp(lua_State *L) {
  const int oldfloor = luaL_checkint(L, 1);
  const int newfloor = luaL_checkint(L, 2);
  const int x = luaL_checkint(L, 3);
  const int y = luaL_checkint(L, 4);

  int i;
  int playernum = CHAR_getPlayerMaxNum();

  for (i = 0; i < playernum; i++) {
    if (CHAR_getCharUse(i) == FALSE)
      continue;
    if (CHAR_getInt(i, CHAR_FLOOR) != oldfloor)
      continue;
    CHAR_warpToSpecificPoint(i, newfloor, x, y);
  }
  return 1;
}
#endif

#ifdef _ALLBLUES_LUA_1_3
static int createPet(lua_State *L) {
  const int enemyid = luaL_checkint(L, 1);
  const int UpLevel = luaL_checkint(L, 2);

  int i;
  for (i = 0; i < ENEMY_getEnemyNum(); i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == enemyid) {
      break;
    }
  }
  if (i == ENEMY_getEnemyNum()) {
    lua_pushinteger(L, -1);
    return 1;
  }

  int petindex = ENEMY_createPet(i, -1, -1, -1, -1);

  for (i = 0; i < UpLevel; i++) {
    CHAR_PetLevelUp(petindex);
    CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
  }
  CHAR_setInt(petindex, CHAR_LV, CHAR_getInt(petindex, CHAR_LV) + UpLevel);
  CHAR_complianceParameter(petindex);
  CHAR_setInt(petindex, CHAR_HP, CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));

  CHAR_LoginBesideSetWorkInt(petindex, -1);
  lua_pushinteger(L, petindex);

  return 1;
}

static int setCharPet(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int petindex = luaL_checkint(L, 2);

  int havepetelement = CHAR_getCharPetElement(index);
  if (havepetelement > -1) {
    CHAR_setCharPet(index, havepetelement, petindex);
    CHAR_setWorkInt(petindex, CHAR_WORKPLAYERINDEX, index);
  }

  lua_pushinteger(L, havepetelement);

  return 1;
}

static int setCharPetNew(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int pethaveid = luaL_checkint(L, 2);
  const int petindex = luaL_checkint(L, 3);

  CHAR_setCharPet(index, pethaveid, petindex);

  lua_pushinteger(L, pethaveid);

  return 1;
}

static int getCharPet(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int havepetid = luaL_checkint(L, 2);

  lua_pushinteger(L, CHAR_getCharPet(index, havepetid));

  return 1;
}

static int setCharPoolPet(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int petindex = luaL_checkint(L, 2);

  int havepetelement = CHAR_getCharPoolPetElement(index);
  if (havepetelement > -1) {
    CHAR_setCharPoolPet(index, havepetelement, petindex);
    CHAR_setWorkInt(petindex, CHAR_WORKPLAYERINDEX, index);
  }

  lua_pushinteger(L, havepetelement);

  return 1;
}

static int getCharPoolPet(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int havepetid = luaL_checkint(L, 2);

  lua_pushinteger(L, CHAR_getCharPoolPet(index, havepetid));

  return 1;
}

static int setDepotPetIndex(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int petindex = luaL_checkint(L, 2);

  int havepetelement = CHAR_getCharPoolPetElement(index);
  if (havepetelement > -1) {
    CHAR_setDepotPetIndex(index, havepetelement, petindex);
    CHAR_setWorkInt(petindex, CHAR_WORKPLAYERINDEX, index);
  }

  lua_pushinteger(L, havepetelement);

  return 1;
}

static int getDepotPetIndex(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int havepetid = luaL_checkint(L, 2);

  lua_pushinteger(L, CHAR_getDepotPetIndex(index, havepetid));

  return 1;
}

static int DelItem(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int itemindex = luaL_checkint(L, 2);

  CHAR_DelItem(charaindex, itemindex);

  return 1;
}

static int DelItemForIndex(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int itemindex = luaL_checkint(L, 2);
  int i;
  int tmindex = -1;
  int id;
  for (i = CHAR_STARTITEMARRAY; i < CHAR_getMaxInventory(charaindex); i++) {
    tmindex = CHAR_getItemIndex(charaindex, i);
    if (ITEM_CHECKINDEX(tmindex)) {
      if (tmindex == itemindex) {
        CHAR_DelItem(charaindex, i);
        lua_pushinteger(L, 1);
        return 1;
      }
    }
  }
  lua_pushinteger(L, -1);
  return 1;
}

static int getFd(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);

  lua_pushinteger(L, getfdFromCharaIndex(charaindex));
  return 1;
}

static int Updata(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int indextable =
      getCharBaseValue(L, 2, CharBaseUpdata, arraysizeof(CharBaseUpdata), 1);

  CHAR_send_P_StatusString(charaindex, indextable);
  return 1;
}

static int Additem(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int itemid = luaL_checkint(L, 2);
  int itemindex = -1;
  int emptyitemindexinchara = CHAR_findEmptyItemBox(charaindex);

  if (emptyitemindexinchara >= 0) {
    itemindex = ITEM_makeItemAndRegist(itemid);

    if (itemindex > -1) {
      CHAR_setItemIndex(charaindex, emptyitemindexinchara, itemindex);
      ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
      ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, charaindex);
      CHAR_sendItemDataOne(charaindex, emptyitemindexinchara);
      LogItem(CHAR_getChar(charaindex, CHAR_NAME),
              CHAR_getChar(charaindex, CHAR_CDKEY), itemindex, "LUA制作道具",
              CHAR_getInt(charaindex, CHAR_FLOOR),
              CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
              ITEM_getChar(itemindex, ITEM_UNIQUECODE),
              ITEM_getChar(itemindex, ITEM_NAME),
              ITEM_getInt(itemindex, ITEM_ID), charaindex);
    }
  }
  lua_pushinteger(L, itemindex);
  return 1;
}

static int FinditemHave(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int itemid = luaL_checkint(L, 2);
  int i;
  int itemindex = -1;
  int id;
  for (i = CHAR_STARTITEMARRAY; i < CHAR_getMaxInventory(charaindex); i++) {
    itemindex = CHAR_getItemIndex(charaindex, i);
    if (ITEM_CHECKINDEX(itemindex)) {
      id = ITEM_getInt(itemindex, ITEM_ID);
      if (id == itemid) {
        lua_pushinteger(L, i);
        return 1;
      }
    }
  }
  lua_pushinteger(L, -1);
  return 1;
}

static int Finditem(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int itemid = luaL_checkint(L, 2);
  int i;
  int itemindex = -1;
  int id;
  for (i = CHAR_STARTITEMARRAY; i < CHAR_getMaxInventory(charaindex); i++) {
    itemindex = CHAR_getItemIndex(charaindex, i);
    if (ITEM_CHECKINDEX(itemindex)) {
      id = ITEM_getInt(itemindex, ITEM_ID);
      if (id == itemid) {
        lua_pushinteger(L, itemindex);
        return 1;
      }
    }
  }
  lua_pushinteger(L, -1);
  return 1;
}

static int AddPet(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int enemyid = luaL_checkint(L, 2);
  const int UpLevel = luaL_checkint(L, 3);

  int i, havepetelement;
  for (i = 0; i < ENEMY_getEnemyNum(); i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == enemyid) {
      break;
    }
  }
  if (i == ENEMY_getEnemyNum()) {
    lua_pushinteger(L, -1);
    return 1;
  }

  for (havepetelement = 0; havepetelement < CHAR_MAXPETHAVE; havepetelement++) {
    int petindex = CHAR_getCharPet(index, havepetelement);
    if (!CHAR_CHECKINDEX(petindex))
      break;
  }

  if (havepetelement == CHAR_MAXPETHAVE) {
    lua_pushinteger(L, -1);
    return 1;
  }

  int petindex = ENEMY_createPet(i, -1, -1, -1, -1);
  if (!CHAR_CHECKINDEX(petindex)) {
    lua_pushinteger(L, -1);
    return 1;
  }

  for (i = CHAR_getInt(petindex, CHAR_LV); i < UpLevel; i++) {
    CHAR_PetLevelUp(petindex);
    CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
    CHAR_setInt(petindex, CHAR_LV, CHAR_getInt(petindex, CHAR_LV) + 1);
  }

  CHAR_setCharPet(index, havepetelement, petindex);

  CHAR_setWorkInt(petindex, CHAR_WORKPLAYERINDEX, index);

  char token[64];
  CHAR_setInt(petindex, CHAR_VARIABLEAI, 10000);
  CHAR_complianceParameter(petindex);
  CHAR_setInt(petindex, CHAR_HP, CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));
  if (CHAR_getInt(index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    CHAR_setChar(petindex, CHAR_OWNERCDKEY, CHAR_getChar(index, CHAR_CDKEY));
    CHAR_setChar(petindex, CHAR_OWNERCHARANAME, CHAR_getChar(index, CHAR_NAME));
  }
  snprintf(token, sizeof(token), "K%d", havepetelement);
  CHAR_sendStatusString(index, token);
  snprintf(token, sizeof(token), "W%d", havepetelement);
  CHAR_sendStatusString(index, token);

  LogPet(CHAR_getChar(index, CHAR_NAME), CHAR_getChar(index, CHAR_CDKEY),
         CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
         "LUA的char.AddPet", CHAR_getInt(index, CHAR_FLOOR),
         CHAR_getInt(index, CHAR_X), CHAR_getInt(index, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE), petindex, index);

  lua_pushinteger(L, petindex);

  return 1;
}

static int AddPetCf(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int enemyid = luaL_checkint(L, 2);
  const int UpLevel = luaL_checkint(L, 3);
  const int trn = luaL_checkint(L, 4);
  const int vital = luaL_checkint(L, 5);
  const int str = luaL_checkint(L, 6);
  const int tgh = luaL_checkint(L, 7);
  const int dex = luaL_checkint(L, 8);

  int i, havepetelement;
  for (i = 0; i < ENEMY_getEnemyNum(); i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == enemyid) {
      break;
    }
  }
  if (i == ENEMY_getEnemyNum()) {
    lua_pushinteger(L, -1);
    return 1;
  }

  for (havepetelement = 0; havepetelement < CHAR_MAXPETHAVE; havepetelement++) {
    int petindex = CHAR_getCharPet(index, havepetelement);
    if (!CHAR_CHECKINDEX(petindex))
      break;
  }

  if (havepetelement == CHAR_MAXPETHAVE) {
    lua_pushinteger(L, -1);
    return 1;
  }

  int petindex = ENEMY_createPet(i, vital, str, tgh, dex);
  if (!CHAR_CHECKINDEX(petindex)) {
    lua_pushinteger(L, -1);
    return 1;
  }

  for (i = CHAR_getInt(petindex, CHAR_LV); i < UpLevel; i++) {
    CHAR_PetLevelUp(petindex);
    CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
    CHAR_setInt(petindex, CHAR_LV, CHAR_getInt(petindex, CHAR_LV) + 1);
  }
  CHAR_setInt(petindex, CHAR_TRANSMIGRATION, trn);

  CHAR_setCharPet(index, havepetelement, petindex);

  //	CHAR_setChar( petindex, CHAR_OWNERCDKEY, CHAR_getChar( index,
  //CHAR_CDKEY)); 	CHAR_setChar( petindex, CHAR_OWNERCHARANAME, CHAR_getChar(
  //index, CHAR_NAME));

  CHAR_setWorkInt(petindex, CHAR_WORKPLAYERINDEX, index);

  char token[64];

  CHAR_setInt(petindex, CHAR_VARIABLEAI, 10000);
  CHAR_complianceParameter(petindex);

  CHAR_setInt(petindex, CHAR_HP, CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));

  snprintf(token, sizeof(token), "K%d", havepetelement);
  CHAR_sendStatusString(index, token);
  snprintf(token, sizeof(token), "W%d", havepetelement);
  CHAR_sendStatusString(index, token);

  LogPet(CHAR_getChar(index, CHAR_NAME), CHAR_getChar(index, CHAR_CDKEY),
         CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
         "LUA的char.AddPetCf", CHAR_getInt(index, CHAR_FLOOR),
         CHAR_getInt(index, CHAR_X), CHAR_getInt(index, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE), petindex, index);

  lua_pushinteger(L, petindex);

  return 1;
}

static int AddPetCf2(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int enemyid = luaL_checkint(L, 2);
  const int UpLevel = luaL_checkint(L, 3);
  const int trn = luaL_checkint(L, 4);
  const int vital = luaL_checkint(L, 5);
  const int str = luaL_checkint(L, 6);
  const int tgh = luaL_checkint(L, 7);
  const int dex = luaL_checkint(L, 8);

  int i, havepetelement;
  for (i = 0; i < ENEMY_getEnemyNum(); i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == enemyid) {
      break;
    }
  }
  if (i == ENEMY_getEnemyNum()) {
    lua_pushinteger(L, -1);
    return 1;
  }

  for (havepetelement = 0; havepetelement < CHAR_MAXPETHAVE; havepetelement++) {
    int petindex = CHAR_getCharPet(index, havepetelement);
    if (!CHAR_CHECKINDEX(petindex))
      break;
  }

  if (havepetelement == CHAR_MAXPETHAVE) {
    lua_pushinteger(L, -1);
    return 1;
  }

  int petindex = ENEMY_createPet2(i, vital, str, tgh, dex);
  if (!CHAR_CHECKINDEX(petindex)) {
    lua_pushinteger(L, -1);
    return 1;
  }

  for (i = CHAR_getInt(petindex, CHAR_LV); i < UpLevel; i++) {
    CHAR_PetLevelUp(petindex);
    CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
    CHAR_setInt(petindex, CHAR_LV, CHAR_getInt(petindex, CHAR_LV) + 1);
  }
  CHAR_setInt(petindex, CHAR_TRANSMIGRATION, trn);

  CHAR_setCharPet(index, havepetelement, petindex);

  //	CHAR_setChar( petindex, CHAR_OWNERCDKEY, CHAR_getChar( index,
  //CHAR_CDKEY)); 	CHAR_setChar( petindex, CHAR_OWNERCHARANAME, CHAR_getChar(
  //index, CHAR_NAME));

  CHAR_setWorkInt(petindex, CHAR_WORKPLAYERINDEX, index);

  char token[64];

  CHAR_setInt(petindex, CHAR_VARIABLEAI, 10000);
  CHAR_complianceParameter(petindex);

  CHAR_setInt(petindex, CHAR_HP, CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));

  snprintf(token, sizeof(token), "K%d", havepetelement);
  CHAR_sendStatusString(index, token);
  snprintf(token, sizeof(token), "W%d", havepetelement);
  CHAR_sendStatusString(index, token);

  LogPet(CHAR_getChar(index, CHAR_NAME), CHAR_getChar(index, CHAR_CDKEY),
         CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
         "LUA的char.AddPetCf2", CHAR_getInt(index, CHAR_FLOOR),
         CHAR_getInt(index, CHAR_X), CHAR_getInt(index, CHAR_Y),
         CHAR_getChar(petindex, CHAR_UNIQUECODE), petindex, index);

  lua_pushinteger(L, petindex);

  return 1;
}

#ifdef _PLAYER_NPC
static int setPlayerNpc(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int petindex = luaL_checkint(L, 2);
  int i;
  for (i = 1; i < CHAR_PARTYMAX; i++) {
    if (CHAR_getWorkInt(charaindex, i + CHAR_WORKPARTYINDEX1) == -1) {
      break;
    }
  }
  if (i == CHAR_PARTYMAX) {
    CHAR_endCharOneArray(petindex);
  } else {
    if (CHAR_CHECKINDEX(petindex) == TRUE) {
      Object object;
      int objindex;

      CHAR_setInt(petindex, CHAR_FLOOR, CHAR_getInt(charaindex, CHAR_FLOOR));
      CHAR_setInt(petindex, CHAR_X, CHAR_getInt(charaindex, CHAR_X));
      CHAR_setInt(petindex, CHAR_Y, CHAR_getInt(charaindex, CHAR_Y));

      object.type = OBJTYPE_CHARA;
      object.index = petindex;
      object.x = CHAR_getInt(petindex, CHAR_X);
      object.y = CHAR_getInt(petindex, CHAR_Y);
      object.floor = CHAR_getInt(petindex, CHAR_FLOOR);

      objindex = initObjectOne(&object);

      if (objindex == -1) {
        CHAR_endCharOneArray(petindex);
      } else {
        CHAR_setWorkInt(petindex, CHAR_WORKOBJINDEX, objindex);
        if (CHAR_JoinParty_Main(petindex, charaindex) == FALSE) {
          CHAR_CharaDelete(petindex);
          return 1;
        }

        CHAR_sendCToArroundCharacter(objindex);
      }
    }
  }
  return 1;
}
#endif
#endif
#ifdef _PETSKILL_SHOP_LUA
static int FreePetSkill(lua_State *L) {
  const int petindex = luaL_checkint(L, 1);
  const int petskill = luaL_checkint(L, 2);

  int i;
  for (i = 0; i < CHAR_MAXPETSKILLHAVE; i++) {
    if (petskill == CHAR_getPetSkill(petindex, i)) {
      lua_pushinteger(L, 1);
      return 1;
    }
  }
  lua_pushinteger(L, 0);
  return 1;
}
#endif

static int Coordinate(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  if (CHAR_getWorkInt(index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
    return 0;

  const int X = luaL_checkint(L, 2);
  const int Y = luaL_checkint(L, 3);

  POINT start, end;

  start.x = CHAR_getInt(index, CHAR_X);
  start.y = CHAR_getInt(index, CHAR_Y);

  end.x = X;
  end.y = Y;

  int dir = NPC_Util_getDirFromTwoPoint(&start, &end);

  if (dir != -1) {
    CHAR_walk(index, dir, 0);
  }
  return 1;
}

static int setPetSkill(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int havepetskillindex = luaL_checkint(L, 2);
  const int skillID = luaL_checkint(L, 3);

  CHAR_setPetSkill(index, havepetskillindex, skillID);

  return 1;
}

static int getPetSkill(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int havepetskillindex = luaL_checkint(L, 2);

  lua_pushinteger(L, CHAR_getPetSkill(index, havepetskillindex));

  return 1;
}

static int PetLevelUp(lua_State *L) {
  const int index = luaL_checkint(L, 1);

  CHAR_PetLevelUp(index);

  return 1;
}

static int PetLevelUpNoFame(lua_State *L) {
  const int index = luaL_checkint(L, 1);

  CHAR_PetLevelUpNoFame(index);

  return 1;
}

static int JoinParty(lua_State *L) {
  const int index1 = luaL_checkint(L, 1);
  const int index2 = luaL_checkint(L, 2);

  lua_pushinteger(L, CHAR_JoinParty_Main(index2, index1));

  return 1;
}

#ifdef _ALLBLUES_LUA_1_4
static int getRightTo8(lua_State *L) {
  const int value = luaL_checkint(L, 1);
  const int flg = luaL_checkint(L, 2);

  lua_pushinteger(L, (value >> (4 - flg) * 8) & 0xFF);

  return 1;
}

static int getLiftTo8(lua_State *L) {
  const int value = luaL_checkint(L, 1);
  const int flg = luaL_checkint(L, 2);

  lua_pushinteger(L, (value << (4 - flg) * 8));

  return 1;
}

static int complianceParameter(lua_State *L) {
  const int index = luaL_checkint(L, 1);

  CHAR_complianceParameter(index);

  return 1;
}

static int sendStatusString(lua_State *L) {
  size_t l;
  const int index = luaL_checkint(L, 1);
  char *data = luaL_checklstring(L, 2, &l);

  CHAR_sendStatusString(index, data);

  return 1;
}

static int sendBattleEffect(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int val = luaL_checkint(L, 2);

  CHAR_sendBattleEffect(index, val);

  return 1;
}

static int sendWatchEvent(lua_State *L) {
  const int objindex = luaL_checkint(L, 1);
  const int action =
      getCharBaseValue(L, 2, CharBaseAction, arraysizeof(CharBaseAction), 1);
  const int flg = luaL_checkint(L, 3);

  luaL_checktype(L, 4, LUA_TTABLE);
  int n = luaL_getn(L, 4);
  int *opt = (int *)malloc(n * sizeof(int));
  int i;

  for (i = 0; i < n; i++) {
    opt[i] = getArrayInt(L, i);
  }

  CHAR_sendWatchEvent(objindex, action, opt, n, flg);

  return 1;
}

static int dropPetAbsolute(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int fl = luaL_checkint(L, 2);
  const int fx = luaL_checkint(L, 3);
  const int fy = luaL_checkint(L, 4);
  const int dir = luaL_checkint(L, 5);
  const int val = luaL_checkint(L, 6);
  int dirx[9], diry[9];
  int floor, x, y, i;

  for (i = 0; i < 7; i++) {
    dirx[i + 2] = CHAR_getDX(dir + i + 1);
    diry[i + 2] = CHAR_getDY(dir + i + 1);
  }
  dirx[0] = CHAR_getDX(dir);
  diry[0] = CHAR_getDY(dir);
  dirx[1] = 0;
  diry[1] = 0;
  floor = fl;
  for (i = 0; i < 9; i++) {
    int x = fx + dirx[i];
    int y = fy + diry[i];
    if (PET_isPutPoint(floor, x, y) == TRUE) {
      break;
    }
  }
  if (i == 9)
    i = 1;
  x = fx + dirx[i];
  y = fy + diry[i];

  int objindex = PET_dropPetAbsolute(index, floor, x, y, val);

  if (objindex == -1) {
    return 1;
  }

  CHAR_setWorkInt(index, CHAR_WORKOBJINDEX, objindex);
  CHAR_setInt(index, CHAR_FLOOR, floor);
  CHAR_setInt(index, CHAR_X, x);
  CHAR_setInt(index, CHAR_Y, y);
  CHAR_setInt(index, CHAR_PUTPETTIME, NowTime.tv_sec);

  CHAR_sendCToArroundCharacter(objindex);

  return 1;
}

static int AllWarpToSpecificPoint(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int floor = luaL_checkint(L, 2);
  const int x = luaL_checkint(L, 3);
  const int y = luaL_checkint(L, 4);

  if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER) {
    int i;
    for (i = 0; i < CHAR_PARTYMAX; i++) {
      int subindex = CHAR_getWorkInt(charaindex, CHAR_WORKPARTYINDEX1 + i);
      if (CHAR_CHECKINDEX(subindex) == FALSE)
        continue;
      CHAR_warpToSpecificPoint(subindex, floor, x, y);
      ITEM_WarpDelErrorItem(subindex);
    }
  } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
             CHAR_PARTY_CLIENT) {
    CHAR_talkToCli(charaindex, -1, "队员无法使用。", CHAR_COLORYELLOW);
    return FALSE;
  } else if (CHAR_getWorkInt(charaindex, CHAR_WORKPARTYMODE) ==
             CHAR_PARTY_NONE) {
    ITEM_WarpDelErrorItem(charaindex);
    CHAR_warpToSpecificPoint(charaindex, floor, x, y);
  }

  return 1;
}

static int Findpet(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int petid = luaL_checkint(L, 2);
  const int lv = luaL_checkint(L, 3);
  int i;

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    int petindex = CHAR_getCharPet(charaindex, i);
    if (!CHAR_CHECKINDEX(petindex))
      continue;
    if (CHAR_getInt(petindex, CHAR_PETID) == petid) {
      if (lv > 0) {
        if (CHAR_getInt(petindex, CHAR_LV) != lv) {
          continue;
        }
      }
      lua_pushinteger(L, petindex);
      return 1;
    }
  }

  lua_pushinteger(L, -1);
  return 1;
}

static int FindPetFormMatemo(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int metamo = luaL_checkint(L, 2);
  const int lv = luaL_checkint(L, 3);
  int i;

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    int petindex = CHAR_getCharPet(charaindex, i);
    if (!CHAR_CHECKINDEX(petindex))
      continue;
    if (CHAR_getInt(petindex, CHAR_BASEBASEIMAGENUMBER) == metamo) {
      if (lv > 0) {
        if (CHAR_getInt(petindex, CHAR_LV) != lv) {
          continue;
        }
      }
      lua_pushinteger(L, petindex);
      return 1;
    }
  }

  lua_pushinteger(L, -1);
  return 1;
}

static int FindPetFormEnemyTempID(lua_State *L) {
  size_t l;
  const int charaindex = luaL_checkint(L, 1);
  char *data = luaL_checklstring(L, 2, &l);
  char token[64];
  int enemytempid = -1, lv = 0;
  int i;

  if (getStringFromIndexWithDelim(data, "-", 1, token, sizeof(token)) == TRUE) {
    lv = atoi(token);
  } else {
    lua_pushinteger(L, -1);
  }

  if (getStringFromIndexWithDelim(data, "-", 2, token, sizeof(token)) == TRUE) {
    enemytempid = atoi(token);
  } else {
    lua_pushinteger(L, -1);
  }

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    int petindex = CHAR_getCharPet(charaindex, i);
    if (!CHAR_CHECKINDEX(petindex))
      continue;

    if (CHAR_getInt(petindex, CHAR_PETID) == enemytempid) {
      if (lv > 0) {
        if (CHAR_getInt(petindex, CHAR_LV) != lv) {
          continue;
        }
      }
      lua_pushinteger(L, petindex);
      return 1;
    }
  }

  lua_pushinteger(L, -1);
  return 1;
}

static int DelPet(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int petindex = luaL_checkint(L, 2);
  int i;
  char category[12];

  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    if (petindex == CHAR_getCharPet(charaindex, i)) {
      if (CHAR_getInt(charaindex, CHAR_RIDEPET) == i) {
        CHAR_setInt(charaindex, CHAR_RIDEPET, -1);
        CHAR_send_P_StatusString(charaindex, CHAR_P_STRING_RIDEPET);
        CHAR_complianceParameter(charaindex);
        CHAR_sendCToArroundCharacter(
            CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX));
      }
      LogPet(CHAR_getChar(charaindex, CHAR_NAME), // 平乓仿
             CHAR_getChar(charaindex, CHAR_CDKEY),
             CHAR_getChar(petindex, CHAR_NAME), CHAR_getInt(petindex, CHAR_LV),
             "LUA的char.DelPet", CHAR_getInt(charaindex, CHAR_FLOOR),
             CHAR_getInt(charaindex, CHAR_X), CHAR_getInt(charaindex, CHAR_Y),
             CHAR_getChar(petindex, CHAR_UNIQUECODE), petindex, charaindex);

      CHAR_endCharOneArray(petindex);
      CHAR_setCharPet(charaindex, i, -1);
      snprintf(category, sizeof(category), "K%d", i);
      CHAR_sendStatusString(charaindex, category);
      break;
    }
  }
  return 1;
}

#endif

#ifdef _ALLBLUES_LUA_1_7
static int findEmptyItemBox(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);

  lua_pushinteger(L, CHAR_findEmptyItemBox(charaindex));
  return 1;
}

static int findEmptyPetBox(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  int num = 0;
  int i;
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    int petindex = CHAR_getCharPet(charaindex, i);
    if (!CHAR_CHECKINDEX(petindex)) {
      num++;
    }
  }
  lua_pushinteger(L, num);
  return 1;
}

static int dropPetFollow(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int havepetindex = luaL_checkint(L, 2);
  PET_dropPetFollow(charaindex, havepetindex, -1, -1, -1);
  return 1;
}

static int getItemIndex(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int haveitemindex = luaL_checkint(L, 2);
  int itemindex = CHAR_getItemIndex(charaindex, haveitemindex);
  if (ITEM_CHECKINDEX(itemindex)) {
    lua_pushinteger(L, itemindex);
  } else {
    lua_pushinteger(L, -1);
  }
  return 1;
}

static int getPoolItemIndex(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int haveitemindex = luaL_checkint(L, 2);
  int itemindex = CHAR_getPoolItemIndex(charaindex, haveitemindex);
  if (ITEM_CHECKINDEX(itemindex)) {
    lua_pushinteger(L, itemindex);
  } else {
    lua_pushinteger(L, -1);
  }
  return 1;
}

static int getDepotItemIndex(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int haveitemindex = luaL_checkint(L, 2);
  int itemindex = CHAR_getDepotItemIndex(charaindex, haveitemindex);
  if (ITEM_CHECKINDEX(itemindex)) {
    lua_pushinteger(L, itemindex);
  } else {
    lua_pushinteger(L, -1);
  }
  return 1;
}

static int charSaveFromConnect(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  if (CHAR_getCharUse(charaindex) != FALSE) {
    CHAR_charSaveFromConnect(charaindex, FALSE);
  }
  return 1;
}

#ifdef _RECORD_IP
static int userip(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);

  unsigned long ip;
  int a, b, c, d;
  char strIP[32];

  ip = CHAR_getWorkInt(charaindex, CHAR_WORK_RECORD_IP);

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
#endif

static int DischargeParty(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int flg = luaL_checkint(L, 2);
  CHAR_DischargeParty(charaindex, flg);
  return 1;
}

static int Skillupsend(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);

  CHAR_Skillupsend(charaindex);
  return 1;
}
#endif

#ifdef _ALLBLUES_LUA_1_9
static int logou(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);

  CHAR_logout(charaindex, TRUE);
  return 1;
}

static int copyChar(lua_State *L) {
  const int fromindex = luaL_checkint(L, 1);
  const int toindex = luaL_checkint(L, 2);

  if (!CHAR_CHECKINDEX(fromindex))
    return 1;
  if (!CHAR_CHECKINDEX(toindex))
    return 1;

  CHAR_setInt(toindex, CHAR_BASEIMAGENUMBER,
              CHAR_getInt(fromindex, CHAR_BASEIMAGENUMBER));
  CHAR_setInt(toindex, CHAR_BASEBASEIMAGENUMBER,
              CHAR_getInt(fromindex, CHAR_BASEBASEIMAGENUMBER));
  CHAR_setInt(toindex, CHAR_TRANSMIGRATION,
              CHAR_getInt(fromindex, CHAR_TRANSMIGRATION));
  CHAR_setInt(toindex, CHAR_LV, CHAR_getInt(fromindex, CHAR_LV));
  CHAR_setInt(toindex, CHAR_HP, CHAR_getInt(fromindex, CHAR_HP));
  CHAR_setInt(toindex, CHAR_MP, CHAR_getInt(fromindex, CHAR_MP));
  CHAR_setInt(toindex, CHAR_MAXMP, CHAR_getInt(fromindex, CHAR_MAXMP));
  CHAR_setInt(toindex, CHAR_VITAL, CHAR_getInt(fromindex, CHAR_VITAL));
  CHAR_setInt(toindex, CHAR_STR, CHAR_getInt(fromindex, CHAR_STR));
  CHAR_setInt(toindex, CHAR_TOUGH, CHAR_getInt(fromindex, CHAR_TOUGH));
  CHAR_setInt(toindex, CHAR_DEX, CHAR_getInt(fromindex, CHAR_DEX));
  CHAR_setInt(toindex, CHAR_ALLOCPOINT,
              CHAR_getInt(fromindex, CHAR_ALLOCPOINT));
  CHAR_setInt(toindex, CHAR_PETRANK, CHAR_getInt(fromindex, CHAR_PETRANK));
  CHAR_setInt(toindex, CHAR_LIMITLEVEL,
              CHAR_getInt(fromindex, CHAR_LIMITLEVEL));

  CHAR_setChar(toindex, CHAR_NAME, CHAR_getChar(fromindex, CHAR_NAME));
  CHAR_setChar(toindex, CHAR_OWNTITLE, CHAR_getChar(fromindex, CHAR_OWNTITLE));

  CHAR_complianceParameter(toindex);

  return 1;
}

#ifdef _ALL_SERV_SEND
static int talkToAllServer(lua_State *L) {
  size_t l;
  char *message = luaL_checklstring(L, 1, &l);
  const int color =
      getCharBaseValue(L, 2, CharBaseColor, arraysizeof(CharBaseColor), 1);
  saacproto_AllServSend_send(message, color);
  return 1;
}
#endif

#endif

static int earnFame(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int num = luaL_checkint(L, 2);
  CHAR_earnFame(charaindex, num);
  return 1;
}

static int Encounter(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return 0;
  int fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  setStayEncount(fd);
#ifdef _USER_CHARLOOPS
  {
    Char *ch;
    ch = CHAR_getCharPointer(charaindex);
    if (ch == NULL)
      return;
    strcpysafe(ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
               sizeof(ch->charfunctable[CHAR_LOOPFUNCTEMP1]),
               "CHAR_BattleStayLoop"); //战斗
    CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, getBattleSpeed());
    CHAR_constructFunctable(charaindex);
  }
#endif
}

static int StopEncounter(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  if (CHAR_CHECKINDEX(charaindex) == FALSE)
    return 0;
  int fd = CHAR_getWorkInt(charaindex, CHAR_WORKFD);
  if (fd != -1) {
    clearStayEncount(fd);
#ifdef _USER_CHARLOOPS
    Char *ch;
    ch = CHAR_getCharPointer(charaindex);
    if (ch == NULL)
      return;
    strcpysafe(ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
               sizeof(ch->charfunctable[CHAR_LOOPFUNCTEMP1]), ""); //战斗
    CHAR_setInt(charaindex, CHAR_LOOPINTERVAL, 0);
    CHAR_constructFunctable(charaindex);
#endif
  }
}

static int getDX(lua_State *L) {
  size_t l;
  char *diy = luaL_checklstring(L, 1, &l);

  lua_pushinteger(L, CHAR_getDX(diy));
  return 1;
}

static int getDY(lua_State *L) {
  size_t l;
  char *dir = luaL_checklstring(L, 1, &l);

  lua_pushinteger(L, CHAR_getDY(dir));
  return 1;
}

static int sendAction(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  const int actionno = luaL_checkint(L, 2);
  const int flg = luaL_checkint(L, 3);
  CHAR_sendAction(charaindex, actionno, flg);
  return 1;
}

static int GetOldLevelExp(lua_State *L) {
  const int level = luaL_checkint(L, 1);
  lua_pushinteger(L, CHAR_GetOldLevelExp(level));
  return 1;
}

#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
static int getCharSkill(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int sindex = luaL_checkint(L, 2);
  lua_pushinteger(L, CHAR_getCharSkill(index, sindex));
  return 1;
}

static int setCharSkill(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int sindex = luaL_checkint(L, 2);
  const int data = luaL_checkint(L, 3);
  if (CHAR_setCharSkill(index, sindex, data) == -1) {
    return 0;
  } else {
    return 1;
  }
}

static int DelSProfeesionSkill(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int sindex = luaL_checkint(L, 2);
  CHAR_HaveSkill *pSkil;
  pSkil = CHAR_getCharHaveSkill(index, sindex);
  SKILL_makeSkillData(&pSkil->skill, 0, 0);
  pSkil->use = 0;
}

#endif

static int DropPet(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int havepetindex = luaL_checkint(L, 2);
  PET_dropPet(index, havepetindex);
  return 1;
}

static int getMyMaxPilenum(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  lua_pushinteger(L, CHAR_getMyMaxPilenum(index));
  return 1;
}

static int PileItemFromItemBoxToItemBox(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  const int meindex = luaL_checkint(L, 2);
  const int toindex = luaL_checkint(L, 3);
  CHAR_PileItemFromItemBoxToItemBox(index, meindex, toindex);
  return 1;
}

static int GetEmptyItemNum(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(index)) {
    lua_pushinteger(L, 0);
    return 1;
  }
  int i, itemunm = 0;
  for (i = CHAR_STARTITEMARRAY; i < CHAR_getMaxInventory(index); i++) {
    int itemindex;
    itemindex = CHAR_getItemIndex(index, i);
    if (itemindex == -1) {
      itemunm++;
    }
  }
  lua_pushinteger(L, itemunm);
  return 1;
}

static int GetEmptyPetNum(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(index)) {
    lua_pushinteger(L, 0);
    return 1;
  }
  int i, petunm = 0;
  for (i = 0; i < CHAR_MAXPETHAVE; i++) {
    if (CHAR_getCharPet(index, i) == -1) {
      petunm++;
    }
  }
  lua_pushinteger(L, petunm);
  return 1;
}

static int GetGmLevel(lua_State *L) {
  size_t l;
  char *cdkey = luaL_checklstring(L, 1, &l);
  int gmlevel = 0, i;
  for (i = 0; i < GMMAXNUM; i++) {
    if (strcmp(cdkey, gminfo[i].cdkey) == 0) {
      gmlevel = gminfo[i].level;
      break;
    }
  }
  lua_pushinteger(L, gmlevel);
  return 1;
}

static int GetPetSkill(lua_State *L) {
  const int petindex = luaL_checkint(L, 1);
  const int pos = luaL_checkint(L, 2);

  if (pos < 0 || pos >= CHAR_MAXPETSKILLHAVE) {
    return -1;
  }

  int ret = CHAR_getPetSkill(petindex, pos);
  lua_pushinteger(L, ret);
  return 1;
}

static int SetPetSkill(lua_State *L) {
  const int petindex = luaL_checkint(L, 1);
  const int pos = luaL_checkint(L, 2);
  const int newskill = luaL_checkint(L, 3);
  if (pos < 0 || pos >= CHAR_MAXPETSKILLHAVE) {
    return 0;
  }
  int ret = CHAR_setPetSkill(petindex, pos, newskill);
  lua_pushinteger(L, ret);
  return 1;
}

static int HealAll(lua_State *L) {
  const int index = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(index)) {
    lua_pushinteger(L, 0);
    return 1;
  }
  NPC_HealerAllHeal(index);
  return 1;
}

static int getPartyNum(lua_State *L) {
  int i, tempcount = 0;
  const int index = luaL_checkint(L, 1);

  if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) ==
      CHAR_PARTY_NONE) { //无组队的情况下
    tempcount++;
  } else {
    for (i = 0; i < CHAR_PARTYMAX; i++) {
      if (CHAR_getWorkInt(index, CHAR_WORKPARTYINDEX1 + i) != -1) {
        tempcount++;
      }
    }
  }
  lua_pushinteger(L, tempcount);
  return 1;
}

#ifdef _NB_CHARSTRTITLE
tagNameCodeMode NewNames[] = {
    {0, NB_NEWNAME0},    {1, NB_NEWNAME1},    {2, NB_NEWNAME2},
    {3, NB_NEWNAME3},    {4, NB_NEWNAME4},    {5, NB_NEWNAME5},
    {6, NB_NEWNAME6},    {7, NB_NEWNAME7},    {8, NB_NEWNAME8},
    {9, NB_NEWNAME9},    {10, NB_NEWNAME10},  {11, NB_NEWNAME11},
    {12, NB_NEWNAME12},  {13, NB_NEWNAME13},  {14, NB_NEWNAME14},
    {15, NB_NEWNAME15},  {16, NB_NEWNAME16},  {17, NB_NEWNAME17},
    {18, NB_NEWNAME18},  {19, NB_NEWNAME19},  {20, NB_NEWNAME20},
    {21, NB_NEWNAME21},  {22, NB_NEWNAME22},  {23, NB_NEWNAME23},
    {24, NB_NEWNAME24},  {25, NB_NEWNAME25},  {26, NB_NEWNAME26},
    {27, NB_NEWNAME27},  {28, NB_NEWNAME28},  {29, NB_NEWNAME29},
    {30, NB_NEWNAME30},  {31, NB_NEWNAME31},  {32, NB_NEWNAME0},
    {33, NB_NEWNAME1},   {34, NB_NEWNAME2},   {35, NB_NEWNAME3},
    {36, NB_NEWNAME4},   {37, NB_NEWNAME5},   {38, NB_NEWNAME6},
    {39, NB_NEWNAME7},   {40, NB_NEWNAME8},   {41, NB_NEWNAME9},
    {42, NB_NEWNAME10},  {43, NB_NEWNAME11},  {44, NB_NEWNAME12},
    {45, NB_NEWNAME13},  {46, NB_NEWNAME14},  {47, NB_NEWNAME15},
    {48, NB_NEWNAME16},  {49, NB_NEWNAME17},  {50, NB_NEWNAME18},
    {51, NB_NEWNAME19},  {52, NB_NEWNAME20},  {53, NB_NEWNAME21},
    {54, NB_NEWNAME22},  {55, NB_NEWNAME23},  {56, NB_NEWNAME24},
    {57, NB_NEWNAME25},  {58, NB_NEWNAME26},  {59, NB_NEWNAME27},
    {60, NB_NEWNAME28},  {61, NB_NEWNAME29},  {62, NB_NEWNAME30},
    {63, NB_NEWNAME31},  {64, NB_NEWNAME0},   {65, NB_NEWNAME1},
    {66, NB_NEWNAME2},   {67, NB_NEWNAME3},   {68, NB_NEWNAME4},
    {69, NB_NEWNAME5},   {70, NB_NEWNAME6},   {71, NB_NEWNAME7},
    {72, NB_NEWNAME8},   {73, NB_NEWNAME9},   {74, NB_NEWNAME10},
    {75, NB_NEWNAME11},  {76, NB_NEWNAME12},  {77, NB_NEWNAME13},
    {78, NB_NEWNAME14},  {79, NB_NEWNAME15},  {80, NB_NEWNAME16},
    {81, NB_NEWNAME17},  {82, NB_NEWNAME18},  {83, NB_NEWNAME19},
    {84, NB_NEWNAME20},  {85, NB_NEWNAME21},  {86, NB_NEWNAME22},
    {87, NB_NEWNAME23},  {88, NB_NEWNAME24},  {89, NB_NEWNAME25},
    {90, NB_NEWNAME26},  {91, NB_NEWNAME27},  {92, NB_NEWNAME28},
    {93, NB_NEWNAME29},  {94, NB_NEWNAME30},  {95, NB_NEWNAME31},
    {96, NB_NEWNAME0},   {97, NB_NEWNAME1},   {98, NB_NEWNAME2},
    {99, NB_NEWNAME3},   {100, NB_NEWNAME4},  {101, NB_NEWNAME5},
    {102, NB_NEWNAME6},  {103, NB_NEWNAME7},  {104, NB_NEWNAME8},
    {105, NB_NEWNAME9},  {106, NB_NEWNAME10}, {107, NB_NEWNAME11},
    {108, NB_NEWNAME12}, {109, NB_NEWNAME13}, {110, NB_NEWNAME14},
    {111, NB_NEWNAME15}, {112, NB_NEWNAME16}, {113, NB_NEWNAME17},
    {114, NB_NEWNAME18}, {115, NB_NEWNAME19}, {116, NB_NEWNAME20},
    {117, NB_NEWNAME21}, {118, NB_NEWNAME22}, {119, NB_NEWNAME23},
    {120, NB_NEWNAME24}, {121, NB_NEWNAME25}, {122, NB_NEWNAME26},
    {123, NB_NEWNAME27}, {124, NB_NEWNAME28}, {125, NB_NEWNAME29},
    {126, NB_NEWNAME30}, {127, NB_NEWNAME31}};

static int CheckNewName(lua_State *L) { //获取称号权限
  const int NameNo = luaL_checkint(L, 1);
  const int namecode1 = luaL_checkint(L, 2);
  const int namecode2 = luaL_checkint(L, 3);
  const int namecode3 = luaL_checkint(L, 4);
  const int namecode4 = luaL_checkint(L, 5);
  int ret = -1;
  int i;
  int tm_num;
  for (i = 0; i < sizeof(NewNames) / sizeof(tagNameCodeMode); i++) {
    if ((tm_num = (i / 32)) == 1) {
      if (NewNames[i].NameNo == NameNo && (NewNames[i].Code & namecode2)) {
        ret = 1;
        break;
      }
    } else if ((tm_num = (i / 32)) == 2) {
      if (NewNames[i].NameNo == NameNo && (NewNames[i].Code & namecode3)) {
        ret = 1;
        break;
      }
    } else if ((tm_num = (i / 32)) == 3) {
      if (NewNames[i].NameNo == NameNo && (NewNames[i].Code & namecode4)) {
        ret = 1;
        break;
      }
    } else if (NewNames[i].NameNo == NameNo && (NewNames[i].Code & namecode1)) {
      ret = 1;
      break;
    }
  }
  lua_pushinteger(L, ret);
  return 1;
}

static int SetNewName(lua_State *L) { //设置称号权限
  const int meindex = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(meindex)) {
    lua_pushinteger(L, 0);
    return 1;
  }
  const int index = luaL_checkint(L, 2);
  int i = 0;
  for (i = 0; i < 128; i++) {
    if (index == NewNames[i].NameNo) {
      int LRCode = -1;
      if (index < 32) {
        LRCode = CHAR_getInt(meindex, CHAR_NB_NEWNAME1);
        LRCode = LRCode | NewNames[i].Code;
        CHAR_setInt(meindex, CHAR_NB_NEWNAME1, LRCode);
      } else if (index < 64) {
        LRCode = CHAR_getInt(meindex, CHAR_NB_NEWNAME2);
        LRCode = LRCode | NewNames[i].Code;
        CHAR_setInt(meindex, CHAR_NB_NEWNAME2, LRCode);
      } else if (index < 96) {
        LRCode = CHAR_getInt(meindex, CHAR_NB_NEWNAME3);
        LRCode = LRCode | NewNames[i].Code;
        CHAR_setInt(meindex, CHAR_NB_NEWNAME3, LRCode);
      } else if (index < 128) {
        LRCode = CHAR_getInt(meindex, CHAR_NB_NEWNAME4);
        LRCode = LRCode | NewNames[i].Code;
        CHAR_setInt(meindex, CHAR_NB_NEWNAME4, LRCode);
      }
    }
  }
  return 1;
}

static int DelNewName(lua_State *L) { //移除称号权限
  const int meindex = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(meindex)) {
    lua_pushinteger(L, 0);
    return 1;
  }
  const int index = luaL_checkint(L, 2);

  int i = 0;
  for (i = 0; i < MAXNOINDEX; i++) {
    if (index == NewNames[i].NameNo) {
      int LRCode = -1;
      if (index < 32) {
        LRCode = CHAR_getInt(meindex, CHAR_NB_NEWNAME1);
        LRCode = LRCode ^ NewNames[i].Code;
        CHAR_setInt(meindex, CHAR_NB_NEWNAME1, LRCode);
        break;
      } else if (index < 64) {
        LRCode = CHAR_getInt(meindex, CHAR_NB_NEWNAME2);
        LRCode = LRCode ^ NewNames[i].Code;
        CHAR_setInt(meindex, CHAR_NB_NEWNAME2, LRCode);
        break;
      } else if (index < 96) {
        LRCode = CHAR_getInt(meindex, CHAR_NB_NEWNAME3);
        LRCode = LRCode ^ NewNames[i].Code;
        CHAR_setInt(meindex, CHAR_NB_NEWNAME3, LRCode);
        break;
      } else if (index < 128) {
        LRCode = CHAR_getInt(meindex, CHAR_NB_NEWNAME4);
        LRCode = LRCode ^ NewNames[i].Code;
        CHAR_setInt(meindex, CHAR_NB_NEWNAME4, LRCode);
        break;
      }
    }
  }
  return 1;
}
#endif

#ifdef _NB_CHARDISPTITLE
static int sendArroundCharaData(lua_State *L) { //移除称号权限
  const int index = luaL_checkint(L, 1);
  if (!CHAR_CHECKINDEX(index)) {
    lua_pushinteger(L, 0);
    return 1;
  }
  CHAR_sendArroundCharaData(index);
  lua_pushinteger(L, 1);
  return 1;
}
#endif

static int GetIndexForCdKey(lua_State *L) {
  size_t l;
  char *cdkey = luaL_checklstring(L, 1, &l);
  int TM_Ret = -1;
  int i;
  int playernum = CHAR_getPlayerMaxNum();
  for (i = 0; i < playernum; i++) {
    if (!CHAR_getCharUse(i))
      continue;
    if (!strcmp(CHAR_getChar(i, CHAR_CDKEY), cdkey)) {
      TM_Ret = i;
      break;
    }
  }
  lua_pushinteger(L, TM_Ret);
  return 1;
}

typedef struct {
  char arg[32];
  int Code;
} tagNewRideCode;
tagNewRideCode NewRides[] = {
    {"RIDE_PET0", RIDE_PET0},   {"RIDE_PET1", RIDE_PET1},
    {"RIDE_PET2", RIDE_PET2},   {"RIDE_PET3", RIDE_PET3},
    {"RIDE_PET4", RIDE_PET4},   {"RIDE_PET5", RIDE_PET5},
    {"RIDE_PET6", RIDE_PET6},   {"RIDE_PET7", RIDE_PET7},
    {"RIDE_PET8", RIDE_PET8},   {"RIDE_PET9", RIDE_PET9},
    {"RIDE_PET10", RIDE_PET10}, {"RIDE_PET11", RIDE_PET11},
    {"RIDE_PET12", RIDE_PET12}, {"RIDE_PET13", RIDE_PET13},
    {"RIDE_PET14", RIDE_PET14},
#ifdef _RIDE_CF
    {"RIDE_PET15", RIDE_PET15}, {"RIDE_PET16", RIDE_PET16},
    {"RIDE_PET17", RIDE_PET17}, {"RIDE_PET18", RIDE_PET18},
    {"RIDE_PET19", RIDE_PET19}, {"RIDE_PET20", RIDE_PET20},
    {"RIDE_PET21", RIDE_PET21}, {"RIDE_PET22", RIDE_PET22},
    {"RIDE_PET23", RIDE_PET23}, {"RIDE_PET24", RIDE_PET24},
    {"RIDE_PET25", RIDE_PET25}, {"RIDE_PET26", RIDE_PET26},
    {"RIDE_PET27", RIDE_PET27}, {"RIDE_PET28", RIDE_PET28},
    {"RIDE_PET29", RIDE_PET29}, {"RIDE_PET30", RIDE_PET30},
    {"RIDE_PET31", RIDE_PET31}, {"RIDE_PET32", RIDE_PET0},
    {"RIDE_PET33", RIDE_PET1},  {"RIDE_PET34", RIDE_PET2},
    {"RIDE_PET35", RIDE_PET3},  {"RIDE_PET36", RIDE_PET4},
    {"RIDE_PET37", RIDE_PET5},  {"RIDE_PET38", RIDE_PET6},
    {"RIDE_PET39", RIDE_PET7},  {"RIDE_PET40", RIDE_PET8},
    {"RIDE_PET41", RIDE_PET9},  {"RIDE_PET42", RIDE_PET10},
    {"RIDE_PET43", RIDE_PET11}, {"RIDE_PET44", RIDE_PET12},
    {"RIDE_PET45", RIDE_PET13}, {"RIDE_PET46", RIDE_PET14},
    {"RIDE_PET47", RIDE_PET15}, {"RIDE_PET48", RIDE_PET16},
    {"RIDE_PET49", RIDE_PET17}, {"RIDE_PET50", RIDE_PET18},
    {"RIDE_PET51", RIDE_PET19}, {"RIDE_PET52", RIDE_PET20},
    {"RIDE_PET53", RIDE_PET21}, {"RIDE_PET54", RIDE_PET22},
    {"RIDE_PET55", RIDE_PET23}, {"RIDE_PET56", RIDE_PET24},
    {"RIDE_PET57", RIDE_PET25}, {"RIDE_PET58", RIDE_PET26},
    {"RIDE_PET59", RIDE_PET27}, {"RIDE_PET60", RIDE_PET28},
    {"RIDE_PET61", RIDE_PET29}, {"RIDE_PET62", RIDE_PET30},
    {"RIDE_PET63", RIDE_PET31},
#ifdef _ADD_RIDE_CF
    {"RIDE_PET64", RIDE_PET0},  {"RIDE_PET65", RIDE_PET1},
    {"RIDE_PET66", RIDE_PET2},  {"RIDE_PET67", RIDE_PET3},
    {"RIDE_PET68", RIDE_PET4},  {"RIDE_PET69", RIDE_PET5},
    {"RIDE_PET70", RIDE_PET6},  {"RIDE_PET71", RIDE_PET7},
    {"RIDE_PET72", RIDE_PET8},  {"RIDE_PET73", RIDE_PET9},
    {"RIDE_PET74", RIDE_PET10}, {"RIDE_PET75", RIDE_PET11},
    {"RIDE_PET76", RIDE_PET12}, {"RIDE_PET77", RIDE_PET13},
    {"RIDE_PET78", RIDE_PET14}, {"RIDE_PET79", RIDE_PET15},
    {"RIDE_PET80", RIDE_PET16}, {"RIDE_PET81", RIDE_PET17},
    {"RIDE_PET82", RIDE_PET18}, {"RIDE_PET83", RIDE_PET19},
    {"RIDE_PET84", RIDE_PET20}, {"RIDE_PET85", RIDE_PET21},
    {"RIDE_PET86", RIDE_PET22}, {"RIDE_PET87", RIDE_PET23},
    {"RIDE_PET88", RIDE_PET24}, {"RIDE_PET89", RIDE_PET25},
    {"RIDE_PET90", RIDE_PET26}, {"RIDE_PET91", RIDE_PET27},
    {"RIDE_PET92", RIDE_PET28}, {"RIDE_PET93", RIDE_PET29},
    {"RIDE_PET94", RIDE_PET30}, {"RIDE_PET95", RIDE_PET31},
#endif
#endif
};

static int GiveRide(lua_State *L) {
  size_t l;
  const int index = luaL_checkint(L, 1);
  const int rideindex = luaL_checkint(L, 2);

  if (rideindex < 32) {
    int LRCode = CHAR_getInt(index, CHAR_LOWRIDEPETS);
    LRCode = LRCode | NewRides[rideindex].Code;
    CHAR_setInt(index, CHAR_LOWRIDEPETS, LRCode);
  } else if (rideindex < 64) {
    int LRCode = CHAR_getInt(index, CHAR_LOWRIDEPETS1);
    LRCode = LRCode | NewRides[rideindex].Code;
    CHAR_setInt(index, CHAR_LOWRIDEPETS1, LRCode);
  } else if (rideindex < 96) {
    int LRCode = CHAR_getInt(index, CHAR_HIGHRIDEPET2);
    LRCode = LRCode | NewRides[rideindex].Code;
    CHAR_setInt(index, CHAR_HIGHRIDEPET2, LRCode);
  }
  return 1;
}

static int DelRide(lua_State *L) {
  size_t l;
  const int index = luaL_checkint(L, 1);
  const int rideindex = luaL_checkint(L, 2);

  if (rideindex < 32) {
    int LRCode = CHAR_getInt(index, CHAR_LOWRIDEPETS);
    LRCode = LRCode ^ NewRides[rideindex].Code;
    CHAR_setInt(index, CHAR_LOWRIDEPETS, LRCode);
  } else if (rideindex < 64) {
    int LRCode = CHAR_getInt(index, CHAR_LOWRIDEPETS1);
    LRCode = LRCode ^ NewRides[rideindex].Code;
    CHAR_setInt(index, CHAR_LOWRIDEPETS1, LRCode);
  } else if (rideindex < 96) {
    int LRCode = CHAR_getInt(index, CHAR_HIGHRIDEPET2);
    LRCode = LRCode ^ NewRides[rideindex].Code;
    CHAR_setInt(index, CHAR_HIGHRIDEPET2, LRCode);
  }
  return 1;
}

static int SendCharAttack(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);

  int fd = getfdFromCharaIndex(charaindex);
  int battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
  int EscapeFree;
  int endFlg;
  int iNum = -1, At_SoubiIndex;
  int i;
  int side;

  {

    BATTLE *pBattle;
    BATTLE_ENTRY *pEntry;
    if (BATTLE_CHECKINDEX(battleindex) == FALSE)
      return BATTLE_ERR_BATTLEINDEX;
    pBattle = &BattleArray[battleindex];
    side = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLESIDE) == 0 ? 1 : 0;
    pEntry = pBattle->Side[side].Entry;
    int ordinal[10] = {9, 7, 5, 6, 8, 4, 2, 0, 1, 3};
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      if (CHAR_CHECKINDEX(pEntry[ordinal[i]].charaindex) != TRUE)
        continue;
      if (CHAR_getInt(pEntry[ordinal[i]].charaindex, CHAR_HP) <= 0)
        continue;
      iNum = ordinal[i] + 10;
      break;
    }
  }

  if (checkErrorStatus(charaindex)) {
    BattleCommandDispach(fd, "N");
    return;
  }

  EscapeFree = 0;

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM2, iNum);
  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);

  At_SoubiIndex = CHAR_getItemIndex(charaindex, CHAR_ARM);
  if (ITEM_CHECKINDEX(At_SoubiIndex) == FALSE) {

  } else {

    if (ITEM_getInt(At_SoubiIndex, ITEM_TYPE) == ITEM_BOOMERANG) {
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_BOOMERANG);
#ifdef _PETSKILL_BECOMEFOX
      if (CHAR_getInt(charaindex, CHAR_BASEIMAGENUMBER) == 101749 ||
          CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) !=
              -1) { //若是变成小狐狸不可以使用回力标
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE);
      }
#endif
#ifdef _PETSKILL_BECOMEPIG
      if (CHAR_getInt(charaindex, CHAR_BECOMEPIG) > -1) { //变成乌力了
        CHAR_setWorkInt(charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK);
      }
#endif
    }
  }

  CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);

  CHAR_SETWORKINT_LOW(charaindex, CHAR_WORKBATTLECOM3, 1);

  endFlg = 1;

  BATTLE_MpDown(charaindex, 5);

  if (endFlg) {
    BATTLE_ActSettingSend(battleindex);
  }

  if (BattleArray[battleindex].turn == 0) {
    BattleArray[battleindex].flg |= EscapeFree;
  } else {
    BattleArray[battleindex].flg &= EscapeFree;
  }

  return 1;
}

static int SendPetAttack(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);

  int fd = getfdFromCharaIndex(charaindex);
  int battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
  int EscapeFree;
  int endFlg;
  int iNum = -1, petskillindex = -1, ToNo = -1, petnum, petindex;
  BATTLE_ENTRY *pEntry;
  int pindex, i, j;

  iNum = 0;
  ToNo = -1;

  {

    BATTLE *pBattle;
    BATTLE_ENTRY *pEntry;
    int side;
    if (BATTLE_CHECKINDEX(battleindex) == FALSE)
      return BATTLE_ERR_BATTLEINDEX;
    pBattle = &BattleArray[battleindex];
    side = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLESIDE) == 0 ? 1 : 0;
    pEntry = pBattle->Side[side].Entry;
    int ordinal[10] = {9, 7, 5, 6, 8, 4, 2, 0, 1, 3};
    for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
      if (CHAR_CHECKINDEX(pEntry[ordinal[i]].charaindex) != TRUE)
        continue;
      if (CHAR_getInt(pEntry[ordinal[i]].charaindex, CHAR_HP) <= 0)
        continue;
      ToNo = ordinal[i] + 10;
      break;
    }
  }

  petnum = CHAR_getInt(charaindex, CHAR_DEFAULTPET);
  petindex = CHAR_getCharPet(charaindex, petnum);

  {
#ifdef _FIX_PETSKILL_BUG
    if (CHAR_getWorkInt(petindex, CHAR_WORKBATTLEMODE) ==
            BATTLE_CHARMODE_C_OK ||
        CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) !=
            BATTLE_CHARMODE_C_OK) {
      CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      CHAR_setWorkInt(charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      print("3071!!!!!!!!!!!!\n");
    } else
#endif
        if (iNum < 0 || iNum >= CHAR_MAXPETSKILLHAVE
#ifdef _PETSKILLBUG // Add By Syu 2002.0805
            || (CHAR_getFlg(charaindex, CHAR_ISDIE) == TRUE) ||
            (CHAR_getInt(charaindex, CHAR_HP) <= 0)
#endif
            || checkErrorStatus(petindex)) {
      // sprintf( szBuffer, "Command(%s)(自动)", CHAR_getUseName( petindex ) );
      CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      print("3085!!!!!!!!!!!!\n");
#ifdef _PETSKILLBUG // Add By Syu 2002.0805
    } else if ((CHAR_getInt(petindex, CHAR_TRANSMIGRATION) < 1) &&
               (iNum >= CHAR_getInt(petindex, CHAR_SLOT))) {
      // sprintf( szBuffer, "Command(%s)(自动)", CHAR_getUseName( petindex ) );
      CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      print("3093!!!!!!!!!!!!\n");
#endif
    } else {
      petskillindex = PETSKILL_GetArray(petindex, iNum);
#ifdef _FIXWOLF // Syu ADD 修正狼人变身Bug
      if ((CHAR_getPetSkill(petindex, iNum) == 600) && (ToNo == -1)) {
        for (j = 0; j < 2; j++) {
          pEntry = BattleArray[battleindex].Side[j].Entry;
          for (i = 0; i < BATTLE_ENTRY_MAX; i++) {
            pindex = pEntry[i].charaindex;
            if (pindex == petindex) {
              ToNo = i + (j * 10);
              break;
            }
          }
        }
      }
#endif
      if (petskillindex >= 0 &&
          PETSKILL_Use(petindex, iNum, ToNo, NULL) == TRUE) {
        // sprintf( szBuffer, "Command(%s)(石版)", CHAR_getUseName( petindex )
        // );

        EscapeFree = 0;

      } else {
        // sprintf( szBuffer, "Command(%s)(自动)", CHAR_getUseName( petindex )
        // );
        CHAR_setWorkInt(petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK);
      }
    }
  }
  endFlg = 1;

  BATTLE_MpDown(charaindex, 10);

  if (endFlg) {
    BATTLE_ActSettingSend(battleindex);
  }

  if (BattleArray[battleindex].turn == 0) {
    BattleArray[battleindex].flg |= EscapeFree;
  } else {
    BattleArray[battleindex].flg &= EscapeFree;
  }

  return 1;
}

static int MakeLeaderBot(lua_State *L) {

  //[1]Name, [2]Metamo
  //[3]Floor, [4]X, [5]Y, [6]Angle
  //[7]Enemy Id(Base Stats), [8] Level
  //[9]Party Low Dex, [10] Party High Dex
  //[11]VITAL, [12] STR, [13]TOUGH, [14]DEX,
  //[15]Pet Id, [16]Pet Level
  size_t NameStrLen;
  const char *Name = luaL_checklstring(L, 1, &NameStrLen);
  const int Metamo = luaL_checkinteger(L, 2);
  const int Floor = luaL_checkinteger(L, 3);
  const int X = luaL_checkinteger(L, 4);
  const int Y = luaL_checkinteger(L, 5);
  const int Dir = luaL_checkinteger(L, 6);
  const int BotEnemyId = luaL_checkinteger(L, 7);
  const int BotLv = luaL_checkinteger(L, 8);
  const int LowSpd = luaL_checkinteger(L, 9);
  const int HighSpd = luaL_checkinteger(L, 10);
  const int BotVit = luaL_checkinteger(L, 11);
  const int BotStr = luaL_checkinteger(L, 12);
  const int BotTgh = luaL_checkinteger(L, 13);
  const int BotDex = luaL_checkinteger(L, 14);
  const int BotWeapId = luaL_checkinteger(L, 15);
  const int BotArmorId = luaL_checkinteger(L, 16);
  const int BotPetId = luaL_checkinteger(L, 17);
  const int BotPetLv = luaL_checkinteger(L, 18);
  const int BotPetDex = luaL_checkinteger(L, 19);
  const int BotPetWeapId = luaL_checkinteger(L, 20);
  int i;
  Object object;
  int objindex;
  int petindex;
  int itemindex;

  if (NameStrLen > 32) {
    print("**********Bot Name Too long!!!\n");
    lua_pushinteger(L, -1);
    return;
  } else if (NameStrLen <= 0) {
    print("**********Bot Name Empty!!!\n");
    lua_pushinteger(L, -1);
    return;
  }

  for (i = 0; i < ENEMY_getEnemyNum(); i++) {
    if (ENEMY_getInt(i, ENEMY_ID) == BotEnemyId)
      break;
  }

  if (i == ENEMY_getEnemyNum()) {
    print("********[No Enemy Id]BOT failed.\n");
    lua_pushinteger(L, -1);
    return 1;
  }

  int npcindex = ENEMY_createEnemy(i, 0);
  if (npcindex < 0) {
    print("********[Creation Failed]BOT failed.\n");
    lua_pushinteger(L, -1);
    return -1;
  }

  CHAR_setChar(npcindex, CHAR_NAME, Name);
  CHAR_setInt(npcindex, CHAR_WHICHTYPE, CHAR_TYPEPLAYERNPC);
  CHAR_setInt(npcindex, CHAR_BASEBASEIMAGENUMBER, Metamo);
  CHAR_setInt(npcindex, CHAR_BASEIMAGENUMBER, Metamo);
  CHAR_setInt(npcindex, CHAR_FLOOR, Floor);
  CHAR_setInt(npcindex, CHAR_X, X);
  CHAR_setInt(npcindex, CHAR_Y, Y);
  CHAR_setInt(npcindex, CHAR_DIR, Dir);

  CHAR_setInt(npcindex, CHAR_MODAI, 10000);
  CHAR_setInt(npcindex, CHAR_VARIABLEAI, 10000);

  for (i = 0; i < BotLv; i++) {
    CHAR_PetLevelUp(npcindex);
    CHAR_PetAddVariableAi(npcindex, AI_FIX_PETLEVELUP);
    CHAR_setInt(npcindex, CHAR_LV, CHAR_getInt(npcindex, CHAR_LV) + 1);
  }
  CHAR_setInt(npcindex, CHAR_MAXMP, 100);
  CHAR_setInt(npcindex, CHAR_MP, 100);
  CHAR_setInt(npcindex, CHAR_VITAL, BotVit * 100);
  CHAR_setInt(npcindex, CHAR_STR, BotStr * 100);
  CHAR_setInt(npcindex, CHAR_TOUGH, BotTgh * 100);
  CHAR_setInt(npcindex, CHAR_DEX, BotDex * 100);
  CHAR_setInt(npcindex, CHAR_RIDEPET, -1);

  char owntitle[256];
  sprintf(owntitle, "鉴惯仿:%d", BotDex);
  CHAR_setChar(npcindex, CHAR_OWNTITLE, owntitle);

  if ((itemindex = ITEM_makeItemAndRegist(BotWeapId)) > 0) {
    CHAR_setItemIndex(npcindex, CHAR_ARM, itemindex);
    ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, npcindex);
    ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
  }

  if ((itemindex = ITEM_makeItemAndRegist(BotArmorId)) > 0) {
    CHAR_setItemIndex(npcindex, CHAR_BODY, itemindex);
    ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, npcindex);
    ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
  }

  CHAR_complianceParameter(npcindex);
  CHAR_setInt(npcindex, CHAR_HP, CHAR_getWorkInt(npcindex, CHAR_WORKMAXHP));

  object.type = OBJTYPE_CHARA;
  object.index = npcindex;
  object.x = CHAR_getInt(npcindex, CHAR_X);
  object.y = CHAR_getInt(npcindex, CHAR_Y);
  object.floor = CHAR_getInt(npcindex, CHAR_FLOOR);

  objindex = initObjectOne(&object);

  if (objindex == -1) {
    CHAR_endCharOneArray(npcindex);
    print("********[Init Object Failed]BOT failed.\n");
    lua_pushinteger(L, -1);
    return -1;
  }

  CHAR_LoginBesideSetWorkInt(npcindex, -1);
  // CHAR_setWorkChar( npcindex, CHAR_WORKPARTYNAME      , Name );
  CHAR_setWorkInt(npcindex, CHAR_WORKOBJINDEX, objindex);
  // CHAR_setWorkInt(  npcindex, CHAR_WORKTYPE           , CHAR_TYPELEADERBOT );
  CHAR_setWorkInt(npcindex, CHAR_WORKSTREETVENDOR, -1);
  CHAR_setWorkInt(npcindex, CHAR_WORKPARTYLOWDEX, LowSpd);
  CHAR_setWorkInt(npcindex, CHAR_WORKPARTYHIGHDEX, HighSpd);
  // CHAR_setWorkInt(  npcindex, CHAR_WORKPARTYALLOWJOIN , TRUE );
  // CHAR_setWorkInt(  npcindex, CHAR_WORKPARTYFORCEAI   , FALSE );

  CHAR_setWorkInt(npcindex, CHAR_WORK_OFFLINE, 1);
  CHAR_setFlg(npcindex, CHAR_ISPARTY, 1);

  {
    int EQ_BBI = -1, EQ_ARM = -1, EQ_NUM = -1, EQ_BI = -1;
    int CH_BI = CHAR_getInt(npcindex, CHAR_BASEIMAGENUMBER);

    EQ_BBI = CHAR_getInt(npcindex, CHAR_BASEBASEIMAGENUMBER);

    EQ_ARM = CHAR_getItemIndex(npcindex, CHAR_ARM);

    EQ_NUM = ITEM_FIST;

    if (ITEM_CHECKINDEX(EQ_ARM))
      EQ_NUM = ITEM_getInt(EQ_ARM, ITEM_TYPE);

    EQ_BI = CHAR_getNewImagenumberFromEquip(EQ_BBI, EQ_NUM);
    if (EQ_BI != -1)
      CHAR_setInt(npcindex, CHAR_BASEIMAGENUMBER, EQ_BI);
  }

  if (BotPetId > -1) {
    for (i = 0; i < ENEMY_getEnemyNum(); i++) {
      if (ENEMY_getInt(i, ENEMY_ID) == BotPetId)
        break;
    }

    if (i == ENEMY_getEnemyNum()) {
      print("********[No enemy ID]BOT PET failed.\n");
      lua_pushinteger(L, -1);
      return -1;
    }

    petindex = ENEMY_createPets(npcindex, i);
    if (!CHAR_CHECKINDEX(petindex)) {
      print("********[Pet Creation Failed]BOT PET failed.\n");
      lua_pushinteger(L, -1);
      return -1;
    }

    for (i = 0; i < BotPetLv; i++) {
      CHAR_PetLevelUp(petindex);
      CHAR_PetAddVariableAi(petindex, AI_FIX_PETLEVELUP);
      CHAR_setInt(petindex, CHAR_LV, CHAR_getInt(petindex, CHAR_LV) + 1);
      CHAR_setInt(petindex, CHAR_DEX, BotPetDex);
    }
    CHAR_setInt(petindex, CHAR_DEX, BotPetDex * 100);
    CHAR_setMaxExpFromLevel(petindex, CHAR_getInt(petindex, CHAR_LV));
    CHAR_setInt(petindex, CHAR_VARIABLEAI, 10000);

    char owntitle2[256];
    sprintf(owntitle2, "鉴惯仿:%d", BotPetDex);
    CHAR_setChar(petindex, CHAR_OWNTITLE, owntitle2);

    if ((itemindex = ITEM_makeItemAndRegist(BotPetWeapId)) > 0) {
      CHAR_setItemIndex(petindex, CHAR_ARM, itemindex);
      ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, petindex);
      ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
    }

    CHAR_complianceParameter(petindex);
    CHAR_setInt(petindex, CHAR_HP, CHAR_getWorkInt(petindex, CHAR_WORKMAXHP));

    CHAR_LoginBesideSetWorkInt(petindex, -1);
    // CHAR_setWorkInt(  petindex, CHAR_WORKTYPE           , CHAR_TYPELEADERBOT
    // );

    CHAR_setInt(npcindex, CHAR_DEFAULTPET, 0);

    CHAR_setWorkInt(petindex, CHAR_WORK_OFFLINE, 1);
    CHAR_setWorkInt(petindex, CHAR_WORKPARTYMODE, CHAR_PARTY_LEADER);
    CHAR_setInt(petindex, CHAR_WHICHTYPE, CHAR_TYPEPLAYERPETNPC);
  }

  lua_pushinteger(L, npcindex);

  if (Bot.Count < arraysizeof(Bot.Index))
    Bot.Index[Bot.Count++] = npcindex;
  else
    print("Leader bot index storage full!!!\n");

  return 1;
}

static int BattleStayLoop(lua_State *L) {
  const int charaindex = luaL_checkint(L, 1);
  if (CHAR_getWorkInt(charaindex, CHAR_WORK_OFFLINE) > 0) {
    if (CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEMODE) ==
        BATTLE_CHARMODE_NONE) {
      CHAR_setWorkChar(charaindex, CHAR_WORKWALKARRAY, "");
      BATTLE_CreateVsEnemy(charaindex, 0, -1);
    }
    return 1;
  }

  return 1;
}

static const luaL_Reg charlib[] = {
    {"getCharNum", getCharNum},
    {"getPlayerMaxNum", getPlayerMaxNum},
    {"getPetMaxNum", getPetMaxNum},
    {"getOthersMaxNum", getOthersMaxNum},
    {"check", check},
    {"setWorkChar", setWorkChar},
    {"getWorkChar", getWorkChar},
    {"setWorkInt", setWorkInt},
    {"getWorkInt", getWorkInt},
    {"setChar", setChar},
    {"getChar", getChar},
    {"setInt", setInt},
    {"getInt", getInt},
    {"setFlg", setFlg},
    {"getFlg", getFlg},
    {"setFunctionPointer", setFunctionPointer},
    {"delFunctionPointer", delFunctionPointer},
    {"TalkToCli", TalkToCli},
    {"TalkToRound", TalkToRound},
    {"talkToFloor", talkToFloor},
    {"talkToParty", talkToParty},
    {"talkToServer", talkToServer},
    {"BoundRandWalk", BoundRandWalk},
    {"ToAroundChar", ToAroundChar},
#ifdef _ALLBLUES_LUA_1_1
    {"Walk", Walk},
    {"WalkPoint", WalkPoint},
    {"WarpToSpecificPoint", WarpToSpecificPoint},
    {"MapAllWarp", MapAllWarp},
    {"RandRandWalk", RandRandWalk},
#endif
#ifdef _ALLBLUES_LUA_1_3
    {"createPet", createPet},
    {"setCharPet", setCharPet},
    {"setCharPetNew", setCharPetNew},
    {"getCharPet", getCharPet},
    {"setCharPoolPet", setCharPoolPet},
    {"getCharPoolPet", getCharPoolPet},
    {"setDepotPetIndex", setDepotPetIndex},
    {"getDepotPetIndex", getDepotPetIndex},
    {"DelItem", DelItem},
    {"DelItemForIndex", DelItemForIndex},
    {"getFd", getFd},
    {"Updata", Updata},
    {"Additem", Additem},
    {"AddPet", AddPet},
    {"Finditem", Finditem},
    {"FinditemHave", FinditemHave},
#ifdef _PLAYER_NPC
    {"setPlayerNpc", setPlayerNpc},
#endif
    {"Coordinate", Coordinate},
    {"setPetSkill", setPetSkill},
    {"getPetSkill", getPetSkill},
    {"PetLevelUp", PetLevelUp},
    {"PetLevelUp", PetLevelUp},
    {"PetLevelUpNoFame", PetLevelUpNoFame},
    {"JoinParty", JoinParty},
#endif
#ifdef _ALLBLUES_LUA_1_4
    {"getLiftTo8", getLiftTo8},
    {"getRightTo8", getRightTo8},
    {"complianceParameter", complianceParameter},
    {"sendStatusString", sendStatusString},
    {"sendBattleEffect", sendBattleEffect},
    {"dropPetAbsolute", dropPetAbsolute},
    {"AddPetCf", AddPetCf},
    {"AddPetCf2", AddPetCf2},
    {"sendWatchEvent", sendWatchEvent},
    {"AllWarpToSpecificPoint", AllWarpToSpecificPoint},
    {"Findpet", Findpet},
    {"DelPet", DelPet},
    {"FindPetFormEnemyTempID", FindPetFormEnemyTempID},
    {"FindPetFormMatemo", FindPetFormMatemo},
#endif
#ifdef _PETSKILL_SHOP_LUA
    {"FreePetSkill", FreePetSkill},
#endif
#ifdef _ALLBLUES_LUA_1_7
    {"findEmptyPetBox", findEmptyPetBox},
    {"findEmptyItemBox", findEmptyItemBox},
    {"dropPetFollow", dropPetFollow},
    {"getItemIndex", getItemIndex},
    {"getPoolItemIndex", getPoolItemIndex},
    {"getDepotItemIndex", getDepotItemIndex},

    {"charSaveFromConnect", charSaveFromConnect},
    {"DischargeParty", DischargeParty},
    {"Skillupsend", Skillupsend},
#ifdef _RECORD_IP
    {"userip", userip},
#endif
#endif
#ifdef _ALLBLUES_LUA_1_9
    {"logou", logou},
    {"copyChar", copyChar},
#ifdef _ALL_SERV_SEND
    {"talkToAllServer", talkToAllServer},
#endif
#endif
    {"earnFame", earnFame},
    {"Encounter", Encounter},
    {"StopEncounter", StopEncounter},
    {"getDX", getDX},
    {"getDY", getDY},
    {"sendAction", sendAction},
    {"GetOldLevelExp", GetOldLevelExp},
#ifdef _PROFESSION_SKILL // WON ADD 人物职业技能
    {"getCharSkill", getCharSkill},
    {"setCharSkill", setCharSkill},
    {"DelSProfeesionSkill", DelSProfeesionSkill},
#endif
    {"DropPet", DropPet},

    {"getData", getData},
    {"getMyMaxPilenum", getMyMaxPilenum},
    {"PileItemFromItemBoxToItemBox", PileItemFromItemBoxToItemBox},
    {"GetEmptyItemNum", GetEmptyItemNum},
    {"GetEmptyPetNum", GetEmptyPetNum},
    {"GetGmLevel", GetGmLevel},
    {"GetPetSkill", GetPetSkill},
    {"SetPetSkill", SetPetSkill},
    {"HealAll", HealAll},         //全状态恢复
    {"getPartyNum", getPartyNum}, //获取队伍人数
#ifdef _NB_CHARSTRTITLE
    {"CheckNewName", CheckNewName}, //判断称号编号是否开启
    {"SetNewName", SetNewName},
    {"DelNewName", DelNewName},
#endif
#ifdef _NB_CHARDISPTITLE
    {"sendArroundCharaData", sendArroundCharaData},
#endif
    {"GetIndexForCdKey", GetIndexForCdKey},

    {"GiveRide", GiveRide},
    {"DelRide", DelRide},
    {"SendCharAttack", SendCharAttack},
    {"SendPetAttack", SendPetAttack},
    {"MakeLeaderBot", MakeLeaderBot},
    {"BattleStayLoop", BattleStayLoop},

    {NULL, NULL}};

LUALIB_API int luaopen_Char(lua_State *L) {
  luaL_register(L, "char", charlib);
  return 1;
}

#endif
