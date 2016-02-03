#include "GUI_WM.h"
#include "GUI.h"
#include "RootWindow.h"

#if GUI_USE_MEMORY
static GUI_RECT __AllDraw_Rect;
#endif

static void WM__WindowTimer(WM_Obj *pObj);
static void WM__RefreshBackgnd(WM_Obj *pWin, RECT_LIST RectList);

/*
 * ע��,���ڹ������ж��ȫ�ֱ���,�����ʹ�ò���ϵͳʱ
 * Ҫע���̰߳�ȫ��(��������).
 **/

/* ���ڹ�������ʼ�� */
void WM_Init(void)
{
    GUI_hWin *p = &_hRootWin;
    
    *p = GUI_fastmalloc(sizeof(WM_Obj));
    WM_RootWindowInit(*p);
}

/*
 * ����������
 * pTop:������Ĵ���,����������������Ӵ���.
 * pData:����pFunc�Ĳ���,�����ṩ���ͱ�̵Ľӿ�.
 * pFunc:����ʱ���ʵ�ֹ��ܵĺ���,pFunc���ط���ʱ���������᷵��1,
 *      ���߱����������������,������󷵻�0.
 **/
static u_8 WM__TraversalWindows(WM_Obj *pRoot, void *pData, u_8 (*pFunc)(WM_Obj*, void*))
{
    WM_Obj *pWin = pRoot;

    while (1) {
        while (1) {
            if (pFunc(pWin, pData)) { /* ʵ�ֹ��� */
                return 1;
            }
            if (pWin->hFirstChild == NULL) {
                break;
            }
            pWin = pWin->hFirstChild; /* �����ӽڵ� */
        }
        while (pWin->hNext == NULL) {
            pWin = pWin->hParent; /* ���� */
            if (pWin == pRoot) {
                return 0;
            }
        }
        pWin = pWin->hNext; /* ����ͬ���ڵ� */
    }
}

/* ���ڹ������ػ� */
static u_8 WM__WindowRepaint(WM_Obj *pWin, void *pData)
{
    RECT_LIST RectList;

    WM__WindowTimer(pWin); /* ���ڼ�ʱ�� */
    if (WM_CheckInvalid(pWin)) { /* ������Ҫ�ػ� */
        /* ���㴰�ڲü�����,������Ϊ��ǰ�Ļ������� */
        RectList = GUI_CalcWindowRectList(pWin);
        if (RectList != NULL) {
            WM_MESSAGE Msg;
            GUI_RECT Rect;

            /* ͸��������ˢ���ڵ����� */
            if (pWin->Style & WM_WINDOW_TRANS) {
                WM__RefreshBackgnd(pWin, RectList);
            }
            /* �ػ洰�� */
            Msg.MsgId = WM_PAINT;
            Msg.hWin = (WM_hWin)pWin;
            Rect = WM_GetTaliorInvalidRect(pWin);
            GUI_SetNowRectList(RectList, &Rect);
            pWin->WinCb(&Msg);
            GUI_FreeRectList(RectList);
#if GUI_USE_MEMORY
            /* ��Ҫˢ�µľ��� */
            __AllDraw_Rect = GUI_RectOrCalc(&pWin->Rect, &__AllDraw_Rect);
#endif
        }
        WM_CleanInvalid(pWin); /* �����Ч�� */
    }
    return 0;
}

/* ͨ��ִ�лص��ػ���Ч����(���й���) */
void WM_Exec(void)
{
    GUI_Lock();
    WM__TraversalWindows(_pRootWin, NULL, WM__WindowRepaint);
#if GUI_USE_MEMORY
    /* ���ڴ����ݸ��µ�LCD�� */
    __AllDraw_Rect = GUI_RectAndCalc(&_pRootWin->Rect, &__AllDraw_Rect);
    GUI_DrawFromMem(__AllDraw_Rect.x0, __AllDraw_Rect.y0,
                    __AllDraw_Rect.x1 - __AllDraw_Rect.x0 + 1,
                    __AllDraw_Rect.y1 - __AllDraw_Rect.y0 + 1);
    __AllDraw_Rect.x0 = 0;
    __AllDraw_Rect.y0 = 0;
    __AllDraw_Rect.x1 = 0;
    __AllDraw_Rect.y1 = 0;
#endif
    GUI_Unlock();
}

