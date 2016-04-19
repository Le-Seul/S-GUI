#include "LISTBOX.h"
#include "GUI.h"
#include "SCROLLBAR.h"
#include <string.h>

#define LBOX_DFT_LBKCOLOR       0x00111F1F        /* �ڲ�����ɫ */
#define LBOX_DFT_LSELCOLOR      0x000A0A0A        /* ѡ��list���������ɫ */
#define LBOX_DFT_LSELBKCOLOR    0x004876FF        /* ѡ��list��ı���ɫ */
#define LBOX_DFT_LNCOLOR        0X00E3E3E3        /* δѡ�е�list������ɫ */
#define LBOX_DFT_RIMCOLOR       0x00000000        /* �߿���ɫ */
#define LBOX_DFT_LINECOLOR      0x002A3033        /* �ָ�����ɫ */

#define LBOX_DFT_HEIGHT         20      /* ��Ŀ�߶� */
#define LBOX_DFT_SCROENDWID     48      /* ������ʾĩβ�հ׿�� */
#define LBOX_DFT_UNSCRO_TIME    600     /* ѡ���ʼ��������ms�� */
#define LBOX_DFT_TIMER          50      /* ѡ������ʾ����һ�����ؼ��ʱ��(ms) */
#define LBOX_DFT_SCRO_PIX       2       /* ÿ�ι����������� */
#define LBOX_SCB_WIDTH          5       /* ���������Ϊ5������ */

static void _DrawPage(LISTBOX_Obj *pObj);
static void _ItemMove(LISTBOX_Obj *pObj, GUI_POINT *p);
static void _CheckSetItem(LISTBOX_Obj *pObj, GUI_POINT *p);
static void _TimerHandle(LISTBOX_Obj *pObj);
static void _SetSpeed(LISTBOX_Obj *pObj, GUI_POINT *p);
static void LISTBOX__SetSelInfo(LISTBOX_Obj *pObj);

/* LISTBOX�ؼ��Զ��ص����� */
static void __Callback(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId) {
        case WM_PAINT :
            _DrawPage(pMsg->hWin);
            break;
        case WM_DELETE :
            /* ɾ������ */
            List_Delete(((LISTBOX_Obj *)pMsg->hWin)->pList);
            break;
        case WM_TP_CHECKED :
            WM_SetForegroundWindow(pMsg->hWin);
            ((LISTBOX_Obj *)pMsg->hWin)->lTime = GUI_GetTime();
            break;
        case WM_TP_PRESS :
            _ItemMove(pMsg->hWin, (GUI_POINT *)pMsg->Param);
            _SetSpeed(pMsg->hWin, (GUI_POINT *)pMsg->Param);
            break;
        case WM_TP_REMOVED:
            _CheckSetItem(pMsg->hWin, (GUI_POINT *)pMsg->Param);
            break;
        case WM_TIME_UPDATA:
            _TimerHandle(pMsg->hWin);
            break;
        default :
            WM_DefaultProc(pMsg);
    }
}

/*
 * ����LISTBOX
 * x0:LISTBOX�ؼ�����������(����ڸ�����)
 * y0:LISTBOX�ؼ�����������(����ڸ�����)
 * xSize:LISTBOX�ؼ���ˮƽ���
 * ySize:LISTBOX�ؼ�����ֱ�߶�
 * hParent:�����ھ��
 * Id:����ID
 * Flag:����״̬
 **/
