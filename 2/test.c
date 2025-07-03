#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "utf8.h"

int test_string(uint8_t *s) {
	uint32_t buf[1000];
	// printf("Parsing %s\n", s);
	uint64_t result = parse_utf8(s, strlen(s) + 1, buf);
	// printf("%d\n", result);

	for (int i = 0; i < result; i++) {
		printf("%ld ", buf[i]);
	}
	printf("\n");

	return result;
}

int main(void) {
	uint8_t *s = "hello";
	uint8_t s2[] = {0xe3, 0xba, 0x80, 0};
	uint8_t s3[] = {0xe2, 0xa0, 0xac, 0};
	uint8_t s4[] = {0xe2, 0xa0, 0xac, 0xe3, 0xba, 0x80, 0};
	uint8_t s5[] = {0xe2, 0xe2, 0xec, 0xe3, 0xba, 0x80, 0};
	uint8_t s6[] = {0xa0, 0xa0, 0xac, 0xe3, 0xba, 0x80, 0};
	uint8_t s7[] = {0xe2, 0x41, 0xac, 0xe3, 0xba, 0x80, 0};
	uint8_t s8[] = {0xc8, 0x80, 0};
	uint8_t s9[] = {0};
	uint8_t s10[] = {};
	uint8_t s11[] = {0xf0, 0x9d, 0x93, 0x80};

	printf("result: %lld\n", test_string(s));
	printf("result: %lld\n", test_string(s2));
	printf("result: %lld\n", test_string(s3));
	printf("result: %lld\n", test_string(s4));
	printf("result: %lld\n", test_string(s8));
	printf("result: %lld\n", test_string(s9));
	printf("result: %lld\n", test_string(s10));
	printf("result: %lld\n", test_string(s11));
	printf("result: %lld\n", test_string(s7));

	return 0;
}

