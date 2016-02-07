#ifndef __GUI_CORE_H
#define __GUI_CORE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"
#include "GUI_Botton.h"
#include "GUI_Queue.h"

#define _hRootWin   (GUI_Data->RootWin)
#define _PaintArea  (GUI_Data->PaintArea)

/* ���� */
typedef struct { i_16 x0, y0, x1, y1; } GUI_RECT;
/* �� */
typedef struct { i_16 x, y; } GUI_POINT;

/* �������� */
typedef struct RECT_NODE {
    GUI_RECT Rect;
    struct RECT_NODE *pNext;
} RECT_NODE, *RECT_LIST;

typedef struct{
    GUI_PHY_INFO phy_info; /* Ӳ����Ϣ */
    GUI_TIME guitime;      /* GUI����ʱ�� */
    RECT_LIST RectList;    /* �ü�����˽�ж� */
    RECT_LIST NowRectList; /* ��ǰ���ƵĲü������� */
    GUI_RECT *PaintArea;   /* ��ǰ���Ƶ����� */
    GUI_hWin RootWin;      /* ������ */
    GUI_QUEUE *MsgQueue;   /* GUI��Ϣ���� */
#if GUI_USE_MEMORY
    GUI_COLOR *lcdbuf; /* LCD���� */
#endif
}GUI_WORK_SPACE;

extern GUI_WORK_SPACE *GUI_Data;

GUI_RESULT GUI_Init(void);
void GUI_Unload(void);
void GUI_ScreenSize(u_16 *xSize, u_16 *ySize);
u_16 GUI_GetScreenWidth(void);
u_16 GUI_GetScreenHeight(void);
void GUI_Delay(GUI_TIME tms);
GUI_RESULT GUI_GetMessage(GUI_MESSAGE *pMsg);
GUI_RESULT GUI_PostMessage(GUI_MESSAGE *pMsg);
void GUI_LOCK(void);
void GUI_UNLOCK(void);

void GUI_SetNowRectList(RECT_LIST l, GUI_RECT *p);
void GUI_DrawAreaInit(GUI_RECT *p);
GUI_RECT *GUI_GetNowArea(void);
u_8 GUI_GetNextArea(GUI_RECT *pRect);

#endif /* __GUI_CORE_H */
