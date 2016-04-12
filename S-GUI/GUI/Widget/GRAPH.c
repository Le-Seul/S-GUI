#include "GRAPH.h"
#include "GUI.h"

#define DFT_BACKCOLOR        0x002B2B2B
#define DFT_FONTCOLOR        0x00ffffff
#define DFT_EDGECOLOR        0x00787878
#define DEF_GRIDCOLOR        0x003F4F4F
#define DFT_FONT             Font_ASCII_8X16

static void _DrawGrid(GRAPH_Obj *pObj, GUI_RECT *pr);
static void _DrawData(GRAPH_Obj *pObj, GUI_RECT *pr);
static void _DeleteData(GRAPH_Obj *pWin);

static void __Paint(GRAPH_Obj *pObj)
{
    i_16 x0, y0, x1, y1;
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
    GUI_FillRect(x0, y0, xSize, ySize, Color);
    Rect = WM_GetWindowRect(pObj);
    x0 = Rect.x0;
    y0 = Rect.y0;
    x1 = Rect.x1;
    y1 = Rect.y1;
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    /* ���Ʊ߿� */
    Color = pObj->Widget.Skin.EdgeColor[0];
    GUI_DrawRect(x0, y0, xSize, ySize, Color);
    _DrawGrid(pObj, &Rect); /* �������� */
    _DrawData(pObj, &Rect); /* �������� */
}

