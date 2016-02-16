#include "GUI_WM.h"
#include "GUI.h"
#include "RootWindow.h"

/* �ҵ�����Z�������Ӵ��ڣ��������Լ�������һ�����ڣ�ע�ⴰ��hһ��������NULL */
#define WM__FindChildEnd(h) \
    (((WM_Obj *)WM_GetTopChildWindow(h))->hNextLine)

#if GUI_USE_MEMORY
static GUI_RECT __PaintArea;
#endif

static int __InvalidWindowNum = 0;

static void WM__SendMessage(WM_hWin hWin, WM_MESSAGE *pMsg);
static void WM__RefreshBackgnd(WM_Obj *pWin, RECT_LIST RectList);

/*
 * ע��,���ڹ������ж��ȫ�ֱ���,�����ʹ�ò���ϵͳʱ
 * Ҫע���̰߳�ȫ��(��������).
 **/

/* ���ڹ�������ʼ�� */
GUI_RESULT WM_Init(void)
{
    GUI_hWin *p = &_hRootWin;
    
    *p = GUI_fastmalloc(sizeof(WM_Obj));
    if (*p == NULL) {
        return GUI_ERR;
    }
    WM_RootWindowInit(*p);
    return GUI_OK;
}

/* ���ڹ������ػ� */
static u_8 WM__WindowRepaint(WM_Obj *pWin)
{
    RECT_LIST RectList;

    if (WM_CheckInvalid(pWin)) { /* ������Ҫ�ػ� */
        /* ���㴰�ڲü�����,������Ϊ��ǰ�Ļ������� */
        RectList = GUI_CalcWindowRectList(pWin);
        if (RectList != NULL) {
            GUI_RECT Rect;

            /* ͸��������ˢ���ڵ����� */
            if (pWin->Style & WM_WINDOW_TRANS) {
                WM__RefreshBackgnd(pWin, RectList);
            }
            /* �ػ洰�� */
            Rect = WM_GetTaliorInvalidRect(pWin);
            GUI_SetNowRectList(RectList, &Rect);
            WM_SendMessage(pWin, WM_PAINT, (GUI_PARAM)NULL);
            GUI_FreeRectList(RectList);
#if GUI_USE_MEMORY
            /* ��Ҫˢ�µľ��� */
            __PaintArea = GUI_RectOrCalc(&pWin->Rect, &__PaintArea);
#endif
        }
        WM_CleanInvalid(pWin); /* �����Ч�� */
    }
    return 0;
}

/* �ػ����д��� */
static void _PaintAll(void)
{
    WM_Obj *pWin;

    if (__InvalidWindowNum) {
        GUI_LOCK();
        /* �������ػ洰�� */
        for (pWin = _hRootWin; pWin; pWin = pWin->hNextLine) {
            WM__WindowRepaint(pWin);
            --__InvalidWindowNum;
        }
        GUI_UNLOCK();
    }
}

/* �ɷ���Ϣ */
static void WM__DispatchMessage(WM_MESSAGE *pMsg)
{
    if (pMsg->hWin) { /* ��Ŀ�괰�ڵ���Ϣ */
        WM__SendMessage(pMsg->hWin, pMsg);
    } else {
        GUI_TouchPadMessageHandle(pMsg); /* �������¼� */
    }
}

/* ͨ��ִ�лص��ػ���Ч����(���й���) */
void WM_Exec(void)
{
    WM_MESSAGE Msg;

    /* WM��Ϣ�� */
    while (GUI_GetMessage(&Msg) == GUI_OK) {
        WM__DispatchMessage(&Msg); /* �ɷ���Ϣ */
    }
    GUI_TimerHandle();
    _PaintAll(); /* �ػ����д��� */
#if GUI_USE_MEMORY
    GUI_LOCK();
    /* ���ڴ����ݸ��µ�LCD�� */
    if (__PaintArea.x1 || __PaintArea.y1) {
        __PaintArea = GUI_RectAndCalc(&_pRootWin->Rect, &__PaintArea);
        GUI_DrawFromMem(__PaintArea.x0, __PaintArea.y0,
            __PaintArea.x1 - __PaintArea.x0 + 1,
            __PaintArea.y1 - __PaintArea.y0 + 1);
        __PaintArea.x0 = 0;
        __PaintArea.y0 = 0;
        __PaintArea.x1 = 0;
        __PaintArea.y1 = 0;
    }
    GUI_UNLOCK();
#endif
}

