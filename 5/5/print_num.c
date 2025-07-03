#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>

static void printm(char *s) {
	write(STDOUT_FILENO, s, strlen(s));
}

static void print_num(intptr_t n, int base) {
	if (n == 0) {
		printm("0");
		return;
	}

	char reverse_buf[101];
	const int original_n = n;
	const int original_base = base;

	if (base == 0) {
		base = 16;
	}

	if (n < 0 && base != 16) {
		n *= -1;
	}


	for (int i = 0; i < 100; i++) {
		char digit = n % base;
		digit += '0';

		if (digit > '9') {
			digit += 'A' - '9' - 1;
		}

		reverse_buf[i] = digit;
		n /= base;
		
		if (n == 0) {
			break;
		}
	}

	char buf[102];
	int start_index = 0;
	if (original_n < 0 && base != 16) {
		buf[0] = '-';
		start_index = 1;
	}
	int length = strlen(reverse_buf);

	for (int i = 0; i < length; i++) {
		buf[i + start_index] = reverse_buf[length - 1 - i];
	}

	if (original_base == 16) {
		printm("0x");
	}
	printm(buf);
	printm("\n");
	memset(buf, 0, 102);
	memset(reverse_buf, 0, 101);
}

int main(void) {
	print_num(123, 10);
	print_num(-123, 16);
	print_num(4571453, 10);
	print_num(0x1234, 16);
	print_num(0x1234abcdef, 16);
	int x = 123;
	print_num((uintptr_t)&x, 0);

	return 0;
}
