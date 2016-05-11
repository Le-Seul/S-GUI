#include "GUI_DispChar.h"
#include "GUI.h"
#include "string.h"
#include "GUI_Font.h"

/* ��ȡ�ַ���Ϣ */
static GUI_CHARINFO * _GetCharInfo(const u_16 Char)
{
    GUI_FONT_PROP *pProp = GUI_Context.Font->pProp;

    while (pProp->pNext) {
        if (Char >= pProp->FirstChar && Char <= pProp->LastChar) {
            return &pProp->pCharInfo[Char];
        }
        pProp = pProp->pNext;
    }
    return NULL;
}

/* ��ȡ��ǰ�����ַ��ĸ߶� */
u_16 GUI_GetCharHeight(void)
{
    return GUI_Context.Font->CharHeight;
}

/* ��ȡ��ǰ�����ַ��Ŀ�� */
u_16 GUI_GetCharWidth(const u_16 Char)
{
    GUI_CHARINFO *pInfo = _GetCharInfo(Char);

    if (pInfo) {
        return pInfo->XSize;
    }
    return 0;
}

u_16 GUI_DispChar(i_16 x, i_16 y, const char *ch, GUI_COLOR Color, GUI_FONT Font)
{
    return 0;
}

/* �ڵ�ǰ����ʾ��������ʾһ���ַ��� */
void GUI_DispStringCurRect(i_16 x0,
                           i_16 y0,
                           const char *str,
                           GUI_COLOR Color,
                           GUI_FONT *Font)
{
    
}

/* �ڵ�ǰ����ʾ��������ʾһ���ַ���,��ʾN���ַ� */
void GUI_DispStringCurRectN(i_16 x0,
                           i_16 y0,
                           const char *str,
                           GUI_COLOR Color,
                           GUI_FONT *Font,
                           u_16 n)
{
    
}

/* �ڵ�ǰ����ʾ�����µ�ָ�����������ʾ�ַ��� */
void GUI_DspStringCurRectMiddle(i_16 x0,
                                i_16 y0,
                                i_16 xSize,
                                i_16 ySize,
                                const char *str,
                                GUI_COLOR Color,
                                GUI_FONT *Font)
{
    
}

/* ��ָ����������ʾһ���ַ��� */
void GUI_DispStringAt(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      const char *str,
                      GUI_COLOR Color,
                      GUI_FONT *Font)
{

}
