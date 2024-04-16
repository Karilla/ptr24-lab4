#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cobalt/stdio.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    int ret;

    mlockall(MCL_CURRENT | MCL_FUTURE);
	
    munlockall();

    return EXIT_SUCCESS;
}

