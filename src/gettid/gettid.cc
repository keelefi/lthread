#include "gettid.hh"

#include <unistd.h>
#include <sys/syscall.h>

pid_t gettid(void)
{
    return syscall(
            SYS_gettid);
}
