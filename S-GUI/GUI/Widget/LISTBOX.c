#include "LISTBOX.h"
#include "GUI.h"
#include "SCROLLBAR.h"

static void LISTBOX__DrawPage(LISTBOX_Obj *pObj);
static void LISTBOX__TextScroll(LISTBOX_Obj *pObj);
static void LISTBOX__SetSelInfo(LISTBOX_Obj *pObj);

/* LISTBOX�ؼ��Զ��ص����� */
static void __Callback(WM_MESSAGE *pMsg)
{
    static i_16 dY;
    /* ����Ƿ�ΪLISTBOX�ؼ� */
    if (WM_CheckWindowSign(pMsg->hWin, WIDGET_LISTBOX)) {
        return;
    }
    switch (pMsg->MsgId) {
        case WM_PAINT :
            LISTBOX__DrawPage(pMsg->hWin);
            break;
        case WM_DELETE :
            GUI_fastfree(pMsg->hWin);
            break;
        case WM_TP_CHECKED :
            WM_SetActiveMainWindow(pMsg->hWin);
            break;
        case WM_TP_PRESS :
            dY += GUI_GetTouchPad_MoveY();
            if (dY > 20) {
                dY = 0;
                LISTBOX_ItemDown(pMsg->hWin);
            } else if (dY < -20) {
                dY = 0;
                LISTBOX_ItemUp(pMsg->hWin);
            }
            break;
        case WM_TP_LEAVE :
            dY = 0;
            break;
        case WM_TIME_UPDATA :
            LISTBOX__TextScroll(pMsg->hWin);
            break;
    }
}

/*
 *����LISTBOX
 *x0:LISTBOX�ؼ�����������(����ڸ�����)
 *y0:LISTBOX�ؼ�����������(����ڸ�����)
 *xSize:LISTBOX�ؼ���ˮƽ���
 *ySize:LISTBOX�ؼ�����ֱ�߶�
 *hParent:�����ھ��
 *Id:����ID
 *Flag:����״̬
 *cb:�û��ص�����ָ��
 */
WM_hWin LISTBOX_Create(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      WM_hWin hParent,
                      u_16 Id,
                      u_8 Flag,
                      u_16 namepos)
{
    LISTBOX_Obj *pObj;
    
    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, Flag,
                                  WIDGET_LISTBOX, Id, __Callback,
                                  sizeof(LISTBOX_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        return NULL;
    }
    /* �����û��� */
    pObj->Widget.Win.UserRect.x0 = pObj->Widget.Win.Rect.x0;
    pObj->Widget.Win.UserRect.y0 = pObj->Widget.Win.Rect.y0;
    pObj->Widget.Win.UserRect.x1 = pObj->Widget.Win.Rect.x1;
    pObj->Widget.Win.UserRect.y1 = pObj->Widget.Win.Rect.y1;
    /* ��ɫ */
    pObj->Widget.Skin.BackColor[0] = LBOX_DFT_LBKCOLOR;   //δѡ�б���
    pObj->Widget.Skin.BackColor[1] = LBOX_DFT_LSELBKCOLOR;//ѡ�б���
    pObj->Widget.Skin.FontColor[0] = LBOX_DFT_LNCOLOR;    //δѡ������
    pObj->Widget.Skin.FontColor[1] = LBOX_DFT_LSELCOLOR;  //ѡ������
    pObj->Widget.Skin.EdgeColor[0] = LBOX_DFT_RIMCOLOR;   //�߿���ɫ
    pObj->Widget.Skin.EdgeColor[0] = LBOX_DFT_LINECOLOR;  //�ָ�����ɫ
    WIDGET_SetFont(pObj, GUI_DEF_FONT);
    pObj->ItemHei=LBOX_DFT_HEIGHT;             //��Ŀ�߶�+�ָ��߸߶�
    pObj->TopIndex = 0;
    pObj->SelIndex = 0;                           //ѡ�е�����
    pObj->SelPixs = 0;
    pObj->PageItems = ySize / (pObj->ItemHei + 1);//ÿҳ����ʾ����Ŀ��
    pObj->ItemNum=0;                //����Ŀ������
     pObj->pList=ListInit(namepos);    //������,�������ݳ���Ϊnamepos
    pObj->LastNode=pObj->pList;
    pObj->StrTab = NULL;
    pObj->hScro  = NULL;
    pObj->ScbWidth = LBOX_SCB_WIDTH;
    pObj->DispPosPix = 0;
    WM_SetWindowTimer(pObj, LBOX_DFT_SCRO_TIME);//��Ҫ���ڼ�ʱ��
    return pObj;
}

