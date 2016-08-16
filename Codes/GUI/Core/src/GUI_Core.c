#include "GUI_Core.h"
#include "GUI.h"

GUI_HWIN GUI_RootWin;        /* ������ */
GUI_AREA GUI_AreaHeap;       /* �ü������ */
GUI_CONTEXT GUI_Context;     /* GUI������ */
static u_32 __LockTaskId;
static u_16 __TaskLockCnt;

/* �ڴ�����ʼ�� */
static GUI_RESULT _MemInit(void)
{
    int page = 0;
    u_32 size;
    void *p;
    GUI_RESULT res = GUI_ERR;

    do {
        p = _GUI_GetHeapBuffer(page++, &size);
        if (GUI_HeapInit(p, size) == GUI_OK) {
            res = GUI_OK;
        }
    } while (p != NULL);
    return res;
}

/* GUI��ʼ�� */
GUI_RESULT GUI_Init(void)
{
    /* �ڴ�����ʼ�� */
    if (_MemInit() == GUI_ERR) {
        return GUI_ERR;
    }
    /* ��ʼ������ϵͳ��ش��� */
    GUI_InitOS();
    /* ��ʼ��ͼ��Ӳ�� */
    GUI_DeviceInit();
    /* ��ʼ�����ڼ�����ü�˽�ж� */
    if (GUI_RectListInit(GUI_RECT_HEAP_SIZE) == GUI_ERR) {
        return GUI_ERR;
    }
    /* ��ʼ����Ϣ���� */
    if (GUI_MessageQueueInit() == GUI_ERR) {
        return GUI_ERR;
    }
    /* ��ʼ�����ڹ����� */
    if (WM_Init() == GUI_ERR) {
        return GUI_ERR;
    }
    GUI_SetFont(&GUI_DEF_FONT);
    GUI_SetPenSize(1); /* Ĭ���߿�Ϊ1 */
	GUI_Context.AAEnable = 0;
	GUI_Context.AAFactor = 3;
    return GUI_OK;
}

/* ���ڴ���ж��GUI */
void GUI_Unload(void)
{
    GUI_LOCK();
    WM_DeleteWindow(_hRootWin); /* ɾ�����д��� */
    GUI_MessageQueueDelete();   /* ɾ����Ϣ���� */
    GUI_UNLOCK();
}

/* ��ȡ��Ļ�ߴ� */
void GUI_ScreenSize(u_16 *xSize, u_16 *ySize)
{
    *xSize = GUI_GDev.xSize;
    *ySize = GUI_GDev.ySize;
}

/* ��ȡ��Ļ��� */
u_16 GUI_GetScreenWidth(void)
{
    return GUI_GDev.xSize;
}

/* ��ȡ��Ļ�߶� */
u_16 GUI_GetScreenHeight(void)
{
    return GUI_GDev.ySize;
}

/* GUI��ʱ������ */
void GUI_Delay(GUI_TIME tms)
{
    GUI_TIME t = GUI_GetTime();

    WM_Exec();
    t = GUI_GetTime() - t; /* ����ִ��WM_Exec()��ʱ�� */
    if (tms > t) {
        _GUI_Delay_ms(tms - t); /* ��ʱ */
    }
}

/* -------------------- GUI������ -------------------- */

/* GUI���� */
void GUI_LOCK(void)
{
    /* ��û�����������Ѿ��������������� */
    if (!__TaskLockCnt || __LockTaskId != GUI_GetTaskId()) {
        GUI_TaskLock();
        __LockTaskId = GUI_GetTaskId();
    }
    ++__TaskLockCnt;
}

/* GUI���� */
void GUI_UNLOCK(void)
{
    if (--__TaskLockCnt == 0) {
        GUI_TaskUnlock();
    }
}

/* ����ͼ�������� */
static void _CopyContext(GUI_CONTEXT *pDst, GUI_CONTEXT *pSrc)
{
    pDst->Font = pSrc->Font;
    pDst->BGColor = pSrc->BGColor;
    pDst->FGColor = pSrc->FGColor;
    pDst->FontColor = pSrc->FontColor;
    pDst->PenSize = pSrc->PenSize;
}

/* GUI��ʼ���� */
GUI_BOOL GUI_StartPaint(GUI_HWIN hWin, GUI_CONTEXT *Backup)
{
    GUI_RECT *r;
    GUI_AREA Area;

    Area = GUI_GetWindowClipArea(hWin); /* ��ȡ���ڵļ����� */
    if (Area) {
        _CopyContext(Backup, &GUI_Context); /* ����ͼ�������� */
        r = WM_GetWindowRect(hWin);
        GUI_Context.Area = Area;
        GUI_Context.InvalidRect = WM_GetWindowInvalidRect(hWin);
        GUI_Context.hWin = hWin;
        GUI_Context.WinPos.x = r->x0;
        GUI_Context.WinPos.y = r->y0;
        return GUI_OK;
    }
    return GUI_ERR;
}

