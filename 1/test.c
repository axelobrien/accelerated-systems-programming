#include <stdio.h>
#include <stdlib.h>
#include "libcs50.h"

int main(void) {
	char s[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0'}; 
	printf("%d\n", cs50_strlen(s));

	char s1[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0'}; 
	char s2[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0'}; 
	
	printf("%d\n", cs50_strcmp(s1, s2));
	
	char s3[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0'}; 
	char s4[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '.', '\0'}; 
	
	printf("%d\n", cs50_strcmp(s3, s4));
	
	char s5[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '\0'}; 
	char s6[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '.', '\0'}; 
	
	printf("%d\n", cs50_strcmp(s5, s6));

	char buffer[14];
	printf("%c\n", cs50_strcpy(buffer, s6)[-3]);

	char s7[] = {'H', 'e', 'l', 'l', 'o', '\0'};
	char s8[] = {'9', 'W', 'o', 'r', 'l', 'd', '!', '\0'};
	char b2[14];
	cs50_strcat(b2, s7);
	cs50_strcat(b2, s8);
	printf("%s\n", b2);

	char b3[1000];
	char s9[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	
	cs50_strncpy(b3, s9, 26);
	printf("%s\n", b3);

	char *s_octal = "+012";
	char *s_negative = "            \t \t \t \r -40";
	char *s_hex = "     \f    0xfF";
	char *s_hex2 = " 0X0fkakdjhloasdhoa";
	char *s_base36 = "               -bzZ1WoP6";
	char *s_invalid = " +-1999shdfbjdsa";
	char *s_basic = "100";

	char *endptr;

	printf("%d\n", cs50_strtol(s_octal, (char const **)&endptr, 0));
	printf("%s\n", endptr);
	printf("%d\n", cs50_strtol(s_negative, (char const **)&endptr, 10));
	printf("%s\n", endptr);
	printf("%d\n", cs50_strtol(s_hex, (char const **)&endptr, 16));
	printf("%s\n", endptr);
	printf("%d\n", cs50_strtol(s_hex2, (char const **)&endptr, 0));
	printf("%s\n", endptr);
	printf("%d\n", cs50_strtol(s_invalid, (char const **)&endptr, 0));
	printf("%s\n", endptr);
	printf("%d\n", cs50_strtol(s_basic, (char const **)&endptr, 0));
	printf("%s\n", endptr);

	printf("%d\n", cs50_strtol(s_base36, (char const **)&endptr, 36));
	printf("%d\n", strtol(s_base36, (char **)&endptr, 36));
}
