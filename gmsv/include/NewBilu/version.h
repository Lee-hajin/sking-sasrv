#ifndef __ALLBLUES_H__
#define __ALLBLUES_H__

//2018.03.19����������겢ȥ���󲿷���LUA����ʵ�ֵĹ��ܺ����õĹ���

#define SERVER_VERSIONCF // ������Ϣ��ʾ
#define SERVER_VERSION "�汾��Ϣ��New Version for Linux"
#ifdef SERVER_VERSIONCF
#define SERVER_VERSION1 "��Ȩ��Ϣ��New Version"
#define SERVER_VERSION2 "��˰汾��ver3.0 2019"
#define SERVER_VERSION3 "���汾���� New Version �͹�����ʹ��"
#endif

//#define _DEBUG_ALL												// (�ɿ���) ȫ������
#define _GMSV_DEBUG		// (�ɿ���) GMSV������Ϣ
#define _ABSOLUTE_DEBUG // (�ɿ���) ���Ե���

#define _SA_VERSION 'L' //ʯ���汾��Ĭ����L����ʯ���޸�ΪZ����Ӧlua��dll
#define _DEFAULT_PKEY "www.93sa.com"
#define _RUNNING_KEY "www.93sa.com"
//#define _NB_DEFAULT_KEY											// [2018/03/19]��̬��Կ
#define _SASQL // ����MYSQL����

#define MANOR_MAX_NUM 10	  // (4.0) ׯ԰����
#define FAMILY_MAX_MEMBER 200 // ��������

//�������汾���� ----------------------------------------
#define _VERSION_NEW // �°汾
//#define _VERSION_182    									// 1.82�汾(δ���) ֧��1.82�ͻ��ˣ���¼�����KEY��δ�޸�

#define NEWDISPATC
#define _NEW_FONT			  // (�ɿ���) ��Ӧ������ɫ���Ӻ����Ӧ�ͻ���
#define _STOPLOGININFORMATION // (�ɿ���) ������رմ󲿷ֵ�¼��Ϣ
#define _DUELTIME24HOUR		  // (�ɿ���) ��ս�ӳٿ�սʱ��24Сʱ
//#define _ASSESS_SYSDOWNUPNEWS							// (�ɿ���) ����ϴ�������Ϣ �رպ����

//�°汾8.0��key���ͻ�����Ҫ��Ӧ
#ifdef _VERSION_NEW
#define _CHATROOMPROTOCOL // (�ɿ���) Syu ADD ������Ƶ�� 8/27
#ifdef _CHATROOMPROTOCOL
#define _NPC_SAVEPOINT // (�ɿ���) ��¼�����������
#endif
#define _OBJSEND_C			 // (�ɿ���) ANDY _OBJSEND_C
#define _ADD_SHOW_ITEMDAMAGE // (�ɿ���) WON  ��ʾ��Ʒ�;ö�
#define _SHOW_ITEM			 // (�ɿ���) ��װ��
#define _SHOW_ITEM_NAME		 // (�ɿ���) ��װ�������
#define _CHANGETRADERULE	 // (�ɿ���) Syu ADD ���׹����޶�
#define _TRADE_SHOWHP		 // (�ɿ���) Syu ADD ������ʾѪ��
#define _TRADESYSTEM2		 // (�ɿ���) Syu ADD �½���ϵͳ
#ifdef _TRADESYSTEM2
#define _TRADE_PK // (�ɿ���) ����PK
#endif
#define _VIP_POINT_PK // (�ɿ���) ���ֵ�PK

#define _NPC_MAKEPAIR		  // (�ɿ���) ANDY NPC ���
#define _ITEM_PILENUMS		  // (�ɿ���) ANDY �����زĶѵ� �� itemset4
#define _ITEM_PILEFORTRADE	  // (�ɿ���) ANDY ���׶ѵ�
#define _ITEM_EQUITSPACE	  // (�ɿ���) ANDY ��װ����λ
#define _EQUIT_ARRANGE		  // (�ɿ���) ANDY ���� ��Ҫ itemset5.txt
#define _EQUIT_ADDPILE		  // (�ɿ���) ANDY ���ӿɶѵ��� ��Ҫ itemset5.txt
#define _EQUIT_NEWGLOVE		  // (�ɿ���) ANDY ������λ
#define _NEWREQUESTPROTOCOL	  // (�ɿ���) Syu ADD ����ProtocolҪ��ϸ��
#define _OUTOFBATTLESKILL	  // (�ɿ���) Syu ADD ��ս��ʱ����Protocol
#define _NPC_ADDLEVELUP		  // (�ɿ���) ANDY NPC������ҵȼ�
#define _CHAR_NEWLOGOUT		  // (�ɿ���) ANDY �ǳ��»���
#define _NPC_EXCHANGEMANTRANS // (�ɿ���) Change ��exchangeman�����ж�ת��
#define _CHANNEL_MODIFY		  // (�ɿ���) Ƶ����������
#define _STREET_VENDOR		  // (�ɿ���) ��̯����
#ifdef _STREET_VENDOR
#define _STREET_VENDOR_TRANS  // (�ɿ���) ��̯����ת��
#define _STREET_VENDOR_EFFECT // (�ɿ���) ��̯������ʾ
#define _CANCEL_STREET_VENDOR // (�ɿ���) ��ֹ����̯
#define _MULTI_STREETVENDOR	  // (�ɿ���) �๦�ܰ�̯
#define _STREET_VENDOR_FAX	  // (�ɿ���) ��̯���ؼ�˰��
#ifdef _SASQL
#define _ONLINE_SHOP_MYSQL	 // (�ɿ���) �����̳Ǹĳ�MYSQL�ػر�
#define _STREET_VENDOR_MYSQL // (�ɿ���) MYSQL VipPoint�Ұ�̯
							 //#define _NB_SQL_EnemyBase								// [2018.07.03]��enemybase������sql
							 //#define _NB_SQL_Enemy										// [2018.07.03]��enemy������sql
