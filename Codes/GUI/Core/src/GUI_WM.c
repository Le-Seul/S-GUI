#include "GUI_WM.h"
#include "GUI.h"

/* �ҵ�����Z�������Ӵ��ڣ��������Լ�������һ�����ڣ�ע�ⴰ��hһ��������NULL */
#define WM__FindChildEnd(h) \
    (WM_HandleToPtr(WM_GetTopChildWindow(h))->hNextLine)

WM_HWIN gui_rootwin;        /* ������ */

static int __InvalidWindowNum = 0;

/* ���ڹ�������ʼ�� */
GUI_RESULT WM_Init(void)
{
    gui_rootwin = GUI_Malloc(sizeof(WM_Obj));
    if (gui_rootwin == NULL) {
        return GUI_ERR;
    }
    WM_RootWindowInit(gui_rootwin);
    return GUI_OK;
}

/*
* �򴰿ڹ�������ָ�����ڷ�����Ϣ��
* hWin:Ҫָ�����ڵľ��,ΪNULLʱ��ֱ�ӷ��ء�
**/
void WM__SendMessage(WM_HWIN hWin, WM_MESSAGE *pMsg)
{
    static u_8 __MseeageCount;

    if (hWin == NULL) { /* hWin����ΪNULL */
        return;
    }
    GUI_LOCK();
    if (__MseeageCount < GUI_MAX_MSG_NEST) {
        ++__MseeageCount;
        pMsg->hWin = hWin;
        if (WM_HandleToPtr(hWin)->winCb) {
            WM_HandleToPtr(hWin)->winCb(pMsg);
        }
        --__MseeageCount;
    }
#if GUI_DEBUG_MODE
    else {
        GUI_DEBUG_OUT("GUI message nesting exceeded.");
    }
#endif
    GUI_UNLOCK();
}

/* �򴰿ڹ�������ָ�����ڷ�����Ϣ */
void WM_SendMessage(WM_HWIN hWin, int MsgId, GUI_PARAM Param)
{
    WM_MESSAGE Msg;

    Msg.MsgId = MsgId;
    Msg.Param = Param;
    WM__SendMessage(hWin, &Msg);
}

/* �򴰿ڹ�������ָ�����ڵĸ����ڷ�����Ϣ */
void WM_SendMessageToParent(WM_HWIN hWin, GUI_MESSAGE *pMsg)
{
    pMsg->hWinSrc = hWin;
    WM__SendMessage(WM_HandleToPtr(hWin)->hParent, pMsg);
}

/* ����Ϣ���з�����Ϣ */
GUI_RESULT WM_PostMessage(WM_HWIN hWin, int MsgId, GUI_PARAM Param)
{
    WM_MESSAGE Msg;

    Msg.hWin = hWin;
    Msg.MsgId = MsgId;
    Msg.Param = Param;
    return GUI_PostMessage(&Msg);
}

/*
 @ ��һ�����βü������ڵĿɼ������С�
 @ ����ֵ:FALSE,�þ����봰�ڵĿɼ������ཻ��
 @       TRUE,�ü��ɹ���
 **/
static GUI_BOOL WM__ClipAtParent(GUI_RECT *pr, WM_Obj *pWin)
{
    for (; pWin; pWin = pWin->hParent) {
        if (GUI_RectOverlay(pr, pr, &pWin->rect) == FALSE) {
            return FALSE;
        }
    }
    return TRUE;
}

/* ���ڲ���������Ч�����ڲ����� */
static void _Invalidate1Abs(WM_Obj *pWin, GUI_RECT *pr)
{
    if (pWin->status & WM_WS_INVAILD) {
        GUI_RectSum(&pWin->invalidRect, &pWin->invalidRect, pr);
    } else {
        pWin->invalidRect = *pr;
        pWin->status |= WM_WS_INVAILD;
        ++__InvalidWindowNum;
    }
}

/* �ػ�һ������ */
static void _PaintOne(WM_Obj *pWin)
{
    GUI_CONTEXT Context;

    if (GUI_PaintStart(pWin, &Context) == GUI_OK) {
        /* �ػ洰�� */
        WM_SendMessage(pWin, WM_PAINT, 0);
        GUI_PaintEnd(&Context);
    }
}

