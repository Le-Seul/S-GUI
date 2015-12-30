#include "GUI_TouchPad.h"
#include "GUI.h"

static i_16 __GUI_TP_X, __GUI_TP_Y, __GUI_TP_MoveX, __GUI_TP_MoveY;

void GUI_TouchPadSendValue(u_16 x, u_16 y, u_16 State)
{
    GUI_hWin hWin;
    static GUI_hWin Last_hWin = NULL;
    static u_16 LastState = GUI_TP_REMOVED;
    
    /* һֱû�б����� */
    if (State == LastState && State == GUI_TP_REMOVED) {
        return;
    }
    __GUI_TP_MoveX = (i_16)x - __GUI_TP_X;
    __GUI_TP_MoveY = (i_16)y - __GUI_TP_Y;
    __GUI_TP_X = x;
    __GUI_TP_Y = y;
    LastState = State;
    if (State == GUI_TP_CHECKED) {
        hWin = WM_GetExposedWindow(x, y);
        /* �ж��Ƿ�����һ�εĴ��� */
        if (hWin != NULL) {
            if (Last_hWin == NULL) { /* ��һ�δ��� */
                Last_hWin = hWin;
                WM_SendMessage(hWin, WM_TP_CHECKED, NULL);
            } else if (hWin == Last_hWin) { /* һֱ�ڴ��� */
                WM_SendMessage(hWin, WM_TP_PRESS, NULL);
            } else if (WM_FindWindow(Last_hWin)) { /* �뿪�˿ؼ�,�Ҵ��ڴ��� */
                WM_SendMessage(Last_hWin, WM_TP_LEAVE, NULL);
            }
        }
    } else {  /* �����ɿ� */
        if (WM_FindWindow(Last_hWin)) {  /* ���ڻ����� */
            WM_SendMessage(Last_hWin, WM_TP_REMOVED, NULL);
        }
        Last_hWin = NULL;
    }
}

i_16 GUI_GetTouchPad_X(void)
{
    return __GUI_TP_X;
}

i_16 GUI_GetTouchPad_Y(void)
{
    return __GUI_TP_Y;
}

i_16 GUI_GetTouchPad_MoveX(void)
{
    return __GUI_TP_MoveX;
}

i_16 GUI_GetTouchPad_MoveY(void)
{
    return __GUI_TP_MoveY;
}

