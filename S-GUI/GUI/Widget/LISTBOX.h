#ifndef __LISTBOX_H
#define __LISTBOX_H

#include "Widget.h"
#include "Linked_List.h"

#define LBOX_DFT_LBKCOLOR       0x00524C3C        /* �ڲ�����ɫ */
#define LBOX_DFT_LSELCOLOR      0x00FFFFFF        /* ѡ��list���������ɫ */
#define LBOX_DFT_LSELBKCOLOR    0x00CDCD96        /* ѡ��list��ı���ɫ */
#define LBOX_DFT_LNCOLOR        0x00000000        /* δѡ�е�list������ɫ */
#define LBOX_DFT_RIMCOLOR       0x00000000        /* �߿���ɫ */
#define LBOX_DFT_LINECOLOR      0x0033302A        /* �ָ�����ɫ */

#define LBOX_DFT_HEIGHT         28      /* ��Ŀ�߶� */
#define LBOX_DFT_SCROENDWID     48      /* ������ʾĩβ�հ׿�� */
#define LBOX_DFT_UNSCRO_TIME    600     /* ѡ���ʼ��������ms�� */
#define LBOX_DFT_SCRO_TIME      50      /* ѡ������ʾ����һ�����ؼ��ʱ��(ms) */
#define LBOX_SCB_WIDTH          5       /* ���������Ϊ5������ */

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
u_8 LISTBOX_addlist(WM_hWin hWin,char *name);
void LISTBOX_ItemDown(WM_hWin hWin);
void LISTBOX_ItemUp(WM_hWin hWin);
u_16 LISTBOX_GetSel(WM_hWin hWin);
char *LISTBOX_GetSelText(WM_hWin hWin);
u_8 LISTBOX_SetSel(WM_hWin hWin, u_16 Index);
u_8 LISTBOX_SetSelFromStr(WM_hWin hWin, const char *Str);

#endif
