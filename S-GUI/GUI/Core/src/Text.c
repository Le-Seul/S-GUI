#include "Text.h"
#include "GUI.h"
#include "string.h"
#include "GUI_Font.h"

/* ��ȡһ���ַ����ڵ�ǰ��������ռ�������� */
u_16 GetStringPixel(const char *str, GUI_FONT Font)
{
    u_16 len = 0;
    
    while (*str) {
        len += Font->CharWidget(str++);
    }
    return len;
}

/* ����һ������ */
u_16 GUI_SkipWord(const char *str, GUI_FONT Font, u_16 x_pix, u_16 *len)
{
    u_16 pix = 0, t_pix;
    
    *len = 0;
    if (*str > 127) { /* ȫ���ַ� */
        
    } else if ((*str >= 'A' && *str <= 'Z') ||
                (*str >= 'a' && *str <= 'z')) {
        while ((*str >= 'A' && *str <= 'Z') ||
               (*str >= 'a' && *str <= 'z')) {
            t_pix = Font->CharWidget(str++);
            if (pix  + t_pix > x_pix) {
                break;
            }
            pix += t_pix;
            (*len)++;
        }
    } else if (*str){
        pix = Font->CharWidget(str++);
        *len = 1;
    }
    return pix;
}

/* ��ʾһ��ASCII�ַ���2pp�Ŀ���� */
static void _CharASCII_2PP(i_16 x, i_16 y, char ch, GUI_COLOR Color, GUI_FONT Font)
{
    u_8 btx, yCnt, xCnt, bl, d1, d2, i;
    u_16 chh, chw, bCnt;
    const unsigned char *pTab;

    pTab = Font->GetChar(&ch);
    chh = Font->CharHeight;
    chw = Font->CharWidget(&ch);
    bl = Font->CharWidgetBytes(&ch);
    /* ���ڲü�����ͷ��� */
    CHECK_X(x, x + chw);
    CHECK_Y(y, y + chh);
    bCnt = 0;   /* �ֽڼ��� */
    for (yCnt = 0; yCnt < chh; ++yCnt) /* chh�и߶� */
    {        
        xCnt = 0; /* x�������ؼ��� */
        for (btx = 0; btx < bl; ++btx) {
            /* ��ȡһ���ֽڵ���ģ���� */
            d1 = pTab[bCnt + btx];
            for (i = 0; i < 4 && xCnt < chw; ++i) {
                /* �ó�������ص�͸���� */
                d2 = (d1 >> ((3 - i) << 1)) & 0x03;
                if (d2) {
                    d2 = (0x03 - d2) * 85;
                    GUI_DrawPoint(x + xCnt, y + yCnt, Color | ((u_32)d2 << 24));
                }
                ++xCnt;
            }
        }
        bCnt += bl;
    }
}

/* ��ʾһ��ASCII�ַ���4pp�Ŀ���� */
static void _CharASCII_4PP(i_16 x, i_16 y, char ch, GUI_COLOR Color, GUI_FONT Font)
{
    u_8 btx, yCnt, xCnt, bl, d1, d2, i;
    u_16 chh, chw, bCnt;
    const unsigned char *pTab;

    pTab = Font->GetChar(&ch);
    chh = Font->CharHeight;
    chw = Font->CharWidget(&ch);
    bl = Font->CharWidgetBytes(&ch);
    /* ���ڲü�����ͷ��� */
    CHECK_X(x, x + chw);
    CHECK_Y(y, y + chh);
    bCnt = 0;   /* �ֽڼ��� */
    for (yCnt = 0; yCnt < chh; ++yCnt) /* chh�и߶� */
    {        
        xCnt = 0; /* x�������ؼ��� */
        for (btx = 0; btx < bl; ++btx) {
            /* ��ȡһ���ֽڵ���ģ���� */
            d1 = pTab[bCnt + btx];
            for (i = 0; i < 2 && xCnt < chw; ++i) {
                /* �ó�������ص�͸���� */
                d2 = (d1 >> ((1 - i) << 2)) & 0x0f;
                if (d2) {
                    d2 = (0x0f - d2) * 17;
                    GUI_DrawPoint(x + xCnt, y + yCnt, Color | ((u_32)d2 << 24));
                }
                ++xCnt;
            }
        }
        bCnt += bl;
    }
}

