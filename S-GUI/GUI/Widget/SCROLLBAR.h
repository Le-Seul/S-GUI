#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H

#include "GUI.h"

typedef struct {
    WIDGET Widget;
    u_16   Totality;   /* ���� */
    u_16   Loation;    /* ��ǰλ�� */
    u_16   MinThick;   /* ������С�߶� */
}SCROLLBAR_Obj;

WM_hWin SCROLLBAR_Create(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      WM_hWin hParent,
                      u_16 Id,
                      u_8 Flag);
GUI_RESULT SCROLLBAR_SetTotality(GUI_hWin hWin, u_16 Totality);
GUI_RESULT SCROLLBAR_SetLoation(GUI_hWin hWin, u_16 Loation);

#endif
