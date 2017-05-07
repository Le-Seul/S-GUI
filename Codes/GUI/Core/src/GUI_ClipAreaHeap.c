#include "GUI_ClipAreaHeap.h"
#include "GUI_Malloc.h"
#include "GUI_Rect.h"

/* ˽�б��� */
static GUI_AREA GUI_AreaHeap;       /* �ü������ */

/* ��ȡһ�����ڲü��� */
static GUI_AREA _GetRectList(int num)
{
    GUI_AREA pNode, pList;

    if (!num) {
        return NULL;
    }
    /* �����ڴ�(����һ����ͷ) */
    pList = GUI_Malloc(sizeof(AREA_NODE));
    for (pNode = pList; --num && pNode; pNode = pNode->pNext) {
        pNode->pNext = GUI_Malloc(sizeof(AREA_NODE));
    }
    if (pNode == NULL) {
        /* �ͷ����� */
        for (pNode = pList; pNode; pNode = pNode->pNext) {
            GUI_Free(pNode);
        }
        return NULL;
    } else {
        pNode->pNext = NULL;
    }
    return pList;
}

/* ���ڲü���������˽�жѳ�ʼ�� */
GUI_RESULT GUI_RectListInit(void)
{
    GUI_AreaHeap = _GetRectList(1);
    if (GUI_AreaHeap) {
        return GUI_OK;
    }
    return GUI_ERR;
}

/* �ͷſ��еļ������ */
void GUI_FreeIdleRectList(void)
{
    GUI_AREA pNode, pNext;

    pNode = GUI_AreaHeap->pNext;
    while (pNode) {
        pNext = pNode->pNext;
        GUI_Free(pNode);
        pNode = pNext;
    }
    GUI_AreaHeap->pNext = NULL;
}

/* ����һ���ü��������� */
GUI_AREA GUI_GetRectList(int num)
{
    int n = num;
    GUI_AREA pNode, pLast, Area = GUI_AreaHeap;

    if (!num && !Area) {
        return NULL;
    }
    pNode = Area;
    pLast = Area;
ReTest:
    while (pNode && n--) {
        pLast = pNode;
        pNode = pNode->pNext;
    }
    if (pNode == NULL) { /* �������� */
        pLast->pNext = _GetRectList(n + 1);
        pNode = pLast->pNext;
        goto ReTest;
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
 @ -Src��Dst��������Ч�ؾ���.
 **/
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