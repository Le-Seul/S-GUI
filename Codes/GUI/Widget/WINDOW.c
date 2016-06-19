#include "WINDOW.h"
#include "GUI.h"

#define WINDOW_DEF_CAPHEIGHT   20
/* Ĭ�ϵĴ���caption����ɫcaption����ɫ. */
/* �Լ����屳��ɫ. */
#define WINDOW_CAPTION_COLOR1       0x0042789B  /* ����CAPTION���ɫ */
#define WINDOW_CAPTION_COLOR2       0x00FFFFFF  /* ���ڷǻ��ɫ */
#define WINDOW_TITLE_COLOR1         0x00FFFFFF  /* ���ڱ�����ɫ */
#define WINDOW_TITLE_COLOR2         0x00909090  /* ���ڱ�����ɫ */
#define WINDOW_BODY_BKC             0x00FFFFFF  /* ���ڵ�ɫ */

/* ��׼����߿���ɫ���� */
#define WINDOW_EDGE_COLOR           0x002A3033  /* ������ɫ */

static WM_HWIN __GetClient(WINDOW_Obj *pObj)
{
    return pObj->hClient;
}

/* Window�Ի溯�� */
static void __Paint(WM_HWIN hWin)
{
    u_16 xSize, ySize;
    GUI_COLOR Color;
    GUI_RECT Rect;
    WINDOW_Obj *pObj = hWin;

    GUI_GetClientRect(&Rect);
    xSize = Rect.x1 + 1;
    ySize = Rect.y1 + 1;
    
    /* ���Ʊ����� */
    if (pObj->Widget.Win.hNext) {
        Color = pObj->Widget.Skin.CaptionColor[1];
        GUI_SetFontColor(pObj->Widget.Skin.FontColor[1]);
    } else {
        Color = pObj->Widget.Skin.CaptionColor[0];
        GUI_SetFontColor(pObj->Widget.Skin.FontColor[0]);
    }
    GUI_FillRect(1, 1, xSize - 2, pObj->CaptionHeight - 1, Color);
    /* ���Ʊ��� */
    GUI_SetFont(WIDGET_GetFont(pObj));
    GUI_Val2Rect(&Rect, 2, 1, xSize - 4, pObj->CaptionHeight);
    GUI_DispStringInRect(&Rect, pObj->Title, GUI_ALIGN_VCENTER); /* ��ֱ���� */
    /* ���Ʊ߿� */
    if (pObj->Widget.Win.hNext) {
        Color = pObj->Widget.Skin.FontColor[1];
    } else {
        Color = pObj->Widget.Skin.CaptionColor[0];
    }
    GUI_DrawRect(0, 0, xSize, ySize, Color);
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
    WM_CALLBACK *Cb;

    Cb = ((WINDOW_Obj*)pMsg->hWin)->UserCb;
    switch (pMsg->MsgId) {
    case WM_PAINT:
        __Paint(pMsg->hWin);
        break;
    case WM_GET_CLIENT:
        pMsg->Param = (GUI_PARAM)__GetClient(pMsg->hWin);
        break;
    case WM_SET_FOCUS:
        WINDOW_SetFocus(pMsg); /* ���ý��� */
        break;
    default:
        if (Cb) {
            Cb(pMsg);
        }
        WM_DefaultProc(pMsg);
    }
}

/* �ͻ����Ի溯�� */
static void __PaintClient(WM_HWIN hWin)
{
    GUI_RECT Rect;
    WINDOW_Obj *pObj = WM_GetParentHandle(hWin);

    GUI_GetClientRect(&Rect);
    /* ���Ʊ��� */
    GUI_FillRect(0, 0, Rect.x1 + 1, Rect.y1 + 1,
        pObj->Widget.Skin.BackColor[0]);
}

