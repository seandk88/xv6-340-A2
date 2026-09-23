#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  uint64 used = getusedmem();
  // pages to bytes
  used = used * PGSIZE;

  printf("Used memory: [%ld] bytes\n", used);

  exit(0);
}
