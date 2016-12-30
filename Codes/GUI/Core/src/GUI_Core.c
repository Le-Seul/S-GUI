#include "GUI_Core.h"
#include "GUI.h"

GUI_HWIN GUI_RootWin;        /* ������ */
GUI_AREA GUI_AreaHeap;       /* �ü������ */
GUI_CONTEXT GUI_Context;     /* GUI������ */
static void * __LockPtr;
static u_32 __LockTaskId;
static int __TaskLockCnt;

/* GUI��ʼ�� */
GUI_RESULT GUI_Init(void)
{
    /* �ڴ�����ʼ�� */
    if (GUI_MemoryManagementInit() == GUI_ERR) {
        return GUI_ERR;
    }
    /* ��ʼ������ϵͳ��ش��� */
    __LockPtr = GUI_TaskCreateLock();
    /* ��ʼ��ͼ��Ӳ�� */
    GUI_DeviceInit();
    /* ��ʼ�����ڼ�����ü�˽�ж� */
    if (GUI_RectListInit() == GUI_ERR) {
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
    GUI_DeleteDeviceList();     /* ɾ���豸�б� */
    GUI_UNLOCK();
}

/* ��ȡ��Ļ�ߴ� */
void GUI_ScreenSize(int *xSize, int *ySize)
{
    *xSize = GUI_GDev->Width;
    *ySize = GUI_GDev->Height;
}

/* ��ȡ��Ļ��� */
int GUI_GetScreenWidth(void)
{
    return GUI_GDev->Width;
}

/* ��ȡ��Ļ�߶� */
int GUI_GetScreenHeight(void)
{
    return GUI_GDev->Height;
}

/* GUI��ʱ������ */
void GUI_Delay(GUI_TIME tms)
{
    GUI_TIME t = GUI_GetTime();
    static GUI_TIME t_last;

    WM_Exec();
    if (t > t_last + GUI_AUTO_MANAGE_TIME) {
        t_last = t;
        GUI_FreeIdleRectList();
    }
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
    if (!__TaskLockCnt || __LockTaskId != GUI_TaskGetId()) {
        GUI_TaskLock(__LockPtr);
        __LockTaskId = GUI_TaskGetId();
    }
    ++__TaskLockCnt;
}

/* GUI���� */
void GUI_UNLOCK(void)
{
    if (--__TaskLockCnt == 0) {
        GUI_TaskUnlock(__LockPtr);
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

		r.x0 = pInvalid->x0 * (int)factor;
		r.y0 = pInvalid->y0 * (int)factor;
		r.x1 = (pInvalid->x1 + 1) * (int)factor - 1;
		r.y1 = (pInvalid->y1 + 1) * (int)factor - 1;
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

			r.x0 = Area->Rect.x0 * (int)factor;
			r.y0 = Area->Rect.y0 * (int)factor;
			r.x1 = (Area->Rect.x1 + 1) * (int)factor;
			r.y1 = (Area->Rect.y1 + 1) * (int)factor - 1;
		} else {
			r = Area->Rect;
		}
        res = GUI_RectOverlay(ClipRect, DrawRect, &r);
    }
    return res;
}

/* ת������Ļ���� */
void GUI_ClientToScreen(int *x, int *y)
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
void GUI_ScreenToClient(int *x, int *y)
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
