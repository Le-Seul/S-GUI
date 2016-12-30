#ifndef __GUI_Malloc_H
#define __GUI_Malloc_H

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
typedef struct MEM_HEAP {
    size_t Size;
    struct MEM_HEAP *pNext; /* ָ����һ���ڴ�� */
    MEM_NODE MemPool;       /* �ڴ�ضѿռ�, ������Ϊ���ĳ�Ա */
} MEM_HEAP;

GUI_RESULT GUI_MemoryManagementInit(void);
void * GUI_Malloc(size_t Size);
void GUI_Free(void *Ptr);
size_t GUI_GetMemUsage(void);
size_t GUI_GetMemSize(void);

#endif /* __MEMALLOC_H */
