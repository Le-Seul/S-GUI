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

/* ���ڹ�������ʼ�� */
GUI_RESULT WM_Init(void)
{
    GUI_HWIN *p = &_hRootWin;
    
    *p = GUI_fastmalloc(sizeof(WM_Obj));
    if (*p == NULL) {
        return GUI_ERR;
    }
    WM_RootWindowInit(*p);
    return GUI_OK;
}

/*
* �򴰿ڹ�������ָ�����ڷ�����Ϣ��
* hWin:Ҫָ�����ڵľ��,ΪNULLʱ��ֱ�ӷ��ء�
**/
static void WM__SendMessage(WM_HWIN hWin, WM_MESSAGE *pMsg)
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
#if GUI_DEBUG_MODE
    else {
        GUI_DEBUG_OUT("GUI message nesting exceeded.");
    }
#endif
    GUI_UNLOCK();
}

/* �򴰿ڹ�������ָ�����ڷ�����Ϣ */
void WM_SendMessage(WM_HWIN hWin, u_16 MsgId, GUI_PARAM Param)
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
    WM__SendMessage(((WM_Obj*)hWin)->hParent, pMsg);
}

/* ����Ϣ���з�����Ϣ */
GUI_RESULT WM_PostMessage(WM_HWIN hWin, u_16 MsgId, GUI_PARAM Param)
{
    WM_MESSAGE Msg;

    Msg.hWin = hWin;
    Msg.MsgId = MsgId;
    Msg.Param = Param;
    return GUI_PostMessage(&Msg);
}

/* �ü�͸�����ڵ��Ӵ��� */
static RECT_LIST _ClipTransChildren(RECT_LIST L, WM_Obj *pWin)
{
    for (pWin = pWin->hFirstChild; pWin && L; pWin = pWin->hNext) {
        if (pWin->Status & WM_WS_TRANS) {
            L = _ClipTransChildren(L, pWin); /* �ü��Ӵ��� */
        } else {
            L = GUI_ReCalcRectList(L, &pWin->Rect);  /* �ü�͸�����ڵĺ��� */
        }
    }
    return L;
}

/* Ϊһ�����ڼ���ü��������� */
static RECT_LIST WM__ClipWindowArea(WM_HWIN hWin)
{
    WM_Obj *pWin = hWin, *pObj;
    GUI_RECT Rect1;
    RECT_LIST RectList;

    /* �ڿ����ڵ�֮ǰ,���ھ�ֻ��һ���ü����� */
    RectList = GUI_GetRectList(1);
    if (RectList) {
        Rect1 = WM_GetTaliorInvalidRect(pWin);
        RectList->Rect = Rect1;
    }
    /* ���ڻᱻ���Ķ����ǻ����ұߵ��ֵ���(����еĻ�)�ü�,Ҳ�����ڵ����������ĺ��Ӻ��ֵ��ǣ�
    ������㴰�ڵĲü��������������ܵõ�������ڱ������ڵ���Ĳü���������.
    */
    if (pWin->hFirstChild) {
        /* �ȱ����Ӵ��� */
        pObj = pWin->hFirstChild;
        while (pObj && RectList) {
            /* �������ͨ���ھ�ֱ�Ӽ���ü��������͸�����ھ������ĺ���������ü� */
            if (pObj->Status & WM_WS_TRANS) {
                RectList = _ClipTransChildren(RectList, pObj);
            } else {
                RectList = GUI_ReCalcRectList(RectList, &pObj->Rect);
            }
            pObj = pObj->hNext; /* ���ұ��� */
        }
    }
    /* �ٱ������ұߵ�ͬ�����ڼ����ȵ�ͬ������ */
    pObj = pWin;
    while (pObj != _pRootWin && RectList) {
        while (pObj->hNext && RectList) {
            pObj = pObj->hNext; /* ���ұ��� */
                                /* �������ͨ���ھ�ֱ�Ӽ���ü��������͸�����ھ������ĺ���������ü� */
            if (pObj->Status & WM_WS_TRANS) {
                RectList = _ClipTransChildren(RectList, pObj);
            } else {
                RectList = GUI_ReCalcRectList(RectList, &pObj->Rect);
            }
        }
        pObj = pObj->hParent; /* ���ϱ��� */
    }
    return RectList;
}