/* ���ƴ��ڵ�͸���Ӵ��� */
static void _PaintTransChild(WM_Obj *pWin)
{
    GUI_RECT r = pWin->invalidRect;

    for (pWin = pWin->hFirstChild; pWin; pWin = pWin->hNext) {
        if ((pWin->status & WM_WS_TRANS)
            && GUI_RectOverlay(&pWin->invalidRect, &pWin->rect, &r)) {
            _PaintOne(pWin);
            _PaintTransChild(pWin); /* �ڻ�������͸���Ӵ��� */
        }
    }
}

/* ���ƴ���֮�ϵ�����͸������ */
static void _PaintTransTop(WM_Obj *pWin)
{
    WM_Obj *pParent;
    GUI_RECT r = pWin->invalidRect;

    for (pParent = pWin; pParent; pParent = pParent->hParent) {
        for (pWin = pParent->hNext; pWin; pWin = pWin->hNext) {
            if ((pWin->status & WM_WS_TRANS)
                && GUI_RectOverlay(&pWin->invalidRect, &pWin->rect, &r)) {
                _PaintOne(pWin);
                _PaintTransChild(pWin);
            }
        }
    }
}

/* �ػ����д��� */
static void _PaintAll(void)
{
    WM_Obj *pWin = gui_rootwin;

    if (__InvalidWindowNum) {
        int _WaitScreen(void);
        void _OutScreen(void);
        
        if (_WaitScreen()) {
            return;
        }
        /* �������ػ洰�� */
        while (__InvalidWindowNum && pWin) {
            GUI_LOCK();
            if (pWin->status & WM_WS_INVAILD) { /* ������Ҫ�ػ� */
                _PaintOne(pWin);
                pWin->status &= ~(WM_WS_INVAILD); /* ���������Ч��־ */
                --__InvalidWindowNum;
                _PaintTransChild(pWin);
                _PaintTransTop(pWin);
            }
            pWin = pWin->hNextLine;
            GUI_UNLOCK();
        }
        _OutScreen();
        __InvalidWindowNum = 0;
    }
}

/* �ɷ���Ϣ */
static void WM__DispatchMessage(WM_MESSAGE *pMsg)
{
    GUI_RESULT res = GUI_ERR;

    /* GUI_ERRΪ1��ֻҪ�������GUI_OK res���븳ֵ֮��ͻ���0 */
    res &= GUI_TouchPadMessageProc(pMsg); /* �������¼� */
    res &= GUI_KeyMessageProc(pMsg);      /* ��������¼� */
    if (res == GUI_ERR) {
        WM__SendMessage(pMsg->hWin, pMsg); /* ֱ�ӷ��͵����� */
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
    GUI_TimerHandler(); /* ����ʱ�� */
    _PaintAll(); /* �ػ����д��� */
}

/*
 @ ��ȡ������Ч�������С
 @ -�ú���ͨ����Ŀ�괰�ڵ��������е����ȴ��ڵ��û���ȡ�����õ���Ч����
 @ -hWin������NULL
 **/
void WM_GetWindowAreaRect(WM_HWIN hWin, GUI_RECT *pRect)
{
    GUI_LOCK();
    *pRect = WM_HandleToPtr(hWin)->rect;
    while (hWin != gui_rootwin) {
        hWin = WM_HandleToPtr(hWin)->hParent;
        GUI_RectOverlay(pRect, pRect, &WM_HandleToPtr(hWin)->rect);
    }
    GUI_UNLOCK();
}

/* ��òü���Ĵ�����Ч�����С��hWin������NULL */
void WM_GetTaliorInvalidRect(WM_HWIN hWin, GUI_RECT *pRect)
{
    GUI_LOCK();
    WM_GetWindowAreaRect(hWin, pRect);
    GUI_RectOverlay(pRect, pRect, &WM_HandleToPtr(hWin)->invalidRect);
    GUI_UNLOCK();
}

/* ��ñ�ĳ������Z��С1�Ĵ��� */
WM_HWIN WM_GetFrontWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = gui_rootwin;

    if (hWin == gui_rootwin) { /* ������Z����С */
        return NULL;
    }
    GUI_LOCK();
    while (pWin && pWin->hNextLine != hWin) {
        pWin = pWin->hNextLine;
    }
    GUI_UNLOCK();
    return pWin;
}

