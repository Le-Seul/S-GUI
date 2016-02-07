#include "BUTTON.h"
#include "GUI.h"

/* �����ؼ��Ի溯�� */
static void _BUTTON_Paint(WM_hWin hWin)
{
    i_16 x0, y0;
    u_16 xSize, ySize;
    GUI_COLOR Color, FontColor;
    GUI_RECT Rect;
    BUTTON_Obj *pObj = hWin;
    
    Rect = ((WM_Obj*)hWin)->Rect;
    x0 = Rect.x0;
    y0 = Rect.y0;
    xSize = Rect.x1 - x0 + 1;
    ySize = Rect.y1 - y0 + 1;
    Color = pObj->Widget.Skin.EdgeColor[0];
    /* ���Ʊ߿� */
    GUI_DrawRect(x0, y0, xSize, ySize, Color);
    if (pObj->Check) {
        Color = pObj->Widget.Skin.BackColor[1];
        FontColor = pObj->Widget.Skin.FontColor[1];
    } else {
        Color = pObj->Widget.Skin.BackColor[0];
        FontColor = pObj->Widget.Skin.FontColor[0];
    }
    /* ���ư����ڲ� */
    GUI_FillRect(x0 + 1, y0 + 1, xSize - 2, ySize - 2, Color);
    /* ���Ʊ��� */
    GUI_DspStringCurRectMiddle(x0 + 1, y0 + 1,
                               xSize - 2, ySize - 2,
                               pObj->Title, FontColor,
                               WIDGET_GetFont(pObj));
}

static void _BUTTON_Callback(WM_MESSAGE *pMsg)
{
    /* ����Ƿ�ΪBUTTON�ؼ� */
    WIDGET_SignErrorReturnVoid(pMsg->hWin, WIDGET_BUTTON);
    switch (pMsg->MsgId) {
        case WM_PAINT :
            _BUTTON_Paint(pMsg->hWin);
            break;
        case WM_DELETE :
            GUI_fastfree(pMsg->hWin);
            break;
        case WM_TP_CHECKED :
            BUTTON_Check(pMsg->hWin, 1);
            WM_SetActiveMainWindow(pMsg->hWin);
            WM_PostMessageToParent(pMsg->hWin, WM_BUTTON_CLICKED, NULL);
            break;
        case WM_TP_REMOVED :
            BUTTON_Check(pMsg->hWin, 0);
            WM_PostMessageToParent(pMsg->hWin, WM_BUTTON_RELEASED, NULL);
            break;
        case WM_TP_LEAVE   :
            WM_PostMessageToParent(pMsg->hWin, WM_NUTTON_MOVED_OUT, NULL);
            break;
    }
}

/* ���������ؼ� */
WM_hWin BUTTON_Create(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      WM_hWin hParent,
                      u_16 Id,
                      u_8 Flag)
{
    BUTTON_Obj *pObj;
    
    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, Flag,
                                  WIDGET_BUTTON, Id, _BUTTON_Callback,
                                  sizeof(BUTTON_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        return NULL;
    }
    /* �����û��� */
    pObj->Widget.Win.UserRect.x0 = pObj->Widget.Win.Rect.x0 + 1;
    pObj->Widget.Win.UserRect.y0 = pObj->Widget.Win.Rect.y0 + 1;
    pObj->Widget.Win.UserRect.x1 = pObj->Widget.Win.Rect.x1 - 1;
    pObj->Widget.Win.UserRect.y1 = pObj->Widget.Win.Rect.y1 - 1;
    /* ��ɫ */
    pObj->Widget.Skin.EdgeColor[0] = 0x00FFFFFF;  /* ����δ���� */
    pObj->Widget.Skin.EdgeColor[1] = 0x00FFFFFF;  /* ���߰��� */
    pObj->Widget.Skin.BackColor[0] = 0X0066FF99;
    pObj->Widget.Skin.BackColor[1] = 0X007171C6;
    pObj->Widget.Skin.FontColor[0] = 0X00367200;
    pObj->Widget.Skin.FontColor[1] = 0X00FFFFFF;
    pObj->Check = 0;                /* û�а��� */
    BUTTON_SetTitle(pObj, "");      /* ���ó�ʼ�ַ��� */
    BUTTON_SetFont(pObj, GUI_DEF_FONT);
    
    return pObj;
}

GUI_RESULT BUTTON_SetTitle(WM_hWin hWin, const char *str)
{
    /* ����Ƿ�ΪBUTTON�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_BUTTON);
    ((BUTTON_Obj*)hWin)->Title = (char*)str;
    return GUI_OK;
}

GUI_RESULT BUTTON_SetFont(WM_hWin hWin, GUI_FontType Font)
{
    /* ����Ƿ�ΪBUTTON�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_BUTTON);
    WIDGET_SetFont(hWin, Font);
    return GUI_OK;
}

/* ��������API */
GUI_RESULT BUTTON_Check(WM_hWin hWin, u_8 NewStatus)
{
    BUTTON_Obj *pObj = hWin;
    
    /* ����Ƿ�ΪBUTTON�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_BUTTON);
    pObj->Check = NewStatus;
    WM_Invalidate(hWin);
    return GUI_OK;
}
