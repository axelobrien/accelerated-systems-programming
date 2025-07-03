#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stddef.h>

int cs50_open(char const *pathname, int flags, mode_t mode) {
	return syscall(SYS_open, pathname, flags, mode);
}
