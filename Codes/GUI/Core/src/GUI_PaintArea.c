#include "GUI_PaintArea.h"
#include "GUI_WM.h"
#include "GUI_ClipArea.h"
#include "GUI_Rect.h"

/* ����ʱĬ�ϵ�������*/
static GUI_PAINTCONTEXT paint_context;

/* ����Ĭ�ϵ������� */
void GUI_BackupContext(void)
{
    paint_context.font = gui_context.font;
    paint_context.background = gui_context.background;
    paint_context.foreground = gui_context.foreground;
    paint_context.fontColor = gui_context.fontColor;
    paint_context.penSize = gui_context.penSize;
}

/* ͬ������������ */
void GUI_SyncContext(void)
{
    gui_context.font = paint_context.font;
    gui_context.background = paint_context.background;
    gui_context.foreground = paint_context.foreground;
    gui_context.fontColor = paint_context.fontColor;
    gui_context.penSize = paint_context.penSize;
}

/* ���ͼ�������� */
void GUI_SetContext(GUI_HWIN hWin)
{
    GUI_RECT *r;
    GUI_AREA Area;

    Area = GUI_GetWindowClipArea(hWin); /* ��ȡ���ڵļ����� */
    r = WM_GetWindowRect(hWin);
    gui_context.hWin = hWin;
    gui_context.area = Area;
    gui_context.invalidRect = WM_GetWindowInvalidRect(hWin);
    gui_context.winPos.x = r->x0;
    gui_context.winPos.y = r->y0;
}

/* ��ʼ���������� */
void GUI_PaintAreaInit(GUI_RECT *p)
{
    GUI_RECT *pInvalid = gui_context.invalidRect;

    if (GUI_RectOverlay(&gui_context.drawRect, pInvalid, p)) {
        gui_context.pAreaNode = gui_context.area;
    } else {
        gui_context.pAreaNode = NULL; /* ��ͼ�����뵱ǰ����Ч���������ཻ */
    }
}

/* ��ȡ��һ�����ƾ��� */
GUI_BOOL GUI_NextPaintArea(void)
{
    GUI_BOOL res = FALSE;
    GUI_AREA area;

    while (gui_context.pAreaNode && res == FALSE) { /* ֱ���ҵ���һ���ཻ�ľ��� */
        area = gui_context.pAreaNode;
        gui_context.pAreaNode = area->pNext;
        res = GUI_RectOverlay(&gui_context.clipRect, &gui_context.drawRect, &area->Rect);
    }
    return res;
}
