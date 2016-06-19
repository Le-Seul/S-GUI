/*****************************************************************************
 * S-GUI 2Dͼ�ο�
 *****************************************************************************/

#include "GUI_2D.h"
#include "GUI.h"

/* ȫ͸��ֱ�ӷ��� */
#define RETURN_TRANSPARENT() \
    if (Color >> 24 == 0xff) return;

static void _SetPixel(i_16 x, i_16 y, GUI_COLOR Color)
{
    CHECK_X(x);
    CHECK_Y(y);
    GL_SetPixel(x, y, Color);
}

/* ���� */
void GUI_DrawPoint(i_16 x,i_16 y,GUI_COLOR Color)
{
    GUI_RECT r1;

    RETURN_TRANSPARENT();
    GUI_ClientToScreen(&x, &y); /* ת������Ļ����ϵ */
    GUI_Val2Rect(&r1, x, y, 1, 1);
    GUI_DrawAreaInit(&r1);
    while (GUI_GetNextArea()) { /* �������е���ʾ���� */
        if (Color >> 24) {
            Color = GUI_AlphaBlend(Color, GL_GetPixel(x, y), Color >> 24);
        }
        _SetPixel(x, y, Color);
    }
}

/* ����͸��Ч���Ĵ�ֱ�� */
static void __VertLineAlpha(i_16 x0, i_16 y0, u_16 len, GUI_COLOR Color)
{
    i_16  y1 = y0 + len - 1;
    u_16 Alpha = Color >> 24;
    GUI_COLOR tColor;
    
    CHECK_X(x0);
    CLIP_Y(y0, y1);
    len = y1 >= y0 ? y1 - y0 + 1 : 0;
    while (len--) {
        tColor = GUI_AlphaBlend(Color, GL_GetPixel(x0, y0 + len), Alpha);
        GL_SetPixel(x0, y0 + len, tColor);
    }
}

/* ����͸����ֱ�� */
static void __VertLine(i_16 x0, i_16 y0, u_16 len, GUI_COLOR Color)
{
    i_16  y1 = y0 + len - 1;

    CHECK_X(x0);
    CLIP_Y(y0, y1);
    len = y1 >= y0 ? y1 - y0 + 1 : 0;
    while (len--) {
        GL_SetPixel(x0, y0 + len, Color);
    }
}

/* ����ֱ��
 * x0,y0:����
 * len:�߳���
 * color:��ɫ
 **/
void GUI_VertLine(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    GUI_RECT r1;

    RETURN_TRANSPARENT();
    GUI_ClientToScreen(&x0, &y0); /* ת������Ļ����ϵ */
    if (GUI_Val2Rect(&r1, x0, y0, 1, len) == FALSE) {
        return; /* ����Ϊ0 */
    }
    GUI_DrawAreaInit(&r1);
    while (GUI_GetNextArea()) { /* �������е���ʾ���� */
        if (Color >> 24) {
            __VertLineAlpha(x0, y0, len, Color);
        } else {
            __VertLine(x0, y0, len, Color);
        }
    }
}

/* ��ˮƽ��
 * x0,y0:����
 * len:�߳���
 * color:��ɫ
 **/
void GUI_HoriLine(i_16 x0, i_16 y0, u_16 len, GUI_COLOR Color)
{
    GUI_RECT r1;

    RETURN_TRANSPARENT();
    GUI_ClientToScreen(&x0, &y0); /* ת������Ļ����ϵ */
    if (GUI_Val2Rect(&r1, x0, y0, len, 1) == FALSE) {
        return; /* ����Ϊ0 */
    }
    GUI_DrawAreaInit(&r1);
    while (GUI_GetNextArea()) { /* �������е���ʾ���� */
        GL_DrawHLine(x0, y0, r1.x1, Color);
    }
}

/* �����ο� */
void GUI_DrawRect(i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, GUI_COLOR Color)
{
    GUI_HoriLine(x0 + 1, y0, xSize - 2, Color);
    GUI_HoriLine(x0 + 1, y0 + ySize - 1, xSize - 2, Color);
    GUI_VertLine(x0, y0, ySize, Color);
    GUI_VertLine(x0 + xSize - 1, y0, ySize, Color);
}

void _FillRect(i_16 x0, i_16 y0, i_16 x1, i_16 y1, GUI_COLOR Color)
{
    CLIP_X(x0, x1);
    CLIP_Y(y0, y1);
    GUI_GDev.FillRect(x0, y0, x1, y1, Color);
}

/* ������ */
void GUI_FillRect(i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, GUI_COLOR Color)
{
    GUI_RECT r;
    
    RETURN_TRANSPARENT();
    GUI_ClientToScreen(&x0, &y0); /* ת������Ļ����ϵ */
    /* ����������ת��Ϊ�ṹ�� */
    if (GUI_Val2Rect(&r, x0, y0, xSize, ySize) == FALSE) {
        return; /* �Ƿ� */
    }
    GUI_DrawAreaInit(&r);
    while (GUI_GetNextArea()) { /* �������е���ʾ���� */
        _FillRect(x0, y0, r.x1, r.y1, Color);
    }
}

