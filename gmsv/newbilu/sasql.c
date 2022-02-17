#include "NewBilu/version.h"

#ifdef _SASQL // New addition

#include "NewBilu/sasql.h"
#include "char_base.h"
#include "configfile.h"

#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
MYSQL mysql;
MYSQL_RES *mysql_result;
MYSQL_ROW mysql_row;
MYSQL_FIELD *fields;

typedef struct tagSQLConfig {
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
} SQLConfig;

SQLConfig sqlconfig;

int AutoReg;
int openbackground;

int getHash(char *s) {
  int i;
  int h = 0;
  for (i = 0;; i++) {
    if (s[i] == 0) {
      break;
    }
    h += s[i];
  }
  return h;
}

static int readSqlConfig(char *path) {
  char buf[255];
  FILE *fp;
  fp = fopen(path, "r");
  if (fp == NULL) {
    return -2;
  }
  while (fgets(buf, sizeof(buf), fp)) {
    char command[255];
    char param[255];
    chop(buf);
    easyGetTokenFromString(buf, 1, command, sizeof(command));
    easyGetTokenFromString(buf, 2, param, sizeof(param));
    if (strcmp(command, "sql_IP") == 0) {
      strcmp(sqlconfig.sql_IP, param);
      snprintf(sqlconfig.sql_IP, sizeof(sqlconfig.sql_IP), param);
      printf("\nDatabase address:  %s", sqlconfig.sql_IP);
    } else if (strcmp(command, "sql_Port") == 0) {
      sqlconfig.sql_Port = atoi(param);
      snprintf(sqlconfig.sql_Port1, sizeof(sqlconfig.sql_Port1), param);
      printf("\nDatabase port:  %d", sqlconfig.sql_Port);
    } else if (strcmp(command, "sql_ID") == 0) {
      strcmp(sqlconfig.sql_ID, param);
      snprintf(sqlconfig.sql_ID, sizeof(sqlconfig.sql_ID), param);
      printf("\nDatabase user:  %s", sqlconfig.sql_ID);
    } else if (strcmp(command, "sql_PS") == 0) {
      strcmp(sqlconfig.sql_PS, param);
      snprintf(sqlconfig.sql_PS, sizeof(sqlconfig.sql_PS), param);
      printf("\nDatabase pass code:  %s", sqlconfig.sql_PS);
    } else if (strcmp(command, "sql_DataBase") == 0) {
      strcmp(sqlconfig.sql_DataBase, param);
      snprintf(sqlconfig.sql_DataBase, sizeof(sqlconfig.sql_DataBase), param);
      printf("\nLog in the database name:%s", sqlconfig.sql_DataBase);
    } else if (strcmp(command, "sql_Table") == 0) {
      strcmp(sqlconfig.sql_Table, param);
      snprintf(sqlconfig.sql_Table, sizeof(sqlconfig.sql_Table), param);
      printf("\nUser information table name:  %s", sqlconfig.sql_Table);
    } else if (strcmp(command, "sql_LOCK") == 0) {
      snprintf(sqlconfig.sql_LOCK, sizeof(sqlconfig.sql_LOCK), param);
      printf("\nUser locks the table name:  %s", sqlconfig.sql_LOCK);
    } else if (strcmp(command, "sql_NAME") == 0) {
      strcmp(sqlconfig.sql_NAME, param);
      snprintf(sqlconfig.sql_NAME, sizeof(sqlconfig.sql_NAME), param);
      printf("\nAccount field name:  %s", sqlconfig.sql_NAME);
    } else if (strcmp(command, "sql_PASS") == 0) {
      strcmp(sqlconfig.sql_PASS, param);
      snprintf(sqlconfig.sql_PASS, sizeof(sqlconfig.sql_PASS), param);
      printf("\nPassword field name:  %s", sqlconfig.sql_PASS);
    } else if (strcmp(command, "openbackground") == 0) {
      sqlconfig.openbackground = atoi(param);
      printf("Backstage function: %s\n", sqlconfig.openbackground == 1 ? "YES" : "NO");
    } else if (strcmp(command, "AutoReg") == 0) {
      sqlconfig.AutoReg = atoi(param);
      printf("Open automatic reg: %s\n", sqlconfig.AutoReg == 1 ? "YES" : "NO");
    }
  }
  fclose(fp);
  return 0;
}

BOOL sasql_init(void) {
  if ((mysql_init(&mysql) == NULL) & readSqlConfig("sql.cf")) {
    printf("\nDatabase initialization failed!");
    exit(1);
    return FALSE;
  }
  if (!mysql_real_connect(&mysql, sqlconfig.sql_IP,
                          sqlconfig.sql_ID,       //??
                          sqlconfig.sql_PS,       //��?
                          sqlconfig.sql_DataBase, //??��?��?
                          sqlconfig.sql_Port, NULL, 0)) {
    printf("\nDatabase connection failed！\n");
    return FALSE;
  }

  mysql_query(&mysql, "set names 'euckr'");
  printf("\nDatabase connection is successful!\n");
  return TRUE;
}

