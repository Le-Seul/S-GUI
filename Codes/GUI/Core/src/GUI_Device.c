#include "GUI_Device.h"
#include "GUI.h"

GUI_DRIVER  GUI_GDev;       /* ��ǰʹ�õ����� */
GUI_GDEV    GUI_Screen;     /* ��Ļ�豸 */
GUI_GDEV   *GUI_CurDevice;  /* ��ǰ��ͼ���豸 */
GUI_DRIVER *GUI_CurDriver;  /* ��ǰ��ͼ���豸�������� */
GUI_GLAPI   GUI_glAPI;      /* �����Ļ�ͼ���� */

#define HL_SetPixel GUI_GDev.SetPixel
#define HL_GetPixel GUI_GDev.GetPixel

/* Ĭ�ϻ��㺯�� */
static void _SetPixel(GUI_FLIPOUT *Cmd)
{

}

/* Ĭ�϶�ȡ���غ��� */
static GUI_COLOR _GetPixel(GUI_FLIPOUT *Cmd)
{
    return 0;
}

/* ���豸�ϻ�ˮƽ�� */
static void _DrawHLine(GUI_FLIPOUT *Cmd)
{
    while (Cmd->x0 <= Cmd->x1) {
        HL_SetPixel(Cmd);
        Cmd->x0++;
    }
}

/* ���豸�ϻ���ֱ�� */
static void _DrawVLine(GUI_FLIPOUT *Cmd)
{
    while (Cmd->y0 <= Cmd->y1) {
        HL_SetPixel(Cmd);
        Cmd->y0++;
    }
}

/* ���豸��������� */
static void _FillRect(GUI_FLIPOUT *Cmd)
{
    i_16 x0 = Cmd->x0;

    for (; Cmd->y0 <= Cmd->y1; Cmd->y0++) {
        GUI_GDev.DrawHLine(Cmd);
        Cmd->x0 = x0;
    }
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
            HL_SetPixel(Cmd);
            (u_8 *)pSrc += 1;
        }
        pSrc += Cmd->Offset;
    }
}

/* ����λͼ */
static void _DrawBitmap(GUI_FLIPOUT *Cmd)
{
    int i, j, pixBytes;
    i_16 x0 = Cmd->x0, y0 = Cmd->y0;
    const void *pSrc = Cmd->pSrc;

    if (Cmd->SrcFormat == GUI_RGB565) {
        pixBytes = 2;
    } else if (Cmd->SrcFormat == GUI_RGB888) {
        pixBytes = 3;
    } else if (Cmd->SrcFormat == GUI_LOG) {
        _DrawLogBitmap(Cmd);
        return;
    }
    Cmd->Offset *= pixBytes;
    for (j = 0; j < Cmd->ySize; ++j) {
        for (i = 0; i < Cmd->xSize; ++i) {
            Cmd->Color = GUI_RGB565To888(*(u_16 *)pSrc);
            Cmd->x0 = x0 + i;
            Cmd->y0 = y0 + j;
            HL_SetPixel(Cmd);
            (u_8 *)pSrc += pixBytes;
        }
        (u_8 *)pSrc += Cmd->Offset;
    }
}

/* �������� */
void _GL_SetPixel(i_16 x, i_16 y, GUI_COLOR Color)
{
    GUI_FLIPOUT Cmd;

    Cmd.x0 = x;
    Cmd.y0 = y;
    Cmd.Color = Color;
    HL_SetPixel(&Cmd);
}

/* ��ȡ���� */
GUI_COLOR _GL_GetPixel(i_16 x, i_16 y)
{
    GUI_FLIPOUT Cmd;

    Cmd.x0 = x;
    Cmd.y0 = y;
    return HL_GetPixel(&Cmd);
}

/* �ü��������� */
static void _glSetPixelClip(i_16 x, i_16 y)
{
    GUI_FLIPOUT Cmd;

    CHECK_X(x);
    CHECK_Y(y);
    Cmd.x0 = x;
    Cmd.y0 = y;
    Cmd.Color = GUI_Context.FGColor;
    HL_SetPixel(&Cmd);
}

/* ����ˮƽ�� */
static void _glDrawHLine(i_16 x0, i_16 y0, i_16 x1)
{
    GUI_FLIPOUT Cmd;

    CLIP_X(x0, x1);
    CHECK_Y(y0);
    Cmd.x0 = x0;
    Cmd.y0 = y0;
    Cmd.x1 = x1;
    Cmd.Color = GUI_Context.FGColor;
    if (x0 <= x1) {
        GUI_GDev.DrawHLine(&Cmd);
    }
}

/* ���ƴ�ֱ�� */
void _GL_DrawVLine(i_16 x0, i_16 y0, i_16 y1)
{
    GUI_FLIPOUT Cmd;

    CLIP_Y(y0, y1);
    CHECK_X(x0);
    Cmd.x0 = x0;
    Cmd.y0 = y0;
    Cmd.y1 = y1;
    Cmd.Color = GUI_Context.FGColor;
    if (y0 <= y1) {
        GUI_GDev.DrawVLine(&Cmd);
    }
}

