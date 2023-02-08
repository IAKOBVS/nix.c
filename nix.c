#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>

#include "nix.h"
#include "/home/james/c/jString/jstr.h"
#include "/home/james/c/jArray/jarr.h"

#define IF_DO(STATE, DO) \
	if (STATE) \
		DO

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
	if (!fd)
		goto ERROR;;
	if (!(dest->str = malloc((dest->size = 512))))
		goto ERROR_CLOSE;
	fgets(dest->str, dest->size, fd);
	if (ferror(fd)
	|| (dest->size > ((dest->len = strlen(dest->str)) * 2) && (!(dest->str = realloc(dest->str, (dest->size = dest->len * 2))))))
		goto ERROR_CLOSE_FREE;
	dest->str[dest->len] = '\0';
	fclose(fd);
	return dest->size;

ERROR_CLOSE_FREE:
	jstrDeletePtr(dest);
ERROR_CLOSE:
	fclose(fd);
ERROR:
	perror("");
	return 0;
}

int cat(char *filename, Jstr *dest)
{
	FILE *fd = fopen(filename, "r");
	if (!fd)
		goto ERROR;
	if (!(dest->str = malloc((dest->size = sizeOfFile(filename) + 1))))
		goto ERROR_CLOSE;
	dest->len = fread(dest->str, 1, dest->size, fd);
	if (dest->len) {
		dest->str[dest->len] = '\0';
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
	int fileSize;
	Jstr fileStr;
	if (!(fileStr.str = malloc((fileSize = sizeOfFile(filename)))))
		goto ERROR;
	fileStr.size = fileSize;
	if (!(fileSize = cat(filename, &fileStr)))
		goto ERROR_FREE;
	int i;
	int count;
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
	default:
		goto ERROR;
	}
	jstrDelete(fileStr);
	return count;

ERROR_FREE:
	jstrDelete(fileStr);
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
		for (int i = 0;; ) {
			for ( ; src[i] != delim; ++i) {
				if (i >= srcLen)
					goto EXIT_SWITCH;
				if (src[i] == '\n')
					goto EXIT_LOOPS_1;
				dest->str[j++] = src[i];
			}
			for ( ; src[i] != '\n'; ++i)
				if (i >= srcLen)
					goto EXIT_SWITCH;
EXIT_LOOPS_1:
			dest->str[j++] = '\n';
		}
		break;
	default:
		for (int i = 0, n = 1;; ) {
			do {
				for ( ; src[i] != delim; ++i) {
					if (i >= srcLen)
						goto EXIT_SWITCH;
					if (src[i] == '\n')
						goto EXIT_LOOPS_DEFAULT;
				}
				while (src[i] == delim)
					++i;
				++n;
			} while (n<nStr);
			for ( ; src[i] != delim && src[i] != '\n'; ++i) {
				if (i >= srcLen)
					goto EXIT_SWITCH;
				if (src[i] == '\n')
					goto EXIT_LOOPS_DEFAULT;
				dest->str[j++] = src[i];
			}
			for ( ; src[i] != '\n'; ++i)
				if (i >= srcLen)
					goto EXIT_SWITCH;
EXIT_LOOPS_DEFAULT:
			dest->str[j++] = '\n';
		}
	}
SUCCESS:
	if (srcLen > (j * 2)) {
		srcLen = j * 2;
		if (!(dest->str = realloc(dest->str, srcLen)))
			goto ERROR_FREE;
		dest->str[++j] = '\0';
		return (dest->len = j);
	}

ERROR_FREE:
	jstrDeletePtr(dest);
ERROR:
	perror("");
	return 0;
EXIT_SWITCH:
	if (j)
		goto SUCCESS;
	goto ERROR_FREE;
}

int awkFile(char delim, int nStr, char *filename, Jstr *dest)
{
	int fileSize = cat(filename, dest);
	if (!fileSize)
		goto ERROR;
	int ret = awk(delim, nStr, dest->str, fileSize, dest);
	if (ret)
		return ret;

	jstrDeletePtr(dest);
ERROR:
	perror("");
	return 0;
}