WM_HWIN LISTBOX_Create(i_16 x0,
    i_16 y0,
    u_16 xSize,
    u_16 ySize,
    WM_HWIN hParent,
    u_16 Id,
    u_8 Style)
{
    LISTBOX_Obj *pObj;
    
    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, Style,
        Id, __Callback, sizeof(LISTBOX_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        return NULL;
    }
    /* ��ɫ */
    pObj->Widget.Skin.BackColor[0] = LBOX_DFT_LBKCOLOR;   /* δѡ�б��� */
    pObj->Widget.Skin.BackColor[1] = LBOX_DFT_LSELBKCOLOR;/* ѡ�б��� */
    pObj->Widget.Skin.FontColor[0] = LBOX_DFT_LNCOLOR;    /* δѡ������ */
    pObj->Widget.Skin.FontColor[1] = LBOX_DFT_LSELCOLOR;  /* ѡ������ */
    pObj->Widget.Skin.EdgeColor[0] = LBOX_DFT_RIMCOLOR;   /* �߿���ɫ */
    pObj->Widget.Skin.EdgeColor[0] = LBOX_DFT_LINECOLOR;  /* �ָ�����ɫ */
    WIDGET_SetFont(pObj, GUI_DEF_FONT);
    pObj->ItemHei = LBOX_DFT_HEIGHT;             /* ��Ŀ�߶�+�ָ��߸߶� */
    pObj->TopIndex = 0;
    pObj->SelIndex = 0;                           /* ѡ�е����� */
    pObj->SelPixs = 0;
    pObj->PageItems = ySize / (pObj->ItemHei + 1); /* ÿҳ����ʾ����Ŀ�� */
    pObj->ItemNum = 0;                /* ����Ŀ������ */
    pObj->pList = List_Init();    /* ������,�������ݳ���Ϊnamepos */
    pObj->LastNode = pObj->pList;
    pObj->hScro = NULL;
    pObj->DispPosPix = 0;
    pObj->yPos = 0;
    pObj->MoveFlag = 0;
    pObj->ScroSpeed = 0;
    return pObj;
}

/* �����б������� */
static u_8 __CreateScro(GUI_HWIN hWin)
{
    LISTBOX_Obj *pObj = hWin;
    if (pObj->hScro == NULL) {
        i_16 x0;
        GUI_RECT Rect;
        
        Rect = WM_GetWindowRect(pObj);
        x0 = Rect.x1 - Rect.x0 - LBOX_SCB_WIDTH + 1;
        pObj->hScro = SCROLLBAR_Create(x0, 0, LBOX_SCB_WIDTH,
                                       Rect.y1 - Rect.y0 + 1,
                                       pObj, WM_NULL_ID, 0);
        return GUI_OK;
    }
    return GUI_ERR;
}

//����һ��pObj����Ŀ
//GUI_OK,���ӳɹ�;
//GUI_ERR,����ʧ��
GUI_RESULT LISTBOX_AddList(WM_HWIN hWin, char *name)
{
    LISTBOX_Obj *pObj = hWin;

    List_InsertNode(pObj->LastNode, name, strlen(name) + 1); /* ���뵽�����β */
    pObj->LastNode = pObj->LastNode->pNext;   /* ���б�����ʹ�� */
    pObj->ItemNum++; /* ����Ŀ������1�� */
    if (pObj->ItemNum > pObj->PageItems) {
        __CreateScro(pObj);
        SCROLLBAR_SetTotality(pObj->hScro, pObj->ItemNum * pObj->ItemHei);
        SCROLLBAR_SetLoation(pObj->hScro, pObj->SelIndex * pObj->ItemHei);
    }
    LISTBOX__SetSelInfo(pObj);  /* ѡ���� */
    return GUI_OK;
}

//��ȡָ����Ŀ������
//pObjĿ���б��
//idxposҪ��ȡ���ֵ���Ŀ��
static LIST Get__ItemName(WM_HWIN hWin,u_16 Item)
{
    LISTBOX_Obj *pObj = hWin;

    return List_GetNodePtr(pObj->pList, Item + 1);
}

static LIST Get__NextItemName(LISTBOX_Obj *pObj, LIST pNode)
{
    return pNode->pNext;
}

static void _DrawItem(LISTBOX_Obj *pObj, i_16 yPos, char *Str, char Sel)
{
    GUI_FONT Font;
    i_16 x0, y0, xSize, ySize, xPixPos;
    GUI_COLOR FontColor, BkColor;
    GUI_RECT *pRect;

    /* ��ȡ�����û��� */
    pRect = &pObj->Widget.Win.Rect;
    /* ������Ŀλ�� */
    x0 = pRect->x0;
    y0 = pRect->y0 + yPos;
    xSize = pRect->x1 - x0 + 1;
    ySize = pObj->ItemHei;
    if (Sel) /* ѡ�е���Ŀ */
    {
        BkColor = WIDGET_GetBackColor(pObj, 1);
        FontColor = WIDGET_GetFontColor(pObj, 1);
        xPixPos = pObj->DispPosPix;
    } else {                    /* δѡ�е���Ŀ */
        BkColor = WIDGET_GetBackColor(pObj, 0);
        FontColor = WIDGET_GetFontColor(pObj, 0);
        xPixPos = 0;
    }
    GUI_FillRect(x0, y0, xSize, ySize - 1, BkColor);
    /* �ָ��� */
    BkColor = WIDGET_GetEdgeColor(pObj, 0);
    GUI_HoriLine(x0, y0 + ySize - 1, xSize, BkColor);
    /* ��ʾ��Ŀ����,���ֲ��ܸ��Ƿָ��� */
    Font = WIDGET_GetFont(pObj);
    if (pObj->ItemHei - 1 > Font->CharHeight) { /* ����߶Ⱦ�����ʾ */
        y0 += (pObj->ItemHei - 1 - Font->CharHeight) / 2;
    }
    x0 -= xPixPos;   /* ��ʾƫ�� */
    GUI_DispStringCurRect(x0, y0, Str, FontColor, Font);
}

