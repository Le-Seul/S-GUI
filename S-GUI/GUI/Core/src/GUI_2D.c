#include "GUI_2D.h"
#include "GUI.h"
#include "math.h"

u_16 GUI_RGB888To565(u_32 RGB)
{
    u_16 R,G,B;
    B = (RGB >> 19) & 0x1F;
    G = (RGB >> 10) & 0x3F;
    R = (RGB >>  3) & 0x1F;
    return (R << 11) | (G << 5) | B;   
}

u_32 GUI_RGB565To888(u_16 RGB)
{
    u_32 Color;
    Color  = ((u_32)RGB & 0x001F) << 19;  //B
    Color |= ((u_32)RGB & 0x07E0) << 5;   //G
    Color |= ((u_32)RGB & 0xF800) >> 8;   //R
    return Color;
}

/*
*Alpha���
*BkColor:������ɫ
*Color:Ŀ����ɫ
*Alpha:͸����(0~256)
*/
GUI_COLOR GUI_AlphaBlend(GUI_COLOR Color, GUI_COLOR BkColor, u_16 Alpha)
{
    /* Calc Color seperations for FgColor first */
    GUI_COLOR R = (BkColor & 0xFF)    * Alpha;
    GUI_COLOR G = (BkColor & 0xFF00)  * Alpha;
    GUI_COLOR B = (BkColor & 0xFF0000)* Alpha;
    /* Add Color seperations for BkColor */
    Alpha = 256 - Alpha;
    R += (Color & 0xFF)     * Alpha;
    G += (Color & 0xFF00)   * Alpha;
    B += (Color & 0xFF0000) * Alpha;
    R = (R >> 8);
    G = (G >> 8) & 0xFF00;
    B = (B >> 8) & 0xFF0000;
    return R | G | B;
}


//���ü���
void GUI_DrawTailorPoint(i_16 x,i_16 y,GUI_COLOR Color)
{
    GUI_RECT r1, r2;

    GUI_Val2Rect(&r1, x, y, 1, 1);
    GUI_DrawAreaInit();
    while (GUI_GetNextArea(&r2)) { /* �������е���ʾ���� */
        if (GUI_RectOverlay(&r2, &r2, &r1) == GUI_OK) {
            if (Color >> 24) {
                Color = GUI_AlphaBlend(Color, GUI_ReadPixel(x, y), Color >> 24);
            }
            GUI_DrawPixel(x, y, Color);
        }
    }
}

/* ��ȡ�ü��� */
GUI_COLOR GUI_ReadTailorPoint(i_16 x, i_16 y)
{
    GUI_RECT *r;
    
    r = GUI_GetNowArea();
    if (x >= r->x0 && x <= r->x1 &&
        y >= r->y0 && y <= r->y1) {
        return GUI_ReadPixel(x, y);
    } else {
        return 0x00000000;
    }
}

/* ����͸��Ч���Ĵ�ֱ�� */
void GUI_VertLineAlpha(i_16 x0, i_16 y0, u_16 len, GUI_COLOR Color)
{
    u_16 Alpha = Color >> 24;
    GUI_COLOR tColor;
    
    while (len--) {
        tColor = GUI_AlphaBlend(Color, GUI_ReadPixel(x0, y0 + len), Alpha);
        GUI_DrawPixel(x0, y0 + len, tColor);
    }    
}

/* ����ֱ�� */
void GUI_VertLine(i_16 x0, i_16 y0, u_16 len, GUI_COLOR Color)
{
    while (len--) {
        GUI_DrawPixel(x0, y0 + len, Color);
    }    
}

//����ֱ�ü���
//x0,y0:����
//len:�߳���
//color:��ɫ
void GUI_VertTailorLine(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    GUI_RECT r1, r2;

    if (GUI_Val2Rect(&r1, x0, y0, 1, len) == GUI_ERR) {
        return; /* ����Ϊ0 */
    }
    GUI_DrawAreaInit();
    while (GUI_GetNextArea(&r2)) { /* �������е���ʾ���� */
        if (GUI_RectOverlay(&r2, &r2, &r1) == GUI_OK) {
            x0 = r2.x0;
            y0 = r2.y0;
            len = r2.y1 - r2.y0 + 1;
            if (Color >> 24) {
                GUI_VertLineAlpha(x0, y0, len, Color);
            } else {
                GUI_VertLine(x0, y0, len, Color);
            }
        }
    }
}

/*  
*����͸��Ч����ˮƽ��
*/
void GUI_HoriLineAlpha(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    u_16 Alpha = Color >> 24;
    GUI_COLOR tColor;
    while (len--) {
        tColor = GUI_AlphaBlend(Color, GUI_ReadPixel(x0 + len, y0), Alpha);
        GUI_DrawPixel(x0 + len, y0, tColor);
    }
}

