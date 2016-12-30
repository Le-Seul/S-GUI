#include "GUI_Device.h"
#include "GUI.h"

GUI_GDEV        *GUI_GDev;       /* ��ǰʹ�õ�ͼ���豸 */
GUI_GDEV         GUI_Screen;     /* ��Ļ�豸 */
GUI_GLAPI        GUI_glAPI;      /* �����Ļ�ͼ���� */
static GUI_GDEV *GUI_GDevList;   /* ͼ���豸�б� */

#define HL_SetPixel GUI_GDev->SetPixel
#define HL_GetPixel GUI_GDev->GetPixel

/* Ĭ�ϻ��㺯�� */
static void _SetPixel(int x, int y, GUI_COLOR Color)
{

}

/* Ĭ�϶�ȡ���غ��� */
static GUI_COLOR _GetPixel(int x, int y)
{
    return 0;
}

/* ���豸�ϻ�ˮƽ�� */
static void _DrawHLine(int x0, int y0, int x1, GUI_COLOR Color)
{
    while (x0 <= x1) {
        HL_SetPixel(x0, y0, Color);
        ++x0;
    }
}

/* ���豸�ϻ���ֱ�� */
static void _DrawVLine(int x0, int y0, int y1, GUI_COLOR Color)
{
    while (y0 <= y1) {
        HL_SetPixel(x0, y0, Color);
        ++y0;
    }
}

/* ���豸��������� */
static void _FillRect(GUI_FLIPOUT *Cmd)
{
    int x0 = Cmd->x0, y0 = Cmd->y0, x1 = Cmd->x1, y1 = Cmd->y1;
    GUI_COLOR Color = Cmd->Color;

    while (y0 <= y1) {
        GUI_GDev->DrawHLine(x0, y0, x1, Color);
    }
}

/* ���Ʋ�ɫ��λͼ */
static void _DrawLogBitmap(GUI_FLIPOUT *Cmd)
{
    int i, j;
    int x0 = Cmd->x0, y0 = Cmd->y0;
    const u_8 *pSrc = Cmd->pSrc;
    const GUI_COLOR *pLog = Cmd->pLog->pPalEntries;

    for (j = 0; j < (int)Cmd->ySize; ++j) {
        for (i = 0; i < (int)Cmd->xSize; ++i) {
            HL_SetPixel(x0 + i, y0 + j, pLog[*pSrc]);
            pSrc += 1;
        }
        pSrc += Cmd->Offset;
    }
}

/* ����λͼ */
static void _DrawBitmap(GUI_FLIPOUT *Cmd)
{
    int i, j, pixBytes = 1;
    int x0 = Cmd->x0, y0 = Cmd->y0;
    const u_8 *pSrc = (const u_8 *)Cmd->pSrc;

    if (Cmd->SrcFormat == GUI_RGB565) {
        pixBytes = 2;
    } else if (Cmd->SrcFormat == GUI_RGB888) {
        pixBytes = 3;
    } else if (Cmd->SrcFormat == GUI_LOG) {
        _DrawLogBitmap(Cmd);
        return;
    }
    Cmd->Offset *= pixBytes;
    for (j = 0; j < (int)Cmd->ySize; ++j) {
        for (i = 0; i < (int)Cmd->xSize; ++i) {
            HL_SetPixel(x0 + i, y0 +j, GUI_RGB565To888(*(u_16 *)pSrc));
            pSrc += pixBytes;
        }
        pSrc += Cmd->Offset;
    }
}

/* �������� */
void _GL_SetPixel(int x, int y, GUI_COLOR Color)
{
    HL_SetPixel(x, y, Color);
}

/* ��ȡ���� */
GUI_COLOR _GL_GetPixel(int x, int y)
{
    return HL_GetPixel(x, y);
}

/* �ü��������� */
static void _glSetPixelClip(int x, int y)
{
    CHECK_X(x);
    CHECK_Y(y);
    HL_SetPixel(x, y, GUI_Context.FGColor);
}

/* ����ˮƽ�� */
static void _glDrawHLine(int x0, int y0, int x1)
{
    CLIP_X(x0, x1);
    CHECK_Y(y0);
    if (x0 <= x1) {
        GUI_GDev->DrawHLine(x0, y0, x1, GUI_Context.FGColor);
    }
}

/* ���ƴ�ֱ�� */
void _GL_DrawVLine(int x0, int y0, int y1)
{
    CLIP_Y(y0, y1);
    CHECK_X(x0);
    if (y0 <= y1) {
        GUI_GDev->DrawVLine(x0, y0, y1, GUI_Context.FGColor);
    }
}