/* ����һҳpObj,��pObj->TopIndex��ʼ���� */
static void _DrawPage(LISTBOX_Obj *pObj)
{
    LIST pNode;
    GUI_RECT Rect;
    u_16 i, PageItems;
    i_16 x0, y0, xSize, ySize;

    y0 = pObj->yPos;
    PageItems = pObj->PageItems; /* һҳ������ʾ������ */
    pNode = Get__ItemName(pObj, pObj->TopIndex);
    for (i = 0; i <= PageItems && pNode; ++i) /* ��ʾ��Ŀ */
    {
        _DrawItem(pObj, i * pObj->ItemHei + y0,
            pNode->pData, pObj->TopIndex + i == pObj->SelIndex);
        pNode = Get__NextItemName(pObj, pNode);
        /* �Ѿ��������һ��,��i��ȻҪ�Լ�1 */
        if (i + pObj->TopIndex + 1 == pObj->ItemNum) {
            ++i;
            break;
        }
    }
    Rect = WM_GetWindowRect(pObj);
    i *= (pObj->ItemHei);  /* Ҫ��յ�y����ƫ�� */
    x0 = Rect.x0;
    y0 += Rect.y0 + i;
    xSize = Rect.x1 - Rect.x0 + 1;
    ySize = Rect.y1 - Rect.y0 + 1;
    /* ���Ϊ��ɫ */
    GUI_FillRect(x0, y0, xSize, ySize, WIDGET_GetBackColor(pObj, 0));
}

/* �����б�� */
static void _ItemMove(LISTBOX_Obj *pObj, GUI_POINT *p)
{
    i_16 Top, y, yPos;

    /* ����ƶ�̫�� */
    if (!pObj->MoveFlag && GUI_ABS(p[1].y) < 2) {
        return;
    }
    if (GUI_ABS(p[1].y) > 0) {
        y = pObj->yPos + p[1].y;
        Top = pObj->TopIndex - y / pObj->ItemHei;
        yPos = y % pObj->ItemHei;
        if (y > 0) {
            Top -= 1;
            yPos = -(pObj->ItemHei - yPos);
        }
        if (Top < 0) {
            Top = 0;
            yPos = 0;
            pObj->ScroSpeed = 0;
        } else if (Top >= pObj->ItemNum) {
            Top = pObj->ItemNum - 1;
            yPos = 0;
            pObj->ScroSpeed = 0;
        }
        pObj->yPos = yPos;
        pObj->TopIndex = Top;
        if (pObj->hScro) {  /* ���ù����� */
            SCROLLBAR_SetLoation(pObj->hScro,
                pObj->TopIndex * pObj->ItemHei - yPos);
        }
        WM_Invalidate(pObj);
        pObj->MoveFlag = 1;
    }
}

/* ���ñ������ѡ���� */
static void _CheckSetItem(LISTBOX_Obj *pObj, GUI_POINT *p)
{
    int i;

    if (!pObj->MoveFlag) {
        if (GUI_CheckPointAtRect(p[0].x, p[0].y,
            &pObj->Widget.Win.Rect) == TRUE) {
            i = pObj->TopIndex + (-pObj->yPos
                + p[0].y - pObj->Widget.Win.Rect.y0) / pObj->ItemHei;
            if (i >= 0 && i < pObj->ItemNum) {
                WM_MESSAGE Msg;

                pObj->SelIndex = (u_16)i;
                /* ����ѡ����Ŀ����Ϣ */
                LISTBOX__SetSelInfo(pObj);
                WM_Invalidate(pObj);  /* ��Ч������ */
                Msg.MsgId = WM_LISTBOX_CHECK;
                WM_SendMessageToParent(pObj, &Msg);
            }
        }
    } else {
        if (GUI_ABS(pObj->ScroSpeed) > 20) {
            GUI_SetWindowTimer(pObj, LBOX_DFT_TIMER);
        } else {
            pObj->MoveFlag = 0;
        }
    }
}