#endif
#endif
#define _WATCH_EVENT  // (�ɿ���) ��ѯ������
#define _ADD_DUNGEON  // (�ɿ���) Change ׷�ӵ���
#define _ADD_STATUS_2 // (�ɿ���) ������������״̬�ڶ���
#define _ALCHEMIST	  // (�ɿ���) Robin ��������
#define _JOBDAILY	  // (�ɿ���) cyg ������־����
#define _FONT_SIZE	  // (�ɿ���) Robin ���ʹ�С����
#ifdef _FONT_SIZE
#define _TALK_SIZE_ITEM // (�ɿ���) ����������
#endif
#define _PETSKILL_FIXITEM // (�ɿ���) ANDY �޸� && ͬ������
#define _CHECK_ITEMDAMAGE // (�ɿ���) ����������  7/26
#define _TEAM_KICKPARTY	  // (�ɿ���) ANDY �ӳ�����(��ս��״̬)
#define _TELLCHANNEL	  // (�ɿ���) Syu ADD ����Ƶ��
#define _EMENY_CHANCEMAN  // (�ɿ���) ANDY EMENY ѡ��
#define _STANDBYPET		  // (�ɿ���) Robin ��������
#define _CHAR_POOLITEM	  // (�ɿ���) ANDY ������ֿ߲⹲��
#define _NPC_DEPOTITEM	  // (�ɿ���) ANDY ������ֿ߲⹲��
#define _CHAR_POOLPET	  // (�ɿ���) Robin �������ֿ⹲��
#define _NPC_DEPOTPET	  // (�ɿ���) Robin �������ֿ⹲��
#define _NEW_MANOR_LAW	  // (�ɿ���) ��ׯ԰����
#ifdef _NEW_MANOR_LAW
#define _MOMENTUM_NPC // (�ɿ���) ���ƽű�
#endif
#define _TAKE_ITEMDAMAGE // (�ɿ���) ANDY ������  ITEMSET2
#define _SKILL_NOCAST	 // (�ɿ���) vincent  �輼:��Ĭ //��Ҫ��#define _MAGIC_NOCAST
#define _PET_FUSION		 // (�ɿ���) ANDY �����ں� enemybase1.txt
#ifdef _PET_FUSION
#define _FUSIONBEIT_TRANS // (�ɿ���) [CF����]�ںϳ���ת��
#define _FUSIONBEIT_FIX	  // (�ɿ���) [GM����]ȡ���ںϳ������
#define _NPC_FUSION		  // (�ɿ���) ANDY NPC�����ں�
#endif
#define _ITEM_EDITBASES		  // (�ɿ���) ANDY Ӫ����
#define _PET_EVOLUTION		  // (�ɿ���) ANDY ������
#define _STATUS_WATERWORD	  // (�ɿ���) ANDY ˮ����״̬
#define _ITEM_WATERWORDSTATUS // (�ɿ���) ANDY ˮ����״̬����
#define _PETSKILL_ACUPUNCTURE // (�ɿ���) Change �輼:�����Ƥ (client�˵�_PETSKILL_ACUPUNCTUREҲҪ��)
#define _SHOW_FUSION		  // (�ɿ���) Change �ںϳ�����CLIENT��ֻ��ʾת����,�޸�Ϊ�ںϳ���ʾΪ�ں�(client��ҲҪ��)
#define _TREASURE_BOX		  // (�ɿ���) ANDY ����
#define _SHOOTCHESTNUT		  // (�ɿ���) Syu ADD �輼��������
#define _DP_NPC				  // (�ɿ���) DP�ű�

#define _ANGEL_SUMMON // (�ɿ���) Robin �����ٻ�
#ifdef _ANGEL_SUMMON
#define _ANGLE_EMAIL		// (�ɿ���) ���ٺ��Զ�������Ƭ
#define _ANGEL_TIME			// (�ɿ���) [CF����]�Զ��徫���ٻ�ʱ��
#define _CANCEL_ANGLE_TRANS // (�ɿ���) ȡ�������ٻ�
#endif
#define _PREVENT_TEAMATTACK	 // (�ɿ���) ANDY ��ֹͬ�ӻ��� 12/04 ����
#define _SKILLLIMIT			 // (�ɿ���) Syu ADD ���ù����ҷ�����
#define _PROFESSION_ADDSKILL // (�ɿ���) Change ׷��ְҵ����
#define _CHAR_PROFESSION	 // (�ɿ���) WON ADD ����ְҵ��λ
#define _PROFESSION_SKILL	 // (�ɿ���) WON ADD ����ְҵ����
#ifdef _PROFESSION_SKILL
#define _NPC_WELFARE		 // (�ɿ���) WON ADD ְҵNPC
#define _PROSKILL_OPTIMUM	 // (�ɿ���) Robin ְҵ���ܴ�����ѻ�
#define _MAGIC_RESIST_EQUIT	 // (�ɿ���) WON ADD ְҵ����װ��
#define _NPC_ProfessionTrans // (�ɿ���) Change ��ְҵNPC�����ж�ת��
#define _FIX_MAGIC_RESIST	 // (�ɿ���) Change ְҵħ������
#endif
#define _ALLDOMAN // (�ɿ���) Syu ADD ���а�NPC
#ifdef _ALLDOMAN
#define _ALLDOMAN_DEBUG // (�ɿ���) Change ��������server�����а���ʧbug
#endif
#define _REDHAT_V9		// (�ɿ���) Change ��REDHAT9.0�İ汾�±���ʱ��
#define _PETSKILL_LER	// (�ɿ���) �׶�����
#define _EQUIT_RESIST	// (�ɿ���) Change ��һװ������(�ɿ���һ�����쳣״̬)�п��� �� �� ���� ħ�� ��Ĭ ����
#define _SUIT_ADDPART4	// (�ɿ���) Change ��װ���ܵ��ĵ� �ж�����% ��ǿ��ʦħ��(����30%) �ֿ���ʦ��ħ��
#define _PROSK99		// (�ɿ���) Change ���ְ���ȼ��Ͱ���
#define _ITEM_TYPETABLE // (�ɿ���) Change ׷��typetable
#define _RIGHTCLICK		// (�ɿ���) Robin �����Ҽ�����
#define _TEACHER_SYSTEM // (�ɿ���) ��ʦϵͳ
#define _PET_ITEM		// (�ɿ���) ����װ��
#define _ONLINE_SHOP	// (�ɿ���) 8.0�����̳�
#define _FM_POINT_PK	// (�ɿ���) ׯ԰���廥��ׯ԰

#define _TAKE_ITEMDAMAGE_FIX	 // (�ɿ���) ����װ����
#define _TAKE_ITEMDAMAGE_FOR_PET // (�ɿ���) ��װװ����
#define _NEW_RIDEPETS			 // (�ɿ���) ANDY �����
#ifdef _NEW_RIDEPETS
#define _RIDEMODE_20	// (�ɿ���) 2.0���ģʽ
#define _GM_RIDE		// (�ɿ���) [GM����]�������Ȩ��
#define _RIDE_CF		// (�ɿ���) �Զ������
#define _FM_LEADER_RIDE // (�ɿ���) �峤ר�����
#define _ADD_RIDE_CF	// (�ɿ���) �����Զ����������
#endif

#define _VIP_SHOP	  // (�ɿ���) ��Ա�̵�
#define _NEW_VIP_SHOP // (�ɿ���) ��Ա�̵�(��SAAC���)
#ifdef _NEW_VIP_SHOP
#define _COST_ITEM // (�ɿ���) ��ֵ����
#endif
#define _TRANS		 // (�ɿ���) ����ת������
#define _TALK_ACTION // (�ɿ���) ˵������
//#define _PROSKILL_ERR_KICK							// (�ɿ���) �Ƿ������Զ�T����
#define _ITEM_SETLOVER	// (�ɿ���) ��鹦��
#define _GM_METAMO_RIDE // (�ɿ���) GM���������г�
#define _CHAR_CDKEY		// (�ɿ���) ��ֹ��������
#define _SAVE_GAME_ID	// (�ɿ���) ��¼����ʾ����ķ�������
#define _AUTO_PK		// (�ɿ���) �Զ���PKϵͳ
#ifdef _AUTO_PK
#define _FORMULATE_AUTO_PK // (�ɿ���) �ƶ��Զ���PKϵͳ(��SAAC���)
#endif
#define _GM_KING	  // (�ɿ���) GM��ħ��
#define _PLAYER_MOVE  // (�ɿ���) ���˳�ƹ���
#define _TRANS_7_NPC  // (�ɿ���) ������תNPC
#define _TRANS_7_SHOW // (�ɿ���) ����7ת��ʾ
//#define _DBSAVE_CHAR										// (�ɿ���) ˫��浵
#define _ITEM_COLOER		 // (�ɿ���) ��Ʒ��ɫ����
#define _LOGOUTTIME			 // (�ɿ���) �������ʱ����ʾ
#define _SAMETHING_SAVEPOINT // (�ɿ���) ���ĳЩ�¼��Զ��浵
#define _ITEM_PET_LOCKED	 // (�ɿ���) ��Ʒ������(��SAAC���)
#define _ONLINE_COST		 // (�ɿ���) ���߳�ֵ(��SAAC���)
#define _VIPPOINT_OLD_TO_NEW // (�ɿ���) ���ֵ��ת��(��SAAC���)
#define _ADD_NEWEVENT_1024	 // (�ɿ���) ���������1024
#define _INSLAY_NOT_PILENUMS // (�ɿ���) ���ܾ���������Ʒ
#define _ONLINE_TIME		 // (�ɿ���) ����ʱ��
#define _SUPER				 // (�ɿ���) ���ƶ˵���������֧��
#define _NPC_CHECK_SCRIPT	 // (�ɿ���) �жϽű���

#define _SYSTEM_SPEAD	   // (�ɿ���) ϵͳ����
#define _FILTER_TALK	   // (�ɿ���) ����˵��
#define _COMPOUND_EXP	   // (�ɿ���) �ϳɾ���
#define _ASSESS_ABILITY	   // (�ɿ���) ��������
#define _PAUCTION_MAN	   // (�ɿ���) ����ϵͳ
#define _PLAYER_OVERLAP_PK // (�ɿ���) ����ص�ʱPK
#define _FIMALY_PK_TIME	   // (�ɿ���) ���忪Ƭʱ���趨
#define _FIX_COPY_MM_BUG   // (�ɿ���) �޸�����MM��BUG
#define _FIX_PETSKILL_BUG  // (�ɿ���) �޸����＼���ص�ʹ��
//#define _CHARADATA_SAVE_SQL							// (�ɿ���) ������д��SQL
#define _MISSION_TRAIN		 // (�ɿ���) ������
#define _DEL_DROP_PET		 // (�ɿ���) ɾ�����ϳ���
#define _PET_EVOLVE			 // (�ɿ���) �������
#define _FREE_SAVE			 // (�ɿ���) ��������¼
#define _FIX_ADD_EXP		 // (�ɿ���) �޸����Ӿ������
#define _RECORD_IP			 // (�ɿ���) ��¼IP
#define _FIX_STW_SPEED_ENEMY // (�ɿ���) �޸���ҿ�������
//#define _DEL_NOT_25_NEED_ITEM						// (�ɿ���) ɾ����2.5�����ץ��
#define _LOCK_PET_ITEM	   // (�ɿ���) �󶨳��������
#define _ULTIMATE_ANNOUNCE // (�ɿ���) ����֪ͨ
#define _SHIP_MATEMO	   // (�ɿ���) ���ı�������
#define _FIX_CORE_LOOP	   // (�ɿ���) �޸�CORE_LOOP

#define _PET_MM			   // (�ɿ���) [����]�����ʯMM
#define _FM_ITEM		   // (�ɿ���) [����]��������
#define _LUCK_ITEM		   // (�ɿ���) [����]��������
#define _SILENTZERO		   // (�ɿ���) [����]������Ʒ
#define _ITEM_GOLD		   // (�ɿ���) [����]��Ǯ����Ʒ
#define _MYSTERIOUS_GIFT   // (�ɿ���) [����]��������
#define _ITEM_METAMO_TIME  // (�ɿ���) [����]���Զ�������Ľ�ָ
#define _BATTLE_PK		   // (�ɿ���) [����]ǿ��PK���ߣ����˻ؼ�¼��
#define _PET_LEVEL_ITEM	   // (�ɿ���) [����]����ȼ�ͻ����Ʒ
#define _PET_BEATITUDE	   // (�ɿ���) [����]����ף��
#define _NEW_PET_BEATITUDE // (�ɿ���) [����]��һ������ף��
#define _ITEM_EFMETAMO	   // (�ɿ���) [����]���ñ�����ָ
#define _ITEM_RIDE		   // (�ɿ���) [����]ѱ��װ��
#define _GET_MULTI_ITEM	   // (�ɿ���) [����]һ�θ������Ʒ
#define _SUPER_MAN_ITEM	   // (�ɿ���) [����]��Ʒ�˵���
#define _ITEM_EXP		   // (�ɿ���) [����]�������
//#define _PET_ADD_EXP										// (�ɿ���) [����]���ﾭ����������
#define _MEMORY_ITEM   // (�ɿ���) [����]�����ָ
#define _ITEM_POOLITEM // (�ɿ���) [����]�������ֿ߲�
#define _ITEM_POOLPET  // (�ɿ���) [����]��������ֿ�
#define _SHOW_PET_ABL  // (�ɿ���) [����]��ʾ��������
#define _NEWEVENT_ITEM // (�ɿ���) [����]�����������
#define _OLYMPIC_TORCH // (�ɿ���) [����]����ƥ�˻��

#define _CREATE_MM_1_2	  // (�ɿ���) [GM����]������Ʒ���ٷ���
#define _SendTo			  // (�ɿ���) [GM����]���͵��������
#define _LOCK_IP		  // (�ɿ���) [GM����]����IP
#define _RELOAD_CF		  // (�ɿ���) [GM����]�ض�CF
#define _MAKE_MAP		  // (�ɿ���) [GM����]��ͼ����
#define _MAKE_PET_CF	  // (�ɿ���) [GM����]�Զ������ɳ�
#define _MAKE_PET_ABILITY // (�ɿ���) [GM����]�Զ��������Χ
#define _GM_SAVE_ALL_CHAR // (�ɿ���) [GM����]GM����浵

