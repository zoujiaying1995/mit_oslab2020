#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
#if defined(LAB_PGTBL) || defined(LAB_LOCK)
    statsinit();
#endif
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging //相当于是启用了分页机制。 这里将全局内核页表写进了页表基地址寄存器。此时还没有任何进程跑动。
    // procinit();      // process table //已被修改为只初始化进程的锁。原本是映射全局内核页表中的内核栈。64个进程逐一映射，从TRAMPOLINE往下映射，使用的是同一个全局内核页表。之后将全局页表基地址写到页表基地址寄存器。
    my_procinit();
    trapinit();      // trap vectors // 疑惑的是，为啥要重复写全局页表到寄存器？？这里还只是cpu0啊，重复写就是重复覆盖，但值都是一样的。
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode cache
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
#ifdef LAB_NET
    pci_init();
    sockinit();
#endif    
    userinit();      // first user process //这里调用了allocproc。为第一个进程创建了内核页表和用户页表，且将内核页表的基地址写到了寄存器上。只是创建进程，还没运行。
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }
  // printf("scheduler ..\n");
  scheduler();        // 所有cpu在完成上述操作后直接执行调度器。
}
