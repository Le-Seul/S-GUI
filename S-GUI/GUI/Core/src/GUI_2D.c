/*****************************************************************************
 * S-GUI 2Dͼ�ο�
 *****************************************************************************/

#include "GUI_2D.h"
#include "GUI.h"

/* ȫ͸��ֱ�ӷ��� */
#define RETURN_TRANSPARENT() \
    if (Color >> 24 == 0xff) return;

/* RGB888��ʽת��ΪRGB565��ʽ */
u_16 GUI_RGB888To565(u_32 RGB)
{
    u_16 R, G, B;
    R = (RGB >> 19) & 0x1F;
    G = (RGB >> 10) & 0x3F;
    B = (RGB >> 3) & 0x1F;
    return (R << 11) | (G << 5) | B;
}

/* RGB565��ʽת��ΪRGB888��ʽ */
u_32 GUI_RGB565To888(u_16 RGB)
{
    u_32 Color;
    Color = ((u_32)RGB & 0xF800) << 8;   /* R */
    Color |= ((u_32)RGB & 0x07E0) << 5;   /* G */
    Color |= ((u_32)RGB & 0x001F) << 3;   /* B */
    return Color;
}

/*
 * Alpha���
 * BkColor:������ɫ
 * Color:Ŀ����ɫ
 * Alpha:͸����(0~256)
 **/
GUI_COLOR GUI_AlphaBlend(GUI_COLOR Color, GUI_COLOR BkColor, u_16 Alpha)
{
    /* Calc Color seperations for FgColor first */
    GUI_COLOR R = (BkColor & 0xFF)    * Alpha;
    GUI_COLOR G = (BkColor & 0xFF00)  * Alpha;
    GUI_COLOR B = (BkColor & 0xFF0000)* Alpha;
    /* Add Color seperations for BkColor */
    Alpha = 256 - Alpha;
    R += (Color & 0xFF)     * Alpha;
    G += (Color & 0xFF00)   * Alpha;
    B += (Color & 0xFF0000) * Alpha;
    R = (R >> 8);
    G = (G >> 8) & 0xFF00;
    B = (B >> 8) & 0xFF0000;
    return R | G | B;
}


/* ���� */
void GUI_DrawPoint(i_16 x,i_16 y,GUI_COLOR Color)
{
    GUI_RECT r1, r2;

    RETURN_TRANSPARENT();
    GUI_Val2Rect(&r1, x, y, 1, 1);
    GUI_DrawAreaInit(&r1);
    while (GUI_GetNextArea(&r2)) { /* �������е���ʾ���� */
        if (GUI_RectOverlay(&r2, &r2, &r1)) {
            if (Color >> 24) {
                Color = GUI_AlphaBlend(Color, GUI_ReadPixel(x, y), Color >> 24);
            }
            GUI_DrawPixel(x, y, Color);
        }
    }
}

/* ��ȡ�� */
GUI_COLOR GUI_ReadPoint(i_16 x, i_16 y)
{
    GUI_RECT *r;
    
    r = GUI_GetNowArea();
    if (r && x >= r->x0 && x <= r->x1 &&
        y >= r->y0 && y <= r->y1) {
            return GUI_ReadPixel(x, y);
    }
    return 0x00000000;
}

/* ����͸��Ч���Ĵ�ֱ�� */
static void __VertLineAlpha(i_16 x0, i_16 y0, u_16 len, GUI_COLOR Color)
{
    u_16 Alpha = Color >> 24;
    GUI_COLOR tColor;
    
    while (len--) {
        tColor = GUI_AlphaBlend(Color, GUI_ReadPixel(x0, y0 + len), Alpha);
        GUI_DrawPixel(x0, y0 + len, tColor);
    }    
}

/* ����͸����ֱ�� */
static void __VertLine(i_16 x0, i_16 y0, u_16 len, GUI_COLOR Color)
{
    while (len--) {
        GUI_DrawPixel(x0, y0 + len, Color);
    }    
}

/* ����ֱ��
 * x0,y0:����
 * len:�߳���
 * color:��ɫ
 **/
