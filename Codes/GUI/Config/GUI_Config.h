#ifndef __GUI_CONFIG_H
#define __GUI_CONFIG_H

/* ������Ϣ���������Ϣ�� */
#define GUI_MSG_QUEUE_SIZE    10

/* Ĭ������ */
#define GUI_DEF_FONT          GUI_FontUI17_4pp

/* �����ϢǶ�ײ��� */
#define GUI_MAX_MSG_NEST      10

/* �ü���������Ѵ�С��ע�ⲻ��̫С��������ʵ��������ʱ��Ӧ��������
 * GUI_RECT_HEAP_SIZE ��ֵ���������û�н����ȥ�ų������ط������⡣
 **/
#define GUI_RECT_HEAP_SIZE    100

/* ʹ��ͼ�μ����� */
#define GUI_USE_GRAPHPHY      0

/* ����ģʽ���� */
#define GUI_DEBUG_MODE        1

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
