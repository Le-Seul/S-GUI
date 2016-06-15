#include "GUI_Rect.h"
#include "GUI.h"

/* ����ʾ���ε�����ת��Ϊ�ṹ�� */
GUI_BOOL GUI_Val2Rect(GUI_RECT *pDst,
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
        return TRUE;
    }
    return FALSE;
}

/* ȡ���������ཻ�Ĳ���,����洢��*pDst����,��GUI_RectAndCalc()�� */
GUI_BOOL GUI_RectOverlay(GUI_RECT *pDst, GUI_RECT *a, GUI_RECT *b)
{
    /*  ���ϽǵĽ���  */
    pDst->x0 = MAX(a->x0, b->x0);
    pDst->y0 = MAX(a->y0, b->y0);
    /*  ���½ǵĽ���  */
    pDst->x1 = MIN(a->x1, b->x1);
    pDst->y1 = MIN(a->y1, b->y1);
    
    if (pDst->x0 > pDst->x1 || pDst->y0 > pDst->y1) {
        return FALSE; /* �������β��ཻ */
    }
    return TRUE;
}

/* ȡ�������β���,��GUI_RectOrCalc()�죬a��b������NULL */
void GUI_RectSum(GUI_RECT *pDst, GUI_RECT *a, GUI_RECT *b)
{
    /*  ���Ͻǵ����  */
    pDst->x0 = MIN(a->x0, b->x0);
    pDst->y0 = MIN(a->y0, b->y0);
    /*  ���½ǵ��յ�  */
    pDst->x1 = MAX(a->x1, b->x1);
    pDst->y1 = MAX(a->y1, b->y1);
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
GUI_BOOL GUI_CheckPointAtRect(i_16 x, i_16 y, GUI_RECT *Rect)
{
    if (x >= Rect->x0 && x <= Rect->x1 &&
        y >= Rect->y0 && y <= Rect->y1) {
        return TRUE;
    }
    return FALSE;
}

/* ���һ�������Ƿ�Ϊ�� */
GUI_BOOL GUI_CheckRectNull(GUI_RECT *Rect)
{
    if (Rect->x1 >= Rect->x0 && Rect->y1 >= Rect->y0) {
        return TRUE;  /* �ǿ� */
    }
    return FALSE;
}

/* ������������Ƿ��ཻ*/
GUI_BOOL GUI_CheckRectIntersect(GUI_RECT *pRect1, GUI_RECT *pRect2)
{
    if (pRect1->x0 > pRect2->x1 || pRect1->y0 > pRect2->y1
     || pRect1->x1 < pRect2->x0 || pRect1->y1 < pRect2->y0) {
        return FALSE;    /* ���ཻ */
    }
    return TRUE;/* �ཻ */
}

/* ���һ�������Ƿ������һ������ */
GUI_BOOL GUI_RectInclude(GUI_RECT *pSrc, GUI_RECT *pDst)
{
    if (pSrc->x0 <= pDst->x0 && pSrc->y0 <= pDst->y0
     && pSrc->x1 >= pDst->x1 && pSrc->y1 >= pDst->y1) {
        return TRUE;
    }
    return FALSE;
}

/* �ƶ�һ������ */
void GUI_MoveRect(GUI_RECT *Rect, i_16 dX, i_16 dY)
{
    Rect->x0 += dX;
    Rect->x1 += dX;
    Rect->y0 += dY;
    Rect->y1 += dY;
}

/* ���ڲü���������˽�жѳ�ʼ�� */
GUI_RESULT GUI_RectListInit(u_16 num)
{
    GUI_AREA pNode;

    /* �����ڴ�(����һ����ͷ) */
    ++num;
    GUI_AreaHeap = GUI_fastmalloc(sizeof(AREA_NODE) * (u_32)num);
    if (GUI_AreaHeap == NULL) {
        return GUI_ERR; /* ����ʧ�� */
    }
    pNode = GUI_AreaHeap;
    while (--num) { /* ��������pNext */
        pNode->pNext = pNode + 1;
        ++pNode;
    }
    pNode->pNext = NULL;
    return GUI_OK;
}

/* ����һ���ü��������� */
GUI_AREA GUI_GetRectList(u_16 num)
{
    GUI_AREA pNode;
    GUI_AREA Area = GUI_AreaHeap;

    if (!num) {
        return NULL;
    }
    pNode = Area;
    while (pNode && num--) {
        pNode = pNode->pNext;
    }
    if (pNode == NULL) { /* �������� */
#if GUI_DEBUG_MODE
        GUI_DEBUG_OUT("GUI clip rect heap overflow.");
#endif
        return NULL;
    }
    Area = Area->pNext;
    GUI_AreaHeap->pNext = pNode->pNext;
    pNode->pNext = NULL;
    return Area;
}

/* �ͷ�һ���ü��������� */
GUI_RESULT GUI_FreeRectList(GUI_AREA Area)
{
    GUI_AREA p;
    
    if (Area == NULL) {
        return GUI_ERR;
    }
    p = GUI_AreaHeap->pNext;
    GUI_AreaHeap->pNext = Area; /* ���뵽�������ǰ�� */
    while (Area->pNext) {
        Area = Area->pNext;
    }
    Area->pNext = p;
    return GUI_OK;
}

/* �ͷ�һ��������ڵ�,ע��Area����ΪNULL */
void GUI_FreeRectListNode(GUI_AREA Area)
{
    GUI_AREA p;

    p = GUI_AreaHeap->pNext;
    GUI_AreaHeap->pNext = Area; /* ���뵽�������ǰ�� */
    Area->pNext = p;
}

/* ������Src�þ���Dstȥ�ü�
 * -Src��Dst��������Ч�ؾ���.
 */
GUI_AREA GUI_RectCut(GUI_RECT *Src, GUI_RECT *Dst)
{
    u_8 n = 0;
    GUI_RECT r;
    GUI_AREA p;

    GUI_AREA Area;

    if (GUI_RectOverlay(&r, Src, Dst) == FALSE) { /* �ж��Ƿ��ཻ */
        /* ���ཻ���ü��������Src�����Լ� */
        Area = GUI_GetRectList(1); /* �������� */
        if (Area) {
            Area->Rect = *Src;
        }
        return Area;
    }
    /* Src��ȫ��Src��Dst���ཻ�����ڵ�,�������вü����� */
    if (Src->x0 >= r.x0 && Src->x1 <= r.x1
     && Src->y0 >= r.y0 && Src->y1 <= r.y1) {
        return NULL;
    }
    /* ÿһ�����������Ա�����һ�����ηָ�Ϊ4�� */
    Area = GUI_GetRectList(4); /* �������� */
    if (Area == NULL) { /* ����ʧ�� */
        return NULL;
    }
    p = Area;
    /* �������ü����� */
    if (Src->y0 < r.y0) {
        p->Rect.x0 = Src->x0;
        p->Rect.y0 = Src->y0;
        p->Rect.x1 = Src->x1;
        p->Rect.y1 = r.y0 - 1;
        p = p->pNext;
        ++n;
    }
    if (r.y1 < Src->y1) {
        p->Rect.x0 = Src->x0;
        p->Rect.y0 = r.y1 + 1;
        p->Rect.x1 = Src->x1;
        p->Rect.y1 = Src->y1;
        p = p->pNext;
        ++n;
    }
    if (Src->x0 < r.x0) {
        p->Rect.x0 = Src->x0;
        p->Rect.y0 = r.y0;
        p->Rect.x1 = r.x0 - 1;
        p->Rect.y1 = r.y1;
        p = p->pNext;
        ++n;
    }
    if (r.x1 < Src->x1) {
        p->Rect.x0 = r.x1 + 1;
        p->Rect.y0 = r.y0;
        p->Rect.x1 = Src->x1;
        p->Rect.y1 = r.y1;
        p = p->pNext;
        ++n;
    }
    GUI_FreeRectList(p); /* �ͷŶ�������� */
    if (n) { /* ���ü�������Ϊ0ʱ��������β */
        for (p = Area; --n; p = p->pNext);
        p->pNext = NULL;
        return Area;
    }
    /* ����ü�����Ϊ0��˵����Src���ڵ����򷵻�ֵΪNULL */
    /* bug���.�˴���Ӧ�ñ�ִ�е�����Ϊǰ���Ѿ��ж����ڵ� */
    return NULL;
}