/* 
 @ ���ĳ������������Ӵ���.
 @ �������ӵ���Ӵ����򷵻�������Ӵ���,���򷵻ش����Լ�.
 **/
WM_HWIN WM_GetTopChildWindow(WM_HWIN hWin)
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

/* ��ȡZ����ߵĴ��ڵľ�� */
WM_HWIN WM_GetTopWindow(void)
{
    return WM_GetTopChildWindow(gui_rootwin);
}

/* ��һ��������ӵ������������µ���� */
void WM_AttachWindow(WM_HWIN hWin, WM_HWIN hParent)
{
    WM_Obj *pWin = hWin, *pObj;

    if (hWin == NULL) {
        return;
    }
    GUI_LOCK();
    WM_Invalidate(WM_GetForegroundWindow()); /* �Ƚ�֮ǰ��ǰ��������Ч�� */
    if (hParent == NULL) { /* hParentΪNULLʱ��Ϊ�����ڵ��Ӵ��� */
        pObj = gui_rootwin;
    } else {
        pObj = hParent;
    }
    pWin->hNext = NULL;
    pWin->hParent = pObj;
    if (pObj->hFirstChild == NULL) { /* ������û���Ӵ��� */
        pObj->hFirstChild = pWin;
    } else { /* �������Ѿ����Ӵ��� */
        pObj = pObj->hFirstChild;
        if (pObj->status & WM_WS_STICK) {
            pWin->hNext = pObj;
            pObj = pObj->hParent;
            pObj->hFirstChild = pWin;
        } else {
            WM_Obj *pNext = pObj->hNext;

            while (pNext && !(pNext->status & WM_WS_STICK)) {
                pObj = pNext;
                pNext = pNext->hNext;
            }
            pWin->hNext = pObj->hNext;
            pObj->hNext = pWin;
            /* ȡ�������Z����ߵ��Ӵ��� */
            pObj = WM_GetTopChildWindow(pObj);
        }
    }
    /* �������� */
    pWin = WM_GetTopChildWindow(hWin);
    pWin->hNextLine = pObj->hNextLine;
    pObj->hNextLine = hWin;
    GUI_UNLOCK();
}

/* ��һ�����ڴӴ��������Ƴ� */
void WM_RemoveWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin, *pObj;
    
    /* ���ڲ���ΪNULL,�����������Ƴ� */
    if (hWin == NULL || pWin->hParent == NULL) {
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
void WM_DeleteWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin, *pObj, *pFront;

    if (hWin == NULL) { /* hWin����ΪNULL */
        return;
    }
    GUI_LOCK();
    /* ���ڵ��Ĵ�����Ч�� */
    WM_InvalidCoverWindow(pWin, &pWin->rect);
    pFront = WM_GetFrontHandle(pWin);
    WM_RemoveWindow(pWin); /* ���Ƴ����� */
    GUI_ClipWindows(pFront); /* ���¼�������� */
    while (pWin) {
        pObj = pWin;
        pWin = pWin->hNextLine;
        WM_SendMessage(pObj, WM_DELETE, 0);
        GUI_TimerDeleteAtWindow(pObj);
        GUI_DeleteWindowClipArea(pObj);
        GUI_Free(pObj);
    }
    if (GUI_CurrentActiveWindow() == hWin) { /* ��ɾ�����ǻ���� */
        GUI_SetActiveWindow(NULL);
        WM_SetActiveWindow(pFront); /* �������û���� */
        WM_SetWindowFocus(pFront); /* ���ý��㴰�� */
    }
    GUI_UNLOCK();
}

/* ���ô��ڵ����뽹�� */
GUI_RESULT WM_SetWindowFocus(WM_HWIN hWin)
{
    WM_HWIN hFocus;
    WM_MESSAGE Msg;

    if (hWin == NULL) {
        return GUI_ERR;
    }
    GUI_LOCK();
    hFocus = WM_GetWindowFocus(); /* ��ȡ���ڽ��� */
    if (hWin != hFocus) {
        WM_SendMessage(hFocus, WM_KILL_FOCUS, 0); /* ʧȥ�����¼� */
        /* ���õ�ǰ����Ϊ���뽹�� */
        Msg.hWinSrc = hWin;
        Msg.MsgId = WM_SET_FOCUS;
        Msg.Param = 1;
        WM__SendMessage(hWin, &Msg);
    }
    GUI_UNLOCK();
    return GUI_OK;
} 