/* GUI���ƽ��� */
void GUI_EndPaint(GUI_CONTEXT *Backup)
{
    _CopyContext(&GUI_Context, Backup); /* ��ԭͼ�������� */
}

/* ��ȡ��ǰ���ƵĴ��� */
GUI_HWIN GUI_GetPaintWindow(void)
{
    return GUI_Context.hWin;
}

/* ���ص�ǰ�ļ����� */
GUI_AREA GUI_GetClipArea(void)
{
    return GUI_Context.Area;
}

/* ��ʼ���������� */
void GUI_DrawAreaInit(GUI_RECT *p)
{
    GUI_RECT r;
    GUI_RECT *pInvalid = GUI_Context.InvalidRect;

	if (GUI_Context.AAEnable) {
		int factor = GUI_Context.AAFactor;

		r.x0 = pInvalid->x0 * (i_16)factor;
		r.y0 = pInvalid->y0 * (i_16)factor;
		r.x1 = (pInvalid->x1 + 1) * (i_16)factor - 1;
		r.y1 = (pInvalid->y1 + 1) * (i_16)factor - 1;
    } else {
        r = *pInvalid;
    }
    if (GUI_RectOverlay(&GUI_Context.DrawRect, &r, p)) {
        GUI_Context.pAreaNode = GUI_Context.Area;
    } else {
        GUI_Context.pAreaNode = NULL; /* ��ͼ�����뵱ǰ����Ч���������ཻ */
    }
}

/* ��ȡ��һ���ü����� */
GUI_BOOL GUI_GetNextArea(void)
{
    GUI_BOOL res = FALSE;
    GUI_AREA Area;
	GUI_RECT r;
	GUI_RECT *ClipRect = &GUI_Context.ClipRect;
	GUI_RECT *DrawRect = &GUI_Context.DrawRect;

    while (GUI_Context.pAreaNode && res == FALSE) { /* ֱ���ҵ���һ���ཻ�ľ��� */
        Area = GUI_Context.pAreaNode;
        GUI_Context.pAreaNode = Area->pNext;
		if (GUI_Context.AAEnable) {
			int factor = GUI_Context.AAFactor;

			r.x0 = Area->Rect.x0 * (i_16)factor;
			r.y0 = Area->Rect.y0 * (i_16)factor;
			r.x1 = (Area->Rect.x1 + 1) * (i_16)factor;
			r.y1 = (Area->Rect.y1 + 1) * (i_16)factor - 1;
		} else {
			r = Area->Rect;
		}
        res = GUI_RectOverlay(ClipRect, DrawRect, &r);
    }
    return res;
}

/* ת������Ļ���� */
void GUI_ClientToScreen(i_16 *x, i_16 *y)
{
    *x += GUI_Context.WinPos.x;
    *y += GUI_Context.WinPos.y;
}

/* ����ת������Ļ���� */
void GUI_ClientToScreenRect(GUI_RECT *pRect)
{
    pRect->x0 += GUI_Context.WinPos.x;
    pRect->y0 += GUI_Context.WinPos.y;
    pRect->x1 += GUI_Context.WinPos.x;
    pRect->y1 += GUI_Context.WinPos.y;
}

/* ת������������ */
void GUI_ScreenToClient(i_16 *x, i_16 *y)
{
    *x -= GUI_Context.WinPos.x;
    *y -= GUI_Context.WinPos.y;
}

/* ��ȡ��ǰ�����ڴ�������ϵ�µľ��� */
void GUI_GetClientRect(GUI_RECT *pRect)
{
    GUI_RECT *p;

    p = WM_GetWindowRect(GUI_Context.hWin);
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = p->x1 - p->x0;
    pRect->y1 = p->y1 - p->y0;
}

/* ���õ�ǰ���� */
void GUI_SetFont(GUI_FONT *Font)
{
    GUI_Context.Font = Font;
}

/* ���ñ���ɫ */
void GUI_SetBGColor(GUI_COLOR Color)
{
    GUI_Context.BGColor = Color;
}

/* ����ǰ��ɫ */
void GUI_SetFGColor(GUI_COLOR Color)
{
    GUI_Context.FGColor = Color;
}

/* ����������ɫ */
void GUI_SetFontColor(GUI_COLOR Color)
{
    GUI_Context.FontColor = Color;
}

/* ���û��ʴ�С */
void GUI_SetPenSize(int Width)
{
    GUI_Context.PenSize = Width;
}

/* ���ÿ���ݵȼ� */
void GUI_SetAAFactor(int Factor)
{
    GUI_Context.AAFactor = Factor;
}

/* GUI������� */
#if GUI_DEBUG_MODE
void GUI_DebugOut(const char *s)
{
    _GUI_DebugOut(s);
}
#endif
