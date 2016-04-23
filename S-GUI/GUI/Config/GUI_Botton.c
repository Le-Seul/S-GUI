/*****************************************************************************
 * ���ļ���S-GUI��ֲʱ��Ҫ���õĵײ�������
 * ����ļ���Windows����ϵͳ��ʹ�á�
 *****************************************************************************/

#include "GUI_Botton.h"

/* �û��Լ���ӵ�ͷ�ļ� */
#include <stdio.h>
#include "..\..\simulate_lcd.h"
#include "..\..\memalloc.h"

/* ����Windows�Ļ����ź��� */
HANDLE GUI_hMutex;

/* GUI�������ʼ�� */
void GUI_InitOS(void)
{
    GUI_hMutex = CreateMutex(NULL, FALSE, NULL); /* ���������� */
}

/* GUI���� */
void GUI_TaskLock(void)
{
    WaitForSingleObject(GUI_hMutex, INFINITE); /* �ȴ������� */
}

/* GUI���� */
void GUI_TaskUnlock(void)
{
    ReleaseMutex(GUI_hMutex); /* �ͷŻ����� */
}

/* GUI��ȡ����ID */
u_32 GUI_GetTaskId(void)
{
    return GetCurrentThreadId();
}

/* ���GUIʱ��,��λms */
GUI_TIME GUI_GetTime(void)
{
    return GetTickCount(); /* ����Windows�������ms�� */
}

/* ��ʱ,��λms */
void _GUI_Delay_ms(GUI_TIME tms)
{
    Sleep(tms); /* Windows�µ���ʱ���� */
}

/* GUI��̬�ڴ�֧�� */
void *GUI_malloc(u_32 size)
{
    return memalloc(size, sim_mem2);
}

void GUI_free(void *ptr)
{
    memfree(ptr, sim_mem2);
}

void *GUI_fastmalloc(u_32 size)
{
    return memalloc(size, sim_mem1);
}

void GUI_fastfree(void *ptr)
{
    memfree(ptr, sim_mem1);
}

/* GUIͼ��Ӳ�������ӿ� */
static GUI_COLOR Phy_GetPixel(u_16 x, u_16 y);
static void Phy_SetPixel(u_16 x, u_16 y, GUI_COLOR Color);
static void Phy_DispArea(u_16 x, u_16 y, u_16 xSize, u_16 ySize,
    GUI_COLOR *pBuffer, u_16 rowlen);

/* ͼ��Ӳ����ʼ�� */
void GUI_Phy_Init(GUI_PHY_INFO *phys)
{
    phys->xSize = (u_16)sim_getWidth();
    phys->ySize = (u_16)sim_getHeight();
    phys->GetPixel = Phy_GetPixel;
    phys->SetPixel = Phy_SetPixel;
    phys->DispArea = Phy_DispArea;
    sim_lcd_init();
}

/* ��ȡ��Ļ�ϵĵ� */
static GUI_COLOR Phy_GetPixel(u_16 x, u_16 y)
{
    return sim_readPix(x, y);
}

/* д����Ļ�ϵ�һ���� */
static void Phy_SetPixel(u_16 x, u_16 y, GUI_COLOR Color)
{
    sim_drawPix(x, y, Color);
}

/* ��GUI���ڲ�������д�뵽��Ļ��д��һ������ */
static void Phy_DispArea(u_16 x,
    u_16 y,
    u_16 xSize,
    u_16 ySize,
    GUI_COLOR *pBuffer,
    u_16 rowlen)
{
    sim_dispArea(x, y, xSize, ySize, pBuffer, rowlen);
}

/* GUI������� */
#if GUI_DEBUG_MODE
void _GUI_DebugOut(const char *s)
{
    /* �����Win32���� */
    AllocConsole();
    unsigned long lgsize;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(hOutput, s, strlen(s), &lgsize, 0);
    while (1);
}
#endif