/* ��ʾһ��ASCII�ַ� */
static void _CharASCII(i_16 x, i_16 y, char ch, GUI_COLOR Color, GUI_FONT Font)
{
    u_8 temp, t, pos, bytes, a;
    u_16 ch_h, ch_w, b;
    const unsigned char *pTab;

    pTab = Font->GetChar(&ch);
    ch_h = Font->CharHeight;
    ch_w = Font->CharWidget(&ch);
    bytes = Font->CharWidgetBytes(&ch);
    /* ���ڲü�����ͷ��� */
    CHECK_X(x, x + ch_w);
    CHECK_Y(y, y + ch_h);
    for (pos = 0; pos < ch_h; pos++)
    {        
        for (t = 0; t < bytes; t++) {
            temp = pTab[pos * bytes + t];
            if (t + 1 == bytes) {
                b = ch_w - t * 8;
            } else {
                b = 8;
            }
            for (a = 0; a < b; a++) {
                if(temp & 0x80)
                {
                    GUI_DrawPoint(x + t * 8 + a, y + pos, Color);
                }
                temp <<= 1;    
            }
        }  
    }
}

u_16 GUI_DispChar(i_16 x, i_16 y, const char *ch, GUI_COLOR Color, GUI_FONT Font)
{
    u_16 ch_w;
    
    ch_w = Font->CharWidget(ch);
    if (*ch <= 127) {
        if (Font->FontType == GUI_FONTTYPE_PROP)
            _CharASCII(x, y, *ch, Color, Font);
        else if (Font->FontType == GUI_FONTTYPE_PROP_AA4)
            _CharASCII_4PP(x, y, *ch, Color, Font);
        else if (Font->FontType == GUI_FONTTYPE_PROP_AA2)
            _CharASCII_2PP(x, y, *ch, Color, Font);
    } else {
        /* ���� */
        /* pos += 16; */
    }
    return ch_w;
}

/* �ڵ�ǰ����ʾ��������ʾһ���ַ��� */
void GUI_DispStringCurRect(i_16 x0,
                           i_16 y0,
                           const char *str,
                           GUI_COLOR Color,
                           GUI_FONT Font)
{
    u_16 i, slen, pos = 0;

    slen = (u_16)GUI_Strlen(str);
    for (i = 0; i < slen; ++i) {
        pos += GUI_DispChar(x0 + pos, y0, str + i, Color, Font);
    }
}

/* �ڵ�ǰ����ʾ��������ʾһ���ַ���,��ʾN���ַ� */
void GUI_DispStringCurRectN(i_16 x0,
                           i_16 y0,
                           const char *str,
                           GUI_COLOR Color,
                           GUI_FONT Font,
                           u_16 n)
{
    u_16 i, slen, pos = 0;

    slen = (u_16)GUI_Strlen(str);
    for (i = 0; i < slen && i < n; ++i) {
        pos += GUI_DispChar(x0 + pos, y0, str + i, Color, Font);
    }
}

/* �ڵ�ǰ����ʾ�����µ�ָ�����������ʾ�ַ��� */
void GUI_DspStringCurRectMiddle(i_16 x0,
                                i_16 y0,
                                i_16 xSize,
                                i_16 ySize,
                                const char *str,
                                GUI_COLOR Color,
                                GUI_FONT Font)
{
    u_16 xpos, ypos;
    
    xpos = GetStringPixel(str, Font);
    if (xpos >= xSize) {
        xpos = 0;
    } else {
        xpos = (xSize - xpos) / 2;
    }
    ypos = Font->CharHeight;
    if (ypos >= ySize) {
        ypos = 0;
    } else {
        ypos = (ySize - ypos) / 2;
    }
    GUI_DispStringCurRect(x0 + xpos, y0 + ypos, str, Color, Font);
}

/* ��ָ����������ʾһ���ַ��� */
void GUI_DispStringAt(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      const char *str,
                      GUI_COLOR Color,
                      GUI_FONT Font)
{

}
