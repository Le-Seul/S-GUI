/*****************************************************************************
 * ���ļ���S-GUI��ֲʱ��Ҫ���õĵײ�������
 * ����ļ���Windows����ϵͳ��ʹ�á�
 *****************************************************************************/

#include "GUI_Botton.h"

/* �û��Լ���ӵ�ͷ�ļ� */
#include <stdio.h>
#include "../../App/sdlscreen.h"
#include "SDL/SDL.h"

/* GUI���������� */
void * GUI_TaskCreateLock(void)
{
    return (void *)SDL_CreateMutex();
}

/* GUI���� */
void GUI_TaskLock(void *pLock)
{
    SDL_LockMutex((SDL_mutex *)pLock);
}

/* GUI���� */
void GUI_TaskUnlock(void *pLock)
{
    SDL_UnlockMutex((SDL_mutex *)pLock);
}

/* GUI��ȡ����ID */
u_32 GUI_TaskGetId(void)
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
    static u_32 heap1[1024 * 5 / 4];

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

/* ���Ʋ�ɫ��λͼ */
static void _DrawLogBitmap(GUI_FLIPOUT *Cmd)
{
    int i, j;
    i_16 x0 = Cmd->x0, y0 = Cmd->y0;
    const u_8 *pSrc = Cmd->pSrc;
    const GUI_COLOR *pLog = Cmd->pLog->pPalEntries;

    for (j = 0; j < Cmd->ySize; ++j) {
        for (i = 0; i < Cmd->xSize; ++i) {
            Cmd->Color = pLog[*pSrc];
            Cmd->x0 = x0 + i;
            Cmd->y0 = y0 + j;
            Phy_SetPixel(Cmd);
            pSrc += 1;
        }
        pSrc += Cmd->Offset;
    }
}

void Phy_DrawBitmap(GUI_FLIPOUT *Cmd)
{
    if (Cmd->SrcFormat == GUI_RGB888) {
        HAL_DrawBitmap(HAL_RGB888, Cmd->pSrc,
            Cmd->x0, Cmd->y0, Cmd->xSize, Cmd->ySize, Cmd->Offset);
    } else if (Cmd->SrcFormat == GUI_LOG) {
        _DrawLogBitmap(Cmd);
    }
}

/* GUI������� */
#if GUI_DEBUG_MODE
void _GUI_DebugOut(const char *s)
{
    puts(s);
    while (1);
}
#endif
