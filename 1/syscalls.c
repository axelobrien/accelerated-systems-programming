#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "libcs50.h"

int main(void) {
	
	printf("AAA");
	int fd = cs50_open("/home/axel/assignments/1/file.txt", 1, 0);

	if (fd < 0) {
		printf("open failed\n");
		cs50_exit(fd);
	}

	char s[] = "Saluton, mondo!";
	ssize_t write_status = cs50_write(fd, s, (sizeof s) / (sizeof *s));
	printf("AAA");

	if (write_status < 0) {
		printf("%d, %s, %ld", fd, s, (sizeof s) / (sizeof *s));
		printf("write failed\n");
		cs50_exit(write_status);	
	}

	char buf[1024];
	printf("AAA");
	ssize_t read_status = cs50_read(fd, buf, 1023);
	if (read_status < 0) {
		printf("read failed");
		cs50_exit(fd);
	}

	if (read_status < 1023) {
                if (buf[read_status - 1] != '\0') {
                        buf[read_status] = '\0';
                }
        }

	cs50_puts(buf);
	printf("%s\n", buf);
	return 0;
}
