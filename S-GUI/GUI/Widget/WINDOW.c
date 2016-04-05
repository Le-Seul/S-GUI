#include "WINDOW.h"
#include "GUI.h"

#define WINDOW_DEF_CAPHEIGHT   20
/* Ĭ�ϵĴ���caption����ɫcaption����ɫ. */
/* �Լ����屳��ɫ. */
#define WINDOW_CAPTION_UPC          0x003C4C52  /* ����CAPTION�ϲ������ɫ */
#define WINDOW_CAPTION_DOWNC        0x00333E42  /* ����CAPTION�²������ɫ */
#define WINDOW_TITLE_COLOR          0x0096CDCD  /* ���ڱ�����ɫ */
#define WINDOW_BODY_BKC             0x00292323  /* ���ڵ�ɫ */
#define WINDOW_FONT_COLOR           0x00FFFFFF  /* ����������ɫ */

/* ��׼����߿���ɫ���� */
#define STD_WIN_RIM_OUTC            0x002A3033  /* ������ɫ */
#define STD_WIN_RIM_MIDC            0x0054666D  /* ������ɫ */
#define STD_WIN_RIM_INC             0x00303D42  /* ������ɫ */

static WM_HWIN __GetClient(WINDOW_Obj *pObj)
{
    return pObj->hClient;
}

/* Window�Ի溯�� */
static void __Paint(WM_HWIN hWin)
{
    i_16 x0, y0;
    u_16 xSize, ySize;
    GUI_COLOR Color;
    GUI_RECT Rect;
    WINDOW_Obj *pObj = hWin;

    Rect = ((WM_Obj*)hWin)->Rect;
    x0 = Rect.x0;
    y0 = Rect.y0;
    xSize = Rect.x1 - x0 + 1;
    ySize = Rect.y1 - y0 + 1;
    
    /* ������� */
    Color = pObj->Widget.Skin.EdgeColor[0];
    GUI_DrawRect(x0, y0, xSize, ySize, Color);
    /* �����п� */
    Color = pObj->Widget.Skin.EdgeColor[1];
    GUI_VertLine(x0 + 1, y0  + pObj->CaptionHeight + 1,
                       ySize - pObj->CaptionHeight - 2, Color);
    GUI_VertLine(x0 + xSize - 2, y0  + pObj->CaptionHeight + 1,
                       ySize - pObj->CaptionHeight - 2, Color);
    GUI_HoriLine(x0 + 1, y0  + ySize - 2, xSize - 2, Color);
    /* �����ڿ� */
    Color = pObj->Widget.Skin.EdgeColor[2];
    GUI_DrawRect(x0 + 2, y0 + pObj->CaptionHeight + 1,
                 xSize - 4, ySize - pObj->CaptionHeight - 3, 
                 Color);
    /* ���Ʊ����� */
    Color = pObj->Widget.Skin.CaptionColor[0];
    GUI_FillRect(x0 + 1, y0 + 1, xSize - 2,
                 pObj->CaptionHeight / 2, Color);
    Color = pObj->Widget.Skin.CaptionColor[1];
    GUI_FillRect(x0 + 1, y0 + pObj->CaptionHeight / 2 + 1,
                 xSize - 2, pObj->CaptionHeight / 2, Color);

    /* ���Ʊ��� */
    Color = pObj->Widget.Skin.FontColor[0];
    GUI_DspStringCurRectMiddle(x0 + 1, y0 + 1, xSize - 2,
                               pObj->CaptionHeight,
                               pObj->Title, Color, 
                               WIDGET_GetFont(pObj));
}

/* WINDOW���ý��㺯�� */
static void WINDOW_SetFocus(GUI_MESSAGE *pMsg)
{
    WINDOW_Obj *pObj = pMsg->hWin;

    if (!pMsg->Param) { /* ������һ�����뽹�� */
        if (pObj->hFocus == NULL || !((WM_Obj *)pObj->hFocus)->hNext) {
            pMsg->hWinSrc = ((WM_Obj *)pObj->hClient)->hFirstChild;
        } else {
            pMsg->hWinSrc = ((WM_Obj *)pObj->hFocus)->hNext;
        }
    } else if (pMsg->hWinSrc == NULL) { /* ֱ���������뽹�� */
        if (pObj->hFocus) {
            pMsg->hWinSrc = pObj->hFocus;
        } else {
            pMsg->hWinSrc = ((WM_Obj *)pObj->hClient)->hFirstChild;
        }
    }
    pObj->hFocus = WIDGET_SetFocus(pMsg);
}

/* WINDOW�ؼ��Զ��ص����� */
static void __Callback(WM_MESSAGE *pMsg)
{
    WM_CALLBACK *cb;

    switch (pMsg->MsgId) {
    case WM_PAINT:
        __Paint(pMsg->hWin);
        break;
    case WM_DELETE:
        return;
    case WM_GET_CLIENT:
        pMsg->Param = (GUI_PARAM)__GetClient(pMsg->hWin);
        break;
    case WM_SET_FOCUS:
        WINDOW_SetFocus(pMsg); /* ���ý��� */
        break;
    default:
        WM_DefaultProc(pMsg);
        cb = ((WINDOW_Obj*)pMsg->hWin)->UserCb;
        if (cb) {
            pMsg->hWin = pMsg->hWin;
            cb(pMsg);
        }
    }
}

