#include "GUI_KeyBoard.h"
#include "GUI.h"

/*
*�Ѽ�ֵ���͵���Ϣ����
*/
void GUI_SendKey(u_8 Key, u_8 Status)
{
    if (Status == GUI_KEYDOWN) { /* �������� */
        WM_PostMessage(WM_GetActiveWindow(), WM_KEYDOWN, Key);
    } else if (Status == GUI_KEYUP){ /* �����ɿ� */
        WM_PostMessage(WM_GetActiveWindow(), WM_KEYUP, Key);
    }
}

/* �������� */
GUI_RESULT GUI_KeyMessageProc(GUI_MESSAGE *pMsg)
{
    if (pMsg->MsgId == WM_KEYUP) {
        GUI_LOCK();
        /* �����ڻ��ǻ����ʱ���ݼ�ֵ */
        if (pMsg->hWin == WM_GetActiveWindow()) {
            WM__SendMessage(pMsg->hWin, pMsg);
        }
        GUI_UNLOCK();
        return GUI_OK;
    }
    return GUI_ERR;
}