#define _RE_GM_COMMAND	// (�ɿ���) [Data]�Զ���GM��������
#define _NPC_GET_PETABI // (�ɿ���) [Data]NPC���Զ���ɳ��ĳ���
//#define _NEED_ITEM_ENEMY								// (�ɿ���) [Data]ץ��������Ʒ�Զ���
#define _NPC_SCRIPT // (�ɿ���) [Data]NPC�ű���
#ifdef _NPC_SCRIPT
#define _NPC_CHECKTIME	// (�ɿ���) [Data]NPCʱ���ж�
#define _FAME_CHECK_NPC // (�ɿ���) [Data]NPC�����ж�
#ifdef _SASQL
#define _SQL_BUY_FUNC // (�ɿ���) [Data]SQL������
#endif
#define _DEL_FAME	 // (�ɿ���) [Data]�ۼ�����
#define _NPC_INT_SET // (�ɿ���) [Data]NPC��������
#endif
#define _MANOR_EQUIP	 // (�ɿ���) [Data]ׯ԰ר����װ
#define _ITEM_UPLEVEL	 // (�ɿ���) [Data]װ������
#define _ITEM_PERCENTAGE // (�ɿ���) [Data]������Ʒ�ӳɰٷֱ�

#define _GET_BATTLE_EXP	  // (�ɿ���) [CF����]���鱶������
#define _USER_EXP_CF	  // (�ɿ���) [CF����]�û��Զ��徭��
#define _TRANS_LEVEL_CF	  // (�ɿ���) [CF����]ת����Խ160������
#define _UNLAW_WARP_FLOOR // (�ɿ���) [CF����]��ֹ���͵���
#define _UNREG_NEMA		  // (�ɿ���) [CF����]��ֹ��������
#define _WATCH_FLOOR	  // (�ɿ���) [CF����]�Զ���ɹ�ս��ͼ
#define _BATTLE_FLOOR	  // (�ɿ���) [CF����]�Զ���ǿ��ս����ͼ
#define _SKILLUPPOINT_CF  // (�ɿ���) [CF����]�Զ�����������
#define _RIDELEVEL		  // (�ɿ���) [CF����]�Զ���������ȼ�
#define _REVLEVEL		  // (�ɿ���) [CF����]�Զ���ԭ�ȼ�
#define _FIX_CHARLOOPS	  // (�ɿ���) [CF����]������ħ��ʯ����ʱ��
#define _BATTLE_GOLD	  // (�ɿ���) [CF����]��ȡ����ͬʱ��ý�Ǯ
#define _PLAYER_NUM		  // (�ɿ���) [CF����]�����������
#define _ENEMY_ACTION	  // (�ɿ���) [CF����]��Ҳ������л���
#define _PLAYERMAXPOINT	  // (�ɿ���) [CF����]��������
#define _CHECK_PEPEAT	  // (�ɿ���) [CF����]�Ƿ�������(����Ƿ���ͬ����ĳ�������Ƶ�)
#define _PET_AND_ITEM_UP  // (�ɿ���) [CF����]�ɼ���˶����ĳ�������
#define _FM_JOINLIMIT	  // (�ɿ���) [CF����]������ٴμ�������ʱ������
#define _LOGIN_DISPLAY	  // (�ɿ���) [CF����]���������ʾ����
#define _AUTO_DEL_PET	  // (�ɿ���) [CF����]�Զ�ɾ������
#define _AUTO_DEL_ITEM	  // (�ɿ���) [CF����]�Զ�ɾ����Ʒ
#define _POOL_ITEM_BUG	  // (�ɿ���) [CF����]��ֹǿ���BUG
#define _NO_JOIN_FLOOR	  // (�ɿ���) [CF����]��ֹ���͵���
//#define _TRANS_POINT_UP									// (�ɿ���) [CF����]ÿת��������
#define _UNLAW_THIS_LOGOUT // (�ɿ���) [CF����]��ֹԭ�ǵĵ�ͼ
#define _NO_STW_ENEMY	   // (�ɿ���) [CF����]��ֹ��ҿ�������
#define _PET_TRANS_ABILITY // (�ɿ���) [CF����]����ת��������������

#define _POINT_AMPOINT // (�ɿ���) [����]���ֵ���

#define _OPEN_E_PETSKILL	 // (�ɿ���) [�輼]���ų���E����
#define _ONE_PET_SKILL		 // (�ɿ���) [�輼]һ���Գ輼
#define _NEW_SKILL			 // (�ɿ���) [�輼]�¼��ܿ���
#define _EXPANSION_VARY_WOLF // (�ɿ���) [�輼]��չ���±���
#ifdef _NEW_SKILL
#define _ITEM_ATTSKILLMAGIC		// (�ɿ���) [�輼]ANDY ���߼���
#define _MASSAGE_PETSKILL		// (�ɿ���) [�輼]��ɱ������
#define _EXPANSION_ITEM_INSLAY	// (�ɿ���) [�輼]��չ�ⱦʯ
#define _STRENGTH_PETSKILL		// (�ɿ���) [�輼]Ԫ���輼
#define _EXPANSION_POWERBALANCE // (�ɿ���) [�輼]��չPETSKILL_PowerBalance
#define _RESURRECTION_PETSKILL	// (�ɿ���) [�輼]����輼
#define _LOSTLOST_PETSKILL		// (�ɿ���) [�輼]���ܾ���
#define _GRAPPLING_PETSKILL		// (�ɿ���) [�輼]������
#define _PETSKILL_EXPLODE		// (�ɿ���) [�輼]Change �輼:���ѹ���
#define _PETOUT_PETSKILL		// (�ɿ���) [�輼]����Ϣ�輼
//#define _INVERSION_PETSKILL								// (�ɿ���) [�輼]��ת����
#define _FIX_PETSKILL_HECTOR // (�ɿ���) [�輼]�޸��ü����޷�����turn
#endif

#define _THE_WORLD_SEND	 // (�ɿ���) [����]����˵��Ƶ��
#define _ALL_SERV_SEND	 // (�ɿ���) [����]����Ƶ��
#define _SUPER_FLOOR_MIC // (�ɿ���) [����]��ͼ��˷�

#define _AMPOINT_LOG	  // (�ɿ���) [LOG]���ֵ��¼
#define _SQL_VIPPOINT_LOG // (�ɿ���) [LOG]�ػرҼ�¼

#define _ALLBLUES_LUA // (�ɿ���) ʹ��LUA�ű�
#ifdef _ALLBLUES_LUA
#define _ALLBLUES_LUA_1_1		 // (�ɿ���) LUA1.1��
#define _ALLBLUES_LUA_1_2		 // (�ɿ���) LUA1.2��
#define _ALLBLUES_LUA_1_3		 // (�ɿ���) LUA1.3��
#define _ALLBLUES_LUA_1_4		 // (�ɿ���) LUA1.4��
#define _ALLBLUES_LUA_1_5		 // (�ɿ���) LUA1.5��
#define _ALLBLUES_LUA_1_6		 // (�ɿ���) LUA1.6��
#define _ALLBLUES_LUA_1_7		 // (�ɿ���) LUA1.7��
#define _ALLBLUES_LUA_1_8		 // (�ɿ���) LUA1.8��
#define _ALLBLUES_LUA_1_9		 // (�ɿ���) LUA1.9��
#define _PETSKILL_SHOP_LUA		 // (�ɿ���) ���＼���̵�LUA
#define _OFFLINE_SYSTEM			 // (�ɿ���) ����ϵͳ
#define _PLAYER_NPC				 // (�ɿ���) ������
#define _DATA_INFO_SHOW			 // (�ɿ���) ���ְ�ť�ص�
#define _LUA_BATTLEOVERCALL		 // [2018.05.26]ս��ص�
#define _LUA_PROFESSIONSKILLCALL // [2018.06.06]ְҵ���ܻص�
#define _LUA_CHECKPETMAIL		 // [2018.08.25]�����ʼ�LUA�ص�
#define _LUA_CHECKPARTYOUT		 // [2018.08.25]�Ŷ�����LUA�ص�
#define _NB_LUA_MovePetItem		 // [2018.09.05]����װ��LUA�ص�
#define _NB_LUA_BattlePetSkill	 // [2018.09.17]ս���輼LUA�ص�
#define _NB_LUA_MAGIC_MP_RATE	 // [2018.10.02]����ʱ�����ص�
#endif
/*-----------------------------------------------------------------*/
#endif

