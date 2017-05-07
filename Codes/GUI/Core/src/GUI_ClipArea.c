#include "GUI_ClipArea.h"
#include "GUI.h"

/* ע��: Ŀǰ֧��͸�����ڵļ���������߼���δʵ��. */

/**
 @ �������ų�һ������.
 @ Area:Ҫ���޸ĵļ�����.
 @ Rect:��Ҫ�ų��ľ���.
 @ ����ֵ:�ų����κ�ļ�����.
 **/
GUI_AREA GUI_ClipExcludeRect(GUI_AREA Area, GUI_RECT *Rect)
{
    GUI_AREA pNode, pDst = NULL, pNext = NULL, p;

    /* �������ͷ,ֱ����÷ǿյ�ָ��(����ͷ) */
    for (pNode = Area; !pDst && pNode; pNode = pNext) {
        /* �����һ�������� */
        pDst = GUI_RectCut(&pNode->Rect, Rect);
        pNext = pNode->pNext;
        GUI_FreeRectListNode(pNode); /* �ͷŽڵ� */
    }
    if (pDst != NULL) {
        p = pDst;
        for (; pNode; pNode = pNext) {
            for (; p->pNext; p = p->pNext); /* ������β */
            p->pNext = GUI_RectCut(&pNode->Rect, Rect); /* ����������������� */
            pNext = pNode->pNext;
            GUI_FreeRectListNode(pNode); /* �ͷŽڵ� */
        }
    }
    return pDst;
}

/* �ü�͸�����ڵ��Ӵ��� */
static GUI_AREA _ClipTransChildren(GUI_AREA L, WM_Obj *pWin)
{
    for (pWin = pWin->hFirstChild; pWin && L; pWin = pWin->hNext) {
        if (pWin->status & WM_WS_TRANS) {
            L = _ClipTransChildren(L, pWin); /* �ü��Ӵ��� */
        } else {
            L = GUI_ClipExcludeRect(L, &pWin->rect);  /* �ü�͸�����ڵĺ��� */
        }
    }
    return L;
}

/* �ü�һ������ */
GUI_AREA GUI_ClipOneWindow(GUI_HWIN hWin)
{
    WM_Obj *pWin = hWin, *pObj;
    GUI_RECT r;
    GUI_AREA Area;

    /* �ڿ����ڵ�֮ǰ,���ھ�ֻ��һ���ü����� */
    Area = GUI_GetRectList(1);
    if (Area) {
        WM_GetWindowAreaRect(pWin, &r);;
        Area->Rect = r;
    }
    /* ���ڻᱻ���Ķ����ǻ����ұߵ��ֵ���(����еĻ�)�ü�,Ҳ�����ڵ����������ĺ��Ӻ��ֵ��ǣ�
       ������㴰�ڵĲü��������������ܵõ�������ڱ������ڵ���Ĳü���������. */
    /* �ٱ������ұߵ�ͬ�����ڼ����ȵ�ͬ������ */
    pObj = pWin;
    while (pObj != gui_rootwin && Area) {
        while (pObj->hNext && Area) {
            pObj = pObj->hNext; /* ���ұ��� */
            /* �������ͨ���ھ�ֱ�Ӽ���ü��������͸�����ھ������ĺ���������ü� */
            if (pObj->status & WM_WS_TRANS) {
                Area = _ClipTransChildren(Area, pObj);
            } else {
                Area = GUI_ClipExcludeRect(Area, &pObj->rect);
            }
        }
        pObj = pObj->hParent; /* ���ϱ��� */
    }
    if (pWin->hFirstChild && Area) {
        /* �ȱ����Ӵ��� */
        pObj = pWin->hFirstChild;
        while (pObj && Area) {
            /* �������ͨ���ھ�ֱ�Ӽ���ü��������͸�����ھ������ĺ���������ü� */
            if (pObj->status & WM_WS_TRANS) {
                Area = _ClipTransChildren(Area, pObj);
            } else {
                Area = GUI_ClipExcludeRect(Area, &pObj->rect);
            }
            pObj = pObj->hNext; /* ���ұ��� */
        }
    }
    return Area;
}

