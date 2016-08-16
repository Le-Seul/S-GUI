#ifndef __GUI_CORE_H
#define __GUI_CORE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"
#include "GUI_Botton.h"
#include "GUI_Queue.h"
#include "GUI_Font.h"
#include "GUI_Surface.h"

#define GUI_ALIGN_TOP     0x01 /* �������� */
#define GUI_ALIGN_BOTTOM  0x02 /* �ײ����� */
#define GUI_ALIGN_VCENTER 0x04 /* ��ֱ���Ķ��� */
#define GUI_ALIGN_LEFT    0x08 /* ����� */
#define GUI_ALIGN_RIGHT   0x10 /* �Ҷ��� */
#define GUI_ALIGN_HCENTER 0x20 /* ˮƽ���� */

#define GUI_HEAP_FAST     0 /* ���ٵĶ�(MCU�ڲ���SRAM)ҳ�� */
#define GUI_HEAP_HCAP     1 /* �������Ķ�(����SDRAM)ҳ�� */

#define _hRootWin   GUI_RootWin

#define _ClipRect   (GUI_Context.ClipRect)

#define GUI_DEBUG_OUT(s) GUI_DebugOut(s);

/* �������� */
typedef struct AREA_NODE {
    GUI_RECT Rect;
    struct AREA_NODE *pNext;
} AREA_NODE, *GUI_AREA;

/* GUI�����Ľṹ�� */
typedef struct {
    GUI_HWIN hWin;          /* ��ǰ���ƴ��� */
    GUI_POINT WinPos;       /* ����ԭ��������� */
    GUI_HWIN hActive;       /* ��ǰ����� */
    GUI_RECT *InvalidRect;  /* ��ǰ���ڵĵ���Ч���� */
    GUI_RECT DrawRect;      /* ��ǰҪ����ͼ�η�Χ�ľ��� */
    GUI_RECT ClipRect;      /* ��ǰ��Ļ�ϵĲü����� */
    GUI_AREA Area;          /* ��ǰ���ƴ��ڵļ����� */
    GUI_AREA pAreaNode;     /* ��ǰ�ļ�����ڵ� */
    GUI_FONT *Font;         /* ��ǰ���� */
    GUI_COLOR FGColor;      /* ��ǰǰ��ɫ */
    GUI_COLOR BGColor;      /* ��ǰ����ɫ */
    GUI_COLOR FontColor;    /* ��ǰ������ɫ */
    int PenSize;            /* �����߿� */
	int AAFactor;           /* ��������� */
	int AAEnable;           /* �����ģʽ�Ѿ����� */
	int AAHL;               /* ʹ�ø߷ֱ������� */
    GUI_SURFACE *Surface;   /* ��ǰ������� */
}GUI_CONTEXT;

extern GUI_CONTEXT GUI_Context;
extern GUI_HWIN GUI_RootWin;
extern GUI_AREA GUI_AreaHeap;

GUI_RESULT GUI_Init(void);
void GUI_Unload(void);
void GUI_ScreenSize(u_16 *xSize, u_16 *ySize);
u_16 GUI_GetScreenWidth(void);
u_16 GUI_GetScreenHeight(void);
void GUI_Delay(GUI_TIME tms);
void GUI_LOCK(void);
void GUI_UNLOCK(void);
void GUI_DebugOut(const char *s);
GUI_BOOL GUI_StartPaint(GUI_HWIN hWin, GUI_CONTEXT *Backup);
void GUI_EndPaint(GUI_CONTEXT *Backup);
GUI_HWIN GUI_GetPaintWindow(void);
GUI_AREA GUI_GetClipArea(void);
void GUI_DrawAreaInit(GUI_RECT *p);
GUI_BOOL GUI_GetNextArea(void);
void GUI_ClientToScreen(i_16 *x, i_16 *y);
void GUI_ScreenToClient(i_16 *x, i_16 *y);
void GUI_ClientToScreenRect(GUI_RECT *pRect);
void GUI_GetClientRect(GUI_RECT *pRect);
void GUI_SetFont(GUI_FONT *Font);
void GUI_SetBGColor(GUI_COLOR Color);
void GUI_SetFGColor(GUI_COLOR Color);
void GUI_SetFontColor(GUI_COLOR Color);
void GUI_SetPenSize(int Width);
void GUI_SetAAFactor(int Factor);

#endif /* __GUI_CORE_H */
