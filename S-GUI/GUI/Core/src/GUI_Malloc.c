#include "GUI_Malloc.h"
#include "GUI_Core.h"

/**
 @ �ڴ�س�ʼ��.
 @ Mem: �ڴ�ص�ַ(���������ַ��4�ֽڶ����, �Ա���������������).
 @ Size: �ڴ�ش�С(Byte).
 @ ����ֵ: GUI_OK: �ڴ�س�ʼ���ɹ�, GUI_ERR: �ڴ�س�ʼ��ʧ��.
 **/
GUI_RESULT GUI_HeapInit(void *Mem, size_t Size)
{
    MEM_HEAP *mPool;

    if (Size < sizeof(MEM_HEAP)) {
#if GUI_DEBUG_MODE
        GUI_DEBUG_OUT("GUI heap size is below the minimum.");
#endif
        return GUI_ERR; /* ����̫С */
    }
    mPool = Mem;
    mPool->Size = Size;
    mPool->Usage = 0;
    mPool->MemPool.Tag = 0;
    mPool->MemPool.pLast = NULL;
    mPool->MemPool.pNext = NULL;
    mPool->MemPool.Size = Size - (sizeof(MEM_HEAP) - sizeof(MEM_NODE));
    return GUI_OK;
}

/**
 @ �����ڴ�, �ڲ�����.
 @ pl: �ڴ�صĵ�һ���ѽڵ��ַ.
 @ Size: Ҫ������ֽ���.
 @ ����ֵ: ���뵽�Ķѽڵ��ַ, ����NULL��ʾ����ʧ��.
 **/
static MEM_NODE * __alloc(MEM_NODE *pl, size_t Size)
{
    MEM_NODE *pn;

    /* Ѱ�ҿռ��㹻�Ŀ��нڵ� */
    while (pl && (pl->Size < Size + sizeof(MEM_NODE) || pl->Tag)) {
        pl = pl->pNext;
    }
    if (pl) {
        /* �ռ乻������зָ� */
        if (pl->Size > Size + sizeof(MEM_NODE) * 2) {
            pl->Size -= Size + sizeof(MEM_NODE);
            pn = (MEM_NODE *)((char *)pl + pl->Size);
            pn->pNext = pl->pNext;
            pn->Size = Size + sizeof(MEM_NODE);
            pn->pLast = pl;
            pl->pNext = pn;
            pl = pn;
            pn = pl->pNext;
            if (pn) {
                pn->pLast = pl;
            }
        }
        pl->Tag = 1;
    }
    return pl;
}

/**
 @ �ͷ��ڴ�, �ڲ�����.
 @ pl: ��Ҫ�ͷŵĶѽڵ��ַ.
 **/
static void __free(MEM_NODE *pl)
{
    MEM_NODE *pn;

    pl->Tag = 0; /* ���Ϊ���� */
    /* ���ǰһ���ѽڵ��ǿ��еľͺϲ������ڵ� */
    pn = pl->pLast; /* ָ��ǰһ������ */
    if (pn && !pn->Tag) {
        pn->pNext = pl->pNext;
        pn->Size += pl->Size;
        pl = pn;
        pn = pl->pNext;
        if (pn) {
            pn->pLast = pl;
        }
    }
    /* �����һ���ѽڵ��ǿ��еľͺϲ������ڵ� */
    pn = pl->pNext; /* ָ����һ���� */
    if (pn && !pn->Tag) {
        pl->pNext = pn->pNext;
        pl->Size += pn->Size;
        pn = pl->pNext;
        if (pn) {
            pn->pLast = pl;
        }
    }
}

/**
 @ �����ڴ�, �ⲿ����.
 @ Size: Ҫ������ֽ���.
 @ Mem: �ڴ�ص�ַ.
 @ ����ֵ: ���뵽�ռ���׵�ַ.
 **/
void * GUI_Malloc(size_t Size, void * Mem)
{
    MEM_NODE *pn;
    if (Size) {
        if (Size & 0x03) { /* 4�ֽڶ��� */
            Size += 4 - Size & 0x03;
        }
        pn = &((MEM_HEAP *)Mem)->MemPool;
        pn = __alloc(pn, Size);
        if (pn) {
            ((MEM_HEAP *)Mem)->Usage += pn->Size;
            return (char *)pn + sizeof(MEM_NODE);
        }
    }
#if GUI_DEBUG_MODE
    GUI_DEBUG_OUT("GUI alloc failed (heap overflow).");
#endif
    return NULL;
}

/**
 @ �ͷ��ڴ�, �ⲿ����.
 @ pl: ��Ҫ�ͷŵĶѽڵ��ַ.
 @ Mem: �ڴ��ָ��.
 **/
void GUI_Free(void *Ptr, void *Mem)
{
    if (Ptr) {
        Ptr = (char *)Ptr - sizeof(MEM_NODE);
        ((MEM_HEAP *)Mem)->Usage -= ((MEM_NODE *)Ptr)->Size;
        __free(Ptr);
    }
}

/**
 @ ��ȡ�ڴ����ʹ���ֽ���.
 @ Mem: �ڴ�ص�ַ.
 @ ����ֵ: �ڴ����ʹ���ֽ���.
 **/
size_t MemGetUsageBytes(void *Mem)
{
    return ((MEM_HEAP*)Mem)->Usage;
}

/**
 @ ��ȡ�ڴ������(��λΪByte).
 @ Mem: �ڴ�ص�ַ.
 @ ����ֵ: �ڴ������(��λΪByte).
 **/
size_t MemGetSizeBytes(void *Mem)
{
    return ((MEM_HEAP*)Mem)->Size - (sizeof(MEM_HEAP) - sizeof(MEM_NODE));
}
