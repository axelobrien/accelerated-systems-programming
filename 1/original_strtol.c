#include <stdio.h>
#include <stdlib.h>


int main(void) {
	char *s = "    \t 12";
	printf("%x\n", strtol(s, &s, 0));
}
