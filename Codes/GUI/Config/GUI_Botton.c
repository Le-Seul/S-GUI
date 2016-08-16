/*****************************************************************************
 * ���ļ���S-GUI��ֲʱ��Ҫ���õĵײ�������
 * ����ļ���Windows����ϵͳ��ʹ�á�
 *****************************************************************************/

#include "GUI_Botton.h"

/* �û��Լ���ӵ�ͷ�ļ� */
#include <stdio.h>
#include "../../App/sdlscreen.h"
#include "SDL/SDL.h"

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
    static u_32 heap0[1024 * 1 / 4];
    static u_32 heap1[1024 * 16 / 4];

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
static GUI_COLOR Phy_GetPixel(GUI_FLIPOUT *Cmd);
static void Phy_SetPixel(GUI_FLIPOUT *Cmd);
void Phy_FillRect(GUI_FLIPOUT *Cmd);
void Phy_DrawBitmap(GUI_FLIPOUT *Cmd);

/* ͼ��Ӳ����ʼ�� */
void GUI_UserConfig(GUI_DRIVER *phy)
{
    phy->xSize = HAL_SCREEN_W;
    phy->ySize = HAL_SCREEN_H;
    phy->GetPixel = Phy_GetPixel;
    phy->SetPixel = Phy_SetPixel;
    phy->FillRect = Phy_FillRect;
    phy->DrawBitmap = Phy_DrawBitmap;
}

/* ��ȡ��Ļ�ϵĵ� */
static GUI_COLOR Phy_GetPixel(GUI_FLIPOUT *Cmd)
{
    return HAL_GetPixel(Cmd->x0, Cmd->y0);
}

/* д����Ļ�ϵ�һ���� */
static void Phy_SetPixel(GUI_FLIPOUT *Cmd)
{
    HAL_SetPixel(Cmd->x0, Cmd->y0, Cmd->Color);
}

void Phy_FillRect(GUI_FLIPOUT *Cmd)
{
    HAL_FillRect(Cmd->x0, Cmd->y0, Cmd->x1, Cmd->y1, Cmd->Color);
}

void Phy_DrawBitmap(GUI_FLIPOUT *Cmd)
{
    if (Cmd->SrcFormat == GUI_RGB888) {
        HAL_DrawBitmap(HAL_RGB888, Cmd->pSrc,
            Cmd->x0, Cmd->y0, Cmd->xSize, Cmd->ySize, Cmd->Offset);
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
