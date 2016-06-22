#ifndef __WINDOW_H
#define __WINDOW_H

#include "WIDGET.h"

typedef struct {
    WIDGET Widget;
    WM_HWIN hClient;        /* �ͻ������ */
    WM_HWIN hFocus;         /* ���ڵ�ǰ���� */
    WM_HWIN hBtn;           /* �رհ�ť */
    char *Title;
    u_8  CaptionHeight;     /* �������߶� */
    WM_CALLBACK *UserCb;    /* �û����ܻص����� */
}WINDOW_Obj;

WM_HWIN WINDOW_Create(i_16 x0, i_16 y0,  u_16 xSize, u_16 ySize,
    WM_HWIN hParent, u_16 Id, u_8 Style, WM_CALLBACK *cb);
GUI_RESULT WINDOW_SetTitle(WM_HWIN hWin, const char *str);
GUI_RESULT WINDOW_SetFont(WM_HWIN hWin, GUI_FONT *Font);
GUI_RESULT WINDOW_SetAllAlpha(WM_HWIN hWin, u_8 Alpha);

#endif
