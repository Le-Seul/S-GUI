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

WM_HWIN WINDOW_Create(int x0, int y0,  int xSize, int ySize,
    WM_HWIN hParent, int Id, u_8 Style, WM_CALLBACK *cb);
GUI_RESULT WINDOW_SetTitle(WM_HWIN hWin, const char *str);
GUI_RESULT WINDOW_SetFont(WM_HWIN hWin, GUI_FONT *Font);

#endif
