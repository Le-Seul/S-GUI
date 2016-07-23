#include "GUI_Queue.h"
#include "GUI.h"

GUI_QUEUE *__MsgQueue;

/*
 * ��Ϣ���г�ʼ��
 * Capacity:��������
 * ����ֵ��һ���Ѿ���ʼ���Ķ���ָ��
 */
GUI_QUEUE * GUI_QueueInit(u_16 Capacity)
{
    GUI_QUEUE *pQue = GUI_Malloc(sizeof(GUI_QUEUE));

    pQue->pArray = GUI_Malloc((u_32)Capacity * sizeof(GUI_MESSAGE));
    pQue->Capacity = Capacity;
    pQue->size = 0;
    pQue->front = 1;
    pQue->rear = 0;
    return pQue;
}

/* ɾ������ */
void GUI_QueueDelete(GUI_QUEUE *pQue)
{
    if (pQue) {
        GUI_Free(pQue->pArray);
        GUI_Free(pQue);
    }
}

/*
 * ����Ϣ�������ȡһ����Ϣ
 **/
GUI_RESULT GUI_GetMessageQueue(GUI_QUEUE *pQue, GUI_MESSAGE *pMsg)
{
    if (!pQue) {
        return GUI_ERR;
    }
    GUI_LOCK();
    if (!pQue->size) {   /* ����Ϊ�� */
        GUI_UNLOCK();
        return GUI_ERR;
    }
    --pQue->size;
    *pMsg = pQue->pArray[pQue->front];
    if (++pQue->front == pQue->Capacity) {  /* ��ͷ�ƻص���ͷ */
        pQue->front = 0;
    }
    GUI_UNLOCK();
    return GUI_OK;
}

/*
 * ����Ϣ���з���һ����Ϣ
 * pQue:�¼�����ָ��
 * pMsg:��Ҫ���͵���Ϣ
 **/
GUI_RESULT GUI_PostMessageQueue(GUI_QUEUE *pQue, GUI_MESSAGE *pMsg)
{
    if (!pQue) {
        return GUI_ERR;
    }
    if (pQue->size == pQue->Capacity - 1) { /* �������� */
#if GUI_DEBUG_MODE
        GUI_DEBUG_OUT("GUI message queue is full.");
#endif
        return GUI_ERR;
    }
    ++pQue->size;
    if (++pQue->rear == pQue->Capacity) {  /* ��β�ƻص���ͷ */
        pQue->rear = 0;
    }
    pQue->pArray[pQue->rear] = *pMsg;
    return GUI_OK;
}

/* -------------------- GUI��Ϣ���� -------------------- */
/* GUI��Ϣ���г�ʼ�� */
GUI_RESULT GUI_MessageQueueInit(void)
{
    __MsgQueue = GUI_QueueInit(GUI_MSG_QUEUE_SIZE);
    if (__MsgQueue == NULL) {
#if GUI_DEBUG_MODE
        GUI_DEBUG_OUT("Failure to create message queue.");
#endif
        return GUI_ERR;
    }
    return GUI_OK;
}

/* ɾ��GUI��Ϣ���� */
void GUI_MessageQueueDelete(void)
{
    GUI_QueueDelete(__MsgQueue);
    __MsgQueue = NULL;
}

/* ��GUI��Ϣ�����ж�ȡһ����Ϣ */
GUI_RESULT GUI_GetMessage(GUI_MESSAGE *pMsg)
{
    GUI_RESULT res;

    GUI_LOCK();
    res = GUI_GetMessageQueue(__MsgQueue, pMsg);
    GUI_UNLOCK();
    return res;
}

/* ��GUI��Ϣ���з���һ����Ϣ */
GUI_RESULT GUI_PostMessage(GUI_MESSAGE *pMsg)
{
    GUI_RESULT res;

    GUI_LOCK();
    res = GUI_PostMessageQueue(__MsgQueue, pMsg);
    GUI_UNLOCK();
    return res;
}
