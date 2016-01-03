#include "ICONVIEW.h"
#include "GUI.h"

/* ICONVIEW�ػ溯�� */
static void __Paint(GUI_hWin hWin)
{
    ICONVIEW_Obj *pObj = hWin;

}

/* ICONVIEW�ؼ���Ϣ������ */
static void __Callback(WM_MESSAGE *pMsg)
{
    /* ����Ƿ�ΪICONVIEW�ؼ� */
    if (WM_CheckWindowSign(pMsg->hWin, WIDGET_ICONVIEW)) {
        return;
    }
    switch (pMsg->MsgId) {
    case WM_PAINT :
        __Paint(pMsg->hWin);
        break;
    case WM_DELETE :
        GUI_fastfree(pMsg->hWin);
        break;
    case WM_TP_CHECKED :
        WM_SetActiveMainWindow(pMsg->hWin);
        break;
    case WM_TP_PRESS :
        
        break;
    case WM_TP_LEAVE :
        
        break;
    }
}

/*
* ����ICONVIEW
* x0:LISTBOX�ؼ�����������(����ڸ�����)
* y0:LISTBOX�ؼ�����������(����ڸ�����)
* xSize:LISTBOX�ؼ���ˮƽ���
* ySize:LISTBOX�ؼ�����ֱ�߶�
* hParent:�����ھ��
* Id:����ID
* Flag:����״̬
* cb:�û��ص�����ָ��
**/
GUI_hWin ICONVIEW_Create(i_16 x0,
    i_16 y0,
    u_16 xSize,
    u_16 ySize,
    WM_hWin hParent,
    u_16 Id,
    u_8 Flag,
    u_16 namepos)
{
    ICONVIEW_Obj *pObj;

    pObj = WM_CreateWindowAsChild(x0, y0, xSize, ySize, hParent, Flag,
        WIDGET_LISTBOX, Id, __Callback,
        sizeof(ICONVIEW_Obj) - sizeof(WM_Obj));
    if (pObj == NULL) {
        return NULL;
    }

    return pObj;
}
