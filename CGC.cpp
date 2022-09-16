#include "CGC.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_GC_NUM 15

CgcVm *CGC_NewVm()
{
    CgcVm *vm = (CgcVm *)malloc(sizeof(CgcVm));
    vm->head.elem = NULL;
    vm->head.elem_ptr = NULL;
    vm->head.next = NULL;
    vm->head.prev = NULL;
    vm->GcNum = 0;

    return vm;
}

void Assert(int condition, const char *msg)
{
    if (condition)
    {
        printf("%s\n", msg);
        exit(1);
    }
}

void Push(CgcVm *vm, GcNode *node)
{
    node->next = vm->head.next;
    node->prev = &(vm->head);
    if (vm->head.next)
        vm->head.next->prev = node;
    vm->head.next = node;
}

void Free(CgcVm *vm, GcNode *node)
{
    Assert(node == &(vm->head), "Error: Cannot free head node");
    if (node->elem != NULL)
        free(node->elem);
    free(node);
}

void Sweep(CgcVm *vm)
{
    GcNode *p = vm->head.next;
    while (p)
    {
        GcNode *tmp = p;
        p = p->next;
        if (*(tmp->elem_ptr) == NULL)
        {
            if (tmp->prev)
                tmp->prev->next = tmp->next;
            if (tmp->next)
                tmp->next->prev = tmp->prev;

            Free(vm, tmp);
        }
    }

    vm->GcNum = 0;
}

void CGC_GarbageCollect(CgcVm *vm)
{
    Sweep(vm);
}

GcNode *NewGcNode(CgcVm *vm, size_t size)
{
    GcNode *node = (GcNode *)malloc(sizeof(GcNode));
    node->varSize = size;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

void CGC_Deposit(CgcVm *vm, void *ptr, size_t size)
{
    GcNode *node = NewGcNode(vm, size);
    node->elem = *(char **)ptr;
    node->elem_ptr = (char **)ptr;
    Push(vm, node);
}

void Pop(CgcVm *vm)
{
    GcNode *p = vm->head.next;
    if (p != NULL)
    {
        if (p->next)
            p->next->prev = &(vm->head);
        vm->head.next = p->next;
        Free(vm, p);
        p = NULL;
    }
}

CgcVm *CGC_DeleteVm(CgcVm *vm)
{
    while (vm->head.next != NULL)
        Pop(vm);
    free(vm);
    vm = NULL;

    return NULL;
}

void *CGC_Malloc(size_t size)
{
    void *i = malloc(size);
    return i;
}