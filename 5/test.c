#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

int main(void) {
	char *buf = malloc(400);
	memset(buf, 'a', 399);
	buf[399] = '\n';

	char *buf2 = malloc(0x5000);
	buf2[0] = 'b';
	buf2[1] = '\n';

	char *buf3 = malloc(0x5000);
	buf3[0] = 'c';
	buf3[1] = '\n';

	char *buf4 = malloc(400);
	memset(buf4, 'd', 399);
	buf4[399] = '\n';

	free(buf);
	free(buf2);
	free(buf3);
	free(buf4);

	write(STDOUT_FILENO, buf, 400);
	write(STDOUT_FILENO, buf2, 2);
	write(STDOUT_FILENO, buf4, 400);
	return 0;
}