/* ��ȡ���ڵ����뽹�� */
WM_HWIN WM_GetWindowFocus(void)
{
    WM_MESSAGE Msg;

    Msg.hWin = NULL;
    Msg.MsgId = WM_GET_FOCUS;
    WM__SendMessage(GUI_CurrentActiveWindow(), &Msg);
    return Msg.hWin;
}

/* ���û���� */
GUI_RESULT WM_SetActiveWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin;

    if (hWin == NULL) {
        return GUI_ERR;
    }
    GUI_LOCK();
    if (GUI_CurrentActiveWindow() != pWin) { /* ����ڷ������ */
        WM_SetForegroundWindow(pWin); /* ����Ϊǰ������ */
        WM_SendMessage(GUI_CurrentActiveWindow(), WM_KILL_FOCUS, 0); /* ʧȥ���� */
        if (pWin->hParent) { /* �����ڻ��߸����ڵ�һ���Ӵ��� */
            while (pWin->hParent != gui_rootwin) {
                pWin = pWin->hParent;
            }
        }
        GUI_SetActiveWindow(pWin);
        WM_Invalidate(pWin); /* �������Ч�� */
    }
    GUI_UNLOCK();
    return GUI_OK;
}

/* ��ȡ����ھ�� */
WM_HWIN WM_GetActiveWindow(void)
{
    return GUI_CurrentActiveWindow();
}

/*
 @ ����ǰ������
 @ hWin:Ҫ���õĴ��ھ��
 @ ����ֵ:GUI_OK,����;GUI_ERR,����,û�иô��ڻ򴰿�Ϊ������
 **/
GUI_RESULT WM_SetForegroundWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin;

    if (hWin == NULL || hWin == gui_rootwin) {
        return GUI_ERR;
    }
    GUI_LOCK();
    /* ���ҵ���λ�ڸ������µ����� */
    while (pWin && pWin->hParent != gui_rootwin) {
        pWin = pWin->hParent;
    }
    WM_MoveToTop(pWin);
    GUI_UNLOCK();
    return GUI_OK;
}

/* ��ȡǰ�����ڵľ�� */
WM_HWIN WM_GetForegroundWindow(void)
{
    WM_Obj *pWin = WM_HandleToPtr(gui_rootwin)->hFirstChild;

    if (pWin) {
        GUI_LOCK();
        while (pWin->hNext != NULL) { /* ����ͬ���ڵ� */
            pWin = pWin->hNext;
        }
        GUI_UNLOCK();
    }
    return pWin;
}

/*
 @ ��һ�������ƶ���������µ���ײ�.
 @ ���ô˺�������ʹ����ʧȥ�����(����еĻ�).
 **/
void WM_MoveToBottom(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin, *pParent, *pObj, *pFront;

    if (pWin == NULL || pWin->hParent == NULL) {
        return;
    }
    pParent = pWin->hParent;
    pObj = pParent->hFirstChild;
    if (pObj->status & WM_WS_BACKGND
        || pWin->status & (WM_WS_BACKGND | WM_WS_STICK)) {
        return; /* ������������, �޷��ٰѴ����ƶ�����ײ� */
    }
    GUI_LOCK();
    WM_InvalidCoverWindow(pWin, &pWin->rect); /* ���ڵ��Ĵ�����Ч�� */
    pFront = WM_GetFrontHandle(hWin);
    WM_RemoveWindow(pWin); /* ���Ƴ����� */
    pObj = pParent->hFirstChild;
    /* ���Ӹ����� */
    pParent->hFirstChild = pWin;
    pParent->hNextLine = pWin;
    pWin->hParent = pParent;
    /* �����ֵ� */
    pWin->hNext = pObj;
    pWin = WM_GetTopChildWindow(pWin);
    pWin->hNextLine = pObj;
    GUI_ClipWindows(hWin); /* ���¼�������� */
    WM_InvalidTree(hWin); /* ���ڼ������е��Ӵ�����Ч�� */
    GUI_ClipWindows(pFront); /* ���¼�������� */
    WM_InvalidTree(pWin); /* ֮ǰ���ڵ��Ĵ������¼�������� */
    GUI_UNLOCK();
}

/* 
 @ ��һ�������ƶ����������µ����
 @ ���ô˺�������ı䴰��ԭ�еĻ����.
 **/
