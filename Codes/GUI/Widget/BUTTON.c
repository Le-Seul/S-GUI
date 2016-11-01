﻿#include "BUTTON.h"
#include "GUI.h"

/* 按键控件自绘函数 */
static void __Paint(WM_HWIN hWin)
{
    u_16 xSize, ySize;
    GUI_RECT Rect;
    BUTTON_Obj *pObj = hWin;
    GUI_HWIN hFocus = WM_GetWindowFocus();
    
    GUI_GetClientRect(&Rect);
    xSize = Rect.x1 + 1;
    ySize = Rect.y1 + 1;
    if (pObj->Check) {
        GUI_SetFGColor(0x002060FF);
    } else {
        GUI_SetFGColor(0x00708090);
    }
    /* 绘制边框 */
    GUI_DrawRect(0, 0, xSize, ySize);
    if (!pObj->Check && hFocus == pObj) {
        GUI_SetFGColor(0x002060FF);
        GUI_DrawRect(1, 1, xSize - 2, ySize - 2);
    }
    if (pObj->Check && hFocus == pObj) {
        GUI_SetFGColor(0X00B0E2FF);
        GUI_SetFontColor(0X00000000);
    } else {
        GUI_SetFGColor(0X00E5E5E5);
        GUI_SetFontColor(0X00000000);
        pObj->Check = 0;
    }
    /* 绘制按键内部 */
    if (!pObj->Check && hFocus == pObj) {
        GUI_FillRect(2, 2, xSize - 4, ySize - 4);
    } else {
        GUI_FillRect(1, 1, xSize - 2, ySize - 2);
    }
    /* 绘制标题 */
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
            WM_SetActiveWindow(pMsg->hWin); /* 设置为活动窗口 */
            WM_SetWindowFocus(pMsg->hWin); /* 设置为焦点窗口 */
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
        default:
            WM_DefaultProc(pMsg);
    }
}

/* 创建按键控件 */
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
    pObj->Check = 0;                /* 没有按下 */
    WIDGET_SetPaintFunction(pObj, __Paint);
    BUTTON_SetTitle(pObj, "");      /* 设置初始字符串 */
    BUTTON_SetFont(pObj, &GUI_DEF_FONT);
    return pObj;
}

void BUTTON_SetTitle(WM_HWIN hWin, const char *str)
{
    ((BUTTON_Obj*)hWin)->Title = (char *)str;
}

void BUTTON_SetFont(WM_HWIN hWin, GUI_FONT *Font)
{
    WIDGET_SetFont(hWin, Font);
}

/* 按键按下API */
void BUTTON_Check(WM_HWIN hWin, u_8 NewStatus)
{
    BUTTON_Obj *pObj = hWin;
    
    pObj->Check = NewStatus;
    WM_Invalidate(hWin);
}

/* 获取按键状态 */
u_8 BUTTON_GetStatus(WM_HWIN hWin)
{
    return ((BUTTON_Obj *)hWin)->Check;
}