/* ���ù����ٶ� */
static void _SetSpeed(LISTBOX_Obj *pObj, GUI_POINT *p)
{
    GUI_TIME t = GUI_GetTime();

    pObj->ScroSpeed = (i_16)(p[1].y * LBOX_DFT_TIMER / (int)(t - pObj->lTime + 1));
    pObj->ScroSpeed *= 4;
    pObj->lTime = t;
}

/* ���ٹ��� */
static void _TimerHandle(LISTBOX_Obj *pObj)
{
    if (pObj->MoveFlag) {
        if (GUI_ABS(pObj->ScroSpeed)) {
            i_16 d;
            GUI_POINT Point[2];

            Point[1].y = pObj->ScroSpeed;
            _ItemMove(pObj, Point);
            d = -pObj->ScroSpeed / 8;
            if (GUI_ABS(d) == 0 && pObj->ScroSpeed) {
                d = pObj->ScroSpeed > 0 ? -1 : 1;
            }
            pObj->ScroSpeed += d;
            if (!pObj->ScroSpeed) {
                pObj->ScroSpeed = 0;
                pObj->MoveFlag = 0;
                GUI_SetWindowTimer(pObj, 0);
            }
        } else {
            pObj->ScroSpeed = 0;
            GUI_SetWindowTimer(pObj, 0);
        }
    }
}

/* ����ѡ����Ŀ����Ϣ */
static void LISTBOX__SetSelInfo(LISTBOX_Obj *pObj)
{
    char *Str;

    pObj->DispPosPix = 0;
    Str = Get__ItemName(pObj, pObj->SelIndex)->pData;
    pObj->SelItem = Str;
    pObj->SelPixs = GetStringPixel(Str, WIDGET_GetFont(pObj));
}

/* ����ѡ����Ŀ�� */
u_16 LISTBOX_GetSel(WM_HWIN hWin)
{
    LISTBOX_Obj *pObj = hWin;

    return pObj->SelIndex;
}

/* ����ѡ����Ŀ���ı� */
char *LISTBOX_GetSelText(WM_HWIN hWin)
{
    LISTBOX_Obj *pObj = hWin;

    return pObj->SelItem;
}

/* ����ѡ���� */
GUI_RESULT LISTBOX_SetSel(WM_HWIN hWin, u_16 Index)
{
    u_16 Temp;
    LISTBOX_Obj *pObj = hWin;
    
    if (Index < pObj->ItemNum) {
        pObj->SelIndex = Index;
        if (pObj->hScro) {  /* ���ù����� */
            SCROLLBAR_SetLoation(pObj->hScro, Index);
        }
        Index += 1;
        Temp = Index % pObj->PageItems;
        if (Temp) {
            pObj->TopIndex = Index - Temp;
        } else {
            pObj->TopIndex = Index - pObj->PageItems;
        }
        /* ����ѡ����Ŀ����Ϣ */
        LISTBOX__SetSelInfo(pObj);
        return GUI_OK;
    }
    return GUI_ERR;  /* ��������һ�� */
}

/* ����ѡ����,��������ַ��������� */
GUI_RESULT LISTBOX_SetSelFromStr(WM_HWIN hWin, const char *Str)
{
    u_16 Index;
    LISTBOX_Obj *pObj = hWin;
    
    Index = (u_16)List_FindStr(pObj->pList, (char*)Str);
    if (Index) {
        return LISTBOX_SetSel(pObj, Index - 1);
    }
    return GUI_ERR;
}

/* �б��ʹ�ù�����ʾ���� */
void LISTBOX_ScrollDisplay(GUI_HWIN hWin)
{
    GUI_SetWindowTimer(hWin, LBOX_DFT_TIMER);
}
