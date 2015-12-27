#include "GUI_Core.h"
#include "GUI.h"

GUI_WORK_SPACE *GUI_Data;
GUI_RECT __Now_DrawRect;  //GUI��ǰ���Ի��Ƶ�����

void GUI_Init(void)
{    
    GUI_Data = GUI_fastmalloc(sizeof(GUI_WORK_SPACE));
    if (GUI_Data == NULL) {
        return;
    }
    GUI_Phy_Init(&GUI_Data->phy_info);
    GUI_InitOS();
    GUI_RectListInit(50); /* ע���ڴ��Ƿ��㹻 */
    GUI_KeyBufferInit(); /* ������������ʼ�� */
    WM_Init();
#if GUI_USE_MEMORY
    GUI_Data->lcdbuf = GUI_malloc(sizeof(GUI_COLOR)
                                 * GUI_Data->phy_info.xSize
                                 * GUI_Data->phy_info.ySize);
#endif
    GUI_Data->guitime=0;
}

/* ���ڴ���ж��GUI */
void GUI_Unload(void)
{
    WM_DeleteWindow(_hRootWin); /* ɾ�����д��� */
    GUI_KeyBufferDelete();      /* ɾ���������� */
    GUI_fastfree(GUI_Data);     /* ɾ��GUI�����ռ� */
}

/* ��ȡ��Ļ�ߴ� */
void GUI_ScreenSize(u_16 *xSize, u_16 *ySize)
{
    *xSize = GUI_Data->phy_info.xSize;
    *ySize = GUI_Data->phy_info.ySize;
}

/* ��ȡ��Ļ��� */
u_16 GUI_GetScreenWidth(void)
{
    return GUI_Data->phy_info.xSize;
}

/* ��ȡ��Ļ�߶� */
u_16 GUI_GetScreenHeight(void)
{
    return GUI_Data->phy_info.ySize;
}

/* GUI��ʱ������ */
void GUI_Delay(GUI_TIME tms)
{
    GUI_KeyProcess();
    WM_Exec();  //���ڹ�����
    _GUI_Delay_ms(tms); //��ʱ
}

/*
 * �¼����г�ʼ��
 * capacity:��������
 * ItemSize:ÿ���ṹ��Ԫ���ֽ���
 */
GUI_QUEUE* GUI_EventQueueInit(u_16 capacity, u_16 ItemSize)
{
    GUI_QUEUE *pQue = GUI_fastmalloc(sizeof(GUI_QUEUE));

    pQue->pArray = GUI_fastmalloc((u_32)capacity * ItemSize);
    pQue->capacity = capacity;
    pQue->ItemSize = ItemSize;
    pQue->size = 0;
    pQue->front = 1;
    pQue->rear = 0;
    return pQue;
}

/* ɾ������ */
void GUI_EventQueueDelete(GUI_QUEUE *pQue)
{
    if (pQue) {
        GUI_fastfree(pQue->pArray);
        GUI_fastfree(pQue);
    }
}

/*
*���¼��������ȡһ���¼�
*/
u_8 GUI_GetEvent(GUI_QUEUE *pQue, GUI_EVENT event)
{
    u_16 i = pQue->ItemSize;
    u_8 *p1, *p2;
    
    if (!pQue->size) {   //����Ϊ��
        return GUI_ERR;
    }
    pQue->size--;
    p1 = (u_8*)event;
    p2 = (u_8*)pQue->pArray + (u_32)pQue->front * i;
    while(i--) {
        *p1++ = *p2++;
    }
    if (++pQue->front == pQue->capacity) {  //��ͷ�ƻص���ͷ
        pQue->front = 0;
    }
    return GUI_OK;
}

/*
*���¼����з���һ���¼�
*pQue:�¼�����ָ��
*event:��Ҫ���͵��¼�
*/
u_8 GUI_SendEvent(GUI_QUEUE *pQue, GUI_EVENT event)
{
    u_16 i = pQue->ItemSize;
    u_8 *p1, *p2;
    
    if (pQue->size == pQue->capacity - 1) { //��������
        return GUI_ERR;
    }
    pQue->size++;
    if (++pQue->rear == pQue->capacity) {  //��β�ƻص���ͷ
        pQue->rear = 0;
    }
    p1 = (u_8*)pQue->pArray + (u_32)pQue->rear * i;
    p2 = (u_8*)event;
    while(i--) {
        *p1++ = *p2++;
    }
    return GUI_OK;
}

/*
*����¼�����
*/
void GUI_CleanQueue(GUI_QUEUE *pQue)
{
    pQue->size = 0;
    pQue->front = 1;
    pQue->rear = 0;
}

/*
*����¼������Ƿ�Ϊ��
*/
u_8 GUI_QueueIsEmpty(GUI_QUEUE *pQue)
{
    if (!pQue->size) {   //����Ϊ��
        return 1;
    }
    return 0;
}

/*  */
void GUI_SetNowRectList(RECT_LIST l)
{
    GUI_Data->NowRectList = l;
}

RECT_LIST GUI_GetNowRectList(void)
{
    return GUI_Data->NowRectList;
}

static RECT_NODE *__NowRectNode = NULL;

void GUI_DrawAreaInit(void)
{
    __NowRectNode = GUI_Data->NowRectList;
}

GUI_RECT *GUI_GetNowArea(void)
{
    return &__NowRectNode->Rect;
}

u_8 GUI_GetNextArea(GUI_RECT *pRect)
{    
    if (__NowRectNode == NULL) {
        return 0;
    }
    *pRect = __NowRectNode->Rect;
    __NowRectNode = __NowRectNode->pNext;
    return 1;
}
