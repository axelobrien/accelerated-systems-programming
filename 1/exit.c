#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

void cs50_exit(int status) {
	syscall(SYS_exit, status);
}
