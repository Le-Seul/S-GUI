#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

#define ADT_ERR 1
#define ADT_OK  0

typedef unsigned short u_16;

typedef void *PData;
typedef struct Node Node;
typedef struct Node *PNode;
typedef PNode List;

struct Node{
    PData data;
    PNode next;
    u_16 datalen;
};

List ListInit(u_16 len);//����һ���������������һ���ձ�ͷ
u_16 ListLen(List pl);//��ȡ������
PData List_GetNodeData(List pl,u_16 node_num);//��ȡ��node_num�����ڵ�����
u_16 GetNodeNum(List pl,PNode pt);  //��ȡ����pl��ptָ���������
char InsertNum(PData x,List pl,u_16 num);//���뵽��num���ڵ��
char DeleteNum(List pl,u_16 num);//ɾ����num���ڵ�
void DeleteList(List L);//ɾ������
u_16 FindNodeStr(List pl,char* pd);//�����ַ����ڵ㣬��������ڵ�Ϊ�ַ���ʱ����ʹ��
void Insert(PData x,PNode pl);
PNode GetNodePtr(List pl,u_16 node_num);

#endif /*__LINKED_LIST_H*/
