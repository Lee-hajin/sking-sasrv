#define _CHAR_C_

// Spock +1 2000/12/6
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "main.h"
#include "char.h"
#include "saacproto_serv.h"
#include "util.h"

// CoolFish: Family 2001/6/12
#include "acfamily.h"
#include "version.h"

// Arminius 7.17 memory lock
#include "lock.h"
#include "recv.h"

#ifdef _SASQL
#include "sasql.h"
#endif

static void getCharNameFromString(char *str, char *out);
static void getCharOptionFromString(char *str, char *out);
static void makeCharFileName(char *idstring, char *output, int outlen, int num);
static void makeDeletCharFileName(char *id, char *output, int outlen, int num);
// Nuke *1*1
static int makeSaveCharString(char *out, int outlen,
							  char *nm, char *opt, char *info);
static int findBlankCharIndex(char *id);

static int unlinkCharFile(char *id, int num);
#ifdef _SLEEP_CHAR // ȡ�÷ǻ�Ծ���ﵵ��
static void makeSleepCharFileName(char *id, char *output, int outlen, int num);
static void makeSleepCharPoolItemFileName(char *id, char *output, int outlen);
static void makeSleepCharPoolPetFileName(char *id, char *output, int outlen);
static void makeCharPoolPetFileName(char *id, char *output, int outlen);
#endif
#ifdef _CHAR_POOLITEM
static void makeCharPoolItemFileName(char *id, char *output, int outlen);
#endif
#define SPACE '|'
#define SPACESTRING "|"

#define IS_2BYTEWORD(_a_) ((char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF))