/* �������� */
void GUI_DrawLine(i_16 x0, i_16 y0, i_16 x1, i_16 y1, GUI_COLOR Color)
{
    signed char sgn = 1;
    i_16 err = 0, dx = x1 - x0, dy = y1 - y0, d;

    if (GUI_ABS(dx) >= GUI_ABS(dy)) { /* б��С��1 */
        if (dx < 0) { /* �������� */
            dx = -dx;
            dy = -dy;
            d = x0;
            x0 = x1;
            x1 = d;
            d = y0;
            y0 = y1;
            y1 = d;
        }
        if (dy < 0) {
            dy = -dy;
            sgn = -1;
        }
        d = dx << 1;
        while (x0 <= x1) {
            GUI_DrawPoint(x0, y0, Color);
            ++x0;
            err += dy << 1;
            if (err > dx) {
                err -= dx << 1;
                y0 += sgn;
            }
        }
    } else {
        if (dy < 0) { /* �������� */
            dx = -dx;
            dy = -dy;
            d = x0;
            x0 = x1;
            x1 = d;
            d = y0;
            y0 = y1;
            y1 = d;
        }
        if (dx < 0) {
            dx = -dx;
            sgn = -1;
        }
        d = dy << 1;
        while (y0 <= y1) {
            GUI_DrawPoint(x0, y0, Color);
            ++y0;
            err += dx << 1;
            if (err > dy) {
                err -= dy << 1;
                x0 += sgn;
            }
        }
    }
}

/* ��8�� */
static void _Draw8Point(int x0, int y0, int x, int y, GUI_COLOR Color)
{
    _SetPixel(x0 + x, y0 + y, Color);
    _SetPixel(x0 - x, y0 + y, Color);
    _SetPixel(x0 + y, y0 + x, Color);
    _SetPixel(x0 + y, y0 - x, Color);
    if (y) {
        _SetPixel(x0 + x, y0 - y, Color);
        _SetPixel(x0 - x, y0 - y, Color);
        _SetPixel(x0 - y, y0 + x, Color);
        _SetPixel(x0 - y, y0 - x, Color);
    }
}

static void _DrawCircle(int x0, int y0, int r, GUI_COLOR Color)
{
    int x, y = r, d = -(r >> 1);

    for (x = 0; x <= y; ++x) {
        if (d < 0) {
            d += x;
        } else {
            d += x - y;
            --y;
        }
        _Draw8Point(x0, y0, x, y, Color);
    }
}

/* ��Բ */
void GUI_DrawCircle(i_16 x0, i_16 y0, u_16 r, GUI_COLOR Color)
{
    GUI_RECT Rect;

    GUI_ClientToScreen(&x0, &y0); /* ת������Ļ����ϵ */
    Rect.x0 = x0 - r;
    Rect.x1 = x0 + r;
    Rect.y0 = y0 - r;
    Rect.y1 = y0 + r;
    GUI_DrawAreaInit(&Rect);
    while (GUI_GetNextArea()) { /* �������е���ʾ���� */
        _DrawCircle(x0, y0, r, Color);
    }
}

static void _FillCircle(int x0, int y0, int r, GUI_COLOR Color)
{
    i_32 i;
    int imax = (r * 707) / 1000 + 1;
    i_32 sqmax = r * r + r / 2;
    int x = r;

    GL_DrawHLine(x0 - r, y0, x0 + r, Color);
    for (i = 1; i <= imax; ++i) {
        if (i * i + x * x > sqmax) {
            /* draw lines from outside */
            if (x > imax) {
                GL_DrawHLine(x0 - i + 1, y0 + x, x0 + i - 1, Color);
                GL_DrawHLine(x0 - i + 1, y0 - x, x0 + i - 1, Color);
            }
            --x;
        }
        /* draw lines from inside (center) */
        GL_DrawHLine(x0 - x, y0 + i, x0 + x, Color);
        GL_DrawHLine(x0 - x, y0 - i, x0 + x, Color);
    }
}

/* ���Բ */
void GUI_FillCircle(i_16 x0, i_16 y0, i_16 r, GUI_COLOR Color)
{
    GUI_RECT Rect;

    GUI_ClientToScreen(&x0, &y0); /* ת������Ļ����ϵ */
    Rect.x0 = x0 - r;
    Rect.x1 = x0 + r;
    Rect.y0 = y0 - r;
    Rect.y1 = y0 + r;
    GUI_DrawAreaInit(&Rect);
    while (GUI_GetNextArea()) { /* �������е���ʾ���� */
        _FillCircle(x0, y0, r, Color);
    }
}

void GUI_2DTest(void)
{
    GUI_DrawCircle(200, 200, 220, 0x00000000);
    GUI_DrawCircle(200, 200, 40, 0x00FF0000);
    GUI_FillCircle(200, 300, 50, 0x000000FF);
    GUI_DrawLine(320, 100, 400, 220, 0);
}
