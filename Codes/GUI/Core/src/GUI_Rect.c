#include "GUI_Rect.h"

#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#define MIN(a, b)           ((a) < (b) ? (a) : (b))

/* ����ʾ���ε�����ת��Ϊ�ṹ�� */
GUI_BOOL GUI_Val2Rect(GUI_RECT *pDst,
    int x0,
    int y0,
    int xSize,
    int ySize)
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

/* ȡ���������ཻ�Ĳ���,����洢��*pDst���� */
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

/* ���һ�����Ƿ���һ�������� */
GUI_BOOL GUI_CheckPointAtRect(int x, int y, GUI_RECT *Rect)
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
void GUI_MoveRect(GUI_RECT *Rect, int dX, int dY)
{
    Rect->x0 += dX;
    Rect->x1 += dX;
    Rect->y0 += dY;
    Rect->y1 += dY;
}
