#include "GUI_Queue.h"
#include "GUI.h"

/*
 * ��Ϣ���г�ʼ��
 * Capacity:��������
 * ����ֵ��һ���Ѿ���ʼ���Ķ���ָ��
 */
GUI_QUEUE * GUI_QueueInit(u_16 Capacity)
{
    GUI_QUEUE *pQue = GUI_fastmalloc(sizeof(GUI_QUEUE));

    pQue->pArray = GUI_fastmalloc((u_32)Capacity * sizeof(GUI_MESSAGE));
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
        GUI_fastfree(pQue->pArray);
        GUI_fastfree(pQue);
    }
}

/*
 * ����Ϣ�������ȡһ����Ϣ
 **/
GUI_RESULT GUI_GetMessageQueue(GUI_QUEUE *pQue, GUI_MESSAGE *pMsg)
{
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
    if (pQue->size == pQue->Capacity - 1) { /* �������� */
        return GUI_ERR;
    }
    ++pQue->size;
    if (++pQue->rear == pQue->Capacity) {  /* ��β�ƻص���ͷ */
        pQue->rear = 0;
    }
    pQue->pArray[pQue->rear] = *pMsg;
    return GUI_OK;
}