/* GRAPH�ؼ��Զ��ص����� */
static void __Callback(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId) {
    case WM_PAINT:
        __Paint(pMsg->hWin);
        break;
    case WM_DELETE:
        _DeleteData(pMsg->hWin);
        break;
    default:
        WM_DefaultProc(pMsg);
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
GUI_HWIN GRAPH_Create(i_16 x0,
    i_16 y0,
    u_16 xSize,
    u_16 ySize,
    WM_HWIN hParent,
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
    /* ������ɫ */
    pObj->Widget.Skin.BackColor[0] = DFT_BACKCOLOR;
    pObj->Widget.Skin.FontColor[0] = DFT_FONTCOLOR;
    pObj->Widget.Skin.EdgeColor[0] = DFT_EDGECOLOR;
    pObj->Widget.Skin.EdgeColor[1] = DEF_GRIDCOLOR;
    pObj->Widget.Skin.Font = DFT_FONT;
    GRAPH_SetScale(pObj, 0, 100, 0, 100, 50, 50);
    pObj->List = List_Init();
    return pObj;
}

/* ���ñ����� */
GUI_RESULT GRAPH_SetScale(GUI_HWIN hWin,
    i_16 x0,
    i_16 y0,
    u_16 xScale,
    u_16 yScale,
    u_16 xDist,
    u_16 yDist)
{
    GRAPH_Obj *pObj = hWin;

    /* ����Ƿ�ΪGRAPH�ؼ� */
    WIDGET_SignErrorReturn(hWin, WIDGET_GRAPH);
    pObj->Scale.x0 = x0;
    pObj->Scale.y0 = y0;
    pObj->Scale.xScale = xScale;
    pObj->Scale.yScale = yScale;
    pObj->Scale.xDist = xDist;
    pObj->Scale.yDist = yDist;
    return GUI_OK;
}

/* �������� */
static void _DrawGrid(GRAPH_Obj *pObj, GUI_RECT *pr)
{
    i_16 i, x0 = pr->x0, y0 = pr->y0, x1 = pr->x1, y1 = pr->y1;
    i_16 xSize = x1 - x0 - 1, ySize = y1 -y0 - 1;
    u_16 xDist = pObj->Scale.xDist, yDist = pObj->Scale.yDist;
    GUI_COLOR Color = pObj->Widget.Skin.EdgeColor[1];
    
    for (i = x0 + xDist; i < x1; i += xDist) {
        GUI_VertLine(i, y0 + 1, ySize, Color);
    }
    for (i = y1 - yDist; i > y0; i -= yDist) {
        GUI_HoriLine(x0 + 1, i, xSize, Color);
    }
}

/* ���Ƶ� */
static void __DrawXY(GRAPH_Obj *pObj, GRAPH_DATA *pData, GUI_RECT *pr)
{
    u_16 i;
    int x0, y0, x1, y1;
    int xScale, yScale;
    GUI_COLOR Color = pData->Color;
    GRAPH_XYDATA *Data = pData->pData;
    GRAPH_SCALE *pScale = &pObj->Scale;

    xScale = pScale->xScale;
    yScale = pScale->yScale;
    for (i = 1; i < Data->ItemNum; ++i) {
        /* ת��Ϊ�������� */
        x0 = pr->x0 + (Data->xData[i - 1] - pScale->x0) * xScale / 100;
        y0 = pr->y1 - (Data->yData[i - 1] - pScale->y0) * yScale / 100;
        x1 = pr->x0 + (Data->xData[i] - pScale->x0) * xScale / 100;
        y1 = pr->y1 - (Data->yData[i] - pScale->y0) * yScale / 100;
        /* ���Ƶ� */
        x0 = GUI_MAX(pr->x0 + 1, x0);
        y0 = GUI_MAX(pr->y0 + 1, y0);
        x1 = GUI_MIN(pr->x1 - 1, x1);
        y1 = GUI_MIN(pr->y1 - 1, y1);
        GUI_DrawLine((i_16)x0, (i_16)y0, (i_16)x1, (i_16)y1, Color);
    }
}

/* ���Ƶ� */
static void __DrawTY(GRAPH_Obj *pObj, GRAPH_DATA *pData, GUI_RECT *pr)
{
    int i;
    int x0, y0, x1, y1;
    int xScale, yScale;
    GUI_COLOR Color = pData->Color;
    GRAPH_TYDATA *Data = pData->pData;
    GRAPH_SCALE *pScale = &pObj->Scale;

    xScale = pScale->xScale;
    yScale = pScale->yScale;
    for (i = Data->tStart + 1; i < Data->ItemNum; ++i) {
        /* ת��Ϊ�������� */
        x0 = pr->x0 + (i - 1 - pScale->x0) * xScale / 100;
        y0 = pr->y1 - (Data->yData[i - 1] - pScale->y0) * yScale / 100;
        x1 = pr->x0 + (i - pScale->x0) * xScale / 100;
        y1 = pr->y1 - (Data->yData[i] - pScale->y0) * yScale / 100;
        /* ���Ƶ� */
        x0 = GUI_MAX(pr->x0 + 1, x0);
        y0 = GUI_MAX(pr->y0 + 1, y0);
        x1 = GUI_MIN(pr->x1 - 1, x1);
        y1 = GUI_MIN(pr->y1 - 1, y1);
        GUI_DrawLine((i_16)x0, (i_16)y0, (i_16)x1, (i_16)y1, Color);
    }
}

static void _DrawData(GRAPH_Obj *pObj, GUI_RECT *pr)
{
    LIST List;

    if (pObj->List == NULL) {
        return;
    }
    for (List = pObj->List->pNext; List; List = List->pNext) {
        switch (((GRAPH_DATA *)List->pData)->Status) {
        case GRAPH_XY_DATA:
            __DrawXY(pObj, List->pData, pr);
            break;
        case GRAPH_TY_DATA:
            __DrawTY(pObj, List->pData, pr);
            break;
        }
    }
}

/* ɾ��XY��ʽ������ */
static void _DeleteXYData(GRAPH_DATA *p)
{
    GRAPH_XYDATA *pd = p->pData;

    GUI_fastfree(pd->xData);
    GUI_fastfree(pd->yData);
    GUI_fastfree(pd);
}

/* ɾ��TY��ʽ������ */
static void _DeleteTYData(GRAPH_DATA *p)
{
    GRAPH_TYDATA *pd = p->pData;

    GUI_fastfree(pd->yData);
    GUI_fastfree(pd);
}

/* ɾ������ */
static void _DeleteData(GRAPH_Obj *pWin)
{
    LIST l, i;

    if (pWin->List == NULL) {
        return;
    }
    l = pWin->List;
    while (l) {
        i = l->pNext;
        if (l->pData) {
            switch (((GRAPH_DATA *)l->pData)->Status) {
            case GRAPH_XY_DATA:
                _DeleteXYData(l->pData);
                break;
            case GRAPH_TY_DATA:
                _DeleteTYData(l->pData);
                break;
            }
            GUI_fastfree(l->pData);
        }
        l = i;
    }
}

/* ����һ��XY��ʽ��ͼ������ */
GUI_HWIN GRAPH_XY_DataCreate(int *xData,
    int *yData,
    int ItemNum,
    int MaxItemNum,
    GUI_COLOR Color,
    u_8 Style)
{
    int i, *iData, *jData;
    GRAPH_DATA *pObj;
    GRAPH_XYDATA *pData;

    pObj = GUI_fastmalloc(sizeof(GRAPH_DATA));
    pData = GUI_fastmalloc(sizeof(GRAPH_XYDATA));
    iData = GUI_fastmalloc(MaxItemNum * sizeof(int));
    jData = GUI_fastmalloc(MaxItemNum * sizeof(int));
    if (!pObj || !pData || !iData || !jData) {
        GUI_fastfree(pObj);
        GUI_fastfree(pData);
        GUI_fastfree(iData);
        GUI_fastfree(jData);
        return NULL;
    }
    pObj->Status = GRAPH_XY_DATA;
    pObj->pData = pData;
    pObj->Color = Color;
    pObj->Style = Style;
    pObj->Title = NULL;
    pData->xData = iData;
    pData->yData = jData;
    if (ItemNum > MaxItemNum) {
        ItemNum = MaxItemNum;
    }
    pData->ItemNum = ItemNum;
    pData->MaxItemNum = MaxItemNum;
    for (i = 0; i < ItemNum; ++i) {
        iData[i] = xData[i];
        jData[i] = yData[i];
    }
    return pObj;
}

/* ����һ��TY��ʽ��ͼ������ */
GUI_HWIN GRAPH_TY_DataCreate(int *yData,
    int tStart,
    int ItemNum,
    int MaxItemNum,
    GUI_COLOR Color,
    u_8 Style)
{
    int i, *iData;
    GRAPH_DATA *pObj;
    GRAPH_TYDATA *pData;

    pObj = GUI_fastmalloc(sizeof(GRAPH_DATA));
    pData = GUI_fastmalloc(sizeof(GRAPH_XYDATA));
    iData = GUI_fastmalloc(MaxItemNum * sizeof(int));
    if (!pObj || !pData || !iData) {
        GUI_fastfree(pObj);
        GUI_fastfree(pData);
        GUI_fastfree(iData);
        return NULL;
    }
    pObj->Status = GRAPH_TY_DATA;
    pObj->pData = pData;
    pObj->Color = Color;
    pObj->Style = Style;
    pObj->Title = NULL;
    pData->yData = iData;
    pData->tStart = tStart;
    if (ItemNum > MaxItemNum) {
        ItemNum = MaxItemNum;
    }
    pData->ItemNum = ItemNum;
    pData->MaxItemNum = MaxItemNum;
    for (i = 0; i < ItemNum; ++i) {
        iData[i] = yData[i];
    }
    return pObj;
}

/* ����һ��ͼ������� */
void GRAPH_XY_DataEdit(GUI_HWIN hData, int *pX, int *pY, int Num)
{
    int i;
    GRAPH_DATA *pObj = hData;
    GRAPH_XYDATA *pData;

    if (pObj == NULL || pObj->pData == NULL) {
        return;
    }
    pData = pObj->pData;
    if (Num < pData->MaxItemNum) {
        pData->ItemNum = Num;
    } else {
        pData->ItemNum = pData->MaxItemNum;
    }
    for (i = 0; i < pData->ItemNum; ++i) {
        pData->xData[i] = pX[i];
        pData->yData[i] = pY[i];
    }
}

/* ����һ��ͼ������� */
void GRAPH_TY_DataEdit(GUI_HWIN hData, int *pY, int tStart, int Num)
{
    int i;
    GRAPH_DATA *pObj = hData;
    GRAPH_TYDATA *pData;

    if (pObj == NULL || pObj->pData == NULL) {
        return;
    }
    pData = pObj->pData;
    if (Num < pData->MaxItemNum) {
        pData->ItemNum = Num;
    } else {
        pData->ItemNum = pData->MaxItemNum;
    }
    pData->tStart = tStart;
    for (i = 0; i < pData->ItemNum; ++i) {
        pData->yData[i] = pY[i];
    }
}

/* ��ȡһ��ͼ��ľ�� */
GUI_HWIN GRAPH_GethData(GUI_HWIN hWin, int Num)
{
    LIST List;
    GRAPH_Obj *pWin = hWin;

    /* ����Ƿ�ΪGRAPH�ؼ� */
    if (WM_CheckWindowSign(hWin, WIDGET_GRAPH) == GUI_ERR) {
        return NULL;
    }
    if (Num && pWin->List) {
        List = pWin->List;
        while (Num-- && List) {
            List = List->pNext;
        }
        if (List) {
            return List->pData;
        }
    }
    return NULL;
}

/* ���һ��ͼ�� */
void GRAPH_AttachData(GUI_HWIN hGraph, GUI_HWIN hData)
{
    GRAPH_Obj *pGraph = hGraph;

    List_InsertEnd(pGraph->List, hData, 0); /* ����ĩβ����ڵ� */
}
