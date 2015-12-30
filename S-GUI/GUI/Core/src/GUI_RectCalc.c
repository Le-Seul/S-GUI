#include "GUI_RectCalc.h"
#include "GUI.h"

/* ����ʾ���ε�����ת��Ϊ�ṹ�� */
GUI_RESULT GUI_Val2Rect(GUI_RECT *pDst,
                        i_16 x0,
                        i_16 y0,
                        u_16 xSize,
                        u_16 ySize)
{
    pDst->x0 = x0;
    pDst->y0 = y0;
    pDst->x1 = x0 + xSize - 1;
    pDst->y1 = y0 + ySize - 1;
    if (xSize && ySize) {
        return GUI_OK;
    }
    return GUI_ERR;
}

/* ȡ���������ཻ�Ĳ���,����洢��*pDst����,��GUI_RectAndCalc�� */
GUI_RESULT GUI_RectOverlay(GUI_RECT *pDst, GUI_RECT *a, GUI_RECT *b)
{
    /*  ���ϽǵĽ���  */
    pDst->x0 = a->x0 > b->x0 ? a->x0 : b->x0;
    pDst->y0 = a->y0 > b->y0 ? a->y0 : b->y0;
    /*  ���½ǵĽ���  */
    pDst->x1 = a->x1 < b->x1 ? a->x1 : b->x1;
    pDst->y1 = a->y1 < b->y1 ? a->y1 : b->y1;
    
    if (pDst->x0 > pDst->x1 || pDst->y0 > pDst->y1) {
        return GUI_ERR; /* �������β��ཻ */
    }
    return GUI_OK;
}

/* ���������� */
GUI_RECT GUI_RectAndCalc(GUI_RECT *pRect1, GUI_RECT *pRect2)
{
    GUI_RECT Rect;
    
    /*  ���ϽǵĽ���  */
    Rect.x0 = pRect1->x0 > pRect2->x0 ? pRect1->x0 : pRect2->x0;
    Rect.y0 = pRect1->y0 > pRect2->y0 ? pRect1->y0 : pRect2->y0;
    /*  ���½ǵĽ���  */
    Rect.x1 = pRect1->x1 < pRect2->x1 ? pRect1->x1 : pRect2->x1;
    Rect.y1 = pRect1->y1 < pRect2->y1 ? pRect1->y1 : pRect2->y1;
    return Rect;
}

/* ���λ����� */
GUI_RECT GUI_RectOrCalc(GUI_RECT *pRect1, GUI_RECT *pRect2)
{
    GUI_RECT Rect;
    
    /*  ���ϽǵĿ�ʼ��  */
    Rect.x0 = pRect1->x0 < pRect2->x0 ? pRect1->x0 : pRect2->x0;
    Rect.y0 = pRect1->y0 < pRect2->y0 ? pRect1->y0 : pRect2->y0;
    /*  ���½ǵĽ�����  */
    Rect.x1 = pRect1->x1 > pRect2->x1 ? pRect1->x1 : pRect2->x1;
    Rect.y1 = pRect1->y1 > pRect2->y1 ? pRect1->y1 : pRect2->y1;
    return Rect;
}

/* ���һ�����Ƿ���һ�������� */
u_8 GUI_CheckPointAtRect(u_16 x, u_16 y, GUI_RECT *Rect)
{
    if (x >= Rect->x0 && x <= Rect->x1 &&
        y >= Rect->y0 && y <= Rect->y1) {
        return 1;
    }
    return 0;
}

/* ���һ�������Ƿ�Ϊ�� */
u_8 GUI_CheckRectNull(GUI_RECT *Rect)
{
    if (Rect->x1 >= Rect->x0 && Rect->y1 >= Rect->y0) {
        return 1;  /* �ǿ� */
    }
    return 0;
}

/* ������������Ƿ��ཻ*/
u_8 GUI_CheckRectIntersect(GUI_RECT *pRect1, GUI_RECT *pRect2)
{
    if (pRect1->x0 > pRect2->x1 || pRect1->y0 > pRect2->y1
     || pRect1->x1 < pRect2->x0 || pRect1->y1 < pRect2->y0) {
        return 0;    /* ���ཻ */
    }
    return 1;/* �ཻ */
}

void GUI_MoveRect(GUI_RECT *Rect, i_16 dX, i_16 dY)
{
    Rect->x0 += dX;
    Rect->x1 += dX;
    Rect->y0 += dY;
    Rect->y1 += dY;
}

