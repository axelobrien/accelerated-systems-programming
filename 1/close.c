#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

int cs50_close(int fd) {
	return syscall(SYS_close, fd);
}
