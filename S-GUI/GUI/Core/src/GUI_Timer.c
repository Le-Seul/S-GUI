#include "GUI_Timer.h"
#include "GUI.h"

static GUI_TIMER *_ListPtr = NULL;

static GUI_TIMER * _FindWindowInList(GUI_HWIN hWin)
{
    GUI_TIMER *pNode = _ListPtr;

    if (pNode) {
        while (pNode && pNode->hWin != hWin) {
            pNode = pNode->pNext;
        }
    }
    return pNode;
}

static void _DeleteWindowInList(GUI_HWIN hWin)
{
    if (_ListPtr) {
        GUI_TIMER *pNode = _ListPtr, *pLast = NULL;

        while (pNode && pNode->hWin != hWin ) {
            pLast = pNode;
            pNode = pNode->pNext;
        }
        if (pNode) { /* pNode��ΪNULL˵�����ڴ��� */
            if (pLast) { /* ���ڲ��ڵ�1���ڵ� */
                pLast->pNext = pNode->pNext;
                GUI_fastfree(pNode);
            } else {
                GUI_fastfree(_ListPtr);
                _ListPtr = NULL;
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
    if (_ListPtr) {
        GUI_TIMER *pNode;

        for (pNode = _ListPtr; pNode; pNode = pNode->pNext) {
            if (GUI_GetTime() >= pNode->LastTime + pNode->Interval) {
                WM_SendMessage(pNode->hWin, WM_TIME_UPDATA, (GUI_PARAM)NULL);
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
            /* �����½ڵ� */
            pNode = _ListPtr;
            _ListPtr = GUI_fastmalloc(sizeof(GUI_TIMER));
            _ListPtr->hWin = hWin;
            _ListPtr->Interval = Interval;
            _ListPtr->LastTime = GUI_GetTime();
            _ListPtr->pNext = pNode;
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
