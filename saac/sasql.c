#define _SASQL_C_

#include "version.h"

#ifdef _SASQL //������

#include "main.h"
#include "sasql.h"
#include "util.h"
#include "md5.h"
#include <mysql/mysql.h>

MYSQL mysql;
MYSQL_RES *mysql_result;
MYSQL_ROW mysql_row;
MYSQL_FIELD *fields;

typedef struct tagConfig
{
	char sql_IP[32];
	int sql_Port;
	char sql_Port1[16];
	char sql_ID[16];
	char sql_PS[32];
	char sql_DataBase[16];
	char sql_Table[16];
	char sql_LOCK[16];
	char sql_NAME[16];
	char sql_PASS[16];
	int AutoReg;
	int openbackground;
} Config;

Config config;

static int readSqlConfig(char *path)
{
	char buf[255];
	FILE *fp;

	fp = fopen(path, "r");
	if (fp == NULL)
	{
		return -2;
	}

	while (fgets(buf, sizeof(buf), fp))
	{
		char command[255];
		char param[255];
		chop(buf);

		easyGetTokenFromString(buf, 1, command, sizeof(command));
		easyGetTokenFromString(buf, 2, param, sizeof(param));

		if (strcmp(command, "sql_IP") == 0)
		{
			snprintf(config.sql_IP, sizeof(config.sql_IP), param);
			printf("\n데이터베이스 주소 : %s", config.sql_IP);
		}
		else if (strcmp(command, "sql_Port") == 0)
		{
			config.sql_Port = atoi(param);
			snprintf(config.sql_Port1, sizeof(config.sql_Port1), param);
			printf("\n数据库端口： %d", config.sql_Port);
		}
		else if (strcmp(command, "sql_ID") == 0)
		{
			snprintf(config.sql_ID, sizeof(config.sql_ID), param);
			printf("\n数据库用户： %s", config.sql_ID);
		}
		else if (strcmp(command, "sql_PS") == 0)
		{
			snprintf(config.sql_PS, sizeof(config.sql_PS), param);
			printf("\n数据库密码： %s", config.sql_PS);
		}
		else if (strcmp(command, "sql_DataBase") == 0)
		{
			snprintf(config.sql_DataBase, sizeof(config.sql_DataBase), param);
			printf("\n登陆数据库名：%s", config.sql_DataBase);
		}
		else if (strcmp(command, "sql_Table") == 0)
		{
			snprintf(config.sql_Table, sizeof(config.sql_Table), param);
			printf("\n用户信息表名： %s", config.sql_Table);
		}
		else if (strcmp(command, "sql_LOCK") == 0)
		{
			snprintf(config.sql_LOCK, sizeof(config.sql_LOCK), param);
			printf("\n用户锁定表名： %s", config.sql_LOCK);
		}
		else if (strcmp(command, "sql_NAME") == 0)
		{
			snprintf(config.sql_NAME, sizeof(config.sql_NAME), param);
			printf("\n账号字段名称： %s", config.sql_NAME);
		}
		else if (strcmp(command, "sql_PASS") == 0)
		{
			snprintf(config.sql_PASS, sizeof(config.sql_PASS), param);
			printf("\n密码字段名称： %s", config.sql_PASS);
		}
		else if (strcmp(command, "openbackground") == 0)
		{
			config.openbackground = atoi(param);
			if (config.openbackground == 1)
			{
				printf("\n后台功能：   YES");
			}
			else
			{
				printf("\n后台功能：   NO");
			}
		}
		else if (strcmp(command, "AutoReg") == 0)
		{
			config.AutoReg = atoi(param);
			if (config.AutoReg)
			{
				printf("\n开放自动注册：YES");
			}
			else
			{
				printf("\n开放自动注册：NO");
			}
		}
	}
	fclose(fp);
	return 0;
}

BOOL sasql_init(void)
{
	if ((mysql_init(&mysql) == NULL) & readSqlConfig("acserv.cf"))
	{
		printf("\n Database initialization failed!");
		exit(1);
		return FALSE;
	}

	if (!mysql_real_connect(&mysql,
							config.sql_IP,
							config.sql_ID,		 //�ʺ�
							config.sql_PS,		 //����
							config.sql_DataBase, //ѡ������Ͽ�
							config.sql_Port,
							NULL,
							0))
	{
		printf("\n데이터베이스 연결이 실패했습니다! \n");
		return FALSE;
	}

	mysql_query(&mysql, "set names 'utf8'");
	printf("\n Database connection is successful!\n");
	return TRUE;
}

