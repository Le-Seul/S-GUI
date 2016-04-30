#ifndef __GUI_BOTTON_H
#define __GUI_BOTTON_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"

typedef struct {
    u_16 xSize; /* ��ʾ�豸�Ŀ�� */
    u_16 ySize; /* ��ʾ�豸�ĸ߶� */
    void (*SetPixel)(u_16 x, u_16 y, GUI_COLOR Color); /* ��ʾĳ������ */
    GUI_COLOR (*GetPixel)(u_16 x, u_16 y);             /* ��ȡĳ������ */
} GUI_PHY_INFO;

void GUI_InitOS(void);
void GUI_TaskLock(void);
void GUI_TaskUnlock(void);
u_32 GUI_GetTaskId(void);
GUI_TIME GUI_GetTime(void);
void _GUI_Delay_ms(GUI_TIME tms);
void * _GUI_GetHeapBuffer(int Page, u_32 *Size);
void GUI_Phy_Init(GUI_PHY_INFO *phys);
void _GUI_DebugOut(const char *s);

#if GUI_USE_GRAPHPHY
void LCD_FillRect(i_16 x0, i_16 y0, i_16 x1, i_16 y1, GUI_COLOR Color);
void LCD_DrawBitmap(u_32 ColorFormat, const unsigned char *pPixel,
    i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, u_16 Offset);
#endif

#endif /* __GUI_BOTTON_H */
