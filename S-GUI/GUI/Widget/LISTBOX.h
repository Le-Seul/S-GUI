#ifndef __LISTBOX_H
#define __LISTBOX_H

#include "Widget.h"
#include "linked_list.h"

typedef struct {
    WIDGET Widget;
    u_16 ItemHei;        /* ��Ŀ�߶� */
    u_16 ItemNum;        /* ��Ŀ������ */
    u_16 TopIndex;       /* ���˵���Ŀ���� */
    u_16 DispPosPix;     /* ��ǰ��ƫ�� */
    u_16 PageItems;      /* ÿҳ����Ŀ�� */
    u_16 SelPixs;        /* ѡ����Ŀ�����س��� */
    u_16 SelIndex;       /* ѡ�е����� */
    char *SelItem;       /* ѡ�е���Ŀ */
    LIST pList;          /* ���� */
    LIST LastNode;       /* β���ڵ� */
    WM_HWIN hScro;       /* ��������� */
    i_16 yPos;           /* ��һ����Ŀ��yֵƫ�� */
    char MoveFlag;       /* �ƶ����ı�� */
    i_16 ScroSpeed;      /* �����ٶ� */
    GUI_TIME lTime;      /* �ϴθ���ʱ�� */
}LISTBOX_Obj;

WM_HWIN LISTBOX_Create(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      WM_HWIN hParent,
                      u_16 Id,
                      u_8 Flag);
GUI_RESULT LISTBOX_AddList(WM_HWIN hWin,char *name);
u_16 LISTBOX_GetSel(WM_HWIN hWin);
char *LISTBOX_GetSelText(WM_HWIN hWin);
GUI_RESULT LISTBOX_SetSel(WM_HWIN hWin, u_16 Index);
GUI_RESULT LISTBOX_SetSelFromStr(WM_HWIN hWin, const char *Str);
void LISTBOX_ScrollDisplay(GUI_HWIN hWin);

#endif