static void __ClientCallback(WM_MESSAGE *pMsg)
{
    WM_HWIN hParent;
    WM_CALLBACK *Cb;

    hParent = WM_GetParentHandle(pMsg->hWin);
    switch (pMsg->MsgId) {
    case WM_PAINT:
        __PaintClient(pMsg->hWin);
        break;
    case WM_DELETE:
        break;
    case WM_SET_FOCUS:
        pMsg->hWin = hParent;
        WINDOW_SetFocus(pMsg); /* ���ý��� */
        break;
    case WM_TP_PRESS: /* ���ƶ����� */
        break;
    case WM_TP_LEAVE: /* ���ƶ����� */
        break;
    default:
        Cb = ((WINDOW_Obj*)hParent)->UserCb;
        if (Cb) {
            pMsg->hWin = hParent;
            Cb(pMsg);
        }
        WM_DefaultProc(pMsg);
    }
}

static void __CreateClient(WINDOW_Obj *pObj)
{
    int xSize, ySize;
    GUI_RECT *pr = &pObj->Widget.Win.Rect;

    xSize = pr->x1 - pr->x0 - 1;
    ySize = pr->y1 - pr->y0 - pObj->CaptionHeight;
    if (xSize < 0) {
        xSize = 0;
    }
    if (ySize < 0) {
        ySize = 0;
    }
    pObj->hClient = WM_CreateWindowAsChild(1, pObj->CaptionHeight,
        (u_16)xSize, (u_16)ySize, pObj, 0, WM_NULL_ID, __ClientCallback, 0);
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
    u_8 Style,
    WM_CALLBACK *cb)
{
    WINDOW_Obj *pObj;
    
    GUI_LOCK();
    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent,
        Style, Id, __Callback, sizeof(WINDOW_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        GUI_UNLOCK();
        return NULL;
    }
    pObj->CaptionHeight = WINDOW_DEF_CAPHEIGHT;  /* �������߶� */
    /* ��ɫ */
    pObj->Widget.Skin.CaptionColor[0] = WINDOW_CAPTION_COLOR1;  /* ������ */
    pObj->Widget.Skin.CaptionColor[1] = WINDOW_CAPTION_COLOR2;
    pObj->Widget.Skin.EdgeColor[0] = WINDOW_EDGE_COLOR;        /* ���� */
    pObj->Widget.Skin.BackColor[0] = WINDOW_BODY_BKC;          /* ��ɫ */
    pObj->Widget.Skin.FontColor[0] = WINDOW_TITLE_COLOR1;
    pObj->Widget.Skin.FontColor[1] = WINDOW_TITLE_COLOR2;
    pObj->UserCb = cb;
    pObj->hFocus = NULL;
    pObj->hClient = NULL;
    WINDOW_SetTitle(pObj, ""); /* ���ó�ʼ�ַ��� */
    WINDOW_SetFont(pObj, &GUI_DEF_FONT);
    __CreateClient(pObj); /* �����ͻ��� */
    WM_SendMessage(pObj, WM_CREATED, (GUI_PARAM)NULL);
    WM_SetFocusWindow(pObj);
    GUI_UNLOCK();
    return pObj;
}

/* WINDOW���ñ��� */
GUI_RESULT WINDOW_SetTitle(WM_HWIN hWin, const char *str)
{
    if (hWin) {
        ((WINDOW_Obj*)hWin)->Title = (char*)str;
        return GUI_OK;
    }
    return GUI_ERR;
}

/* WINDOW�������� */
GUI_RESULT WINDOW_SetFont(WM_HWIN hWin, GUI_FONT *Font)
{
    WIDGET_SetFont(hWin, Font);
    return GUI_OK;
}

/* WINDOW����Ϊ͸������ */
GUI_RESULT WINDOW_SetAllAlpha(WM_HWIN hWin, u_8 Alpha)
{
    WINDOW_Obj *pObj = hWin;
    
    /* ����Alpha */
    WIDGET_Alpha(hWin, WIDGET_ALL, 0, Alpha);
    WIDGET_SetTransWindow(pObj->hClient);
    /* ������Ч�� */
    WM_Invalidate(hWin);
    WM_Invalidate(pObj->hClient);
    return GUI_OK;
}
