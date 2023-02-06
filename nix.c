#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "nix.h"
#include "/home/james/c/jString/jstr.h"
#include "/home/james/c/jArray/jarr.h"

#define IF_DO(STATE, DO) \
	if (STATE) \
		DO
#define ERROR_IF(STATE) \
	IF_DO(STATE, goto ERROR)
#define ERROR_CLOSE_IF(STATE) \
	IF_DO(STATE, goto ERROR_CLOSE)
#define ERROR_FREE_IF(STATE) \
	IF_DO(STATE, goto ERROR_FREE)
#define ERROR_CLOSE_FREE_IF(STATE) \
	IF_DO(STATE, goto ERROR_CLOSE_FREE)
#define EXIT_LOOPS_IF(STATE) \
	IF_DO(STATE, goto EXIT_LOOPS)

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

/* get first line of file */
int head(char *filename, Jstr *dest)
{
	FILE *fd = fopen(filename, "r");
	ERROR_IF(!fd);
	ERROR_CLOSE_IF(!(dest->str = malloc((dest->size = 512))));
	fgets(dest->str, dest->size, fd);
	ERROR_CLOSE_FREE_IF(ferror(fd)
	|| (dest->size > (dest->len * 2) && (!(dest->str = realloc(dest->str, (dest->size = dest->len * 2))))));
	dest->str[dest->len + 1] = '\0';
	fclose(fd);
	return dest->size;

ERROR_CLOSE_FREE:
	jstrDeletePtr(dest);
	dest->size = 0;
ERROR_CLOSE:
	fclose(fd);
ERROR:
	perror("");
	return 0;
}

int cat(char *filename, Jstr *dest)
{
	FILE *fd = fopen(filename, "r");
	ERROR_IF(!fd);
	ERROR_CLOSE_IF(!(dest->str = malloc((dest->size = sizeOfFile(filename) + 1))));
	dest->len = fread(dest->str, 1, dest->size, fd);
	if (dest->len) {
		dest->str[dest->len + 1] = '\0';
		fclose(fd);
		return dest->len;
	}
	jstrDeletePtr(dest);
ERROR_CLOSE:
	fclose(fd);
ERROR:
	perror("");
	return 0;
}

/* flags: l' =  line; 'w' =  word */
int wc(char flag, char *filename)
{
	Jstr fileStr;
	int i;
	int count;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize)
		flag = 0;
	switch (flag) {
	case 'l':
		i = 0;
		count = 0;
		do {
			if (fileStr.str[i] == '\n')
				++count;
			++i;
		} while (i<fileSize);
		break;
	case 'w':
		i = 0;
		count = 0;
		do {
			switch (fileStr.str[i]) {
			case ' ':
			case '\n':
				++i;
				continue;
			default:
				++count;
			}
			++i;
		} while (i<fileSize);
		break;
	case 0:
	default:
		perror("");
		return 0;
	}
	jstrDelete(fileStr);
	return count;
}

int awk(char delim, int nStr, char *src, int srcLen, Jstr *dest)
{
	ERROR_IF(!srcLen && !(srcLen = strlen(src)));
	int j = 0;
	switch (nStr) {
	case 0:
		goto ERROR_FREE;
	case 1:
		for (int i = 0;; ) {
			for ( ; src[i] != delim; ++i) {
				EXIT_LOOPS_IF(i >= srcLen);
				dest->str[j++] = src[i];
			}
			for ( ; src[i] != '\n'; ++i)
				EXIT_LOOPS_IF(i >= srcLen);
			dest->str[j++] = '\n';
		}
		break;
	default:
		for (int i = 0, n = 1;; ) {
			do {
				for ( ; src[i] != delim; ++i)
					EXIT_LOOPS_IF(i >= srcLen);
				while (src[i] == delim)
					++i;
				++n;
			} while (n<nStr);
			for ( ; src[i] != delim; ++i) {
				EXIT_LOOPS_IF(i >= srcLen);
				dest->str[j++] = src[i];
			}
			for ( ; src[i] != '\n'; ++i)
				EXIT_LOOPS_IF(i >= srcLen);
			dest->str[j++] = '\n';
		}
	}
SUCCESS:
	if (srcLen > (j * 2)) {
		srcLen = j * 2;
		ERROR_FREE_IF(!(dest->str = realloc(dest->str, srcLen)));
		dest->str[++j] = '\0';
		return j;
	}

ERROR_FREE:
	jstrDeletePtr(dest);
ERROR:
	perror("");
	return 0;
EXIT_LOOPS:
	if (j)
		goto SUCCESS;
	goto ERROR_FREE;
}

int awkFile(char delim, int nStr, char *filename, Jstr *dest)
{
	int fileSize = cat(filename, dest);
	ERROR_IF(!fileSize);
	int ret = awk(delim, nStr, dest->str, fileSize, dest);
	if (ret)
		return ret;

	jstrDeletePtr(dest);
ERROR:
	perror("");
	return 0;
}