int sasql_mysql_query(char *sqlstr)
{
	mysql_query(&mysql, "set names 'utf8'");
	return mysql_query(&mysql, sqlstr);
}

void sasql_close(void)
{
	mysql_close(&mysql);
}

BOOL sasql_ckeckStrint(char *str)
{
	int i;
	for (i = 0; i < strlen(str); i++)
	{
		if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') && (str[i] < '0' || str[i] > '9') && (str[i] != '.') && (str[i] != '-'))
		{
			return FALSE;
		}
	}
	return TRUE;
}

int sasql_query(char *nm, char *pas)
{
	char sqlstr[1024];
	if (sasql_ckeckStrint(nm) == FALSE)
	{
		printf("�쳣�ַ����û���%s\n", nm);
		return 3;
	}
	//sprintf(sqlstr,"select `%s`, `salt` from %s where %s=BINARY'%s'",config.sql_PASS, config.sql_Table,config.sql_NAME,nm);
	sprintf(sqlstr, "select `%s`,TO_DAYS( NOW( ) ) - TO_DAYS( LoginTime ) from %s where %s=BINARY'%s'", config.sql_PASS, config.sql_Table, config.sql_NAME, nm);

	if (!sasql_mysql_query(sqlstr))
	{
		int num_row = 0;
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);

		if (num_row > 0)
		{
			char token[64];
			mysql_row = mysql_fetch_row(mysql_result);
			strcpy(token, mysql_row[0]);
#ifdef _OLDPS_TO_MD5PS
			if (strlen(mysql_row[0]) > 16 && strlen(pas) <= 16)
			{
				if (strcmp(MD5String(pas), token) == 0)
				{
					return 1;
				}
				else
				{
					printf("�û�%s�������\n", nm);
					return 2;
				}
			}
			else if (strlen(pas) > 16 && strlen(mysql_row[0]) <= 16)
			{
				if (strcmp(MD5String(token), pas) == 0)
				{
					return 1;
				}
				else
				{
					printf("�û�%s�������\n", nm);
					return 2;
				}
			}
			else
			{
				if (strcmp(pas, token) == 0)
				{
					return 1;
				}
				else
				{
					printf("�û�%s�������\n", nm);
					return 2;
				}
			}
#else
			if (strcmp(pas, token) == 0)
			{
				return 1;
			}
			else
			{
				printf("�û�%s�������\n", nm);
				return 2;
			}
#endif
		}
		else
		{
			printf("�û�%sδע�ᣡ\n", nm);
			return 3;
		}
	}
	else
	{
		printf("\n���ݿ����ʧ�ܣ�\n");
		printf("�����������ݿ�...");
		sasql_close();
		sasql_init();
		printf("���\n");
		return 0;
	}
}

#ifdef _SQL_BACKGROUND
BOOL sasql_online(char *ID, char *NM, char *IP, char *MAC1, char *MAC2, char *MAC3, int flag)
{
	if (config.openbackground == 1)
	{
		if (ID != NULL)
		{
			if (sasql_ckeckStrint(ID) == FALSE)
			{
				printf("�쳣�ַ����û���%s\n", ID);
				return TRUE;
			}
		}

		char sqlstr[256];
		if (flag == 0)
			sprintf(sqlstr, "update %s set Online=0 where %s=BINARY'%s'", config.sql_Table, config.sql_NAME, ID);
		else if (flag == 1)
			sprintf(sqlstr, "update %s set LoginTime=NOW(), IP='%s',MAC1='%s',MAC2='%s',MAC3='%s', Online=0 where %s=BINARY'%s'", config.sql_Table, IP, MAC1, MAC2, MAC3, config.sql_NAME, ID);
		else if (flag == 2)
			sprintf(sqlstr, "update %s set LoginTime=NOW(), Online=1, OnlineName='%s', Path='char/0x%x'where %s=BINARY'%s'", config.sql_Table, NM, getHash(ID) & 0xff, config.sql_NAME, ID);
		else if (flag == 3)
			sprintf(sqlstr, "update %s set Online=0", config.sql_Table);
		if (!sasql_mysql_query(sqlstr))
		{
			return TRUE;
		}
		printf("\n�������ݿ�ʧ��%s\n", sqlstr);
	}
	return TRUE;
}
#endif