//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void GUI_HoriLine(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    while (len--) {
        GUI_DrawPixel(x0 + len, y0, Color);
    }    
}

//��ˮƽ�ü���
//x0,y0:����
//len:�߳���
//color:��ɫ
void GUI_HoriTailorLine(i_16 x0,i_16 y0,u_16 len,GUI_COLOR Color)
{
    GUI_RECT r1, r2;

    if (GUI_Val2Rect(&r1, x0, y0, len, 1) == GUI_ERR) {
        return; /* ����Ϊ0 */
    }
    GUI_DrawAreaInit();
    while (GUI_GetNextArea(&r2)) { /* �������е���ʾ���� */
        if (GUI_RectOverlay(&r2, &r2, &r1) == GUI_OK) {
            x0 = r2.x0;
            y0 = r2.y0;
            len = r2.x1 - r2.x0 + 1;
            if (Color >> 24) {
                GUI_HoriLineAlpha(x0, y0, len, Color);
            } else {
                GUI_HoriLine(x0, y0, len, Color);
            }
        }
    }
}

//�����ο�
void GUI_DrawRect(i_16 x0,i_16 y0,u_16 xSize,u_16 ySize,GUI_COLOR Color)
{
    GUI_HoriLine(x0, y0, xSize, Color);
    GUI_HoriLine(x0, y0 + ySize - 1, xSize, Color);
    GUI_VertLine(x0, y0, ySize, Color);
    GUI_VertLine(x0 + xSize - 1, y0, ySize, Color);
}

//���ü����ο�
void GUI_DrawTailorRect(i_16 x0,i_16 y0,u_16 xSize,u_16 ySize,GUI_COLOR Color)
{
    GUI_HoriTailorLine(x0 + 1, y0, xSize - 2, Color);
    GUI_HoriTailorLine(x0 + 1, y0 + ySize - 1, xSize - 2, Color);
    GUI_VertTailorLine(x0, y0, ySize, Color);
    GUI_VertTailorLine(x0 + xSize - 1, y0, ySize, Color);
}

void GUI_FillRectAlpha(i_16 x0, i_16 y0,u_16 xSize,u_16 ySize,GUI_COLOR Color)
{
    i_16 x, y;
    u_16 Alpha = Color >> 24;
    GUI_COLOR tColor; 
    
    xSize += x0;
    ySize += y0;
    for (y = y0; y < ySize; y++) {
        for (x = x0; x < xSize; x++) {
            tColor = GUI_AlphaBlend(Color, GUI_ReadPixel(x, y), Alpha);
            GUI_DrawPixel(x, y, tColor);
        }
    }
}

//������
void GUI_FillRect(i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, GUI_COLOR Color)
{                                     
    i_16 x, y;
    
    xSize += x0;
    ySize += y0;
    for (y = y0; y < ySize; y++) {
        for (x = x0; x < xSize; x++) {
            GUI_DrawPixel(x, y, Color);
        }
    }
}

//���ü�����
void GUI_FillTailorRect(i_16 x0, i_16 y0, u_16 xSize, u_16 ySize, GUI_COLOR Color)
{                                     
    GUI_RECT r1, r2;
    
    /* ����������ת��Ϊ�ṹ�� */
    if (GUI_Val2Rect(&r1, x0, y0, xSize, ySize) == GUI_ERR) {
        return;//�Ƿ�
    }
    GUI_DrawAreaInit();
    while (GUI_GetNextArea(&r2)) { /* �������е���ʾ���� */
        if (GUI_RectOverlay(&r2, &r2, &r1) == GUI_OK) {
            x0 = r2.x0;
            y0 = r2.y0;
            xSize = r2.x1 + 1 - x0;
            ySize = r2.y1 + 1 - y0;
            if (Color >> 24) {
                GUI_FillRectAlpha(x0, y0, xSize, ySize, Color);
            } else {
                GUI_FillRect(x0, y0, xSize, ySize, Color);
            }
        }
    }
}