/*
 * �򴰿ڹ�������ָ�����ڷ�����Ϣ��
 * hWin:Ҫָ�����ڵľ��,ΪNULLʱ��ֱ�ӷ��ء�
 **/
static void WM__SendMessage(WM_hWin hWin, WM_MESSAGE *pMsg)
{
    static u_8 __MseeageCount;
    
    if (hWin == NULL) { /* hWin����ΪNULL */
        return;
    }
    GUI_LOCK();
    if (__MseeageCount < GUI_MAX_MSG_NEST) {
        ++__MseeageCount;
        pMsg->hWin = hWin;
        if (((WM_Obj *)hWin)->WinCb) {
            ((WM_Obj *)hWin)->WinCb(pMsg);
        }
        --__MseeageCount;
    }
    GUI_UNLOCK();
}

/* �򴰿ڹ�������ָ�����ڷ�����Ϣ */
void WM_SendMessage(WM_hWin hWin, u_16 MsgId, GUI_PARAM Param)
{
    WM_MESSAGE Msg;

    Msg.MsgId = MsgId;
    Msg.Param = Param;
    WM__SendMessage(hWin, &Msg);
}

/* �򴰿ڹ�������ָ�����ڵĸ����ڷ�����Ϣ */
void WM_SendMessageToParent(WM_hWin hWin, u_16 MsgId, GUI_PARAM Param)
{
    WM_MESSAGE Msg;

    Msg.hWinSrc = hWin;
    Msg.MsgId = MsgId;
    Msg.Param = Param;
    WM__SendMessage(((WM_Obj*)hWin)->hParent, &Msg);
}

/* ����Ϣ���з�����Ϣ */
GUI_RESULT WM_PostMessage(WM_hWin hWin, u_16 MsgId, GUI_PARAM Param)
{
    WM_MESSAGE Msg;

    Msg.hWin = hWin;
    Msg.MsgId = MsgId;
    Msg.Param = Param;
    return GUI_PostMessage(&Msg);
}

/*
 * ��ȡ������Ч�������С
 * -�ú���ͨ����Ŀ�괰�ڵ��������е����ȴ��ڵ��û���ȡ�����õ���Ч����
 * -hWin������NULL
 **/
GUI_RECT WM_GetWindowAreaRect(WM_hWin hWin)
{
    GUI_RECT Rect;

    GUI_LOCK();
    Rect = ((WM_Obj*)hWin)->Rect;
    while (((WM_Obj*)hWin) != _pRootWin) {
        hWin = ((WM_Obj*)hWin)->hParent;
        Rect = GUI_RectAndCalc(&((WM_Obj*)hWin)->UserRect,&Rect);
    }
    GUI_UNLOCK();
    return Rect;
}

/* ��òü���Ĵ�����Ч�����С��hWin������NULL */
GUI_RECT WM_GetTaliorInvalidRect(WM_hWin hWin)
{
    GUI_RECT Rect;

    GUI_LOCK();
    Rect = WM_GetWindowAreaRect(hWin);
    Rect = GUI_RectAndCalc(&Rect, &((WM_Obj*)hWin)->InvalidRect);
    GUI_UNLOCK();
    return Rect;
}

/* ��ñ�ĳ������Z��С1�Ĵ��� */
WM_hWin WM_GetFrontWindow(WM_hWin hWin)
{
    WM_Obj *pWin = _pRootWin;

    if (hWin == _hRootWin) { /* ������Z����С */
        return NULL;
    }
    GUI_LOCK();
    while (pWin && pWin->hNextLine != hWin) {
        pWin = pWin->hNextLine;
    }
    GUI_UNLOCK();
    return pWin;
}