void WM_MoveToTop(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin, *pParent;

    if (hWin == NULL || hWin == gui_rootwin) {
        return;
    }
    GUI_LOCK();
    if (pWin->hNext
        && !(pWin->status & (WM_WS_STICK | WM_WS_BACKGND))) {
        pParent = pWin->hParent;
        WM_RemoveWindow(pWin); /* ���Ƴ����� */
        WM_AttachWindow(pWin, pParent); /* ���봰�ڵ���� */
        GUI_ClipWindows(pWin); /* ���¼�������� */
        WM_InvalidTree(pWin); /* ���ڼ������е��Ӵ�����Ч�� */
    }
    GUI_UNLOCK();
}

/* �������ö�, ͬʱ���ô��ڱ�Ϊ����� */
void WM_SetStickWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin;

    if (hWin == NULL && pWin->status & WM_WS_STICK) {
        return;
    }
    GUI_LOCK();
    WM_MoveToTop(pWin);
    pWin->status |= WM_WS_STICK;
    WM_SetActiveWindow(pWin);
    GUI_UNLOCK();
}

/* ȡ���ö����� */
void WM_ResetStickWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin;

    if (hWin == NULL) {
        return;
    }
    GUI_LOCK();
    pWin->status &= ~WM_WS_STICK;
    WM_MoveToTop(pWin);
    GUI_UNLOCK();
}

/*
 @ ����������Ϊ��������, ���ڱ����Ǹ����ڵ�һ���Ӵ���. ÿһ��������
 @ ���ֻ����һ����������, ���ñ��������滻�Ѿ����ڵı�������(�������).
 @ �������ڽ�һֱ����ͬ���𴰿ڵĵײ�. 
 **/
void WM_SetBackgroundWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin, *pParent, *pObj;

    if (!pWin || !pWin->hParent) {
        return;
    }
    GUI_LOCK();
    pWin->status &= ~WM_WS_BACKGND;
    pParent = pWin->hParent;
    pObj = pParent->hFirstChild;
    if (pObj->status & WM_WS_BACKGND) {
        pObj->status &= ~WM_WS_BACKGND;
    }
    WM_MoveToBottom(pWin);
    pWin->status |= WM_WS_BACKGND;
    GUI_UNLOCK();
}

/*
 @ ����һ��������Ϊָ�����ڵ��Ӵ���
 @ ��hParentΪNULLʱ,���ڽ���RootWindow���Ӵ���
 **/
WM_HWIN WM_CreateWindowAsChild(int x0,             /* �M���� */
                               int y0,             /* ������ */
                               int xSize,          /* ��� */
                               int ySize,          /* �߶� */
                               WM_HWIN hParent,     /* �����ھ�� */
                               int Style,          /* ���ڷ�� */
                               int Id,             /* ����ID */
                               WM_CALLBACK *WinCb,  /* ���ڹ��ܻص����� */
                               u_32 bytes)          /* ���ڽṹ�������ֽ� */
{
    WM_Obj *pObj, *pParent = hParent;

    GUI_LOCK();
    /* ��鴰��ID�Ƿ��Ѿ�ע�� */
    pObj = WM_GetWindowHandle(Id);
    if (pObj != NULL) {
        WM_SetActiveWindow(pObj); /* ���Ѿ�ע��Ĵ��ڷŵ���ǰ */
        WM_SetWindowFocus(pObj); /* ���ô��ڽ��� */
        GUI_UNLOCK();
        return NULL;
    }
    pObj = GUI_Malloc(sizeof(WM_Obj) + bytes);
    if (pObj == NULL) {
        GUI_UNLOCK();
        return NULL;
    }
    pObj->hFirstChild = NULL;
    pObj->hNextLine = NULL;
    pObj->status = Style;
    pObj->id = Id;
    pObj->winCb = WinCb;
    if (pParent) {
        x0 += pParent->rect.x0;
        y0 += pParent->rect.y0;
    }
    pObj->rect.x0 = x0;
    pObj->rect.y0 = y0;
    pObj->rect.x1 = x0 + xSize - 1;
    pObj->rect.y1 = y0 + ySize - 1;
    WM_AttachWindow(pObj, pParent); /* ע�ᵽ������ */
    GUI_ClipNewWindow(pObj); /* ���¼����� */
    WM_SetActiveWindow(pObj); /* ����Ϊ����� */
    WM_Invalidate(pObj);
    if (Style & WM_WS_STICK) {
        WM_SetStickWindow(pObj);
    } else if (Style & WM_WS_BACKGND) {
        WM_SetBackgroundWindow(pObj);
    }
    GUI_UNLOCK();
    return pObj;
}

