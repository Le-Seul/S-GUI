#ifndef __ROOTWINDOW_H
#define __ROOTWINDOW_H

#include "GUI.h"

/* �û��Զ���ĸ����ڻ��ƺ��� */
extern void (*RootWinPaint_Cb)(WM_hWin hWin);
/* �û��Զ���ĸ����ڶ�ʱ�������� */
extern void (*RootWinTimer_Cb)(WM_hWin hWin);

void WM_RootWindowInit(WM_Obj *pObj);

#endif
