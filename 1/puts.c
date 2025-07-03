#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "libcs50.h"

int cs50_puts(char const *s) {
	
	int string_status = syscall(SYS_write, STDOUT_FILENO, s, cs50_strlen(s));
	if (string_status < 0) {
		return -1;
	}
	char newline[] = "\n";
	int newline_status = syscall(SYS_write, STDOUT_FILENO, newline, (sizeof newline) / (sizeof *newline));
	
	if (newline_status < 0) {
		return -1;
	}

	return string_status + 1;
}
