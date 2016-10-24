#ifndef __WIDGET_H
#define __WIDGET_H

#include "GUI_WM.h"
#include "GUI_Font.h"

#define WIDGET_DRAW_EDGE1      0x0000
#define WIDGET_DRAW_EDGE2      0x0001
#define WIDGET_DRAW_EDGE3      0x0002

#define WIDGET_DRAW_BACK1      0x0010
#define WIDGET_DRAW_BACK2      0x0011

#define WIDGET_DRAW_FONT1      0x0020
#define WIDGET_DRAW_FONT2      0x0021

/* WIDGETƤ��λ�ö��� */
#define WIDGET_ALL             0x00
#define WIDGET_EDGE            0x01
#define WIDGET_BACK            0x02
#define WIDGET_CAPTION         0x03
#define WIDGET_FONT            0x04

typedef void WIDGET_PAINT(GUI_HWIN);

typedef struct {
    GUI_FONT      *Font;            /* ���� */
    WIDGET_PAINT  *Paint;           /* ���ƻص����� */
} WIDGET_SKIN;                      /* ���ڿؼ�Ƥ�� */

typedef struct {
    WM_Obj Win;
    WIDGET_SKIN Skin;
} WIDGET;

void WIDGET_SetFont(GUI_HWIN hWin, GUI_FONT *Font);
GUI_FONT *WIDGET_GetFont(GUI_HWIN hWin);
GUI_RESULT WIDGET_SetPaintFunction(GUI_HWIN hWin, WIDGET_PAINT *Paint);
void WIDGET_Paint(GUI_HWIN hWin);

#endif