/*
 * ע�⣺����ļ�����Ϣ������ʵ���Ϻ��п��ܹ������̲߳���ȫ�Ļ����
 * ��Ϊ����û��GUI��Դ������ʱ����Ϊ������Щ���������ڴ���ʳ�ͻ��
 **/

/*
 * �򴰿ڹ�������ָ�����ڷ�����Ϣ
 * hWin:Ҫָ�����ڵľ��,ΪNULLʱ�Ὣ��Ϣ��������ǰ�����
 **/
void WM__SendMessage(WM_hWin hWin, WM_MESSAGE *pMsg)
{
    static u_8 __MseeageCount;
    
    if (__MseeageCount < GUI_MAX_MSG_NEST) {
        ++__MseeageCount;
        pMsg->hWin = hWin;
        if (((WM_Obj*)hWin)->WinCb) {
            ((WM_Obj*)hWin)->WinCb(pMsg);
        }
        --__MseeageCount;
    }
}

/* �򴰿ڹ�������ָ�����ڷ��Ͳ������ݵ���Ϣ */
void MW__SendMsgNoData(WM_hWin hWin, u_16 MsgId)
{
    WM_MESSAGE Msg;

    Msg.data = NULL;
    Msg.hWinSrc = NULL;
    Msg.MsgId = MsgId;
    WM__SendMessage(hWin, &Msg);
}

/* �򴰿ڹ�������ָ�����ڷ�����Ϣ */
void WM_SendMessage(WM_hWin hWin, u_16 MsgId, void *data)
{          
    WM_MESSAGE Msg;

    Msg.MsgId = MsgId;
    Msg.data = data;
    WM__SendMessage(hWin, &Msg);
}

/* �򴰿ڹ�������ָ�����ڵĸ����ڷ�����Ϣ */
void WM_PostMessageToParent(WM_hWin hWin, u_16 MsgId, void *data)
{
    WM_MESSAGE Msg;

    Msg.hWinSrc = hWin;
    Msg.MsgId = MsgId;
    Msg.data = data;
    WM__SendMessage(((WM_Obj*)hWin)->hParent, &Msg);
}

/*
 * ��ȡ������Ч�������С
 * -�ú���ͨ����Ŀ�괰�ڵ��������е����ȴ��ڵ��û���ȡ�����õ���Ч����
 **/
GUI_RECT WM_GetWindowAreaRect(WM_hWin hWin)
{
    GUI_RECT Rect;

    Rect = ((WM_Obj*)hWin)->Rect;
    while (((WM_Obj*)hWin) != _pRootWin) {
        hWin = ((WM_Obj*)hWin)->hParent;
        Rect = GUI_RectAndCalc(&((WM_Obj*)hWin)->UserRect,&Rect);
    }
    return Rect;
}

/* ��òü���Ĵ�����Ч�����С */
GUI_RECT WM_GetTaliorInvalidRect(WM_hWin hWin)
{
    GUI_RECT Rect;

    Rect = WM_GetWindowAreaRect(hWin);
    Rect = GUI_RectAndCalc(&Rect, &((WM_Obj*)hWin)->InvalidRect);
    return Rect;
}

/* �򴰿��������һ������ */
void WM_AttachWindow(WM_hWin hWin, WM_hWin hParent)
{
    WM_Obj *pObj,*pParent;
    if (hParent == NULL) {
        pParent = _pRootWin;
    } else {
        pParent = hParent;
    }
    pObj = hWin;
    pObj->hNext = NULL;
    pObj->hParent = pParent;
    if (pParent->hFirstChild == NULL) {
        pParent->hFirstChild = pObj;
    } else {
        WM_Obj *pWin = pParent->hFirstChild;
        while (pWin->hNext) {
            pWin = pWin->hNext;
        }
        pWin->hNext = pObj;
    }
}

