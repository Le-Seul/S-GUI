#ifndef __GUI_CORE_H
#define __GUI_CORE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"
#include "GUI_Botton.h"
#include "GUI_Queue.h"
#include "GUI_Font.h"

#define GUI_HEAP_FAST     0 /* ���ٵĶ�(MCU�ڲ���SRAM)ҳ�� */
#define GUI_HEAP_HCAP     1 /* �������Ķ�(����SDRAM)ҳ�� */

#define _hRootWin   (GUI_Data ? GUI_Data->RootWin : NULL)

#define _ClipRect   (GUI_Context.ClipRect)

#define GUI_DEBUG_OUT(s) GUI_DebugOut(s);

#define GUI_malloc(S)     GUI_Malloc((S), GUI_Heap[GUI_HEAP_HCAP])
#define GUI_free(P)       GUI_Free((P), GUI_Heap[GUI_HEAP_HCAP])
#define GUI_fastmalloc(S) GUI_Malloc((S), GUI_Heap[GUI_HEAP_FAST])
#define GUI_fastfree(P)   GUI_Free((P), GUI_Heap[GUI_HEAP_FAST])

/* ���� */
typedef struct { i_16 x0, y0, x1, y1; } GUI_RECT;
/* �� */
typedef struct { i_16 x, y; } GUI_POINT;

/* �������� */
typedef struct AREA_NODE {
    GUI_RECT Rect;
    struct AREA_NODE *pNext;
} AREA_NODE, *GUI_AREA;

typedef struct{
    GUI_PHY_INFO phy_info; /* Ӳ����Ϣ */
    GUI_HWIN RootWin;      /* ������ */
    GUI_AREA AreaHeap;     /* �ü������ */
}GUI_WORK_SPACE;

/* GUI�����Ľṹ�� */
typedef struct {
    GUI_HWIN hFocus;        /* ��ǰ�����뽹�� */
    GUI_RECT *InvalidRect;  /* ��ǰ����Ч���� */
    GUI_RECT DrawRect;      /* ��ǰҪ����ͼ�ε����ξ��� */
    GUI_RECT ClipRect;      /* ��ǰʵ���������Ļ�ϵĲü����� */
    GUI_AREA Area;          /* ��ǰ���ƴ��ڵĲü����� */
    GUI_AREA pAreaNode;     /* ��ǰ�Ĳü�����ڵ� */
    const GUI_FONT Font;    /* ��ǰ���� */
    GUI_COLOR FGColor;      /* ��ǰǰ��ɫ */
    GUI_COLOR BGColor;      /* ��ǰ����ɫ */
}GUI_CONTEXT;

extern void *GUI_Heap[2];
extern GUI_WORK_SPACE *GUI_Data;
extern GUI_CONTEXT GUI_Context;

GUI_RESULT GUI_Init(void);
void GUI_Unload(void);
void GUI_ScreenSize(u_16 *xSize, u_16 *ySize);
u_16 GUI_GetScreenWidth(void);
u_16 GUI_GetScreenHeight(void);
void GUI_Delay(GUI_TIME tms);
void GUI_LOCK(void);
void GUI_UNLOCK(void);
void GUI_DebugOut(const char *s);

void GUI_SetNowRectList(GUI_AREA l, GUI_RECT *p);
void GUI_DrawAreaInit(GUI_RECT *p);
GUI_BOOL GUI_GetNextArea(void);

#endif /* __GUI_CORE_H */
