#ifndef __VERSION_H__
#define __VERSION_H__

#include "correct_bug.h" //BUG FIX
#include "common.h"
#include "NewBilu/version.h"
#include "NewBilu/debug.h"

/*Server 版本控制*/
#define _NEW_SERVER_
#define _SERVER_NUMS

#define UNIQUE_KEYCODEITEM 100
#define UNIQUE_KEYCODEPET "i"

#define ITEM_AURA_

#define _FAMILY_MANORNUM_CHANGE      // CoolFish 用来修改庄园数量(4.0)
#ifdef _FAMILY_MANORNUM_CHANGE       // CoolFish 用来修改庄园数量(4.0)
#define FAMILY_MAXHOME MANOR_MAX_NUM // (4.0) 家族据点
#define MANORNUM MANOR_MAX_NUM       // (4.0) 庄园数量
#define FMPOINTNUM MANOR_MAX_NUM     // (4.0) 有据点家族的最大数量
#endif
#define _FMVER21              // (可开放) 家族族长功能，用来添加成员设置职务的
#define _PERSONAL_FAME        // (可开放) Arminius 8.30 家族个人声望
#define _EVERYONE_RIDE        // (可开放) Robin 0924 骑宠平民化
#define _ATTACK_MAGIC         // (可开放)	提供战场中的攻击性咒术
#define _ADD_ACTION           // (可开放) 一般NPC动作状况的设定
#define _UNIQUE_P_I           // (可开放) CoolFish 2001/10/11  宠物、道具编码
#define _NEWSAVE              // (可开放) Robin 新版人物储存
#define _STORECHAR            // (可开放) Robin 当机不回朔
#define _DROPCHECK            // (可开放) Robin 防止重复丢道具
#define _DROPCHECK2           // (可开放) Robin 防止重复丢道具
#define _PET_TRANS            // (可开放) 宠物转生功能
#define _NEW_WARPMAN          // (可开放) NPC多重WARP功能
#define _PET_TALK             // (可开放) 宠物对话
#define _PET_LIMITLEVEL       // (可开放) 特殊宠设计(玛蕾)
#define _MAP_NOEXIT           // (可开放) 特殊地图不得原登&原登回指定点
#define _GMRELOAD             // (可开放) CoolFish: GM指令、层级Reload 2001/11/12
#define _GAMBLE_ROULETTE      // (可开放) 彦豪 赌场轮盘	宝宝小猪
#define _FIX_GAMBLENUM        // (可开放) 轮盘修正 : 下线扣积分
#define _GAMBLE_BANK          // (可开放) 彦豪  赌场银行
#define _DROPSTAKENEW         // (可开放) CoolFish:  赌场下注
#define _PETRACE              // (可开放) CoolFish: 2001/12/3 宠物竞速场
#define _NEWEVENT             // (可开放) CoolFish: 2001/12/5 新的任务旗标
#define _ITEM_FIRECRACKER     // (可开放) 战斗中使用鞭炮可以把年兽吓走
#define _MANOR_PKRULE         // (可开放)庄园PK规则(酷哥定的)
#define _PSKILL_FALLGROUND    // (可开放) 宠物技能  (落马术)
#define _ENEMY_FALLGROUND     // (可开放) NPC 落马
#define _Item_MoonAct         // (可开放) 月神的守护
#define _Item_DeathAct        // (可开放) 恶魔的首饰
#define _Item_PetAct          // (可开放) 驯兽戒指
#define _Item_ReLifeAct       // (可开放) 需要itemset1.txt 才能开) 替身娃娃
#define _BATTLESTEAL_FIX      // (可开放) 修正偷窃
#define _CFREE_petskill       // (可开放) 需要petskill1.txt 才能开) 新学习宠物技能技能
#define _ITEM_MAXUSERNUM      // (可开放) 新 itemset1.txt ITEM 使用次数
#define _PET_TALKPRO          // (可开放) 宠物对话加强  & 改 END
#define _PRO_NPCFIX           // (可开放) NPC功能加强
#define _PRO_BATTLEENEMYSKILL // (可开放) 战斗加强  提升NPC的AI技能
#define _NEW_WARPPOINT        // (可开放) 传送点多重条件判断WARP
#define _FIX_CHECKITEM        // (可开放) 道具不做非法检查
#define _ADD_ENCOUNT          // (可开放) WON 增加敌遭遇触发修件
#define _WON_PET_MAIL_LOG     // (可开放) WON 玩家寄宠邮 LOG
//#define _LOSE_FINCH_													// (不可开) ANDY 朱雀技能
#define _ADD_POOL_ITEM              // (可开放) WON 增加可寄放的道具
#define _ITEM_METAMO                // (可开放) shan metamo sprite. code:robin->shan
#define _ENEMY_ATTACK_AI            // (可开放) shan enemy attack mode. code:shan
#define _ITEM_TIME_LIMIT            // (可开放) shan time limit of item. code:shan
#define _MIND_ICON                  // (可开放) shan show icon from char mind. code:shan
#define _BATTLENPC_WARP_PLAYER      // (可开放) shan npc warp player when n round. code:shan
#define _ITEM_WARP_FIX_BI           // (可开放) shan fix bbi to bi. code:shan
#define _SKILL_DAMAGETOHP           // (可开放) ANDY 嗜血技
#define _Skill_MPDAMAGE             // (可开放) ANDY MP伤害
#define _WAEI_KICK                  // (可开放) ANDY 跨星球踢人
#define _BATTLE_TIMESPEED           // (可开放) ANDY 战斗时间 测试(北京不必开，挡加速功能)
#define _FIXBUG_ATTACKBOW           // (可开放) ANDY 修正骑宠使用投掷武器bug
#define _NPCCHANGE_PLAYERIMG        // (可开放) ANDY NPC改变玩家图号, 强制落马,认玩家图号 (测完)
#define _EQUIT_DEFMAGIC             // (可开放) ANDY 抗魔装备
#define _FIX_MAGICDAMAGE            // (可开放) ANDY 修正魔法公式 叁期开放
#define _ITEM_CONSTITUTION          // (可开放) ANDY 修正个人体质
#define _MAGIC_REHPAI               // (可开放) ANDY 补血AI
#define _TRANSER_MAN                // (可开放) ANDY 传送师
#define _FIX_FIRECRACKER            // (可开放) ANDY 修正 鞭炮
#define _FIX_ITEMPROB               // (可开放) ANDY 修正 掉宝率
#define _ADD_DEAMGEDEFC             // (可开放) ANDY 额外伤害 & 附加攻击  ITEMSET2
#define _FIX_MAXCHARMP              // (可开放) ANDY MP上限
#define _ITEM_INSLAY                // (可开放) ANDY 镶宝石 动作
#define _ITEMSET2_ITEM              // (可开放) ANDY ITEMSET2
#define _ITEM_NPCCHANGE             // (可开放) ANDY 工匠 (提炼, 精工)
#define _FIX_SETWORKINT             // (可开放) ANDY (FIXWORKINT)
#define _ITEM_ADDEXP                // (可开放) vincent  道具:指定时间内增加经验值上升幅度
#define _ITEM_ADDEXP2               // 智果时间可以累加 Robin
#define _FIX_METAMORIDE             // (可开放) ANDY
#define _SKILL_WILDVIOLENT_ATT      // (可开放) vincent  宠技:狂暴攻击
#define _MAGIC_WEAKEN               // (可开放) vincent  精灵:虚弱
#define _SKILL_WEAKEN               // (可开放) vincent  宠技:虚弱//需要开#define _MAGIC_WEAKEN
#define _SKILL_SPEEDY_ATT           // (可开放) vincent  宠技:疾速攻击
#define _SKILL_GUARDBREAK2          // (可开放) vincent  宠技:破除防御2
#define _SKILL_SACRIFICE            // (可开放) vincent  宠技:救援
#define _PSKILL_MODIFY              // (可开放) ANDY 属性强化攻击
#define _PSKILL_MDFYATTACK          // (可开放) ANDY 攻击转化属性
#define _MAGIC_DEFMAGICATT          // (可开放) ANDY 魔法防御
#define _MAGIC_SUPERWALL            // (可开放) ANDY 铁壁防御
#define _OTHER_MAGICSTAUTS          // (可开放) ANDY
#define _SKILL_TOOTH                // (可开放) ANDY   齿术
#define _MAGIC_DEEPPOISON           // (可开放) vincent  精灵:剧毒
#define _MAGIC_BARRIER              // (可开放) vincent  精灵:魔障
#define _MAGIC_NOCAST               // (可开放) vincent  精灵:沉默
#define _ITEM_CRACKER               // (可开放) vincent  道具:拉炮 //需要开#define _MIND_ICON
#define _SKILL_DEEPPOISON           // (可开放) vincent  宠技:剧毒 //需要开#define _MAGIC_DEEPPOISON
#define _SKILL_BARRIER              // (可开放) vincent  宠技:魔障 //需要开#define _MAGIC_BARRIER
#define _SKILL_ROAR                 // (可开放) vincent  宠技:大吼(克年兽)
#define _SKILL_REFRESH              // (可开放) vincent  宠技:解除异常状态
#define _ITEM_REFRESH               // (可开放) vincent  解除异常状态道具
#define _MAGIC_TOCALL               // (可开放) kjl     召唤  02/06/20 kjl
#define _ITEMSET3_ITEM              // (可开放) ANDY itemset3.txt来吉卡功能
#define _SUIT_ITEM                  // (可开放) ANDY (套装) 来吉卡 需用itemset3.txt
#define _PETSKILL_SETDUCK           // (可开放) ANDY 蒙奇技能
#define _VARY_WOLF                  // (可开放) pet skill : vary wolf. code:shan
#define _USEWARP_FORNUM             // (可开放) ANDY 记忆羽毛 来吉卡
#define _IMPRECATE_ITEM             // (可开放) ANDY 特殊道具 来吉卡
#define _MAGICPET_SKILL             // (可开放) ANDY 魔法宠技能
#define _ITEM_CHECKWARES            // (可开放) ANDY 锁定传送不能
#define _NPC_REPLACEMENT            // (可开放) ANDY 置换战斗npc
#define _TYPE_TOXICATION            // (可开放) ANDY 中毒无法回血
#define _AVID_TRADETRYBUG           // (可开放) ANDY 防止交易中丢宠/道具 寄(宠物)邮件 石币
#define _ASSESS_SYSEFFICACY         // (可开放) ANDY 评估系统效能 12/04 更新
#define _ASSESS_SYSEFFICACY_SUB     // (可开放) Robin 评估系统效能 增加检查内回圈
#define _ITEM_ORNAMENTS             // (可开放) ANDY 装饰道具  12/04 更新  //出版部
#define _CHIKULA_STONE              // (可开放) ANDY 奇克拉之石 12/04 更新  //来吉卡 宠包
#define _SEND_EFFECT                // (可开放) WON  AC送下雪、下雨等特效 12/04 更新
#define _PETMAIL_DEFNUMS            // (可开放) ANDY 宠物邮件限制
#define _PETSKILL_TIMID             // (可开放) ANDY 宠技-怯战
#define _PETS_SELECTCON             // (可开放) ANDY 宠物不可点选出战
#define _CHRISTMAS_REDSOCKS         // (可开放) ANDY 圣诞红袜
#define _FIX_ARRAYBUG               // (可开放) ANDY 修正array 溢位
#define _USER_CHARLOOPS             // (可开放) ANDY 新用物件LOOP
#define _BATTLE_PROPERTY            // (可开放) ANDY 战斗属性特性
#define _PETSKILL_PROPERTY          // (可开放) ANDY 无属魔兽宠技
#define _ITEM_FIXALLBASE            // (可开放) ANDY 修复之石
#define _ITEM_LOVERPARTY            // (可开放) ANDY 情人节道具
#define _ITEM_FORUSERNAMES          // (可开放) ANDY 道具所有人
#define _BATTLECOMMAND_TIME         // (可开放) ANDY 防止玩家开缓速(回合战斗时间)
#define _NPCENEMY_ADDPOWER          // (可开放) ANDY 修改怪物的能力值
#define _PETSKILL_CANNEDFOOD        // (可开放) ANDY 宠物技能罐头  for 6.0
#define _TEST_DROPITEMS             // (可开放) ANDY 测试丢宠
#define _MAP_WARPPOINT              // (可开放) ANDY Map WarpPoint
#define _BENEMY_EVENTRUN            // (可开放) ANDY 战斗npc给道具
#define _BATTLE_ABDUCTII            // (可开放) ANDY 旅途夥伴2
#define _BATTLE_LIGHTTAKE           // (可开放) ANDY 采光术
#define _BATTLE_ATTCRAZED           // (可开放) ANDY 疯狂暴走
#define _CAPTURE_FREES              // (可开放) ANDY 补抓条件
#define _THROWITEM_ITEMS            // (可开放) 丢掷後有奖品
#define _ACFMPK_LIST                // (可开放) ANDY 家族对战列表修正
#define _PETSKILL2_TXT              // (可开放) ANDY petskill2.txt
#define _PETSKILL_CHECKTYPE         // (可开放) ANDY 宠物技能使用时机
#define _PETSKILL_TEAR              // (可开放) ANDY 宠技 撕裂伤口
#define _ITEMSET4_TXT               // (可开放) ANDY itemset4.txt
#define _PET_LOSTPET                // (可开放) ANDY 遗失宠物搜寻计划宠物
#define _ITEMSET5_TXT               // (可开放) ANDY itemset5.txt
#define _ITEMSET6_TXT               // (可开放) ANDY itemset6.txt
#define _EQUIT_SEQUENCE             // (可开放) ANDY 攻击顺序 需要 itemset5.txt
#define _EQUIT_HITRIGHT             // (可开放) ANDY 命中 需要 itemset5.txt
#define _EQUIT_NEGLECTGUARD         // (可开放) ANDY 忽视目标防御力% 需要 itemset6.txt
#define _FIX_MAX_GOLD               // (可开放) WON ADD 增加人物金钱上限
#define _PET_SKILL_SARS             // (可开放) WON ADD 毒煞蔓延
#define _SONIC_ATTACK               // (可开放) WON ADD 音波攻击
#define _NET_REDUCESEND             // (可开放) ANDY 减少DB资料传送
#define _FEV_ADD_NEW_ITEM           // (可开放) Change 增加复活守精
#define _LOCKHELP_OK                // (可开放) Syu ADD 锁定不可加入战斗
#define _SIMPLIFY_ITEMSTRING        // (可开放) ANDY 简化道具字串
#define _SIMPLIFY_PETSTRING         // (可开放) ANDY 简化宠物存档字串
#define _SIMPLIFY_ITEMSTRING2       // (可开放) ANDY 简化道具字串2
#define _WOLF_TAKE_AXE              // (可开放) WON ADD 抓双头狼的限制
#define _FIX_UNNECESSARY            // (可开放) ANDY 修正不必要的判断
#define _ITEM_MAGICRECOVERY         // (可开放) ANDY 新光镜守
#define _PETSKILL_GYRATE            // (可开放) Change 宠技:回旋攻击
#define _PETSKILL_RETRACE           // (可开放) Change 宠技:追迹攻击
#define _PETSKILL_HECTOR            // (可开放) Change 宠技:威吓
#define _PETSKILL_FIREKILL          // (可开放) Change 宠技:火线猎杀
#define _PETSKILL_DAMAGETOHP        // (可开放) Change 宠技:暗月狂狼(嗜血技的变体)
#define _PETSKILL_BECOMEFOX         // (可开放) Change 宠技:媚惑术
#define _PETSKILL_SHOWMERCY         // (可开) Change 宠技:手下留情
#define _NPC_ActionFreeCmp          // (可开) Change 替NPC参数加入新的比较方式
#define _NPC_NewDelPet              // (可开) Change 替NPC加入删除宠物(原本就有了,只是用warpman使用时不成功,所以先做一个顶一下)
#define _CHRISTMAS_REDSOCKS_NEW     // (可开) Change 新圣诞红袜
#define _PETSKILL_COMBINED          // (可开) Change 宠技:难得糊涂 [综合魔法(依参数乱数取出魔法)]
#define _NPC_ADDWARPMAN1            // (可开放) Change npcgen_warpman增加功能(设定组队人数、设定组队男女人数)
#define _ITEM_QUITPARTY             // (可开放) Change 队伍解散时特定道具即会消失
#define _ITEM_ADDEQUIPEXP           // (可开) Change 装备後经验值增加
#define _ITEM_LVUPUP                // (可开) Change 突破宠物等级限制的药(玄武专用)
#define _PETSKILL_BECOMEPIG         // (可开) Change 宠技:乌力化
#define _ITEM_UNBECOMEPIG           // (可开) Change 乌力化的解药
#define _PET_TALKBBI                // (可开) Change 增加判断bbi
#define _ITEM_USEMAGIC              // (可开) Change 由道具使用魔法
#define _ITEM_PROPERTY              // (可开) Change 改变四属性道具
#define _SUIT_ADDENDUM              // (可开) Change 替套装增加功能 (_SUIT_ITEM要开)
#define _ITEM_CHECKDROPATLOGOUT     // (可开) Change 检查回纪录点时身上有无登出後消失道具,若有,不可回纪录点
#define _FM_MODIFY                  // 家族功能修改(开放)
#define _PETSKILL_OPTIMUM           // Robin 宠物技能处理最佳化
#define _MAGIC_OPTIMUM              // Robin Magic table 最佳化
#define _AC_PIORITY                 // Nuke 提高AC的优先权(可开)
#define _MAP_TIME                   // 特殊地图,会倒数(开放)
#define _ITEM_ADDPETEXP             // (可开) Change 增加宠物经验值道具(可吃此道具宠物,平常不可获得经验值)
#define _TRANS_6                    // (可开) Change 人物6转
#define _PET_2TRANS                 // (可开) Change 宠物2转
#define _PET_2LIMITLEVEL            // (可开) Change 帮助宠物2转的玛蕾
#define _MAGICSTAUTS_RESIST         // (可开) Change (火电冰)抗性精灵
#define _SUIT_TWFWENDUM             // (可开) Change 四属性套装功能 (_SUIT_ITEM要开)
#define _EXCHANGEMAN_REQUEST_DELPET // (可开) Change 替exchangeman的TYPE:REQUEST增加删除宠物
#define _DEL_DROP_GOLD              // 删除地上过时的石币  Robin
#define _ITEM_STONE                 // cyg 奇怪的石头
#define _HELP_NEWHAND               // cyg 新创角色给予新手道具)
#define _DEF_GETYOU                 // cyg GM指令 查询自己两格  围内的玩家帐号
#define _DEF_NEWSEND                // cyg GM指令 传送玩家时可加句子也可不加
#define _DEF_SUPERSEND              // cyg GM指令 可传送已某帐号为中心附近3格内所有玩家
#define _SUIT_ADDPART3              // (可开) Change 套装功能第叁弹 回避提高% 沉默抗性提高% 敏提高% 防提高% 攻提高%
#define _PETSKILL_2TIMID            // (可开) Change 怯战第二版(狂狮怒吼)
#define _TIME_TICKET                // Robin 限时入场 森林捉宠用
#define _HALLOWEEN_EFFECT           // 万圣节特效
#define _ADD_reITEM                 // (可开) Change 检查道具栏位空格
#define _ADD_NOITEM_BATTLE          // 新增NPC检查人物身上没道具会进入战斗
#define _PETSKILL_BATTLE_MODEL      // 宠物技能战斗模组
#define _PETSKILL_ANTINTER          // (可开) Change 宠技:蚁之葬 参考_PETSKILL_2TIMID
#define _PETSKILL_REGRET            // (可开) Change 宠技:憾甲一击 参考_SONIC_ATTACK
#define _FIX_FAMILY_PK_LIMIT        // (可开放) WON 将可挑战庄园的由前十大改为前二十大
#define _NEWOPEN_MAXEXP             // (可开) ANDY MAX经验值
#define _CHAR_FIXDATADEF            // (可开放) ANDY 修正人物资料初始
#define _NPC_EXCHANGEMANTRANS       // (可开) Change 替exchangeman加入判断转生
#define _ADD_NEWEVENT               // (可开) 新任务旗标
#define _BATTLE_NEWPOWER            // (可开放)
//眠啊郴侩
#define _CHAR_INFORMATION // 某腐磐 沥焊 犬牢												BY. 窍疵狼 胶沛俊捞瘤
#define _CHAR_PETDATA     // 其飘 殿鞭 拌魂 棺 历厘									BY. 窍疵狼 胶沛俊捞瘤
#define _ITEM_RESETPET    // 其飘 檬扁拳 备浇												BY. 窍疵狼 胶沛俊捞瘤
#define _CHAR_PARTY       // 府歹 困烙, 颇萍 曼啊,呕硼 疙飞绢				BY. 窍疵狼 胶沛俊捞瘤
#define _CHAT_MERGE       // 钦己 酒捞袍 积己(瓷仿摹 何咯) 疙飞绢		BY. 窍疵狼 胶沛俊捞瘤
#define _CHAR_POINT       // 阿 器牢飘 沥焊 (固肯)									BY. 窍疵狼 胶沛俊捞瘤
#define _UP_ON_OFF        // 版氰摹 备浇 ONOFF 疙飞绢 							BY. 窍疵狼 胶沛俊捞瘤
#define _ITEM_NOENEMY     // 寒没狼 家扼 ONOFF 酒捞袍, 疙飞绢				BY. 窍疵狼 胶沛俊捞瘤
//#define _NEW_CHAR_RIDECODE		// 某腐磐 积己 矫 扼捞靛 内靛 何咯				BY. 窍疵狼 胶沛俊捞瘤
#define _HIGHRANK_PETMAKE    // 己厘伏(鞭)捞 臭篮 其飘 积己						BY. 窍疵狼 胶沛俊捞瘤
#define _ITEM_NAMECOLOR      // 酒捞袍 捞抚 祸惑 汲沥									BY. 窍疵狼 胶沛俊捞瘤
#define _HP_HEAL             // 眉仿 胶泡 厚肥 郴备仿 雀汗							BY. 窍疵狼 胶沛俊捞瘤
#define _1LV_ENEMYENCOUNT    // 1饭骇 其飘 免泅 皋技瘤									BY. 窍疵狼 胶沛俊捞瘤
#define _CHAR_LEVELUPMSG     // 某腐磐 饭骇诀 皋技瘤, 郴备仿 雀汗			BY. 窍疵狼 胶沛俊捞瘤
#define _PET_LEVELUPMSG      // 其飘 饭骇诀 皋技瘤, 郴备仿 雀汗				BY. 窍疵狼 胶沛俊捞瘤
#define _CHAR_GETITEMMSG     // 酒捞袍 裙垫 皋技瘤(颇萍盔)							BY. 窍疵狼 胶沛俊捞瘤
#define _NPC_ACTIONCONDITION // 咀记 浚乔矫 馆览 炼扒(颇萍, 饭骇 30)		BY. 窍疵狼 胶沛俊捞瘤
#define _NPC_ACTIONWARP      // 咀记 浚乔矫 况橇 矫胶袍								BY. 窍疵狼 胶沛俊捞瘤
//#define _CHAR_LEVELACHIEVE		// 饭骇 崔己 器牢飘												BY. 窍疵狼 胶沛俊捞瘤
#define _ITEM_UNCONFIRMED // 固犬牢 酒捞袍													BY. 窍疵狼 胶沛俊捞瘤
#define _ITEM_METAMOGET   // 馒侩屈 函脚酒捞袍(单捞磐 历厘)					BY. 窍疵狼 胶沛俊捞瘤
#define _CHAR_LIMITFLOOR  // 啪铰力茄 备开 矫胶袍										BY. 窍疵狼 胶沛俊捞瘤
#define _CHAR_PARTAUTO    // 磊悼 颇萍 概莫
#define _CHAR_EVENTPOINT  //BY. 窍疵狼 胶沛俊捞瘤
#define _CHAT_GETVIPPOINT
#define _LEADERBOT_COMBO
#define _CHAR_INFOSTORAGE
#define _CHAR_TRANSPOINT
#define _ITEM_SELLNUMERROR
#define _ITEM_RANDOMBOX // 犬伏 技何炼沥 冠胶
#define _GM_HELPER      //
#define _ITEM_ADDPOINT
#define _ITEM_UPGRADE // 酒捞袍 碍拳
#endif