/* ���ĳ������Z����ߵ��Ӵ��� */
WM_hWin WM_GetTopChildWindow(WM_hWin hWin)
{
    WM_Obj *pWin = hWin;

    GUI_LOCK();
    if (pWin && pWin->hFirstChild) {
        pWin = pWin->hFirstChild;
        while (pWin->hFirstChild || pWin->hNext) {
            if (pWin->hNext) {
                pWin = pWin->hNext;
            } else {
                pWin = pWin->hFirstChild;
            }
        }
    }
    GUI_UNLOCK();
    return pWin;
}

/* ��һ��������ӵ��������µ���� */
void WM_AttachWindow(WM_hWin hWin, WM_hWin hParent)
{
    WM_Obj *pWin = hWin, *pObj, *ptr;

    if (hWin == NULL) {
        return;
    }
    GUI_LOCK();
    if (hParent == NULL) { /* hParentΪNULLʱ��Ϊ�����ڵ��Ӵ��� */
        pObj = _pRootWin;
    } else {
        pObj = hParent;
    }
    pWin->hNext = NULL;
    pWin->hParent = pObj;
    if (pObj->hFirstChild == NULL) { /* ������û���Ӵ��� */
        pObj->hFirstChild = pWin;
    } else { /* �������Ѿ����Ӵ��� */
        pObj = pObj->hFirstChild;
        while (pObj->hNext) { /* ֱ�����ڵ����ֵ� */
            pObj = pObj->hNext;
        }
        pObj->hNext = pWin;
        /* ȡ�������Z����ߵ��Ӵ��� */
        pObj = WM_GetTopChildWindow(pObj);
    }
    /* �������� */
    ptr = WM_GetTopChildWindow(pWin);
    ptr->hNextLine = pObj->hNextLine;
    pObj->hNextLine = pWin;
    GUI_UNLOCK();
}

/* ��һ�����ڴӴ��������Ƴ� */
void WM_RemoveWindow(WM_hWin hWin)
{
    WM_Obj *pWin = hWin, *pObj;
    
    /* ���ڲ���ΪNULL,�����������Ƴ� */
    if (hWin == NULL || hWin == _pRootWin) {
        return;
    }
    pObj = pWin->hParent;
    GUI_LOCK();
    if (pObj->hFirstChild == pWin) { /* ���Ǹ����ڵĵ�һ���Ӵ��� */
        pObj->hFirstChild = pWin->hNext;   
    } else {
        pObj = pObj->hFirstChild;
        while (pObj->hNext != pWin) { /* ֱ��Ŀ�괰�ڵ����ֵ� */
            pObj = pObj->hNext;
        }
        pObj->hNext = pWin->hNext;
        pObj = WM_GetFrontWindow(pWin);
    }
    /* �����ڵ� */
    pWin->hParent = NULL;
    pWin->hNext = NULL;
    /* �������� */
    pWin = WM_GetTopChildWindow(pWin);
    pObj->hNextLine = pWin->hNextLine;
    pWin->hNextLine = NULL;
    GUI_UNLOCK();
}

/* ɾ������ */
void WM_DeleteWindow(WM_hWin hWin)
{
    WM_Obj *pWin = hWin;

    if (hWin == NULL) { /* hWin����ΪNULL */
        return;
    }
    GUI_LOCK();
    /* ���ڵ��Ĵ�����Ч�� */
    WM_InvalidCoverWindow(pWin, &pWin->Rect);
    WM_RemoveWindow(pWin); /* ���Ƴ����� */
    while (pWin) {
        hWin = pWin;
        pWin = pWin->hNextLine;
        /* ɾ�����ڵĶ�ʱ�� */
        GUI_SetWindowTimer(hWin, 0);
        /* ���ûص�����ɾ���ڵ� */
        WM_SendMessage(hWin, WM_DELETE, (GUI_PARAM)NULL);
    }
    GUI_UNLOCK();
}