void sasql_timeout() {
  int live = mysql_ping(&mysql);
  if (live) {
    printf("\nDatabase lookup failed!\n");
    printf("Reconnect the database ...");
    sasql_close();
    sasql_init();
  }
}

int sasql_mysql_query(char *sqlstr) {
  mysql_query(&mysql, "set names 'euckr'");
  int ret = mysql_query(&mysql, sqlstr);
  if (!ret) {
    return ret;
  }
  // sasql_timeout();
  return -1;
}

void sasql_close(void) { mysql_close(&mysql); }

BOOL sasql_ckeckStrint(char *str) {
  int i;
  for (i = 0; i < strlen(str); i++) {
    if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') &&
        (str[i] < '0' || str[i] > '9') && (str[i] != '.')) {
      return FALSE;
    }
  }
  return TRUE;
}

int sasql_query(char *nm, char *pas) {

  char sqlstr[1024];
  if (sasql_ckeckStrint(nm) == FALSE) {
    printf("User name of the exception character%s\n", nm);
    return 4;
  }

  // sprintf(sqlstr,"select `%s`, `salt` from %s where
  // %s=BINARY'%s'",config.sql_PASS, config.sql_Table,config.sql_NAME,nm);
  sprintf(sqlstr, "select `%s` from %s where %s=BINARY'%s'", sqlconfig.sql_PASS,
          sqlconfig.sql_Table, sqlconfig.sql_NAME, nm);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);

    if (num_row > 0) {
      char token[64];
      mysql_row = mysql_fetch_row(mysql_result);
      strcpy(token, mysql_row[0]);
#ifdef _OLDPS_TO_MD5PS
      if (strlen(mysql_row[0]) > 16 && strlen(pas) <= 16) {
        if (strcmp(MD5String(pas), token) == 0) {
          return 1;
        } else {
          printf("user %s wrong password!\n", nm);
          return 2;
        }
      } else if (strlen(pas) > 16 && strlen(mysql_row[0]) <= 16) {
        if (strcmp(MD5String(token), pas) == 0) {
          return 1;
        } else {
          printf("user %s wrong password!\n", nm);
          return 2;
        }
      } else {
        if (strcmp(pas, token) == 0) {
          return 1;
        } else {
          printf("user %s wrong password!\n", nm);
          return 2;
        }
      }
#else
      if (strcmp(pas, token) == 0) {
        return 1;
      } else {
        printf("user %s wrong password!\n", nm);
        return 2;
      }
#endif
    } else {
      printf("user %s Uncore book!\n", nm);
      return 3;
    }
  } else {
    sasql_timeout();
    printf("Finish\n");
    return 0;
  }
}

BOOL sasql_online(char *ID, char *NM, char *IP, char *MAC1, char *MAC2,
                  char *MAC3, int flag) {
  if (sqlconfig.openbackground == 1) {
    if (ID != NULL) {
      if (sasql_ckeckStrint(ID) == FALSE) {
        printf("User name of the exception character%s\n", ID);
        return TRUE;
      }
    }

    char sqlstr[256];
    memset(sqlstr, 0, sizeof(sqlstr));
    if (flag == 0)
      sprintf(sqlstr, "update %s set Online=0,Offline=0 where %s=BINARY'%s'",
              sqlconfig.sql_Table, sqlconfig.sql_NAME, ID);
    else if (flag == 1)
      sprintf(sqlstr,
              "update %s set LoginTime=NOW(), "
              "IP='%s',MAC1='%s',MAC2='%s',MAC3='%s', Online=%d where "
              "%s=BINARY'%s'",
              sqlconfig.sql_Table, IP, MAC1, MAC2, MAC3, getServernumber(),
              sqlconfig.sql_NAME, ID);
    else if (flag == 2)
      sprintf(
          sqlstr,
          "update %s set RightChar='%s', Path='char/0x%x' where %s=BINARY'%s'",
          sqlconfig.sql_Table, NM, getHash(ID) & 0xff, sqlconfig.sql_NAME, ID);
    else if (flag == 3)
      sprintf(sqlstr, "update %s set Online=0,Offline=0 where Online=%d",
              sqlconfig.sql_Table, getServernumber());
    else if (flag == 4)
      sprintf(
          sqlstr,
          "update %s set LeftChar='%s', Path='char/0x%x' where %s=BINARY'%s'",
          sqlconfig.sql_Table, NM, getHash(ID) & 0xff, sqlconfig.sql_NAME, ID);
    if (!sasql_mysql_query(sqlstr)) {
      return TRUE;
    }
    printf("\nUpdate database failed%s\n", sqlstr);
    sasql_timeout();
  }
  return TRUE;
}