//��Բ�Ǿ���/���Բ�Ǿ���
//x,y,width,height:Բ�Ǿ��ε�λ�úͳߴ�
//r:Բ�ǵİ뾶.
//mode:0,�����ο�;1,������.
//upcolor:�ϰ벿����ɫ
//downcolor:�°벿����ɫ
void GUI_DrawArcRect(u_16 x,u_16 y,u_16 width,u_16 height,u_8 r,u_8 mode,GUI_COLOR upcolor,GUI_COLOR downcolor)
{
    u_16 btnxh=0;
    if(height%2)btnxh=height+1;//��ż������
    else btnxh=height;
    if(mode)//���
    {
         GUI_FillRect(x+r,y,width-2*r,btnxh/2,upcolor);            //����
        GUI_FillRect(x+r,y+btnxh/2,width-2*r,btnxh/2,downcolor);    //����
        GUI_FillRect(x,y+r,r,btnxh/2-r,upcolor);                    //����
        GUI_FillRect(x,y+btnxh/2,r,btnxh/2-r,downcolor);            //����
        GUI_FillRect(x+width-r,y+r,r,btnxh/2-r,upcolor);            //����
        GUI_FillRect(x+width-r,y+btnxh/2,r,btnxh/2-r,downcolor);    //����
    }else
    {
        GUI_HoriLine (x+r,y,width-2*r,upcolor);                    //��           
        GUI_HoriLine (x+r,y+btnxh-1,width-2*r,downcolor);            //��           
        GUI_VertLine (x,y+r,btnxh/2-r,upcolor);                    //����           
        GUI_VertLine (x,y+btnxh/2,btnxh/2-r,downcolor);            //����           
        GUI_VertLine (x+width-1,y+r,btnxh/2-r,upcolor);            //����
        GUI_VertLine (x+width-1,y+btnxh/2,btnxh/2-r,downcolor);    //����                   
    }
    GUI_DrawArc(x,y,x+r,y+r,x+r,y+r,r,upcolor,mode);//����    
    GUI_DrawArc(x,y+btnxh-r,x+r,y+btnxh-1,x+r,y+btnxh-r-1,r,downcolor,mode);//����    
    GUI_DrawArc(x+width-r,y,x+width,y+r,x+width-r-1,y+r,r,upcolor,mode);     //����
    GUI_DrawArc(x+width-r,y+btnxh-r,x+width,y+btnxh-1,x+width-r-1,y+btnxh-r-1,r,downcolor,mode);//����
}

//�������ڻ���
//(sx,sy),(ex,ey):�趨����ʾ��Χ
//(x,y):������
//color:��ɫ
static void _draw_expoint(u_16 sx,u_16 sy,u_16 ex,u_16 ey,u_16 x,u_16 y,GUI_COLOR color)        
{
    if(x<=ex && x>=sx && y<=ey && y>=sy)
    {
        GUI_DrawPixel(x,y,color);        
    }
}

//��8��(Bresenham�㷨)          
//(sx,sy),(ex,ey):�趨����ʾ��Χ
//(rx,ry,a,b):����
//color:��ɫ
static void _draw_circle8(u_16 sx,u_16 sy,u_16 ex,u_16 ey,u_16 rx,u_16 ry,int a,int b,GUI_COLOR color)
{
    _draw_expoint(sx,sy,ex,ey,rx+a,ry-b,color);              
    _draw_expoint(sx,sy,ex,ey,rx+b,ry-a,color);                       
    _draw_expoint(sx,sy,ex,ey,rx+b,ry+a,color);                           
    _draw_expoint(sx,sy,ex,ey,rx+a,ry+b,color);             
    _draw_expoint(sx,sy,ex,ey,rx-a,ry+b,color);                  
    _draw_expoint(sx,sy,ex,ey,rx-b,ry+a,color);               
    _draw_expoint(sx,sy,ex,ey,rx-b,ry-a,color);                      
    _draw_expoint(sx,sy,ex,ey,rx-a,ry-b,color);                  
}        

//��ָ��λ�û�һ��ָ����С��Բ
//(rx,ry):Բ��
//(sx,sy),(ex,ey):�趨����ʾ��Χ
//r    :�뾶
//color:��ɫ
//mode :0,�����;1,���
void GUI_DrawArc(u_16 sx,u_16 sy,u_16 ex,u_16 ey,u_16 rx,u_16 ry,u_16 r,GUI_COLOR color,u_8 mode)
{
    int a,b,c;
    int di;      
    a=0;b=r;      
    di=3-(r<<1);    //�ж��¸���λ�õı�־
    while(a<=b)
    {
        if(mode)for(c=a;c<=b;c++)_draw_circle8(sx,sy,ex,ey,rx,ry,a,c,color);//��ʵ��Բ
         else _draw_circle8(sx,sy,ex,ey,rx,ry,a,b,color);                       //������Բ
        a++;
        //ʹ��Bresenham�㷨��Բ     
        if(di<0)di +=4*a+6;      
        else
        {
            di+=10+4*(a-b);   
            b--;
        }                               
    }
}
