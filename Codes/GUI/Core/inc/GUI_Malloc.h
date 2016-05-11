#ifndef __GUI_MALLOC_H
#define __GUI_MALLOC_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"
#include <stdio.h>

/* �ڴ�ضѽڵ� */
typedef struct MEM_NODE {
    size_t Tag;             /* ��ʹ�õĶѽڵ��� */
    size_t Size;            /* �ѽڵ��ֽ��� */
    struct MEM_NODE *pLast; /* ָ����һ���ѽڵ� */
    struct MEM_NODE *pNext; /* ָ����һ���ѽڵ� */
} MEM_NODE;

/* �ڴ�ض��� */
typedef struct {
    size_t Size;            /* �ڴ�������ֽ��� */
    size_t Usage;           /* �ڴ��ʹ�����ֽ��� */
    MEM_NODE MemPool;       /* �ڴ�ضѿռ� */
} MEM_HEAP;

GUI_RESULT GUI_HeapInit(void *Mem, size_t Size);
void * GUI_Malloc(size_t Size, void *Mem);
void GUI_Free(void *Ptr, void *Mem);
size_t MemGetUsageBytes(void *Mem);
size_t MemGetSizeBytes(void *Mem);

#endif /* __MEMALLOC_H */
