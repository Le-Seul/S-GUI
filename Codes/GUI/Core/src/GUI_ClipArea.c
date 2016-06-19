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

/**
 @ GUI�����һ������ʱ���¼�����.
 @ hWin:��ӵ��´��ھ��.
 **/
void GUI_ClipNewWindow(GUI_HWIN hWin)
{
    WM_Obj *pWin = hWin;
    GUI_RECT Rect;
    GUI_AREA Area;

    /* ͸������ֱ�ӷ��� */
    if (pWin->Status & WM_WS_TRANS) {
        pWin->ClipArea = NULL;
        return;
    }
    WM_GetWindowAreaRect(hWin, &Rect);
    /* �ڿ����ڵ�֮ǰ,���ھ�ֻ��һ���ü����� */
    Area = GUI_GetRectList(1);
    if (Area) { /* ���½����ڵĶ������д���ʱ����Bug */
        Area->Rect = Rect;
        pWin->ClipArea = Area;
    }
    do {
        pWin = pWin->hParent; /* ����͸�������� */
    } while (pWin && pWin->Status & WM_WS_TRANS);
    /* �Ӵ��ڵĸ��׿�ʼ���㡣 */
    while (pWin && pWin != hWin) {
        if (!(pWin->Status & WM_WS_TRANS)) {
            /* �ڴ���ԭ��������Ļ������ų��´��ڵľ��� */
            pWin->ClipArea = GUI_ClipExcludeRect(pWin->ClipArea, &Rect);
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
    } while (pWin && pWin->Status & WM_WS_TRANS);
    while (pWin) {
        GUI_FreeRectList(pWin->ClipArea);
        if (!(pWin->Status & WM_WS_TRANS)) {
            GUI_ClipNewWindow(pWin);
        } else {
            pWin->ClipArea = NULL; /* ͸�����ڲ����������  */
        }
        pWin = pWin->hNextLine;
    }
}

/**
 @ ɾ�����ڵļ�����.
 **/
void GUI_DeleteWindowClipArea(GUI_HWIN hWin)
{
    GUI_FreeRectList(((WM_Obj *)hWin)->ClipArea);
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
        /* �ȼ��㵽hWin֮ǰ�ļ����� */
        for (pWin = pWin->hParent; pWin != pEnd; pWin = pWin->hNextLine) {
            GUI_FreeRectList(pWin->ClipArea);
            if (!(pWin->Status & WM_WS_TRANS)) {
                GUI_ClipNewWindow(pWin);
            } else {
                pWin->ClipArea = NULL; /* ͸�����ڲ����������  */
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
        if (pWin->Status & WM_WS_TRANS) {
            return GUI_CurrentClipArea();
        }
        return pWin->ClipArea;
    }
    return NULL;
}