#define _FLOOR_DIFI_FIELD  // ¥���ֶ�
#define _FM_FMPOINTPK_LIST // ׯ԰��ս�б�
#define _BATTLE_BOUT_TIME  // ս���غ�ʱ��
#define _ACTIVE_GAME	   // ����ֵ
#define _NO_HELP_MAP	   // ��ֹHELP
#define _BATTLE_TIME	   // ս��ʱ��
#define _VIP_TRANS		   // VIPת������
//#define _FIX_CHAR_LOOP				// ��������ѭ��
#define _FIX_SAVE_CHAR		   // �������浵����
#define _FIX_P_VS_P_PARENT	   // ����PVP����ս��
#define _FMPOINT_PKTIME_SELECT // ����ѡ����սʱ��
#define _CHECK_SEVER_IP		   // �жϷ�����IP
#define _DAMMAGE_CALC		   // �Զ����˺�
#define _PLAYER_TITLE		   // ��ҳƺ�
#define _VIP_PET_TRANS		   // ��Աת��
//#define _UNIFIDE_MALINASI			// ͳһ������˿������
//#define _NO1_B2_NONO					// B2ѧ�ڵ�һλʱ�Զ�����21��
//#define _FMPOINT_APPEND_ABI		// ׯ԰�����Ա��������
#define _ITEM_OVER_LAP		   // �����ϷŹ���
#define _NEW_LOAD_NPC		   // ��ȡNPC
#define _NO_CLS_EVENT_255	   // �����255������
#define _BATTLE_OVER_STAR_TIME // ս�������¿�ʼʱ����
#define _MISSION_TIME		   // �����ʱ
#define _MVP_LOCK_JP		   // ��MVP��Ʒ��
//#define _FMPOINTPK_FOR_TWO		// ÿ����ս���Ϊ����
#define _BURST_LOG		 // �ֶ���־
#define _MASK_ENCOUNTER	 // ����ԭ�ط��
#define _NEWCLISETMAC	 // ��¼�ͻ��˵�MAC��ַ
#define _NEWCLISETSERVID // ��¼�ͻ��˷�����ID
//#define _FMPOINT_PK_MESSAGE		// ��սȫ��ͨ��
#define _REGISTRATION_TIME // ǩ��ʱ��
#define _FAME_REG_TIME	   // ����ǩ��ʱ��
#ifdef _OFFLINE_SYSTEM
#define _OFFLINE_TIME // ����ʱ��
#endif
#define _ITEM_USE_TIME	// ����ʹ��ʱ��
#define _PET_FUSION_LUA // �ںϳ�LUA
//#define _PET_ENEMY_DEVELOP_UP // ȫ����������ɳ�
#define _FIRST_LOCK_ITEM	 // ��һ�ν��׺��Զ�����Ʒ
#define _CTRL_TRANS_DEVELOP	 // ���Ƴɳ�
#define _ABDUCT_NO_PETOUT	 // �ó̻���ֱͬʱ����
#define _ATTACK_RAND_SORT	 // ����˳�����
#define _FM_POINT_APPLY_FAME // ׯ԰������������
//#define _NO_GM_MAGIC					// ȥ������Ҫ��GMָ��
#define _FM_LIAOLI	   // �������Ϳ������������ҹ�Ч����
#define _ITEM_EXP_LV   // ��ԭԭ���ľ��鵤
#define _SKILL_25_FIX  // 2.5�ͻ��˼����޸��ͻ�����ʾ
#define _PETUP_GET_EXP // ��������ǿ�ƶ���131-140���飬131��ǰ�������60000
//#define _OFFLINE_BATTLE_RETEUN	// ս����������
//#define	_NO_EVENT_FAME					// ������(�����������ϳ�)���Ӹ�������
#define _PLAYER_LEVELUP_FAME // ��������ʱ��������ĳɺͳ���һ��
//#define _NOT_FMPK_WAIT					// ��ս����ս��
#define _NO_ENEMYID_GROUP // CF��ָ������ID��������
#define _NEW_MAP_NO_FAME  // ���ý���ָ����ͼ������û������
#define _TALK_AREA_CF	  // CF�ж���˵������������Сֵ
//#define	_OFFLINE_RETEUN				// �����Զ�����
#define _FAMILY_PK_FIX // �޸�������������
//#define	_CREATE_TRANS_SAVEPOINT	// ��������� �����ʽ��� �̵� CHAR_SAVEPOINT
#define _LV_1_NO_ESCAPE // �趨1�����ﲻ����
//#define	_P_THREAD_RUN					// ���߳�ģʽ
#define _RIDE_ATT_PET_SKILL // �޸���սͬ�輼��BUG
//#define	_BATTLE_RAND_DEX				// ��������

#define FAMLYNUM_CF				// (2018/04/06)�Զ���ׯ԰��Ŀ
#define _NB_SEND_PETINDEX		// (2018/04/08)K������ͳ�������
#define _NB_PETINITBILITY		// (2018/04/08)��¼�����ʼֵ
#define _NB_STREETVENDORPETDATA // (2018/04/11)��̯��Ϣ�������Q����
#define _NB_TMWORKVER			// (2018/04/12)����10����ʱ�������ִ�(��ʱ����1~10,��ʱ�ִ�1~10)
#define _NB_PETREPOINT			// (2018/04/13)��¼�����¯ֵ
#define _NB_PETKILL_COST		// (2018/04/25)�����̵�ʯ�Ҽ������۸��趨
#define _NB_ALLOW_FMPK			// (2018/05/01)�Զ�����ս���أ�AllowFMPK=1 (0�ر�, 1����), FMPKNoMsg=����·���������߹ݣ��뵽��ս��·�����߹ݣ�

#define _PET_FUSION_2	  // [2018/04/23]�����ںϣ��ںϵڶ���
#define _NB_TLPOINT		  // [2018/05/06]������������
#define _NB_ITEM_ADDEXP	  // [2018/05/06]�����������鿨
#define _NB_DRAGON_FUSION // [2018/05/09]2D��������
#define _NB_CF_SHARE_EXP  // [2018/05/09]�����Ƿ���
//#define _FIX_FM_FMPOINT					// [2018/05/15]�޸�ׯ԰�ݵ���ʾ
#define _NB_GM_SERVERLIST  // [2018/05/20]���ָ��
#define BOSS_AI			   // [2018/05/27]BOSSAI��ǿ
#define _NB_SILENT_SWITCH  // [2018/06/13]���Կ���
#define _NB_NOBATTLEBUTTON // [2018/06/15]ս����ť���ط��

#define _PETSKILL_PROVOKEFIGHT // [2018/06/17]���Ϸ���
#define _PETSKILL_ADDATTCRAZED // [2018/06/17]ΪPETSKILL_AttackCrazed����״̬����
#define _PRO3_ADDSKILL		   // [2018/06/17]����ְҵ�Ľ׼���
#define _PETSKILL_RIDE		   // [2018/06/17]������
#define _PRO_ABSOLUTE_DEFENSE  // [2018/06/17]���Է���
#define _PRO_KILLME			   // [2018/06/17]�����յ�
#define _PETSKILL_ISULTIMATE   // [2018/06/17]PETSKILL_StatusChange������ɽ���Ч��

#define _NB_SKIN		// [2018/07/08]Ƥ��ϵͳ
#define _PETSKILL_PLUS1 // [2018/07/27]��ǿ Pet Skill ��������(������), �Լ�PETSKILL_NONE�����쳣(˯��ʯ...)״̬

#define _NB_FIX_POOLITEM	   // [2018/08/08]�޸����ֿ߲������ͷ�
#define _NB_FIX_SPRINTF		   // [2018/08/08]�޸�sprintf������Խ�������BUG
#define _NB_FIX_SHUTDOWN_SAVE  // [2018/08/08]�޸�dump���󱣳������Ϣ��BUG
#define _NB_FIX_RIDE_PET_TRADE // [2018/08/08]�޸�����еĳ����ܽ��׵�BUG
#define _NB_FIX_SOCK_WRITE_OP  // [2018/08/08]�޸���ֹwrite��ʱ���ں˻��������Ĳ���
#define _NB_FIX_MAGIC_NOINDEX  // [2018/08/08]�޸�ħ������ս��indexΪ-1��BUG
#define _NB_FIX_MEMORY		   // [2018/08/14]��д�ڴ����
#define _FLOOR_FUBEN		   // [2018/08/28]����ʱ��
#define _PET_PETZUOBI		   // [2018/08/30]��������ֵ����ֱ��ͨ��LUA���г�����������
//#define _NB_LV1PETBASE					// [2018/09/09]���Ұ��1�����ﲶץ����Χ��Ʒ��

#define _NB_SAVEWININDEX // (2018/10/11)��¼������������

#define _NB_FAMILYBADGE // (2018/10/15)�������
#ifdef _NB_FAMILYBADGE
#define FamilyBadgeStart 36326
#define FamilyBadgeEnd 36336
#endif
//#define _NB_CHARSTRTITLE					// [2018/11/01]���ֳƺ�ϵͳ(���ɺ�ͼ��ƺŹ���)
#define _NB_CHARDISPTITLE // [2018/11/05]ͼ��ƺ�ϵͳ(���ɺ����ֳƺŹ���)
#define _NB_CHARHALO	  // [2018/11/03]�⻷ϵͳ

#ifdef _NB_CHARHALO
//#define _NB_BATTLEHALO						// (2018/11/03)ս������ʾ�⻷
#endif

#define _NB_EVENTDAYTIME	// (2018/12/21)ÿ������ʱ��
#define _NB_MAGICNOEXP		// (2018/12/22)�Ƿ��ֹħ�����Ժ���������  MAGICNOEXP=1(1Ϊ��ֹ������0Ϊ����ֹ)
#define _NB_CreateTJMap		// (2018/12/28)�����ѻ���ͼ��ʽ
#define _NB_FIX_ACUPUNCTURE // (2019/01/06)�޸��������������CLIENT

#define MODIFY_WIZARD_SKILL	 // (2019/01/06)�޸���ʦ����(ʤ˼�޸�)
#define MODIFY_WARRIOR_SKILL // (2019/01/06)�޸İ��Ǽ���(ʤ˼�޸�)
#define MODIFY_HUNTER_SKILL	 // (2019/01/06)�޸����˼���(ʤ˼�޸�)

#define _NB_GETINFO_LSSPROTO // (2019/01/06)��ȡ������Ϣ���

#define _NB_FIX_BATTLE_COM_S_BLOOD // (2019/01/12)������Ѫ100%MISS������
#define _NB_DragonFunsionId		   // (2019/01/12)�Զ���2D����ת���ͱ���趨
#define _NB_EVENTVER			   // (2019/01/13)����������
#define _NB_SAVEVER				   // (2019/01/13)10�������Ĵ洢����

#define _NB_GMCHECKEVENT // (2019/01/21)GM�鿴NPC����������������
#define _NB_NEWDAYEVENT	 // (2018/01/29)��������ʱ��
#define _NB_CDKEYLOCK	 // (2019/02/08)��Ż���(sql����Lock�ֶΣ�ֵΪ0������ֵΪ1�޷���¼��ֵΪ2�޷�����)

#define _ITEM_ATTUPDATA // (2019/02/18)װ��ǿ������
#endif