/* ��ȡ��ǰ����ڵľ�� */
WM_hWin WM_GetActiveMainWindow(void)
{
    WM_Obj *pWin = _RootWin.hFirstChild;

    GUI_LOCK();
    while (pWin->hNext != NULL) { /* ����ͬ���ڵ� */
        pWin = pWin->hNext;
    }
    GUI_UNLOCK();
    return pWin;
}

/*
 * ���û����(������ڻ������ǰ����ʾ)
 * hWin:Ҫ���õĴ��ھ��
 * ����ֵ:GUI_OK,����;GUI_ERR,����,û�иô���
 **/
GUI_RESULT WM_SetActiveMainWindow(WM_hWin hWin)
{
    WM_Obj *pObj = hWin;

    if (hWin == NULL) {
        return GUI_ERR;
    }
    GUI_LOCK();
    while (pObj && pObj->hParent != _pRootWin) {
        pObj = pObj->hParent;
    }
    if (WM_GetActiveMainWindow() != pObj) {
        WM_RemoveWindow(pObj); /* ���Ƴ����� */
        WM_AttachWindow(pObj, NULL); /* ���봰�ڵ���� */
        WM_InvalidTree(pObj); /* ���ڼ������е��Ӵ�����Ч�� */
    }
    GUI_UNLOCK();
    return GUI_OK;
}

/*
 * ����һ��������Ϊָ�����ڵ��Ӵ���
 * ��hParentΪNULLʱ,���ڽ���RootWindow���Ӵ���
 **/
WM_hWin WM_CreateWindowAsChild(i_16 x0,             /* �M���� */
                               i_16 y0,             /* ������ */
                               u_16 xSize,          /* ��� */
                               u_16 ySize,          /* �߶� */
                               WM_hWin hParent,     /* �����ھ�� */
                               u_16 Flag,           /* ��־��ָ�� */
                               u_8 Sign,            /* ���ڱ�ʶ�� */
                               u_16 Id,             /* ����ID */
                               WM_CALLBACK *WinCb,  /* ���ڹ��ܻص����� */
                               u_32 bytes)          /* ���ڽṹ�������ֽ� */
{
    WM_Obj *pObj, *pParent = hParent;

    GUI_LOCK();
    /* ��鴰��ID�Ƿ��Ѿ�ע�� */
    pObj = WM_GetWindowHandle(Id);
    if (pObj != NULL) {
        WM_SetActiveMainWindow(pObj); /* ���Ѿ�ע��Ĵ��ڷŵ���ǰ */
        GUI_UNLOCK();
        return NULL;
    }
    pObj = GUI_fastmalloc(sizeof(WM_Obj) + bytes);
    if (pObj == NULL) {
        GUI_UNLOCK();
        return NULL;
    }
    pObj->hFirstChild = NULL;
    WM_AttachWindow(pObj, pParent); /* ע�ᵽ������ */
    pObj->Style = Flag;
    pObj->Sign = Sign;
    pObj->Id = Id;
    pObj->WinCb = WinCb;
    if (pParent) {
        x0 += pParent->UserRect.x0;
        y0 += pParent->UserRect.y0;
    }
    pObj->Rect.x0 = x0;
    pObj->Rect.y0 = y0;
    pObj->Rect.x1 = x0 + xSize - 1;
    pObj->Rect.y1 = y0 + ySize - 1;
    WM_Invalidate(pObj); /* ��������ʧЧ */
    GUI_UNLOCK();
    /* ����Ϣ���з��ʹ����Ѵ�����Ϣ(�Ժ�ᴦ����Ϣ) */
    WM_PostMessage(pObj, WM_CREATED, (GUI_PARAM)NULL);
    return pObj;
}

/* 
 * ��鴰�ڱ�ʶ�� 
 * ����ֵ:0,��ʶ��ƥ��;1,��ʶ����ƥ��
 **/
GUI_RESULT WM_CheckWindowSign(WM_hWin hWin, u_16 Sign)
{
    if (hWin && ((WM_Obj*)hWin)->Sign == Sign) {
        return GUI_OK;
    }
    return GUI_ERR;
}