/* ������ */
static void _glFillRect(i_16 x0, i_16 y0, i_16 x1, i_16 y1)
{
    GUI_FLIPOUT Cmd;

    CLIP_X(x0, x1);
    CLIP_Y(y0, y1);
    Cmd.x0 = x0;
    Cmd.y0 = y0;
    Cmd.x1 = x1;
    Cmd.y1 = y1;
    Cmd.Color = GUI_Context.FGColor;
    GUI_GDev.FillRect(&Cmd);
}

/* ����λͼ */
void _GL_DrawBitmap(u_8 PixelFormat,
    const unsigned char *pPixel,
    i_16 x0,
    i_16 y0,
    u_16 xSize,
    u_16 ySize,
    int Offset,
    const LCD_LOGPALETTE *pLog)
{
    GUI_FLIPOUT Cmd;

    Cmd.SrcFormat = PixelFormat;
    Cmd.pSrc = pPixel;
    Cmd.x0 = x0;
    Cmd.y0 = y0;
    Cmd.xSize = xSize;
    Cmd.ySize = ySize;
    Cmd.Offset = Offset;
    Cmd.pLog = pLog;
    GUI_GDev.DrawBitmap(&Cmd);
}

/* LCD��ʼ�� */
void GUI_DeviceInit(void)
{
    GUI_glAPI.SetPixelClip = _glSetPixelClip;
    GUI_glAPI.DrawHLine = _glDrawHLine;
    GUI_glAPI.FillRect = _glFillRect;
    GUI_GDev.xSize = 0;
    GUI_GDev.ySize = 0;
    GUI_GDev.SetPixel = _SetPixel;
    GUI_GDev.GetPixel = _GetPixel;
    GUI_GDev.DrawHLine = _DrawHLine;
    GUI_GDev.DrawVLine = _DrawVLine;
    GUI_GDev.FillRect = _FillRect;
    GUI_GDev.DrawBitmap = _DrawBitmap;
    GUI_UserConfig(&GUI_GDev); /* ִ���û��ĳ�ʼ������ */
}

/* �ڴ�ӳ����ͼ���豸����ʾƫ��λ������ */
void _SetPosition(u_16 x, u_16 y)
{
    GUI_CurDevice->Position = (void *)((int)GUI_CurDevice->FrameBuffer
        + ((int)y * (int)GUI_CurDevice->Width + (int)x)
        * GUI_CurDevice->BytesPerPixel);
}

/* �ڵ�ǰλ����д������ */
void _WriteCurPosition(GUI_COLOR ColorIndex)
{
    switch (GUI_CurDevice->BytesPerPixel) {
    case 1: /* 8bpp */
        *(u_8 *)GUI_CurDevice->Position = (u_8)ColorIndex;
        break;
    case 2: /* 16bpp */
        *(u_16 *)GUI_CurDevice->Position = (u_16)ColorIndex;
        break;
    case 3: /* 24bpp */
        *(u_8 *)GUI_CurDevice->Position
            = (u_8)((ColorIndex >> 16) & 0xFF);
        *(u_8 *)((u_32)GUI_CurDevice->Position + 1)
            = (u_8)((ColorIndex >> 8) & 0xFF);
        *(u_8 *)((u_32)GUI_CurDevice->Position + 2)
            = (u_8)(ColorIndex & 0xFF);
        break;
    case 4: /* 32bpp */
        *(u_32 *)GUI_CurDevice->Position = (u_32)ColorIndex;
        break;
    }
    GUI_CurDevice->Position = (void *)((u_32)GUI_CurDevice->Position
        + GUI_CurDevice->BytesPerPixel);
}

/* ��ʾ����ƫ��ƫ�� */
void _DispOffset(int OffsetPixels)
{
    GUI_CurDevice->Position = (void *)((u_32)GUI_CurDevice->Position
        + OffsetPixels * GUI_CurDevice->BytesPerPixel);
}

/* ����֡�����ַ */
void GUI_SetFrameBuffer(GUI_GDEV *gDevice, void *Ptr)
{
    gDevice->FrameBuffer = Ptr;
}

/* ����������ɫ��ʽ */
GUI_RESULT GUI_SetPixelFormat(GUI_GDEV *gDevice, int PixelFormat)
{
    if (PixelFormat == GUI_ARGB8888) {
        gDevice->BytesPerPixel = 4;
    } else if (PixelFormat == GUI_RGB888) {
        gDevice->BytesPerPixel = 3;
    } else if (PixelFormat == GUI_RGB565) {
        gDevice->BytesPerPixel = 2;
    } else if (PixelFormat == GUI_L8) {
        gDevice->BytesPerPixel = 1;
    } else {
        return GUI_ERR;
    }
    gDevice->PixelFormat = PixelFormat;
    return GUI_OK;
}
