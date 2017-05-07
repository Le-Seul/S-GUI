#ifndef __GUI_ROOTWIN_H
#define __GUI_ROOTWIN_H

#include "GUI_Typedef.h"

/* �û��Զ���ĸ����ڻ��ƺ��� */
extern void (*RootWinPaint_Cb)(GUI_HWIN hWin);
/* �û��Զ���ĸ����ڶ�ʱ�������� */
extern void (*RootWinTimer_Cb)(GUI_HWIN hWin);

void WM_RootWindowInit(GUI_HWIN hWin);
void GUI_SetRootWindowTimer(GUI_TIME timer);

#endif
