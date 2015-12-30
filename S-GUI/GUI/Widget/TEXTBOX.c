#include "TEXTBOX.h"
#include "GUI.h"

void TEXTBOX_LineFeedDisp( const char *str, GUI_COLOR Color, GUI_FontType Font, GUI_RECT *Rect);

/* �Ի溯�� */
static void __Paint(WM_hWin hWin)
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
    GUI_FillTailorRect(x0, y0, xSize, ySize, Color);
    /* �����ı����� */
    Color = pObj->Widget.Skin.FontColor[0];
    TEXTBOX_LineFeedDisp(pObj->Text, Color,  WIDGET_GetFont(pObj), &Rect);
}

/* WINDOW�ؼ��Զ��ص����� */
static void __Callback(WM_MESSAGE *pMsg)
{
    /* ����Ƿ�ΪWINDOW�ؼ� */
    if (WM_CheckWindowSign(pMsg->hWin, WIDGET_TEXTBOX)) {
        return;
    }
    switch (pMsg->MsgId) {
        case WM_PAINT :
            __Paint(pMsg->hWin);
            break;
        case WM_DELETE :
            GUI_fastfree(pMsg->hWin);
            break;
        case WM_TP_CHECKED :
            WM_SetActiveMainWindow(pMsg->hWin);
            break;
        case WM_TP_PRESS:
            break;
        case WM_TP_LEAVE:
            break;
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
WM_hWin TEXTBOX_Create(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      WM_hWin hParent,
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
    /* �����û��� */
    pObj->Widget.Win.UserRect.x0 = pObj->Widget.Win.Rect.x0;
    pObj->Widget.Win.UserRect.y0 = pObj->Widget.Win.Rect.y0;
    pObj->Widget.Win.UserRect.x1 = pObj->Widget.Win.Rect.x1;
    pObj->Widget.Win.UserRect.y1 = pObj->Widget.Win.Rect.y1;
    /* ��ɫ */
    pObj->Widget.Skin.BackColor[0] = TEXTBOX_DEF_BKC;
    pObj->Widget.Skin.FontColor[0] = TEXTBOX_TEXT_COLOR;
    TEXTBOX_SetText(pObj, "");      /* ���ó�ʼ�ַ��� */
    TEXTBOX_SetFont(pObj, GUI_DEF_FONT);
    
    return pObj;
}

/* TEXTBOX���ñ��� */
u_8 TEXTBOX_SetText(WM_hWin hWin, const char *str)
{
    GUI_RECT Rect;

    /* ����Ƿ�ΪTEXTBOX�ؼ� */
    if (WM_CheckWindowSign(hWin, WIDGET_TEXTBOX)) {
        return 1;
    }
    ((TEXTBOX_Obj*)hWin)->Text = (char*)str;
    Rect = WM_GetWindowAreaRect(hWin);
    hWin = WM_GetParentHandle(hWin);
    WM_InvalidateRect(hWin, &Rect);  /* ��������ʧЧ */
    return 0;
}

/* TEXTBOX�������� */
u_8 TEXTBOX_SetFont(WM_hWin hWin, GUI_FontType Font)
{
    /* ����Ƿ�ΪTEXTBOX�ؼ� */
    if (WM_CheckWindowSign(hWin, WIDGET_TEXTBOX)) {
        return 1;
    }
    WIDGET_SetFont(hWin, Font);
    return 0;
}

/* TEXTBOX����Ϊ͸������ */
void TEXTBOX_SetAllAlpha(WM_hWin hWin, u_8 Alpha)
{
    GUI_RECT Rect;
    
    WIDGET_Alpha(hWin, WIDGET_ALL, 0, Alpha);
    Rect = WM_GetWindowAreaRect(hWin);
    hWin = WM_GetParentHandle(hWin);
    WM_InvalidateRect(hWin, &Rect); /* ��������ʧЧ */
}

/* �Զ�������ʾ */
void TEXTBOX_LineFeedDisp( const char *str, GUI_COLOR Color, GUI_FontType Font, GUI_RECT *Rect)
{
    u_16 pix, ch_num, x_pix;
    i_16 x0, x1, y0;
    
    x0 = Rect->x0;
    y0 = Rect->y0;
    x1 = Rect->x1;
    x_pix = x1 - x0 + 1;
    while (*str) {
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
