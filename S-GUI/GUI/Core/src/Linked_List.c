#include "Linked_List.h"
#include "stdio.h"
#include "string.h"
#include "mystring.h"

#include "GUI.h"

#define ADT_TEST 0

#define MALLOC(x)  GUI_malloc(x)
#define FREE(x)    GUI_free(x);

/* ��xָ������ݲ��뵽plָ��Ľڵ�� */
void Insert(PData x, PNode pl)
{
    PNode tmp;
    u_16 i, len;
    len = pl->datalen;
    tmp = MALLOC(sizeof(Node));
    if (len) {
        tmp->data = MALLOC(len);
    } else {
        tmp->data = x;
    } /* ָ������ */
    if (tmp == NULL || tmp->data == NULL) {
#if ADT_TEST == 1
        printf("Out of space!\n");
        return;
#else
        /* Error code */

#endif
    }
    for (i = 0; i < len; i++) { /* �������� */
        ((char *)(tmp->data))[i] = ((char *)x)[i];
    }
    tmp->datalen = len;
    tmp->next = pl->next;
    pl->next = tmp;
}

/* ��������pl�нڵ�pt��ǰ�� */
static List FindPrecious(List pl, PNode pt)
{
    PNode p;
    p = pl;
    while (p->next != NULL && p->next != pt) {
        p = p->next;
    }
    return p;
}

/* ɾ��ptָ��Ľڵ�,�����½ڵ�ĵ�ַ */
static PNode Delete(List pl, PNode pt)
{
    PNode p;
    p = FindPrecious(pl, pt);
    if (pl != pt) {
        p->next = pt->next;
        if (pt->datalen) { /* ����ָ������ */
            FREE(pt->data);
        }
        FREE(pt);
    }
    return p->next;
}

/* ����һ���������������һ���ձ�ͷ */
List ListInit(u_16 len)
{
    PNode tmp;
    u_16 i;
    tmp = MALLOC(sizeof(List));
    if (len) { /* ����ָ������ */
        tmp->data = MALLOC(len);
    }
    if (tmp == NULL || tmp->data == NULL) {
#if ADT_TEST == 1
        printf("Out of space!\n");
        return;
#else 
        /* Error code */

#endif
    }
    for (i = 0; i < len; i++) {
        ((char *)(tmp->data))[i] = 0;
    }
    tmp->datalen = len;
    tmp->next = NULL;
    return tmp;
}

/* ��ȡ������ */
u_16 ListLen(List pl)
{
    u_16 i;
    for (i = 0; pl->next != NULL; i++) {
        pl = pl->next;
    }
    return i;
}

/* �õ���node_num�����ڵĵ�ַ */
PNode GetNodePtr(List pl, u_16 node_num)
{
    u_16 i;
    if (node_num > ListLen(pl)) {
        return NULL;
    }
    for (i = 0; i < node_num; i++) {
        pl = pl->next;
    }
    return pl;
}

/* ��ȡ��node_num�����ڵ����ݵ�ַ
 * �����ָ�������򷵻�ָ��ֵ
 **/
PData List_GetNodeData(List pl, u_16 node_num)
{
    PNode p;
    p = GetNodePtr(pl, node_num);
    if (p != NULL) {
        return p->data;
    } else {
        return NULL;
    }
}

/* ��ȡ����pl��ptָ��������� */
u_16 GetNodeNum(List pl, PNode pt)
{
    u_16 i;
    if (pl == pt) {
        return 0;
    }
    for (i = 1; pl->next != pt; i++) {
        pl = pl->next;
    }
    return i;
}

/* ���뵽��num���ڵ��
 * num��1��ʼ����
 **/
char InsertNum(PData x, List pl, u_16 num)
{
    PNode p;
    if (num > ListLen(pl)) {
        return ADT_ERR;
    }
    p = GetNodePtr(pl, num);
    Insert(x, p);
    return ADT_OK;
}

/* ɾ����num���ڵ� */
char DeleteNum(List pl, u_16 num)
{
    PNode p;
    if (num > ListLen(pl) || num == 0) {
        return ADT_ERR;
    }
    p = GetNodePtr(pl, num);
    Delete(pl, p);
    return ADT_OK;
}

/* ɾ������ */
void DeleteList(List L)
{
    PNode tmp;
    while (L != NULL) {
        tmp = L->next;
        if (L->datalen) { /* ����ָ������ */
            FREE(L->data);
        }
        FREE(L);
        L = tmp;
    }
}

/* �����ַ����ڵ㣬��������ڵ�Ϊ�ַ���ʱ����ʹ��,�����ִ�Сд */
u_16 FindNodeStr(List pl, char *pd)
{
    u_16 num;
    num = 1;
    while (pl->next != NULL) {
        pl = pl->next;
        if (mystricmp(pd, pl->data) == 0) { /* ���Դ�Сд�Ƚ� */
            return num;
        }
        num++;
    }
    return 0;
}

#if ADT_TEST==1
typedef struct {
    float a;
    char b;
} Data;

static void PrintfList(List pl)
{
    while (pl != NULL) {
        printf("%g\n", ((Data*)pl->data)->a);
        pl = pl->next;
    }
}

int main(void)
{
    List pl, pt;
    u_16 i;
    char status;
    Data a;
    Data *b;
    a.a = 1.33;
    a.b = 3;
    pl = ListInit(sizeof(Data));//��������

    for (i = 0; i < 20; i++) {
        status = InsertNum(&a, pl, i);
        if (status == ADT_ERR) {
            printf("Insert Error!\n");
        }
        a.a = a.a + 1;
    }
    PrintfList(pl);
    printf("ListLen=%d\n\n", ListLen(pl));

    for (i = 0; i < 20; i++) {
        status = DeleteNum(pl, 5);
        if (status == ADT_ERR) {
            printf("Delete Error!\n");
        }
        PrintfList(pl);
        printf("ListLen=%d\n\n", ListLen(pl));
    }

    b = List_GetNodeData(pl, 2);
    if (b != NULL) {
        printf("\nList Node2=%g\n", b->a);
        printf("\nNode 5 Num=%d\n", GetNodeNum(pl, GetNodePtr(pl, 5)));
        printf("sizeof(Node)=%d\n", sizeof(Node));
    } else {
        printf("Error:PData==NULL");
    }

    DeleteList(pl);

    pl = ListInit(100);//����һ���ַ��������ַ����������Ϊ100�ֽ�

    InsertNum("Item0", pl, 0);
    InsertNum("Item1", pl, 0);
    InsertNum("Item2", pl, 1);
    printf((void*)List_GetNodeData(pl, 1));
    printf("\n");
    printf((void*)List_GetNodeData(pl, 2));
    printf("\n");
    printf((void*)List_GetNodeData(pl, 3));
    printf("\nItem2 is %d node.\n", FindNodeStr(pl, "Item2"));
    DeleteList(pl);
}
#endif


