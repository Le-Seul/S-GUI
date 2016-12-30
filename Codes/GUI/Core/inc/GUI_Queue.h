#ifndef __GUI_QUEUE_H
#define __GUI_QUEUE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"

typedef u_32 GUI_PARAM;

/* ��Ϣ�ṹ�嶨�� */
typedef struct {
    int MsgId;       /* ��Ϣ��� */
    GUI_HWIN hWin;    /* Ŀ�괰�ھ�� */
    GUI_HWIN hWinSrc; /* Դ���ھ��  */
    GUI_PARAM Param;  /* ���� */
} GUI_MESSAGE;

/* ���нṹ���� */
typedef struct {
    GUI_MESSAGE *pArray; /* ����ָ�� */
    int Capacity;       /* ������������ */
    int size;           /* ����Ŀǰ���ݵ�Ԫ���� */
    int front;          /* ��ͷ */
    int rear;           /* ��β */
} GUI_QUEUE;

GUI_QUEUE * GUI_QueueInit(int Capacity);
void GUI_QueueDelete(GUI_QUEUE *pQue);
GUI_RESULT GUI_GetMessageQueue(GUI_QUEUE *pQue, GUI_MESSAGE *pMsg);
GUI_RESULT GUI_PostMessageQueue(GUI_QUEUE *pQue, GUI_MESSAGE *pMsg);
GUI_RESULT GUI_MessageQueueInit(void);
void GUI_MessageQueueDelete(void);
GUI_RESULT GUI_GetMessage(GUI_MESSAGE *pMsg);
GUI_RESULT GUI_PostMessage(GUI_MESSAGE *pMsg);

#endif /* __GUI_QUEUE_H */
