#include "GUI_Core.h"
#include "GUI.h"

GUI_WORK_SPACE *GUI_Data;
static u_32 __LockTaskId;
static u_16 __TaskLockCnt;

/* GUI��ʼ�� */
GUI_RESULT GUI_Init(void)
{    
    GUI_Data = GUI_fastmalloc(sizeof(GUI_WORK_SPACE));
    if (GUI_Data == NULL) {
        return GUI_ERR;
    }
    GUI_Phy_Init(&GUI_Data->phy_info);
    GUI_InitOS();
    GUI_RectListInit(GUI_RECT_HEAP_SIZE); /* ע���ڴ��Ƿ��㹻 */
    GUI_Data->MsgQueue = GUI_QueueInit(GUI_MSG_QUEUE_SIZE);
    if (GUI_Data->MsgQueue == NULL) {
        return GUI_ERR;
    }
    if (WM_Init() == GUI_ERR) {
        return GUI_ERR;
    }
#if GUI_USE_MEMORY
    GUI_Data->lcdbuf = GUI_malloc(sizeof(GUI_COLOR)
                                  * GUI_Data->phy_info.xSize
                                  * GUI_Data->phy_info.ySize);
#endif
    return GUI_OK;
}

/* ���ڴ���ж��GUI */
void GUI_Unload(void)
{
    WM_DeleteWindow(_hRootWin); /* ɾ�����д��� */
    GUI_QueueDelete(GUI_Data->MsgQueue); /* ɾ����Ϣ���� */
    GUI_fastfree(GUI_Data); /* ɾ��GUI�����ռ� */
    GUI_Data = NULL;
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
    GUI_TIME t = GUI_GetTime();

    WM_Exec();
    t = GUI_GetTime() - t; /* ����ִ��WM_Exec()��ʱ�� */
    if (tms > t) {
        _GUI_Delay_ms(tms - t); /* ��ʱ */
    }
}

/* -------------------- GUI��Ϣ���� -------------------- */
/* ��GUI��Ϣ�����ж�ȡһ����Ϣ */
GUI_RESULT GUI_GetMessage(GUI_MESSAGE *pMsg)
{
    GUI_RESULT res;

    GUI_LOCK();
    res = GUI_GetMessageQueue(GUI_Data->MsgQueue, pMsg);
    GUI_UNLOCK();
    return res;
}

/* ��GUI��Ϣ���з���һ����Ϣ */
GUI_RESULT GUI_PostMessage(GUI_MESSAGE *pMsg)
{
    GUI_RESULT res;

    GUI_LOCK();
    res = GUI_PostMessageQueue(GUI_Data->MsgQueue, pMsg);
    GUI_UNLOCK();
    return res;
}

/* -------------------- GUI������ -------------------- */

/* GUI���� */
void GUI_LOCK(void)
{
    /* ��û�����������Ѿ��������������� */
    if (!__TaskLockCnt || __LockTaskId != GUI_GetTaskId()) {
        GUI_TaskLock();
        __LockTaskId = GUI_GetTaskId();
    }
    ++__TaskLockCnt;
}

/* GUI���� */
void GUI_UNLOCK(void)
{
    if (--__TaskLockCnt == 0) {
        GUI_TaskUnlock();
    }
}

/* -------------------- GUI���λ��� -------------------- */
/* �������ڻ�������Ĳü��������� */
void GUI_SetNowRectList(RECT_LIST l, GUI_RECT *p)
{
    GUI_Data->NowRectList = l;
    GUI_Data->PaintArea = p;
}

/* �������ڻ�������Ĳü��������� */
RECT_LIST GUI_GetNowRectList(void)
{
    return GUI_Data->NowRectList;
}

static RECT_NODE *__NowRectNode = NULL;

/* ��ʼ���������� */
void GUI_DrawAreaInit(GUI_RECT *p)
{
    if (GUI_CheckRectIntersect(GUI_Data->PaintArea, p)) {
        __NowRectNode = GUI_Data->NowRectList;
    } else {
        __NowRectNode = NULL; /* ��ͼ�����뵱ǰ����Ч���������ཻ */
    }
}

/* ���ص�ǰ�Ĳü����� */
GUI_RECT *GUI_GetNowArea(void)
{
    if (__NowRectNode) {
        return &__NowRectNode->Rect;
    }
    return NULL;
}

/* ��ȡ��һ���ü�����
 * pRect:���ڱ��淵�صĲü�����
 * ����ֵ:0����ǰ��������Ĳü������Ѿ���ȡ�꣬����������GUI_GetNextArea
 *          ����0ʱӦ�ý�����ǰͼ���ڲü�����������Ļ���ѭ����
 *       1����ǰ��������Ļ���û�л��ƵĲü����Σ�����������GUI_GetNextArea
 *          ����0ʱӦ�ü�����ǰͼ���ڲü�����������Ļ���ѭ����
 **/
u_8 GUI_GetNextArea(GUI_RECT *pRect)
{
    if (__NowRectNode == NULL) {
        return 0;
    }
    *pRect = __NowRectNode->Rect;
    __NowRectNode = __NowRectNode->pNext;
    return 1;
}

/* GUI������� */
#if GUI_DEBUG_MODE
void GUI_DebugOut(const char *s)
{
    _GUI_DebugOut(s);
}
#endif