#ifdef _SQL_REGISTER
BOOL sasql_register(char *id, char *ps)
{
	char sqlstr[256];
	if (config.AutoReg != 1)
		return FALSE;

	if (sasql_ckeckStrint(id) == FALSE)
	{
		printf("�쳣�ַ����û���%s\n", id);
		return FALSE;
	}
	if (sasql_ckeckStrint(ps) == FALSE)
	{
		printf("�쳣�ַ�����Ϸ����%s\n", ps);
		return FALSE;
	}

#ifdef _OLDPS_TO_MD5PS
	sprintf(sqlstr, "INSERT INTO %s (%s,%s,RegTime,Path) VALUES (BINARY'%s','%s',NOW(),'char/0x%x')", config.sql_Table, config.sql_NAME, config.sql_PASS, id, MD5String(ps), getHash(id) & 0xff);
#else
	sprintf(sqlstr, "INSERT INTO %s (%s,%s,RegTime,Path) VALUES (BINARY'%s','%s',NOW(),'char/0x%x')", config.sql_Table, config.sql_NAME, config.sql_PASS, id, ps, getHash(id) & 0xff);
#endif

	if (!sasql_mysql_query(sqlstr))
	{
		printf("\n���û�ע��ɹ���\n");
		return TRUE;
	}
	printf("\n���û�ע��ʧ�ܣ�\n");
	return FALSE;
}
#endif

