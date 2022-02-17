/* output by ../../bin/lsgen.perl 0.41 ( 1998 May)
 * made Thu Feb 17 17:30:27 2000
 * user ringo
 * host indy
 * file /opt/ringo/chsa/src/saac/./saacproto_serv.c
 * util ./saacproto_util.c , ./saacproto_util.h
 * src  /opt/ringo/chsa/src/saac/../../doc/saacproto.html
 */
#define _SAACPROTOSERV_C_
#include "version.h"
#include "saacproto_serv.h"
#include "main.h"

// Nuke : This 'fd' is not a socket, it's the index of structure connect
int saacproto_ServerDispatchMessage(int fd, char *encoded, char *debugfun)
{

	unsigned int msgid;
	char funcname[1024];
	saacproto_splitString(encoded);
	saacproto_GetMessageInfo(&msgid, funcname, sizeof(funcname), saacproto.token_list);
	if (strcmp(funcname, "ACServerLogin") == 0)
	{
		char *servername;
		char *serverpas;
		servername = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		serverpas = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		saacproto_ACServerLogin_recv(fd, servername, serverpas);
		return 0;
	}

	if (strcmp(funcname, "ACServerLogout") == 0)
	{

		saacproto_ACServerLogout_recv(fd);
		return 0;
	}

	if (strcmp(funcname, "ACCharList") == 0)
	{
		char *id;
		char *pas;
		char *ip;
		char *mac1;
		char *mac2;
		char *mac3;
		int mesgid;
		int charlistflg;

		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		pas = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		ip = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		mac1 = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		mac2 = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		mac3 = saacproto_wrapStringAddr(saacproto_stringwrapper[6], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[7]));
		mesgid = saacproto_demkstr_int(saacproto.token_list[8]);
		charlistflg = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACCharList_recv(fd, id, pas, ip, mac1, mac2, mac3, mesgid, charlistflg);
		return 0;
	}

	if (strcmp(funcname, "ACCharLoad") == 0)
	{
		char *id;
		char *pas;
		char *charname;
		int lock;
		char *opt;
		int mesgid;
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		pas = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		lock = saacproto_demkstr_int(saacproto.token_list[5]);
		opt = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		mesgid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACCharLoad_recv(fd, id, pas, charname, lock, opt, mesgid);
		return 0;
	}

	if (strcmp(funcname, "ACCharSave") == 0)
	{
		char *id;
		char *charname;
		char *opt;
		char *charinfo;
		int unlock;
		int mesgid;
#ifdef _NewSave
		int charindex;
#endif
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		opt = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charinfo = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		unlock = saacproto_demkstr_int(saacproto.token_list[6]);
		mesgid = saacproto_demkstr_int(saacproto.token_list[7]);

#ifdef _NewSave
		charindex = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACCharSave_recv(fd, id, charname, opt, charinfo, unlock, mesgid, charindex);
#else
		saacproto_ACCharSave_recv(fd, id, charname, opt, charinfo, unlock, mesgid);
#endif
		return 0;
	}

	if (strcmp(funcname, "ACCharDelete") == 0)
	{
		char *id;
		char *passwd;
		char *charname;
		char *option;
		int mesgid;
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		passwd = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		option = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		mesgid = saacproto_demkstr_int(saacproto.token_list[6]);
		saacproto_ACCharDelete_recv(fd, id, passwd, charname, option, mesgid);
		return 0;
	}

	if (strcmp(funcname, "ACLock") == 0)
	{
		char *id;
		int lock;
		int mesgid;
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		lock = saacproto_demkstr_int(saacproto.token_list[3]);
		mesgid = saacproto_demkstr_int(saacproto.token_list[4]);
		saacproto_ACLock_recv(fd, id, lock, mesgid);
		return 0;
	}

#ifdef _WAEI_KICK
	if (strcmp(funcname, "ACKick") == 0)
	{
		char *id;  //���ߵ��ʺ�
		int flg;   //�¼�flg
		int clifd; //GM
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		clifd = saacproto_demkstr_int(saacproto.token_list[3]);
		flg = saacproto_demkstr_int(saacproto.token_list[4]);
		saacproto_ACKick_recv(fd, id, flg, clifd);
		return 0;
	}
