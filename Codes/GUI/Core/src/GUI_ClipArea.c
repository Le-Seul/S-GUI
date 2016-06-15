#include "GUI_ClipArea.h"
#include "GUI.h"

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

    Rect = WM_GetWindowAreaRect(hWin);
    /* �ڿ����ڵ�֮ǰ,���ھ�ֻ��һ���ü����� */
    Area = GUI_GetRectList(1);
    if (Area) { /* ���½����ڵĶ������д���ʱ����Bug */
        Area->Rect = Rect;
        pWin->ClipArea = Area;
    }
    /* �Ӵ��ڵĸ��׿�ʼ���㡣 */
    for (pWin = pWin->hParent; pWin && pWin != hWin; pWin = pWin->hNextLine) {
        if (!(pWin->Status & WM_WS_TRANS)) {
            /* �ڴ���ԭ��������Ļ������ų��´��ڵľ��� */
            pWin->ClipArea = GUI_ClipExcludeRect(pWin->ClipArea, &Rect);
        }
    }
}

/**
 @ ����ָ�����ڼ������洰�ڵļ�����.
 @ hWin:��ʼ���������Ĵ��ھ��.
 @ hEnd:���һ��Ҫ���������Ĵ���.
 **/
void GUI_WindowsClipArea(GUI_HWIN hWin, GUI_HWIN hEnd)
{
    WM_Obj *pWin = hWin;

    for (pWin = pWin->hParent; pWin; pWin = pWin->hNextLine) {
        GUI_FreeRectList(pWin->ClipArea);
        GUI_ClipNewWindow(pWin);
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
 @ ���㴰�ڼ�����.
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
            GUI_ClipNewWindow(pWin);
        }
    }
    GUI_UNLOCK();
}