/* ͸��������Ч�� */
static void _InvalidateTrans(WM_Obj *pWin, GUI_RECT *pr)
{
    int Status;
    WM_Obj *pParent = pWin->hParent;
    WM_Obj *pStop = pWin;
    GUI_RECT r, ri;

    if (pParent == NULL) {
        return;
    }
    Status = pParent->status;
    if (GUI_RectOverlay(&r, pr, &pParent->rect) == FALSE) {
        return;
    }
    if (Status & WM_WS_TRANS) {
        _InvalidateTrans(pParent, pr); /* ��������͸������ */
    } else {
        _Invalidate1Abs(pParent, pr); /* �ȰѸ�������Ч�� */
    }
    /* ���������е��Ӵ�����Ч�� */
    for (pWin = pParent->hFirstChild; pWin && pWin != pStop;) {
        ri = r;
        if (WM__ClipAtParent(&ri, pWin)) {
            if (!(pWin->status & WM_WS_TRANS)) { /* ͸�����ڲ��� */
                _Invalidate1Abs(pWin, &ri);
            }
            pWin = pWin->hNextLine;
        } else { /* �ô�������Ч�����ཻ��������������Ӵ��� */
            if (pWin->hNext) {
                pWin = pWin->hNext;
            } else {
                pWin = pWin->hNextLine;
            }
        }
    }
}

/* ���ڲ���������Ч�� */
void WM_InvalidateRect(WM_HWIN hWin, GUI_RECT *pRect)
{
    WM_Obj *pWin;
    GUI_RECT r;

    if (hWin) {
        pWin = hWin;
        GUI_LOCK();
        if (pRect == NULL) {
            r = pWin->rect;
        } else {
            r = *pRect;
        }
        if (WM__ClipAtParent(&r, pWin)) { /* ��Ч����ü�Ϊ���ڿɼ����� */
            if (pWin->status & WM_WS_TRANS) {
                _InvalidateTrans(pWin, &r);
            } else {
                _Invalidate1Abs(pWin, &r);
            }
        }
        GUI_UNLOCK();
    }
}

/* ����������Ч�� */
void WM_Invalidate(WM_HWIN hWin)
{
    WM_InvalidateRect(hWin, NULL);
}

/* ��һ�����ڼ����Ӵ�����Ч�� */
GUI_RESULT WM_InvalidTree(WM_HWIN hWin)
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

/* ��ȡ���ڵĳߴ� */
GUI_RECT * WM_GetWindowRect(WM_HWIN hWin)
{
    return &WM_HandleToPtr(hWin)->rect;
}

/* ��ȡ������Ч���� */
GUI_RECT * WM_GetWindowInvalidRect(WM_HWIN hWin)
{
    return &WM_HandleToPtr(hWin)->invalidRect;
}

/*
 @ �ڴ�������Ѱ��һ������
 @ ����ֵ:0,û���������;1,���������
 @ �ⲿ����
 **/
GUI_RESULT WM_FindWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = gui_rootwin;
    
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
WM_HWIN WM_GetDialogItem(WM_HWIN hWin, int Id)
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
        while (pWin && pWin->id != Id && pWin != hWin) {
            pWin = pWin->hNextLine;
        }
    } else {
        pWin = NULL;
    }
    GUI_UNLOCK();
    return pWin;
}

/* ���ָ��ID�Ĵ��ھ�� */
WM_HWIN WM_GetWindowHandle(int Id)
{
    WM_Obj *pWin = gui_rootwin;

    /* WM_NULL_ID����Ѱ�� */
    if (Id == WM_NULL_ID) {
        return NULL;
    }
    GUI_LOCK();
    while (pWin && pWin->id != Id) { /* �������� */
        pWin = pWin->hNextLine;
    }
    GUI_UNLOCK();
    return pWin;
}

/* ��ȡ�ؼ�ID */
int WM_GetDialogId(WM_HWIN hWin)
{
    if (hWin) {
        return WM_HandleToPtr(hWin)->id;
    }
    return 0;
}

