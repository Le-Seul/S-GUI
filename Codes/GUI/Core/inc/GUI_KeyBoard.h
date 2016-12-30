#ifndef __GUI_KEYBOARD_H
#define __GUI_KEYBOARD_H

#include "GUI_Typedef.h"
#include "GUI_Queue.h"

#define GUI_KEYUP        0x00
#define GUI_KEYDOWN      0x01

/* 0x00 ~ 0x7FΪ��׼ASCII��� */
#define KEY_BACK         0x08 /* �˸�� */
#define KEY_TAB          0x09 /* TAB�� */
#define KEY_ENTER        0x0D /* �س� */
#define KEY_SHIFT        0x10 /* Shift�� */
#define KEY_SPACE        0x20 /* �ո� */
/* 0x80 ~ 0xFFΪ��չ��ֵ */
#define KEY_ESC          0x80 /* ESC�� */
#define KEY_DELETE       0x81 /* Delete�� */
#define KEY_INSERT       0x82 /* Insert�� */
#define KEY_PGUP         0x83 /* Page Up�� */
#define KEY_PGDN         0x84 /* Page Down�� */
#define KEY_END          0x85 /* End�� */
#define KEY_HOME         0x86 /* Home�� */
#define KEY_LEFT         0x90 /* �� */
#define KEY_UP           0x91 /* �� */
#define KEY_RIGHT        0x92 /* �� */
#define KEY_DOWN         0x93 /* �� */
#define KEY_LBUTTON      0x94 /* ������ */
#define KEY_RBUTTON      0x95 /* ����Ҽ� */
#define KEY_MBUTTON      0x96 /* ����м� */

void GUI_SendKey(u_8 Key, u_8 Status);
GUI_RESULT GUI_KeyMessageProc(GUI_MESSAGE *pMsg);

#endif /* __GUI_KEYBOARD_H */
