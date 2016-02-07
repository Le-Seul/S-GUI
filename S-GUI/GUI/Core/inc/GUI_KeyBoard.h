#ifndef __GUI_KEYBOARD_H
#define __GUI_KEYBOARD_H

#include "GUI_Typedef.h"

/* 1~127Ϊ��׼ASCII��� */
/* 128~255Ϊ������ֵ */
#define GUI_KEY_UP       128
#define GUI_KEY_DOWN     129
#define GUI_KEY_LEFT     130
#define GUI_KEY_RIGHT    131
#define GUI_KEY_ENTER    134
#define GUI_KEY_RETURN   135

void GUI_KeyBufferInit(void); /* ���̻�������ʼ�� */
void GUI_KeyBufferDelete(void); /* ɾ�����̻����� */
void GUI_SendKeyMsg(u_8 Key); /* �Ѽ�ֵ�洢�����̻����� */
u_8 GUI_GetKey(void); /* �Ӽ��̻�������ȡ��ֵ */
void GUI_KeyProcess(void); /* ���̴��� */
void GUI_CleanKeyBuffer(void); /* ��ռ��̻����� */

#endif /* __GUI_KEYBOARD_H */