int sasql_queryonline(char *ID) {
  if (sqlconfig.openbackground == 1) {
    if (ID != NULL) {
      if (sasql_ckeckStrint(ID) == FALSE) {
        printf("User name of the exception character%s\n", ID);
        return TRUE;
      }
    }

    char sqlstr[256];
    sprintf(sqlstr, "select Online from `%s` where %s=BINARY'%s'",
            sqlconfig.sql_Table, sqlconfig.sql_NAME, ID);

    if (!sasql_mysql_query(sqlstr)) {
      printf("\nNew user registration is successful!\n");
      int num_row = 0;
      mysql_free_result(mysql_result);
      mysql_result = mysql_store_result(&mysql);
      num_row = mysql_num_rows(mysql_result);
      if (num_row > 0) {
        mysql_row = mysql_fetch_row(mysql_result);

        return atoi(mysql_row[0]);
      }
    }
    printf("\nNew user registration failed!\n");
    sasql_timeout();
    return -1;
  }
  return -1;
}

BOOL sasql_register(char *id, char *ps) {
  char sqlstr[256];
  if (sqlconfig.AutoReg != 1)
    return FALSE;

  if (sasql_ckeckStrint(id) == FALSE) {
    printf("User name of the exception character %s\n", id);
    return FALSE;
  }
  if (sasql_ckeckStrint(ps) == FALSE) {
    printf("Abnormal characters game password %s\n", ps);
    return FALSE;
  }

#ifdef _OLDPS_TO_MD5PS
  sprintf(sqlstr,
          "INSERT INTO %s (%s,%s,RegTime,Path) VALUES "
          "(BINARY'%s','%s',NOW(),'char/0x%x')",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, sqlconfig.sql_PASS, id,
          MD5String(ps), getHash(id) & 0xff);
#else
  sprintf(sqlstr,
          "INSERT INTO %s (%s,%s,RegTime,Path) VALUES "
          "(BINARY'%s','%s',NOW(),'char/0x%x')",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, sqlconfig.sql_PASS, id, ps,
          getHash(id) & 0xff);
#endif

  if (!sasql_mysql_query(sqlstr)) {
    printf("\nNew user registration is successful!\n");
    return TRUE;
  }
  sasql_timeout();
  printf("\nNew User registration failed!\n");
  return FALSE;
}

BOOL sasql_chehk_lock(char *idip) {
  if (sasql_ckeckStrint(idip) == FALSE) {
    printf("Anomaly character%s\n", idip);
    return FALSE;
  }
  char sqlstr[256];
  sprintf(sqlstr, "select * from `%s` where Name='%s'", sqlconfig.sql_LOCK,
          idip);
  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);

    if (num_row > 0) {
      return TRUE;
    }
  }
  sasql_timeout();
  return FALSE;
}

BOOL sasql_add_lock(char *idip) {
  if (sasql_ckeckStrint(idip) == FALSE) {
    printf("Anomaly character %s\n", idip);
    return FALSE;
  }
  char sqlstr[256];
  sprintf(sqlstr, "INSERT INTO `%s` (Name) VALUES (BINARY'%s')",
          sqlconfig.sql_LOCK, idip);
  if (!sasql_mysql_query(sqlstr)) {
    printf("\nAdd lock %s success!\n", idip);
    return TRUE;
  }
  sasql_timeout();
  return FALSE;
}

BOOL sasql_del_lock(char *idip) {
  if (sasql_ckeckStrint(idip) == FALSE) {
    printf("Anomaly character %s\n", idip);
    return FALSE;
  }
  char sqlstr[256];
  sprintf(sqlstr, "delete from `%s` where Name=BINARY'%s'", sqlconfig.sql_LOCK,
          idip);
  if (!sasql_mysql_query(sqlstr)) {
    printf("\nunlock %s success!\n", idip);
    return TRUE;
  }
  sasql_timeout();
  return FALSE;
}

#ifdef _ITEM_PET_LOCKED
char *sasql_ItemPetLocked(char *id, char *safepasswd) {
  if (sasql_ckeckStrint(id) == FALSE) {
    printf("Anomaly character %s\n", id);
    return "잠금 해제,이 서비스 관리자에게 문의하십시오!";
  }
  if (sasql_ckeckStrint(safepasswd) == FALSE) {
    printf("Anomaly character %s\n", safepasswd);
    return "잠금 해제,이 서비스 관리자에게 문의하십시오!";
  }
  char sqlstr[256];
  sprintf(sqlstr, "select SafePasswd from `%s` where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);

    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      if (mysql_row[0] != NULL && strlen(mysql_row[0]) > 0) {
        if (strcmp(safepasswd, mysql_row[0]) == 0) {
          return "보안 잠금 장치가 성공적으로 잠금 해제되었습니다!";
        } else {
          return "암호가 올바르지 않으면 보안 잠금이 말할 수 없을 수 없습니다!";
        }
      } else {
        return "안전을 보장하기 위해 안전한 잠금 코드를 설정하지 않았습니다. 6개 이상의 암호를 입력하십시오."
               "보안 잠금 암호가되어 기억하십시오!";
      }
    }
  }
  sasql_timeout();
  return "잠금 해제,이 서비스 관리자에게 문의하십시오!";
}

