#ifndef __GUI_DRIVER_H
#define __GUI_DRIVER_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"

#define GL_SetPixel       _GL_SetPixel
#define GL_GetPixel       _GL_GetPixel
#define GL_DrawBitmap     _GL_DrawBitmap
#define GL_SetPixelClip   GUI_glAPI.SetPixelClip
#define GL_DrawHLine      GUI_glAPI.DrawHLine
#define GL_DrawVLine      _GL_DrawVLine
#define GL_FillRect	      GUI_glAPI.FillRect

/* ��ͼָ�� */
typedef struct {
    i_16 x0, y0, x1, y1;
    u_16 xSize, ySize;
    u_32 Offset;
    GUI_COLOR Color;
    const unsigned char *pSrc;
    unsigned char *pDst;
    u_8 SrcFormat, DstFormat;
}GUI_FLIPOUT;

/* ͼ���豸�������� */
typedef struct {
    u_16 xSize; /* ��ʾ�豸�Ŀ�� */
    u_16 ySize; /* ��ʾ�豸�ĸ߶� */
    void (*SetPixel)(GUI_FLIPOUT *); /* ��ʾĳ������ */
    GUI_COLOR (*GetPixel)(GUI_FLIPOUT *);             /* ��ȡĳ������ */
    void (*DrawHLine)(GUI_FLIPOUT *);
    void (*DrawVLine)(GUI_FLIPOUT *);
    void (*FillRect)(GUI_FLIPOUT *);
    void (*DrawBitmap)(GUI_FLIPOUT *);
} GUI_DRIVER;

/* ͼ���豸����ṹ */
typedef struct {
    void *FrameBuffer; /* ֡���� */
    void *Position;    /* ��ǰ������λ�� */
    i_16 x0;           /* �������豸�е�x0 */
    i_16 y0;           /* �������豸�е�y0 */
    u_16 Width;
    u_16 Height;
    int BytesPerPixel; /* ��ɫ��� */
    int PixelFormat;   /* ������ɫ��ʽ */
    GUI_DRIVER Phy;    /* �������� */
} GUI_GDEV;

/* ��ͼAPI���� */
typedef struct {
    void (*SetPixelClip)(i_16, i_16);
    void (*DrawHLine)(i_16, i_16, i_16);
    void (*FillRect)(i_16, i_16, i_16, i_16);
} GUI_GLAPI;

extern GUI_DRIVER GUI_GDev;
extern GUI_GLAPI   GUI_glAPI;

void _GL_SetPixel(i_16 x, i_16 y, GUI_COLOR);
GUI_COLOR _GL_GetPixel(i_16 x, i_16 y);
void _GL_DrawVLine(i_16 x0, i_16 y0, i_16 y1);
void _GL_DrawBitmap(u_8 PixelFormat, const unsigned char *pPixel,
    i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, int Offset);
void GUI_DeviceInit(void);

#endif /* __GUI_DRIVER_H */