#define USERLOG(id, format, args...)                                           \
	{                                                                          \
		char body[1000];                                                       \
		char fn[1000];                                                         \
		if (id[0])                                                             \
		{                                                                      \
			snprintf(body, sizeof(body), "%s.log.%d", id, get_rotate_count()); \
			makeDirFilename(fn, sizeof(fn), logdir, getHash(id), body);        \
			LOGBASE(fn, "%u ", (unsigned int)time(NULL));                      \
			LOGBASE(fn, format, ##args);                                       \
		}                                                                      \
	}

#ifdef _ANGEL_SUMMON
struct MissionTable missiontable[MAXMISSIONTABLE];
#endif

/*

	
		�𼰰����̻�ƥ������ľ���������ѷ�������  ��ë��������
		
			
				char *c0 : ID
				char *c1 : Passwd  �ֵ�  ������ƥ�¾�������ئ��
				char *c2 : Charname
				char *c3 : opt
				Spock fixed, opt change to process
				char *c3 : process
				char *c4 : ��������
				int i0 : lock  
				int i1 : mesgid 
*/

void charLoadCallback(int ti, int auth, char *c0, char *c1,
					  char *c2, char *c3, char *c4, int i0, int i1)
{
	// Spock deleted 2000/11/2
	//static int process_id = 0;
	// CoolFish: Init charindex 2001/10/16
#ifdef _NewSave
	int charindex = -1;
#else
	int charindex;
#endif
	char loadbuf[CHARDATASIZE];
	char infobuf[CHARDATASIZE];
	int lock = i0;
	char *process = c3;
	char *id = c0;
	char *passwd = c1;
	char *charname = c2;
	int mesgid = i1;

	char *deadline = c4;

	// Spock deleted 2000/11/2
	//process_id++;

	if (auth != 0)
	{
		char data[100];

		snprintf(data, sizeof(data), "%d", auth);
#ifdef _NewSave
		saacproto_ACCharLoad_send(ti, FAILED, data, mesgid, charindex);
#else
		saacproto_ACCharLoad_send(ti, FAILED, data, mesgid);
#endif
		return;
	}
	if (isLocked(id))
	{
		int process = atoi(c3);
#ifdef _NewSave
		saacproto_ACCharLoad_send(ti, FAILED, "locked", mesgid, charindex);
#else
		saacproto_ACCharLoad_send(ti, FAILED, "locked", mesgid);
#endif
		DeleteMemLock(getHash(id) & 0xff, id, &process); // ���AP������AC����
		log("\n (%s) ACͬһ��ϵ�ظ����룬����!! ", id);
		saacproto_ACKick_recv(ti, id, 1, -1); //����

		checkGSUCheck(id);
		return;
	}

	charindex = getCharIndexByName(id, charname);
#ifdef _NewSave
	//log("\n����װ�����:%d �˺�:%s ����:%s\n", charindex, id, charname);
#endif

	if (charindex < 0)
	{
		/* ƽ�ҷ»������ئ�м�ƥ�޷¡�ئ���� */
#ifdef _NewSave
		saacproto_ACCharLoad_send(ti, FAILED,
								  "char nonexistent", mesgid, charindex);
#else
		saacproto_ACCharLoad_send(ti, FAILED,
								  "char nonexistent", mesgid);
#endif
		return;
	}

	//    log( "loadCharOne: id:[%s] char:[%s]\n", id, charname );

	if (loadCharOne(id, charindex, loadbuf, sizeof(loadbuf)) < 0)
	{
#ifdef _NewSave
		saacproto_ACCharLoad_send(ti, FAILED,
								  "cannot load ( disk i/o error?)", mesgid, charindex);
#else
		saacproto_ACCharLoad_send(ti, FAILED,
								  "cannot load ( disk i/o error?)", mesgid);
#endif
		return;
	}
	else
	{
#ifdef _CHAR_POOLITEM
		checkCharPoolItem(id);
#endif
#ifdef _CHAR_POOLPET
		checkCharPoolPet(id);
#endif
	}
	{ //ttom
		char *c_ptr;
		c_ptr = loadbuf;
		while (*c_ptr != '\0')
		{
			if (IS_2BYTEWORD(*c_ptr))
			{
				if (*(c_ptr + 1) == ' ')
				{
					*(c_ptr + 1) = 0x41;
				}
				c_ptr++;
				if (*c_ptr == '\0')
					break;
			}
			c_ptr++;
		}
	} //ttom

	if (lock)
	{
		char result[100];
		char retdata[100];

#ifdef _LOCK_ADD_NAME
		if (lockUser(getGSName(ti), id, charname, passwd, 1, result, sizeof(result),
					 retdata, sizeof(retdata), process, deadline) < 0)
		{
#else
		// Spock 2000/11/2
		if (lockUser(getGSName(ti), id, passwd, 1, result, sizeof(result),
					 retdata, sizeof(retdata), process, deadline) < 0)
		{
#endif
			saacproto_ACCharLoad_send(ti, FAILED, "lock FAIL!!", mesgid, charindex);
			return;
		}
	}
	memset(infobuf, 0, sizeof(infobuf));
	getCharInfoFromString(loadbuf, infobuf);
	makeStringFromEscaped(infobuf);
#ifdef _NewSave
	saacproto_ACCharLoad_send(ti, SUCCESSFUL, infobuf, mesgid, charindex);
#else
	saacproto_ACCharLoad_send(ti, SUCCESSFUL, infobuf, mesgid);
#endif

#ifdef _WAEI_KICK
	saacproto_ACKick_recv(ti, id, 10, -1); //��������ϵ
#endif
}

#ifdef _NewSave
int charSave(int ti, char *id,
			 char *charname, char *opt, char *charinfo,
			 int unlock, int mesgid, int charindex)
#else
int charSave(int ti, char *id,
			 char *charname, char *opt, char *charinfo,
			 int unlock, int mesgid)
#endif
{
#ifdef _NewSave
#else
	int charindex;
#endif
	char savebuf[CHARDATASIZE];
	int ret = -1;

	memset(savebuf, 0, sizeof(savebuf));

	//andy_log
	if (strstr(charinfo, "DATAEND=") == NULL)
	{
		FILE *fp;
		if ((fp = fopen("badpetstring.txt", "a+")) != NULL)
		{
			fprintf(fp, "%s\n", charinfo);
			fclose(fp);
		}
		log("err add batpetstring.txt:%s[%s] !\n", id, charname);
	}

	if (unlock)
	{
		char result[100];
		char retdata[100];
#ifdef _LOCK_ADD_NAME
		if ((ret = lockUser(getGSName(ti), id, "", "0", 0, result, sizeof(result),
							retdata, sizeof(retdata), "0", "0")) < 0)
		{
#else
		if ((ret = lockUser(getGSName(ti), id, "0", 0, result, sizeof(result),
							retdata, sizeof(retdata), "0", "0")) < 0)
		{
#endif
			log("����:%s ʧ��!!\n", id);
		}
	}
	// Nuke *1 add escape
	if (makeSaveCharString(savebuf, sizeof(savebuf),
						   charname, opt, charinfo) < 0)
	{
		log("\n AC�浵:̫��  ");
		saacproto_ACCharSave_send(ti, FAILED, "too long", mesgid);
		// Spock fixed 2000/11/1
		return ret;
	}
#ifdef _NewSave
	if (charindex == -1)
		charindex = getCharIndexByName(id, charname);
#else
	// Nuke *1-
	charindex = getCharIndexByName(id, charname);
#endif
	if (charindex < 0)
	{
		int blankind = findBlankCharIndex(id);
		if (blankind < 0)
		{
			log("\n ACCharSave:char full  ");
			saacproto_ACCharSave_send(ti, FAILED, "char full", mesgid);
			return ret;
		}
		else
		{
			charindex = blankind;
		}
	}

	log("�˺�:[%s] ����:[%s]\n", id, charname);
	if (saveCharOne(id, charindex, savebuf) < 0)
	{
		log("\n ACCharSave:disk I/O error or a bug  ");
		saacproto_ACCharSave_send(ti, FAILED, "disk I/O error or a bug", mesgid);
		return ret;
	}

	saacproto_ACCharSave_send(ti, SUCCESSFUL, "", mesgid);
	return ret;
}

void charListCallback(int ti, int auth, char *c0, char *c1,
					  char *c2, char *c3, char *c4, int i0, int i1, int charlistflg)
{
	char listbuf[CHARDATASIZE];
	char *id = c0;
	int mesgid = i0;

	//log(" �����б�����:%s:%d \n", id, auth);

	if (auth != 0)
	{
		char data[100];
		snprintf(data, sizeof(data), "%d", auth);
		saacproto_ACCharList_send(ti, FAILED, "����˺���ʱ�޷���½��", mesgid);
		total_ng_charlist++;
		return;
	}
	// ȡ������ unlock ����
	if (charlistflg == 0)
	{
		if (isLocked(id))
		{
			saacproto_ACCharList_send(ti, FAILED, "�� ��½��Ϸʧ�ܣ��볢�����µ�½ ��", mesgid);
			saacproto_ACKick_recv(ti, id, 1, -1);
			checkGSUCheck(id);
			total_ng_charlist++;
			return;
		}
	}
#ifdef _SLEEP_CHAR // ������µ�, ���ɵ������µ�
	{
		char fn_old[256], fn_new[256];
		FILE *fp_old, *fp_new;
		int i;

		// �����ﵵ
		for (i = 0; i < MAXCHAR_PER_USER; i++)
		{
			makeCharFileName(id, fn_new, sizeof(fn_new), i);
			fp_new = fopen(fn_new, "r");
			if (fp_new == NULL)
			{
				makeSleepCharFileName(id, fn_old, sizeof(fn_old), i);
				fp_old = fopen(fn_old, "r");
				if (fp_old != NULL)
				{
					fclose(fp_old);
					rename(fn_old, fn_new); // ����
					//filecopy( fn_old, fn_new); // ����
					log(" �Ƶ�_%s ", fn_new);
				}
			}
			else
			{
				fclose(fp_new);
			}
		}
#ifdef _CHAR_POOLITEM
		// �Ʋ���Ʒ�⵵
		makeCharPoolItemFileName(id, fn_new, sizeof(fn_new));
		fp_new = fopen(fn_new, "r");
		if (fp_new == NULL)
		{
			makeSleepCharPoolItemFileName(id, fn_old, sizeof(fn_old));
			fp_old = fopen(fn_old, "r");
			if (fp_old != NULL)
			{
				fclose(fp_old);
				rename(fn_old, fn_new); // ����
				//filecopy( fn_old, fn_new); // ����
				log(" �Ƶ�_%s ", fn_new);
			}
		}
		else
		{
			fclose(fp_new);
		}
#endif
#ifdef _CHAR_POOLPET
		// �Ʋֳ���⵵
		makeCharPoolPetFileName(id, fn_new, sizeof(fn_new));
		fp_new = fopen(fn_new, "r");
		if (fp_new == NULL)
		{
			makeSleepCharPoolPetFileName(id, fn_old, sizeof(fn_old));
			fp_old = fopen(fn_old, "r");
			if (fp_old != NULL)
			{
				fclose(fp_old);
				rename(fn_old, fn_new); // ����
				//filecopy( fn_old, fn_new); // ����
				log(" �Ƶ�_%s ", fn_new);
			}
		}
		else
		{
			fclose(fp_new);
		}

#endif
	}
#endif

	loadCharNameAndOption(id, listbuf, sizeof(listbuf));
	// Arminius
	saacproto_ACCharList_send(ti, SUCCESSFUL, listbuf, mesgid);
	total_ok_charlist++;
}

void charDeleteCallback(int ti, int auth,
						char *c0, char *c1, char *c2, char *c3,
						char *c4, int i0, int i1)
{
	int charindex;
	char *id = c0;
	char *charname = c2;
	int mesgid = i0;
	struct tm *now;
	time_t timenow;
	char logfile[128];

	if (auth != 0)
	{
		char data[100];
		snprintf(data, sizeof(data), "%d", auth);
		saacproto_ACCharDelete_send(ti, FAILED, data, mesgid);
		return;
	}

	if (isLocked(id))
	{
		int process = atoi(c3);
		saacproto_ACCharDelete_send(ti, FAILED, "locked", mesgid);
		DeleteMemLock(getHash(id) & 0xff, id, &process); // ���AP������AC����
		log("\n (%s) ACͬһ��ϵ�ظ����룬����!! ", id);
		saacproto_ACKick_recv(ti, id, 1, -1); //����

		checkGSUCheck(id);
		return;
	}

	charindex = getCharIndexByName(id, charname);
	if (charindex < 0)
	{
		saacproto_ACCharDelete_send(ti, FAILED, "nochar", mesgid);
		return;
	}

	if (unlinkCharFile(id, charindex) < 0)
	{
		saacproto_ACCharDelete_send(ti, FAILED, "fileI/O", mesgid);
		return;
	}
#ifdef _FAMILY
	// CoolFish: Family 2001/6/12
	{
		int index = 0, fmindex = 0, fmcharindex = 0;
		char fmname[32];
		index = ACgetFMInfoFromChar(fmname, &fmindex, charname, id, &fmcharindex);
		if (index != -1)
		{
			if (fmcharindex == 0)
			{
#ifdef _FMVER21
				if (ChangeFMLeader(index, fmname, fmindex) >= 0)
				{
					if (ACDelFM(index, fmname, fmindex) >= 0)
						log("ɾ�������Ա:%d ������:%s ��������:%d ����:%s, �˺�:%s, ������������:%d\n",
							index, fmname, fmindex, charname, id, fmcharindex);
				}
				log("ChangeFMLeader_3 index:%d fmname:%s fmindex:%d\n", index, fmname, fmindex);
#else
				if (ACDelFM(index, fmname, fmindex) >= 0)
					log("ɾ�������Ա:%d ������:%s ��������:%d ����:%s, �˺�:%s, ������������:%d\n",
						index, fmname, fmindex, charname, id, fmcharindex);
#endif
			}
			else
			{
#ifdef _FMVER21
				if (ACMemberLeaveFM(index, fmname, fmindex, charname, 0, 0, fmcharindex) >= 0)
					log("ACMemberLeaveFM index:%d ������:%s ��������:%d ����:%s, �˺�:%s, ������������:%d\n",
						index, fmname, fmindex, charname, id, fmcharindex);
#else
				if (ACMemberLeaveFM(index, fmname, fmindex, charname, 0, fmcharindex) >= 0)
					log("ACMemberLeaveFM index:%d ������:%s ��������:%d ����:%s, �˺�:%s, ������������:%d\n",
						index, fmname, fmindex, charname, id, fmcharindex);
#endif
			}
		}
	}
#endif
	// Spock 2000/12/6
	time(&timenow);
	now = localtime(&timenow);
	sprintf(logfile, "log/chardel/%04d%02d%02d.log",
			now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	LOGBASE(logfile, "%04d/%02d/%02d %02d:%02d:%02d id:[%s] char:[%s] index:[%d]\n",
			now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
			now->tm_hour, now->tm_min, now->tm_sec,
			id, charname, charindex);
	// Spock end
	USERLOG(id, "�˺�ɾ��: �ɹ�\n");
	saacproto_ACCharDelete_send(ti, SUCCESSFUL, "ok", mesgid);
}

void dummyCallback(int ti, int auth,
				   char *c0, char *c1, char *c2, char *c3,
				   char *c4, int i0, int i1)
{
	return;
}

static void getCharNameFromString(char *str, char *out)
{
	int c;

	out[0] = '\0';
	for (c = 0;; c++)
	{
		if (IS_2BYTEWORD(str[c]))
		{
			out[c] = str[c];
			c++; // �ݼ�  ٯ��
			out[c] = str[c];
			continue;
		}

		out[c] = str[c];
		if (str[c] == '\0')
			break;

		if (str[c] == SPACE)
		{
			out[c] = '\0';
			break;
		}
	}
}

static void getCharOptionFromString(char *str, char *out)
{
	int c;
	int outc = 0;
	out[outc] = '\0';
	for (c = 0;; c++)
	{
		if (str[c] == '\0')
			break;
		if (IS_2BYTEWORD(str[c]))
		{
			c++;
			if (str[c] == '\0')
				break; // ����  ٯ����ƥ��NULLئ�ձ�
			continue;
		}

		if (str[c] == SPACE)
		{
			c++;
			for (;; c++)
			{

				if (str[c] == '\0')
				{ // ��  �������� NULL   ľ���δ���
					out[outc] = '\0';
					break;
				}

				// ����֨�������  ٯ����������  ئ���ֵ������������������
				//----   For 2Byte Code ----
				if (IS_2BYTEWORD(str[c]))
				{
					out[outc++] = str[c++]; // ��������  �����
					out[outc++] = str[c];	// ��������  �����
					if (str[c] == '\0')
						break; // ����  ٯ����ƥ��NULLئ�ձ�
					continue;
				}

				// ɧ��  ٯ���������������
				out[outc] = str[c]; // ���������
				if (str[c] == '\0')
					break; // ��  �������ձδ���

				if (str[c] == SPACE)
				{
					out[outc] = '\0';
					break;
				}
				outc++;
			}
			return;
		}
	}
}

void getCharInfoFromString(char *str, char *out)
{
	int c = 0, outc = 0;
	int space_counter = 0;

	out[outc] = '\0';
	for (c = 0;; c++)
	{
		if (str[c] == '\0')
			break;
		if (IS_2BYTEWORD(str[c]))
		{
			c++;
			continue;
		}
		else if (str[c] == SPACE)
		{
			space_counter++;
		}
		if (space_counter == 2 && str[c] == SPACE)
		{
			c++;
			for (;; c++)
			{
				if (IS_2BYTEWORD(str[c]))
				{
					out[outc++] = str[c++];
					out[outc++] = str[c];
					if (str[c] == '\0')
					{
						out[outc] = '\0';
						break;
					}
					continue;
				}

				out[outc] = str[c];
				if (str[c] == '\0')
					break;
				if (str[c] == SPACE)
				{
					out[outc] = '\0';
					break;
				}
				outc++;
			}
			return;
		}
	}
}
static void makeDeletCharFileName(char *id, char *output, int outlen, int num)
{
	char body[1024];
	if (strlen(id) < 1)
		return;

	struct tm tm1;
	time_t timep;
	time(&timep);
	memcpy(&tm1, localtime(&timep), sizeof(tm1));

	snprintf(body, sizeof(body), "%s.%d.char.%d.%d.%d.%d.%d.%d", id, num,
			 tm1.tm_year + 1900,
			 tm1.tm_mon + 1,
			 tm1.tm_mday,
			 tm1.tm_hour,
			 tm1.tm_min,
			 tm1.tm_sec);
	makeDirFilename(output, outlen, "char_delet", getHash(id), body);
}

static void makeCharFileName(char *id, char *output, int outlen, int num)
{
	char body[1024];
	if (strlen(id) < 1)
		return;

	snprintf(body, sizeof(body), "%s.%d.char", id, num);
	makeDirFilename(output, outlen, chardir, getHash(id), body);
}

#ifdef _SLEEP_CHAR // ȡ�÷ǻ�Ծ���ﵵ��
static void makeSleepCharFileName(char *id, char *output, int outlen, int num)
{
	char body[1024];
	if (strlen(id) < 1)
		return;

	snprintf(body, sizeof(body), "%s.%d.char", id, num);
	makeDirFilename(output, outlen, sleepchardir, getHash(id), body);
}
#endif

int loadCharNameAndOption(char *id, char *output, int outlen)
{
	char chbuf[MAXCHAR_PER_USER][CHARDATASIZE];
	char namework[CHARDATASIZE];
	char optionwork[CHARDATASIZE];
	char nm_work[CHARDATASIZE];

	int i, count = 0;
	int so_far_bytes = 0;
	output[0] = 0;

	for (i = 0; i < MAXCHAR_PER_USER; i++)
	{
		int l;
		char delim[2];
		chbuf[i][0] = 0;
		loadCharOne(id, i, chbuf[i], sizeof(chbuf[i]));

		getCharNameFromString(chbuf[i], namework);
		getCharOptionFromString(chbuf[i], optionwork);

		if (chbuf[i][0] == 0)
			continue;
		if (strlen(output) > 0)
		{
			delim[0] = SPACE;
			delim[1] = 0;
		}
		else
		{
			delim[0] = 0;
		}

		snprintf(nm_work, sizeof(nm_work),
				 "%s%s|%s", delim, namework, optionwork);
		l = strlen(nm_work);

		if ((so_far_bytes + l) < outlen)
		{
			strcat(output, nm_work);
			so_far_bytes += l;
			count++;
		}
	}

	return count;
}

int loadCharOne(char *id, int num, char *output, int outlen)
{
	char fn[1024];
	FILE *fp;
	char c_temp, *c_ptr;
	c_ptr = output;

	makeCharFileName(id, fn, sizeof(fn), num);
	fp = fopen(fn, "r");
	if (fp == NULL)
	{
		return -1;
	}

	do
	{
		c_temp = fgetc(fp);
		*c_ptr = c_temp;
		c_ptr++;
	} while (c_temp != EOF);
	*c_ptr = '\0';

	if (output[0] == '|' && output[1] == '|')
	{
		return -1;
	}

#ifdef _FIX_CAHR_VERSION
	{
		char *strp;
		while ((strp = strstr(output, "\\a")) != NULL)
		{
			*(strp + 1) = 'n';
		}
	}
#endif

	fclose(fp);
	return 0;
}

int saveCharOne(char *id, int num, char *input)
{
	char fn[1024];
	FILE *fp;
	char *strp;
	char *strp1; // Won ���� hp Ϊ������

	makeCharFileName(id, fn, sizeof(fn), num);
	fp = fopen(fn, "w");
	if (fp == NULL)
	{
		log("save error 01: %d", errno); //Change add errno
		return -1;
	}

	// Won ���� hp Ϊ������
	if ((strp = strstr(input, "\\nhp=-")) != NULL)
	{
		strp1 = strstr(input, "\\nmp=");
		*(strp + 5) = '1';
		strcpy((strp + 6), strp1);
	}

	// Arminius
	if ((strp = strstr(input, "\\nhp=0\\n")) != NULL)
		*(strp + 5) = '1';

#ifdef _FIX_CAHR_VERSION
	{
		char *strp;
		while ((strp = strstr(input, "\\n")) != NULL)
		{
			*(strp + 1) = 'a';
		}
	}
#endif

	fprintf(fp, "%s", input);
	fclose(fp);

	log("д�� %s �����ļ�:%s\n", id, fn);
	chmod(fn, 0777);
	return 0;
}

static int makeSaveCharString(char *out, int outlen,
							  char *nm, char *opt, char *info)
{
	char nmwork[CHARDATASIZE];
	char optwork[CHARDATASIZE];
	char infowork[CHARDATASIZE];
	char outwork[CHARDATASIZE];
	char *nmwork_p, *optwork_p, *infowork_p;
	int l;

	/* ������ƻ� */

	strncpy(nmwork, nm, sizeof(nmwork));
	nmwork[strlen(nm)] = 0;

	strncpy(optwork, opt, sizeof(optwork));
	optwork[strlen(opt)] = 0;

	strncpy(infowork, info, sizeof(infowork));
	infowork[strlen(info)] = 0;

	/* �޵��������� */
	/*    if ( esc ) {
	nmwork_p = makeEscapeString1( nm , nmwork ,sizeof( nmwork ));
	optwork_p = makeEscapeString1( opt , optwork , sizeof( optwork ));
	infowork_p = makeEscapeString1( info , infowork , sizeof( infowork ));
} else {*/

	nmwork_p = makeEscapeString(nm, nmwork, sizeof(nmwork));
	optwork_p = makeEscapeString(opt, optwork, sizeof(optwork));
	infowork_p = makeEscapeString(info, infowork, sizeof(infowork));

	//    }
	/* ��ئ���£����л��޷¡����뼰�׻�����Է����ͱ���ľ�£� */
	snprintf(outwork, sizeof(outwork),
			 "%s" SPACESTRING "%s" SPACESTRING "%s",
			 nmwork_p, optwork_p, infowork_p);

	l = strlen(outwork);
	if (l >= (outlen - 1))
	{
		return -1;
	}

	/* NULL   ٯ��    ��ؤ�¾���0ë��ľ�����о��ϼ��֣� */
	memcpy(out, outwork, l + 1);

	return 0;
}

int getCharIndexByName(char *id, char *charname)
{
	int i;
	for (i = 0; i < MAXCHAR_PER_USER; i++)
	{
		char output[CHARDATASIZE];
		if (loadCharOne(id, i, output, sizeof(output)) < 0)
		{
			/* ؤ�л����¡� */
			continue;
		}
		else
		{
			/* ƽ�ҷ»�������� */
			char cn[CHARDATASIZE];
			getCharNameFromString(output, cn);

			/* Escapeëݩ�� */
			if (strcmp(charname, makeStringFromEscaped(cn)) == 0)
			{
				return i;
			}
			else
			{
			}
		}
	}
	return -1;
}

/*

  ƽ�ҷ¼�ؤ�л����µ�������ë��������
	
		  ئ��ؤ�л���ئ�У�0����ئ��ؤ�嵩��������index.
		
			
				ǩ������ƥ��ئ�������չ�ľë�����ʷ����
				��������������ƥ����֧Ƿ�����֣�
				
*/
static int findBlankCharIndex(char *id)
{
	int i;
	char output[CHARDATASIZE];
	for (i = 0; i < MAXCHAR_PER_USER; i++)
	{
		if (loadCharOne(id, i, output, sizeof(output)) < 0)
		{
			return i;
		}
	}
	return -1;
}

static int unlinkCharFile(char *id, int num)
{
	char fname[1024];
	char dname[1024];
#ifdef _SLEEP_CHAR //ɾ������ʱ�ɵ�Ҳɾ
	makeSleepCharFileName(id, fname, sizeof(fname), num);
	unlink(fname);
#endif
	makeCharFileName(id, fname, sizeof(fname), num);
	makeDeletCharFileName(id, dname, sizeof(dname), num);

	return rename(fname, dname);
}

#ifdef _LOCK_ADD_NAME
int lockUser(char *gmsvname, char *id, char *name, char *passwd, int lock,
			 char *result, int resultlen,
			 char *retdata, int retdatalen, char *process, char *deadline)
#else
int lockUser(char *gmsvname, char *id, char *passwd, int lock,
			 char *result, int resultlen,
			 char *retdata, int retdatalen, char *process, char *deadline)
#endif
{
	int ret = -1;

	if (!id[0])
	{
		snprintf(result, resultlen, FAILED);
		snprintf(retdata, retdatalen, "bad id");
		return -1;
	}

	retdata[0] = 0;

	if (lock)
	{
		if (isLocked(id))
		{
			snprintf(result, resultlen, FAILED);
			snprintf(retdata, retdatalen, "already locked");
			log("д���ڴ���Ϣ: �û�:%x/%s �Ѿ�ͬ������ !!\n", getHash(id), id);
			return -1;
		}
		else
		{
#ifdef _LOCK_ADD_NAME
			if (InsertMemLock(getHash(id) & 0xff, id, name, passwd, gmsvname, atoi(process), deadline))
#else
			if (InsertMemLock(getHash(id) & 0xff, id, passwd, gmsvname, atoi(process), deadline))
#endif
				return 0;
			else
				return -1;
		}
	}
	else
	{
		if (!isLocked(id))
		{
			log("ɾ���ڴ���Ϣ: �û�:%x/%s û������!!\n", getHash(id), id);
		}
		if (DeleteMemLock(getHash(id) & 0xff, id, &ret))
		{
			snprintf(result, resultlen, SUCCESSFUL);
			snprintf(retdata, retdatalen, "removed");
			return ret;
		}
		else
		{
			snprintf(result, resultlen, FAILED);
			snprintf(retdata, retdatalen, "�����Ƴ�����");

			log("���ܽ��� %x:%s !\n", getHash(id), id);
			return ret;
		}
	}
}

int isLocked(char *id)
{
	if (!id[0])
		return 1; // invalid id: lock it
	return isMemLocked(getHash(id) & 0xff, id);
}

int checkValidCharName(char *a)
{
	int i;
	for (i = 0;; i++)
	{
		if (a[i] == 0)
			break;
		if (a[i] == SPACE)
			return -1;
	}
	return 0;
}

#ifdef _CHAR_POOLITEM
static void makeCharPoolItemFileName(char *id, char *output, int outlen)
{
	char poolitem[256];
	if (strlen(id) < 1)
		return;
	snprintf(poolitem, sizeof(poolitem), "%s.item", id);
	makeDirFilename(output, outlen, chardir, getHash(id), poolitem);
}

#ifdef _SLEEP_CHAR // ȡ�÷ǻ�Ծ����ֿ⵵��
static void makeSleepCharPoolItemFileName(char *id, char *output, int outlen)
{
	char poolitem[256];
	if (strlen(id) < 1)
		return;
	snprintf(poolitem, sizeof(poolitem), "%s.item", id);
	makeDirFilename(output, outlen, sleepchardir, getHash(id), poolitem);
}
#endif

int InsertCharPoolItem(char *id, char *input, int sizes)
{
	char fn[256];
	FILE *fp;

	if (sizes <= 0)
		return sizes;
	makeCharPoolItemFileName(id, fn, sizeof(fn));

	fp = fopen(fn, "a+");
	if (fp == NULL)
	{
		fp = fopen(fn, "w");
		if (fp == NULL)
		{
			log("���� %s ����.\n", fn);
			return -1;
		}
	}

	fprintf(fp, "%s", input);
	fclose(fp);

	log("���� %s ��Ʒ�ֿ��ļ�:%s\n", id, fn);
	return sizes;
}

int saveCharPoolItem(char *id, char *input, int sizes)
{
	char fn[256];
	FILE *fp;

	if (sizes < 0)
		return sizes;
	if (input == NULL)
		return -1;
	makeCharPoolItemFileName(id, fn, sizeof(fn));

	fp = fopen(fn, "w");
	if (fp == NULL)
	{
		log("���� %s ����.\n", fn);
		return -1;
	}

	fprintf(fp, "%s", input);
	fclose(fp);

	log("д�� %s ���ߵ����ļ�:%s\n", id, fn);
	return sizes;
}

int loadCharPoolItemOne(char *id, char *output, int outlen)
{
	char fn[256];
	FILE *fp;
	char c_temp, *c_ptr;
	int sizes = 0;
	c_ptr = output;

	makeCharPoolItemFileName(id, fn, sizeof(fn));

	fp = fopen(fn, "r");
	if (fp == NULL)
	{
		if ((fp = fopen(fn, "w")) == NULL)
			return -1;
		return 0;
	}

	do
	{
		c_temp = fgetc(fp);
		*c_ptr = c_temp;
		c_ptr++;
		sizes++;
		if (sizes >= (outlen - 1))
			break;
	} while (c_temp != EOF);
	*c_ptr = '\0';
	fclose(fp);
	return sizes;
}

int checkCharPoolItem(char *id)
{
	char fn[256];
	FILE *fp;

	makeCharPoolItemFileName(id, fn, sizeof(fn));
	fp = fopen(fn, "r");
	if (fp == NULL)
	{
		if (saveCharPoolItem(id, "", 0) < 0)
		{
			log("\n �޷���������:%s ��Ʒ�ֿ��ļ�!!", id);
			return -1;
		}
	}
	else
	{
		fclose(fp);
	}

	return 1;
}
#endif

#ifdef _CHAR_POOLPET
static void makeCharPoolPetFileName(char *id, char *output, int outlen)
{
	char poolpet[256];
	if (strlen(id) < 1)
		return;
	snprintf(poolpet, sizeof(poolpet), "%s.pet", id);
	makeDirFilename(output, outlen, chardir, getHash(id), poolpet);
}

#ifdef _SLEEP_CHAR // ȡ�÷ǻ�Ծ����ֿ⵵��
static void makeSleepCharPoolPetFileName(char *id, char *output, int outlen)
{
	char poolpet[256];
	if (strlen(id) < 1)
		return;
	snprintf(poolpet, sizeof(poolpet), "%s.pet", id);
	makeDirFilename(output, outlen, sleepchardir, getHash(id), poolpet);
}
#endif

int InsertCharPoolPet(char *id, char *input, int sizes)
{
	char fn[256];
	FILE *fp;

	if (sizes <= 0)
		return sizes;
	makeCharPoolPetFileName(id, fn, sizeof(fn));

	fp = fopen(fn, "a+");
	if (fp == NULL)
	{
		fp = fopen(fn, "w");
		if (fp == NULL)
		{
			log("���� %s ����.\n", fn);
			return -1;
		}
	}

	fprintf(fp, "%s", input);
	fclose(fp);

	log("���� %s ����ֿ��ļ�:%s\n", id, fn);
	return sizes;
}

int saveCharPoolPet(char *id, char *input, int sizes)
{
	char fn[256];
	FILE *fp;

	if (sizes < 0)
		return sizes;
	if (input == NULL)
		return -1;
	makeCharPoolPetFileName(id, fn, sizeof(fn));

	fp = fopen(fn, "w");
	if (fp == NULL)
	{
		log("���� %s ����.\n", fn);
		return -1;
	}

	fprintf(fp, "%s", input);
	fclose(fp);

	log("д�� %s ���ﵵ���ļ�:%s\n", id, fn);
	return sizes;
}

int loadCharPoolPetOne(char *id, char *output, int outlen)
{
	char fn[256];
	FILE *fp;

	char c_temp, *c_ptr;
	int sizes = 0;
	c_ptr = output;

	makeCharPoolPetFileName(id, fn, sizeof(fn));

	fp = fopen(fn, "r");
	if (fp == NULL)
	{
		if ((fp = fopen(fn, "w")) == NULL)
			return -1;
		return 0;
	}

	do
	{
		c_temp = fgetc(fp);
		*c_ptr = c_temp;
		c_ptr++;
		sizes++;
		if (sizes >= (outlen - 1))
			break;
	} while (c_temp != EOF);
	*c_ptr = '\0';
	fclose(fp);
	return sizes;
}

int checkCharPoolPet(char *id)
{
	char fn[256];
	FILE *fp;

	makeCharPoolPetFileName(id, fn, sizeof(fn));
	fp = fopen(fn, "r");
	if (fp == NULL)
	{
		if (saveCharPoolPet(id, "", 0) < 0)
		{
			log("\n �޷���������:%s ����ֿ��ļ�!!", id);
			return -1;
		}
	}
	else
	{
		fclose(fp);
	}

	return 1;
}
#endif

int filecopy(char *oldfilename, char *newfilename)
{
	FILE *oldfile;
	FILE *newfile;
	char c_temp;

	if ((oldfile = fopen(oldfilename, "r")) == NULL)
	{
		return -1;
	}
	if ((newfile = fopen(newfilename, "w")) == NULL)
	{
		return -1;
	}

	while (1)
	{
		c_temp = fgetc(oldfile);
		if (c_temp == EOF)
			break;
		fputc(c_temp, newfile);
	};

	fclose(oldfile);
	fclose(newfile);

	return 0;
}
