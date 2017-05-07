#include "GUI_PaintArea.h"
#include "GUI_WM.h"
#include "GUI_ClipArea.h"
#include "GUI_Rect.h"

/* ����ͼ�������� */
static void _BackupContext(GUI_CONTEXT *pDst, GUI_CONTEXT *pSrc)
{
    pDst->font = pSrc->font;
    pDst->background = pSrc->background;
    pDst->foreground = pSrc->foreground;
    pDst->fontColor = pSrc->fontColor;
    pDst->penSize = pSrc->penSize;
}

/* GUI��ʼ���� */
GUI_BOOL GUI_PaintStart(GUI_HWIN hWin, GUI_CONTEXT *Backup)
{
    GUI_RECT *r;
    GUI_AREA Area;

    Area = GUI_GetWindowClipArea(hWin); /* ��ȡ���ڵļ����� */
    if (Area) {
        _BackupContext(Backup, &gui_context); /* ����ͼ�������� */
        r = WM_GetWindowRect(hWin);
        gui_context.area = Area;
        gui_context.invalidRect = WM_GetWindowInvalidRect(hWin);
        gui_context.hWin = hWin;
        gui_context.winPos.x = r->x0;
        gui_context.winPos.y = r->y0;
        return GUI_OK;
    }
    return GUI_ERR;
}

/* GUI���ƽ��� */
void GUI_PaintEnd(GUI_CONTEXT *Backup)
{
    _BackupContext(&gui_context, Backup); /* ��ԭͼ�������� */
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
