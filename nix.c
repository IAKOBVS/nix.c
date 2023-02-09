#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
/* #include <assert.h> */

#include "nix.h"
#include "/home/james/c/jString/jstr.h"
#include "/home/james/c/jArray/jarr.h"

int sizeOfFile(char *filename)
{
	struct stat st;
	return (!stat(filename, &st) ? st.st_size : 0);
}

/* flags: 'w' = overwrite; 'a' = append */
int tee(char *flag, char *inStr, char *filename)
{
	FILE *fd = fopen(filename, flag);
	if (fd) {
		fputs(inStr, fd);
		fclose(fd);
		return 1;
	}
	perror("");
	return 0;
}

#define ERROR_ -1
#define ERROR_CLOSE -2
#define ERROR_CLOSE_FREE -3

/* get first line of file */
int head(char *filename, Jstr *dest)
{
	int error;
	FILE *fd = fopen(filename, "r");
	if ((!fd && (error = ERROR_, 1))
	|| (!(dest->str = malloc(512)) && (error = ERROR_CLOSE, 1))
	|| (fgets(dest->str, dest->size, fd), ferror(fd) && (error = ERROR_CLOSE_FREE, 1))
	|| (dest->size > ((dest->len = strlen(dest->str)) * 2) && (!(dest->str = realloc(dest->str, (dest->size = dest->len * 2))) && (error = ERROR_CLOSE_FREE, 1))))
		goto ERROR;
	dest->str[dest->len] = '\0';
	fclose(fd);
	return dest->size;

ERROR:
	switch (error) {
	case ERROR_CLOSE_FREE:
		jstrDeletePtr(dest);
	case ERROR_CLOSE:
		fclose(fd);
	case ERROR_:
		perror("");
	}
	return 0;
}

#undef ERROR_
#undef ERROR_CLOSE
#undef ERROR_CLOSE_FREE

#define ERROR_ -1
#define ERROR_CLOSE -2
#define ERROR_CLOSE_FREE -3

int cat(char *filename, Jstr *dest)
{
	int error;
	FILE *fd = fopen(filename, "r");
	if ((!fd && (error = ERROR_, 1))
	|| (!(dest->str = malloc((dest->size = sizeOfFile(filename) + 1))) && (error = ERROR_CLOSE, 1))
	|| (!(dest->len = fread(dest->str, 1, dest->size, fd)) && (error = ERROR_CLOSE_FREE, 1)))
		goto ERROR;
	fclose(fd);
	dest->str[dest->len] = '\0';
	return dest->size;

ERROR:
	switch (error) {
	case ERROR_CLOSE_FREE:
		jstrDeletePtr(dest);
	case ERROR_CLOSE:
		fclose(fd);
	case ERROR_:
		perror("");
	}
	return 0;
}

#undef ERROR_
#undef ERROR_CLOSE
#undef ERROR_CLOSE_FREE

/* flags: l' =  line; 'w' =  word */
int wc(char flag, char *src)
{
	int count = 0;
	switch (flag) {
	case 'l':
		while (*src)
			if (*src++ == '\n')
				++count;
		break;
	case 'w':
		while (*src)
			switch (*src++) {
			case ' ':
			case '\n':
				++src;
				continue;
			default:
				++count;
			}
		break;
	default:
		goto ERROR;
	}
	return count;

ERROR:
	perror("");
	return 0;
}

int awk(char delim, int nStr, char *src, int srcLen, Jstr *dest)
{
	if ((!srcLen && !(srcLen = strlen(src)))
	|| (!dest->size && !(dest->str = malloc((dest->size = srcLen)))))
		goto ERROR;
	int j = 0;
	switch (nStr) {
	case 0:
		goto ERROR_FREE;
	case 1:
		for (;;) {
			for ( ; *src != delim && *src != '\n'; ++j, ++src) {
				if (!*src)
					goto IF_SUCCESS;
				if (*src == '\n')
					goto SKIP_LOOPS_1;
				dest->str[j] = *src;
			}
			for ( ; *src != '\n'; ++src) {
				if (!*src)
					goto IF_SUCCESS;
			}
SKIP_LOOPS_1:
			dest->str[j++] = '\n';
			++src;
		}
		break;
	default:
		for (int n = 1;; ) {
			do {
				for ( ; *src != delim; ++src) {
					if (!*src)
						goto IF_SUCCESS;
					if (*src == '\n')
						goto SKIP_LOOPS;
				}
				while (*src == delim)
					++src;
				++n;
			} while (n < nStr);
			for ( ; *src != delim; ++j, ++src) {
				if (!*src)
					goto IF_SUCCESS;
				if (*src == '\n')
					goto SKIP_LOOPS;
				dest->str[j] = *src;
			}
			for ( ; *src != '\n'; ++src) {
				if (!*src)
					goto IF_SUCCESS;
			}
SKIP_LOOPS:
			dest->str[j++] = '\n';
			++src;
		}
	}
SUCCESS:
	if (srcLen > (j * 2))
		if (!(dest->str = realloc(dest->str, (dest->size = j * 2))))
			goto ERROR_FREE;
	dest->str[j] = '\0';
	return (dest->len = j);

IF_SUCCESS:
	if (j)
		goto SUCCESS;
ERROR_FREE:
	jstrDeletePtr(dest);
ERROR:
	perror("");
	return 0;
}

int awkFile(char delim, int nStr, char *filename, Jstr *dest)
{
	int fileSize = cat(filename, dest);
	if (fileSize) {
		int ret = awk(delim, nStr, dest->str, fileSize, dest);
		if (ret)
			return ret;

		jstrDeletePtr(dest);
	}
	perror("");
	return 0;
}
