#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

    char* ch = 0x0;
    printf("%c", *ch);

    exit(0);
}