/* ���ڲ���������Ч�� */
void WM_InvalidateRect(WM_hWin hWin, GUI_RECT *pRect)
{
    WM_Obj* pWin = hWin;
    GUI_RECT *r = &pWin->Rect, *ri = &pWin->InvalidRect;

    GUI_LOCK();
    if (pRect == NULL) {
        pWin->InvalidRect = pWin->Rect;
    } else { /* ����� */
        if (ri->x0 > pRect->x0) {
            ri->x0 = pRect->x0 >= r->x0 ? pRect->x0 : r->x0;
        }
        if (ri->y0 > pRect->y0) {
            ri->y0 = pRect->y0 >= r->y0 ? pRect->y0 : r->y0;
        }
        if (ri->x1 < pRect->x1) {
            ri->x1 = pRect->x1 <= r->x1 ? pRect->x1 : r->x1;
        }
        if (ri->y1 < pRect->y1) {
            ri->y1 = pRect->y1 <= r->y1 ? pRect->y1 : r->y1;
        }
    }
    ++__InvalidWindowNum;
    GUI_UNLOCK();
}

/* ����������Ч�� */
void WM_Invalidate(WM_hWin hWin)
{
    WM_InvalidateRect(hWin, NULL);
}

/* ��һ�����ڼ����Ӵ�����Ч�� */
GUI_RESULT WM_InvalidTree(WM_hWin hWin)
{
    WM_Obj *pWin = hWin;

    if (hWin == NULL) {
        return GUI_ERR;
    }
    GUI_LOCK();
    /* �ҵ������Ӵ��ڵ��յ� */
    hWin = WM__FindChildEnd(pWin);
    while (pWin && pWin != hWin) {
        WM_InvalidateRect(pWin, NULL);
        pWin = pWin->hNextLine;
    }
    GUI_UNLOCK();
    return GUI_OK;
}

/* 
 * �ж�һ�������Ƿ���Ч
 * ����0:����Ч,1:��Ч
 **/
u_8 WM_CheckInvalid(WM_hWin hWin)
{
    GUI_RECT Rect;
    
    Rect = WM_GetWindowInvalidRect(hWin);
    return GUI_CheckRectNull(&Rect);
}

/* ���������Ч�� */
void WM_CleanInvalid(WM_hWin hWin)
{
    WM_Obj* pWin = hWin;

    GUI_LOCK();
    /* x0,y0����Ϊ��Ļ�ߴ��ֵ */
    pWin->InvalidRect.x0 = GUI_GetScreenWidth();
    pWin->InvalidRect.y0 = GUI_GetScreenHeight();
    pWin->InvalidRect.x1 = 0;
    pWin->InvalidRect.y1 = 0;
    GUI_UNLOCK();
}

/* ��ȡ���ڵĳߴ� */
GUI_RECT WM_GetWindowRect(WM_hWin hWin)
{
    return ((WM_Obj*)hWin)->Rect;
}

/* ��ȡ�����û�����ߴ� */
GUI_RECT WM_GetWindowUserRect(WM_hWin hWin)
{
    return ((WM_Obj*)hWin)->UserRect;
}

/* ��ȡ������Ч����ߴ� */
GUI_RECT WM_GetWindowInvalidRect(WM_hWin hWin)
{
    return ((WM_Obj*)hWin)->InvalidRect;
}

/*
 * �ڴ�������Ѱ��һ������
 * ����ֵ:0,û���������;1,���������
 * �ⲿ����
 **/
GUI_RESULT WM_FindWindow(WM_hWin hWin)
{
    WM_Obj *pWin = _hRootWin;
    
    if (pWin) {
        GUI_LOCK();
        while (pWin && pWin != hWin) { /* ���������� */
            pWin = pWin->hNextLine;
        }
        GUI_UNLOCK();
        if (pWin == hWin) {
            return GUI_OK; /* �ҵ��˴��� */
        }
    }
    return GUI_ERR; /* �����ڸô��� */
}

