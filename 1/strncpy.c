#include <stddef.h>

char *cs50_strncpy(char *dst, char const *src, size_t dsize) {
	int i = 0;
	int isDoneWithString = 1;
	while (i < dsize - 1) {
                if (isDoneWithString != 0) {
			*dst = *src;
			if (*src == '\0') {
				isDoneWithString = 1;
			}
		} else {
			*dst = 0;
		}
                dst++;
                src++;
		i++;
        }
	*dst = '\0';
        return dst;
}

