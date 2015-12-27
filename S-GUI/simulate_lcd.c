#include "simulate_lcd.h"
#undef WM_PAINT /* WM_PAINT�궨���ͻ */
#include "GUI_Test.h"

#define SIM_LCD_WIDTH   480  /* ģ����Ļ��� */
#define SIM_LCD_HEIGHT  320  /* ģ����Ļ�߶� */

static SIM_DEVICE sim_lcd;

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
            GUI_TouchPadSendValue((u_16)sim_lcd.tPad.x, (u_16)sim_lcd.tPad.y, GUI_TP_CHECKED);
        }
        sim_updata();
        Sleep(10);
    }
    return 0;
}

/* ģ������ʼ���� */
void simulate_lcd_start(void)
{
    CreateThread(NULL, 0, ThreadGUI, NULL, 0, NULL); /* �½��߳� */
    CreateThread(NULL, 0, ThreadDisp, NULL, 0, NULL); /* �½��߳� */
}

/* ��Ļģ������ʼ�� */
void sim_lcd_init(void)
{
    sim_lcd.win_w = GetSystemMetrics(SM_CXSCREEN);
    sim_lcd.win_h = GetSystemMetrics(SM_CYSCREEN);
    sim_lcd.BufSize = sizeof(COLORREF) * sim_lcd.win_w * sim_lcd.win_h;
    /* �����ڴ棬������������ */
    sim_lcd.PixBuf = malloc(sim_lcd.BufSize);
    memset(sim_lcd.PixBuf, 0x00, sim_lcd.BufSize);
    /* ��ȡ���ھ�� */
    sim_lcd.hwnd = GetForegroundWindow();
    /* ��ȡ�ͻ�����DC */
    sim_lcd.hdc = GetDC(sim_lcd.hwnd);
    /* Ϊ֡���崴��һ��DC */
    sim_lcd.hFrame = CreateCompatibleDC(sim_lcd.hdc);
    /* Ϊ�ͻ�������һ��Bitmap */
    sim_lcd.hBmp = CreateCompatibleBitmap(sim_lcd.hdc, sim_lcd.win_w, sim_lcd.win_h);
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
    COLORREF r, g, b;
    
    r = Color & 0x000000ff;
    g = Color & 0x0000ff00;
    b = Color & 0x00ff0000;
    sim_lcd.PixBuf[y * sim_lcd.win_w + x] = r << 16 | g | b >> 16;
}

/* ��Ļģ������ȡ���� */
COLORREF sim_readPix(int x, int y)
{
    COLORREF r, g, b;

    b = sim_lcd.PixBuf[y * sim_lcd.win_w + x];
    r = b & 0x00ff0000;
    g = b & 0x0000ff00;
    b = b & 0x000000ff;
    return r >> 16 | g | b << 16;
}

/* ��Ļģ������ʾһ������ */
void sim_dispArea(int x, int y, int Width, int Height, COLORREF *Buffer, int rowlen)
{
    int i, win_w = sim_lcd.win_w;
    COLORREF r, g, b;
    COLORREF *pLCD = sim_lcd.PixBuf;

    Buffer += y * rowlen + x; /* ƫ�Ƶ���һ��Ҫ��ʾ�ĵ��λ�� */
    pLCD   += y * win_w + x; /* ƫ�Ƶ���һ��Ҫ��ʾ�ĵ��λ�� */
    while (Height--) {
        for (i = 0; i < Width; ++i) {
            b = Buffer[i];
            r = b & 0x000000ff;
            g = b & 0x0000ff00;
            b = b & 0x00ff0000;
            pLCD[i] = r << 16 | g | b >> 16;
        }
        Buffer += rowlen;
        pLCD += win_w;
    }
}

/* ��Ļģ����ˢ����ʾ,������״̬ */
void sim_updata(void)
{
    SetBitmapBits(sim_lcd.hBmp, sim_lcd.BufSize, sim_lcd.PixBuf);
    BitBlt(sim_lcd.hdc, 0, 0,
           sim_lcd.win_w, sim_lcd.win_h,
           sim_lcd.hFrame, 0, 0,
           SRCCOPY);
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
    GUI_TouchPadSendValue((u_16)sim_lcd.tPad.x, (u_16)sim_lcd.tPad.y, GUI_TP_REMOVED);
    sim_lcd.padStatus = GUI_TP_REMOVED;
}
