/*****************************************************************************
 * ���ļ���S-GUI��ֲʱ��Ҫ���õĵײ�������
 * ����ļ���Windows����ϵͳ��ʹ�á�
 *****************************************************************************/

#include "GUI_Botton.h"

/* �û��Լ���ӵ�ͷ�ļ� */
#include <stdio.h>
#include "..\..\sdlscreen.h"
#include "SDL.h"

/* ����Windows�Ļ����ź��� */
SDL_mutex *GUI_Mutex;

/* GUI�������ʼ�� */
void GUI_InitOS(void)
{
    GUI_Mutex = SDL_CreateMutex();
}

/* GUI���� */
void GUI_TaskLock(void)
{
    SDL_LockMutex(GUI_Mutex);
}

/* GUI���� */
void GUI_TaskUnlock(void)
{
    SDL_UnlockMutex(GUI_Mutex);
}

/* GUI��ȡ����ID */
u_32 GUI_GetTaskId(void)
{
    return SDL_GetThreadID(NULL);
}

/* ���GUIʱ��,��λms */
GUI_TIME GUI_GetTime(void)
{
    return SDL_GetTicks();
}

/* ��ʱ,��λms */
void _GUI_Delay_ms(GUI_TIME tms)
{
    SDL_Delay(tms);
}

/* �ڴ�ѿռ���� */
void * _GUI_GetHeapBuffer(int Page, u_32 *Size)
{
    static char heap0[1024 * 256];
    static char heap1[1024 * 1024];

    if (Page == 0) {
        *Size = sizeof(heap0);
        return heap0;
    } else if (Page == 1) {
        *Size = sizeof(heap1);
        return heap1;
    }
    return NULL;
}

/* GUIͼ��Ӳ�������ӿ� */
static GUI_COLOR Phy_GetPixel(u_16 x, u_16 y);
static void Phy_SetPixel(u_16 x, u_16 y, GUI_COLOR Color);

/* ͼ��Ӳ����ʼ�� */
void GUI_Phy_Init(GUI_PHY_INFO *phys)
{
    phys->xSize = HAL_SCREEN_W;
    phys->ySize = HAL_SCREEN_H;
    phys->GetPixel = Phy_GetPixel;
    phys->SetPixel = Phy_SetPixel;
}

/* ��ȡ��Ļ�ϵĵ� */
static GUI_COLOR Phy_GetPixel(u_16 x, u_16 y)
{
    return HAL_GetPixel(x, y);
}

/* д����Ļ�ϵ�һ���� */
static void Phy_SetPixel(u_16 x, u_16 y, GUI_COLOR Color)
{
    HAL_SetPixel(x, y, Color);
}

/* ������ */
void LCD_FillRect(i_16 x0, i_16 y0, i_16 x1, i_16 y1, GUI_COLOR Color)
{
    HAL_FillRect(x0, y0, x1, y1, Color);
}


void LCD_DrawBitmap(u_32 ColorFormat, const unsigned char *pPixel,
    i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, u_16 Offset)
{
    if (ColorFormat == GUI_RGB888) {
        HAL_DrawBitmap(HAL_RGB888, pPixel, x0, y0, xSize, ySize, Offset);
    }
}

/* GUI������� */
#if GUI_DEBUG_MODE
void _GUI_DebugOut(const char *s)
{
    printf(s);
    while (1);
}
#endif
