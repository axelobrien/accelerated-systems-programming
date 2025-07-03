#include <stddef.h>
#include <sys/types.h>

char *cs50_strcat(char *dst, char const *src);

int cs50_strcmp(char const *s1, char const *s2);

char *cs50_strcpy(char *dst, char const *src);

int cs50_strlen(const char *s);

char *cs50_strncpy(char *dst, char const *src, size_t dsize);

long cs50_strtol(char const *nptr, char const **endptr, int base);

int cs50_close(int fd);

void cs50_exit(int status);

int cs50_getchar(void);

int cs50_open(char const *pathname, int flags, mode_t mode);

int cs50_puts(char const *s);

ssize_t cs50_read(int fd, void *buf, size_t count);

ssize_t cs50_write(int fd, void const *buf, size_t count);