/*
 @ ��������������±�¶�Ĵ���(��ָ������ѡ�еĴ���).
 @ ����ֵ:ѡ�д��ڵľ��.
 **/
WM_HWIN WM_GetExposedWindow(int x, int y)
{
    WM_Obj *p1 = gui_rootwin, *p2 = NULL, *pWin = NULL;
    GUI_RECT r, rParent = p1->rect; /* �����ڵľ��� */

    GUI_LOCK(); 
    /* �ҵ�ͬ������Z����ߵ�ѡ�д��ڣ�Ȼ���������Ƿ��к��ӻ������Ƿ���ѡ�д��ڣ�
       �����û�к��ӻ�����û��ѡ�д����򷵻�������ڣ���������򴰿����Ѱ��. */
    do {
        while (p1) { /* ����ͬ������ */
            /* ���㴰���ڱ����ȴ��ڲü�֮��ľ��Σ���¶���Σ���
               Ȼ������������Ƿ���������ѡ��. */
            if (GUI_RectOverlay(&r, &p1->rect, &rParent)) {
                if (GUI_CheckPointAtRect(x, y, &r)) {
                    p2 = p1; /* ��¼ѡ�д��� */
                }
            }
            p1 = p1->hNext;
        } /* ѭ������֮��p2��ΪZ����ߵı�ѡ�д��� */
        if (p2 == pWin) { /* �Ӵ���û��û��ѡ�д��� */
            break;
        }
        pWin = p2; /* ��¼ĿǰZ����ߵ�ѡ�д��� */
        p1 = pWin->hFirstChild; /* ���������ѡ�д��ڵ��Ӵ��� */
        /* ��һ�����ڵı�¶�����õ�ǰѡ�д��ڵı�¶���������� */
        GUI_RectOverlay(&rParent, &rParent, &pWin->rect);
    } while (p1); /* ѡ�д���û���Ӵ��ڣ�����ѭ�� */
    GUI_UNLOCK();
    return pWin;
}

/* ��ȡ�����ھ�� */
WM_HWIN WM_GetParentHandle(WM_HWIN hWin)
{
    return WM_HandleToPtr(hWin)->hParent;
}

/* ��ȡĳ�����ڵ����洰�� */
WM_HWIN WM_GetDsektopWindow(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin;

    if (pWin == NULL || pWin->hParent == NULL) {
        return pWin;
    }
    while (pWin && pWin->hParent != gui_rootwin) {
        pWin = pWin->hParent;
    }
    return pWin;
}

/* ��ȡ���ڵ�ǰһ���ڵ� */
WM_HWIN WM_GetFrontHandle(WM_HWIN hWin)
{
    WM_Obj *pObj = hWin;

    GUI_LOCK();
    pObj = pObj->hParent;
    if (pObj && pObj->hFirstChild != hWin) {
        pObj = pObj->hFirstChild;
        while (pObj->hNext != hWin) {
            pObj = pObj->hNext;
        }
    }
    GUI_UNLOCK();
    return pObj;
}

/* ����һ�������ڸǵĴ��ڼ����Ӵ�����Ч����
 @ ������ɾ������ʱ��Ҫ�ѽ�Ҫ��ɾ���Ĵ����ڵ��Ĵ�����Ч�� 
 **/
void WM_InvalidCoverWindow(WM_HWIN hWin, GUI_RECT *pRect)
{
    WM_Obj *p = hWin;

    /* hWin����ΪNULL,Ҳ������RootWindow */
    if (p == NULL && p->hParent == NULL) { 
        return;
    }
    /* �վ���ֱ�ӷ��� */
    if (GUI_CheckRectNull(pRect) == FALSE) {
        return;
    }
    GUI_LOCK();
    /*����������ֻ���ڸǵ����ĸ�����*/
    for (p = p->hParent;  p && p != hWin; p = p->hNextLine) {
            WM_InvalidateRect(p, pRect);
    }
    GUI_UNLOCK();
}