/* ��һ�����ڴӴ��������Ƴ� */
static void WM__RemoveWindow(WM_hWin hWin)
{
    WM_Obj *pWin, *pt;
    
    pWin = hWin;
    pt = pWin->hParent;
    if (pt == NULL) { /* ������,�����Ƴ� */
        return;
    }
    if (pt->hFirstChild == pWin) {
        pt->hFirstChild = pWin->hNext;
    } else {
        pt = pt->hFirstChild;
        while (pt->hNext != pWin) {
            pt = pt->hNext;
        }
        pt->hNext = pWin->hNext;
    }
    pWin->hParent = NULL;
    pWin->hNext = NULL; /* �����ڵ� */
}

/*
 * ɾ��һ������,������ڱ����Ѿ����봰����(�����ٲ�����ͬ���ڵ�) 
 * �ڲ�����
 **/
static void WM__DeleteWindow(WM_hWin hWin)
{
    WM_MESSAGE Msg;
    WM_Obj *pObj = NULL, *pWin = hWin; /* ע�������pObj */

    if (hWin == NULL) {
        return;
    }
    while (1) {
        while (pWin->hFirstChild) { /* ֱ������Ľڵ� */
            pWin = pWin->hFirstChild;
        }
        if (pWin->hNext == NULL) { /* pWinû��ͬ���ڵ� */
            hWin = pWin;
            pWin = pWin->hParent; /* ���ص����ڵ� */
            if (pWin == NULL) { /* �������� */
                break;
            }
            if (pWin->hFirstChild == hWin) { /* hWin��pWin�ĵ�һ���ӽڵ� */
                pWin->hFirstChild = NULL; /* ���hWin�븸�ڵ�Ĺ��� */
            } else { /* hWin����pWin�ĵ�һ���ڵ� */
                pObj->hNext = NULL; /* ���hWin��ǰ���ͬ���ڵ�Ĺ��� */
            }
            Msg.hWin = hWin;
            Msg.MsgId = WM_DELETE;
            ((WM_Obj*)hWin)->WinCb(&Msg); /* ���ûص�����ɾ���ڵ� */
        } else { /* ����ͬ���ڵ� */
            pObj = pWin; /* pObj��������һ��ͬ���ڵ�ĵ����ڶ����ڵ� */
            pWin = pWin->hNext;
        }
    }
}

/* ɾ������ */
void WM_DeleteWindow(WM_hWin hWin)
{    
    GUI_Lock();
    /* ���ڵ��Ĵ�����Ч�� */
    WM_InvalidCoverWindow(hWin, &((WM_Obj*)hWin)->Rect);
    WM__RemoveWindow(hWin); /* ���Ƴ����� */
    WM__DeleteWindow(hWin);
    GUI_Unlock();
}

/* ��ȡ��ǰ����ڵľ�� */
WM_hWin WM_GetActiveMainWindow(void)
{
    WM_Obj *pWin = _RootWin.hFirstChild;
    while (pWin->hNext != NULL) { /* ����ͬ���ڵ� */
        pWin = pWin->hNext;
    }
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
    GUI_Lock();
    while (pObj->hParent != _pRootWin) {
        pObj = pObj->hParent;
    }
    if (WM_GetActiveMainWindow() != pObj) {
        WM__RemoveWindow(pObj); /* ���Ƴ����� */
        WM_AttachWindow(pObj, NULL); /* ���봰�ڵ���� */
        WM_InvalidTree(pObj); /* ���ڼ������е��Ӵ�����Ч�� */
    }
    GUI_Unlock();
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

    /* ��鴰��ID�Ƿ��Ѿ�ע�� */
    pObj = WM_GetDialogItem(hParent, Id);
    if (pObj != NULL) {
        WM_SetActiveMainWindow(pObj);
        return NULL;
    }
    pObj = GUI_fastmalloc(sizeof(WM_Obj) + bytes);
    if (pObj == NULL) {
        return NULL;
    }
    GUI_Lock();
    WM_AttachWindow(pObj, pParent); /* ע�ᵽ������ */
    pObj->hFirstChild = NULL;
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
    GUI_Unlock();
    return pObj;
}

