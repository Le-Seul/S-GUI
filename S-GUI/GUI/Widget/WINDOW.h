#ifndef __WINDOW_H
#define __WINDOW_H

#include "GUI.h"

#define WINDOW_DEF_CAPHEIGHT   32
/* Ĭ�ϵĴ���caption����ɫcaption����ɫ. */
/* �Լ����屳��ɫ. */
#define WINDOW_CAPTION_UPC          0x00524C3C  /* ����CAPTION�ϲ������ɫ */
#define WINDOW_CAPTION_DOWNC        0x00423E33  /* ����CAPTION�²������ɫ */
#define WINDOW_TITLE_COLOR          0x00CDCD96  /* ���ڱ�����ɫ */
#define WINDOW_BODY_BKC             0x00232329  /* ���ڵ�ɫ */
#define WINDOW_FONT_COLOR           0x00FFFFFF  /* ����������ɫ */

/* ��׼����߿���ɫ���� */
#define STD_WIN_RIM_OUTC            0x0033302A  /* ������ɫ */
#define STD_WIN_RIM_MIDC            0x006D6654  /* ������ɫ */
#define STD_WIN_RIM_INC             0x00423D30  /* ������ɫ */

typedef struct {
    WIDGET Widget;
    char *Title;
    u_8  CaptionHeight;     /* �������߶� */
    WM_CALLBACK *UserCb;    /* �û����ܻص����� */
}WINDOW_Obj;

WM_hWin WINDOW_Create(i_16 x0, i_16 y0,  u_16 xSize, u_16 ySize,
                      WM_hWin hParent, u_16 Id, u_8 Flag, WM_CALLBACK *cb);
u_8 WINDOW_SetTitle(WM_hWin hWin, const char *str);
u_8 WINDOW_SetFont(WM_hWin hWin, GUI_FontType Font);
void WINDOW_SetAllAlpha(WM_hWin hWin, u_8 Alpha);

#endif

