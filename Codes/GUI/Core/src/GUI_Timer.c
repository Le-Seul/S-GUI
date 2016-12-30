#include "GUI_Timer.h"
#include "GUI.h"

static GUI_TIMER *__TimerList = NULL;

/* �ڶ�ʱ��������Ѱ�ҵ�ǰ�ڵ�
 @ ���ڵ�ǰ�ڵ�ͷ��ؽ�ڵ�ָ��, ���򷵻�NULL.
 */
static GUI_TIMER * _FindInList(GUI_HTMR hTimer)
{
    GUI_TIMER *pNode = __TimerList;

    if (pNode) {
        while (pNode && pNode != hTimer) {
            pNode = pNode->pNext;
        }
    }
    return pNode;
}

/* ��ʱ�������¼� */
static void __TimerEvent(GUI_TIMER *p)
{
    int per = p->Period;
    GUI_TIME t = GUI_GetTime();

    if (per && t >= p->SetTime) {
        if (p->Mode == GUI_TMR_ONE) { /* ���δ��� */
            p->Period = 0;
        } else { /* �Զ����� */
            /* ������һ�ζ�ʱ�����µ�ʱ�� */
            p->SetTime = t + (per - (t - p->SetTime) % per);
        }
        WM_SendMessage(p->hWin, WM_TIMER, (u_32)p);
    }
}

/* ���ڼ�ʱ��
 * ���ڶ�ʱ�������ÿ����˶�ʱ���ܵĴ����ڶ�ʱʱ�䵽�Ժ����һ��WM_TIMER��Ϣ
 **/
void GUI_TimerHandler(void)
{
    GUI_TIMER *pNode, *pNext;

    GUI_LOCK();
    for (pNode = __TimerList; pNode; pNode = pNext) {
        /* ����pNext��ֵ�Է�ֹ�ڻص�������ɾ����ʱ������ִ��� */
        pNext = pNode->pNext;
        __TimerEvent(pNode);
    }
    GUI_UNLOCK();
}

/* ������ʱ�� */
GUI_HWIN GUI_TimerCreate(GUI_HWIN hWin, int Id, int Period, u_8 Mode)
{
    GUI_TIMER *pNode;

    GUI_LOCK();
    /* �����½ڵ㵽����ͷ */
    pNode = __TimerList;
    __TimerList = GUI_Malloc(sizeof(GUI_TIMER));
    __TimerList->hWin = hWin;
    __TimerList->Id = Id;
    __TimerList->Period = Period;
    __TimerList->SetTime = GUI_GetTime() + Period;
    __TimerList->Mode = Mode;
    __TimerList->pNext = pNode;
    GUI_UNLOCK();
    return pNode;
}

/* ɾ����ʱ�� */
void GUI_TimerDetete(GUI_HTMR hTimer)
{
    GUI_TIMER *pNode = __TimerList, *pLast = NULL;

    GUI_LOCK();
    while (pNode && pNode != hTimer) {
        pLast = pNode;
        pNode = pNode->pNext;
    }
    if (pNode) { /* pNode��ΪNULL˵�����ڽڵ� */
        if (pLast) { /* ���ڵ�1���ڵ� */
            pLast->pNext = pNode->pNext;
        } else { /* ɾ����һ���ڵ� */
            __TimerList = pNode->pNext;
        }
        GUI_Free(pNode);
    }
    GUI_UNLOCK();
}

/* ɾ��ĳ���������еĶ�ʱ�� */
void GUI_TimerDeleteAtWindow(GUI_HWIN hWin)
{
    GUI_TIMER *pNode = __TimerList, *pLast = NULL, *pNext;

    GUI_LOCK();
    while (pNode) {
        pNext = pNode->pNext;
        if (pNode->hWin == hWin) {
            if (pLast) { /* ���ڵ�1���ڵ� */
                pLast->pNext = pNext;
            } else { /* ɾ����һ���ڵ� */
                __TimerList = pNext;
            }
            GUI_Free(pNode);
        } else {
            pLast = pNode;
        }
        pNode = pNext;
    }
    GUI_UNLOCK();
}

/* ��ȡ��ʱ��ID */
int GUI_GetTimerId(GUI_HTMR hTimer)
{
    int Id = 0;

    GUI_LOCK();
    if (_FindInList(hTimer)) {
        Id = ((GUI_TIMER *)hTimer)->Id;
    }
    GUI_UNLOCK();
    return Id;
}

/* ���ö�ʱ�� */
void GUI_ResetTimer(GUI_HTMR hTimer, int Period)
{
    GUI_LOCK();
    if (_FindInList(hTimer)) {
        ((GUI_TIMER *)hTimer)->Period = Period;
        ((GUI_TIMER *)hTimer)->SetTime = GUI_GetTime() + Period;
    }
    GUI_UNLOCK();
}