char *sasql_ItemPetLocked_Passwd(char *id, char *safepasswd) {
  if (sasql_ckeckStrint(id) == FALSE) {
    printf("anomaly 캐릭터%s\n", id);
    return "비밀번호 수정 실패,이 서비스 관리자에게 문의하십시오!";
  }
  if (sasql_ckeckStrint(safepasswd) == FALSE) {
    printf("anomaly 캐릭터%s\n", safepasswd);
    return "비밀번호 수정 실패,이 서비스 관리자에게 문의하십시오!";
  }
  char sqlstr[256];
  sprintf(sqlstr, "select SafePasswd from %s where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      if (mysql_row[0] != NULL && strlen(mysql_row[0]) > 0) {
        return "보안 암호가 이미 있고 수정할 수 없습니다!";
      } else {
        char sqlstr[256];
        sprintf(sqlstr,
                "update %s set SafePasswd=BINARY'%s' where %s=BINARY'%s'",
                sqlconfig.sql_Table, safepasswd, sqlconfig.sql_NAME, id);

        if (!sasql_mysql_query(sqlstr)) {
          return "보안 암호가 성공적으로 수정되면 보안 암호를 안전하게 "
                 "보관하십시오!";
        }
        return "비밀번호 수정 실패,이 서비스 관리자에게 문의하십시오!";
      }
    } else {
      return "보안 암호가 이미 존재하는 계정이 존재하지 않습니다!";
    }
  } else {
    sasql_timeout();
    printf("完成\n");
    return "비밀번호 수정 실패,이 서비스 관리자에게 문의하십시오!";
  }
}

int sasql_ItemPetLocked_Char(char *id, char *safepasswd) {
  if (sasql_ckeckStrint(id) == FALSE) {
    printf("異常字符%s\n", id);
    return 0;
  }
  if (sasql_ckeckStrint(safepasswd) == FALSE) {
    printf("異常字符%s\n", safepasswd);
    return 0;
  }
  char sqlstr[256];
  sprintf(sqlstr, "select SafePasswd from %s where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      if (mysql_row[0] != NULL && strlen(mysql_row[0]) > 0) {
        if (strcmp(safepasswd, mysql_row[0]) == 0) {
          return 1;
        } else {
          printf("用戶%s安全密碼錯誤！\n", id);
          return -1;
        }
      } else {
        return 0;
      }
    } else {
      printf("用戶%s未注冊！\n", id);
      return -1;
    }
  } else {
    sasql_timeout();
    printf("完成\n");
    return -1;
  }
}
#endif

int sasql_getVipPoint(int charaindex) {
  char sqlstr[256];
  char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
  sprintf(sqlstr, "select VipPoint from `%s` where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      return atoi(mysql_row[0]);
    }
  } else {
    sasql_timeout();
  }
  return -1;
}

int sasql_setVipPoint(int charaindex, int point) {
  char sqlstr[256];
  char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
  sprintf(sqlstr, "update %s set VipPoint=%d where %s=BINARY'%s'",
          sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    // printf("��?%s?����?%d��\n",id,point);
    return point;
  }
  sasql_timeout();
  return -1;
}

int sasql_getVipPointForCdkey(char *id) {
  if (sasql_ckeckStrint(id) == FALSE) {
    // printf("?����ݬ����?٣%s\n",id);
    return -1;
  }

  char sqlstr[256];
  sprintf(sqlstr, "select VipPoint from `%s` where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);

      return atoi(mysql_row[0]);
    }
  }
  sasql_timeout();
  return -1;
}

int sasql_setVipPointForCdkey(char *id, int point) {
  if (sasql_ckeckStrint(id) == FALSE) {
    // printf("?����ݬ����?٣%s\n",id);
    return -1;
  }

  char sqlstr[256];
  sprintf(sqlstr, "update %s set VipPoint=%d where %s=BINARY'%s'",
          sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    // printf("��?%s?����?%d��\n",id,point);
    return point;
  }
  sasql_timeout();
  return -1;
}

int sasql_getPayPoint(int charaindex) {
  char sqlstr[256];
  char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
  sprintf(sqlstr, "select PayPoint from `%s` where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);

      return atoi(mysql_row[0]);
    }
  }
  sasql_timeout();
  return -1;
}

int sasql_setPayPoint(int charaindex, int point) {
  char sqlstr[256];
  char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
  sprintf(sqlstr, "update %s set PayPoint=%d where %s=BINARY'%s'",
          sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    printf("用戶%s當前充值點%d！\n", id, point);
    return point;
  }
  sasql_timeout();
  return -1;
}

int sasql_getPayPointForCdkey(char *id) {
  if (sasql_ckeckStrint(id) == FALSE) {
    printf("異常字符%s\n", id);
    return -1;
  }

  char sqlstr[256];
  sprintf(sqlstr, "select PayPoint from `%s` where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);

      return atoi(mysql_row[0]);
    }
  }
  sasql_timeout();
  return -1;
}

