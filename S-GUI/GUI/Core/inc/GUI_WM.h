#ifndef __GUI_WM_H
#define __GUI_WM_H

#include "GUI_Typedef.h"
#include "GUI_Core.h"

/* ��Ϣ���� */
/* 0����,��ʾû����Ϣ */
#define WM_NULL_EVENT    0x0000

/* GUIϵͳ������Ϣ */
#define WM_INIT_DIALOG       0x0100    /* �Ի����ʼ�� */
#define WM_PAINT             0x0101    /* �ػ� */
#define WM_PRE_PAINT         0x0102
#define WM_POST_PAINT        0x0103
#define WM_DELETE            0x0104
#define WM_TP_CHECKED        0x0105    /* �����尴�� */
#define WM_TP_REMOVED        0x0106    /* �������ɿ� */
#define WM_TP_PRESS          0x0107    /* ��������һֱ�ڵ�ǰ���� */
#define WM_TP_LEAVE          0x0108    /* ���������뿪��ǰ���� */
#define WM_BUTTON_CLICKED    0x0200    /* BUTTON�ؼ����� */
#define WM_BUTTON_RELEASED   0x0201    /* BUTTON�ؼ��Ѿ��ͷ� */
#define WM_NUTTON_MOVED_OUT  0x0202    /* BUTTON�����Ȼ��ָ���ƿ� */
#define WM_KEY_CHECKED       0x0210
#define WM_WIONDOW_CHECKED   0x0203     /* ���ڱ���� */
#define WM_TIME_UPDATA       0x0210     /* ʱ����� */
/* 1024�Ժ�Ϊ��չ��Ϣ(�û��ɶ���) */

/* Window״̬���� */
#define WM_WINDOW_ACTIVE    0x0001   /* ����� */
#define WM_WINDOW_TRANS     0x0002   /* ͸������ */
#define WM_WINDOW_MOVE      0x0004   /* ���ƶ��Ĵ��� */
#define WM_WINDOW_TIMER     0x0008   /* ������Ҫ��ʱ */


#define WM_USER_ID          0x0200

#define _pRootWin ((WM_Obj*)_hRootWin)
#define _RootWin (*_pRootWin)

typedef GUI_hWin WM_hWin;
typedef GUI_MESSAGE WM_MESSAGE;

/* ������Ϣ����ص����� */
typedef void WM_CALLBACK (WM_MESSAGE *pMsg);
/* �����ػ�ص����� */
typedef void GUI_PAINT_CALLBACK (GUI_hWin hWin);

/* ���ڹ���ע���ڵ� */
typedef struct {
    u_8 z;               /* ���ڵ�z���� */
    u_8 IadFlag;         /* ������Ч��� */
    WM_MESSAGE *pMsg;    /* ������Ϣָ�� */
    WM_CALLBACK *WinCb;  /* ���ڹ��ܻص����� */
    GUI_PAINT_CALLBACK *PaintCallback;  /* �����ػ溯�� */
} WM_REGISTRY_NODE;

/* ���ڹ��������� */
typedef struct {
    GUI_RECT Rect;          /* ���ڳߴ� */
    GUI_RECT InvalidRect;   /* ������Ч���� */
    GUI_RECT UserRect;      /* �����û����� */
    WM_hWin hParent;        /* ������ָ�� */
    WM_hWin hFirstChild;    /* ��һ���Ӵ���ָ�� */
    WM_hWin hNext;          /* ָ����һ��ͬ������ */
    WM_hWin hNextLine;      /* ָ����һ���� */
    WM_CALLBACK *WinCb;     /* ������Ϣ��Ӧ�ص����� */
    u_16 Style;             /* ���ڷ�� */
    u_8 Sign;               /* �������ͱ�ʶ�� */
    u_16 Id;                /* ����Id */
    GUI_TIME LastTime;      /* �ϴθ���ʱ�� */
    u_16 TimeCount;         /* ʱ�������� */
} WM_Obj;

GUI_RESULT WM_Init(void);
void WM_Exec(void);
void WM_SendMessage(WM_hWin hWin, u_16 MsgId, void *data);
void WM_PostMessageToParent(WM_hWin hWin, u_16 MsgId, void *data);
GUI_RECT WM_GetWindowAreaRect(WM_hWin hWin);
GUI_RECT WM_GetTaliorInvalidRect(WM_hWin hWin);
void WM_AttachWindow(WM_hWin hWin, WM_hWin hParent);
WM_hWin WM_GetActiveMainWindow(void);
GUI_RESULT WM_SetActiveMainWindow(WM_hWin hWin);
WM_hWin WM_CreateWindowAsChild(i_16 x0, i_16 y0, u_16 xSize,
                               u_16 ySize, WM_hWin hParent,
                               u_16 Flag, u_8 Sign, u_16 Id,
                               WM_CALLBACK *WinCb, u_32 bytes);
void WM_DeleteWindow(WM_hWin hWin);
u_8 WM_CheckWindowSign(WM_hWin hWin, u_16 Sign);
void WM_InvalidateRect(WM_hWin hWin, GUI_RECT *pRect);
void WM_Invalidate(WM_hWin hWin);
GUI_RESULT WM_InvalidTree(WM_hWin hWin);
u_8 WM_CheckInvalid(WM_hWin hWin);
void WM_CleanInvalid(WM_hWin hWin);
GUI_RECT WM_GetWindowRect(WM_hWin hWin);
GUI_RECT WM_GetWindowUserRect(WM_hWin hWin);
GUI_RECT WM_GetWindowInvalidRect(WM_hWin hWin);
GUI_RESULT WM_FindWindow(WM_hWin hWin);
WM_hWin WM_GetDialogItem(WM_hWin hWin, u_16 Id);
u_16 WM_GetDialogId(WM_hWin hWin);
WM_hWin WM_GetExposedWindow(u_16 x, u_16 y);
WM_hWin WM_GetParentHandle(WM_hWin hWin);
void WM_MoveWindow(WM_hWin hWin, i_16 dX, i_16 dY);
void WM_SetWindowTimer(WM_hWin hWin, u_16 Count);
void WM_SetWindowTimerCount(WM_hWin hWin, u_16 Count);
u_16 WM_GetWindowTimerCount(WM_hWin hWin);

RECT_LIST GUI_CalcWindowRectList(WM_hWin hWin);
void WM_InvalidCoverWindow(WM_hWin hWin, GUI_RECT *pRect);

#endif /* __GUI_WM_H */
