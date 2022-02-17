#ifndef __VERSION_H__
#define __VERSION_H__

#define SERVER_VERSION "版本信息：New Version saac64 for Linux "
#define SERVER_VERSIONCF
#ifdef SERVER_VERSIONCF
#define SERVER_VERSION1 "版权信息：New Version"
#define SERVER_VERSION2 "后端版本：ver3.0 2019"
#define SERVER_VERSION3 "本版本仅限 New Version 和工作室使用"
#endif

#define _LEADERFLAG_VER // 嗯、ㄟ、这个.... (可开放)
#define _FAMILY			//  (可开放)
#define _PERSONAL_FAME	// Arminius 8.30 家族个人声望  (可开放)
#define _FMVER21		// CoolFish: 家族族长职务代理人 2001/8/30 (可开放)
#define _CHAR_POOLITEM	// (可开放) ANDY 共用仓库
#define _CHAR_POOLPET	// (可开放) Robin 共用宠物仓库
#define _NewSave		// 修正人物资料存档方式 (可开放)
#define _LEADERFUNCHECK // CoolFish: 2002/02/06 族长功能错误、禅让错误修正(可开放)
#define _WAEI_KICK		// 跨星球踢人(可开放)(北京不必开)
#define _FIX_10_FMPOINT // (可开放) WON ADD 九大庄园
#define _FIX_LEADER_ERR // (可开放) WON ADD 修正族长问题
#define _REMOVE_FM_LOG	// (可开放) WON ADD 移除不必要的家族 LOG
#define _SEND_EFFECT	// (可开放) WON ADD 送下雪、下雨等特效
#define _AC_SEND_FM_PK	// (可开放) WON ADD 庄园对战列表储存在AC
#define _ACFMPK_LIST	// (可开放) ANDY 庄园对战列表
#define _LOCK_ADD_NAME	// LockNode 增加名字栏位(未开放)
#define _SLEEP_CHAR		// Robin 2004/02/12	活跃人物与非活跃人物分开储存
#define _FM_MODIFY		// 家族功能修改
#define _ANGEL_SUMMON	// Robin 精灵的召唤

#define _TEACHER_SYSTEM // 导师系统(未开放)
#define _ALLDOMAN		// (可开放) Syu ADD 排行榜NPC
#define _CHANNEL_MODIFY // 频道功能整合
#define _NEW_MANOR_LAW	// 新庄园规则

//特殊功能控制----------------------------------------
#define _SASQL			 // MYSQL模块
#define _ITEM_PET_LOCKED // 安全码

#define _FORMULATE_AUTO_PK	  // 制定自动化PK系统
#define _OLDPS_TO_MD5PS		  // 把旧的密码转成MD5密码
#define _FIX_CAHR_VERSION	  // 存档版本更改
#define _LOTTERY_SYSTEM		  // 彩票系统
#define FAMILY_MAX_MEMBER 200 // 家族人数
#define _ALL_SERV_SEND		  // 星球频道
//#define _DEATH_FAMILY_LOGIN_CHECK			//删除人数不足之家族或太久未上线之成员
#define _SET_FMPOINT   // 申请庄园时间
#define _FAMILY_PK_FIX // 修复家族下书问题
#ifdef _SASQL
#define _SQL_BACKGROUND		//网页后台功能   需同时开放_SASQL
#define _SQL_REGISTER		//自动注册功能   需同时开放_SASQL
#define _CHARADATA_SAVE_SQL // 将数据写入SQL
#endif
#define _NB_GM_SERVERLIST // 跨服指令
#define _NB_FAMILYBADGE	  //家族徽章
#endif