int sasql_setPayPointForCdkey(char *id, int point) {
  if (sasql_ckeckStrint(id) == FALSE) {
    printf("異常字符%s\n", id);
    return -1;
  }

  char sqlstr[256];
  sprintf(sqlstr, "update %s set PayPoint=%d where %s=BINARY'%s'",
          sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    printf("用戶%s當前充值點%d！\n", id, point);
    return point;
  }
  sasql_timeout();
  return -1;
}

int sasql_getRmbPoint(int charaindex) {
  char sqlstr[256];
  char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
  sprintf(sqlstr, "select RmbPoint from `%s` where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);

      return atoi(mysql_row[0]);
    }
  }

  sasql_timeout();
  return -1;
}

int sasql_setRmbPoint(int charaindex, int point) {
  char sqlstr[256];
  char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
  sprintf(sqlstr, "update %s set RmbPoint=%d where %s=BINARY'%s'",
          sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    return point;
  }
  sasql_timeout();
  return -1;
}

int sasql_getRmbPointForCdkey(char *id) {
  if (sasql_ckeckStrint(id) == FALSE) {
    printf("異常字符%s\n", id);
    return -1;
  }

  char sqlstr[256];
  sprintf(sqlstr, "select RmbPoint from `%s` where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);

      return atoi(mysql_row[0]);
    }
  }
  sasql_timeout();
  return -1;
}

int sasql_setRmbPointForCdkey(char *id, int point) {
  if (sasql_ckeckStrint(id) == FALSE) {
    printf("異常字符%s\n", id);
    return -1;
  }
  char sqlstr[256];
  sprintf(sqlstr, "update %s set RmbPoint=%d where %s=BINARY'%s'",
          sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);
  if (!sasql_mysql_query(sqlstr)) {
    return point;
  }
  sasql_timeout();
  return -1;
}

#ifdef _PAUCTION_MAN
BOOL sasql_addPauctionInfo(char *cdkey, char *name, char *effect, int cost,
                           int type, char *info, char *string) {
  char sqlstr[1024];
  sprintf(sqlstr,
          "INSERT INTO `PauctionInfo` ( `cdkey` , `name` , `effect` , `cost` , "
          "`type` , `info` , `string` , `day` ) "
          "VALUES ( '%s', '%s', '%s', %d, %d, '%s', '%s', NOW())",
          cdkey, name, effect, cost, type, info, string);

  if (!sasql_mysql_query(sqlstr)) {
    printf("sasql_addPauctionInfo SQL error\n");
    return TRUE;
  }
  return FALSE;
}

int sasql_getPauctionList(char *List, int start, int type, char *cdkey) {
  char sqlstr[1024];
  char tmp[256];
  char buf[256];
  int num_row = 0;
  if (type == 0) {
    if (strlen(cdkey) > 0) {
      sprintf(sqlstr,
              "select `id` , `cdkey` , `effect` from `PauctionInfo` where "
              "`cdkey` = BINARY'%s'",
              cdkey);
    } else {
      sprintf(sqlstr, "select `id` , `cdkey` , `effect` from `PauctionInfo` "
                      "where TO_DAYS( NOW( ) ) - TO_DAYS( day ) < 30");
    }
  } else {
    sprintf(sqlstr,
            "select `id` , `cdkey` , `effect` from `PauctionInfo` where `type` "
            "= %d AND TO_DAYS( NOW( ) ) - TO_DAYS( day ) < 30",
            type);
  }
  if (!sasql_mysql_query(sqlstr)) {
    int line = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);

    if (num_row > 0) {
      while ((mysql_row = mysql_fetch_row(mysql_result))) {
        if (line >= start) {
          sprintf(tmp, "%s|%s|%s", mysql_row[0], mysql_row[1], mysql_row[2]);
          strcat(List, makeEscapeString(tmp, buf, sizeof(buf)));
          strcat(List, "|");

          if (line > start + 10)
            break;
        }
        line++;
      }
    }

    return num_row;
  }

  return 0;
}

BOOL sasql_getPauctionSurvey(char *token, int id) {
  char sqlstr[1024];
  char buf[1024];
  sprintf(sqlstr,
          "select `id` , `name` , `effect`, `cost`, `type`, `info` from "
          "`PauctionInfo` where `id` = %d AND TO_DAYS( NOW( ) ) - TO_DAYS( day "
          ") < 30",
          id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      sprintf(token, "%s|%s|%s|%s|%s|%s", mysql_row[0], mysql_row[1],
              mysql_row[2], mysql_row[3], mysql_row[4],
              makeEscapeString(mysql_row[5], buf, sizeof(buf)));
    }

    return TRUE;
  }

  return FALSE;
}

int sasql_getMyPauction(char *token, int id, char *cdkey) {
  char sqlstr[1024];

  if (strlen(cdkey) > 0) {
    sprintf(sqlstr,
            "select `string` ,`type` from `PauctionInfo` where `id` = %d AND "
            "`cdkey` = BINARY'%s'",
            id, cdkey);
  } else {
    sprintf(sqlstr,
            "select `string` ,`type` from `PauctionInfo` where `id` = %d ", id);
  }
  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);

    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      sprintf(token, "%s", mysql_row[0]);
    }

    return atoi(mysql_row[1]);
  }

  return -1;
}

