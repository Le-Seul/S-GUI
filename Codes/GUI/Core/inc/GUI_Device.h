#ifndef __GUI_DEVICE_H
#define __GUI_DEVICE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"

#define GL_SetPixel  GUI_LCD.SetPixel
#define GL_GetPixel  GUI_LCD.GetPixel
#define GL_DrawHLine GUI_LCD.DrawHLine
#define GL_FillRect  GUI_LCD.FillRect

typedef struct {
    u_16 xSize; /* ��ʾ�豸�Ŀ�� */
    u_16 ySize; /* ��ʾ�豸�ĸ߶� */
    void (*SetPixel)(u_16 x, u_16 y, GUI_COLOR Color); /* ��ʾĳ������ */
    GUI_COLOR (*GetPixel)(u_16 x, u_16 y);             /* ��ȡĳ������ */
    void (*DrawHLine)(i_16 x0, i_16 y0, u_16 len, GUI_COLOR Color);
    void (*FillRect)(i_16 x0, i_16 y0, i_16 x1, i_16 y1, GUI_COLOR Color);
} GUI_DEVICE;

extern GUI_DEVICE GUI_LCD;

void GUI_DeviceInit(void);

#endif /* __GUI_DEVICE_H */
