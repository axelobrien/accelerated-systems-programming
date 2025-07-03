#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

int main(void) {
	void *buf[100];
	for (int i = 0; i < 100; i++) {
		buf[i] = malloc(i * 100);
	}

	for (int i = 0; i < 100; i++) {
		free(buf[i]);
	}
	return 0;
}
