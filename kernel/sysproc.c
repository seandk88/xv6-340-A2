#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

/* uoa */
uint64
sys_getwaittime(void)
{
  int pid;
  uint64 runnable_addr, sleeping_addr;
  uint r_ticks = 0, s_ticks = 0;
  struct proc *myp = myproc(); /* process making the syscall */

  argint(0, &pid);
  argaddr(1, &runnable_addr);
  argaddr(2, &sleeping_addr);

  if (kgetwaitstats(pid, &r_ticks, &s_ticks) < 0) {
    return -1;
  }

  if (copyout(myp->pagetable, runnable_addr, (char *)&r_ticks, sizeof(r_ticks)) < 0) {
    return -1;
  }
  if (copyout(myp->pagetable, sleeping_addr, (char *)&s_ticks, sizeof(s_ticks)) < 0) {
    return -1;
  }

  return 0;
}

uint64
sys_waitx(void)
{
  uint64 addr, rtime_addr, stime_addr;

  argaddr(0, &addr);
  argaddr(1, &rtime_addr);
  argaddr(2, &stime_addr);

  return kwaitx(addr, rtime_addr, stime_addr);
}

uint64
sys_getusedmem(void)
{
  pagetable_t kpt = get_kernel_pagetable();
  return walk_used(kpt);
}

uint64
sys_mprotect(void)
{
  uint64 addr;
  argaddr(0, &addr);

  struct proc *p = myproc();

  pte_t *pte = walk(p->pagetable, addr ,0);
  if (pte == 0){
    printf("Not valid page address\n");
    return 0;
  }
  *pte &= ~PTE_W;
  sfence_vma();

  return 0;
}

uint64
sys_munprotect(void)
{
  uint64 addr;
  argaddr(0, &addr);

  struct proc *p = myproc();

  pte_t *pte = walk(p->pagetable, addr ,0);
  if (pte == 0){
    printf("Not valid page address\n");
    return 0;
  }
  *pte |= PTE_W;
  sfence_vma();

  return 0;
}