/* �����б������� */
static u_8 __CreateScro(GUI_hWin hWin)
{
    LISTBOX_Obj *pObj = hWin;
    if (pObj->hScro == NULL) {
        i_16 x0;
        GUI_RECT Rect;
        
        Rect = WM_GetWindowUserRect(pObj);
        x0 = Rect.x1 - Rect.x0 - pObj->ScbWidth + 1;
        pObj->hScro = SCROLLBAR_Create(x0, 0, pObj->ScbWidth,
                                       Rect.y1 - Rect.y0 + 1,
                                       pObj, 0x0001, 0);
        return GUI_OK;
    }
    return GUI_ERR;
}

//����һ��pObj����Ŀ
//0,���ӳɹ�;
//1,����ʧ��
u_8 LISTBOX_addlist(WM_hWin hWin,char *name)
{
    LISTBOX_Obj *pObj = hWin;
    
    if (WM_CheckWindowSign(hWin, WIDGET_LISTBOX)) {
        return GUI_ERR;
    }
    //if(pObj->StrTab) return GUI_ERR;  //�Ѿ�����ʹ��һ�������ģʽ
    Insert(name,pObj->LastNode); //���뵽�����β
    pObj->LastNode=pObj->LastNode->next;   //���б�����ʹ��
    pObj->ItemNum++;//����Ŀ������1��
    if (pObj->ItemNum > pObj->PageItems) {
        __CreateScro(pObj);
        SCROLLBAR_SetTotality(pObj->hScro, pObj->ItemNum);
        SCROLLBAR_SetLoation(pObj->hScro, pObj->SelIndex);
    }
    LISTBOX__SetSelInfo(pObj);  //ѡ����
    return GUI_OK;
}

/************************************************************
* һ�������pObj��������Ŀ�����ַ�ʽ�����Ŀ���޷���������
* *name�ĳ��ȱ������pObj->list->datalen��
* 0,���ӳɹ�
* 1,����ʧ��
*************************************************************/
u_8 LISTBOX_addall(WM_hWin hWin,const char *pTab,u_16 num)
{
    
    return 0;
}

//��ȡָ����Ŀ������
//pObjĿ���б��
//idxposҪ��ȡ���ֵ���Ŀ��
static PNode Get__ItemName(WM_hWin hWin,u_16 Item)
{
    LISTBOX_Obj *pObj = hWin;

    if(pObj->StrTab == NULL) {
        return GetNodePtr(pObj->pList,Item + 1);
    }
    return NULL;
}

static PNode Get__NextItemName(LISTBOX_Obj *pObj, PNode pNode)
{
    if(!pObj->StrTab) {
        return pNode->next;
    }
    return NULL;
}

//����һ����Ŀ
//pObjҪ���Ƶ��б��
//idxposҪ������Ŀ��ƫ��
static void LISTBOX__DrawList(LISTBOX_Obj *pObj, u_16 ItemPos, char *Str)
{
    GUI_FontType Font;
    i_16 x0, y0, xSize, ySize, xPixPos;
    GUI_COLOR FontColor, BkColor;
    GUI_RECT *pRect;
    
    /* ��ȡ�����û��� */
    pRect = &pObj->Widget.Win.UserRect; 
    /* ������Ŀλ�� */
    x0 = pRect->x0;
    y0 = pRect->y0 + (pObj->ItemHei + 1) * ItemPos;
    xSize = pRect->x1 - x0 + 1;
    ySize = pObj->ItemHei + 1;
    if(ItemPos + pObj->TopIndex == pObj->SelIndex) /* ѡ�е���Ŀ */
    {
        BkColor = WIDGET_GetBackColor(pObj, 1);
        FontColor = WIDGET_GetFontColor(pObj, 1);
        xPixPos = pObj->DispPosPix;
    } else {                    /* δѡ�е���Ŀ */
        BkColor = WIDGET_GetBackColor(pObj, 0);
        FontColor = WIDGET_GetFontColor(pObj, 0);
        xPixPos = 0;
    }
    GUI_FillTailorRect(x0, y0, xSize, ySize - 1, BkColor);
    /* �ָ��� */
    BkColor = WIDGET_GetEdgeColor(pObj, 0);
    GUI_HoriTailorLine(x0 + 1, y0 + ySize - 1, xSize - 2, BkColor);
    /* ��ʾ��Ŀ����,���ֲ��ܸ��Ƿָ��� */
    Font = WIDGET_GetFont(pObj);
    if (pObj->ItemHei > Font->CharHeight) { /* ����߶Ⱦ�����ʾ */
        y0 += (pObj->ItemHei - Font->CharHeight) / 2;
    }
    x0 -= xPixPos;   //��ʾƫ��
    GUI_DispStringCurRect(x0, y0, Str, FontColor, Font);
}

