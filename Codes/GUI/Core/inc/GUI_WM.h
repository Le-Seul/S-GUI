#ifndef __GUI_WM_H
#define __GUI_WM_H

#include "GUI_Typedef.h"
#include "GUI_Queue.h"
#include "GUI_ClipAreaHeap.h"

/* ϵͳ������Ϣ */
#define WM_INIT_DIALOG       0x0100     /* �Ի����ʼ�� */
#define WM_PAINT             0x0101     /* �ػ� */
#define WM_DELETE            0x0102     /* ɾ������ */
#define WM_CREATED           0x0103     /* �����Ѿ����� */
#define WM_TIMER             0x0104     /* ���ڶ�ʱ������ */
#define WM_GET_CLIENT        0x0105     /* ��ȡ�ͻ������ */
#define WM_SET_FOCUS         0x0106     /* �������뽹�� */
#define WM_GET_FOCUS         0x0107     /* ��ȡ���뽹�� */
#define WM_KILL_FOCUS        0x0108     /* ����ʧȥ���� */
#define WM_QUIT              0x0109     /* �˳� */
#define WM_TP_CHECKED        0x0120     /* �����尴�� */
#define WM_TP_REMOVED        0x0121     /* �������ɿ� */
#define WM_TP_PRESS          0x0122     /* ��������һֱ�ڵ�ǰ���� */
#define WM_TP_LEAVE          0x0123     /* ���������뿪��ǰ���� */
#define WM_BUTTON_CLICKED    0x0200     /* BUTTON�ؼ����� */
#define WM_BUTTON_RELEASED   0x0201     /* BUTTON�ؼ��Ѿ��ͷ� */
#define WM_NUTTON_MOVED_OUT  0x0202     /* BUTTON�����Ȼ��ָ���ƿ� */
#define WM_WIONDOW_CHECKED   0x0203     /* ���ڱ���� */
#define WM_LISTBOX_CHECK     0x0204     /* LISTBOX�����(�µ�ѡ����) */
#define WM_KEYDOWN           0x0210     /* �������� */
#define WM_KEYUP             0x0211     /* �����ɿ� */
#define WM_USER_MESSAGE      0x1000     /* �û��Զ������Ϣ */
/* 0x0400�Ժ�Ϊ��չ��Ϣ(�û��Զ���) */

/* ����״̬����(Window status define) */
#define WM_WS_ACTIVE         (1 << 0)   /* ����� */
#define WM_WS_TRANS          (1 << 1)   /* ͸������ */
#define WM_WS_MOVE           (1 << 2)   /* ���ƶ��Ĵ��� */
#define WM_WS_INVAILD        (1 << 3)   /* ��Ч����  */
#define WM_WS_BACKGND        (1 << 4)   /* �������� */
#define WM_WS_STICK          (1 << 5)   /* �ö����� */
#define WM_WS_HIDE           (1 << 6)   /* ���صĴ��� */
#define WM_WS_MEMDEV         (1 << 7)   /* ʹ���ڴ��豸 */

/* ����ID */
#define WM_NULL_ID          0x0000
#define WM_ROOTWIN_ID       0x0001
#define WM_USER_ID          0x0200

#define GUI_COUNTOF(a)      (sizeof(a) / sizeof(a[0]))

typedef GUI_HWIN WM_HWIN;
typedef GUI_MESSAGE WM_MESSAGE;

/* ��Ϣ����ص����� */
typedef void WM_CALLBACK (WM_MESSAGE *pMsg);

/* ���ڹ��������� */
typedef struct {
    WM_HWIN hParent;        /* ������ָ�� */
    WM_HWIN hFirstChild;    /* ��һ���Ӵ���ָ�� */
    WM_HWIN hNext;          /* ָ����һ��ͬ������ */
    WM_HWIN hNextLine;      /* ָ����һ���� */
    GUI_RECT rect;          /* ���ڳߴ� */
    GUI_RECT invalidRect;   /* ������Ч���� */
    GUI_AREA clipArea;      /* ���ڼ����� */
    WM_CALLBACK *winCb;     /* ������Ϣ��Ӧ�ص����� */
    int status;             /* ����״̬ */
    int id;                 /* ����Id */
} WM_Obj;

