#include "GUI_TouchPad.h"
#include "GUI.h"

/* ���ô�����״̬ */
void GUI_TouchPadSendValue(i_16 x, i_16 y, u_16 State)
{
    static u_16 LastState = GUI_TP_REMOVED;

    /*
     * �����嵱ǰ״̬Ϊ���»���һ�ε�״̬Ϊ���¡�
     * Ҳ����˵ֻ����GUI��Ϣ���з���һ�δ����ɿ���Ϣ��
     **/
    if (State == GUI_TP_CHECKED || LastState == GUI_TP_CHECKED) {
        u_32 Point;
        GUI_MESSAGE Msg;

        LastState = State;
        Point = ((u_32)x << 16) | y;
        /* �����Ϣ�ṹ�� */
        Msg.hWin = NULL;
        Msg.hWinSrc = NULL;
        if (State == GUI_TP_CHECKED) {
            Msg.MsgId = WM_TP_CHECKED;
        } else {
            Msg.MsgId = WM_TP_REMOVED;
        }
        Msg.Param = (GUI_PARAM)Point;
        GUI_PostMessage(&Msg); /* ������Ϣ��GUI��Ϣ���� */
    }
}

/* ��������Ϣ���� */
GUI_RESULT GUI_TouchPadMessageProc(GUI_MESSAGE *pMsg)
{
    static GUI_HWIN _CurWin = NULL;
    static GUI_POINT _CurPos;
    
    /* ����Ƿ���WM��������Ϣ */
    if (pMsg->MsgId == WM_TP_CHECKED || pMsg->MsgId == WM_TP_REMOVED) {
        GUI_HWIN hWin;
        GUI_POINT Point;

        GUI_LOCK();
        Point.x = (u_32)pMsg->Param >> 16;
        Point.y = (u_32)pMsg->Param & 0xffff;
        if (pMsg->MsgId == WM_TP_CHECKED) {
            hWin = WM_GetExposedWindow(Point.x, Point.y);
            /* �ж��Ƿ�����һ�εĴ��� */
            if (hWin != NULL) {
                GUI_POINT Pos[2];

                /* ��ǰ���� */
                Pos[0].x = Point.x;
                Pos[0].y = Point.x;
                /* ����ƫ�� */
                Pos[1].x = Point.x - _CurPos.x;
                Pos[1].y = Point.y - _CurPos.y;
                if (_CurWin == NULL) { /* ��һ�δ��� */
                    _CurWin = hWin;
                    WM_SendMessage(hWin, WM_TP_CHECKED, (GUI_PARAM)&Point);
                } else if (hWin == _CurWin) { /* һֱ�ڴ��� */
                    WM_SendMessage(hWin, WM_TP_PRESS, (GUI_PARAM)&Pos);
                } else if (WM_FindWindow(_CurWin) == GUI_OK) {
                    /* �뿪�˿ؼ��Ҵ��ڴ��� */
                    WM_SendMessage(_CurWin, WM_TP_LEAVE, (GUI_PARAM)&Pos);
                }
                _CurPos = Point;
            }
        } else {  /* �����ɿ� */
            if (WM_FindWindow(_CurWin) == GUI_OK) {  /* ���ڻ����� */
                WM_SendMessage(_CurWin, WM_TP_REMOVED, (GUI_PARAM)NULL);
            }
            _CurWin = NULL;
        }
        GUI_UNLOCK();
        return GUI_OK;
    }
    return GUI_ERR;
}
