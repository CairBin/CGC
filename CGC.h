#ifndef CGC_H
#define CGC_H
#include <stddef.h>

#define GC_VM_HEAD CgcVm *vm = CGC_NewVm();
#define GC_VM_END vm = CGC_DeleteVm(vm);
#define GC_Deposit(ptr, type) CGC_Deposit(vm, &ptr, sizeof(type))
#define GC_NewPtr(type, name, value)               \
    type *name = (type *)CGC_Malloc(sizeof(type)); \
    *name = value;                                 \
    GC_Deposit(name, type)
#define GC_VM(text) GC_VM_HEAD text GC_VM_END

#define GC_InitVm() CgcVm *vm = CGC_NewVm()
#define GC_DelVm() vm = CGC_DeleteVm(vm)

#define GC_VmDeposit(vm, ptr, type) CGC_Deposit(vm, &ptr, sizeof(type))
#define GC_VmNewPtr(vm, type, name, value)         \
    type *name = (type *)CGC_Malloc(sizeof(type)); \
    *name = value;                                 \
    GC_VmDeposit(vm, name, type)

typedef struct _CGC_Node
{
    char **elem_ptr;
    char *elem;
    size_t varSize;
    struct _CGC_Node *prev;
    struct _CGC_Node *next;

} GcNode;

typedef struct _CGC_VM
{
    GcNode head;
    int GcNum;
} CgcVm;

CgcVm *CGC_NewVm();
void CGC_GarbageCollect(CgcVm *vm);
void CGC_Deposit(CgcVm *vm, void *ptr, size_t size);
CgcVm *CGC_DeleteVm(CgcVm *vm);
void *CGC_Malloc(size_t size);

#endif