#ifndef __GUI_SURFACE_H
#define __GUI_SURFACE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"

typedef struct {
    void *FrameBuffer; /* ֡���� */
    GUI_POINT Pos;     /* ���ƫ�� */
    u_16 Width;        /* ��� */
    u_16 Height;       /* �߶� */
    int BytesPerPixel; /* ��ɫ��� */
    int PixelFormat;   /* ���ظ�ʽ */
} GUI_SURFACE;

#endif