//����һҳpObj,��pObj->TopIndex��ʼ����
static void LISTBOX__DrawPage(LISTBOX_Obj *pObj)
{
    PNode pNode;
    GUI_RECT Rect;
    u_16 i, PageItems;
    i_16 x0, y0, xSize, ySize;

    PageItems = pObj->PageItems; /* һҳ������ʾ������ */
    pNode = Get__ItemName(pObj, pObj->TopIndex);
    for(i = 0; i <= PageItems; ++i) /* ��ʾ��Ŀ */
    {
        LISTBOX__DrawList(pObj, i, pNode->data);
        pNode = Get__NextItemName(pObj, pNode);
        /* �Ѿ��������һ��,��i��ȻҪ�Լ�1 */
        if(i + pObj->TopIndex + 1 == pObj->ItemNum) {
            ++i;
            break;
        }
    }
    Rect = WM_GetWindowUserRect(pObj);
    i *= (pObj->ItemHei + 1);  /* Ҫ��յ�y����ƫ�� */
    x0 = Rect.x0;
    y0 = Rect.y0 + i;
    xSize = Rect.x1 - Rect.x0 + 1;
    ySize = Rect.y1 - Rect.y0 + 1;
    /* ���Ϊ��ɫ */
    GUI_FillTailorRect(x0, y0, xSize, ySize, WIDGET_GetBackColor(pObj, 0));
}

/* LISTBOX������ʾ */
static void LISTBOX__TextScroll(LISTBOX_Obj *pObj)
{
    u_16 ItemPos;
    u_16 ListWidth;
    GUI_RECT Rect;

    Rect = WM_GetWindowUserRect(pObj);
    ListWidth = Rect.x1 - Rect.x0 + 1;
    if (ListWidth >= pObj->SelPixs) {
        return;   //����Ҫ������ʾ
    }
    if (pObj->DispPosPix == 0) {
        if (WM_GetWindowTimerCount(pObj) == LBOX_DFT_SCRO_TIME) {
            WM_SetWindowTimerCount(pObj, LBOX_DFT_UNSCRO_TIME);
            return;
        }
        WM_SetWindowTimerCount(pObj, LBOX_DFT_SCRO_TIME);
    }
    ItemPos = pObj->SelIndex - pObj->TopIndex;
    if (ListWidth + pObj->DispPosPix == pObj->SelPixs + LBOX_DFT_SCROENDWID) {
        pObj->DispPosPix = 0;
    } else {
        pObj->DispPosPix += 1;
    }
    Rect.y0 += ItemPos * (pObj->ItemHei + 1);
    Rect.y1 = Rect.y0 + pObj->ItemHei - 1;
    WM_InvalidateRect(pObj, &Rect);
}

/* ����ѡ����Ŀ����Ϣ */
static void LISTBOX__SetSelInfo(LISTBOX_Obj *pObj)
{
    char *Str;

    pObj->DispPosPix = 0;
    Str = Get__ItemName(pObj, pObj->SelIndex)->data;
    pObj->SelItem = Str;
    pObj->SelPixs = GetStringPixel(Str, WIDGET_GetFont(pObj));
}