/*
* ��һ�����βü������ڵĿɼ������С�
* ����ֵ:FALSE,�þ����봰�ڵĿɼ������ཻ��
*       TRUE,�ü��ɹ���
**/
static GUI_BOOL WM__ClipAtParent(GUI_RECT *pr, WM_Obj *pWin)
{
    for (; pWin; pWin = pWin->hParent) {
        if (GUI_RectOverlay(pr, pr, &pWin->Rect) == FALSE) {
            return FALSE;
        }
    }
    return TRUE;
}

/* ���ڲ���������Ч�����ڲ����� */
static void _Invalidate1Abs(WM_Obj *pWin, GUI_RECT *pr)
{
    if (pWin->Status & WM_WS_INVAILD) {
        GUI_RectSum(&pWin->InvalidRect, &pWin->InvalidRect, pr);
    } else {
        pWin->InvalidRect = *pr;
        pWin->Status |= WM_WS_INVAILD;
        ++__InvalidWindowNum;
    }
}

/* �ػ�һ������ */
static void _PaintOne(WM_Obj *pWin)
{
    RECT_LIST RectList;

    /* ���㴰�ڲü�����,������Ϊ��ǰ�Ļ������� */
    RectList = WM__ClipWindowArea(pWin);
    if (RectList != NULL) {
        /* �ػ洰�� */
        GUI_SetNowRectList(RectList, &pWin->InvalidRect);
        WM_SendMessage(pWin, WM_PAINT, 0);
        GUI_FreeRectList(RectList);
#if GUI_USE_MEMORY
        /* ��Ҫˢ�µľ��� */
        __PaintArea = GUI_RectOrCalc(&pWin->Rect, &__PaintArea);
#endif
    }
}

/* ���ƴ��ڵ�͸���Ӵ��� */
static void _PaintTransChild(WM_Obj *pWin)
{
    GUI_RECT r = pWin->InvalidRect;

    for (pWin = pWin->hFirstChild; pWin; pWin = pWin->hNext) {
        if ((pWin->Status & WM_WS_TRANS)
            && GUI_RectOverlay(&pWin->InvalidRect, &pWin->Rect, &r)) {
            _PaintOne(pWin);
            _PaintTransChild(pWin); /* �ڻ�������͸���Ӵ��� */
        }
    }
}

/* ���ƴ���֮�ϵ�����͸������ */
static void _PaintTransTop(WM_Obj *pWin)
{
    WM_Obj *pParent;
    GUI_RECT r = pWin->InvalidRect;

    for (pParent = pWin; pParent; pParent = pParent->hParent) {
        for (pWin = pParent->hNext; pWin; pWin = pWin->hNext) {
            if ((pWin->Status & WM_WS_TRANS)
                && GUI_RectOverlay(&pWin->InvalidRect, &pWin->Rect, &r)) {
                _PaintOne(pWin);
                _PaintTransChild(pWin);
            }
        }
    }
}