/* ���ָ��ID���Ӵ��ھ�� */
WM_hWin WM_GetDialogItem(WM_hWin hWin, u_16 Id)
{
    WM_Obj *pWin = hWin;

    /* WM_NULL_ID����Ѱ�� */
    if (Id == WM_NULL_ID) {
        return NULL;
    }
    GUI_LOCK();
    if (pWin && pWin->hFirstChild) {
        pWin = pWin->hFirstChild; /* ���Ӵ��ڿ�ʼѰ�� */
        /* �ҵ������Ӵ��ڵ��յ� */
        hWin = WM__FindChildEnd(hWin);
        while (pWin && pWin->Id != Id && pWin != hWin) {
            pWin = pWin->hNextLine;
        }
    } else {
        pWin = NULL;
    }
    GUI_UNLOCK();
    return pWin;
}

/* ���ָ��ID�Ĵ��ھ�� */
WM_hWin WM_GetWindowHandle(u_16 Id)
{
    WM_Obj *pWin = _pRootWin;

    /* WM_NULL_ID����Ѱ�� */
    if (Id == WM_NULL_ID) {
        return NULL;
    }
    GUI_LOCK();
    while (pWin && pWin->Id != Id) { /* �������� */
        pWin = pWin->hNextLine;
    }
    GUI_UNLOCK();
    return pWin;
}

/* ��ȡ�ؼ�ID */
u_16 WM_GetDialogId(WM_hWin hWin)
{
    if (hWin) {
        return ((WM_Obj*)(hWin))->Id;
    }
    return 0;
}

/*
 * ��������������±�¶�Ĵ���
 * ����ֵ:Ŀ�괰�ڵľ��
 **/
WM_hWin WM_GetExposedWindow(u_16 x, u_16 y)
{
    WM_Obj *pWin = NULL, *p;
    GUI_RECT r;

    GUI_LOCK();
    for (p = _pRootWin;  p; p = p->hNextLine) {
        r = WM_GetWindowAreaRect(p);
        if (GUI_CheckPointAtRect(x, y, &r) == GUI_OK) {
            pWin = p;
        }
    }
    GUI_UNLOCK();
    return pWin;
}

/* ��ȡ�����ھ�� */
WM_hWin WM_GetParentHandle(WM_hWin hWin)
{
    return ((WM_Obj*)hWin)->hParent;
}

/* ����һ�������ڸǵĴ��ڼ����Ӵ�����Ч����
 * ������ɾ������ʱ��Ҫ�ѽ�Ҫ��ɾ���Ĵ����ڵ��Ĵ�����Ч�� 
 **/
void WM_InvalidCoverWindow(WM_hWin hWin, GUI_RECT *pRect)
{
    WM_Obj *p = hWin;
    GUI_RECT r;

    /* hWin����ΪNULL,Ҳ������RootWindow */
    if (hWin == NULL && p->hParent == NULL) { 
        return;
    }
    /* �վ���ֱ�ӷ��� */
    if (GUI_CheckRectNull(pRect) == 0) {
        return;
    }
    GUI_LOCK();
    /*����������ֻ���ڸǵ����ĸ�����*/
    for (p = p->hParent;  p && p != hWin; p = p->hNextLine) {
        if (GUI_RectOverlay(&r, &p->Rect, pRect) == GUI_OK) {
            WM_InvalidateRect(p, &r);
        }
    }
    GUI_UNLOCK();
}

/* �ƶ����ڼ������е��Ӵ��� */
void WM_MoveWindow(WM_hWin hWin, i_16 dX, i_16 dY)
{
    WM_Obj *p = hWin;
    
    GUI_LOCK();
    if (p && p->Style & WM_WINDOW_MOVE && (dX || dY)) {
        /* �Ƚ����ڵ��Ĵ�����Ч�� */
        WM_InvalidCoverWindow(p, &p->Rect);
        /* �ҵ������Ӵ��ڵ��յ� */
        hWin = WM__FindChildEnd(p);
        for (; p != hWin; p = p->hNextLine) { /* �����Ӵ��� */
            GUI_MoveRect(&p->Rect, dX, dY);
            GUI_MoveRect(&p->UserRect, dX, dY);
            WM_Invalidate(p); /* ������Ч�� */
        }
    }
    GUI_UNLOCK();
}

