#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    char* address = sbrk(100);
    // write to make sure memory has been allocated and is writable
    *address = 0x12;

    // uncomment to test invalid page address error
    // void *ptr = (void*)0x40000000L;
    // mprotect(ptr);

    mprotect(address);
    // Try uncommenting this line, and see if the program crashes
    // munprotect(address);
    // *address = 0x34;
    // printf("%d\n", (int)(*address));

    // munprotect(address);
    // *address = 0x56;
    // printf("%d\n", (int)(*address));

    exit(0);
}