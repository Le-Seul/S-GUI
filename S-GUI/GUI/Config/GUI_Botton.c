/*****************************************************************************
 * ���ļ���S-GUI��ֲʱ��Ҫ���õĵײ�������
 * ����ļ���Windows����ϵͳ��ʹ�á�
 *****************************************************************************/

#include "GUI_Botton.h"

/* �û��Լ���ͷ�ļ� */
#include "..\..\simulate_lcd.h"

/* ����Windows�Ļ����ź��� */
HANDLE GUI_hMutex;

/* GUI�������ʼ�� */
void GUI_InitOS(void)
{
    GUI_hMutex = CreateMutex(NULL, FALSE, NULL); /* ���������� */
}

/* GUI���� */
void GUI_Lock(void)
{
    WaitForSingleObject(GUI_hMutex, INFINITE); /* �ȴ������� */
}

/* GUI���� */
void GUI_Unlock(void)
{
    ReleaseMutex(GUI_hMutex); /* �ͷŻ����� */
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
    return malloc(size);
}

void GUI_free(void *ptr)
{
    free(ptr);
}

void *GUI_fastmalloc(u_32 size)
{
    return malloc(size);
}

void GUI_fastfree(void *ptr)
{
    free(ptr);
}

/* GUIͼ��Ӳ�������ӿ� */
static GUI_COLOR Phy_GetPixel(u_16 x, u_16 y);
static void Phy_SetPixel(u_16 x, u_16 y, GUI_COLOR Color);
static void Phy_DispArea(u_16 x, u_16 y, u_16 xSize, u_16 ySize,
    GUI_COLOR *pBuffer, u_16 rowlen);

/* ͼ��Ӳ����ʼ�� */
void GUI_Phy_Init(GUI_PHY_INFO *phys)
{
    phys->xSize = sim_getWidth();
    phys->ySize = sim_getHeight();
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