/* �ͻ����Ի溯�� */
static void __PaintClient(WM_HWIN hWin)
{
    GUI_RECT *pr = &((WM_Obj*)hWin)->Rect;
    WINDOW_Obj *pObj = WM_GetParentHandle(hWin);

    /* ���Ʊ��� */
    GUI_FillRect(pr->x0, pr->y0, pr->x1 - pr->x0 + 1,
        pr->y1 - pr->y0 + 1, pObj->Widget.Skin.BackColor[0]);
}

static void __ClientCallback(WM_MESSAGE *pMsg)
{
    WM_HWIN hParent;
    WM_CALLBACK *cb;

    switch (pMsg->MsgId) {
    case WM_PAINT:
        __PaintClient(pMsg->hWin);
        break;
    case WM_DELETE:
        return;
    case WM_SET_FOCUS:
        pMsg->hWin = WM_GetParentHandle(pMsg->hWin);
        WINDOW_SetFocus(pMsg); /* ���ý��� */
        break;
    default:
        WM_DefaultProc(pMsg);
        hParent = WM_GetParentHandle(pMsg->hWin);
        cb = ((WINDOW_Obj*)hParent)->UserCb;
        if (cb) {
            pMsg->hWin = ((WM_Obj *)pMsg->hWin)->hParent;
            cb(pMsg);
        }
    }
}

static void __CreateClient(WINDOW_Obj *pObj)
{
    int xSize, ySize;
    GUI_RECT *pr = &pObj->Widget.Win.Rect;

    xSize = pr->x1 - pr->x0 - 5;
    ySize = pr->y1 - pr->y0 - pObj->CaptionHeight - 4;
    if (xSize < 0) {
        xSize = 0;
    }
    if (ySize < 0) {
        ySize = 0;
    }
    pObj->hClient = WM_CreateWindowAsChild(3, pObj->CaptionHeight + 2,
        (u_16)xSize, (u_16)ySize, pObj, 0, WIDGET_CLIENT, WM_NULL_ID,
        __ClientCallback, 0);
}

/*
 * �������ڿؼ�
 * x0:WINDOW�ؼ�����������(����ڸ�����)
 * y0:WINDOW�ؼ�����������(����ڸ�����)
 * xSize:WINDOW�ؼ���ˮƽ���
 * ySize:WINDOW�ؼ�����ֱ�߶�
 * hParent:�����ھ��
 * Id:����ID
 * Flag:����״̬
 * cb:�û��ص�����ָ��
 **/
WM_HWIN WINDOW_Create(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      WM_HWIN hParent,
                      u_16 Id,
                      u_8 Flag,
                      WM_CALLBACK *cb)
{
    WINDOW_Obj *pObj;
    
    GUI_LOCK();
    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, Flag,
        WIDGET_WINDOW, Id, __Callback, sizeof(WINDOW_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        GUI_UNLOCK();
        return NULL;
    }
    pObj->CaptionHeight = WINDOW_DEF_CAPHEIGHT;  /* �������߶� */
    /* ��ɫ */
    pObj->Widget.Skin.CaptionColor[0] = WINDOW_CAPTION_UPC;  /* �������ϰ벿�� */
    pObj->Widget.Skin.CaptionColor[1] = WINDOW_CAPTION_DOWNC;/* �������°벿�� */
    pObj->Widget.Skin.EdgeColor[0] = STD_WIN_RIM_OUTC;       /* ���� */
    pObj->Widget.Skin.EdgeColor[1] = STD_WIN_RIM_MIDC;       /* ���� */
    pObj->Widget.Skin.EdgeColor[2] = STD_WIN_RIM_INC;        /* ���� */
    pObj->Widget.Skin.BackColor[0] = WINDOW_BODY_BKC;        /* ��ɫ */
    pObj->Widget.Skin.FontColor[0] = WINDOW_TITLE_COLOR;
    pObj->Widget.Skin.FontColor[1] = WINDOW_FONT_COLOR;
    pObj->UserCb = cb;
    pObj->hFocus = NULL;
    pObj->hClient = NULL;
    WINDOW_SetTitle(pObj, ""); /* ���ó�ʼ�ַ��� */
    WINDOW_SetFont(pObj, GUI_DEF_FONT);
    __CreateClient(pObj); /* �����ͻ��� */
    WM_SendMessage(pObj->hClient, WM_CREATED, (GUI_PARAM)NULL);
    WM_SetFocusWindow(pObj);
    GUI_UNLOCK();
    return pObj;
}

/* WINDOW���ñ��� */
GUI_RESULT WINDOW_SetTitle(WM_HWIN hWin, const char *str)
{
    /* ����Ƿ�ΪWINDOW�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_WINDOW);
    ((WINDOW_Obj*)hWin)->Title = (char*)str;
    return GUI_OK;
}

/* WINDOW�������� */
GUI_RESULT WINDOW_SetFont(WM_HWIN hWin, GUI_FONT Font)
{
    /* ����Ƿ�ΪWINDOW�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_WINDOW);
    WIDGET_SetFont(hWin, Font);
    return GUI_OK;
}

/* WINDOW����Ϊ͸������ */
GUI_RESULT WINDOW_SetAllAlpha(WM_HWIN hWin, u_8 Alpha)
{
    WINDOW_Obj *pObj = hWin;
    
    /* ����Ƿ�ΪWINDOW�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_WINDOW);
    /* ����Alpha */
    WIDGET_Alpha(hWin, WIDGET_ALL, 0, Alpha);
    WIDGET_SetTransWindow(pObj->hClient);
    /* ������Ч�� */
    WM_Invalidate(hWin);
    WM_Invalidate(pObj->hClient);
    return GUI_OK;
}
