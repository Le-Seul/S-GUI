#ifndef __WIDGET_H
#define __WIDGET_H

#include "GUI_WM.h"
#include "GUI_Font.h"

/* ���ڱ�־���� */
#define WIDGET_BUTTON          0x01
#define WIDGET_LISTBOX         0x02
#define WIDGET_WINDOW          0x03
#define WIDGET_TEXTBOX         0x04
#define WIDGET_SCROLLBAR       0x05

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

typedef struct {
    GUI_COLOR      EdgeColor[3];    /* �߿���ɫ */
    GUI_COLOR      BackColor[2];    /* �ڲ�������ɫ */
    GUI_COLOR      CaptionColor[2]; /* ������ɫ */
    GUI_FontType   Font;          /* ���� */
    GUI_COLOR      FontColor[2];  /* ������ɫ */
}WIDGET_SKIN;      /* ���ڿؼ�Ƥ�� */

typedef struct {
    WM_Obj Win;
    WIDGET_SKIN Skin;
}WIDGET;

void WIDGET_SetTransWindow(GUI_hWin hWin);
void WIDGET_Alpha(GUI_hWin hWin, u_8 Part, u_8 Id, u_8 Alpha);
GUI_FontType WIDGET_GetFont(GUI_hWin hWin);
void WIDGET_SetFont(GUI_hWin hWin, GUI_FontType Font);
GUI_COLOR WIDGET_GetBackColor(GUI_hWin hWin, u_8 n);
GUI_COLOR WIDGET_GetEdgeColor(GUI_hWin hWin, u_8 n);
GUI_COLOR WIDGET_GetCaptionColor(GUI_hWin hWin, u_8 n);
GUI_COLOR WIDGET_GetFontColor(GUI_hWin hWin, u_8 n);

#endif
