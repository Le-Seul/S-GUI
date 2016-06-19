#include "GUI_DispChar.h"
#include "GUI.h"
#include "string.h"
#include "GUI_Font.h"

/* ��ȡ�ַ���Ϣ */
static GUI_CHARINFO * _GetCharInfo(const u_16 Char)
{
    GUI_FONT_PROP *pProp = GUI_Context.Font->pProp;

    while (pProp) {
        if (Char >= pProp->FirstChar && Char <= pProp->LastChar) {
            return &pProp->pCharInfo[Char - pProp->FirstChar];
        }
        pProp = pProp->pNext;
    }
    return NULL;
}

/* ��ȡ��ǰ�����ַ��ĸ߶� */
u_16 GUI_GetCharHeight(void)
{
    return GUI_Context.Font->CharHeight;
}

/* ��ȡ��ǰ�����ַ��Ŀ�� */
u_16 GUI_GetCharWidth(u_16 Char)
{
    GUI_CHARINFO *pInfo = _GetCharInfo(Char);

    if (pInfo) {
        return pInfo->XSize;
    }
    return 0;
}

/* ��ȡ��ǰ�ַ�����������ֵ, �ᱻ'\r', '\n', '\0'�ض� */
int GUI_GetStringWidth(const char *Str)
{
    int Width = 0;

    while (*Str && *Str != '\n' && *Str != '\r') {
        Width += GUI_GetCharWidth(*Str++);
    }
    return Width;
}

#if GUI_DISP_FONT_MODE == 0
/* �ַ���ʾ͸���Ȼ�Ϻ��� */
static void _CharSetPixel(i_16 x, i_16 y, u_8 light, u_8 pp)
{
    GUI_COLOR FontColor = GUI_Context.FontColor;

    pp = (1 << pp) - 1;
    if (light == pp) { /* ���ؼ���͸���� */
        GL_SetPixel(x, y, FontColor);
    } else { /* ������������͸���� */
        GUI_COLOR BGColor = GL_GetPixel(x, y);
        GUI_COLOR R = ((FontColor >> 16) & 0xFF) * light / pp;
        GUI_COLOR G = ((FontColor >> 8) & 0xFF) * light / pp;
        GUI_COLOR B = (FontColor & 0xFF) * light / pp;

        light = pp - light;
        R += ((BGColor >> 16) & 0xFF) * light / pp;
        G += ((BGColor >> 8) & 0xFF) * light / pp;
        B += (BGColor & 0xFF) * light / pp;
        GL_SetPixel(x, y, R << 16 | G << 8 | B);
    }
}

/* ��ʾ�ַ� */
static void _DispChar(i_16 x0,    /* ��ʾλ�� */
    i_16 y0,                      /* ��ʾλ�� */
    u_16 x1,                      /* λͼ�߶� */
    u_16 y1,                      /* λͼ��� */
    const unsigned char *pData,  /* λͼ���� */
    u_8 PixBits,                 /* ÿ���ر����� */
    u_16 BytesPerLine)           /* ÿ�е��ֽ��� */
{
    unsigned char data, light, bitCnt;
    u_8 BytePixels = 8 / PixBits, x0Shift, shift = 8 - PixBits;
    i_16 x = x0, y = y0;
    u_16 ySize, i, j;
    const unsigned char *ptab;

    /* �ü�����ʾ���� */
    CLIP_X(x0, x1);
    CLIP_Y(y0, y1);
    ySize = y1 - y0 + 1;
    pData += (y0 - y) * BytesPerLine + (x0 - x) / BytePixels;
    x0Shift = PixBits * ((x0 - x) & (BytePixels - 1));
    for (j = 0; j < ySize; ++j, pData += BytesPerLine) {
        ptab = pData;
        data = *ptab << x0Shift;
        bitCnt = 8 - x0Shift;
        for (i = x0; i <= x1; ++i) {
            light = data >> shift;
            data <<= PixBits;
            bitCnt -= PixBits;
            if (!bitCnt) {
                data = *(++ptab);
                bitCnt = 8;
            }
            if (light) { /* ��ʾ */
                _CharSetPixel(i, y0 + j, light, PixBits);
            }
        }
    }
}
#else
/* ��ѹ�����ַ�, ����ʱʹ�� */
static void _DecompressData(GUI_COLOR *Dst,
    const unsigned char *Src,
    u_8 PixBits,
    int BytesSize,
    GUI_COLOR Color)
{
    u_16 i, temp, code;
    unsigned char scale = 255 / ((1 << PixBits) - 1);
    unsigned char BytePixs = 8 / PixBits;
    unsigned char shift = 8 - PixBits;
    unsigned char mask = 0xFF << shift;

    while (BytesSize--) {
        code = *Src;
        for (i = 0; i < BytePixs; ++i) {
            temp = (code & mask) >> shift;
            code <<= PixBits;
            *Dst++ = ((255 - temp * scale) << 24) | Color;
        }
        ++Src;
    }
}