#endif

	if (strcmp(funcname, "ACUCheck") == 0)
	{
		char *mem_id;
		int status;
		mem_id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		status = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACUCheck_recv(fd, mem_id, status);
		return 0;
	}

	if (strcmp(funcname, "DBUpdateEntryString") == 0)
	{
		char *table;
		char *key;
		char *value;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		key = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		value = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		msgid = saacproto_demkstr_int(saacproto.token_list[5]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[6]);
		saacproto_DBUpdateEntryString_recv(fd, table, key, value, msgid, msgid2);
		return 0;
	}

	if (strcmp(funcname, "DBDeleteEntryString") == 0)
	{
		char *table;
		char *key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		key = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		msgid = saacproto_demkstr_int(saacproto.token_list[4]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_DBDeleteEntryString_recv(fd, table, key, msgid, msgid2);
		return 0;
	}

	if (strcmp(funcname, "DBGetEntryString") == 0)
	{
		char *table;
		char *key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		key = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		msgid = saacproto_demkstr_int(saacproto.token_list[4]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_DBGetEntryString_recv(fd, table, key, msgid, msgid2);
		return 0;
	}

	if (strcmp(funcname, "DBUpdateEntryInt") == 0)
	{
		char *table;
		char *key;
		int value;
		char *info;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		key = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		value = saacproto_demkstr_int(saacproto.token_list[4]);
		info = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		msgid = saacproto_demkstr_int(saacproto.token_list[6]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_DBUpdateEntryInt_recv(fd, table, key, value, info, msgid, msgid2);
		return 0;
	}

	if (strcmp(funcname, "DBGetEntryRank") == 0)
	{
		char *table;
		char *key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		key = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		msgid = saacproto_demkstr_int(saacproto.token_list[4]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_DBGetEntryRank_recv(fd, table, key, msgid, msgid2);
		return 0;
	}

#ifdef _ALLDOMAN // Syu ADD ���а�NPC
	if (strcmp(funcname, "UpdataStele") == 0)
	{
		char *cdkey;
		char *name;
		char *title;
		int level;
		int trns;
		int time;
		int floor;
		cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		name = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		title = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		level = saacproto_demkstr_int(saacproto.token_list[5]);
		trns = saacproto_demkstr_int(saacproto.token_list[6]);
		time = saacproto_demkstr_int(saacproto.token_list[7]);
		floor = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_UpdataStele_recv(fd, cdkey, name, title, level, trns, time, floor);
		return 0;
	}
#endif

	if (strcmp(funcname, "DBDeleteEntryInt") == 0)
	{
		char *table;
		char *key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		key = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		msgid = saacproto_demkstr_int(saacproto.token_list[4]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_DBDeleteEntryInt_recv(fd, table, key, msgid, msgid2);
		return 0;
	}

	if (strcmp(funcname, "DBGetEntryInt") == 0)
	{
		char *table;
		char *key;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		key = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		msgid = saacproto_demkstr_int(saacproto.token_list[4]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_DBGetEntryInt_recv(fd, table, key, msgid, msgid2);
		return 0;
	}

	if (strcmp(funcname, "DBGetEntryByRank") == 0)
	{
		char *table;
		int rank_start;
		int rank_end;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		rank_start = saacproto_demkstr_int(saacproto.token_list[3]);
		rank_end = saacproto_demkstr_int(saacproto.token_list[4]);
		msgid = saacproto_demkstr_int(saacproto.token_list[5]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[6]);
		saacproto_DBGetEntryByRank_recv(fd, table, rank_start, rank_end, msgid, msgid2);
		return 0;
	}

	if (strcmp(funcname, "DBGetEntryByCount") == 0)
	{
		char *table;
		int count_start;
		int num;
		int msgid;
		int msgid2;
		table = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		count_start = saacproto_demkstr_int(saacproto.token_list[3]);
		num = saacproto_demkstr_int(saacproto.token_list[4]);
		msgid = saacproto_demkstr_int(saacproto.token_list[5]);
		msgid2 = saacproto_demkstr_int(saacproto.token_list[6]);
		saacproto_DBGetEntryByCount_recv(fd, table, count_start, num, msgid, msgid2);
		return 0;
	}

	if (strcmp(funcname, "Broadcast") == 0)
	{
		char *id;
		char *charname;
		char *message;
		int flag;
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		message = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		flag = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_Broadcast_recv(fd, id, charname, message, flag);

		return 0;
	}

	if (strcmp(funcname, "Message") == 0)
	{
		char *id_from;
		char *charname_from;
		char *id_to;
		char *charname_to;
		char *message;
		int option;
		id_from = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		charname_from = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		id_to = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charname_to = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		message = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		option = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_Message_recv(fd, id_from, charname_from, id_to, charname_to, message, option);
		return 0;
	}

	if (strcmp(funcname, "MessageAck") == 0)
	{
		char *id;
		char *charname;
		char *result;
		int mesgid;
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		result = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		mesgid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_MessageAck_recv(fd, id, charname, result, mesgid);
		return 0;
	}

	if (strcmp(funcname, "MessageFlush") == 0)
	{
		char *id;
		char *charname;
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		saacproto_MessageFlush_recv(fd, id, charname);
		return 0;
	}
	// ��������
	if (strcmp(funcname, "ACAddFM") == 0)
	{
		int fmleaderlv, fmsprite, charfdid, fmleadergrano;
		char *fmname, *fmleadername, *fmleaderid, *petname, *petattr, *fmrule;
#ifdef _PERSONAL_FAME
		int fame;
#endif
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmleadername = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		fmleaderid = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		fmleaderlv = saacproto_demkstr_int(saacproto.token_list[5]);
		petname = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		petattr = saacproto_wrapStringAddr(saacproto_stringwrapper[6], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[7]));
		fmrule = saacproto_wrapStringAddr(saacproto_stringwrapper[7], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[8]));
		fmsprite = saacproto_demkstr_int(saacproto.token_list[9]);
		fmleadergrano = saacproto_demkstr_int(saacproto.token_list[10]);
#ifdef _PERSONAL_FAME
		fame = saacproto_demkstr_int(saacproto.token_list[11]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[12]);
#ifdef _NB_FAMILYBADGE
		int fmbadge = saacproto_demkstr_int(saacproto.token_list[13]);
		saacproto_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid,
							   fmleaderlv, petname, petattr, fmrule, fmsprite,
							   fmleadergrano, fame, fmbadge, charfdid);
#else
		saacproto_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid,
							   fmleaderlv, petname, petattr, fmrule, fmsprite,
							   fmleadergrano, fame, charfdid);
#endif
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[11]);
		saacproto_ACAddFM_recv(fd, fmname, fmleadername, fmleaderid,
							   fmleaderlv, petname, petattr, fmrule, fmsprite,
							   fmleadergrano, charfdid);
#endif
		return 0;
	}
	// �������
	if (strcmp(funcname, "ACJoinFM") == 0)
	{
#ifdef _PERSONAL_FAME // Arminius: �����������
		int fmindex, charlv, charfdid, index, fame;
#else
		int fmindex, charlv, charfdid, index;
#endif
		char *fmname, *charname, *charid;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charlv = saacproto_demkstr_int(saacproto.token_list[6]);
		index = saacproto_demkstr_int(saacproto.token_list[7]);
#ifdef _PERSONAL_FAME // Arminius: �����������
		fame = saacproto_demkstr_int(saacproto.token_list[8]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACJoinFM_recv(fd, fmname, fmindex, charname,
								charid, charlv, index, fame, charfdid);
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACJoinFM_recv(fd, fmname, fmindex, charname,
								charid, charlv, index, charfdid);
#endif
		return 0;
	}
	// �˳�����
	if (strcmp(funcname, "ACLeaveFM") == 0)
	{
		int fmindex, charfdid, index;
		char *fmname, *charname, *charid;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		index = saacproto_demkstr_int(saacproto.token_list[6]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACLeaveFM_recv(fd, fmname, fmindex, charname,
								 charid, index, charfdid);
		return 0;
	}
	// ��ɢ����
#ifdef _LEADERFUNCHECK
	if (strcmp(funcname, "ACDelFM") == 0)
	{
		int fmindex, charfdid, index;
		char *fmname, *charname, *charid;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACDelFM_recv(fd, fmname, fmindex, index, charname, charid, charfdid);
		return 0;
	}
#else
	if (strcmp(funcname, "ACDelFM") == 0)
	{
		int fmindex, charfdid, index;
		char *fmname;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACDelFM_recv(fd, fmname, fmindex, index, charfdid);
		return 0;
	}
#endif
	// �г������б�
	if (strcmp(funcname, "ACShowFMList") == 0)
	{
		saacproto_ACShowFMList_recv(fd);
		return 0;
	}
	// �г���Ա�б�
	if (strcmp(funcname, "ACShowMemberList") == 0)
	{
		int index;
		char *tmpbuf;
		tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		index = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACShowMemberList_recv(fd, index);
		return 0;
	}
	// �г�������ϸ����
	if (strcmp(funcname, "ACFMDetail") == 0)
	{
		int fmindex, charfdid, index;
		char *fmname;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACFMDetail_recv(fd, fmname, fmindex, index, charfdid);
		return 0;
	}
	// �г���������
	if (strcmp(funcname, "ACFMReadMemo") == 0)
	{
		int index;
		char *tmpbuf;
		tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		index = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACFMReadMemo_recv(fd, index);
		return 0;
	}
	// д���������
	if (strcmp(funcname, "ACFMWriteMemo") == 0)
	{
		int index, fmindex;
		char *fmname, *data;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		index = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACFMWriteMemo_recv(fd, fmname, fmindex, data, index);
		return 0;
	}

	// ���login
	if (strcmp(funcname, "ACFMCharLogin") == 0)
	{
		int fmindex, charlv, charfdid;
#ifdef _FM_MODIFY
		int gsnum;
#endif
		char *fmname, *charname, *charid;
#ifdef _FMVER21
		int eventflag;
#endif
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charlv = saacproto_demkstr_int(saacproto.token_list[6]);

#ifdef _FMVER21
		eventflag = saacproto_demkstr_int(saacproto.token_list[7]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
#ifdef _FM_MODIFY
		gsnum = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv, eventflag, charfdid, gsnum);
#else
		saacproto_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv, eventflag, charfdid);
#endif
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACFMCharLogin_recv(fd, fmname, fmindex, charname, charid, charlv, charfdid);
#endif
		return 0;
	}
	// ���logout
	if (strcmp(funcname, "ACFMCharLogout") == 0)
	{
		int fmindex, charfdid, charlv, index;
		char *fmname, *charname, *charid;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charlv = saacproto_demkstr_int(saacproto.token_list[6]);
		index = saacproto_demkstr_int(saacproto.token_list[7]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACFMCharLogout_recv(fd, fmname, fmindex, charname,
									  charid, charlv, index, charfdid);
		return 0;
	}
	// �峤��˳�Ա������塢�޸ļ����Աְλ
	if (strcmp(funcname, "ACMemberJoinFM") == 0)
	{
		int fmindex, charfdid, index, charindex, result;
		char *fmname, *charname;
#ifdef _FMVER21
		int meindex;
#endif
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charindex = saacproto_demkstr_int(saacproto.token_list[5]);
		index = saacproto_demkstr_int(saacproto.token_list[6]);
		result = saacproto_demkstr_int(saacproto.token_list[7]);
#ifdef _FMVER21
		meindex = saacproto_demkstr_int(saacproto.token_list[8]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACMemberJoinFM_recv(fd, fmname, fmindex, charname,
									  charindex, index, result, meindex, charfdid);
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACMemberJoinFM_recv(fd, fmname, fmindex, charname,
									  charindex, index, result, charfdid);
#endif
		return 0;
	}
	// �峤��˳�Ա�뿪����
	if (strcmp(funcname, "ACMemberLeaveFM") == 0)
	{
		int fmindex, charfdid, index, charindex;
		char *fmname, *charname;
#ifdef _FMVER21
		int meindex;
#endif
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		charname = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charindex = saacproto_demkstr_int(saacproto.token_list[5]);
		index = saacproto_demkstr_int(saacproto.token_list[6]);
#ifdef _FMVER21
		meindex = saacproto_demkstr_int(saacproto.token_list[7]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACMemberLeaveFM_recv(fd, fmname, fmindex, charname,
									   charindex, index, meindex, charfdid);
#else
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACMemberLeaveFM_recv(fd, fmname, fmindex, charname,
									   charindex, index, charfdid);
#endif
		return 0;
	}
	// �г�����ݵ�
	if (strcmp(funcname, "ACFMPointList") == 0)
	{
		saacproto_ACFMPointList_recv(fd);
		return 0;
	}

	// �������ݵ�
	if (strcmp(funcname, "ACSetFMPoint") == 0)
	{
		int fl, x, y, fmindex, index, fmpointindex, charfdid;
		char *fmname;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		fmpointindex = saacproto_demkstr_int(saacproto.token_list[5]);
		fl = saacproto_demkstr_int(saacproto.token_list[6]);
		x = saacproto_demkstr_int(saacproto.token_list[7]);
		y = saacproto_demkstr_int(saacproto.token_list[8]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACSetFMPoint_recv(fd, fmname, fmindex, index,
									fmpointindex, fl, x, y, charfdid);
		return 0;
	}
	// �趨����ݵ�
	if (strcmp(funcname, "ACFixFMPoint") == 0)
	{
		int winindex, winfmindex, loseindex, losefmindex, village;
		char *winfmname, *losefmname;
		winfmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		winfmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		winindex = saacproto_demkstr_int(saacproto.token_list[4]);
		losefmname = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		losefmindex = saacproto_demkstr_int(saacproto.token_list[6]);
		loseindex = saacproto_demkstr_int(saacproto.token_list[7]);
		village = saacproto_demkstr_int(saacproto.token_list[8]);
		saacproto_ACFixFMPoint_recv(fd, winfmname, winfmindex, winindex,
									losefmname, losefmindex, loseindex, village);
		return 0;
	}
	// �峤��ȫԱ�㲥
	if (strcmp(funcname, "ACFMAnnounce") == 0)
	{
		int index, fmindex, color;
		char *fmname, *data;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		color = saacproto_demkstr_int(saacproto.token_list[6]);
		saacproto_ACFMAnnounce_recv(fd, fmname, fmindex, index, data, color);
		return 0;
	}
	// �г��������а�
	if (strcmp(funcname, "ACShowTopFMList") == 0)
	{
		int kindflag;
		char *tmpbuf;
		tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		kindflag = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACShowTopFMList_recv(fd, kindflag);
		return 0;
	}
	// ������������
	if (strcmp(funcname, "ACFixFMData") == 0)
	{
		int fmindex, charfdid, index, kindflag, charindex;
		char *fmname, *data1, *data2;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		kindflag = saacproto_demkstr_int(saacproto.token_list[5]);
		data1 = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		data2 = saacproto_wrapStringAddr(saacproto_stringwrapper[6], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[7]));
		charindex = saacproto_demkstr_int(saacproto.token_list[8]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[9]);
		saacproto_ACFixFMData_recv(fd, fmname, fmindex, index, kindflag, data1, data2, charindex, charfdid);
		return 0;
	}

	// ����У�����������
	if (strcmp(funcname, "ACFixFMPK") == 0)
	{
		int winindex, winfmindex, loseindex, losefmindex;
		char *winfmname, *losefmname;
		winfmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		winfmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		winindex = saacproto_demkstr_int(saacproto.token_list[4]);
		losefmname = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		losefmindex = saacproto_demkstr_int(saacproto.token_list[6]);
		loseindex = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACFixFMPK_recv(fd, winfmname, winfmindex, winindex,
								 losefmname, losefmindex, loseindex);
		return 0;
	}
	// GM������������
	if (strcmp(funcname, "ACGMFixFMData") == 0)
	{
		int index, charfdid;
		char *cmd, *data, *tmpbuf, *charid;
		tmpbuf = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		index = saacproto_demkstr_int(saacproto.token_list[3]);
		charid = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		cmd = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		charfdid = saacproto_demkstr_int(saacproto.token_list[7]);
		saacproto_ACGMFixFMData_recv(fd, index, charid, cmd, data, charfdid);
		return 0;
	}
	// ȡ�ü�������
	if (strcmp(funcname, "ACGetFMData") == 0)
	{
		int fmindex, charfdid, index, kindflag;
		char *fmname;
		fmname = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		fmindex = saacproto_demkstr_int(saacproto.token_list[3]);
		index = saacproto_demkstr_int(saacproto.token_list[4]);
		kindflag = saacproto_demkstr_int(saacproto.token_list[5]);
		charfdid = saacproto_demkstr_int(saacproto.token_list[6]);
		saacproto_ACGetFMData_recv(fd, fmname, fmindex, index, kindflag,
								   charfdid);
		return 0;
	}
	// �㲥�߹��˵�����
	if (strcmp(funcname, "ACManorPKAck") == 0)
	{
		char *data;
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		saacproto_ACManorPKAck_recv(fd, data);
		return 0;
	}

	if (strcmp(funcname, "ACreLoadFmData") == 0)
	{
		int type, data;
		type = saacproto_demkstr_int(saacproto.token_list[2]);
		data = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_ACreLoadFmData_recv(fd, type, data);
		return 0;
	}

#ifdef _AC_SEND_FM_PK
	if (strcmp(funcname, "ACLoadFmPk") == 0)
	{
		int fmpks_pos;
		fmpks_pos = saacproto_demkstr_int(saacproto.token_list[2]);
		saacproto_ACLoadFmPk_recv(fd, fmpks_pos);
		return 0;
	}
#ifdef _ACFMPK_LIST
	if (strcmp(funcname, "ACSendFmPk") == 0)
	{
		int fmpks_pos, userindex, flg;
		char *data;
		fmpks_pos = saacproto_demkstr_int(saacproto.token_list[2]);
		userindex = saacproto_demkstr_int(saacproto.token_list[3]);
		flg = saacproto_demkstr_int(saacproto.token_list[4]);
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		saacproto_ACSendFmPk_recv(fd, fmpks_pos, userindex, flg, data);
		return 0;
	}
#else
	if (strcmp(funcname, "ACSendFmPk") == 0)
	{
		int fmpks_pos;
		char *data;
		fmpks_pos = saacproto_demkstr_int(saacproto.token_list[2]);
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		saacproto_ACSendFmPk_recv(fd, fmpks_pos, data);
		return 0;
	}
#endif

#endif

#ifdef _CHAR_POOLITEM
	if (strcmp(funcname, "ACInsertPoolItem") == 0)
	{
		char *cdkey, *Pooldataarg;
		int clifdid, userindex;

		cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		Pooldataarg = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		userindex = saacproto_demkstr_int(saacproto.token_list[4]);
		clifdid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACCharInsertPoolItem_recv(fd, cdkey, userindex, clifdid, Pooldataarg);
		return 0;
	}
	if (strcmp(funcname, "ACSavePoolItem") == 0)
	{
		char *cdkey, *Pooldataarg;
		int clifdid, userindex;

		cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		Pooldataarg = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		userindex = saacproto_demkstr_int(saacproto.token_list[4]);
		clifdid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACCharSavePoolItem_recv(fd, cdkey, userindex, clifdid, Pooldataarg);
		return 0;
	}

	if (strcmp(funcname, "ACGetPoolItem") == 0)
	{
		char *cdkey;
		int clifdid, userindex, npcid;
		cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		userindex = saacproto_demkstr_int(saacproto.token_list[3]);
		clifdid = saacproto_demkstr_int(saacproto.token_list[4]);
		npcid = saacproto_demkstr_int(saacproto.token_list[5]);

		saacproto_ACCharGetPoolItem_recv(fd, cdkey, userindex, clifdid, npcid);
		return 0;
	}
#endif

#ifdef _CHAR_POOLPET
	if (strcmp(funcname, "ACInsertPoolPet") == 0)
	{
		char *cdkey, *Pooldataarg;
		int clifdid, userindex;

		cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		Pooldataarg = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		userindex = saacproto_demkstr_int(saacproto.token_list[4]);
		clifdid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACCharInsertPoolPet_recv(fd, cdkey, userindex, clifdid, Pooldataarg);
		return 0;
	}
	if (strcmp(funcname, "ACSavePoolPet") == 0)
	{
		char *cdkey, *Pooldataarg;
		int clifdid, userindex;

		cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		Pooldataarg = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		userindex = saacproto_demkstr_int(saacproto.token_list[4]);
		clifdid = saacproto_demkstr_int(saacproto.token_list[5]);
		saacproto_ACCharSavePoolPet_recv(fd, cdkey, userindex, clifdid, Pooldataarg);
		return 0;
	}

	if (strcmp(funcname, "ACGetPoolPet") == 0)
	{
		char *cdkey;
		int clifdid, userindex, npcid;
		cdkey = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		userindex = saacproto_demkstr_int(saacproto.token_list[3]);
		clifdid = saacproto_demkstr_int(saacproto.token_list[4]);
		npcid = saacproto_demkstr_int(saacproto.token_list[5]);

		saacproto_ACCharGetPoolPet_recv(fd, cdkey, userindex, clifdid, npcid);
		return 0;
	}
#endif

#ifdef _ANGEL_SUMMON
	if (strcmp(funcname, "ACMissionTable") == 0)
	{
		int num;
		int type;
		char *data;
		//int charaindex;
		char *angelinfo;

		num = saacproto_demkstr_int(saacproto.token_list[2]);
		type = saacproto_demkstr_int(saacproto.token_list[3]);
		data = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		//charaindex = saacproto_demkstr_int( saacproto.token_list[5] );
		angelinfo = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));

		saacproto_ACMissionTable_recv(fd, num, type, data, angelinfo);
		return 0;
	}
#endif

#ifdef _TEACHER_SYSTEM
	if (strcmp(funcname, "ACCheckCharacterOnLine") == 0)
	{
		int charaindex, flag;
		char *id;
		char *name;

		charaindex = saacproto_demkstr_int(saacproto.token_list[2]);
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		name = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		flag = saacproto_demkstr_int(saacproto.token_list[5]);

		saacproto_ACCheckCharacterOnLine_recv(fd, charaindex, id, name, flag);
		return 0;
	}
#endif

	if (strcmp(funcname, "LockLogin") == 0)
	{
		int flag;
		char *id, *ip;
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		ip = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		flag = saacproto_demkstr_int(saacproto.token_list[4]);
		saacproto_LockLogin_recv(fd, id, ip, flag);
		return 0;
	}

#ifdef _ITEM_PET_LOCKED
	if (strcmp(funcname, "ItemPetLocked") == 0)
	{
		int clifd;
		char *id;
		char *safepasswd;

		clifd = saacproto_demkstr_int(saacproto.token_list[2]);
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		safepasswd = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		saacproto_ItemPetLocked_recv(fd, clifd, id, safepasswd);
		return 0;
	}

	if (strcmp(funcname, "ItemPetLockedPasswd") == 0)
	{
		int clifd;
		char *id;
		char *safepasswd;

		clifd = saacproto_demkstr_int(saacproto.token_list[2]);
		id = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		safepasswd = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		saacproto_ItemPetLockedPasswd_recv(fd, clifd, id, safepasswd);
		return 0;
	}
#endif

#ifdef _FORMULATE_AUTO_PK
	if (strcmp(funcname, "FormulateAutoPk") == 0)
	{
		int clifd = saacproto_demkstr_int(saacproto.token_list[2]);
		char *id = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		int point = saacproto_demkstr_int(saacproto.token_list[4]);
		saacproto_FormulateAutoPk_recv(fd, clifd, id, point);
		return 0;
	}
#endif

#ifdef _LOTTERY_SYSTEM
	if (strcmp(funcname, "LotterySystem") == 0)
	{
		saacproto_LotterySystem_recv(fd);
		return 0;
	}
#endif

#ifdef _ALL_SERV_SEND
	if (strcmp(funcname, "AllServSend") == 0)
	{
		char *data = saacproto_wrapStringAddr(saacproto_stringwrapper[1], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[2]));
		int color = saacproto_demkstr_int(saacproto.token_list[3]);
		saacproto_AllServSend_recv(data, color);
		return 0;
	}
#endif

#ifdef _CHARADATA_SAVE_SQL
	if (strcmp(funcname, "CharadataSaveSQL") == 0)
	{
		Charadata charadata;
		int clifd = saacproto_demkstr_int(saacproto.token_list[2]);
		char *id = saacproto_wrapStringAddr(saacproto_stringwrapper[2], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3]));
		charadata.CHAR_list_String = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[4]));
		charadata.CHAR_list_info1_String = saacproto_wrapStringAddr(saacproto_stringwrapper[4], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[5]));
		charadata.CHAR_list_info2_String = saacproto_wrapStringAddr(saacproto_stringwrapper[5], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[6]));
		charadata.CHAR_list_count_String = saacproto_wrapStringAddr(saacproto_stringwrapper[6], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[7]));
		charadata.CHAR_list_info3_String = saacproto_wrapStringAddr(saacproto_stringwrapper[7], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[8]));
		charadata.CHAR_list_event_String = saacproto_wrapStringAddr(saacproto_stringwrapper[8], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[9]));
		charadata.CHAR_list_info4_String = saacproto_wrapStringAddr(saacproto_stringwrapper[9], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[10]));
		charadata.CHAR_list_attackmagic_String = saacproto_wrapStringAddr(saacproto_stringwrapper[10], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[11]));
		charadata.CHAR_list_info5_String = saacproto_wrapStringAddr(saacproto_stringwrapper[11], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[12]));
		charadata.CHAR_list_profession_String = saacproto_wrapStringAddr(saacproto_stringwrapper[12], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[13]));
		charadata.CHAR_list_info6_String = saacproto_wrapStringAddr(saacproto_stringwrapper[13], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[14]));
		charadata.CHAR_list_info_String = saacproto_wrapStringAddr(saacproto_stringwrapper[14], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[15]));
		charadata.CHAR_list_flg_String = saacproto_wrapStringAddr(saacproto_stringwrapper[15], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[16]));
		charadata.CHAR_list_skill_String = saacproto_wrapStringAddr(saacproto_stringwrapper[16], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[17]));
		charadata.CHAR_list_item_String = saacproto_wrapStringAddr(saacproto_stringwrapper[17], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[18]));
		charadata.CHAR_list_title_String = saacproto_wrapStringAddr(saacproto_stringwrapper[18], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[19]));
		charadata.CHAR_list_mail_String = saacproto_wrapStringAddr(saacproto_stringwrapper[19], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[20]));

		int saveindex = saacproto_demkstr_int(saacproto.token_list[21]);
		int flag = saacproto_demkstr_int(saacproto.token_list[22]);
		saacproto_CharadataSaveSQL_recv(fd, clifd, id, charadata, saveindex, flag);
		return 0;
	}