BOOL sasql_chehk_lock(char *idip)
{
	if (sasql_ckeckStrint(idip) == FALSE)
	{
		printf("�쳣�ַ�%s\n", idip);
		return FALSE;
	}
	char sqlstr[256];
	sprintf(sqlstr, "select * from `%s` where Name='%s'", config.sql_LOCK, idip);
	if (!sasql_mysql_query(sqlstr))
	{
		int num_row = 0;
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);

		if (num_row > 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL sasql_add_lock(char *idip)
{
	if (sasql_ckeckStrint(idip) == FALSE)
	{
		printf("�쳣�ַ�%s\n", idip);
		return FALSE;
	}
	char sqlstr[256];
	sprintf(sqlstr, "INSERT INTO `%s` (Name) VALUES (BINARY'%s')", config.sql_LOCK, idip);
	if (!sasql_mysql_query(sqlstr))
	{
		printf("\n��������%s�ɹ���\n", idip);
		return TRUE;
	}
	return FALSE;
}

BOOL sasql_del_lock(char *idip)
{
	if (sasql_ckeckStrint(idip) == FALSE)
	{
		printf("�쳣�ַ�%s\n", idip);
		return FALSE;
	}
	char sqlstr[256];
	sprintf(sqlstr, "delete from `%s` where Name=BINARY'%s'", config.sql_LOCK, idip);
	if (!sasql_mysql_query(sqlstr))
	{
		printf("\n�������%s�ɹ���\n", idip);
		return TRUE;
	}
	return FALSE;
}

#ifdef _ITEM_PET_LOCKED
char *sasql_ItemPetLocked(char *id, char *safepasswd)
{
	if (sasql_ckeckStrint(id) == FALSE)
	{
		printf("�쳣�ַ�%s\n", id);
		return "�޷����������뱾������Ա��ϵ��";
	}
	if (sasql_ckeckStrint(safepasswd) == FALSE)
	{
		printf("�쳣�ַ�%s\n", safepasswd);
		return "�޷����������뱾������Ա��ϵ��";
	}
	char sqlstr[256];
	sprintf(sqlstr, "select SafePasswd from `%s` where %s=BINARY'%s'", config.sql_Table, config.sql_NAME, id);

	if (!sasql_mysql_query(sqlstr))
	{
		int num_row = 0;
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);

		if (num_row > 0)
		{
			mysql_row = mysql_fetch_row(mysql_result);
			if (mysql_row[0] != NULL && strlen(mysql_row[0]) > 0)
			{
				if (strcmp(safepasswd, mysql_row[0]) == 0)
				{
					return "��ȫ���Ѿ��ɹ�������";
				}
				else
				{
					return "������󣬰�ȫ���޷��⿪��";
				}
			}
			else
			{
				return "����δ���ð�ȫ���������룬Ϊ��ȷ����ȫ��������һ����λ����������Ϊ��ȫ�����벢���ͼ�ס��";
			}
		}
	}
	return "�޷����������뱾������Ա��ϵ��";
}

char *sasql_ItemPetLocked_Passwd(char *id, char *safepasswd)
{
	if (sasql_ckeckStrint(id) == FALSE)
	{
		printf("�쳣�ַ�%s\n", id);
		return "��ȫ�����޸�ʧ�ܣ����뱾������Ա��ϵ��";
	}
	if (sasql_ckeckStrint(safepasswd) == FALSE)
	{
		printf("�쳣�ַ�%s\n", safepasswd);
		return "��ȫ�����޸�ʧ�ܣ����뱾������Ա��ϵ��";
	}
	char sqlstr[256];
	sprintf(sqlstr, "select SafePasswd from %s where %s=BINARY'%s'", config.sql_Table, config.sql_NAME, id);

	if (!sasql_mysql_query(sqlstr))
	{
		int num_row = 0;
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);
		if (num_row > 0)
		{
			mysql_row = mysql_fetch_row(mysql_result);
			if (mysql_row[0] != NULL && strlen(mysql_row[0]) > 0)
			{
				return "��ȫ�����Ѵ���,�޷��ٽ����޸ģ�";
			}
			else
			{
				char sqlstr[256];
				sprintf(sqlstr, "update %s set SafePasswd=BINARY'%s' where %s=BINARY'%s'", config.sql_Table, safepasswd, config.sql_NAME, id);

				if (!sasql_mysql_query(sqlstr))
				{
					return "��ȫ�����޸ĳɹ��������Ʊ�����İ�ȫ���룡";
				}
				return "��ȫ�����޸�ʧ�ܣ����뱾������Ա��ϵ��";
			}
		}
		else
		{
			return "�˺Ų����ڰ�ȫ�����Ѵ��ڣ�";
		}
	}
	else
	{
		printf("\n���ݿ����ʧ�ܣ�\n");
		printf("�����������ݿ�...");
		sasql_close();
		sasql_init();
		printf("���\n");
		return "��ȫ�����޸�ʧ�ܣ����뱾������Ա��ϵ��";
	}
}

int sasql_ItemPetLocked_Char(char *id, char *safepasswd)
{
	if (sasql_ckeckStrint(id) == FALSE)
	{
		printf("�쳣�ַ�%s\n", id);
		return 0;
	}
	if (sasql_ckeckStrint(safepasswd) == FALSE)
	{
		printf("�쳣�ַ�%s\n", safepasswd);
		return 0;
	}
	char sqlstr[256];
	sprintf(sqlstr, "select SafePasswd from %s where %s=BINARY'%s'", config.sql_Table, config.sql_NAME, id);

	if (!sasql_mysql_query(sqlstr))
	{
		int num_row = 0;
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);
		if (num_row > 0)
		{
			mysql_row = mysql_fetch_row(mysql_result);
			if (mysql_row[0] != NULL && strlen(mysql_row[0]) > 0)
			{
				if (strcmp(safepasswd, mysql_row[0]) == 0)
				{
					return 1;
				}
				else
				{
					printf("�û�%s��ȫ�������\n", id);
					return -1;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			printf("�û�%sδע�ᣡ\n", id);
			return -1;
		}
	}
	else
	{
		printf("\n���ݿ����ʧ�ܣ�\n");
		printf("�����������ݿ�...");
		sasql_close();
		sasql_init();
		printf("���\n");
		return -1;
	}
}
#endif

int sasql_onlinenum(char *MAC)
{
	char sqlstr[256];
	sprintf(sqlstr, "select count(*) from %s where Online=1 and MAC='%s'", config.sql_Table, MAC);
	if (!sasql_mysql_query(sqlstr))
	{
		int num_row = 0;
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);
		if (num_row > 0)
		{
			mysql_row = mysql_fetch_row(mysql_result);
			return atoi(mysql_row[0]);
		}
	}
	return 0;
}