/* �ػ����д��� */
static void _PaintAll(void)
{
    WM_Obj *pWin = _pRootWin;

    if (__InvalidWindowNum) {
        /* �������ػ洰�� */
        while (__InvalidWindowNum && pWin) {
            GUI_LOCK();
            if (pWin->Status & WM_WS_INVAILD) { /* ������Ҫ�ػ� */
                _PaintOne(pWin);
                pWin->Status &= ~(WM_WS_INVAILD); /* ���������Ч��־ */
                --__InvalidWindowNum;
                _PaintTransChild(pWin);
                _PaintTransTop(pWin);
            }
            GUI_UNLOCK();
            pWin = pWin->hNextLine;
        }
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
 * ��ȡ������Ч�������С
 * -�ú���ͨ����Ŀ�괰�ڵ��������е����ȴ��ڵ��û���ȡ�����õ���Ч����
 * -hWin������NULL
 **/
GUI_RECT WM_GetWindowAreaRect(WM_HWIN hWin)
{
    GUI_RECT Rect;

    GUI_LOCK();
    Rect = ((WM_Obj*)hWin)->Rect;
    while (((WM_Obj*)hWin) != _pRootWin) {
        hWin = ((WM_Obj*)hWin)->hParent;
        GUI_RectOverlay(&Rect, &Rect, &((WM_Obj*)hWin)->Rect);
    }
    GUI_UNLOCK();
    return Rect;
}

/* ��òü���Ĵ�����Ч�����С��hWin������NULL */
GUI_RECT WM_GetTaliorInvalidRect(WM_HWIN hWin)
{
    GUI_RECT Rect;

    GUI_LOCK();
    Rect = WM_GetWindowAreaRect(hWin);
    Rect = GUI_RectAndCalc(&Rect, &((WM_Obj*)hWin)->InvalidRect);
    GUI_UNLOCK();
    return Rect;
}

/* ��ñ�ĳ������Z��С1�Ĵ��� */
WM_HWIN WM_GetFrontWindow(WM_HWIN hWin)
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

/* ��һ��������ӵ��������µ���� */
void WM_AttachWindow(WM_HWIN hWin, WM_HWIN hParent)
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
void WM_RemoveWindow(WM_HWIN hWin)
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
void WM_DeleteWindow(WM_HWIN hWin)
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
        GUI_fastfree(hWin); /* �ͷſռ� */
    }
    GUI_UNLOCK();
}

/* ��ȡ��ǰ����ڵľ�� */
WM_HWIN WM_GetActiveMainWindow(void)
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
GUI_RESULT WM_SetActiveMainWindow(WM_HWIN hWin)
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
WM_HWIN WM_CreateWindowAsChild(i_16 x0,             /* �M���� */
                               i_16 y0,             /* ������ */
                               u_16 xSize,          /* ��� */
                               u_16 ySize,          /* �߶� */
                               WM_HWIN hParent,     /* �����ھ�� */
                               u_16 Style,          /* ���ڷ�� */
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
    pObj->hNextLine = NULL;
    WM_AttachWindow(pObj, pParent); /* ע�ᵽ������ */
    pObj->Status = Style;
    pObj->Sign = Sign;
    pObj->Id = Id;
    pObj->WinCb = WinCb;
    if (pParent) {
        x0 += pParent->Rect.x0;
        y0 += pParent->Rect.y0;
    }
    pObj->Rect.x0 = x0;
    pObj->Rect.y0 = y0;
    pObj->Rect.x1 = x0 + xSize - 1;
    pObj->Rect.y1 = y0 + ySize - 1;
    WM_Invalidate(pObj); /* ��������ʧЧ */
    GUI_UNLOCK();
    return pObj;
}

/* 
 * ��鴰�ڱ�ʶ�� 
 * ����ֵ:0,��ʶ��ƥ��;1,��ʶ����ƥ��
 **/
