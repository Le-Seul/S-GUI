#ifndef __GUI_KEYBOARD_H
#define __GUI_KEYBOARD_H

#include "GUI_Typedef.h"
#include "GUI_Queue.h"

/* 1~127Ϊ��׼ASCII��� */
/* 128~255Ϊ������ֵ */
#define GUI_KEY_UP       128
#define GUI_KEY_DOWN     129
#define GUI_KEY_LEFT     130
#define GUI_KEY_RIGHT    131
#define GUI_KEY_ENTER    134
#define GUI_KEY_RETURN   135

void GUI_SendKey(u_8 Key);  /* �Ѽ�ֵ�洢�����̻����� */
GUI_RESULT GUI_KeyMessageProc(GUI_MESSAGE *pMsg); /* �����¼������� */

#endif /* __GUI_KEYBOARD_H */
