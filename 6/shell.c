#include <stdio.h> // printf, read, fflush
#include <unistd.h> // execvp, chdir, fork
#include <stdlib.h> // exit
#include <string.h> // strcmp
#include <sys/wait.h> // wait
#include <limits.h> // PATH_MAX

#define MEGABYTE 1024 * 1024

static char parse(char c) {
	switch (c) {
		case '\t':
		case '\r':
		case '\n':
		case ' ':
			return '\0';
		default:
			return c;
	}
}

static int print_line_prefix(char *cwd) {
	printf("\x1b[31m[\x1b[32m%s\x1b[31m]\x1b[0m ✨ ", cwd);
	fflush(stdout);
	return 1;
}

static int tokenize(char **dst, char *src, ssize_t bytes, int already_written) {
		int tokens_count = already_written;

		for (int i = 0; i < bytes; i++) {
			src[i] = parse(src[i]);
		}

		for (int i = 0; i < bytes; i++) {
			if (src[i] == '\0') {
				continue;
			}

			dst[tokens_count] = &(src[i]);
			tokens_count++;
			i += strlen(&(src[i]));

			if (tokens_count == 256) {
				break;
			}
		}

//		This for loop is for debug purposes
//		for (int i = 0; i < tokens_count; i++) {
//			printf("Token %d: ", i);
//			printf("%s\n", dst[i]);
//		}

		return tokens_count;
}

int main(void) {
	char cwd[PATH_MAX];
	char *status = getcwd(cwd, 1024); 

	if (status == NULL) {
		memcpy(cwd, "-", 2);
	}

	char line[MEGABYTE];
	char original_line[MEGABYTE];
	char original_prev_line[MEGABYTE];
	char prev_line[MEGABYTE];
	ssize_t bytes_read = 0;

	print_line_prefix(cwd);

	while ((bytes_read = read(STDIN_FILENO, line, MEGABYTE - 1)) > -1) {
		if (bytes_read == 0) {
			printf("\n");
			exit(EXIT_SUCCESS);
		}

		char *tokens[256];
		memset(tokens, 0, 256);
		memset(line + bytes_read, 0, MEGABYTE - bytes_read);

		if (bytes_read == 1 && *line == '\n') {
			print_line_prefix(cwd);
			continue;
		}

		memcpy(original_line, line, MEGABYTE);
		memcpy(prev_line, original_prev_line, MEGABYTE);
		tokenize(tokens, line, bytes_read, 0);

		if (strcmp(tokens[0], "!!") == 0) {
			if (strlen(prev_line) > 0) {
				memset(tokens, 0, 256);
				int tokens_count = tokenize(tokens, prev_line, strlen(prev_line) + 1, 0);
				tokenize(tokens, original_line + strlen(line), strlen(original_line) + 1 - strlen(line), tokens_count);
				
			}
		} else {
			memcpy(prev_line, original_line, MEGABYTE);
			memcpy(original_prev_line, original_line, MEGABYTE);
		}


		if (strcmp(tokens[0], "cd") == 0) {
			if (tokens[1] == NULL) {
				printf("\"cd\" requires a path argument\n");
				print_line_prefix(cwd);
				continue;
			}

			int status = chdir(tokens[1]);

			if (status == -1) {
				printf("\"cd\" failed, please try again.\n");
			}
			char *new_cwd = getcwd(cwd, 1024);
			print_line_prefix(cwd);
			continue;
		}

		if (strcmp(tokens[0], "exit") == 0) {
			exit(EXIT_SUCCESS);
			continue; // for completeness
		}

		int pid = fork();

		if (pid == 0) {
			int status = execvp(tokens[0], tokens);
			if (status == -1) { //this should always be true
				printf("An error occurred, please try again\n");
			}
		} else {
			wait(NULL);
		}

		print_line_prefix(cwd);
	}

	return 0;
}
