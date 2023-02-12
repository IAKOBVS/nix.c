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
	FILE *fp = fopen(filename, flag);
	if (fp) {
		fputs(inStr, fp);
		fclose(fp);
		return 1;
	}
	perror("");
	return 0;
}

int findDir(char *dir, char **dest)
{
	struct dirent *ep;
	DIR *dp = opendir(dir);
	if (!dp) goto ERROR;
	if (!(*dest = malloc(3200))) goto ERROR;
	for (int i = 0; (ep = readdir(dp)); ) {
		memcpy(*dest, ep->d_name, sizeof ep->d_name);
		*dest[i += strlen(ep->d_name)] = ' ';
	}
	return 1;

ERROR:
	perror("");
	return 0;
}

/* get first line of file */
int head(char *filename, char **dest)
{
	char buf[512];
	int destLen;
	FILE *fp = fopen(filename, "r");
	if (!fp) goto ERROR;
	fgets(buf, 512, fp);
	if (ferror(fp)) goto ERROR_CLOSE;
	fclose(fp);
	if (!(*dest = malloc((destLen = strlen(buf)) + 1))) goto ERROR;
	memcpy(*dest, buf, destLen);
	(*dest)[destLen] = '\0';
	return destLen;

ERROR_CLOSE:
	fclose(fp);
ERROR:
	perror("");
	return 0;
}

int cat(char *filename, char **dest)
{
	FILE *fp = fopen(filename, "r");
	if (!fp) goto ERROR;
	int fileSize = sizeOfFile(filename);
	if (!(*dest = malloc(fileSize + 1))) goto ERROR_CLOSE;
	if (fileSize != fread(*dest, 1, fileSize, fp)) goto ERROR_CLOSE_FREE;
	fclose(fp);
	(*dest)[fileSize] = '\0';
	return fileSize;

ERROR_CLOSE_FREE:
	free(*dest);
ERROR_CLOSE:
	fclose(fp);
ERROR:
	perror("");
	return 0;
}

int wcl(char *src)
{
	int count = 0;
	while (*src)
		if (*src++ == '\n') ++count;
	return count;
}

int wcc(char *src)
{
	int count = 0;
	while (*src)
		switch (*src++) {
		case ' ':
		case '\n':
		case '\t':
		case '\r':
			continue;
		default:
			++count;
		}
	return count;
}

int wcw(char *src)
{
	int count = 0;
	for (int inWord = 0;; ++src)
		switch (*src) {
		case '\0':
			if (inWord)
				return ++count;
			return count;
		case ' ':
		case '\n':
		case '\t':
		case '\r':
			if (inWord) {
				++count;
				inWord = 0;
			}
			continue;
		default:
			if (!inWord)
				inWord = 1;
		}
}

int awk(char delim, int nStr, char *src, int srcLen, Jstr *dest)
{
	if (!srcLen && !(srcLen = strlen(src))) goto ERROR;
	if (!dest->size)
		if (!(dest->str = malloc((dest->size = srcLen)))) {
			dest->size = 0;
			goto ERROR;
		}
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