GUI_RESULT WM_CheckWindowSign(WM_HWIN hWin, u_16 Sign)
{
    if (hWin && ((WM_Obj*)hWin)->Sign == Sign) {
        return GUI_OK;
    }
    return GUI_ERR;
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
    Status = pParent->Status;
    if (GUI_RectOverlay(&r, pr, &pParent->Rect) == FALSE) {
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
            if (!(pWin->Status & WM_WS_TRANS)) { /* ͸�����ڲ��� */
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
            r = pWin->Rect;
        } else {
            r = *pRect;
        }
        if (WM__ClipAtParent(&r, pWin)) { /* ��Ч����ü�Ϊ���ڿɼ����� */
            if (pWin->Status & WM_WS_TRANS) {
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
GUI_RECT WM_GetWindowRect(WM_HWIN hWin)
{
    return ((WM_Obj*)hWin)->Rect;
}

/* ��ȡ������Ч���� */
GUI_RECT WM_GetWindowInvalidRect(WM_HWIN hWin)
{
    return ((WM_Obj*)hWin)->InvalidRect;
}

/*
 * �ڴ�������Ѱ��һ������
 * ����ֵ:0,û���������;1,���������
 * �ⲿ����
 **/
GUI_RESULT WM_FindWindow(WM_HWIN hWin)
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
WM_HWIN WM_GetDialogItem(WM_HWIN hWin, u_16 Id)
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
WM_HWIN WM_GetWindowHandle(u_16 Id)
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
u_16 WM_GetDialogId(WM_HWIN hWin)
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
WM_HWIN WM_GetExposedWindow(u_16 x, u_16 y)
{
    WM_Obj *pWin = NULL, *p;
    GUI_RECT r;

    GUI_LOCK();
    for (p = _pRootWin;  p; p = p->hNextLine) {
        r = WM_GetWindowAreaRect(p);
        if (GUI_CheckPointAtRect(x, y, &r)) {
            pWin = p;
        }
    }
    GUI_UNLOCK();
    return pWin;
}

/* ��ȡ�����ھ�� */
WM_HWIN WM_GetParentHandle(WM_HWIN hWin)
{
    return ((WM_Obj*)hWin)->hParent;
}

/* ����һ�������ڸǵĴ��ڼ����Ӵ�����Ч����
 * ������ɾ������ʱ��Ҫ�ѽ�Ҫ��ɾ���Ĵ����ڵ��Ĵ�����Ч�� 
 **/
void WM_InvalidCoverWindow(WM_HWIN hWin, GUI_RECT *pRect)
{
    WM_Obj *p = hWin;

    /* hWin����ΪNULL,Ҳ������RootWindow */
    if (hWin == NULL && p->hParent == NULL) { 
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
void WM_MoveWindow(WM_HWIN hWin, i_16 dX, i_16 dY)
{
    WM_Obj *p = hWin;
    
    GUI_LOCK();
    if (p && p->Status & WM_WS_MOVE && (dX || dY)) {
        /* �Ƚ����ڵ��Ĵ�����Ч�� */
        WM_InvalidCoverWindow(p, &p->Rect);
        /* �ҵ������Ӵ��ڵ��յ� */
        hWin = WM__FindChildEnd(p);
        for (; p != hWin; p = p->hNextLine) { /* �����Ӵ��� */
            GUI_MoveRect(&p->Rect, dX, dY);
            WM_Invalidate(p); /* ������Ч�� */
        }
    }
    GUI_UNLOCK();
}

/* ���ÿ��ƶ��Ĵ��� */
void WM_SetMoveWindow(WM_HWIN hWin)
{
    ((WM_Obj*)hWin)->Status |= WM_WS_MOVE;
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
    return &pClient->Rect;
}

/* ��ȡ */
void WM_DefaultProc(GUI_MESSAGE *pMsg)
{
    WM_Obj *pWin = pMsg->hWin;
    /* Exec message */
    switch (pMsg->MsgId) {
    case WM_DELETE:
        return;
    case WM_TP_CHECKED:
        WM_SetActiveMainWindow(pMsg->hWin);
        return;
    case WM_TP_PRESS: /* �ƶ����� */
        WM_MoveWindow(pMsg->hWin,
            ((GUI_POINT*)pMsg->Param)[1].x,
            ((GUI_POINT*)pMsg->Param)[1].y);
        return;
    case WM_TP_LEAVE: /* �ƶ����� */
        WM_MoveWindow(pMsg->hWin,
            ((GUI_POINT*)pMsg->Param)[1].x,
            ((GUI_POINT*)pMsg->Param)[1].y);
        return;
    }
    /* ����Ĭ�ϲ��� */
    pMsg->Param = 0;
    pMsg->Param = 0;
}
