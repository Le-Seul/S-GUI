#include "linked_list.h"
#include "GUI.h"

#define MALLOC(x)  GUI_fastmalloc(x)
#define FREE(x)    GUI_fastfree(x);

/* ��������pl�нڵ�pt��ǰ�� */
static LIST FindPrecious(LIST pl, LIST pt)
{
    LIST p;
    p = pl;

    while (p->pNext != NULL && p->pNext != pt) {
        p = p->pNext;
    }
    return p;
}

/* ɾ��ptָ��Ľڵ� */
void List_DeleteNode(LIST pl, LIST pt)
{
    LIST p;

    if (pl && pt) {
        p = FindPrecious(pl, pt);
        if (pl != pt) {
            p->pNext = pt->pNext;
            if (pt->DataLen) { /* ����ָ������ */
                FREE(pt->pData);
            }
            FREE(pt);
        }
    }
}

/* ��xָ������ݲ��뵽plָ��Ľڵ�� */
GUI_RESULT List_InsertNode(LIST pl, void * x, int len)
{
    LIST l;

    if (pl == NULL) {
        return GUI_ERR;
    }
    l = MALLOC(sizeof(NODE));
    if (l == NULL) {
        return GUI_ERR;
    }
    if (len) { /* ����ռ� */
        l->pData = MALLOC(len);
        if (l->pData == NULL) {
            FREE(l);
            return GUI_ERR;
        }
        while (len--) { /* �������� */
            ((char *)(l->pData))[len] = ((char *)x)[len];
        }
    } else { /* ָ������ */
        l->pData = x;
    }
    l->DataLen = len;
    l->pNext = pl->pNext;
    pl->pNext = l;
    return GUI_OK;
}

/* ��һ���ڵ���뵽����ĩβ */
GUI_RESULT List_InsertEnd(LIST pl, void * x, int len)
{
    if (pl == NULL) {
        return GUI_ERR;
    }
    while (pl->pNext) {
        pl = pl->pNext;
    }
    return List_InsertNode(pl, x, len);
}

/* ����һ���������������һ���ձ�ͷ */
LIST List_Init(void)
{
    LIST Front;

    Front = MALLOC(sizeof(NODE));
    Front->pData = NULL;
    if (Front == NULL) {
        return NULL;
    }
    Front->DataLen = 0;
    Front->pNext = NULL;
    return Front;
}

/* ɾ������ */
void List_Delete(LIST L)
{
    LIST l;

    while (L != NULL) {
        l = L->pNext;
        if (L->DataLen) { /* ����ָ������ */
            FREE(L->pData);
        }
        FREE(L);
        L = l;
    }
}

/* ��ȡ������ */
int List_GetSize(LIST pl)
{
    int i = 0;

    if (pl) {
        for (; pl->pNext != NULL; ++i) {
            pl = pl->pNext;
        }
    }
    return i;
}

/* �õ���n�����ڵĵ�ַ */
LIST List_GetNodePtr(LIST pl, int n)
{
    int i;

    if (n > List_GetSize(pl)) {
        return NULL;
    }
    for (i = 0; i < n; ++i) {
        pl = pl->pNext;
    }
    return pl;
}

/* ��ȡ��n�����ڵ�����ָ�� */
void * List_GetNodeData(LIST pl, int n)
{
    LIST p;

    p = List_GetNodePtr(pl, n);
    if (p != NULL) {
        return p->pData;
    } else {
        return NULL;
    }
}

/* ��ȡ����pl��ptָ����������� */
int List_GetNodeIndex(LIST pl, LIST pt)
{
    int i = 0;

    if (pl) {
        for (; pl->pNext != pt; ++i) {
            pl = pl->pNext;
        }
    }
    return i;
}

/* �����ַ����ڵ㣬��������ڵ�Ϊ�ַ���ʱ����ʹ��,�����ִ�Сд */
int List_FindStr(LIST pl, char *pd)
{
    int i = 1;

    if (pl) {
        while (pl->pNext != NULL) {
            pl = pl->pNext;
            if (GUI_Stricmp(pd, pl->pData) == 0) { /* ���Դ�Сд�Ƚ� */
                return i;
            }
            ++i;
        }
    }
    return 0;
}
