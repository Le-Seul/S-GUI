#ifndef __GUI_BOTTON_H
#define __GUI_BOTTON_H

#include "GUI_Typedef.h"

typedef struct {
    u_16 xSize; /* ��ʾ�豸�Ŀ�� */
    u_16 ySize; /* ��ʾ�豸�ĸ߶� */
    void (*SetPixel)(u_16 x, u_16 y, GUI_COLOR Color); /* ��ʾĳ������ */
    GUI_COLOR (*GetPixel)(u_16 x, u_16 y);            /* ��ȡĳ������ */
    /* ��ʾһ��������Ļ,pBuffer��һ����Ϊrowlen����Ļ������ */
    void (*DispArea)(u_16 x,
        u_16 y,
        u_16 xSize,
        u_16 ySize,
        GUI_COLOR *pBuffer,
        u_16 rowlen);
} GUI_PHY_INFO;

void GUI_InitOS(void);
void GUI_Lock(void);
void GUI_Unlock(void);
GUI_TIME GUI_GetTime(void);
void _GUI_Delay_ms(GUI_TIME tms);

void *GUI_malloc(u_32 size);
void GUI_free(void *ptr);
void *GUI_fastmalloc(u_32 size);
void GUI_fastfree(void *ptr);

void GUI_Phy_Init(GUI_PHY_INFO *phys);

#endif /* __GUI_BOTTON_H */