int sasql_PauctionOK(int id, char *cdkey) {
  char sqlstr[1024];
  sprintf(sqlstr, "select `cdkey`, `cost` from `PauctionInfo` where `id` = %d",
          id);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      char tocdkey[64];
      sprintf(tocdkey, "%s", mysql_row[0]);
      int cost = atoi(mysql_row[1]);
      sprintf(sqlstr,
              "select * from `%s` where %s = BINARY'%s' AND VipPoint>%d",
              sqlconfig.sql_Table, sqlconfig.sql_NAME, cdkey, cost);

      if (!sasql_mysql_query(sqlstr)) {
        int num_row = 0;
        mysql_free_result(mysql_result);
        mysql_result = mysql_store_result(&mysql);
        num_row = mysql_num_rows(mysql_result);

        if (num_row > 0) {
          sprintf(sqlstr,
                  "update %s set VipPoint=VipPoint + %d where %s=BINARY'%s' ",
                  sqlconfig.sql_Table, cost, sqlconfig.sql_NAME, tocdkey);
          if (!sasql_mysql_query(sqlstr)) {
            sprintf(sqlstr,
                    "update %s set VipPoint=VipPoint - %d where %s=BINARY'%s' ",
                    sqlconfig.sql_Table, cost, sqlconfig.sql_NAME, cdkey);
            if (!sasql_mysql_query(sqlstr)) {
              return 1;
            }
          }
        } else {
          return -2;
        }
      }
    } else {
      return -1;
    }
  }
  return 0;
}

BOOL sasql_delPauctionBuy(int id, char *cdkey) {
  char sqlstr[1024];

  if (strlen(cdkey) > 0) {
    sprintf(
        sqlstr,
        "delete from `PauctionInfo` where `id` = %d AND `cdkey` = BINARY'%s'",
        id, cdkey);
  } else {
    sprintf(sqlstr, "delete from `PauctionInfo` where `id` = %d", id);
  }

  if (!sasql_mysql_query(sqlstr)) {
    return TRUE;
  }

  return FALSE;
}
#endif

#endif

void sasql_updateCharLockMac(int charaindex, char *cdkey) {
  char sqlstr[1024];
  sprintf(sqlstr,
          "SELECT `newsafe`, `flg` FROM `safedata` WHERE `cdkey` = '%s'",
          cdkey);
  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      if (strcmp(mysql_row[0], CHAR_getWorkChar(charaindex, CHAR_WORK_MAC2)) !=
          0) {
        sprintf(sqlstr,
                "UPDATE `safedata` SET `newsafe`= '%s',`oldsafe` = "
                "'%s',`FixTime` = NOW() where `cdkey` = '%s'",
                CHAR_getWorkChar(charaindex, CHAR_WORK_MAC2), mysql_row[0],
                cdkey);
        sasql_mysql_query(sqlstr);
        return;
      }
    } else {
      sprintf(sqlstr,
              "INSERT INTO `safedata` "
              "(`cdkey`,`newsafe`,`oldsafe`,`flg`,`FixTime`) VALUES "
              "('%s','%s','%s',0,NOW()",
              cdkey, CHAR_getWorkChar(charaindex, CHAR_WORK_MAC2),
              CHAR_getWorkChar(charaindex, CHAR_WORK_MAC2));
      sasql_mysql_query(sqlstr);
      return;
    }
  }
}

void sasql_updateFmPointData(int fmid, int fmtime, int fmnum, int flg) {
  char sqlstr[1024];
  if (flg == 0)
    sprintf(sqlstr,
            "UPDATE `fmpointdata` SET `time`= %d,`num` = %d where `id` = %d",
            fmtime, fmnum, fmid);
  else if (flg == 1)
    sprintf(sqlstr, "UPDATE `fmpointdata` SET `num` = %d where `id` = %d",
            fmnum, fmid);
  else
    return;
  sasql_mysql_query(sqlstr);
}

int sasql_queryFmPointData(int fmid, int flg) {
  char sqlstr[1024];
  if (flg == 0)
    sprintf(sqlstr, "SELECT `time` FROM `fmpointdata` WHERE `id` = %d", fmid);
  else if (flg == 1)
    sprintf(sqlstr, "SELECT `num` FROM `fmpointdata` WHERE `id` = %d", fmid);
  else
    return -1;
  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      return atoi(mysql_row[0]);
    } else {
      return -1;
    }
  }
  return -1;
}

//���������?��
int sasql_getDateNum(char *tablename) {
  char sqlstr[256];
  sprintf(sqlstr, "select * from %s where CF=1", tablename);
  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      // mysql_row=mysql_fetch_row(mysql_result);
      return num_row;
    }
  }
  return 0;
}

