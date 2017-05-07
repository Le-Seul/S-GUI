#include "GUI_Core.h"
#include "GUI.h"

static void * __LockPtr;
static u_32 __LockTaskId;
static int __TaskLockCnt;

/* GUI��ʼ�� */
GUI_RESULT GUI_Init(void)
{
    /* �ڴ�����ʼ�� */
    if (GUI_MemoryManagementInit() == GUI_ERR) {
        return GUI_ERR;
    }
    /* ��ʼ������ϵͳ��ش��� */
    __LockPtr = GUI_TaskCreateLock();
    /* ��ʼ��ͼ��Ӳ�� */
    GUI_DeviceInit();
    /* ��ʼ�����ڼ�����ü�˽�ж� */
    if (GUI_RectListInit() == GUI_ERR) {
        return GUI_ERR;
    }
    /* ��ʼ����Ϣ���� */
    if (GUI_MessageQueueInit() == GUI_ERR) {
        return GUI_ERR;
    }
    /* ��ʼ�����ڹ����� */
    if (WM_Init() == GUI_ERR) {
        return GUI_ERR;
    }
    GUI_SetFont(&GUI_DEF_FONT);
    GUI_SetPenSize(1); /* Ĭ���߿�Ϊ1 */
    return GUI_OK;
}

/* ���ڴ���ж��GUI */
void GUI_Unload(void)
{
    GUI_LOCK();
    WM_DeleteWindow(gui_rootwin); /* ɾ�����д��� */
    GUI_MessageQueueDelete();   /* ɾ����Ϣ���� */
    GUI_DeleteDeviceList();     /* ɾ���豸�б� */
    GUI_UNLOCK();
}

/* GUI��ʱ������ */
void GUI_Delay(GUI_TIME tms)
{
    GUI_TIME t = GUI_GetTime();
    static GUI_TIME t_last;

    WM_Exec();
    if (t > t_last + GUI_AUTO_MANAGE_TIME) {
        t_last = t;
        GUI_FreeIdleRectList();
    }
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
    if (!__TaskLockCnt || __LockTaskId != GUI_TaskGetId()) {
        GUI_TaskLock(__LockPtr);
        __LockTaskId = GUI_TaskGetId();
    }
    ++__TaskLockCnt;
}

/* GUI���� */
void GUI_UNLOCK(void)
{
    if (--__TaskLockCnt == 0) {
        GUI_TaskUnlock(__LockPtr);
    }
}

/* GUI������� */
#if GUI_DEBUG_MODE
void GUI_DebugOut(const char *s)
{
    _GUI_DebugOut(s);
}
#endif
