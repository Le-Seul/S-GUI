#include "GUI_Device.h"
#include "GUI.h"

GUI_GDEV        *gui_graph_dev;  /* ��ǰʹ�õ�ͼ���豸 */
static GUI_GDEV *device_list;    /* ͼ���豸�б� */

/* Ĭ�ϻ��㺯�� */
static void _setPixel(int x, int y, GUI_COLOR Color)
{

}

/* Ĭ�϶�ȡ���غ��� */
static GUI_COLOR _getPixel(int x, int y)
{
    return 0;
}

/* ���豸�ϻ�ˮƽ�� */
static void _drawHLine(int x0, int y0, int x1, GUI_COLOR Color)
{
    while (x0 <= x1) {
        HL_SetPixel(x0, y0, Color);
        ++x0;
    }
}

/* ���豸�ϻ���ֱ�� */
static void _drawVLine(int x0, int y0, int y1, GUI_COLOR Color)
{
    while (y0 <= y1) {
        HL_SetPixel(x0, y0, Color);
        ++y0;
    }
}

/* ���豸��������� */
static void _fillRect(GUI_FLIPOUT *Cmd)
{
    int x0 = Cmd->x0, y0 = Cmd->y0, x1 = Cmd->x1, y1 = Cmd->y1;
    GUI_COLOR Color = Cmd->Color;

    while (y0 <= y1) {
        gui_graph_dev->drawHLine(x0, y0, x1, Color);
    }
}

/* ���Ʋ�ɫ��λͼ */
static void _drawLogBitmap(GUI_FLIPOUT *Cmd)
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
static void _drawBitmap(GUI_FLIPOUT *Cmd)
{
    int i, j, pixBytes = 1;
    int x0 = Cmd->x0, y0 = Cmd->y0;
    const u_8 *pSrc = (const u_8 *)Cmd->pSrc;

    if (Cmd->SrcFormat == GUI_RGB565) {
        pixBytes = 2;
    } else if (Cmd->SrcFormat == GUI_RGB888) {
        pixBytes = 3;
    } else if (Cmd->SrcFormat == GUI_LOG) {
        _drawLogBitmap(Cmd);
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

/* ����λͼ */
void GL_DrawBitmap(u_8 PixelFormat,
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
    gui_graph_dev->drawBitmap(&Cmd);
}

/* LCD��ʼ�� */
void GUI_DeviceInit(void)
{
    device_list = NULL;
    gui_graph_dev = GUI_GetDevice(GUI_DEF_SCREEN);
    gui_graph_dev->width = 0;
    gui_graph_dev->height = 0;
    gui_graph_dev->setPixel = _setPixel;
    gui_graph_dev->getPixel = _getPixel;
    gui_graph_dev->drawHLine = _drawHLine;
    gui_graph_dev->drawVLine = _drawVLine;
    gui_graph_dev->fillRect = _fillRect;
    gui_graph_dev->drawBitmap = _drawBitmap;
    GUI_UserConfig(gui_graph_dev); /* ִ���û��ĳ�ʼ������ */
}

/* ѡ��ĳ��ͼ���豸 */
GUI_RESULT GUI_SelectDevice(unsigned char id)
{
    GUI_GDEV *pNode;

    GUI_LOCK();
    for (pNode = device_list; pNode && pNode->id != id; pNode = pNode->pNext);
    if (pNode) {
        gui_graph_dev = pNode;
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
    pNode = device_list;
    if (pNode == NULL) { /* ����Ϊ���򴴽� */
        pNode = GUI_Malloc(sizeof(GUI_GDEV));
        pNode->id = id;
        pNode->pNext = NULL;
    } else {
        while (pNode->pNext && pNode->id != id) { /* Ѱ��IDΪid���豸 */
            pNode = pNode->pNext;
        }
        if (pNode->id != id) { /* �������豸 */
            pNode->pNext = GUI_Malloc(sizeof(GUI_GDEV));
            pNode = pNode->pNext;
            pNode->id = id;
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
    pNode = device_list;
    if (pNode->id == id) {
        device_list = pNode->pNext;
        GUI_Free(pNode);
    } else if (pNode) { /* ����Ϊ�� */
        while (pNode->pNext && pNode->pNext->id != id) { /* ��Ҫɾ����ǰһ������ */
            pNode = pNode->pNext;
        }
        if (pNode->pNext && pNode->pNext->id == id) {
            if (gui_graph_dev->id == id) { /* ʹ��Ĭ���豸 */
                gui_graph_dev = GUI_GetDevice(GUI_DEF_SCREEN);
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
    pNode = device_list;
    while (pNode) {
        pNext = pNode->pNext;
        GUI_Free(pNode);
        pNode = pNext;
    }
    device_list = NULL;
    GUI_UNLOCK();
}
