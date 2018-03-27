/*****************************************************************************
 * S-GUI 2Dͼ�ο�
 *****************************************************************************/

#include "GUI_GL.h"
#include "GUI_DrawBase.h"
#include "GUI.h"
#include "GUI_Math.h"

/* ȫ͸��ֱ�ӷ��� */
#define RETURN_TRANSPARENT() \
    if (GUI_CurrentForeground() >> 24 == 0xff) return;

/* ���� */
void GUI_DrawPoint(int x,int y)
{
    if (GUI_PenSize() > 1) {
        GUI_FillCircle(x, y, GUI_PenSize() / 2);
    } else {
        gui_gl_apis->setPixel(x, y, GUI_CurrentForeground());
    }
}

/* ����ֱ��
 @ x0,y0:����
 @ len:�߳���
 @ color:��ɫ
 **/
void GUI_VertLine(int x, int y, int len)
{
    gui_gl_apis->drawVLine(x, y, y + len - 1, GUI_CurrentForeground());
}

/* ��ˮƽ��
 @ x0,y0:����
 @ len:�߳���
 @ color:��ɫ
 **/
void GUI_HoriLine(int x, int y, int len)
{
    gui_gl_apis->drawHLine(x, y, x + len - 1, GUI_CurrentForeground());
}

/* �����ο� */
void GUI_DrawRect(int x0, int y0, int width, int height)
{
    int x1 = x0 + width - 1, y1 = y0 + height - 1;
    GUI_COLOR color = GUI_CurrentForeground();

    gui_gl_apis->drawHLine(x0, y0, x1, color);
    gui_gl_apis->drawHLine(x0, y1, x1, color);
    gui_gl_apis->drawVLine(x0, y0 + 1, y1 - 1, color);
    gui_gl_apis->drawVLine(x1, y0 + 1, y1 - 1, color);
}

/* ������ */
void GUI_FillRect(int x0, int y0, int width, int height)
{
    gui_gl_apis->fillRect(x0, y0,
        x0 + width - 1, y0 + height - 1,
        GUI_CurrentForeground());
}

/* ������ֱ�� */
void GUI_DrawLine(int x0, int y0, int x1, int y1)
{
    int dx, dy, p, diff, addx, addy, i;
    GUI_COLOR color = GUI_CurrentForeground();

    if (x0 < x1) {
        dx = x1 - x0;
        addx = 1;
    } else {
        dx = x0 - x1;
        addx = -1;
    }
    if (y0 < y1) {
        dy = y1 - y0;
        addy = 1;
    } else {
        dy = y0 - y1;
        addy = -1;
    }
    if (dx >= dy) {
        dy <<= 1;
        p = dy - dx;
        diff = p - dx;
        for (i = 0; i <= dx; ++i) {
            gui_gl_apis->setPixel(x0, y0, color);
            x0 += addx;
            if (p < 0) {
                p += dy;
            } else {
                y0 += addy;
                p += diff;
            }
        }
    } else {
        dx <<= 1;
        p = dx - dy;
        diff = p - dy;
        for (i = 0; i <= dy; ++i) {
            gui_gl_apis->setPixel(x0, y0, color);
            y0 += addy;
            if (p < 0) {
                p += dx;
            } else {
                x0 += addx;
                p += diff;
            }
        }
    }
}

/* ��8�� */
static void _Draw8Point(int x0, int y0, int x, int y)
{
    GUI_COLOR color = GUI_CurrentForeground();

    gui_gl_apis->setPixel(x0 + x, y0 - y, color);
    gui_gl_apis->setPixel(x0 - y, y0 - x, color);
    gui_gl_apis->setPixel(x0 - x, y0 + y, color);
    gui_gl_apis->setPixel(x0 + y, y0 + x, color);
    if (x && x != y) {
        gui_gl_apis->setPixel(x0 - x, y0 - y, color);
        gui_gl_apis->setPixel(x0 - y, y0 + x, color);
        gui_gl_apis->setPixel(x0 + x, y0 + y, color);
        gui_gl_apis->setPixel(x0 + y, y0 - x, color);
    }
}

/* ��Բ */
void GUI_DrawCircle(int x0, int y0, int r)
{
    int x, y = r, d = -(r >> 1);

    for (x = 0; x <= y; ++x) {
        if (d < 0) {
            d += x;
        } else {
            d += x - y;
            --y;
        }
        _Draw8Point(x0, y0, x, y);
    }
}

