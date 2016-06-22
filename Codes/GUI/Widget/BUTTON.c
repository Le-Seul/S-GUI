#include "BUTTON.h"
#include "GUI.h"

/* �����ؼ��Ի溯�� */
static void __Paint(WM_HWIN hWin)
{
    u_16 xSize, ySize;
    GUI_RECT Rect;
    BUTTON_Obj *pObj = hWin;
    GUI_HWIN hFocus = WM_GetCurrentFocus();
    
    GUI_GetClientRect(&Rect);
    xSize = Rect.x1 + 1;
    ySize = Rect.y1 + 1;
    if (pObj->Check) {
        GUI_SetFGColor(pObj->Widget.Skin.EdgeColor[1]);
    } else {
        GUI_SetFGColor(pObj->Widget.Skin.EdgeColor[0]);
    }
    /* ���Ʊ߿� */
    GUI_DrawRect(0, 0, xSize, ySize);
    if (!pObj->Check && hFocus == pObj) {
        GUI_SetFGColor(pObj->Widget.Skin.EdgeColor[1]);
        GUI_DrawRect(1, 1, xSize - 2, ySize - 2);
    }
    if (pObj->Check && hFocus == pObj) {
        GUI_SetFGColor(pObj->Widget.Skin.BackColor[1]);
        GUI_SetFontColor(pObj->Widget.Skin.FontColor[1]);
    } else {
        GUI_SetFGColor(pObj->Widget.Skin.BackColor[0]);
        GUI_SetFontColor(pObj->Widget.Skin.FontColor[0]);
        pObj->Check = 0;
    }
    /* ���ư����ڲ� */
    if (!pObj->Check && hFocus == pObj) {
        GUI_FillRect(2, 2, xSize - 4, ySize - 4);
    } else {
        GUI_FillRect(1, 1, xSize - 2, ySize - 2);
    }
    /* ���Ʊ��� */
    GUI_SetFont(WIDGET_GetFont(pObj));
    GUI_Val2Rect(&Rect, 1, 1, xSize - 2, ySize - 2);
    GUI_DispStringInRect(&Rect, pObj->Title,
        GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER);
}

static void _BUTTON_Callback(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId) {
        case WM_PAINT :
            WIDGET_Paint(pMsg->hWin);
            break;
        case WM_TP_CHECKED :
            BUTTON_Check(pMsg->hWin, 1);
            WM_SetForegroundWindow(pMsg->hWin);
            WM_SetActiveWindow(pMsg->hWin);
            pMsg->MsgId = WM_BUTTON_CLICKED;
            WM_SendMessageToParent(pMsg->hWin, pMsg);
            break;
        case WM_TP_REMOVED :
            BUTTON_Check(pMsg->hWin, 0);
            pMsg->MsgId = WM_BUTTON_RELEASED;
            WM_SendMessageToParent(pMsg->hWin, pMsg);
            break;
        case WM_TP_LEAVE   :
            pMsg->MsgId = WM_NUTTON_MOVED_OUT;
            WM_SendMessageToParent(pMsg->hWin, pMsg);
            break;
        case WM_KEYDOWN:
            if (WM_DefaultKeyProc(pMsg) == TRUE) {
                break;
            }
            if (pMsg->Param == KEY_SPACE) {
                BUTTON_Check(pMsg->hWin, 1);
                pMsg->MsgId = WM_BUTTON_CLICKED;
                WM_SendMessageToParent(pMsg->hWin, pMsg);
            }
            break;
        case WM_KEYUP:
            if (WM_DefaultKeyProc(pMsg) == TRUE) {
                break;
            }
            if (pMsg->Param == KEY_SPACE && BUTTON_GetStatus(pMsg->hWin)) {
                BUTTON_Check(pMsg->hWin, 0);
                pMsg->MsgId = WM_BUTTON_RELEASED;
                WM_SendMessageToParent(pMsg->hWin, pMsg);
            }
            break;
        case WM_KILL_FOCUS:
            WM_Invalidate(pMsg->hWin);
            break;
        default:
            WM_DefaultProc(pMsg);
    }
}

/* ���������ؼ� */
WM_HWIN BUTTON_Create(i_16 x0,
    i_16 y0,
    u_16 xSize,
    u_16 ySize,
    WM_HWIN hParent,
    u_16 Id,
    u_8 Style)
{
    BUTTON_Obj *pObj;
    
    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent,
        Style, Id, _BUTTON_Callback, sizeof(BUTTON_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        return NULL;
    }
    /* ��ɫ */
    pObj->Widget.Skin.EdgeColor[0] = 0x00708090;  /* ����δ���� */
    pObj->Widget.Skin.EdgeColor[1] = 0x002060FF;  /* ���߰��� */
    pObj->Widget.Skin.BackColor[0] = 0X00E5E5E5;  /* ����δ���� */
    pObj->Widget.Skin.BackColor[1] = 0X00B0E2FF;  /* �������� */
    pObj->Widget.Skin.FontColor[0] = 0X00000000;  /* ����û���� */
    pObj->Widget.Skin.FontColor[1] = 0X00000000;  /* ���尴�� */
    pObj->Check = 0;                /* û�а��� */
    WIDGET_SetPaintFunction(pObj, __Paint);
    BUTTON_SetTitle(pObj, "");      /* ���ó�ʼ�ַ��� */
    BUTTON_SetFont(pObj, &GUI_DEF_FONT);
    return pObj;
}

GUI_RESULT BUTTON_SetTitle(WM_HWIN hWin, const char *str)
{
    ((BUTTON_Obj*)hWin)->Title = (char*)str;
    return GUI_OK;
}

GUI_RESULT BUTTON_SetFont(WM_HWIN hWin, GUI_FONT *Font)
{
    WIDGET_SetFont(hWin, Font);
    return GUI_OK;
}

/* ��������API */
GUI_RESULT BUTTON_Check(WM_HWIN hWin, u_8 NewStatus)
{
    BUTTON_Obj *pObj = hWin;
    
    pObj->Check = NewStatus;
    WM_Invalidate(hWin);
    return GUI_OK;
}

/* ��ȡ����״̬ */
u_8 BUTTON_GetStatus(WM_HWIN hWin)
{
    return ((BUTTON_Obj *)hWin)->Check;
}
