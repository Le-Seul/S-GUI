#include "GUI_Memory.h"
#include "GUI.h"

/* GUI�ڴ��ͼ֧�� */

/* ����Ļ��������������ʾ����Ļ */
void GUI_DrawFromMem(u_16 x, u_16 y, u_16 len, u_16 wid)
{

}

void GUI_DrawBlock(u_16 x, u_16 y, u_16 len, u_16 wid, GUI_COLOR *data)
{

}

/* ��ȡ��Ļ������������ */
void GUI_ReadBack(u_16 x, u_16 y, u_16 len, u_16 wid, GUI_COLOR *pb)
{

}

/* �ָ���Ļ�����������ݣ�pbָ������Դ */
void GUI_RecoverBack(u_16 x, u_16 y, u_16 len, u_16 wid, GUI_COLOR *pb)
{

}

void GUI_MemToMem(u_16 x, u_16 y, u_16 len, u_16 wid, GUI_COLOR *pb)
{

}

/* GUI��ʾ */
void GUI_DrawDevice(i_16 x0, i_16 y0, i_16 x1, i_16 y1)
{
    GUI_DrawFromMem(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
}
