#ifndef __GUI_CORE_H
#define __GUI_CORE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"
#include "GUI_Botton.h"

#define _hRootWin  (GUI_Data->RootWin)

/* ���� */
typedef struct { i_16 x0, y0, x1, y1; } GUI_RECT;

/* �������� */
typedef struct RECT_NODE {
    GUI_RECT Rect;
    struct RECT_NODE *pNext;
} RECT_NODE, *RECT_LIST;

typedef void * GUI_EVENT;   /* GUI�¼� */

/* ���нṹ���� */
typedef struct {
    GUI_EVENT *pArray;  //����ָ��
    u_16 capacity;      //������������
    u_16 ItemSize;      //���������ݵ�Ԫ�ĳ��ȣ����ֽ�Ϊ��λ
    u_16 size;          //����Ŀǰ���ݵ�Ԫ����
    u_16 front;         //��ͷ
    u_16 rear;          //��β
} GUI_QUEUE;

typedef struct{
    GUI_PHY_INFO phy_info; /* Ӳ����Ϣ */
    GUI_TIME guitime;      /* GUI����ʱ�� */
    RECT_LIST RectList;    /* �ü�����˽�ж� */
    RECT_LIST NowRectList; /* ��ǰ���ƵĲü������� */
    GUI_hWin RootWin;     /* ������ */
    GUI_QUEUE *KeyQueue;   /* �������л��� */
#if GUI_USE_MEMORY
    GUI_COLOR *lcdbuf; //LCD����
#endif
}GUI_WORK_SPACE;

extern GUI_WORK_SPACE *GUI_Data;

void GUI_Init(void);
void GUI_Unload(void);
void GUI_ScreenSize(u_16 *xSize, u_16 *ySize);
u_16 GUI_GetScreenWidth(void);
u_16 GUI_GetScreenHeight(void);
void GUI_Delay(GUI_TIME tms); //GUI��ʱ������
GUI_QUEUE* GUI_EventQueueInit(u_16 capacity, u_16 ItemSize); //�¼����г�ʼ��
void GUI_EventQueueDelete(GUI_QUEUE *pQue); //ɾ������
u_8 GUI_GetEvent(GUI_QUEUE *pQue, GUI_EVENT event); //���¼��������ȡһ���¼�
u_8 GUI_SendEvent(GUI_QUEUE *pQue, GUI_EVENT event); //���¼����з���һ���¼�
u_8 GUI_QueueIsEmpty(GUI_QUEUE *pQue);//����¼������Ƿ�Ϊ��
void GUI_CleanQueue(GUI_QUEUE *pQue);//����¼�����

void GUI_SetNowRectList(RECT_LIST l);
void GUI_DrawAreaInit(void);
GUI_RECT *GUI_GetNowArea(void);
u_8 GUI_GetNextArea(GUI_RECT *pRect);

#endif
