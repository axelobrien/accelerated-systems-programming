#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

int cs50_getchar(void) {
	char *buf = {0};
	int status = syscall(SYS_read, STDIN_FILENO, buf, 1);
	if (status != 0) {
		return -1;
	}
	return (int)*buf;
}