/* �ƶ����ڼ������е��Ӵ��� */
void WM_MoveWindow(WM_HWIN hWin, int dx, int dy)
{
    WM_Obj *p = hWin, *pEnd;
    
    GUI_LOCK();
    if (p && p->status & WM_WS_MOVE && (dx || dy)) {
        /* �Ƚ����ڵ��Ĵ�����Ч�� */
        WM_InvalidCoverWindow(p, &p->rect);
        /* �ҵ������Ӵ��ڵ��յ� */
        pEnd = WM__FindChildEnd(p);
        for (; p != pEnd; p = p->hNextLine) { /* �����Ӵ��� */
            GUI_MoveRect(&p->rect, dx, dy);
            WM_Invalidate(p); /* ������Ч�� */
        }
    }
    GUI_ClipWindows(hWin);
    GUI_UNLOCK();
}

/* ���ÿ��ƶ��Ĵ��� */
void WM_SetMoveWindow(WM_HWIN hWin)
{
    WM_HandleToPtr(hWin)->status |= WM_WS_MOVE;
}

/* ��ȡ���ڿͻ������ */
WM_HWIN WM_GetClientWindow(WM_HWIN hWin)
{
    GUI_MESSAGE Msg;

    Msg.MsgId = WM_GET_CLIENT;
    Msg.Param = 0;
    WM__SendMessage(hWin, &Msg);
    return (WM_HWIN)(Msg.Param);
}

/* ��ȡ���ڿͻ������� */
GUI_RECT * WM_GetClientRect(WM_HWIN hWin)
{
    WM_Obj *pClient;

    pClient = WM_GetClientWindow(hWin);
    return &pClient->rect;
}

/* GUI����Ĭ�ϴ����� */
GUI_BOOL WM_DefaultKeyProc(GUI_MESSAGE *pMsg)
{
    if (pMsg->MsgId == WM_KEYDOWN) {
        switch (pMsg->Param) {
        case KEY_TAB: /* TAB���л����� */
            pMsg->MsgId = WM_SET_FOCUS;
            pMsg->Param = 0;
            WM_SendMessage(pMsg->hWin, WM_SET_FOCUS, 0);
            break;
        default:
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/* Ĭ����Ϣ������ */
void WM_DefaultProc(GUI_MESSAGE *pMsg)
{
    /* ������Ϣ */
    switch (pMsg->MsgId) {
    case WM_TP_CHECKED:
        WM_SetActiveWindow(pMsg->hWin); /* ����Ϊ����� */
        WM_SetWindowFocus(pMsg->hWin); /* ����Ϊ���㴰�� */
        break;
    case WM_TP_PRESS: /* �ƶ����� */
        WM_MoveWindow(pMsg->hWin,
            ((GUI_POINT*)pMsg->Param)[1].x,
            ((GUI_POINT*)pMsg->Param)[1].y);
        break;
    case WM_TP_LEAVE: /* �ƶ����� */
        WM_MoveWindow(pMsg->hWin,
            ((GUI_POINT*)pMsg->Param)[1].x,
            ((GUI_POINT*)pMsg->Param)[1].y);
        break;
    case WM_SET_FOCUS: /* �������뽹�� */
        WM_SendMessageToParent(pMsg->hWin, pMsg);
        break;
    case WM_GET_FOCUS: /* ��ȡ���뽹�� */
        WM_SendMessageToParent(pMsg->hWin, pMsg);
        break;
    case WM_KILL_FOCUS:
        WM_Invalidate(pMsg->hWin);
        break;
    case WM_KEYDOWN:
        if (pMsg->Param == KEY_TAB) { /* TAB���л����� */
            pMsg->MsgId = WM_SET_FOCUS;
            pMsg->Param = 0;
            WM__SendMessage(pMsg->hWin, pMsg);
        }
        break;
    default:
        break;
    }
}

/* ��һ�����ڵ���������Ϊ͸�� */
void WM_SetTransWindow(WM_HWIN hWin, u_8 Status)
{
    WM_Obj *pWin = hWin;

    if (hWin) {
        if (Status) {
            if (!(pWin->status & WM_WS_TRANS)) {
                pWin->status |= WM_WS_TRANS;
                GUI_WindowClipArea(pWin); /* ֮ǰ�Ƿ�͸�����ڵ����¼�������� */
            }
        } else {
            if (pWin->status & WM_WS_TRANS) {
                pWin->status &= ~WM_WS_TRANS;
                GUI_WindowClipArea(pWin); /* ֮ǰ��͸�����ڵ����¼�������� */
            }
        }
        WM_Invalidate(hWin); /* ������Ч�� */
    }
}
