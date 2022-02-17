#define _UTIL_C_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>

#include "main.h"
#include "util.h"

#define min(x, y) (                     \
    {                                   \
        typeof(x) __x = (x), __y = (y); \
        (__x < __y) ? __x : __y;        \
    })
#define max(x, y) (                     \
    {                                   \
        typeof(x) __x = (x), __y = (y); \
        (__x < __y) ? __y : __x;        \
    })
#define IS_2BYTEWORD(_a_) ((char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF))
#define PRIME 211
int hashpjw(char *s)
{
    char *p;
    unsigned int h = 0, g;
    for (p = s; *p; p++)
    {
        h = (h << 4) + (*p);
        if ((g = h & 0xf0000000) != 0)
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % PRIME;
}

int getHash(char *s)
{
    int i;
    int h = 0;

    for (i = 0;; i++)
    {
        if (s[i] == 0)
        {
            break;
        }
        h += s[i];
    }
    return h;
}

void easyGetTokenFromString(char *src, int count, char *output, int len)
{
    int i;
    int counter = 0;

    if (len <= 0)
        return;

#define ISSPACETAB(c) ((c) == ' ' || (c) == '\t')

    for (i = 0;; i++)
    {
        if (src[i] == '\0')
        {
            output[0] = '\0';
            return;
        }
        if (i > 0 && !ISSPACETAB(src[i - 1]) &&
            !ISSPACETAB(src[i]))
        {
            continue;
        }

        if (!ISSPACETAB(src[i]))
        {
            counter++;
            if (counter == count)
            {
                int j;
                for (j = 0; j < len - 1; j++)
                {
                    if (src[i + j] == '\0' ||
                        ISSPACETAB(src[i + j]))
                    {
                        break;
                    }
                    output[j] = src[i + j];
                }
                output[j] = '\0';
                return;
            }
        }
    }
}

void prepareDirectories(char *base)
{
    int i;
    char dname[1024];

    for (i = 0; i < 256; i++)
    {
        int ret;
        snprintf(dname, sizeof(dname), "%s/0x%x", base, i);
        ret = mkdir(dname, 0755);
        if (ret < 0 && errno != EEXIST)
        {
            log("mkdir error:%d %s: %s\n", ret, strerror(errno), dname);
        }
        if (ret == 0)
            log(".");
    }
}

void remove_r(char *s)
{
    int i;
    for (i = 0;; i++)
    {
        if (s[i] == '\r')
            s[i] = 0;
        if (s[i] == 0)
            break;
    }
}

void makeDirFilename(char *out, int outlen,
                     char *base, int dirchar, char *child)
{
    snprintf(out, outlen,
             "%s/0x%x/%s", base,
             dirchar & 0xff, child);
}

int isFile(char *fn)
{
    FILE *fp = fopen(fn, "r");
    if (fp)
    {
        fclose(fp);
        return 1;
    }
    return 0;
}

int createFile(char *fn, char *line)
{
    FILE *fp = fopen(fn, "w");
    if (fp == NULL)
    {
        return -1;
    }
    else
    {
        fprintf(fp, "%s", line);
        fclose(fp);
        return 0;
    }
}

#define BOOL int
#define FALSE 0
#define TRUE 1

typedef struct tagEscapeChar
{
    char escapechar;
    char escapedchar;
} EscapeChar;
static EscapeChar escapeChar[] =
    {
        {'\n', 'n'},
        {',', 'c'},
        {'|', 'z'},
        {'\\', 'y'},
};

char *makeStringFromEscaped(char *src)
{
    int i;
    int srclen = strlen(src);
    int searchindex = 0;

    for (i = 0; i < srclen; i++)
    {
        // for 2Byte Word
        if (IS_2BYTEWORD(src[i]))
        {
            src[searchindex++] = src[i++];
            src[searchindex++] = src[i];
        }
        else
        {
            if (src[i] == '\\')
            {
                int j;

                /*  �ݼ�  ٯ�����  */
                i++;
                for (j = 0; j < sizeof(escapeChar) / sizeof(escapeChar[0]); j++)
                {
                    if (escapeChar[j].escapedchar == src[i])
                    {
                        /*  �ݻ��޵�������ƽ�ҷ·�  */
                        src[searchindex++] = escapeChar[j].escapechar;
                        break;
                    }
                }
                /*  �޷¡�֧������������������ƻ�����  */
                if (escapeChar[j].escapedchar != src[i])
                    src[searchindex++] = src[i];
            }
            else
            {
                src[searchindex++] = src[i];
            }
        }
    }
    src[searchindex] = '\0';
    return src;
}

char *makeEscapeString(char *src, char *dest, int sizeofdest)
{
    int i;
    int srclen = strlen(src);
    int destindex = 0;

    for (i = 0; i < srclen; i++)
    {
        BOOL dirty = FALSE;
        int j;
        char escapechar = '\0';
        if (destindex + 1 >= sizeofdest)
            break;
        if (IS_2BYTEWORD(src[i]))
        {
            if (destindex + 2 >= sizeofdest)
                break;
            dest[destindex] = src[i];
            dest[destindex + 1] = src[i + 1];
            destindex += 2;
            i++;
            continue;
        }

        for (j = 0; j < sizeof(escapeChar) / sizeof(escapeChar[0]); j++)
        {
            if (src[i] == escapeChar[j].escapechar)
            {
                dirty = TRUE;
                escapechar = escapeChar[j].escapedchar;
                break;
            }
        }
        if (dirty == TRUE)
        {
            if (destindex + 2 < sizeofdest)
            {
                dest[destindex] = '\\';
                dest[destindex + 1] = escapechar;
                destindex += 2;
                dirty = TRUE;
                continue; /*  �ݼ�  ٯ���׸�  */
            }
            else
            {
                dest[destindex] = '\0';
                return dest;
            }
        }
        else
        {
            dest[destindex] = src[i];
            destindex++;
        }
    }

    dest[destindex] = '\0';
    return dest;
}

char *makeEscapeString1(char *src, char *dest, int sizeofdest)
{
    int i;
    int srclen = strlen(src);
    int destindex = 0;

    for (i = 0; i < srclen; i++)
    {
        BOOL dirty = FALSE;
        int j;
        char escapechar = '\0';
        if (destindex + 1 >= sizeofdest)
            /*  '\0'�л���Իئ�м�ƥ���ƥ��Ի   */
            break;

        /*	// ��ʣ����������  ��������ë��������
		if( IS_2BYTEWORD( src[i] ) ){
			// ��ʣ�֣۹��������������������б��������£�
			// �׷��ƨ��������ƾ�ئ��������������ئ��
			// ������������������˪
	        if( destindex + 2 >= sizeofdest )break;

            dest[destindex] = src[i];
            dest[destindex+1] = src[i+1];
   	        destindex += 2;
   	        i ++;
   	        continue;
		}
	*/
        for (j = 0; j < sizeof(escapeChar) / sizeof(escapeChar[0]); j++)
            if (src[i] == escapeChar[j].escapechar)
            {
                dirty = TRUE;
                escapechar = escapeChar[j].escapedchar;
                break;
            }

        if (dirty == TRUE)
        {
            /*  �޵�����������  ٯ��    */
            if (destindex + 2 < sizeofdest)
            {
                /*  +2 ���е������� '\\' �� 'n'���Ƿ�   */
                /*  ��Ի  ٯ�����з�    */
                dest[destindex] = '\\';
                dest[destindex + 1] = escapechar;
                destindex += 2;
                dirty = TRUE;
                continue; /*  �ݼ�  ٯ���׸�  */
            }
            else
            {
                /*  ��Ի�������뻥�����ƻ�����  */
                dest[destindex] = '\0';
                return dest;
            }
        }
        else
        {
            dest[destindex] = src[i];
            destindex++;
        }
    }
    dest[destindex] = '\0';
    return dest;
}
#undef BOOL
#undef FALSE
#undef TRUE

/* ����strcatsafe���������뻥ؤ��ľ��������������ئ�У� */
int strcatsafe(char *dest, int destlen, char *append)
{
    int dl = strlen(dest);
    int al = strlen(append);

    if ((dl + al) >= destlen)
    {
        return -1;
    }
    else
    {
        strcat(dest, append);
    }
    return 0;
}

char *chop(char *s)
{
    int l = strlen(s);
    if (l >= 1)
    {
        s[l - 1] = 0;
    }
    return s;
}

// CoolFish: Family 2001/5/30

/*void easyGetTokenFromBuf(char *src, char delim, int count, char *output, int len)
{
    int  i;
    int  counter = 0;
    int  wordflag = 0;
    
    if( len <= 0 ) return;
    
    #define ISSEPARATE( c )   ((c) == delim)
    
    for( i=0; ; i++){
    	if( src[i] == '\0' ){
    		output[0] = '\0';
    		return;
    	}
    	if(IS_2BYTEWORD(src[i])){
    		if( i>0 && wordflag == 1 ){
    			i++;
    			wordflag = 1;
    			continue;
    		}
    		if( i>0 && !ISSEPARATE(src[i-1]) && !ISSEPARATE(src[i])){
    			i++;
    			wordflag = 1;
    			continue;
    		}						
    		wordflag = 1;
    	}else{
    		if( i>0 && wordflag ){
    			wordflag = 0;
    			continue;
    		}
    		if( i>0 && !ISSEPARATE(src[i-1]) && !ISSEPARATE(src[i])){
    			wordflag = 0; 
    			continue;
    		}
    		wordflag = 0;
    	}
        if( !ISSEPARATE(src[i]) ){
        	counter++;
        	if( counter == count ){
        		int j;
        		for ( j=0; j<len-1; j++){
        			if( IS_2BYTEWORD( src[i+j] ) ){
		        		output[j] = src[i+(j++)];
		        		output[j] = src[i+j];
		        	}else{
		        		if( src[i+j]=='\0' || ISSEPARATE(src[i+j])){
		        			break;
		        		}
		        		output[j] = src[i+j];
		        	}
		        }
		        output[j] = '\0';
		        return;
		}
		if( wordflag )
			i++;
		if ( i==0)
		{
			if( IS_2BYTEWORD(src[i]) ){
				wordflag = 1;
				i++;
			}
			else
			{
				wordflag = 0;
			}
		}
	}
    }
}
*/

char *strncpy2(char *dest, const char *src, size_t n)
{
    if (n > 0)
    {
        char *d = dest;
        const char *s = src;
        int i;
        for (i = 0; i < n; i++)
        {
            if (*(s + i) == 0)
            {
                /*  ��������ƴ������� NULL   ٯë  ľ��   */
                *(d + i) = '\0';
                return dest;
            }
            if (*(s + i) & 0x80)
            {
                *(d + i) = *(s + i);
                i++;
                if (i >= n)
                {
                    *(d + i - 1) = '\0';
                    break;
                }
                *(d + i) = *(s + i);
            }
            else
                *(d + i) = *(s + i);
        }
    }
    return dest;
}

void strncpysafe(char *dest, const size_t n,
                 const char *src, const int length)
{
    /*
     * src ���� dest �� length ���������
     * strcpy, strncpy ƥ�� dest ��Ի ����������ڻ�
     *   �����ݱ�������,��ƹ��½ʧ�ͱ���������.
     * ������ƥ����strlen( src ) �� length �������лõ�
     * (  �˱������������) �� dest ��������ë  �ͻ���
     * strcpysafe ��  Ԫ��ë���£�
     */

    int Short;
    Short = min(strlen(src), length);

    /* NULL  ٯ ë��  ����  ʤ */
    if (n < Short + 1)
    {
        /*
         * �������뻥��Իئ�м�ƥ n - 1(NULL  ٯ)
         * ƥ strncpy ë����
         */
        strncpy2(dest, src, n - 1);
        dest[n - 1] = '\0';
    }
    else if (n <= 0)
    {
        return;
    }
    else
    {
        /*
         * �������뷴���б�ؤ�¼�ƥ Short ƥstrncpyë����
         * ئ�� src �巴 Short ��Ӯ��  �� NULL ��ئ�м�ƥ��
         * dest �巴 ܰ���ƻ����ʣ�
         */

        strncpy2(dest, src, Short);
        dest[Short] = '\0';
    }
}

void strcpysafe(char *dest, size_t n, const char *src)
{
    /*
     * src ���� dest �����������.
     * strcpy, strncpy ƥ�� dest ��Ի ����������ڻ�
     *   �����ݱ�������,��ƹ��½ʧ�ͱ���������.
     * ��ľë  ʲ�б�, strncpy ��ؤ�»� strlen( src ) �� n ��Ի
     *   �����ݱ巴, dest ��    �� NULL   ٯ�練ئ��ئ��.
     *
     * ���׻��Ȼ� dest ��  ���Ի src ���õ���Ӯ���ݱ巴
     * n-1 ƥ strncpy ë����. �������ݷ������������������
     *
     * n ��  ���ݷ��ƾ�����ئ�¼�ƥ  ���ݷ� ������ئ�У�
     *
     */
    // Nuke +1 (08/25): Danger if src=0
    if (!src)
    {
        *dest = '\0';
        return;
    }
    if (n <= 0) /* ������ئ��   */
        return;

    /*  ������ƥ�� n >= 1 ���������  */
    /*  NULL  ٯë��  �ƻ�  ʤ����  */
    else if (n < strlen(src) + 1)
    {
        /*
         * �������뻥��Իئ�м�ƥ n - 1(NULL  ٯ)
         * ƥ strncpy ë����
         */
        strncpy2(dest, src, n - 1);
        dest[n - 1] = '\0';
    }
    else
        strcpy(dest, src);
}

char *ScanOneByte(char *src, char delim)
{
    // Nuke
    if (!src)
        return NULL;

    //   ٯ  ��ئ��ئ����ƥ����
    for (; src[0] != '\0'; src++)
    {
        if (IS_2BYTEWORD(src[0]))
        {
            // ��ʣ�֣۹��������������������б��������£�
            // �׷��ƨ��������ƾ�ئ��������������ئ��
            if (src[1] != 0)
            {
                src++;
            }
            continue;
        }
        //   ʣ�����ף۳��ƥ���޼�  ٯ��  ʤ
        if (src[0] == delim)
        {
            return src;
        }
    }
    // �����  ������έ������ئ�����ף�
    return NULL;
}

int easyGetTokenFromBuf(char *src, char *delim, int count, char *output, int len)
{                        //ttom this function all change,copy from the second
    int i;               /* �����  �� */
    int length = 0;      /* ��Ի������  ٯ  ��Ӯ�� */
    int addlen = 0;      /* ���ľ��Ӯ�� */
    int oneByteMode = 0; /* ��������ƹ���񾮣� */

    if (strlen(src) == 0)
        return 0;

    if (strlen(delim) == 1)
    {                    // ���񻥨�������ئ�ը�������ƹ���������
        oneByteMode = 1; // �����˨�������  ٯ������������ئ��
    }
    for (i = 0; i < count; i++)
    {
        char *last;
        src += addlen; /* �Ĺ�������Ӯ��ë���� */

        if (oneByteMode)
        {
            // ��������ƹ����������ճ�����ƥ����
            last = ScanOneByte(src, delim[0]);
        }
        else
        {
            last = strstr(src, delim); /* έ������ */
        }
        if (last == NULL)
        {
            /*
             * �Ĺ�����ئ�����׼�ƥ���ͻ�������ƻ� return��
            */
            strcpysafe(output, len, src);

            if (i == count - 1)
                /*�нﵤ���Ĺ�������*/
                return 1;

            /*�Ĺ�����ئ������*/
            return 0;
        }

        /*
           * �Ĺ�����������  �ټ���  ����ë�ƻ���
           * ����Ի����ľ������  ٯ  ��Ӯ��
          */
        length = last - src;

        /*
           * �ݼ�����󼰿б��Ĺ�������Ӯ���� delim ��Ӯ��ë���ƻ�����
          */
        addlen = length + strlen(delim);
    }
    strncpysafe(output, len, src, length);

    return 1;
}
