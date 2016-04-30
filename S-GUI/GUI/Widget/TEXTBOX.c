#include "TEXTBOX.h"
#include "GUI.h"

#define TEXTBOX_DEF_BKC             0x00292323  /* ������ɫ */
#define TEXTBOX_TEXT_COLOR          0x00FFFFFF  /* ������ɫ */

static void TEXTBOX_LineFeedDisp( const char *str, GUI_COLOR Color, GUI_FONT Font, GUI_RECT *Rect);

/* �Ի溯�� */
static void __Paint(WM_HWIN hWin)
{
    i_16 x0, y0;
    u_16 xSize, ySize;
    GUI_COLOR Color;
    GUI_RECT Rect;
    TEXTBOX_Obj *pObj = hWin;

    Rect = ((WM_Obj*)hWin)->Rect;
    x0 = Rect.x0;
    y0 = Rect.y0;
    xSize = Rect.x1 - x0 + 1;
    ySize = Rect.y1 - y0 + 1;
    /* �����ڲ����� */
    Color = pObj->Widget.Skin.BackColor[0];
    GUI_FillRect(x0, y0, xSize, ySize, Color);
    /* �����ı����� */
    Color = pObj->Widget.Skin.FontColor[0];
    TEXTBOX_LineFeedDisp(pObj->Text, Color,  WIDGET_GetFont(pObj), &Rect);
}

/* TEXTBOX�ؼ��Զ��ص����� */
static void __Callback(WM_MESSAGE *pMsg)
{
    /* ����Ƿ�ΪTEXTBOX�ؼ� */
    WIDGET_SignErrorReturnVoid(pMsg->hWin, WIDGET_TEXTBOX);
    switch (pMsg->MsgId) {
        case WM_PAINT :
            __Paint(pMsg->hWin);
            break;
        case WM_DELETE:
            GUI_fastfree(((TEXTBOX_Obj*)pMsg->hWin)->Text);
            break;
        case WM_TP_CHECKED :
            WM_SetForegroundWindow(pMsg->hWin);
            break;
        case WM_TP_PRESS:
            break;
        case WM_TP_LEAVE:
            break;
        default:
            WM_DefaultProc(pMsg);
    }
}

/*
 * �����ı���
 * x0:TEXTBOX�ؼ�����������(����ڸ�����)
 * y0:TEXTBOX�ؼ�����������(����ڸ�����)
 * xSize:TEXTBOX�ؼ���ˮƽ���
 * ySize:TEXTBOX�ؼ�����ֱ�߶�
 * hParent:�����ھ��
 * Id:����ID
 * Flag:����״̬
 * cb:�û��ص�����ָ��
 **/
WM_HWIN TEXTBOX_Create(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      WM_HWIN hParent,
                      u_16 Id,
                      u_8 Flag)
{
    TEXTBOX_Obj *pObj;
    
    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, Flag,
                                  WIDGET_TEXTBOX, Id, __Callback,
                                  sizeof(TEXTBOX_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        return NULL;
    }
    /* ��ɫ */
    pObj->Widget.Skin.BackColor[0] = TEXTBOX_DEF_BKC;
    pObj->Widget.Skin.FontColor[0] = TEXTBOX_TEXT_COLOR;
    pObj->Text = NULL;
    TEXTBOX_SetFont(pObj, GUI_DEF_FONT);
    
    return pObj;
}

/* TEXTBOX�������� */
GUI_RESULT TEXTBOX_SetText(WM_HWIN hWin, const char *str)
{
    GUI_RECT Rect;

    /* ����Ƿ�ΪTEXTBOX�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_TEXTBOX);
    GUI_fastfree(((TEXTBOX_Obj*)hWin)->Text);
    ((TEXTBOX_Obj*)hWin)->Text = GUI_fastmalloc(GUI_Strlen(str) + 1);
    GUI_Strcpy(((TEXTBOX_Obj*)hWin)->Text, str);
    Rect = WM_GetWindowAreaRect(hWin);
    hWin = WM_GetParentHandle(hWin);
    WM_InvalidateRect(hWin, &Rect);  /* ��������ʧЧ */
    return GUI_OK;
}

/* TEXTBOX�������� */
GUI_RESULT TEXTBOX_SetFont(WM_HWIN hWin, GUI_FONT Font)
{
    /* ����Ƿ�ΪTEXTBOX�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_TEXTBOX);
    WIDGET_SetFont(hWin, Font);
    return GUI_OK;
}

/* TEXTBOX����Ϊ͸������ */
GUI_RESULT TEXTBOX_SetAllAlpha(WM_HWIN hWin, u_8 Alpha)
{
    GUI_RECT Rect;
    
    /* ����Ƿ�ΪTEXTBOX�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_TEXTBOX);
    WIDGET_Alpha(hWin, WIDGET_ALL, 0, Alpha);
    Rect = WM_GetWindowAreaRect(hWin);
    hWin = WM_GetParentHandle(hWin);
    WM_InvalidateRect(hWin, &Rect); /* ��������ʧЧ */
    return GUI_OK;
}

/* �Զ�������ʾ */
static void TEXTBOX_LineFeedDisp( const char *str, GUI_COLOR Color, GUI_FONT Font, GUI_RECT *Rect)
{
    u_16 pix, ch_num, x_pix;
    i_16 x0, x1, y0;
    
    x0 = Rect->x0;
    y0 = Rect->y0;
    x1 = Rect->x1;
    x_pix = x1 - x0 + 1;
    while (str && *str) {
        pix = GUI_SkipWord(str, Font, x_pix, &ch_num);
        if (x0 + pix > x1 || *str == '\n') {
            x0 = Rect->x0;
            y0 += Font->CharHeight;
        }
        if (*str == '\n') {
            str += ch_num;
            continue;
        }
        GUI_DispStringCurRectN(x0, y0 , str, Color,  Font, ch_num);
        str += ch_num;
        x0 += pix;
        
    }
}