/* ���ÿ��ƶ��Ĵ��� */
void WM_SetMoveWindow(WM_hWin hWin)
{
    ((WM_Obj*)hWin)->Style |= WM_WINDOW_MOVE;
}

/* Ϊһ�����ڼ���ü��������� */
RECT_LIST GUI_CalcWindowRectList(WM_hWin hWin)
{
    WM_Obj *pWin = hWin, *pObj;
    GUI_RECT Rect1, Rect2;
    RECT_LIST RectList;

    /* �ڿ����ڵ�֮ǰ,���ھ�ֻ��һ���ü����� */
    RectList = GUI_GetRectList(1);
    if (RectList) {
        Rect1 = WM_GetTaliorInvalidRect(pWin);
        RectList->Rect = Rect1;
    }
    /* ���ڻᱻ���Ķ����ǻ����ұߵ��ֵ���(����еĻ�)�ü�,Ҳ�����ڵ���������Щ���Ӻ��ֵ��ǣ�
     *������㴰�ڵĲü����α������ܵõ�������ڱ������ڵ���Ĳü����α�
     **/
    if (pWin->hFirstChild) {
        /* �ȱ��������е��Ӵ��� */
        pObj = pWin->hFirstChild;
        while (pObj && RectList) {
                RectList = GUI_ReCalcRectList(RectList, &pObj->Rect);
            if (pObj->Style & WM_WINDOW_TRANS /* ��֮�ཻ��͸��������Ч�� */
              && GUI_RectOverlay(&Rect2, &pObj->Rect, &Rect1) == GUI_OK) {
                WM_InvalidateRect(pObj, &Rect2);
            }
            pObj = pObj->hNext; /* ���ұ��� */
        }
    }
    /* �ٱ������ұߵ�ͬ�����ڼ����ȴ��ڵ�ͬ������ */
    pObj = pWin;
    while (pObj != _pRootWin && RectList) {
        while (pObj->hNext && RectList){
            pObj = pObj->hNext; /* ���ұ��� */
                RectList = GUI_ReCalcRectList(RectList, &pObj->Rect);
            if (pObj->Style & WM_WINDOW_TRANS /* ��֮�ཻ��͸��������Ч�� */
                && GUI_RectOverlay(&Rect2, &pObj->Rect, &Rect1) == GUI_OK) {
                WM_InvalidateRect(pObj, &Rect2);
            }
        }
        pObj = pObj->hParent; /* ���ϱ��� */
    }
    return RectList;
}

/*****************************************************************************
* �ⲿ�ִ�������ʵ����͸��Ч����
* ע�⣺
* �ⲿ�ֳ����Ч�ʿ��ܺܵ͡�
* 
* ����͸����ԭ��
* ������һ��������͸������ʱ�������ػ������б�͸�������ڵ��Ĵ���֮����ȥ�ػ浱ǰ��͸�����ڣ�
* �Ա�֤��ʾ�����ص������������Ѿ��������Ҫ��ʵ��͸�����ڵĲü����α�����ֱ��������ü���
* �α�Ϊ�������ɱ��ڵ����ڵĲü����α����������ڵ��Ĵ��ھͲ��Ḳ�Ƕ���Ĳ��֣�������ȫ����
* �ڿɼ���͸�������£�Ҳ�������ǲ��ῴ�����ڵ��Ĵ���Խ�����ı߽���ʾ����
* һ��ֵ��ע�������ǣ����͸�������ڵ�����һ��͸�����ڣ��������̾ͻ��ֽ���һ�Σ�ֱ������͸
* �����ڶ������ڵ�͸������Ϊֹ����һ����ʹ�õݹ�����׾�ʵ���ˣ����ǵݹ��Ч�ʲ��ߣ����ҶԶ�
* ջ������Ҳ�ܴ�����һ��Ҫ����͸�����ڶ�͸�����ڵ��ڵ�������
*****************************************************************************/

