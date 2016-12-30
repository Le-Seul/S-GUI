#ifndef __GUI_DEVICE_H
#define __GUI_DEVICE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"
#include "GUI_Bitmap.h"

#define GUI_DEF_SCREEN   0
#define GUI_MEN_SEREEN   0xFF

#define GL_SetPixel       _GL_SetPixel
#define GL_GetPixel       _GL_GetPixel
#define GL_DrawBitmap     _GL_DrawBitmap
#define GL_SetPixelClip   GUI_glAPI.SetPixelClip
#define GL_DrawHLine      GUI_glAPI.DrawHLine
#define GL_DrawVLine      _GL_DrawVLine
#define GL_FillRect	      GUI_glAPI.FillRect

/* ��ͼָ�� */
typedef struct {
    int x0, y0, x1, y1;
    int xSize, ySize;
    u_32 Offset;
    GUI_COLOR Color;
    const LCD_LOGPALETTE *pLog;
    const unsigned char *pSrc;
    unsigned char *pDst;
    u_8 SrcFormat, DstFormat;
}GUI_FLIPOUT;

/* ͼ���豸�������� */
typedef struct GUI_GDEV {
    int Width, Height; /* ��ʾ�豸�Ŀ�Ⱥ͸߶ȸ߶� */
    unsigned char Id;   /* �豸ID */
    int PixelFormat;    /* ������ɫ��ʽ */
    void (*SetPixel)(int, int, GUI_COLOR); /* ��ʾĳ������ */
    GUI_COLOR (*GetPixel)(int, int);       /* ��ȡĳ������ */
    void (*DrawHLine)(int, int, int, GUI_COLOR);
    void (*DrawVLine)(int, int, int, GUI_COLOR);
    void (*FillRect)(GUI_FLIPOUT *);
    void(*DrawBitmap)(GUI_FLIPOUT *);
    struct GUI_GDEV *pNext;
} GUI_GDEV;

/* ��ͼAPI���� */
typedef struct {
    void (*SetPixelClip)(int, int);
    void (*DrawHLine)(int, int, int);
    void (*FillRect)(int, int, int, int);
} GUI_GLAPI;

extern GUI_GDEV   *GUI_GDev;
extern GUI_GLAPI   GUI_glAPI;

void _GL_SetPixel(int x, int y, GUI_COLOR);
GUI_COLOR _GL_GetPixel(int x, int y);
void _GL_DrawVLine(int x0, int y0, int y1);
void _GL_DrawBitmap(u_8 PixelFormat, const unsigned char *pPixel,
    int x0, int y0, int xSize, int ySize, int Offset,
    const LCD_LOGPALETTE *pLog);
void GUI_DeviceInit(void);
GUI_RESULT GUI_SelectDevice(unsigned char id);
GUI_GDEV * GUI_GetDevice(unsigned char id);
void GUI_DeleteDevice(unsigned char id);
void GUI_DeleteDeviceList(void);

#endif /* __GUI_DEVICE_H */