/**
 @ GUI�����һ������ʱ���¼�����.
 @ hWin:��ӵ��´��ھ��.
 **/
void GUI_ClipNewWindow(GUI_HWIN hWin)
{
    WM_Obj *pWin = hWin;
    GUI_RECT Rect;

    /* ͸������ֱ�ӷ��� */
    if (pWin->status & WM_WS_TRANS) {
        pWin->clipArea = NULL;
        return;
    }
    /* �����´��ڵļ����� */
    pWin->clipArea = GUI_ClipOneWindow(hWin);
    WM_GetWindowAreaRect(hWin, &Rect);
    do {
        pWin = pWin->hParent; /* ����͸�������� */
    } while (pWin && pWin->status & WM_WS_TRANS);
    /* �Ӵ��ڵĸ��׿�ʼ���㡣 */
    while (pWin && pWin != hWin) {
        if (!(pWin->status & WM_WS_TRANS)) {
            /* �ڴ���ԭ��������Ļ������ų��´��ڵľ��� */
            pWin->clipArea = GUI_ClipExcludeRect(pWin->clipArea, &Rect);
        }
        pWin = pWin->hNextLine;
    }
}

/**
 @ ����ָ�����ڼ������ڵ����ڵĵļ�����.
 @ hWin:��ʼ���������Ĵ��ھ��.
 @ hEnd:���һ��Ҫ���������Ĵ���.
 **/
void GUI_WindowClipArea(GUI_HWIN hWin)
{
    WM_Obj *pWin = hWin;

    do {
        pWin = pWin->hParent; /* ����͸�������� */
    } while (pWin && pWin->status & WM_WS_TRANS);
    while (pWin) {
        GUI_FreeRectList(pWin->clipArea);
        if (!(pWin->status & WM_WS_TRANS)) {
            GUI_ClipNewWindow(pWin);
        } else {
            pWin->clipArea = NULL; /* ͸�����ڲ����������  */
        }
        pWin = pWin->hNextLine;
    }
}

/**
 @ ɾ�����ڵļ�����.
 **/
void GUI_DeleteWindowClipArea(GUI_HWIN hWin)
{
    GUI_FreeRectList(WM_HandleToPtr(hWin)->clipArea);
}

/**
 @ ���㴰�ڼ��������Ӵ��ڵļ�����.
 @ hWin:Ҫ�ƶ��Ĵ���.
 @ dx:x����λ��.
 @ dy:y����λ��
 **/
void GUI_ClipWindows(GUI_HWIN hWin)
{
    WM_Obj *pWin = hWin, *pEnd;

    GUI_LOCK();
    pEnd = WM_GetTopChildWindow(pWin);
    if (pEnd) {
        pEnd = pEnd->hNextLine;
        if (pWin->hParent) {
            pWin = pWin->hParent;
        }
        /* �ȼ��㵽hWin֮ǰ�ļ����� */
        for (; pWin != pEnd; pWin = pWin->hNextLine) {
            GUI_FreeRectList(pWin->clipArea);
            if (!(pWin->status & WM_WS_TRANS)) {
                GUI_ClipNewWindow(pWin);
            } else {
                pWin->clipArea = NULL; /* ͸�����ڲ����������  */
            }
        }
    }
    GUI_UNLOCK();
}

/* ��ȡ���ڵļ����� */
GUI_AREA GUI_GetWindowClipArea(GUI_HWIN hWin)
{
    WM_Obj *pWin = hWin;

    if (pWin) {
        /* ͸������ֱ�ӷ�����һ�������õļ����� */
        if (pWin->status & WM_WS_TRANS) {
            return GUI_CurrentClipArea();
        }
        return pWin->clipArea;
    }
    return NULL;
}
