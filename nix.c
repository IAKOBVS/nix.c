#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
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

int findDir(char *dir, char **dest)
{
	struct dirent *ep;
	DIR *dp;
	if (!(dp = opendir (dir))
	|| (!(*dest = malloc(3200))))
		goto ERROR;
	for (int i = 0; (ep = readdir(dp)); ) {
		memcpy(*dest, ep->d_name, sizeof ep->d_name);
		*dest[i += strlen(ep->d_name)] = ' ';
	}
	return 1;
ERROR:
	perror("");
	return 0;
}

#define DEFAULT -1
#define CLOSE -2
#define CLOSE_FREE -3

#define ERROR(CODE) \
	(error = CODE), 1

/* get first line of file */
int head(char *filename, Jstr *dest)
{
	int error;
	FILE *fd = fopen(filename, "r");
	if ((!fd && (ERROR(DEFAULT)))
	|| (!(dest->str = malloc((dest->size = 512))) && (dest->size = 0, ERROR(CLOSE)))
	|| (fgets(dest->str, dest->size, fd), ferror(fd) && (ERROR(CLOSE_FREE)))
	|| (dest->size > ((dest->len = strlen(dest->str)) * 2) && (!(dest->str = realloc(dest->str, (dest->size = dest->len * 2))) && (ERROR(CLOSE_FREE)))))
		goto ERROR;
	fclose(fd);
	dest->str[dest->len] = '\0';
	return dest->size;

ERROR:
	switch (error) {
	case CLOSE_FREE:
		jstrDeletePtr(dest);
	case CLOSE:
		fclose(fd);
	case DEFAULT:
		perror("");
	}
	return 0;
}

#undef DEFAULT
#undef CLOSE
#undef CLOSE_FREE

#define DEFAULT -1
#define CLOSE -2
#define CLOSE_FREE -3

int cat(char *filename, char **dest)
{
	int error;
	int fileSize;
	FILE *fd = fopen(filename, "r");
	do {
		if ((!fd && (ERROR(DEFAULT)))
		|| (!(*dest = malloc((fileSize = sizeOfFile(filename) + 1))) && (ERROR(CLOSE)))
		|| (!(fileSize = fread(*dest, 1, fileSize, fd)) && (ERROR(CLOSE_FREE))))
			break;
		fclose(fd);
		(*dest)[fileSize - 1] = '\0';
		return fileSize;
	} while (0);
	switch (error) {
	case CLOSE_FREE:
		free(*dest);
	case CLOSE:
		fclose(fd);
	case DEFAULT:
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
	|| (!dest->size && !(dest->str = malloc((dest->size = srcLen))) && (dest->size = 0, 1)))
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
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (fileSize) {
		int ret = awk(delim, nStr, dest->str, fileSize, dest);
		free(fileStr);
		if (ret)
			return ret;
		jstrDeletePtr(dest);
	}
	perror("");
	return 0;
}
