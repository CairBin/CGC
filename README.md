# CGC

## 项目描述

它的实现原理是在内存堆区维护了一个双向链表，通过向外部提供的接口，对指向动态内存的指针进行了托管。
当托管的指针被置为`NULL`时，双向链表中的结点将会被认为是无效区块。并且每当创建结点的个数达到`MAX_GC_NUM`的时候，进行内存释放(包括数据内存和链表结点内存)。

注意，这里所说的`MAX_GC_NUM`默认值为15。
## 使用说明
根据需要利用CMake生成链接库
使用该链接库并在你的程序中包含头文件:
```c
#include "GCG.h"
```

本项目提供了几个对外的函数:
### 生成虚拟机

```c
CgcVm *CGC_NewVm();
```
为防止内存泄露，您在使用该函数的时候必须按照如下形式调用
```c
CgcVm* vm = CGC_NewVm();
```
此外为方便操作，在`CGC.h`文件中定义了一些宏可以代替上述语句，在实际操作的时候，您只需要选择其一即可

* 宏GC_VM_HEAD
```c
#define GC_VM_HEAD CgcVm *vm = CGC_NewVm();
```

* 宏GC_InitVm()
```c
#define GC_InitVm() CgcVm *vm = CGC_NewVm()
```
很显然，根据这些宏的定义，您会发现它们在实际使用行尾除有一个分号的区别
相对上述函数，不好的是，这些宏仅会生成一个名称为`vm`的变量，这意味着您无法控制变量名称

### GC
该程序对外提供一个GC函数，该函数会被自动调用，但您同样可以手动调用，它需要传入一个虚拟机的指针
其函数声明如下
```c
void CGC_GarbageCollect(CgcVm *vm);
```


### 托管
托管是本程序的核心所在，它能够托管任意类型的指针，并检测它们是否为`NULL`
参数：vm为虚拟机指针，ptr指针的指针(此处必须对指针进行**取地址运算**，而不是解引)，size为数据变量的类型的大小
函数声明如下
```c
void CGC_Deposit(CgcVm *vm, void *ptr, size_t size);
```
其使用方法如下
```c
//假设p为托管的指针
int *p = (int*)malloc(sizeof(int));
*p = 3;
CGC_Deposit(vm,&p,sizeof(int));
```

同样，在本程序中定义了许多与托管相关的宏，使用这些宏无需对指针进行取地址运算
* 宏GC_Deposit(ptr, type)
```c
#define GC_Deposit(ptr, type) CGC_Deposit(vm, &ptr, sizeof(type))
```
* 宏GC_VmNewPtr(vm, type, name, value)
```c
#define GC_VmDeposit(vm, ptr, type) CGC_Deposit(vm, &ptr, sizeof(type))
```
对于宏`GC_Deposit(ptr, type)`，它默认虚拟机指针名称为`vm`
然而对于宏`GC_VmDeposit`，其虚拟机指针名称可以不是`vm`，但必须与您按照上文中生成虚拟机的方式命名一致。

### 内存动态分配
```c
void *CGC_Malloc(size_t size);
```
使用方式同标准库中的`malloc()`函数，不会对生成的指针进行托管

为方便操纵，在`CGC.h`中同样定义了相关的宏，并且这些宏继承了分配内存和托管的功能，并且对指向内存存放的值进行初始化
* 宏GC_NewPtr(type, name, value)
```c
#define GC_NewPtr(type, name, value)               \
    type *name = (type *)CGC_Malloc(sizeof(type)); \
    *name = value;                                 \
    GC_Deposit(name, type)
```
* 宏GC_VmNewPtr(vm, type, name, value)
```c
#define GC_VmNewPtr(vm, type, name, value)         \
    type *name = (type *)CGC_Malloc(sizeof(type)); \
    *name = value;                                 \
    GC_VmDeposit(vm, name, type)
```
二者的区别仍然是虚拟机指针的名称，前者默认名称为`vm`，后者需程序员手动传入
另外，需注意宏的使用方式
```c
//错误
int *a = GC_VmNewPtr(vm,int,a,3);

//正确，生成名为a的指针
GC_VmNewPtr(vm,int,a,3);
```

### 销毁虚拟机
```c
CgcVm *CGC_DeleteVm(CgcVm *vm);
```
其参数是一个要被销毁的虚拟机的指针，使用方式如下
```c
vm = CGC_DeleteVm(vm);
```

对应的有这些宏
* 宏GC_VM_END
```c
#define GC_VM_END vm = CGC_DeleteVm(vm);
```

* 宏GC_DelVm()
```c
#define GC_DelVm() vm = CGC_DeleteVm(vm)
```

它们分别按顺序对应**生成虚拟机**部分的两个宏，同样它们所销毁的虚拟机对应的指针名称都为`vm`
如果您创建的虚拟机的指针名称不是`vm`，则需要使用`CGC_DeleteVm()`函数进行操作。

## 代码风格
本项目提供四种代码风格(以下仅列出`main()`函数中的内容)
您在使用的过程中不应该将它们混淆，即使能够编译且运行成功

* 第一种(不推荐)
```c
int main()
{
	//两个宏后面无分号，并且虚拟机指针名称为vm
	GC_VM_HEAD
		
	/*此处写你的代码*/
		
	GC_VM_END
	return 0;
}

```

* 第二种
```c
int main()
{
	//两个宏后面有分号，并且虚拟机指针名称为vm
	GC_InitVm();
	
	/*此处写你的代码*/
	
	GC_DelVm();
	return 0;
}
```

* 第三种(推荐)
```c
int main()
{
	//正常创建对象的形式，可以根据需要修改指针名称，如vm_name
	CgcVm* vm_name = CGC_NewVm();
	
	/*此处写你的代码*/
	
	vm_name = CGC_DeleteVm();
	return 0;
}
```

* 第四种(GC_VM宏，极不推荐)
```c
int main()
{
	//采用宏文本替换的形式，不利于排查程序错误，极其不推荐
	GC_VM(
	/*此处写你的代码*/
	)
	return 0;
}
```