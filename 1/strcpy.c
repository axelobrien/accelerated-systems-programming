char *cs50_strcpy(char *dst, char const *src) {
	while (*src != '\0') {
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';
	return dst;
}