#ifdef _NB_SQL_Enemy
char *sasql_getEnemyData(int limit) {
  char sqlstr[256];
  char enemydata[2048];
  sprintf(sqlstr, "select * from Data_enemy where CF=1 limit %d,1", limit);
  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    int tablenum = mysql_num_fields(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      strcpy(enemydata, "");
      int i = 0;
      for (i = 2; i < tablenum + 1; i++) {
        if (mysql_row[i] == NULL) { //����?������NULL?��??��???����ݬ
          strcat(enemydata, "");
          strcat(enemydata, ",");
        } else {
          strcat(enemydata, mysql_row[i]);
          strcat(enemydata, ",");
        }
      }
      return enemydata;
    }
  }
  return "";
}
#endif

#ifdef _NB_SQL_EnemyBase
char *sasql_getEnemybaseData(int limit) {
  char sqlstr[256];
  char enemydata[2048];
  sprintf(sqlstr, "select * from Data_enemybase where CF=1 limit %d,1", limit);
  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    int tablenum = mysql_num_fields(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      strcpy(enemydata, "");
      int i = 0;
      for (i = 2; i < tablenum + 1; i++) {
        if (mysql_row[i] == NULL) { //����?������NULL?��??��???����ݬ
          strcat(enemydata, "");
          strcat(enemydata, ",");
        } else {
          strcat(enemydata, mysql_row[i]);
          strcat(enemydata, ",");
        }
      }
      return enemydata;
    }
  }
  return "";
}
#endif

#ifdef _NB_CDKEYLOCK
int sasql_getLock(char *cdkey) {
  char sqlstr[256];
  sprintf(sqlstr, "select `Lock` from `%s` where %s=BINARY'%s'",
          sqlconfig.sql_Table, sqlconfig.sql_NAME, cdkey);

  if (!sasql_mysql_query(sqlstr)) {
    int num_row = 0;
    mysql_free_result(mysql_result);
    mysql_result = mysql_store_result(&mysql);
    num_row = mysql_num_rows(mysql_result);
    if (num_row > 0) {
      mysql_row = mysql_fetch_row(mysql_result);
      // print("Ret=%d\n",atoi(mysql_row[0]));
      return atoi(mysql_row[0]);
    }
  } else {
    sasql_timeout();
  }
  return -1;
}

int sasql_setLock(char *cdkey, int lock) {
  char sqlstr[256];
  sprintf(sqlstr, "update %s set `Lock`=%d where %s=BINARY'%s'",
          sqlconfig.sql_Table, lock, sqlconfig.sql_NAME, cdkey);

  if (!sasql_mysql_query(sqlstr)) {
    // printf("��?%s?����?%d��\n",id,point);
    return lock;
  }
  sasql_timeout();
  return -1;
}

#endif

char SQL_StringBuffer[1024 * 100];