extern WM_HWIN gui_rootwin;

/* inline functions */
static inline WM_Obj* WM_HandleToPtr(WM_HWIN hWin)
{
    return (WM_Obj *)hWin;
}

GUI_RESULT WM_Init(void);
void WM_Exec(void);
void WM__SendMessage(WM_HWIN hWin, WM_MESSAGE *pMsg);
void WM_SendMessage(WM_HWIN hWin, int MsgId, GUI_PARAM Param);
void WM_SendMessageToParent(WM_HWIN hWin, GUI_MESSAGE *pMsg);
GUI_RESULT WM_PostMessage(WM_HWIN hWin, int MsgId, GUI_PARAM Param);
void WM_GetWindowAreaRect(WM_HWIN hWin, GUI_RECT *pRect);
void WM_GetTaliorInvalidRect(WM_HWIN hWin, GUI_RECT *pRect);
WM_HWIN WM_GetFrontWindow(WM_HWIN hWin);
WM_HWIN WM_GetTopChildWindow(WM_HWIN hWin);
WM_HWIN WM_GetTopWindow(void);
void WM_AttachWindow(WM_HWIN hWin, WM_HWIN hParent);
GUI_RESULT WM_SetWindowFocus(WM_HWIN hWin);
WM_HWIN WM_GetWindowFocus(void);
GUI_RESULT WM_SetActiveWindow(WM_HWIN hWin);
WM_HWIN WM_GetActiveWindow(void);
GUI_RESULT WM_SetForegroundWindow(WM_HWIN hWin);
WM_HWIN WM_GetForegroundWindow(void);
void WM_MoveToBottom(WM_HWIN hWin);
void WM_MoveToTop(WM_HWIN hWin);
void WM_SetStickWindow(WM_HWIN hWin);
void WM_ResetStickWindow(WM_HWIN hWin);
void WM_SetBackgroundWindow(WM_HWIN hWin);
WM_HWIN WM_CreateWindowAsChild(int x0, int y0, int xSize, int ySize,
    WM_HWIN hParent, int Style, int Id, WM_CALLBACK *WinCb, u_32 bytes);
void WM_DeleteWindow(WM_HWIN hWin);
void WM_InvalidateRect(WM_HWIN hWin, GUI_RECT *pRect);
void WM_Invalidate(WM_HWIN hWin);
GUI_RESULT WM_InvalidTree(WM_HWIN hWin);
GUI_RECT * WM_GetWindowRect(WM_HWIN hWin);
GUI_RECT * WM_GetWindowInvalidRect(WM_HWIN hWin);
GUI_RESULT WM_FindWindow(WM_HWIN hWin);
WM_HWIN WM_GetDialogItem(WM_HWIN hWin, int Id);
WM_HWIN WM_GetWindowHandle(int Id);
int WM_GetDialogId(WM_HWIN hWin);
WM_HWIN WM_GetExposedWindow(int x, int y);
WM_HWIN WM_GetParentHandle(WM_HWIN hWin);
WM_HWIN WM_GetDsektopWindow(WM_HWIN hWin);
WM_HWIN WM_GetFrontHandle(WM_HWIN hWin);
void WM_MoveWindow(WM_HWIN hWin, int dx, int dy);
void WM_InvalidCoverWindow(WM_HWIN hWin, GUI_RECT *pRect);
WM_HWIN WM_GetClientWindow(WM_HWIN hWin);
GUI_RECT * WM_GetClientRect(WM_HWIN hWin);
GUI_BOOL WM_DefaultKeyProc(GUI_MESSAGE *pMsg);
void WM_DefaultProc(GUI_MESSAGE *pMsg);
void WM_SetTransWindow(WM_HWIN hWin, u_8 Status);

#endif /* __GUI_WM_H */
