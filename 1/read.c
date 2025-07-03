#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stddef.h>

ssize_t cs50_read(int fd, void *buf, size_t count) {
	return syscall(SYS_read, fd, buf, count);
}
