#include "GUI_Malloc.h"
#include "GUI_Core.h"

static MEM_HEAP *__HeapList;   /* ������ */
static size_t __SizeBytes;     /* ������ */
static size_t __UsageBytes;    /* ��ʹ���� */

static _ListInsert(MEM_HEAP *heap)
{
    MEM_HEAP *p = __HeapList;

    if (p) {
        while (p->pNext) {
            p = p->pNext;
        }
        p->pNext = heap;
    } else {
        __HeapList = heap;
    }
}

/**
 @ �ڴ�س�ʼ��.
 @ Mem: �ڴ�ص�ַ(���������ַ��4�ֽڶ����, �Ա���������������).
 @ Size: �ڴ�ش�С(Byte).
 @ ����ֵ: GUI_OK: �ڴ�س�ʼ���ɹ�, GUI_ERR: �ڴ�س�ʼ��ʧ��.
 **/
GUI_RESULT GUI_HeapInit(void *Mem, size_t Size)
{
    MEM_HEAP *mPool;

    if (!Mem || Size < sizeof(MEM_HEAP)) {
        return GUI_ERR;
    }
    mPool = Mem;
    mPool->Size = Size;
    mPool->pNext = NULL;
    mPool->MemPool.Tag = 0;
    mPool->MemPool.pLast = NULL;
    mPool->MemPool.pNext = NULL;
    mPool->MemPool.Size = Size - (sizeof(MEM_HEAP) - sizeof(MEM_NODE));
    _ListInsert(mPool); /* ��ӵ����� */
    __SizeBytes += Size - (sizeof(MEM_HEAP) - sizeof(MEM_NODE));
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
void * GUI_Malloc(size_t Size)
{
    MEM_NODE *pn = NULL;
    MEM_HEAP *ph;

    if (Size) {
        if (Size & 0x03) { /* 4�ֽڶ��� */
            Size += 4 - Size & 0x03;
        }
        /* �������еĶ� */
        for (ph = __HeapList; ph && !pn; ph = ph->pNext) {
            pn = __alloc(&ph->MemPool, Size);
        }
        if (pn) {
            __UsageBytes += pn->Size;
            return (void *)((u_32)pn + sizeof(MEM_NODE));
        }
#if GUI_DEBUG_MODE
        GUI_DEBUG_OUT("GUI alloc failed (heap overflow).");
#endif
    }
    return NULL;
}

/**
 @ �ͷ��ڴ�, �ⲿ����.
 @ pl: ��Ҫ�ͷŵĶѽڵ��ַ.
 @ Mem: �ڴ��ָ��.
 **/
void GUI_Free(void *Ptr)
{
    size_t addr1 = (size_t)Ptr - sizeof(MEM_NODE), addr2;
    MEM_HEAP *ph;

    if (Ptr) {
        for (ph = __HeapList; ph; ph = ph->pNext) {
            addr2 = (size_t)ph;
            if (addr1 > addr2 && addr1 < addr2 + ph->Size) {
                __UsageBytes -= ((MEM_NODE *)addr1)->Size;
                __free((MEM_NODE *)addr1);
                return;
            }
        }
#if GUI_DEBUG_MODE
        GUI_DEBUG_OUT("GUI free filed (pointer is invalid).");
#endif
    }
}

/**
 @ ��ȡ�ڴ����ʹ���ֽ���.
 @ Mem: �ڴ�ص�ַ.
 @ ����ֵ: �ڴ����ʹ���ֽ���.
 **/
size_t GUI_GetMemUsage(void)
{
    return __UsageBytes;
}

/**
 @ ��ȡ�ڴ������(��λΪByte).
 @ Mem: �ڴ�ص�ַ.
 @ ����ֵ: �ڴ������(��λΪByte).
 **/
size_t GUI_GetMemSize(void)
{
    return __SizeBytes;
}
