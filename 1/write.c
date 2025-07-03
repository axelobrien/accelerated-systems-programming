#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stddef.h>

ssize_t cs50_write(int fd, void const *buf, size_t count) {
	return syscall(SYS_write, fd, buf, count);
}
