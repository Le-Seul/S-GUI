#include "DialogBox.h"
#include "GUI.h"

/*
 *�����Ի���
 *pWinfo,�Զ�������������
 *NumWidgets,�Զ��������Ӵ�������
 *cb,�¼���Ӧ�ص�����
 *hParent,�����ھ��,NULL��ʾû�и�����
 *x0,��ʼx����
 *y0,��ʼy����
 */
GUI_hWin GUI_CreateDialogBox(GUI_WIDGET_CREATE_INFO *pWinfo,
                             u_16 NumWidgets,
                             WM_CALLBACK *cb,
                             WM_hWin hParent,
                             i_16 x0,
                             i_16 y0)
{
#if 0
    u_16 i, xSize, ySize;
    GUI_hWin hWin;
    
    x0 += pWinfo[0].x0;
    y0 += pWinfo[0].y0;
    xSize = pWinfo[0].xSize;
    ySize = pWinfo[0].ySize;
    hWin = pWinfo[0].Auto_Create(x0, y0, xSize, ySize, pWinfo[i].Cmd);
    /* �����Ӵ��� */
    for (i = 0; i < NumWidgets; i++) {
        
    }
    return hWin;

    //u_16 i, xSize, ySize;
    GUI_hWin hWin;

    /* �����Ի��� */
    WM_CreateWindowAsChild(x0, y0, 0, 0, hParent, 0, 0, 0, cb,0);
    x0 += pWinfo[0].x0;
    y0 += pWinfo[0].y0;
    xSize = pWinfo[0].xSize;
    ySize = pWinfo[0].ySize;
    return hWin;
#endif
    return NULL;
}

/*
*ɾ���Ի���
*/
void GUI_DeleteDialogBox(GUI_hWin hWin)
{
    
}
