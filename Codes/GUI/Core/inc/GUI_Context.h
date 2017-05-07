#ifndef __GUI_CONTEXT_H
#define __GUI_CONTEXT_H

#include "GUI_Typedef.h"
#include "GUI_Font.h"
#include "GUI_ClipAreaHeap.h"

/* GUI�����Ľṹ�� */
typedef struct {
    GUI_HWIN hWin;          /* ��ǰ���ƴ��� */
    GUI_POINT winPos;       /* ����ԭ��������� */
    GUI_HWIN hActive;       /* ��ǰ����� */
    GUI_RECT *invalidRect;  /* ��ǰ���ڵĵ���Ч���� */
    GUI_RECT drawRect;      /* ��ǰҪ����ͼ�η�Χ�ľ��� */
    GUI_RECT clipRect;      /* ��ǰ��Ļ�ϵĲü����� */
    GUI_AREA area;          /* ��ǰ���ƴ��ڵļ����� */
    GUI_AREA pAreaNode;     /* ��ǰ�ļ�����ڵ� */
    GUI_FONT *font;         /* ��ǰ���� */
    GUI_COLOR foreground;   /* ��ǰǰ��ɫ */
    GUI_COLOR background;   /* ��ǰ����ɫ */
    GUI_COLOR fontColor;    /* ��ǰ������ɫ */
    int penSize;            /* �����߿� */
}GUI_CONTEXT;

extern GUI_CONTEXT gui_context;   /* GUI context */

/* inline functions */

/* set current font */
static inline void GUI_SetFont(GUI_FONT *Font)
{
    gui_context.font = Font;
}

/* get current font */
static inline GUI_FONT* GUI_CurrentFont(void)
{
    return gui_context.font;
}

/* get current background color */
static inline GUI_COLOR GUI_CurrentBackground(void)
{
    return gui_context.background;
}

/* set current background color */
static inline void GUI_SetBackground(GUI_COLOR Color)
{
    gui_context.background = Color;
}

/* get current foreground color */
static inline GUI_COLOR GUI_CurrentForeground(void)
{
    return gui_context.foreground;
}

/* set current foreground color */
static inline void GUI_SetForeground(GUI_COLOR Color)
{
    gui_context.foreground = Color;
}

/* set current font color */
static inline void GUI_SetFontColor(GUI_COLOR Color)
{
    gui_context.fontColor = Color;
}

/* get current font color */
static inline GUI_COLOR GUI_CurrentFontColor(void)
{
    return gui_context.fontColor;
}

/* set current brush size */
static inline void GUI_SetPenSize(int Width)
{
    gui_context.penSize = Width;
}

/* get current brush size */
static inline int GUI_PenSize(void)
{
    return gui_context.penSize;
}

/* get current clip-rect */
static inline GUI_RECT* GUI_CurrentClipRect(void)
{
    return &gui_context.clipRect;
}

/* set current active window */
static inline void GUI_SetActiveWindow(GUI_HWIN hWin)
{
    gui_context.hActive = hWin;
}

/* get current active window */
static inline GUI_HWIN GUI_CurrentActiveWindow(void)
{
    return gui_context.hActive;
}

/* get current paint window */
static inline GUI_HWIN GUI_CurrentPaintWindow(void)
{
    return gui_context.hWin;
}

/* get current clip-area */
static inline GUI_AREA GUI_CurrentClipArea(void)
{
    return gui_context.area;
}

void GUI_ClientToScreen(int *x, int *y);
void GUI_ClientToScreenRect(GUI_RECT *pRect);
void GUI_ScreenToClient(int *x, int *y);
void GUI_GetClientRect(GUI_RECT *pRect);

#endif