/* 
 * ��鴰�ڱ�ʶ�� 
 * ����ֵ:0,��ʶ��ƥ��;1,��ʶ����ƥ��
 **/
u_8 WM_CheckWindowSign(WM_hWin hWin, u_16 Sign)
{
    if (((WM_Obj*)hWin)->Sign == Sign) {
        return 0;
    }
    return 1;
}

/* ���ڲ���������Ч�� */
void WM_InvalidateRect(WM_hWin hWin, GUI_RECT *pRect)
{
    WM_Obj* pWin = hWin;

    if (pRect == NULL) {
        pWin->InvalidRect = pWin->Rect;
    } else { /* ����� */
        if (pWin->InvalidRect.x0 > pRect->x0) {
            pWin->InvalidRect.x0 = pRect->x0;
        }
        if (pWin->InvalidRect.y0 > pRect->y0) {
            pWin->InvalidRect.y0 = pRect->y0;
        }
        if (pWin->InvalidRect.x1 < pRect->x1) {
            pWin->InvalidRect.x1 = pRect->x1;
        }
        if (pWin->InvalidRect.y1 < pRect->y1) {
            pWin->InvalidRect.y1 = pRect->y1;
        }
    }
}

/* ����������Ч�� */
void WM_Invalidate(WM_hWin hWin)
{
    WM_InvalidateRect(hWin, NULL);
}

/* ��һ�����ڼ����Ӵ�����Ч��,�ڲ��ص����� */
static u_8 WM__InvalidTree(WM_Obj *pWin, void *p)
{
    WM_InvalidateRect(pWin, NULL);
    return 0;
}