/* ��ʾ�ַ� */
static void _DispChar(i_16 x0,    /* ��ʾλ�� */
    i_16 y0,                      /* ��ʾλ�� */
    i_16 x1,                      /* λͼ�߶� */
    i_16 y1,                      /* λͼ��� */
    const unsigned char *pData,   /* λͼ���� */
    u_8 PixBits,                  /* ÿ���ر����� */
    u_16 BytesPerLine)            /* ÿ�е��ֽ��� */
{
    i_16 x = x0, y = y0;
#if GUI_DISP_FONT_MODE == 1
    i_16 i;
    static GUI_COLOR buf[GUI_MAX_FONT_WID];
#else
    static GUI_COLOR buf[GUI_MAX_FONT_WID * GUI_MAX_FONT_HEI];
#endif
    GUI_COLOR Color = GUI_Context.FontColor & 0x00FFFFFF;

    /* �ü�����ʾ���� */
    CLIP_X(x0, x1);
    CLIP_Y(y0, y1);
    pData += (y0 - y) * BytesPerLine;
#if GUI_DISP_FONT_MODE == 1
    for (i = y0; i <= y1; ++i) {
        _DecompressData(buf, pData, PixBits, BytesPerLine, Color);
        GL_DrawBitmap(GUI_ARGB8888, (const unsigned char *)(buf + x0 - x), x0, i, x1 - x0 + 1, 1, 0);
        pData += BytesPerLine;
    }
#else
    _DecompressData(buf, pData, PixBits, BytesPerLine * (y1 - y0 + 1), Color);
    GL_DrawBitmap(GUI_ARGB8888, (const unsigned char *)(buf + x0 - x),
        x0, y0, x1 - x0 + 1, y1 - y0 + 1, BytesPerLine * (8 / PixBits) - x1 + x0 - 1);
#endif
}
#endif

/* ��ʾһ���ַ�(��������) */
static u_16 _DispCharAbs(i_16 x, i_16 y, u_16 Char)
{
    u_8 PixBits = GUI_Context.Font->FontType;
    u_16 Width, Height = GUI_GetCharHeight();
    GUI_CHARINFO *cInfo = _GetCharInfo(Char);

    if (cInfo) {
        GUI_RECT r;
        u_8 BytesPerLine = cInfo->BytesPerLine;
        const unsigned char *pData = cInfo->pData;

        Width = cInfo->XSize;
        GUI_Val2Rect(&r, x, y, Width, Height);
        GUI_DrawAreaInit(&r);
        while (GUI_GetNextArea()) { /* �������е���ʾ���� */
            _DispChar(r.x0, r.y0, r.x1, r.y1, pData, PixBits, BytesPerLine);
        }
        return Width;
    }
    return 0; /* ������û�и��ַ� */
}

/* ��ָ��������������ʾһ���ַ�(��������) */
static u_16 _DispCharInRectAbs(GUI_RECT *pRect, u_16 Char)
{
    u_8 PixBits = GUI_Context.Font->FontType;
    u_16 Width, Height = GUI_GetCharHeight();
    GUI_CHARINFO *cInfo = _GetCharInfo(Char);

    if (cInfo) {
        GUI_RECT r;
        u_8 BytesPerLine = cInfo->BytesPerLine;
        const unsigned char *pData = cInfo->pData;

        Width = cInfo->XSize;
        GUI_Val2Rect(&r, pRect->x0, pRect->y0, Width, Height);
        if (GUI_RectOverlay(&r, pRect, &r) == TRUE) {
            GUI_DrawAreaInit(&r);
            while (GUI_GetNextArea()) { /* �������е���ʾ���� */
                _DispChar(pRect->x0, pRect->y0, pRect->x1,
                    pRect->y1, pData, PixBits, BytesPerLine);
            }
            return Width;
        }
    }
    return 0; /* ������û�и��ַ����߲�����ʾ��Χ�� */
}

/* ��ʾһ���ַ� */
u_16 GUI_DispChar(i_16 x, i_16 y, u_16 Char)
{
    return _DispCharAbs(x, y, Char);
}

/* ��ָ��������������ʾһ���ַ� */
u_16 GUI_DispCharInRect(GUI_RECT *pRect, u_16 Char)
{
    GUI_RECT r = *pRect;

    GUI_ClientToScreenRect(&r);
    return _DispCharInRectAbs(&r, Char);
}

/* �ڵ�ǰ����ʾ��������ʾһ���ַ��� */
void GUI_DispString(i_16 x0, i_16 y0, const char *Str)
{
    GUI_ClientToScreen(&x0, &y0);
    while (*Str) {
        x0 += _DispCharAbs(x0, y0, *Str++);
    }
}

/* ��ָ��������������ʾһ���ַ��� */
void GUI_DispStringInRect(GUI_RECT *pRect, const char *Str, u_8 Align)
{
    GUI_RECT r = *pRect;
    u_16 Height = GUI_GetCharHeight();
    int Width = GUI_GetStringWidth(Str);

    GUI_ClientToScreenRect(&r);
    /* ����ˮƽ���뷽ʽ, Ĭ������� */
    switch (Align & (GUI_ALIGN_LEFT | GUI_ALIGN_RIGHT | GUI_ALIGN_HCENTER)) {
    case GUI_ALIGN_RIGHT: /* �Ҷ��� */
        r.x0 = r.x1 - Width;
        break;
    case GUI_ALIGN_HCENTER: /* ˮƽ���� */
        if (r.x1 - r.x0 + 1 > Width) {
            r.x0 = r.x0 + (r.x1 - r.x0 + 1 - Width) / 2;
            break;
        }
    }
    /* ���㴹ֱ���뷽ʽ, Ĭ�϶����� */
    switch (Align & (GUI_ALIGN_TOP | GUI_ALIGN_BOTTOM | GUI_ALIGN_VCENTER)) {
    case GUI_ALIGN_BOTTOM: /* �׶��� */
        r.y0 = r.y1 - Height;
        break;
    case GUI_ALIGN_VCENTER: /* ��ֱ���� */
        if (r.y1 - r.y0 + 1 > Height) {
            r.y0 = r.y0 + (r.y1 - r.y0 + 1 - Height) / 2;
            break;
        }
    }
    while (*Str && *Str != '\n' && *Str != '\r1') {
        r.x0 += _DispCharInRectAbs(&r, *Str++);
    }
}