/* ���Բ */
void GUI_FillCircle(int x0, int y0, int r)
{
    GUI_COLOR color = GUI_CurrentForeground();
    int i, x;
    int sqmax = r * r + r / 2;
    int yMin, yMax;
    /* First step : find uppermost and lowermost coordinates */
    yMin = y0 - r;
    yMax = y0 + r;

    /* Draw top half */
    for (i = 0, x = r; i <= r; i++) {
        int y = y0 - i;
        if ((y >= yMin) && (y <= yMax)) {
            /* calc proper x-value */
            while ((i * i + x * x) > sqmax)
                --x;
            gui_gl_apis->drawHLine(x0 - x, y, x0 + x, color);
        }
    }
    /* Draw bottom half */
    for (i = 0, x = r; i <= r; i++) {
        int y = y0 + i;
        if ((y >= yMin) && (y <= yMax)) {
            /* calc proper x-value */
            while ((i * i + x * x) > sqmax)
                --x;
            gui_gl_apis->drawHLine(x0 - x, y, x0 + x, color);
        }
    }
}

/* ��ȡ����εı߽緶Χ */
void GUI_GetPolyArea(GUI_RECT *r, GUI_POINT *Points, int cnt)
{
    int t;
    int i;

    r->x0 = Points->x;
    r->x1 = r->x0;
    r->y0 = Points->y;
    r->y1 = r->y0;
    for (i = 0; i < cnt; ++i) {
        t = Points[i].x;
        if (t > r->x1) {
            r->x1 = t;
        }
        if (t < r->x0) {
            r->x0 = t;
        }
        t = Points[i].y;
        if (t > r->y1) {
            r->y1 = t;
        }
        if (t < r->y0) {
            r->y0 = t;
        }
    }
}

/* ���ƶ���� */
void GUI_DrawPolygon(GUI_POINT *Points, int cnt)
{
    int i;
    int xPos = 0;
    int yPos = 0;
    GUI_POINT *p1, *p2;

    GUI_ClientToScreen(&xPos, &yPos); /* ��ȡ��Ļƫ�� */
    for (i = 0; i < cnt - 1; ++i) {
        p1 = Points + i;
        p2 = Points + i + 1;
        GUI_DrawLine(p1->x + xPos, p1->y + yPos, p2->x + xPos, p2->y + yPos);
    }
    p1 = Points + i;
    p2 = Points;
    GUI_DrawLine(p1->x + xPos, p1->y + yPos, p2->x + xPos, p2->y + yPos);
}

/* �������, �ڲ����� */
void GUI_FillPolygon(GUI_POINT *Points, int cnt)
{
    int xPos = 0;
    int yPos = 0;
    int yMax = Points->y, yMin = yMax;
    int y, i, j, nodes;
    static int nodeX[GUI_MAX_POLY_CORNERS];
    GUI_POINT *pi, *pj;
    GUI_COLOR color = GUI_CurrentForeground();

    if (cnt > GUI_MAX_POLY_CORNERS) {
        return;
    }

    GUI_ClientToScreen(&xPos, &yPos); /* ��ȡ��Ļƫ�� */
    /* �ҳ���������к���������� */
    for (i = 0; i < cnt; ++i) {
        y = Points[i].y;
        if (y > yMax) {
            yMax = y;
        }
        if (y < yMin) {
            yMin = y;
        }
    }
    for (y = yMin; y <= yMax; ++y) {
        /* ����������ɨ���ߵĽ��� */
        nodes = 0;
        j = cnt - 1;
        for (i = 0; i < cnt; ++i) {
            pi = Points + i; /* �ߵ���� */
            pj = Points + j; /* �ߵ��յ� */
            if (pi->y == y && pj->y == y) { /* ˮƽ��ֱ����� */
                if (pi->x < pj->x) {
                    gui_gl_apis->drawHLine(pi->x + xPos, y + yPos, pj->x + xPos, color);
                } else {
                    gui_gl_apis->drawHLine(pj->x + xPos, y + yPos, pi->x + xPos, color);
                }
            } else if (((pi->y < y) && (pj->y >= y))
                || ((pj->y < y) && (pi->y >= y))) {
                nodeX[nodes++] = pi->x + ((y - pi->y)
                    * (pj->x - pi->x)) / (pj->y - pi->y);
            }
            j = i;
        }
        /* ������������� */
        i = 0;
        while (i < nodes - 1) {
            if (nodeX[i] > nodeX[i + 1]) {
                j = nodeX[i];
                nodeX[i] = nodeX[i + 1];
                nodeX[i + 1] = j;
                if (i) {
                    --i;
                }
            } else {
                ++i;
            }
        }
        /* ����߻��� */
        for (i = 0; i < nodes; i += 2) {
            gui_gl_apis->drawHLine(nodeX[i] + xPos, y + yPos, nodeX[i + 1] + xPos, color);
        }
    }
}

/* �������� */
void GUI_DrawLines(int x, int y, GUI_POINT *Points, int cnt)
{
    int i;
    GUI_POINT *p1, *p2;

    for (i = 0; i < cnt - 1; ++i) {
        p1 = Points + i;
        p2 = Points + i + 1;
        GUI_DrawLine(p1->x + x, p1->y + x, p2->x + y, p2->y + y);
    }
}