#ifdef _FORMULATE_AUTO_PK
BOOL sasql_add_FormulateAutoPk(char *ID, int point)
{
	if (sasql_ckeckStrint(ID) == FALSE)
	{
		printf("�쳣�ַ�%s\n", ID);
		return -1;
	}

	char sqlstr[256];
	sprintf(sqlstr, "select PKPoint from %s where %s=BINARY'%s'", config.sql_Table, config.sql_NAME, ID);
	if (!sasql_mysql_query(sqlstr))
	{
		int num_row = 0;
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);
		if (num_row > 0)
		{
			mysql_row = mysql_fetch_row(mysql_result);
			point += atoi(mysql_row[0]);
			if (point < 0)
			{
				return -1;
			}
			sprintf(sqlstr, "update %s set PKPoint=%d where %s=BINARY'%s'", config.sql_Table, point, config.sql_NAME, ID);
			if (!sasql_mysql_query(sqlstr))
			{
				return point;
			}
		}
	}
	return -1;
}
#endif

#ifdef _OLDPS_TO_MD5PS
void sasql_OldpsToMd5ps()
{
	char sqlstr[256];
	sprintf(sqlstr, "select %s, %s from %s", config.sql_NAME, config.sql_PASS, config.sql_Table);
	if (!sasql_mysql_query(sqlstr))
	{
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		printf("      ת���˺�             ԭ����               ת��MD5��\n");
		while ((mysql_row = mysql_fetch_row(mysql_result)))
		{
			char *name = mysql_row[0];
			char *ps = mysql_row[1];
			if (strlen(ps) <= 16)
			{
				sprintf(sqlstr, "update %s set %s=BINARY'%s' where %s=BINARY'%s'", config.sql_Table, config.sql_PASS, MD5String(ps), config.sql_NAME, name);
				if (!sasql_mysql_query(sqlstr))
				{
					printf("%16s  %16s    %32s\n", name, ps, MD5String(ps));
				}
			}
		}
	}
}
#endif

#ifdef _CHARADATA_SAVE_SQL
void sasql_charadata_Save(char *id, char *table, char *data, int saveindex, int flag)
{
	if (strlen(data) == 0)
		return;

	char sqlstr[1024 * 16];

	if (flag == INSERT)
	{
		sprintf(sqlstr, "INSERT INTO %s VALUES %s", table, data);
	}
	else if (flag == SELECT)
	{
	}
	else if (flag == UPDATE)
	{
		sprintf(sqlstr, "UPDATE %s set %s WHERE CHAR_CDKEY=BINARY'%s' and CHAR_SAVEINDEX=%d", table, data, id, saveindex);
	}
	else if (flag == DELETE)
	{
		sprintf(sqlstr, "DELETE FROM %s WHERE CHAR_CDKEY=BINARY'%s' and CHAR_SAVEINDEX=%d", table, id, saveindex);
	}

	if (!mysql_query(&mysql, sqlstr))
	{
		if (flag == DELETE)
		{
			sasql_charadata_Save(id, table, data, saveindex, INSERT);
		}
		return;
	}
	else
	{
		printf(table);
		if (flag == INSERT)
		{
			printf(" INSERT error\n");
		}
		else if (flag == SELECT)
		{
			printf(" SELECT error\n");
		}
		else if (flag == UPDATE)
		{
			printf(" UPDATE error\n");
		}
		else if (flag == DELETE)
		{
			printf(" DELETE error\n");
		}
	}
}

#endif

