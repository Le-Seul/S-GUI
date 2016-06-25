#ifndef __GUI_DRIVER_H
#define __GUI_DRIVER_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"

#define GL_SetPixel       GUI_GDev.SetPixel
#define GL_GetPixel       GUI_GDev.GetPixel
#define GL_DrawBitmap     GUI_GDev.DrawBitmap
#define GL_SetPixelClip   GUI_glAPI.SetPixel
#define GL_DrawHLine      GUI_glAPI.DrawHLine
#define GL_DrawVLine      GUI_glAPI.DrawVLine
#define GL_FillRect	      GUI_glAPI.FillRect

/* ͼ���豸�������� */
typedef struct {
    u_16 xSize; /* ��ʾ�豸�Ŀ�� */
    u_16 ySize; /* ��ʾ�豸�ĸ߶� */
    void (*SetPixel)(u_16 x, u_16 y, GUI_COLOR Color); /* ��ʾĳ������ */
    GUI_COLOR (*GetPixel)(u_16 x, u_16 y);             /* ��ȡĳ������ */
    void (*DrawHLine)(i_16 x0, i_16 y0, i_16 x1, GUI_COLOR Color);
    void(*DrawVLine)(i_16 x0, i_16 y0, i_16 x1, GUI_COLOR Color);
    void (*FillRect)(i_16 x0, i_16 y0, i_16 x1, i_16 y1, GUI_COLOR Color);
    void (*DrawBitmap)(u_8 cFormat, const unsigned char *pData,
        i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, u_32 Offset);
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
	void (*SetPixel)(i_16, i_16);
	void (*DrawHLine)(i_16, i_16, i_16);
	void (*DrawVLine)(i_16, i_16, i_16);
	void (*FillRect)(i_16, i_16, i_16, i_16);
} GUI_GLAPI;

extern GUI_DRIVER GUI_GDev;
extern GUI_GLAPI   GUI_glAPI;

void GUI_DeviceInit(void);

#endif /* __GUI_DRIVER_H */
