int cs50_strlen(char *s) {
	int length = 0;
	while (*s != '\0') {
		length++;
		s++;
	}
	return length;
}

