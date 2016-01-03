#ifndef __ICONVIEW_H
#define __ICONVIEW_H

#include "WIDGET.h"

typedef struct
{
    WIDGET Widget;
    u_16 IconNum;       /* ͼ������ */
    u_16 SelIndex;      /* ѡ�е����� */
    GUI_COLOR **BmpTab; /* ͼ��ͼƬ���� */
    u_16 IconWidth;     /* ͼ���� */
    u_16 IconHeight;    /* ͼ��߶� */
    u_16 DstncX;        /* ˮƽ��� */
    u_16 DstncY;        /* ��ֱ��� */
} ICONVIEW_Obj;

#endif

