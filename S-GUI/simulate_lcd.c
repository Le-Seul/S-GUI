#include "simulate_lcd.h"
#include "GUI_Test.h"

#define SIM_LCD_WIDTH   480  /* ģ����Ļ��� */
#define SIM_LCD_HEIGHT  320  /* ģ����Ļ�߶� */

static SIM_DEVICE sim_lcd;

/* ��ʱ��һ�� */
GUI_RESULT GUI_Init(void);
void GUI_Delay(GUI_TIME tms);

/* GUI�����߳� */
static DWORD WINAPI ThreadGUI(LPVOID pM)
{
    GUI_Test();
    return 0;
}

/* ˢ����ʾ�߳� */
static DWORD WINAPI ThreadDisp(LPVOID pM)
{
    while (1) {
        /* �����ģ�ⴥ�� */
        if (sim_lcd.padStatus == GUI_TP_CHECKED) {
            GetCursorPos(&sim_lcd.tPad); /* ��ȡ���λ�� */
            ScreenToClient(sim_lcd.hwnd, &sim_lcd.tPad);
            GUI_TouchPadSendValue((u_16)sim_lcd.tPad.x,
                (u_16)sim_lcd.tPad.y, GUI_TP_CHECKED);
        }
        sim_updata();
        Sleep(20);
    }
    return 0;
}

/* ģ������ʼ���� */
void simulate_lcd_start(HWND hWnd)
{
    /* ��ȡ���ھ�� */
    sim_lcd.hwnd = hWnd;
    CreateThread(NULL, 0, ThreadGUI, "ThreadGUI", 0, NULL); /* �½��߳� */
    CreateThread(NULL, 0, ThreadDisp, "ThreadDisp", 0, NULL); /* �½��߳� */
}

/* ��Ļģ������ʼ�� */
void sim_lcd_init(void)
{
    sim_lcd.win_w = GetSystemMetrics(SM_CXSCREEN);
    sim_lcd.win_h = GetSystemMetrics(SM_CYSCREEN);
    sim_lcd.Update = 0;
    sim_lcd.BufSize = sizeof(COLORREF) * sim_lcd.win_w * sim_lcd.win_h;
    /* �����ڴ棬������������ */
    sim_lcd.PixBuf = malloc(sim_lcd.BufSize);
    memset(sim_lcd.PixBuf, 0x00, sim_lcd.BufSize);
    /* ��ȡ�ͻ�����DC */
    sim_lcd.hdc = GetDC(sim_lcd.hwnd);
    /* Ϊ֡���崴��һ��DC */
    sim_lcd.hFrame = CreateCompatibleDC(sim_lcd.hdc);
    /* Ϊ�ͻ�������һ��Bitmap */
    sim_lcd.hBmp = CreateCompatibleBitmap(sim_lcd.hdc,
        sim_lcd.win_w, sim_lcd.win_h);
    /* Ϊ֡�����DCѡ��client_bitmap��Ϊ���� */
    SelectObject(sim_lcd.hFrame, sim_lcd.hBmp);
}

/* ��ȡ������Ļ��� */
int sim_getWidth(void)
{
    return SIM_LCD_WIDTH;
}

/* ��ȡ������Ļ�߶� */
int sim_getHeight(void)
{
    return SIM_LCD_HEIGHT;
}

/* ��Ļģ������ʾ���� */
void sim_drawPix(int x, int y, COLORREF Color)
{
    sim_lcd.PixBuf[y * sim_lcd.win_w + x] = Color;
    sim_lcd.Update = 2;
}

/* ��Ļģ������ȡ���� */
COLORREF sim_readPix(int x, int y)
{
    return sim_lcd.PixBuf[y * sim_lcd.win_w + x];
}

/* ��Ļģ������ʾһ������ */
void sim_dispArea(int x,
    int y,
    int Width,
    int Height,
    COLORREF *Buffer,
    int rowlen)
{
    int i, win_w = sim_lcd.win_w;
    COLORREF *p1, *p2, *pLCD = sim_lcd.PixBuf;

    Buffer += y * rowlen + x; /* ƫ�Ƶ���һ��Ҫ��ʾ�ĵ��λ�� */
    pLCD   += y * win_w + x; /* ƫ�Ƶ���һ��Ҫ��ʾ�ĵ��λ�� */
    while (Height--) {
        p1 = pLCD;
        p2 = Buffer;
        for (i = 0; i < Width; ++i) {
            *p1++ = *p2++;
        }
        Buffer += rowlen;
        pLCD += win_w;
    }
    sim_lcd.Update = 1;
}

/* ��Ļģ����ˢ����ʾ,������״̬ */
void sim_updata(void)
{
    if (sim_lcd.Update) {
        SetBitmapBits(sim_lcd.hBmp, sim_lcd.BufSize, sim_lcd.PixBuf);
        BitBlt(sim_lcd.hdc, 0, 0,
            sim_lcd.win_w, sim_lcd.win_h,
            sim_lcd.hFrame, 0, 0,
            SRCCOPY);
        /* ���sim_lcd.UpdateΪ1���ʾ��֡���£�������Ҫһֱˢ�� */
        if (sim_lcd.Update == 1) {
            sim_lcd.Update = 0; /* ��λ���±�־ */
        } else {
            Sleep(10);
        }
    }
}

/* �����豸���� */
void sim_pad_cleck(void)
{
    GetCursorPos(&sim_lcd.tPad); /* ��ȡ���λ�� */
    ScreenToClient(sim_lcd.hwnd, &sim_lcd.tPad);
    sim_lcd.padStatus = GUI_TP_CHECKED;
}

/* �����豸�ɿ� */
void sim_pad_removed(void)
{
    GUI_TouchPadSendValue((u_16)sim_lcd.tPad.x,
        (u_16)sim_lcd.tPad.y, GUI_TP_REMOVED);
    sim_lcd.padStatus = GUI_TP_REMOVED;
}

/* ����ֵ���ݣ����ã���Win32���������ֵ���ݵ�S-GUI */
void sim_key_send(WPARAM KeyVal, UINT Status)
{
    u_8 Key;

    /* ��ֵת�� */
    switch (KeyVal) {
    case VK_UP:
        Key = KEY_UP;
        break;
    case VK_DOWN:
        Key = KEY_DOWN;
        break;
    case VK_LEFT:
        Key = KEY_LEFT;
    case VK_RIGHT:
        Key = KEY_RIGHT;
        break;
    case VK_SHIFT:
        /* �ڶ����ܼ����� */
        break;
    default:
        Key = (u_8)KeyVal; /* ������ֱ�Ӹ�ֵ */
    }
    if (Status == WM_KEYDOWN) {
        GUI_SendKey(Key, GUI_KEYDOWN);
    } else if (Status == WM_KEYUP) {
        GUI_SendKey(Key, GUI_KEYUP);
    }
}