//���б���ѡ��������һ��
void LISTBOX_ItemDown(WM_hWin hWin)
{
    LISTBOX_Obj *pObj = hWin;
    GUI_RECT Rect;
    u_16 PageItems, ItemPos;

    Rect = WM_GetWindowUserRect(pObj);
    ItemPos = pObj->SelIndex - pObj->TopIndex;
    PageItems = pObj->PageItems;
    if(pObj->SelIndex + 1 < pObj->ItemNum) { /* ��û����ʾ�����һ�� */
        pObj->SelIndex++;
        if(ItemPos + 1 == PageItems) {        /* ���˱�ҳ�����һ�� */
            pObj->TopIndex += PageItems;
        } else {
            Rect.y0 += ItemPos * (pObj->ItemHei + 1);
            Rect.y1 =  Rect.y0 + 2 * pObj->ItemHei + 1;
        }
    } else {               /* �������һ�� */
        pObj->SelIndex = 0;
        pObj->TopIndex = 0;
    }
    /* ����ѡ����Ŀ����Ϣ */
    LISTBOX__SetSelInfo(pObj);
    if (pObj->hScro) {  /* ���ù����� */
        SCROLLBAR_SetLoation(pObj->hScro, pObj->SelIndex);
    }
    WM_InvalidateRect(pObj, &Rect);  /* ��Ч������ */
}

//���б���ѡ����������һ��
void LISTBOX_ItemUp(WM_hWin hWin)
{
    LISTBOX_Obj *pObj = hWin;
    GUI_RECT Rect;
    u_16 PageItems, ItemPos, Temp;

    Rect = WM_GetWindowUserRect(pObj);
    ItemPos = pObj->SelIndex - pObj->TopIndex;
    PageItems = pObj->PageItems;
    if(pObj->SelIndex) {        /* ��û����ʾ����һ�� */
        pObj->SelIndex--;
        if(ItemPos == 0) {      /* ���˱�ҳ�ĵ�һ�� */
            pObj->TopIndex -= PageItems;
        } else {
            Rect.y0 += (ItemPos - 1) * (pObj->ItemHei + 1);
            Rect.y1 =  Rect.y0 + 2 * pObj->ItemHei + 1;
        }
    } else {                    /* ���˵�һ�� */
        pObj->SelIndex = pObj->ItemNum - 1;
        /*   �������һҳ��TopIndexֵ */
        Temp = pObj->ItemNum % PageItems;
        Temp = Temp ? Temp : PageItems;
        pObj->TopIndex = pObj->ItemNum - Temp;
    }
    /* ����ѡ����Ŀ����Ϣ */
    LISTBOX__SetSelInfo(pObj);
    if (pObj->hScro) {  /* ���ù����� */
        SCROLLBAR_SetLoation(pObj->hScro, pObj->SelIndex);
    }
    WM_InvalidateRect(pObj, &Rect);  /* ��Ч������ */
}

/* ����ѡ����Ŀ�� */
u_16 LISTBOX_GetSel(WM_hWin hWin)
{
    LISTBOX_Obj *pObj = hWin;
    
    /* ����Ƿ�ΪLISTBOX�ؼ� */
    if (WM_CheckWindowSign(hWin, WIDGET_LISTBOX)) {
        return 0;
    }
    return pObj->SelIndex;
}

/* ����ѡ����Ŀ���ı� */
char *LISTBOX_GetSelText(WM_hWin hWin)
{
    LISTBOX_Obj *pObj = hWin;
    
    /* ����Ƿ�ΪLISTBOX�ؼ� */
    if (WM_CheckWindowSign(hWin, WIDGET_LISTBOX)) {
        return 0;
    }
    return pObj->SelItem;
}

/* ����ѡ���� */
u_8 LISTBOX_SetSel(WM_hWin hWin, u_16 Index)
{
    u_16 Temp;
    LISTBOX_Obj *pObj = hWin;
    
    /* ����Ƿ�ΪLISTBOX�ؼ� */
    if (WM_CheckWindowSign(hWin, WIDGET_LISTBOX)) {
        return GUI_ERR;
    }
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
u_8 LISTBOX_SetSelFromStr(WM_hWin hWin, const char *Str)
{
    u_16 Index;
    LISTBOX_Obj *pObj = hWin;
    
    /* ����Ƿ�ΪLISTBOX�ؼ� */
    if (WM_CheckWindowSign(hWin, WIDGET_LISTBOX)) {
        return GUI_ERR;
    }
    Index = FindNodeStr(pObj->pList, (char*)Str);
    if (Index) {
        return LISTBOX_SetSel(pObj, Index - 1);
    }
    return GUI_ERR;
}