/* ��һ�����ڼ����Ӵ�����Ч��,�ⲿ����,������ */
GUI_RESULT WM_InvalidTree(WM_hWin hWin)
{
    WM__TraversalWindows(hWin, NULL, WM__InvalidTree);
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

    /* x0,y0����Ϊ��Ļ�ߴ��ֵ */
    pWin->InvalidRect.x0 = GUI_GetScreenWidth();
    pWin->InvalidRect.y0 = GUI_GetScreenHeight();
    pWin->InvalidRect.x1 = 0;
    pWin->InvalidRect.y1 = 0;
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
 * �ڲ�����
 **/
static u_8 WM__FindWindow(WM_Obj *pWin, void *fWin)
{
    if (fWin == pWin) {
        return 1;
    }
    return 0;
}

/*
 * �ڴ�������Ѱ��һ������
 * ����ֵ:0,û���������;1,���������
 * �ⲿ����
 **/
u_8 WM_FindWindow(WM_hWin hWin)
{
    u_8 Res;
    
    GUI_Lock();
    Res = WM__TraversalWindows(_pRootWin, hWin, WM__FindWindow);
    GUI_Unlock();
    return Res;
}

struct _DLG_ITEM {
    WM_Obj* pObj;
    u_16 Id;
};

static u_8 WM__GetDialogItem(WM_Obj *pWin, void *pData)
{
    if (pWin->Id == ((struct _DLG_ITEM*)pData)->Id) {
        ((struct _DLG_ITEM*)pData)->pObj = pWin;
        return 1;
    }
        return 0;
}

/* �õ�ID��ΪId�Ĵ��ڳ�Ա��� */
WM_hWin WM_GetDialogItem(WM_hWin hWin, u_16 Id)
{
    struct _DLG_ITEM data;

    if (hWin == NULL) {
        hWin = _pRootWin;
    }
    if (((WM_Obj*)hWin)->hFirstChild == NULL) {
        return NULL;
    }
    GUI_Lock();
    data.Id = Id;
    data.pObj = NULL;
    WM__TraversalWindows(hWin, &data, WM__GetDialogItem);
    GUI_Unlock();
    return data.pObj;
}

/* ���ָ��ID���Ӵ��ھ��(�������ﴰ��...) */
WM_hWin WM_GetChildItem(WM_hWin hWin, u_16 Id)
{
    WM_Obj *pObj = hWin;
    
    if (pObj == NULL) {
        pObj = _pRootWin;
    }
    pObj = pObj->hFirstChild;
    if (pObj == NULL) {
        return NULL;
    }
    do{
        if (pObj->Id == Id) {
            break;
        }
        pObj = pObj->hNext;
    } while(pObj);
    return pObj;
}

/* ��ȡ�ؼ�ID */
u_16 WM_GetDialogId(WM_hWin hWin)
{
    if (hWin) {
        return ((WM_Obj*)(hWin))->Id;
    }
    return 0;
}

/* ��������������±�¶�Ĵ��� */
struct _EXPOS_WIN {
    u_16 x, y;
    WM_hWin fWin;
    GUI_RECT tRect;
};

/*
 * ��������������±�¶�Ĵ���
 * �ڲ�����
 **/
static u_8 WM__GetExposedWindow(WM_Obj *pObj, void *pData)
{    
    ((struct _EXPOS_WIN *)pData)->tRect = WM_GetWindowAreaRect(pObj);
    if (GUI_CheckPointAtRect(((struct _EXPOS_WIN *)pData)->x,
                             ((struct _EXPOS_WIN *)pData)->y,
                             &((struct _EXPOS_WIN *)pData)->tRect)) {
        ((struct _EXPOS_WIN *)pData)->fWin = pObj;
    }
    return 0;
}

/*
* ��������������±�¶�Ĵ���
* ����ֵ:Ŀ�괰�ڵľ��
* �ⲿ����
**/
WM_hWin WM_GetExposedWindow(u_16 x, u_16 y)
{
    struct _EXPOS_WIN data;
    
    GUI_Lock();
    data.x = x;
    data.y = y;
    data.fWin = NULL;
    WM__TraversalWindows(_pRootWin, &data, WM__GetExposedWindow);
    GUI_Unlock();
    return data.fWin;
}

/* ��ȡ�����ھ�� */
WM_hWin WM_GetParentHandle(WM_hWin hWin)
{
    return ((WM_Obj*)hWin)->hParent;
}

/* ����ṹ�����ڰ�װ���溯������������ */
struct _INV_DATA {
    GUI_RECT *pRect;
    WM_hWin hWin;
};

/* ����һ�������ڸǵĴ�����Ч��,�ڲ��ص����� */
static u_8 WM__InvalidCoverWindow(WM_Obj *pWin, void *pData)
{
    GUI_RECT Rect;

    if (pWin == ((struct _INV_DATA*)pData)->hWin) {
        return 1;
    }
    if (GUI_RectOverlay(&Rect, &pWin->Rect,
        ((struct _INV_DATA*)pData)->pRect) == GUI_OK) {
        WM_InvalidateRect(pWin, &Rect);
    }
    return 0;
}

/* ����һ�������ڸǵĴ�����Ч����
 * ������ɾ������ʱ��Ҫ�ѽ�Ҫ��ɾ���Ĵ����ڵ��Ĵ�����Ч�� 
 **/
void WM_InvalidCoverWindow(WM_hWin hWin, GUI_RECT *pRect)
{
    struct _INV_DATA data;

    data.hWin = hWin;
    data.pRect = pRect;
    WM__TraversalWindows(_pRootWin, &data, WM__InvalidCoverWindow);
}

/* �ƶ����ڻص����� */
static u_8 WM__MoveWindow(WM_Obj *pObj, void *pData)
{
    GUI_RECT *Rect;


    Rect = &pObj->Rect;
    GUI_MoveRect(Rect, ((i_16*)pData)[0],((i_16*)pData)[1]);
    Rect = &pObj->UserRect;
    GUI_MoveRect(Rect, ((i_16*)pData)[0],((i_16*)pData)[1]);
    WM_Invalidate(pObj); /* ������Ч�� */
    return 0;
}

/* �ƶ������ⲿ����API */
void WM_MoveWindow(WM_hWin hWin, i_16 dX, i_16 dY)
{
    i_16 xy[2];
    
    if (!(((WM_Obj*)hWin)->Style & WM_WINDOW_MOVE)) {
        return; /* �����ƶ��Ĵ��� */
    }
    if (dX == 0 && dY == 0) {
        return;
    }
    GUI_Lock();
    xy[0] = dX;
    xy[1] = dY;
    /* �Ƚ����ڵ��Ĵ�����Ч�� */
    WM_InvalidCoverWindow(hWin, &((WM_Obj*)hWin)->Rect);
    WM__TraversalWindows(hWin, xy, WM__MoveWindow);
    WM_Invalidate(hWin); /* ������Ч�� */
    GUI_Unlock();
}

/* ���ÿ��ƶ��Ĵ��� */
void WM_SetMoveWindow(WM_hWin hWin)
{
    ((WM_Obj*)hWin)->Style |= WM_WINDOW_MOVE;
}

/* ���ڼ�ʱ��
 * ���ڶ�ʱ�������ÿ����˶�ʱ���ܵĴ����ڶ�ʱʱ�䵽�Ժ����һ��WM_TIME_UPDATA��Ϣ
 **/
static void WM__WindowTimer(WM_Obj *pObj)
{
    if (pObj->Style & WM_WINDOW_TIMER) {  /* �����м�ʱ�� */
        if (GUI_GetTime() > pObj->LastTime + pObj->TimeCount) {
            WM_MESSAGE Msg;

            Msg.MsgId = WM_TIME_UPDATA;
            Msg.hWin = (WM_hWin)pObj;
            pObj->WinCb(&Msg);
            pObj->LastTime = GUI_GetTime(); /* ���»�ȡʱ�� */
        }
    }
}

/* ���ô��ڼ�ʱ��,ʱ������λΪms */
void WM_SetWindowTimer(WM_hWin hWin, u_16 Count)
{
    WM_Obj *pObj = hWin;
    
    GUI_Lock();
    pObj->Style |= WM_WINDOW_TIMER;
    pObj->TimeCount = Count;
    pObj->LastTime = GUI_GetTime();
    GUI_Unlock();
}

/* ���ô���ʱ�������� */
void WM_SetWindowTimerCount(WM_hWin hWin, u_16 Count)
{
    ((WM_Obj*)hWin)->TimeCount = Count;
}

/* ���ش���ʱ�������� */
u_16 WM_GetWindowTimerCount(WM_hWin hWin)
{
    return ((WM_Obj*)hWin)->TimeCount;
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

/* �ṹ�壬����������Ĵ����װ���� */
struct REF_BACK {
    RECT_LIST List;
    WM_Obj *pObj;
};

/* ˢ��ָ�������ڱ�ĳ�������ڵ�������Ļص����� */
static u_8 WM__RefBackCb(WM_Obj *pWin, void *pData)
{
    RECT_LIST List;

    if (pWin == ((struct REF_BACK*)pData)->pObj) {
        return 1;
    }
    /* ������������Ƿ��ཻ */
    if (GUI_CheckRectIntersect(&((struct REF_BACK*)pData)->pObj->Rect,
                               &pWin->Rect)) {
        /* ����ü����� */
        List = GUI__RectListTo(pWin, ((struct REF_BACK*)pData)->pObj,
                               ((struct REF_BACK*)pData)->List);
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
    return 0;
}

/* ˢ��ָ�������ڱ�ĳ�������ڵ�����������͸������ˢ�±��� */
static void WM__RefreshBackgnd(WM_Obj *pWin, RECT_LIST RectList)
{
    struct REF_BACK data;

    data.pObj = pWin;
    data.List = RectList;
    /* ��������ڵĸ����ڿ�ʼ���� */
    if (pWin->hParent == _hRootWin) {
        WM__TraversalWindows(_hRootWin, &data, WM__RefBackCb);
    }
    WM__TraversalWindows(pWin->hParent, &data, WM__RefBackCb);
}

/*********         ����͸��ʵ�ֲ��ֽ���         *********/