#endif

#ifdef _FAMILY_PK_FIX
	if (strcmp(funcname, "ACFmPkOnlineSn") == 0)
	{
		int manorindex, serverno, flag;

		manorindex = saacproto_demkstr_int(saacproto.token_list[2]);
		flag = saacproto_demkstr_int(saacproto.token_list[3]);

		saacproto_ACFmPkOnlineSn_recv(fd, manorindex, flag);
		return 0;
	}
#endif
#ifdef _NB_GM_SERVERLIST
	if (strcmp(funcname, "GMSERVERLIST") == 0)
	{ //���յ����ָ��
		int itype;
		char *msg;
		itype = saacproto_demkstr_int(saacproto.token_list[2]);																				  //ȡ�ñ��
		msg = saacproto_wrapStringAddr(saacproto_stringwrapper[3], saacproto.workbufsize, saacproto_demkstr_string(saacproto.token_list[3])); //ȡ������
		saacproto_gm_shutdown_recv(fd, itype, msg);																							  //����������GMSV
		return 0;
	}
#endif

	sprintf(debugfun, "%s", funcname);

	return -1;
}

void saacproto_ACServerLogin_send(int fd, char *result, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ACServerLogin");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

void saacproto_ACCharList_send(int fd, char *result, char *output, int id)
{
	saacproto_CreateHeader(saacproto.work, "ACCharList");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(output), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(id), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
static int CharDataLens = 0;
#ifdef _NewSave
void saacproto_ACCharLoad_send(int fd, char *result, char *data, int id, int charindex)
#else
void saacproto_ACCharLoad_send(int fd, char *result, char *data, int id)
#endif
{
	if (strstr(result, "successful") != NULL && strlen(data) > CharDataLens)
	{
		CharDataLens = strlen(data);
		//andy_log
		log("������ݳ���:%d\n", CharDataLens);
	}

	saacproto_CreateHeader(saacproto.work, "ACCharLoad");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(id), saacproto.workbufsize);
#ifdef _NewSave
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charindex), saacproto.workbufsize);
#endif
	saacproto_Send(fd, saacproto.work);
}