/* ������ */
static void _glFillRect(int x0, int y0, int x1, int y1)
{
    GUI_FLIPOUT Cmd;

    CLIP_X(x0, x1);
    CLIP_Y(y0, y1);
    Cmd.x0 = x0;
    Cmd.y0 = y0;
    Cmd.x1 = x1;
    Cmd.y1 = y1;
    Cmd.Color = GUI_Context.FGColor;
    GUI_GDev->FillRect(&Cmd);
}

/* ����λͼ */
void _GL_DrawBitmap(u_8 PixelFormat,
    const unsigned char *pPixel,
    int x0,
    int y0,
    int xSize,
    int ySize,
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
    GUI_GDev->DrawBitmap(&Cmd);
}

/* LCD��ʼ�� */
void GUI_DeviceInit(void)
{
    GUI_glAPI.SetPixelClip = _glSetPixelClip;
    GUI_glAPI.DrawHLine = _glDrawHLine;
    GUI_glAPI.FillRect = _glFillRect;
    GUI_GDevList = NULL;
    GUI_GDev = GUI_GetDevice(GUI_DEF_SCREEN);
    GUI_GDev->Width = 0;
    GUI_GDev->Height = 0;
    GUI_GDev->SetPixel = _SetPixel;
    GUI_GDev->GetPixel = _GetPixel;
    GUI_GDev->DrawHLine = _DrawHLine;
    GUI_GDev->DrawVLine = _DrawVLine;
    GUI_GDev->FillRect = _FillRect;
    GUI_GDev->DrawBitmap = _DrawBitmap;
    GUI_UserConfig(GUI_GDev); /* ִ���û��ĳ�ʼ������ */
}

/* ѡ��ĳ��ͼ���豸 */
GUI_RESULT GUI_SelectDevice(unsigned char id)
{
    GUI_GDEV *pNode;

    GUI_LOCK();
    for (pNode = GUI_GDevList; pNode && pNode->Id != id; pNode = pNode->pNext);
    if (pNode) {
        GUI_GDev = pNode;
    }
    GUI_UNLOCK();
    if (pNode) {
        return GUI_OK;
    }
    return GUI_ERR;
}

/* ��ȡһ��ͼ���豸��ָ��,����豸�����ھʹ���һ�� */
GUI_GDEV * GUI_GetDevice(unsigned char id)
{
    GUI_GDEV *pNode;

    GUI_LOCK();
    pNode = GUI_GDevList;
    if (pNode == NULL) { /* ����Ϊ���򴴽� */
        pNode = GUI_Malloc(sizeof(GUI_GDEV));
        pNode->Id = id;
        pNode->pNext = NULL;
    } else {
        while (pNode->pNext && pNode->Id != id) { /* Ѱ��IDΪid���豸 */
            pNode = pNode->pNext;
        }
        if (pNode->Id != id) { /* �������豸 */
            pNode->pNext = GUI_Malloc(sizeof(GUI_GDEV));
            pNode = pNode->pNext;
            pNode->Id = id;
            pNode->pNext = NULL;
        }
    }
    GUI_UNLOCK();
    return pNode;
}

/* ɾ��ͼ���豸 */
void GUI_DeleteDevice(unsigned char id)
{
    GUI_GDEV *pNode;

    if (id == GUI_DEF_SCREEN) { /* Ĭ���豸����ɾ�� */
        return;
    }
    GUI_LOCK();
    pNode = GUI_GDevList;
    if (pNode->Id == id) {
        GUI_GDevList = pNode->pNext;
        GUI_Free(pNode);
    } else if (pNode) { /* ����Ϊ�� */
        while (pNode->pNext && pNode->pNext->Id != id) { /* ��Ҫɾ����ǰһ������ */
            pNode = pNode->pNext;
        }
        if (pNode->pNext && pNode->pNext->Id == id) {
            if (GUI_GDev->Id == id) { /* ʹ��Ĭ���豸 */
                GUI_GDev = GUI_GetDevice(GUI_DEF_SCREEN);
            }
            pNode->pNext = pNode->pNext->pNext;
            GUI_Free(pNode->pNext);
        }
    }
    GUI_UNLOCK();
}

/* ɾ��ͼ���豸��, �˺�������֮������ʹ��S-GUI */
void GUI_DeleteDeviceList(void)
{
    GUI_GDEV *pNode, *pNext;

    GUI_LOCK();
    pNode = GUI_GDevList;
    while (pNode) {
        pNext = pNode->pNext;
        GUI_Free(pNode);
        pNode = pNext;
    }
    GUI_GDevList = NULL;
    GUI_UNLOCK();
}
