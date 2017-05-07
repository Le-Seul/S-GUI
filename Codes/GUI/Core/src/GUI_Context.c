#include "GUI_Context.h"
#include "GUI.h"

GUI_CONTEXT gui_context;     /* GUI������ */

/* ת������Ļ���� */
void GUI_ClientToScreen(int *x, int *y)
{
    *x += gui_context.winPos.x;
    *y += gui_context.winPos.y;
}

/* ����ת������Ļ���� */
void GUI_ClientToScreenRect(GUI_RECT *pRect)
{
    pRect->x0 += gui_context.winPos.x;
    pRect->y0 += gui_context.winPos.y;
    pRect->x1 += gui_context.winPos.x;
    pRect->y1 += gui_context.winPos.y;
}

/* ת������������ */
void GUI_ScreenToClient(int *x, int *y)
{
    *x -= gui_context.winPos.x;
    *y -= gui_context.winPos.y;
}

/* ��ȡ��ǰ�����ڴ�������ϵ�µľ��� */
void GUI_GetClientRect(GUI_RECT *pRect)
{
    GUI_RECT *p;

    p = WM_GetWindowRect(gui_context.hWin);
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = p->x1 - p->x0;
    pRect->y1 = p->y1 - p->y0;
}
