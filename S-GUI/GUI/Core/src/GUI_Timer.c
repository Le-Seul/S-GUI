#include "GUI_Timer.h"
#include "GUI.h"

static GUI_TIMER *__TimerList = NULL;

/* �ڶ�ʱ��������Ѱ�ҵ�ǰ���� */
static GUI_TIMER * _FindWindowInList(GUI_HWIN hWin)
{
    GUI_TIMER *pNode = __TimerList;

    if (pNode) {
        while (pNode && pNode->hWin != hWin) {
            pNode = pNode->pNext;
        }
    }
    return pNode;
}

/* �ڶ�ʱ��������ɾ��һ������ */
static void _DeleteWindowInList(GUI_HWIN hWin)
{
    if (__TimerList) {
        GUI_TIMER *pNode = __TimerList, *pLast = NULL;

        while (pNode && pNode->hWin != hWin ) {
            pLast = pNode;
            pNode = pNode->pNext;
        }
        if (pNode) { /* pNode��ΪNULL˵�����ڴ��� */
            if (pLast) { /* ���ڲ��ڵ�1���ڵ� */
                pLast->pNext = pNode->pNext;
                GUI_fastfree(pNode);
            } else {
                pNode = __TimerList;
                __TimerList = pNode->pNext;
                GUI_fastfree(pNode);
                
            }
        }
    }
}

/* ���ڼ�ʱ��
 * ���ڶ�ʱ�������ÿ����˶�ʱ���ܵĴ����ڶ�ʱʱ�䵽�Ժ����һ��WM_TIME_UPDATA��Ϣ
 **/
void GUI_TimerHandle(void)
{
    GUI_LOCK();
    if (__TimerList) {
        GUI_TIMER *pNode;

        for (pNode = __TimerList; pNode; pNode = pNode->pNext) {
            if (GUI_GetTime() >= pNode->LastTime + pNode->Interval) {
                WM_SendMessage(pNode->hWin, WM_TIME_UPDATA, 0);
                pNode->LastTime = GUI_GetTime(); /* ���»�ȡʱ�� */
            }
        }
    }
    GUI_UNLOCK();
}

/* ���ô��ڼ�ʱ��,ʱ������λΪms,���IntervalΪ0���ͻ�ɾ���ô��ڵĶ�ʱ�� */
void GUI_SetWindowTimer(GUI_HWIN hWin, GUI_TIME Interval)
{
    if (hWin == NULL) {
        return;
    }
    GUI_LOCK();
    if (Interval == 0) {
        _DeleteWindowInList(hWin);
    } else {
        GUI_TIMER *pNode;

        pNode = _FindWindowInList(hWin);
        if (pNode) { /* ���ڽڵ� */
            pNode->Interval = Interval;
        } else {
            /* �����½ڵ㵽����ͷ */
            pNode = __TimerList;
            __TimerList = GUI_fastmalloc(sizeof(GUI_TIMER));
            __TimerList->hWin = hWin;
            __TimerList->Interval = Interval;
            __TimerList->LastTime = GUI_GetTime();
            __TimerList->pNext = pNode;
        }
    }
    GUI_UNLOCK();
}

/* ���ش��ڼ�����ʱ���� */
GUI_TIME GUI_GetWindowTimer(GUI_HWIN hWin)
{
    GUI_TIMER *pNode;
    GUI_TIME Interval;
    
    if (hWin == NULL) {
        return 0;
    }
    GUI_LOCK();
    pNode = _FindWindowInList(hWin);
    if (pNode) {
        Interval = pNode->Interval;
    } else {
        Interval = 0;
    }
    GUI_UNLOCK();
    return Interval;
}