void GUI_VertLine(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    GUI_RECT r1, r2;

    RETURN_TRANSPARENT();
    if (GUI_Val2Rect(&r1, x0, y0, 1, len) == FALSE) {
        return; /* ����Ϊ0 */
    }
    GUI_DrawAreaInit(&r1);
    while (GUI_GetNextArea(&r2)) { /* �������е���ʾ���� */
        if (GUI_RectOverlay(&r2, &r2, &r1)) {
            x0 = r2.x0;
            y0 = r2.y0;
            len = r2.y1 - r2.y0 + 1;
            if (Color >> 24) {
                __VertLineAlpha(x0, y0, len, Color);
            } else {
                __VertLine(x0, y0, len, Color);
            }
        }
    }
}

/*  
 * ����͸��Ч����ˮƽ��
 **/
static void __HoriLineAlpha(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    u_16 Alpha = Color >> 24;
    GUI_COLOR tColor;
    while (len--) {
        tColor = GUI_AlphaBlend(Color, GUI_ReadPixel(x0 + len, y0), Alpha);
        GUI_DrawPixel(x0 + len, y0, tColor);
    }
}

/* ����͸��ˮƽ��
 * x0,y0:����
 * len:�߳���
 * color:��ɫ
 **/
static void __HoriLine(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    while (len--) {
        GUI_DrawPixel(x0 + len, y0, Color);
    }    
}

/* ��ˮƽ��
 * x0,y0:����
 * len:�߳���
 * color:��ɫ
 **/
void GUI_HoriLine(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    GUI_RECT r1, r2;

    RETURN_TRANSPARENT();
    if (GUI_Val2Rect(&r1, x0, y0, len, 1) == FALSE) {
        return; /* ����Ϊ0 */
    }
    GUI_DrawAreaInit(&r1);
    while (GUI_GetNextArea(&r2)) { /* �������е���ʾ���� */
        if (GUI_RectOverlay(&r2, &r2, &r1)) {
            x0 = r2.x0;
            y0 = r2.y0;
            len = r2.x1 - r2.x0 + 1;
            if (Color >> 24) {
                __HoriLineAlpha(x0, y0, len, Color);
            } else {
                __HoriLine(x0, y0, len, Color);
            }
        }
    }
}

/* �����ο� */
void GUI_DrawRect(i_16 x0,i_16 y0,u_16 xSize,u_16 ySize,GUI_COLOR Color)
{
    GUI_HoriLine(x0 + 1, y0, xSize - 2, Color);
    GUI_HoriLine(x0 + 1, y0 + ySize - 1, xSize - 2, Color);
    GUI_VertLine(x0, y0, ySize, Color);
    GUI_VertLine(x0 + xSize - 1, y0, ySize, Color);
}

/* ���͸������ */
static void __FillRectAlpha(i_16 x0, i_16 y0,u_16 xSize,u_16 ySize,GUI_COLOR Color)
{
    i_16 x, y;
    u_16 Alpha = Color >> 24;
    GUI_COLOR tColor; 
    
    xSize += x0;
    ySize += y0;
    for (y = y0; y < ySize; y++) {
        for (x = x0; x < xSize; x++) {
            tColor = GUI_AlphaBlend(Color, GUI_ReadPixel(x, y), Alpha);
            GUI_DrawPixel(x, y, tColor);
        }
    }
}

/* ����͸������ */
static void __FillRect(i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, GUI_COLOR Color)
{                                     
    i_16 x, y;
    
    xSize += x0;
    ySize += y0;
    for (y = y0; y < ySize; y++) {
        for (x = x0; x < xSize; x++) {
            GUI_DrawPixel(x, y, Color);
        }
    }
}

/* ������ */
void GUI_FillRect(i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, GUI_COLOR Color)
{                                     
    GUI_RECT r1, r2;
    
    RETURN_TRANSPARENT();
    /* ����������ת��Ϊ�ṹ�� */
    if (GUI_Val2Rect(&r1, x0, y0, xSize, ySize) == FALSE) {
        return; /* �Ƿ� */
    }
    GUI_DrawAreaInit(&r1);
    while (GUI_GetNextArea(&r2)) { /* �������е���ʾ���� */
        if (GUI_RectOverlay(&r2, &r2, &r1)) {
            x0 = r2.x0;
            y0 = r2.y0;
            xSize = r2.x1 + 1 - x0;
            ySize = r2.y1 + 1 - y0;
            if (Color >> 24) {
                __FillRectAlpha(x0, y0, xSize, ySize, Color);
            } else {
                __FillRect(x0, y0, xSize, ySize, Color);
            }
        }
    }
}

/* �������� */

/* ��Բ */

/* ��Բ�� */
