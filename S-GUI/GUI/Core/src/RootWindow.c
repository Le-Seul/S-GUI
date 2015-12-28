#include "RootWindow.h"
#include "GUI.h"
#include "stdlib.h"

static char Str[15];

static void __Paint(WM_hWin hWin)
{
    GUI_RECT Rect = WM_GetWindowRect(hWin);

    /* ���Ʊ��� */
    GUI_FillTailorRect(Rect.x0, Rect.y0, Rect.x1 - Rect.x0 + 1,
                       Rect.y1 - Rect.y0 +1, ROOTWINDOW_BACK_COLOR);
    GUI_DispStringCurRect(10, 300, Str, 0x00000000, Font_ASCII_8X16);
}

static void _RootWin_Callback(WM_MESSAGE *pMsg)
{
    static u_8 i = 0;
    GUI_RECT Rect;
    
    switch (pMsg->MsgId) {
        case WM_PAINT : {
            __Paint(pMsg->hWin);
            break;
        }
        case WM_DELETE : {  //�����ڲ���ɾ��
            break;
        }
        case WM_TIME_UPDATA :
            /* �û����� */
            Str[0] = i / 100 + '0';
            Str[1] = i % 100 / 10 + '0';
            Str[2] = i % 10 + '0';
            Str[3] = '\0';
            i++;
            Rect.x0 = 10;
            Rect.y0 = 300;
            Rect.x1 = 180;
            Rect.y1 = 320;
            WM_InvalidateRect(pMsg->hWin, &Rect);
            break;
    }
}

void WM_RootWindowInit(WM_Obj *pObj)
{
    u_16 xSize, ySize;
    
    GUI_ScreenSize(&xSize, &ySize);
    pObj->Rect.x0 = 0;
    pObj->Rect.y0 = 0;
    pObj->Rect.x1 = xSize - 1;
    pObj->Rect.y1 = ySize - 1;
    pObj->UserRect = pObj->Rect;
    pObj->WinCb = _RootWin_Callback;
    pObj->hNext = NULL;
    pObj->hParent = NULL;
    pObj->hFirstChild = NULL;
    pObj->Status = 0x0000;
    WM_Invalidate(pObj);  //��������Ч��
    WM_SetWindowTimer(pObj, 1000);//��Ҫ���ڼ�ʱ��
}