/*
214 :  <li><a name="ACCharSaveS"><font color=blue>servertoclient ACCharSave( string result , string data,int id);<br></font></a>
215 :      ACCharSave�帲����߯�ϣ�
216 :      <br><br>
217 :      <dl>
218 :        <dt> string result
219 :        <dd>"successful" , "failed" ������ľ����
220 :        <dt> string data
221 :        <dd>"failed"���ݷ�    ë�������޼�έ������  ٯ  ƥؤ�£�
222 :        <dt> int id
223 :        <dd> ¦�Ѽ�idë��������������ƻ�߯��
224 :      </dl>
225 :      <br>
226 :      <br>
227 : 
228 : 
229 : 
230 : 
*/

void saacproto_ACCharSave_send(int fd, char *result, char *data, int id)
{
	saacproto_CreateHeader(saacproto.work, "ACCharSave");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(id), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

/*
256 :  <li><a name="ACCharDeleteS"><font color=blue>servertoclient ACCharDelete(string result, string data,int id);<br></font></a>
257 :      ACCharDelete�帲����߯�ϣ�
258 :      <br><br>
259 :      <dl>
260 :        <dt> string result
261 :        <dd>"successful" , "failed" ������ľ����
262 :        <dt> string data
263 :        <dd>"failed"���ݷ�    ë�������޼�έ������  ٯ  ƥؤ�£�
264 :        <dt> int id
265 :        <dd> ¦�Ѽ�id�����������������
266 : 			   
267 :      </dl>
268 :      <br>
269 :      <br>
270 : 
*/

void saacproto_ACCharDelete_send(int fd, char *result, char *data, int id)
{
	saacproto_CreateHeader(saacproto.work, "ACCharDelete");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(id), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
290 :  <LI>    <a name="ACLockS"><font color=blue>servertoclient ACLock( string result, string data ,int id);<br></font></a>
291 :      ACLock�帲����߯�ϣ�
292 :      <br><br>
293 :      <dl>
294 :        <dt>string result
295 :        <dd>"successful" , "failed" ������ľ����
296 :        <dt>string data
297 :        <dd>"failed"���ݷ�    ë�������޼�έ������  ٯ  ƥؤ�£�
298 :        <dt>int id
299 :        <dd>¦�Ѽ�id���������������
300 :      </dl>
301 :      <br>
302 :      <br>
303 : 
*/

void saacproto_ACLock_send(int fd, char *result, char *data, int id)
{
	saacproto_CreateHeader(saacproto.work, "ACLock");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(id), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
318 :  <LI><a name="ACUCheckS"><font color=blue>servertoclient ACUCheck( string mem_id );</font></a><BR>
319 : 	 ʧ���������ӡ�������ձء�ة�ӡ�����帲�ƻ���ؤ�½����ǡ����ػ�����̼�
320 : 	 �ƻ����¾�������ë�����������£�
321 : 	 <BR><BR>
322 : 	 <dl>
323 : 	   <dt>string mem_id
324 : 	   <dd>Ʃ���¸��޼������ǡ���gamersDream member id
325 : 	 </dl>	 
326 : 	 <BR><BR>
327 : 
328 : 
*/

void saacproto_ACUCheck_send(int fd, char *mem_id)
{
	saacproto_CreateHeader(saacproto.work, "ACUCheck");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(mem_id), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
344 :  <LI><a name="DBUpdateEntryStringS"><font color=blue>servertoclient DBUpdateEntryString( string result, string table, string key, int msgid, int msgid2 );</font></a><BR>
345 : 	 ������Ѩ������������  �ƻ��ּ����ص�  ئ��ƥ  ���ƻ�������
346 : 	 <dl>
347 : 	   <dt>string result
348 : 	   <dD>successful ��failed	   
349 : 	   <dt>string table
350 : 	   <dD>
351 : 	   <dt>int msgid
352 : 	   <dD>
353 : 	   <dt>int msgid2
354 : 	   <dD>
355 : 	 </dL>
356 : 
*/

void saacproto_DBUpdateEntryString_send(int fd, char *result, char *table, char *key, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBUpdateEntryString");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(key), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
369 :  <LI><a name="DBDeleteEntryStringS"><font color=blue>servertoclient DBDeleteEntryString( string result, string table, string key , int msgid , int msgid2 );</font></a><BR>
370 : 	   ٯ  �޼����������ּ�����
371 : 	 <dl>
372 : 	   <dt>string result
373 : 	   <dd>successful / failed
374 : 	   <dt>string table
375 : 	   <dd>
376 : 	   <dt>string key
377 : 	   <dd>
378 : 	   <dt>int msgid
379 : 	   <dd>
380 : 	   <dt>int msgid2
381 : 	   <dd>		   
382 : 	 </dl>
383 : 	 
*/

void saacproto_DBDeleteEntryString_send(int fd, char *result, char *table, char *key, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBDeleteEntryString");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(key), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
396 :  <Li><a name="DBGetEntryStringS"><font color=blue>servertoclient DBGetEntryString( string result, string value, string table, string key, int msgid, int msgid2 );</font></a><BR>
397 : 	 ��Ի���Ƽ�߯Ի��
398 : 	 <dl>
399 : 	   <dt>string result
400 : 	   <dd>successful / failed
401 : 	   <dt>string value
402 : 	   <dd>��
403 : 	   <dt>string table
404 : 	   <dd>		   
405 : 	   <dt>string key
406 : 	   <dd>
407 : 	   <dt>int msgid
408 : 	   <dd>
409 : 	   <dt>int msgid2
410 : 	   <dd>		   
411 : 	 </dl>
412 : 
*/

void saacproto_DBGetEntryString_send(int fd, char *result, char *value, char *table, char *key, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBGetEntryString");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(value), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(key), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
435 :  <LI><a name="DBUpdateEntryIntS"><font color=blue>servertoclient DBUpdateEntryInt( string result , string table, string key, int msgid, int msgid2 );</font></a><BR>
436 : 	 DBUpdateEntry��������
437 : 	 <dl>
438 : 	   <dt>string result
439 : 	   <dd>succesful/failed
440 : 	   <dt>string table
441 : 	   <dd>  ��Ƥ��  ë��������������
442 : 	   <dt>string key
443 : 	   <dd>ƽ��ë��������������		   
444 : 	   <dt>int msgid, int msgid2
445 : 	   <dd>msgid ë��������߯�ʣ�
446 : 	 </dl>
447 : 	 <BR><BR>
448 : 
*/
#ifdef _ALLDOMAN // Syu ADD ���а�NPC
void saacproto_UpdataStele_send(int fd, char *data)
{
	saacproto_CreateHeader(saacproto.work, "UpdataStele");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

void saacproto_S_UpdataStele_send(int fd, char *ocdkey, char *oname, char *ncdkey,
								  char *nname, char *title, int level, int trns, int floor)
{
	saacproto_CreateHeader(saacproto.work, "S_UpdataStele");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(ocdkey), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(oname), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(ncdkey), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(nname), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(title), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(level), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(trns), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(floor), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
#endif
void saacproto_DBUpdateEntryInt_send(int fd, char *result, char *table, char *key, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBUpdateEntryInt");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(key), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
463 :  <LI><a name="DBGetEntryRankS"><font color=blue>servertoclient DBGetEntryRank( string result , int rank , int count, string table, string key, int msgid, int msgid2 );</font></a><BR>
464 : 	 ����ë߯�ʣ�
465 : 	 <dl>
466 : 	   <dt>string result
467 : 	   <dd>successful/failed
468 : 	   <dt>int rank
469 : 	   <dd>���ˣ�1��������ë���£�
470 : 	   <dt>int count
471 : 	   <dd>���������ʹ�������  �ټ��޼�������0���շ�Ԫ���£�
472 : 	   <dt>string table
473 : 	   <dd>  ��Ƥ��  ë��������������
474 : 	   <dt>string key
475 : 	   <dd>ƽ��ë��������������
476 : 	   <dt>int msgid, int msgid2
477 : 	   <dd>msgid ë��������߯��
478 : 	 </dl>
479 : 	 <BR><BR>
480 : 
481 : 	 
*/

void saacproto_DBGetEntryRank_send(int fd, char *result, int rank, int count, char *table, char *key, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBGetEntryRank");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(rank), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(count), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(key), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
494 :  <LI><a name="DBDeleteEntryIntS"><font color=blue>servertoclient DBDeleteEntryInt( string result , string table, string key, int msgid, int msgid2 );</font></a><BR>
495 : 	 ���ּ�����
496 : 	 <dl>
497 : 	   <dt>string result
498 : 	   <dd>successful/failed
499 : 	   <dt>string table
500 : 	   <dd>  ��Ƥ��  ë��������������
501 : 	   <dt>string key
502 : 	   <dd>ƽ��ë��������������
503 : 	   <dt>int msgid, int msgid2
504 : 	   <dd>msgid ë��������߯��
505 : 	 </dl>
506 : 	 <BR><BR>
507 : 
*/

void saacproto_DBDeleteEntryInt_send(int fd, char *result, char *table, char *key, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBDeleteEntryInt");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(key), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
520 :  <LI><a name="DBGetEntryIntS"><font color=blue>servertoclient DBGetEntryInt( string result , int value , string table, string key, int msgid, int msgid2 );</font></a><BR>
521 : 	 �޼���������ë������
522 : 	 <dl>
523 : 	   <dt>string result
524 : 	   <dd>successful/failed
525 : 	   <dt>int value
526 : 	   <dd>�������ּ�
527 : 	   <dt>string table
528 : 	   <dd>  ��Ƥ��  ë��������������
529 : 	   <dt>string key
530 : 	   <dd>ƽ��ë��������������
531 : 	   <dt>int msgid, int msgid2
532 : 	   <dd>msgid ë��������߯��
533 : 	 </dl>
534 : 
535 : 
536 : 
*/

void saacproto_DBGetEntryInt_send(int fd, char *result, int value, char *table, char *key, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBGetEntryInt");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(value), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(key), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
552 :  <LI><a name="DBGetEntryByRankS"><font color=blue>servertoclient DBGetEntryByRank( string result , string list , string table, int msgid, int msgid2 );</font></a><BR>
553 : 	 DBGetEntryByRank��������
554 : 	 <dl>
555 : 	   <dt>string result
556 : 	   <dd>
557 : 	   <dt>string list
558 : 	   <dd>"RANK,KEY,SCORE,INFO|RANK,KEY,SCORE,INFO|...."<BR>
559 : 		   RANK������ƥؤ�£�KEY���������˱�ئ�Ȼ����µ���ʧë���Ȼ�����
560 : 		   ƽ����SCORE�����Ѱ�ƥ����ʧƥؤ�£�
561 : 		     ��<br>
562 : 		   "1,RINGO,123465,data1|2,HOGE,4567,data2|2,FUCK,4567,data3"<BR>
563 : 		   ��  �����2�˷�2�����³����ئ�£�
564 : 	   <dt>string table
565 : 	   <dd>  ��Ƥ��  
566 : 	   <dt>int msgid, int msgid2
567 : 	   <dd>
568 : 	 </dl>
569 : 
570 : 
*/

void saacproto_DBGetEntryByRank_send(int fd, char *result, char *list, char *table, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBGetEntryByRank");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(list), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
583 :  <LI><a name="DBGetEntryByCountS"><font color=blue>servertoclient DBGetEntryByCount( string result , string list , string table, int count_start, int msgid, int msgid2 );</font></a><BR>
584 : 	 DBGetEntryByCount ��������
585 : 	 <dl>
586 : 	   <dt>string result
587 : 	   <dd>SUCCESSFUL/FAILED
588 : 	   <dt>string list
589 : 	   <dd>��  ��������1�޼�����ؤ��Ի "INDEX,RANK,KEY,SCORE,INFO" ��4�ۼ���  
590 : 		   ��׬ƥؤԻ����ľ�������� "|" ƥ��ئ���£����ѻ�0��������
591 : 		   ��  ٯ  ��
592 : 	   <dt>int num
593 : 	   <dd>���ʹ���Ի���ʾ���1ئ��1�ۣ�2ئ��2�ۣ�
594 : 	   <dt>int msgid ,msgid2
595 : 	   <dd>����������ID.	   
596 : 	 </dL>
597 : 
*/

void saacproto_DBGetEntryByCount_send(int fd, char *result, char *list, char *table, int count_start, int msgid, int msgid2)
{
	saacproto_CreateHeader(saacproto.work, "DBGetEntryByCount");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(list), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(table), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(count_start), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(msgid2), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
611 :  <LI><a name="Broadcast"><font color=blue>servertoclient Broadcast( string id, string charname, string message );</font></a><BR>
612 : 	 Broadcast��ʧ���������ӡ�������ձء�ة�ӡ�����߼�˪����
613 : 	 <dl>
614 : 	   <dt>string id
615 : 	   <dd>user ID
616 : 	   <dt>string charname
617 : 	   <dd>character name
618 : 	   <dt>string message
619 : 	   <dd>����������
620 : 	   <dt>int flag
621 : 	   <dd>1������������б�巴  ˪��ľئ�У�
622 : 	 </dl>
623 : 
*/

void saacproto_Broadcast_send(int fd, char *id, char *charname, char *message)
{
	saacproto_CreateHeader(saacproto.work, "Broadcast");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(id), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(charname), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(message), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
/*
642 :  <LI><a name="Message"><font color=blue>servertoclient Message( string id_from, string charname_from, string id_to, string charname_to, string message , int option , int mesgid );</font></a><BR>
643 : 	 ʧ���������ӡ�������ձء�ة�ӡ�����嶪��������ë˪�����£�
644 : 	 <dl>
645 : 	   <dt>string id_from
646 : 	   <dd>from ID
647 : 	   <dt>string charname_from
648 : 	   <dd>from character name
649 : 	   <dt>string id_to
650 : 	   <dd>to ID
651 : 	   <dt>string charname_to
652 : 	   <dd>to character name
653 : 	   <dt>string message
654 : 	   <dd>���������⼰    
655 : 	   <dt>int option
656 : 	   <dd>���������(��������ë���)
657 : 	   <dt>int mesgid
658 : 	   <dd>����������ID ʧ���������ӡ��������  ��
659 : 	 </dl>
660 : 
*/

void saacproto_Message_send(int fd, char *id_from, char *charname_from, char *id_to, char *charname_to, char *message, int option, int mesgid)
{
	saacproto_CreateHeader(saacproto.work, "Message");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(id_from), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(charname_from), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(id_to), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(charname_to), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(message), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(option), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(mesgid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

void saacproto_SetServerLogFiles(char *r, char *w)
{
	saacproto_strcpysafe(saacproto_writelogfilename, w, sizeof(saacproto_writelogfilename));
	saacproto_strcpysafe(saacproto_readlogfilename, r, sizeof(saacproto_readlogfilename));
}
int saacproto_InitServer(int (*writefunc)(int, char *, int), int worksiz)
{
	int i;
	if ((void *)writefunc == NULL)
	{
		saacproto.write_func = saacproto_default_write_wrap;
	}
	else
	{
		saacproto.write_func = writefunc;
	}
	saacproto_AllocateCommonWork(worksiz);
	saacproto_stringwrapper = (char **)calloc(1, sizeof(char *) * MAXLSRPCARGS);
	if (saacproto_stringwrapper == NULL)
		return -1;
	memset(saacproto_stringwrapper, 0, sizeof(char *) * MAXLSRPCARGS);
	for (i = 0; i < MAXLSRPCARGS; i++)
	{
		saacproto_stringwrapper[i] = (char *)calloc(1, worksiz);
		if (saacproto_stringwrapper[i] == NULL)
		{
			for (i = 0; i < MAXLSRPCARGS; i++)
			{
				free(saacproto_stringwrapper[i]);
				return -1;
			}
		}
	}
	saacproto.ret_work = (char *)calloc(1, sizeof(worksiz));
	if (saacproto.ret_work == NULL)
	{
		return -1;
	}
	return 0;
}
void saacproto_CleanupServer(void)
{
	int i;
	free(saacproto.work);
	free(saacproto.arraywork);
	free(saacproto.escapework);
	free(saacproto.val_str);
	free(saacproto.token_list);
	for (i = 0; i < MAXLSRPCARGS; i++)
	{
		free(saacproto_stringwrapper[i]);
	}
	free(saacproto_stringwrapper);
	free(saacproto.ret_work);
}

/************************************************************
 * CoolFish: Family 2001/5/18
 ***********************************************************/
// ��������
void saacproto_ACAddFM_send(int fd, char *result, int fmindex, int index,
							int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACAddFM");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(fmindex), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(index), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �������
void saacproto_ACJoinFM_send(int fd, char *result, int recv, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACJoinFM");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(recv), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �˳�����
void saacproto_ACLeaveFM_send(int fd, char *result, int resultflag, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACLeaveFM");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(resultflag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// ��ɢ����
void saacproto_ACDelFM_send(int fd, char *result, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACDelFM");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �г������б�
void saacproto_ACShowFMList_send(int fd, char *result, int num, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ACShowFMList");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(num), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

// �г���Ա�б�
void saacproto_ACShowMemberList_send(int fd, char *result, int index, int num,
									 int acceptflag, int fmjoinnum, char *data
#ifdef _NB_FAMILYBADGE
									 ,
									 int badge
#endif
)
{
	saacproto_CreateHeader(saacproto.work, "ACShowMemberList");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(index), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(num), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(acceptflag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(fmjoinnum), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
#ifdef _NB_FAMILYBADGE
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(badge), saacproto.workbufsize);
#endif
	saacproto_Send(fd, saacproto.work);
}
// �г�������ϸ����
void saacproto_ACFMDetail_send(int fd, char *result, char *data,
							   int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACFMDetail");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �峤��˳�Ա�������
void saacproto_ACMemberJoinFM_send(int fd, char *result, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACMemberJoinFM");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �峤��˳�Ա�뿪����
void saacproto_ACMemberLeaveFM_send(int fd, char *result, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACMemberLeaveFM");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �������֮���login
#ifdef _PERSONAL_FAME // Arminius: �����������
void saacproto_ACFMCharLogin_send(int fd, char *result, int index, int floor,
								  int fmpopular, int joinflag, int fmsetupflag, int flag, int charindex,
								  int charfame, int charfdid
#ifdef _NEW_MANOR_LAW
								  ,
								  int momentum
#endif
)
#else
void saacproto_ACFMCharLogin_send(int fd, char *result, int index, int floor,
								  int fmpopular, int joinflag, int fmsetupflag, int flag, int charindex,
								  int charfdid)
#endif
{
	saacproto_CreateHeader(saacproto.work, "ACFMCharLogin");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(index), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(floor), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(fmpopular), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(joinflag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(fmsetupflag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(flag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charindex), saacproto.workbufsize);
#ifdef _PERSONAL_FAME // Arminius: �����������
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfame), saacproto.workbufsize);
#endif
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
#ifdef _NEW_MANOR_LAW
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(momentum), saacproto.workbufsize);
#endif
	saacproto_Send(fd, saacproto.work);
}
// �������֮���logout
void saacproto_ACFMCharLogout_send(int fd, char *result, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACFMCharLogout");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �г��������԰�
void saacproto_ACFMReadMemo_send(int fd, char *result, int index, int num,
								 int dataindex, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ACFMReadMemo");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(index), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(num), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(dataindex), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// д��������԰�
void saacproto_ACFMWriteMemo_send(int fd, char *result, int index)
{
	saacproto_CreateHeader(saacproto.work, "ACFMWriteMemo");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(index), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �г�����ݵ�
void saacproto_ACFMPointList_send(int fd, char *result, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ACFMPointList");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

// �������ݵ�
void saacproto_ACSetFMPoint_send(int fd, char *result, int r, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACSetFMPoint");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(r), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �趨����ݵ�
void saacproto_ACFixFMPoint_send(int fd, char *result, int r)
{
	saacproto_CreateHeader(saacproto.work, "ACFixFMPoint");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(r), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �峤����ϵ�����Ա�㲥
void saacproto_ACFMAnnounce_send(int fd, char *result, char *fmname,
								 int fmindex, int index, int kindflag, char *data, int color)
{
	saacproto_CreateHeader(saacproto.work, "ACFMAnnounce");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(fmname), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(fmindex), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(index), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(kindflag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(color), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �г��������а�
void saacproto_ACShowTopFMList_send(int fd, char *result, int kindflag,
									int num, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ACShowTopFMList");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(kindflag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(num), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

void saacproto_ACFixFMData_send(int fd, char *result, int kindflag, char *data1, char *data2, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACFixFMData");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(kindflag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data1), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data2), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// �趨����ݵ�
void saacproto_ACFixFMPK_send(int fd, char *result, int award, int winindex,
							  int loseindex)
{
	saacproto_CreateHeader(saacproto.work, "ACFixFMPK");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(award), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(winindex), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(loseindex), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// GM������������
void saacproto_ACGMFixFMData_send(int fd, char *result, char *fmname, int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACGMFixFMData");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(fmname), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// ȡ�ü�������
void saacproto_ACGetFMData_send(int fd, char *result, int kindflag, int data,
								int charfdid)
{
	saacproto_CreateHeader(saacproto.work, "ACGetFMData");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(kindflag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charfdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
//
void saacproto_ACFMClearPK_send(int fd, char *result, char *fmname, int fmindex,
								int index)
{
	saacproto_CreateHeader(saacproto.work, "ACFMClearPK");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(result), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(fmname), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(fmindex), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(index), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
// ֪ͨ����������߹���
void saacproto_ACManorPKAck_send(int fd, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ACManorPKAck");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

#ifdef _AC_SEND_FM_PK // WON ADD ׯ԰��ս�б�������AC
void saacproto_ACLoadFmPk_send(int fd, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ACLoadFmPk");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
#ifdef _ACFMPK_LIST
void saacproto_ACSendFmPk_send(int fd, int userindex, int flg)
{
	saacproto_CreateHeader(saacproto.work, "ACSendFmPk");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(userindex), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(flg), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
#endif

#endif

#ifdef _WAEI_KICK
void saacproto_ACKick_send(int fd, int act, char *data, int id)
{
	saacproto_CreateHeader(saacproto.work, "ACKick");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(act), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(id), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _SEND_EFFECT // WON ADD ����ѩ���������Ч
void saacproto_SendEffect_send(int fd, char *effect)
{
	saacproto_CreateHeader(saacproto.work, "EFFECT");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(effect), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _CHAR_POOLITEM
void saacproto_ACCharSavePoolItem_send(int fd, char *Acces,
									   char *Pooldataarg, int clifdid)
{
	saacproto_CreateHeader(saacproto.work, "ACSavePoolItem");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(Acces), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(Pooldataarg), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(clifdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

void saacproto_ACCharGetPoolItem_send(int fd, char *Acces, char *Pooldataarg, int clifdid, int npcid)
{
	saacproto_CreateHeader(saacproto.work, "ACGetPoolItem");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(Acces), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(Pooldataarg), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(clifdid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(npcid), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _CHAR_POOLPET
void saacproto_ACCharSavePoolPet_send(int fd, char *Acces,
									  char *Pooldataarg, int clifdid)
{
	saacproto_CreateHeader(saacproto.work, "ACSavePoolPet");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(Acces), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(Pooldataarg), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(clifdid), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work);
}

void saacproto_ACCharGetPoolPet_send(int fd, char *Acces, char *Pooldataarg, int clifdid, int npcid)
{
	saacproto_CreateHeader(saacproto.work, "ACGetPoolPet");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(Acces), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(Pooldataarg), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(clifdid), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(npcid), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _ANGEL_SUMMON
//void saacproto_ACMissionTable_send( int fd, int num, int type, char *data, int charaindex)
void saacproto_ACMissionTable_send(int fd, int num, int type, char *data, char *angelinfo)
{
	saacproto_CreateHeader(saacproto.work, "ACMissionTable");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(num), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(type), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	//saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( charaindex ) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(angelinfo), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _TEACHER_SYSTEM
void saacproto_ACCheckCharacterOnLine_send(int fd, int charaindex, int iOnline, char *data, int flag)
{
	saacproto_CreateHeader(saacproto.work, "ACCheckCharacterOnLine");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(charaindex), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(iOnline), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(flag), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _ITEM_PET_LOCKED
void saacproto_ItemPetLocked_send(int fd, int clifd, int flag, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ItemPetLocked");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(clifd), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(flag), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}

void saacproto_ItemPetLockedPasswd_send(int fd, int clifd, char *data)
{
	saacproto_CreateHeader(saacproto.work, "ItemPetLockedPasswd");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(clifd), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _FORMULATE_AUTO_PK
void saacproto_FormulateAutoPk_send(int fd, int clifd, char *data)
{
	saacproto_CreateHeader(saacproto.work, "FormulateAutoPk");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(clifd), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _LOTTERY_SYSTEM
void saacproto_LotterySystem_send(int fd, char *award)
{
	saacproto_CreateHeader(saacproto.work, "LotterySystem");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(award), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _ALL_SERV_SEND
void saacproto_AllServSend_send(int fd, char *data, int color)
{
	saacproto_CreateHeader(saacproto.work, "AllServSend");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(data), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(color), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _FAMILY_PK_FIX
void saacproto_ACFmPkOnlineSn_send(int fd, int manorindex, char *gsname, int flag)
{
	saacproto_CreateHeader(saacproto.work, "ACFmPkOnlineSn");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(manorindex), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(gsname), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(flag), saacproto.workbufsize);

	saacproto_Send(fd, saacproto.work);
}
#endif

#ifdef _NB_GM_SERVERLIST
void saacproto_gm_shutdown_send(int fd, int itype, char *msg)
{
	saacproto_CreateHeader(saacproto.work, "GMSERVERLIST");
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_int(itype), saacproto.workbufsize);
	saacproto_strcatsafe(saacproto.work, saacproto_mkstr_string(msg), saacproto.workbufsize);
	saacproto_Send(fd, saacproto.work); //����GMSV
}
#endif

/* end of the generated server code */