/* ���ڲü���������˽�жѳ�ʼ�� */
u_8 GUI_RectListInit(u_16 num)
{
    RECT_NODE *pNode;

    /* �����ڴ�(����һ����ͷ) */
    ++num;
    GUI_Data->RectList = GUI_fastmalloc(sizeof(RECT_NODE) * (u_32)num);
    if (GUI_Data->RectList == NULL) {
        return 1;
    }
    pNode = GUI_Data->RectList;
    while (--num) { /* ��������pNext */
        pNode->pNext = pNode + 1;
        ++pNode;
    }
    pNode->pNext = NULL;
    return 0;
}

/* ����һ���ü��������� */
RECT_LIST GUI_GetRectList(u_16 num)
{
    RECT_NODE *pNode;
    RECT_LIST List = GUI_Data->RectList;

    if (!num) {
        return NULL;
    }
    pNode = List;
    while (pNode && num--) {
        pNode = pNode->pNext;
    }
    if (pNode == NULL) { /* �������� */
        return NULL;
    }
    List = List->pNext;
    GUI_Data->RectList->pNext = pNode->pNext;
    pNode->pNext = NULL;
    return List;
}

/* �ͷ�һ���ü��������� */
GUI_RESULT GUI_FreeRectList(RECT_NODE *pNode)
{
    RECT_NODE *p;
    
    if (pNode == NULL) {
        return GUI_ERR;
    }
    p = GUI_Data->RectList->pNext;
    GUI_Data->RectList->pNext = pNode; /* ���뵽�������ǰ�� */
    while (pNode->pNext) {
        pNode = pNode->pNext;
    }
    pNode->pNext = p;
    return GUI_OK;
}

/* �ü�һ������ 
 * �㷨�д��Ż�
 */
RECT_LIST GUI_RectCut(GUI_RECT *Src, GUI_RECT *Dst)
{
    u_8 i, j, t[4];
    GUI_RECT Rect[4], r;
    RECT_NODE *pNode;
    RECT_LIST List;

    if (GUI_RectOverlay(&r, Src, Dst) == GUI_ERR) { /* �ж��Ƿ��ཻ */
        List = GUI_GetRectList(1); /* �������� */
        if (List) {
            List->Rect = *Src;
        }
        return List;
    }
    /* ÿһ�����������Ա�����һ�����ηָ�Ϊ4�� */
    Rect[0].x0 = Src->x0;
    Rect[0].y0 = Src->y0;
    Rect[0].x1 = Src->x1;
    Rect[0].y1 = r.y0 - 1;

    Rect[1].x0 = Src->x0;
    Rect[1].y0 = r.y1 + 1;
    Rect[1].x1 = Src->x1;
    Rect[1].y1 = Src->y1;

    Rect[2].x0 = Src->x0;
    Rect[2].y0 = r.y0;
    Rect[2].x1 = r.x0 - 1;
    Rect[2].y1 = r.y1;

    Rect[3].x0 = r.x1 + 1;
    Rect[3].y0 = r.y0;
    Rect[3].x1 = Src->x1;
    Rect[3].y1 = r.y1;
    
    /* ͳ����Ч�������� */
    for (i = 0, j = 0; i < 4; ++i) {
        if (Rect[i].x1 >= Rect[i].x0 && Rect[i].y1 >= Rect[i].y0) {
            ++j;
            t[i] = 1; /* �����Ч���� */
        } else {
            t[i] = 0; /* �����Ч���� */
        }
    }
    List = GUI_GetRectList(j); /* �������� */
    if (List == NULL) {
        return NULL;
    }
    pNode = List;
    for (i = 0; i < 4; ++i) {
        if (t[i]) { /* ����Ч�ľ��θ�ֵ�������Ա */
            pNode->Rect = Rect[i];
            pNode = pNode->pNext;
        }
    }
    return List;
}

/* Ϊһ���ü������������¼���ü��������� */
RECT_LIST GUI_ReCalcRectList(RECT_LIST List, GUI_RECT *Rect)
{
    RECT_NODE *pNode;
    RECT_LIST p = NULL, q;

    /* �������ͷ,ֱ����÷ǿյ�ָ��(����ͷ) */
    for (q = List; !p && q; q = q->pNext) {
        p = GUI_RectCut(&q->Rect, Rect);
    }
    if (p != NULL) {
        pNode = p;
        for (; q; q = q->pNext) { /* �ü�ʣ�µľ��� */
            while (pNode->pNext) { /* ֱ�����һ������ */
                pNode = pNode->pNext;
            }
            pNode->pNext = GUI_RectCut(&q->Rect, Rect); /* �������� */
        }
    }
    GUI_FreeRectList(List); /* �ͷ�ԭ�������� */
    return p;
}