void SQL_UpdatePetRank(int petindex) {
  print("\n\nSQL_UpdatePetRank\n\n");
  if (CHAR_CHECKINDEX(petindex) == FALSE)
    return;

  int Level = CHAR_getInt(petindex, CHAR_LV);

  int Stats_MaxHp = CHAR_getWorkInt(petindex, CHAR_WORKMAXHP);
  int Stats_Atk = CHAR_getWorkInt(petindex, CHAR_WORKATTACKPOWER);
  int Stats_Def = CHAR_getWorkInt(petindex, CHAR_WORKDEFENCEPOWER);
  int Stats_Spd = CHAR_getWorkInt(petindex, CHAR_WORKQUICK);
  int Stats_Total = Stats_MaxHp + Stats_Atk + Stats_Def + Stats_Spd;

  double Growth_HP = 0.0f;
  double Growth_ATK = 0.0f;
  double Growth_DEF = 0.0f;
  double Growth_SPD = 0.0f;
  double Growth_TOTAL = 0.0f;
  double basevit, basestr, basetgh, basedex, baseall,
      baseplus; // S�� ������ ���̽�
  double firstvit, firststr, firsttgh, firstdex; // S�� �ʱ�ġ

  if (Level > 1) // Avoiding floating point calculation
  {
    int Base_VIT = CHAR_getInt(petindex, CHAR_PETDATAH); // storage initials hp
    int Base_STR = CHAR_getInt(petindex, CHAR_PETDATAS); // storage initials str
    int Base_TGH = CHAR_getInt(petindex, CHAR_PETDATAT); // storage initials tgh
    int Base_DEX = CHAR_getInt(petindex, CHAR_PETDATAD); // storage initials dex

    basevit = (double)(CHAR_getInt(petindex, CHAR_DATAVIT) +
                       4.5); // S�� ������ ���̽� ������
    basestr = (double)(CHAR_getInt(petindex, CHAR_DATASTR) +
                       4.5); // S�� ������ ���̽� ���ݷ�
    basetgh = (double)(CHAR_getInt(petindex, CHAR_DATATGH) +
                       4.5); // S�� ������ ���̽� ����
    basedex = (double)(CHAR_getInt(petindex, CHAR_DATADEX) +
                       4.5); // S�� ������ ���̽� ���߷�
    baseall = (double)(CHAR_getInt(petindex,
                                   CHAR_DATAALL)); // S�� ������ ���̽� Initnum
    baseplus = (CHAR_getInt(petindex, CHAR_DATAVIT) +
                CHAR_getInt(petindex, CHAR_DATASTR) +
                CHAR_getInt(petindex, CHAR_DATATGH) +
                CHAR_getInt(petindex, CHAR_DATADEX)); // ���̽� ��

    firstvit =
        (double)((basevit * 4 + basestr + basetgh + basedex) * baseall / 100);
    firststr =
        (double)((basevit * 0.1 + basestr + basetgh * 0.1 + basedex * 0.05) *
                 baseall / 100);
    firsttgh =
        (double)((basevit * 0.1 + basestr * 0.1 + basetgh + basedex * 0.05) *
                 baseall / 100);
    firstdex = (double)(basedex * baseall / 100);

    // hp current growth value
    Growth_HP = Stats_MaxHp - (int)firstvit;
    Growth_HP = Growth_HP / (Level - 1);

    // str current growth value
    Growth_ATK = Stats_Atk - (int)firststr;
    Growth_ATK = Growth_ATK / (Level - 1);

    // tgh current growth value
    Growth_DEF = Stats_Def - (int)firsttgh;
    Growth_DEF = Growth_DEF / (Level - 1);

    Growth_SPD = Stats_Spd - (int)firstdex;
    Growth_SPD = Growth_SPD / (Level - 1);

    Growth_TOTAL = Growth_ATK + Growth_DEF + Growth_SPD;
  }

  char SQL_Query[1024];
  /*
  `sp_PetRank`(
          IN p_UID VARCHAR( 255 ),
          IN P_OWNER VARCHAR( 33 ),
          IN p_EBID INT( 11 ),
          IN p_EID INT( 11 ),
          IN p_LV INT( 11 ),
          IN p_RB INT( 11 ),
          IN p_HP INT( 11 ),
          IN p_ATK INT( 11 ),
          IN p_DEF INT( 11 ),
          IN p_SPD INT( 11 ),
          IN p_TOTAL INT( 11 ),
          IN p_GHP FLOAT,
          IN p_GATK FLOAT,
          IN p_GDEF FLOAT,
          IN p_GSPD FLOAT,
          IN p_GTOTAL FLOAT
          )
          */
  sprintf(
      SQL_Query,
      "CALL sp_PetRank('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f)",
      CHAR_getChar(petindex, CHAR_UNIQUECODE),
      CHAR_getChar(petindex, CHAR_OWNERCHARANAME),
      CHAR_getInt(petindex, CHAR_PETID), -1, Level,
      CHAR_getInt(petindex, CHAR_TRANSMIGRATION), Stats_MaxHp, Stats_Atk,
      Stats_Def, Stats_Spd, Stats_Total, Growth_HP, Growth_ATK, Growth_DEF,
      Growth_SPD, Growth_TOTAL);
  print("\n\n%s\n\n", SQL_Query);
  mysql_query(&mysql, SQL_Query);
}

void SQL_DeletePetInRank(int petindex) {
  if (CHAR_CHECKINDEX(petindex) == FALSE)
    return;

  char SQL_Query[1024];

  sprintf(SQL_Query, "DELETE FROM `PET_Rank` WHERE `id` = BINARY'%s'",
          CHAR_getChar(petindex, CHAR_UNIQUECODE));
  mysql_query(&mysql, SQL_Query);
}

char *SQL_GetTopPetRank(int enemybaseid) {
  SQL_StringBuffer[0] = 0;

  char SQL_Query[1024];
  char SQL_Work[1024];

  sprintf(SQL_Query,
          "SELECT"
          " `owner_name`,"
          " `level`,"
          " `stats_hp`,"
          " `stats_atk`,"
          " `stats_def`,"
          " `stats_spd`,"
          " `stats_total`,"
          " `growth_hp`,"
          " `growth_atk`,"
          " `growth_def`,"
          " `growth_spd`,"
          " `growth_total`"
          " FROM `PET_Rank`"
          " WHERE `enemybase_id` = %d"
          " ORDER BY `stats_total` DESC, `growth_total` DESC, `updated_time` "
          "ASC LIMIT 3",
          enemybaseid);

  if (mysql_query(&mysql, SQL_Query) != 0)
    return "\0";

  mysql_free_result(mysql_result);
  mysql_result = mysql_store_result(&mysql);

  if (mysql_num_rows(mysql_result) == 0)
    return "\0";

  while ((mysql_row = mysql_fetch_row(mysql_result))) {
    sprintf(SQL_Work, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|", mysql_row[0],
            mysql_row[1], mysql_row[2], mysql_row[3], mysql_row[4],
            mysql_row[5], mysql_row[6], mysql_row[7], mysql_row[8],
            mysql_row[9], mysql_row[10], mysql_row[11]);

    makeEscapeString(SQL_Work, SQL_Query, sizeof(SQL_Query));
    strcat(SQL_StringBuffer, SQL_Query);
    strcat(SQL_StringBuffer, "|");
  }

  return SQL_StringBuffer;
}