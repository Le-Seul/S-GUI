#include "GRAPH.h"
#include "GUI.h"

#define DFT_BACKCOLOR        0x00000000
#define DFT_EDGECOLOR        0x00ffffff
#define DFT_FONTCOLOR        0x00ffffff
#define DFT_LINECOLOR        0x00ff0000
#define DFT_FONT             Font_ASCII_8X16
#define DFT_X_DISTANCE       25
#define DFT_Y_DISTANCE       50

static void __Draw_Point(GRAPH_Obj *pObj, GUI_RECT *pr);

static void __Paint(GRAPH_Obj *pObj)
{
    i_16 x0, y0, x1, y1, i;
    u_16 xSize, ySize;
    GUI_COLOR Color;
    GUI_RECT Rect = WM_GetWindowRect(pObj);
    
    x0 = Rect.x0;
    y0 = Rect.y0;
    x1 = Rect.x1;
    y1 = Rect.y1;
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    /* ���Ʊ��� */
    Color = pObj->Widget.Skin.BackColor[0];
    GUI_FillTailorRect(x0, y0, xSize, ySize, Color);
    Rect = WM_GetWindowUserRect(pObj);
    x0 = Rect.x0;
    y0 = Rect.y0;
    x1 = Rect.x1;
    y1 = Rect.y1;
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    /* �������� */
    Color = pObj->Widget.Skin.EdgeColor[0];
    for (i = x0; i < x1; i += pObj->xDist) { /* ��ֱ���� */
        GUI_VertTailorLine(i, y0, ySize, Color);
    }
    for (i = y0; i < y1; i += pObj->yDist) { /* ˮƽ���� */
        GUI_HoriTailorLine(x0, i, xSize, Color);
    }
    GUI_VertTailorLine(x1, y0, ySize, Color);
    GUI_HoriTailorLine(x0, y1, xSize, Color);
    __Draw_Point(pObj, &Rect); /* ���� */
}

/* GRAPH�ؼ��Զ��ص����� */
static void __Callback(WM_MESSAGE *pMsg)
{
    /* ����Ƿ�ΪGRPHA�ؼ� */
    if (WM_CheckWindowSign(pMsg->hWin, WIDGET_GRAPH)) {
        return;
    }
    switch (pMsg->MsgId) {
    case WM_PAINT :
        __Paint(pMsg->hWin);
        break;
    case WM_DELETE :
        GUI_fastfree(pMsg->hWin);
        break;
    }
}

/*
* ����GRAPH
* x0:GRAPH�ؼ�����������(����ڸ�����)
* y0:GRAPH�ؼ�����������(����ڸ�����)
* xSize:GRAPH�ؼ���ˮƽ���
* ySize:GRAPH�ؼ�����ֱ�߶�
* hParent:�����ھ��
* Id:����ID
* Flag:����״̬
**/
GUI_hWin GRAPH_Create(i_16 x0,
    i_16 y0,
    u_16 xSize,
    u_16 ySize,
    WM_hWin hParent,
    u_16 Id,
    u_8 Flag)
{
    GRAPH_Obj *pObj;

    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, Flag,
        WIDGET_GRAPH, Id, __Callback,
        sizeof(GRAPH_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        return NULL;
    }
    /* �����û��� */
    pObj->Widget.Win.UserRect.x0 = pObj->Widget.Win.Rect.x0 + 8;
    pObj->Widget.Win.UserRect.y0 = pObj->Widget.Win.Rect.y0 + 8;
    pObj->Widget.Win.UserRect.x1 = pObj->Widget.Win.Rect.x1 - 8;
    pObj->Widget.Win.UserRect.y1 = pObj->Widget.Win.Rect.y1 - 8;
    /* ������ɫ */
    pObj->Widget.Skin.BackColor[0] = DFT_BACKCOLOR;
    pObj->Widget.Skin.EdgeColor[0] = DFT_EDGECOLOR;
    pObj->Widget.Skin.FontColor[0] = DFT_FONTCOLOR;
    pObj->Widget.Skin.EdgeColor[1] = DFT_LINECOLOR;
    pObj->Widget.Skin.Font = DFT_FONT;
    pObj->xDist = DFT_X_DISTANCE;
    pObj->yDist = DFT_Y_DISTANCE;
    pObj->Len = 0;
    pObj->xData = NULL;
    pObj->yData = NULL;
    return pObj;
}

void GRAPH_SetData(GUI_hWin hWin, i_16 *x, i_16 *y, u_16 len)
{
    i_16 *px, *py;
    GRAPH_Obj *pObj = hWin;
    
    /* ����Ƿ�ΪGRPHA�ؼ� */
    if (WM_CheckWindowSign(hWin, WIDGET_GRAPH)) {
        return;
    }
    pObj->Len = len;
    pObj->xData = GUI_fastmalloc(len * sizeof(i_16));
    pObj->yData = GUI_fastmalloc(len * sizeof(i_16));
    px = pObj->xData;
    py = pObj->yData;
    while (len--) {
        *px++ = *x++;
        *py++ = *y++;
    }
}

/* ���Ƶ� */
static void __Draw_Point(GRAPH_Obj *pObj, GUI_RECT *pr)
{
    u_16 i;
    i_16 tx, ty;
    GUI_COLOR Color;
    
    Color = pObj->Widget.Skin.EdgeColor[1];
    for (i = 0; i < pObj->Len; ++i) {
        tx = pObj->xData[i] + pr->x0;
        ty = pr->y1 - pObj->yData[i];
        if (tx >= pr->x0 && tx <= pr->x1 && ty >= pr->y0 && ty <= pr->y1) {
            GUI_DrawTailorPoint(tx, ty, Color);
        }
    }
}
