#include "GUI_Core.h"
#include "GUI.h"

void *GUI_Heap[2];           /* �ڴ��ָ�� */
GUI_WORK_SPACE *GUI_Data;    /* GUI�������� */
GUI_CONTEXT GUI_Context;     /* GUI������ */
static u_32 __LockTaskId;
static u_16 __TaskLockCnt;

/* GUI��ʼ�� */
GUI_RESULT GUI_Init(void)
{
    int res;
    u_32 HeapSize;

    GUI_Heap[GUI_HEAP_FAST] = _GUI_GetHeapBuffer(GUI_HEAP_FAST, &HeapSize);
    res = GUI_HeapInit(GUI_Heap[GUI_HEAP_FAST], HeapSize);
    GUI_Heap[GUI_HEAP_HCAP] = _GUI_GetHeapBuffer(GUI_HEAP_HCAP, &HeapSize);
    res |= GUI_HeapInit(GUI_Heap[GUI_HEAP_HCAP], HeapSize);
    if (res) {
        return GUI_ERR;
    }
    GUI_Data = GUI_fastmalloc(sizeof(GUI_WORK_SPACE));
    if (GUI_Data == NULL) {
        return GUI_ERR;
    }
    GUI_Phy_Init(&GUI_Data->phy_info);
    GUI_InitOS();
    GUI_RectListInit(GUI_RECT_HEAP_SIZE); /* ע���ڴ��Ƿ��㹻 */
    if (GUI_MessageQueueInit() == GUI_ERR) {
        return GUI_ERR;
    }
    if (WM_Init() == GUI_ERR) {
        return GUI_ERR;
    }
    return GUI_OK;
}

/* ���ڴ���ж��GUI */
void GUI_Unload(void)
{
    WM_DeleteWindow(_hRootWin); /* ɾ�����д��� */
    GUI_MessageQueueDelete();   /* ɾ����Ϣ���� */
    GUI_fastfree(GUI_Data);     /* ɾ��GUI�����ռ� */
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
void GUI_SetNowRectList(GUI_AREA l, GUI_RECT *p)
{
    GUI_Context.Area = l;
    GUI_Context.ClipRect = p;
}

/* �������ڻ�������Ĳü��������� */
GUI_AREA GUI_GetNowRectList(void)
{
    return GUI_Context.Area;
}

/* ��ʼ���������� */
void GUI_DrawAreaInit(GUI_RECT *p)
{
    if (GUI_CheckRectIntersect(GUI_Context.ClipRect, p)) {
        GUI_Context.pAreaNode = GUI_Context.Area;
    } else {
        GUI_Context.pAreaNode = NULL; /* ��ͼ�����뵱ǰ����Ч���������ཻ */
    }
}

/* ���ص�ǰ�Ĳü����� */
GUI_RECT *GUI_GetNowArea(void)
{
    if (GUI_Context.pAreaNode) {
        return &GUI_Context.pAreaNode->Rect;
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
    if (GUI_Context.pAreaNode == NULL) {
        return 0;
    }
    *pRect = GUI_Context.pAreaNode->Rect;
    GUI_Context.pAreaNode = GUI_Context.pAreaNode->pNext;
    return 1;
}

u_8 GUI_GetNextAreaP(GUI_RECT ** p)
{
    if (GUI_Context.pAreaNode == NULL) {
        return 0;
    }
    *p = &GUI_Context.pAreaNode->Rect;
    GUI_Context.pAreaNode = GUI_Context.pAreaNode->pNext;
    return 1;
}

/* GUI������� */
#if GUI_DEBUG_MODE
void GUI_DebugOut(const char *s)
{
    _GUI_DebugOut(s);
}
#endif
