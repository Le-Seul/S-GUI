#include "mystring.h"

/* ���ַ����еĴ�д��ĸת��ΪСд */
void mystrlwr(char *str)
{
    do {
        if (*str >= 'A' && *str <= 'Z')  *str += 32;
    } while (*str++);
}

/* ��������ַ����Ƿ���ͬ�������ִ�Сд */
int mystricmp(char *dst, char *src)
{
    int ch1, ch2;
    do {
        if (((ch1 = (unsigned char)(*dst++)) >= 'A') && (ch1 <= 'Z'))
            ch1 += 0x20;
        if (((ch2 = (unsigned char)(*dst++)) >= 'A') && (ch2 <= 'Z'))
            ch2 += 0x20;
    } while (ch1 && (ch1 == ch2));
    return (ch1 - ch2);
}
