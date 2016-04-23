/*****************************************************************************
 * ��̬�ڴ�������(ʹ����ʽ����ʵ��).
 * �ļ���: memalloc.h
 * ��  ��: V1.2
 * ��  ��: ������
 * ��  ��: 2016/4/23
 *****************************************************************************/
#ifndef __MEMALLOC_H
#define __MEMALLOC_H

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
} MEM_POOL;

int Mem_PoolInit(void *Mem, size_t Size);
void * memalloc(size_t Size, void *Mem);
void memfree(void *Ptr, void *Mem);
size_t MemGetUsageBytes(void *Mem);
size_t MemGetSizeBytes(void *Mem);

#endif /* __MEMALLOC_H */