/* Ϊһ�����ڼ���ü���������ֱ������ĳ������ */
static RECT_LIST GUI__RectListTo(WM_Obj *pWin, WM_Obj *pEnd, RECT_LIST List)
{
    u_16 i;
    WM_Obj *pObj = NULL;
    GUI_RECT Rect;
    RECT_LIST pt, RectList = List;

    /* ͳ�Ʋü����������е��ཻ���� */
    for (i = 0; RectList; RectList = RectList->pNext) {
        if (GUI_CheckRectIntersect(&RectList->Rect, &pWin->Rect)) {
            ++i;
        }
    }
    RectList = GUI_GetRectList(i);
    pt = RectList;
    while (pt && List) { /* �����ཻ���ֵľ������� */
        if (GUI_RectOverlay(&Rect, &List->Rect, &pWin->Rect) == GUI_OK) {
            pt->Rect = Rect;
            pt = pt->pNext;
        }
        List = List->pNext;
    }
    /* ���ڻᱻ���Ķ����ǻ����ұߵ��ֵ���(����еĻ�)�ü�,Ҳ�����ڵ� */
    if (pWin->hFirstChild) {
        /* �ȱ��������е��Ӵ��� */
        pObj = pWin->hFirstChild;
        while (pObj && pObj != pEnd && RectList) {
            RectList = GUI_ReCalcRectList(RectList, &pObj->Rect);
            pObj = pObj->hNext; /* ���ұ��� */
        }
    }
    if (!pEnd || pObj != pEnd) { /* pEndΪNULL���߻�û����pEnd */
        /* �ٱ������ұߵ�ͬ�����ڼ����ȴ��ڵ�ͬ������ */
        pObj = pWin;
        while (pObj != _pRootWin && RectList) {
            while (pObj->hNext && pObj->hNext != pEnd && RectList) {
                pObj = pObj->hNext; /* ���ұ��� */
                RectList = GUI_ReCalcRectList(RectList, &pObj->Rect);
            }
            pObj = pObj->hParent; /* ���ϱ��� */
        }
    }
    return RectList;
}

/* ˢ��ָ�������ڱ�ĳ�������ڵ�������Ļص����� */
static void WM__RefBackCb(WM_Obj *pWin, WM_Obj *pObj, RECT_LIST List)
{
    /* ������������Ƿ��ཻ */
    if (GUI_CheckRectIntersect(&pObj->Rect, &pWin->Rect)) {
        /* ����ü����� */
        List = GUI__RectListTo(pWin, pObj, List);
        if (List != NULL) {
            WM_MESSAGE Msg;

            /* ͸��������ˢ���ڵ����� */
            if (pWin->Style & WM_WINDOW_TRANS) {
                WM__RefreshBackgnd(pWin, List); /* ע��ݹ飡 */
            }
            /* ���Ǳ������͸�������ڵ���һ�����ڣ���������Ҫ�ȱ�ˢ�£�
             * Ȼ�����ʹ�����͸�����ڵõ�ˢ�¡�
             */
            Msg.MsgId = WM_PAINT;
            Msg.hWin = (WM_hWin)pWin;
            GUI_SetNowRectList(List, &pWin->Rect);
            pWin->WinCb(&Msg);
            GUI_FreeRectList(List);
        }
    }
}

/* ˢ��ָ�������ڱ�ĳ�������ڵ�����������͸������ˢ�±��� */
static void WM__RefreshBackgnd(WM_Obj *pWin, RECT_LIST RectList)
{
    WM_Obj *pObj;

    /* ��������ڵĸ����ڿ�ʼ���� */
    for (pObj = pWin->hParent; pObj != pWin; pObj = pObj->hNextLine) {
        WM__RefBackCb(pObj, pWin, RectList);
    }
}

/*********         ����͸��ʵ�ֲ��ֽ���         *********/
