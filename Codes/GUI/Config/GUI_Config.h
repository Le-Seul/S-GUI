#ifndef __GUI_CONFIG_H
#define __GUI_CONFIG_H

/* ������Ϣ���������Ϣ�� */
#define GUI_MSG_QUEUE_SIZE    10

/* Ĭ������ */
#define GUI_DEF_FONT          GUI_FontUI17_4pp

/* �����ϢǶ�ײ��� */
#define GUI_MAX_MSG_NEST      10

/* ����ģʽ���� */
#define GUI_DEBUG_MODE        1

/* �Զ�ά��ʱ�� */
#define GUI_AUTO_MANAGE_TIME  5000

/* ���ƶ����ʱ֧�ֵ������� */
#define GUI_MAX_POLY_CORNERS  1000

/*
 @ �ڴ����Ԫ�Ƿ�ʹ�õ����Ļ�����.
 @ ����û���Ҫ�������̵߳���ʹ��GUI_Malloc()��GUI_Free()������Ҫ������ѡ��.
 */
#define GUI_USE_MEM_LOCK     1

/**
 @ ������ʾģʽ
 @ 0:Ĭ�ϵ�ģʽ.
 @ 1:�м���(ʹ��λͼ����).
 @ 2:�����ַ�����(ʹ��λͼ����).
 **/
#define GUI_DISP_FONT_MODE    0

/**
 @ ��������ַ����,GUI_DISP_FONT_MODE >= 1ʱ��Ч
 **/
#define GUI_MAX_FONT_WID      24

/**
 @ ��������ַ����,GUI_DISP_FONT_MODE == 2ʱ��Ч
 **/
#define GUI_MAX_FONT_HEI      48

#endif
