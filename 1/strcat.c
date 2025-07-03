#include "libcs50.h"

char *cs50_strcat(char *dst, char const *src) {
	cs50_strcpy(dst + cs50_strlen(dst), src);
	return dst;
}