void sasql_CleanCdkey(int date)
{
	char sqlstr[256];

	sprintf(sqlstr, "select `%s` from `%s` where TO_DAYS( NOW( ) ) - TO_DAYS( LoginTime ) > %d", config.sql_NAME, config.sql_Table, date);

	if (!mysql_query(&mysql, sqlstr))
	{
		int num_row = 0;
		int GroupNum = 0;
		int ServerMaxNum = 0;
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if (num_row > 0)
		{
			while ((mysql_row = mysql_fetch_row(mysql_result)))
			{
				char tmp[256] = "";
				char charId[256];

				strcpy(charId, mysql_row[0]);

				int hash = 0;
				int i;
				for (i = 0; i < strlen(charId); i++)
				{
					hash += (int)charId[i];
					hash = hash % 256;
				}

				sprintf(tmp, "rm char/0x%x/%s.* >& /dev/null\n", hash, charId);
				system(tmp);
				sprintf(tmp, "rm char_sleep/0x%x/%s.* >& /dev/null\n", hash, charId);
				system(tmp);
				sprintf(tmp, "rm log/0x%x/%s.* >& /dev/null\n", hash, charId);
				system(tmp);
				sprintf(tmp, "rm mail/0x%x/%s.* >& /dev/null\n", hash, charId);
				system(tmp);
			}
		}
	}

	sprintf(sqlstr, "DELETE FROM `%s` WHERE TO_DAYS( NOW( ) ) - TO_DAYS( LoginTime ) > %d", config.sql_Table, date);
	mysql_query(&mysql, sqlstr);
	printf("���\n");
	return;
}

void sasql_CleanLockCdkey()
{
	char sqlstr[256];

	sprintf(sqlstr, "select `%s` from `%s` where `%s` LIKE '%!%'", config.sql_NAME, config.sql_Table, config.sql_PASS);

	if (!mysql_query(&mysql, sqlstr))
	{
		int num_row = 0;
		int GroupNum = 0;
		int ServerMaxNum = 0;
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if (num_row > 0)
		{
			while ((mysql_row = mysql_fetch_row(mysql_result)))
			{
				char tmp[256] = "";
				char charId[256];

				strcpy(charId, mysql_row[0]);

				int hash = 0;
				int i;
				for (i = 0; i < strlen(charId); i++)
				{
					hash += (int)charId[i];
					hash = hash % 256;
				}

				sprintf(tmp, "rm char/0x%x/%s.* >& /dev/null\n", hash, charId);
				system(tmp);
				sprintf(tmp, "rm char_sleep/0x%x/%s.* >& /dev/null\n", hash, charId);
				system(tmp);
				sprintf(tmp, "rm log/0x%x/%s.* >& /dev/null\n", hash, charId);
				system(tmp);
				sprintf(tmp, "rm mail/0x%x/%s.* >& /dev/null\n", hash, charId);
				system(tmp);
			}
		}
	}

	sprintf(sqlstr, "DELETE FROM `%s` WHERE `%s` LIKE '%!%'", config.sql_Table, config.sql_PASS);
	mysql_query(&mysql, sqlstr);
	printf("���\n");
	return;
}

char *sasql_Mac_Char(char *id, int flg)
{
	if (sasql_ckeckStrint(id) == FALSE)
	{
		printf("�쳣�ַ�%s\n", id);
		return "error";
	}
	char sqlstr[256];
	if (flg == 0)
		sprintf(sqlstr, "select MAC1 from %s where %s=BINARY'%s'", config.sql_Table, config.sql_NAME, id);
	else
		sprintf(sqlstr, "select MAC2 from %s where %s=BINARY'%s'", config.sql_Table, config.sql_NAME, id);

	if (!sasql_mysql_query(sqlstr))
	{
		int num_row = 0;
		mysql_free_result(mysql_result);
		mysql_result = mysql_store_result(&mysql);
		num_row = mysql_num_rows(mysql_result);
		if (num_row > 0)
		{
			mysql_row = mysql_fetch_row(mysql_result);
			if (mysql_row[0] != NULL && strlen(mysql_row[0]) > 0)
			{
				return mysql_row[0];
			}
			else
			{
				return "error";
			}
		}
		else
		{
			return "error";
		}
	}
	else
	{
		printf("\n���ݿ����ʧ�ܣ�\n");
		printf("�����������ݿ�...");
		sasql_close();
		sasql_init();
		printf("���\n");
		return "error";
	}
}

BOOL sasql_craete_userinfo(void)
{
	return FALSE;
}

BOOL sasql_craete_lock(void)
{
	return FALSE;
}

#endif
