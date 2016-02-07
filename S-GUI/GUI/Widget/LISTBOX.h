#ifndef __LISTBOX_H
#define __LISTBOX_H

#include "Widget.h"
#include "Linked_List.h"

typedef struct {
    WIDGET Widget;
    u_16 ItemHei;       /* ��Ŀ�߶� */
    u_16 ItemNum;       /* ��Ŀ������ */
    u_16 TopIndex;      /* ���˵���Ŀ���� */
    u_16 DispPosPix;    /* ��ǰ��ƫ�� */
    u_16 PageItems;     /* ÿҳ����Ŀ�� */
    u_16 SelPixs;       /* ѡ����Ŀ�����س��� */
    u_16 SelIndex;      /* ѡ�е����� */
    char *SelItem;      /* ѡ�е���Ŀ */
    List pList;         /* ���� */
    PNode LastNode;     /* β���ڵ� */
    const char *StrTab;
    WM_hWin hScro;      /* ��������� */
    u_16 ScbWidth;
}LISTBOX_Obj;

WM_hWin LISTBOX_Create(i_16 x0,
                      i_16 y0,
                      u_16 xSize,
                      u_16 ySize,
                      WM_hWin hParent,
                      u_16 Id,
                      u_8 Flag,
                      u_16 namepos);
GUI_RESULT LISTBOX_AddList(WM_hWin hWin,char *name);
void LISTBOX_ItemDown(WM_hWin hWin);
GUI_RESULT LISTBOX_ItemUp(WM_hWin hWin);
u_16 LISTBOX_GetSel(WM_hWin hWin);
char *LISTBOX_GetSelText(WM_hWin hWin);
GUI_RESULT LISTBOX_SetSel(WM_hWin hWin, u_16 Index);
GUI_RESULT LISTBOX_SetSelFromStr(WM_hWin hWin, const char *Str);
void LISTBOX_ScrollDisplay(GUI_hWin hWin);

#endif
