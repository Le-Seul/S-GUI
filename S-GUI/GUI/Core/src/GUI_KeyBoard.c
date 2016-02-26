#include "GUI_KeyBoard.h"
#include "GUI.h"

/*
*�Ѽ�ֵ�洢�����̻�����
*/
void GUI_SendKey(u_8 Key)
{
    WM_PostMessage(GUI_Context.hFocus, WM_KEY, Key);
}

/* �������� */
GUI_RESULT GUI_KeyMessageProc(GUI_MESSAGE *pMsg)
{
    if (pMsg->MsgId == WM_KEY) {
        GUI_LOCK();
        if (pMsg->hWin == GUI_Context.hFocus) {
            WM__SendMessage(pMsg->hWin, pMsg);
        }
        GUI_UNLOCK();
        return GUI_OK;
    }
    return GUI_ERR;
}
