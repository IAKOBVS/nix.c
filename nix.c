#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "nix.h"
#include "../jString/jstr.h"

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
	struct stat fileInfo;
	stat(filename, &fileInfo);
	return fileInfo.st_size;
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
	fprintf(stderr, "tee(%s, %s, %s): ", flag, inStr, filename);
	perror("");
	return 0;
}

/* get first line of file */
int head(char *filename, char **dest)
{
	int mallocSize = 512;
	FILE *fd = fopen(filename, "r");
	ERROR_IF( !fd);
	ERROR_CLOSE_IF( !(*dest = malloc(mallocSize)));
	fgets(*dest, mallocSize, fd);
	ERROR_CLOSE_FREE_IF( ferror_unlocked(fd));
	int strLen = strlen(*dest);
	if (mallocSize > (strLen * 2)) {
		mallocSize = strLen * 2;
		ERROR_CLOSE_FREE_IF( !(*dest = realloc(*dest, mallocSize)));
	}
	(*dest)[strLen + 1] = '\0';
	fclose(fd);
	return mallocSize;

ERROR_CLOSE_FREE:
	free(*dest);
ERROR_CLOSE:
	fclose(fd);
ERROR:
	fprintf(stderr, "head(%s, char **dest): ", filename);
	perror("");
	return 0;
}

int cat(char *filename, char **dest)
{
	FILE *fd;
	fd = fopen(filename, "r");
	ERROR_IF( !fd);
	int fileSize = sizeOfFile(filename) + 1;
	ERROR_CLOSE_IF( !(*dest = malloc(fileSize)));
	int sizeRead = fread_unlocked(*dest, 1, fileSize, fd);
	if (sizeRead) {
		(*dest)[sizeRead + 1] = '\0';
		fclose(fd);
		return sizeRead;
	}
	free(*dest);
ERROR_CLOSE:
	fclose(fd);
ERROR:
	fprintf(stderr, "cat(%s, *dest): ", filename);
	perror("");
	return 0;
}

/* flags: l' =  line; 'w' =  word */
int wc(char flag, char *filename)
{
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize)
		flag=0;
	int i;
	int count;
	switch (flag) {
	case 'l':
		i=0;
		count=0;
		do {
			if (fileStr[i] == '\n')
				++count;
			++i;
		} while (i<fileSize);
		break;
	case 'w':
		i=0;
		count=0;
		do {
			switch (fileStr[i]) {
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
		fprintf(stderr, "wc(%c, %s): ", flag, filename);
		perror("");
		return 0;
	}
	free(fileStr);
	return count;
}

int awk(char delim, int nStr, char **src, int strLen)
{
	ERROR_IF( !strLen && !(strLen = strlen(*src)) );
	char *tmp = malloc(++strLen);
	ERROR_IF( !tmp);
	int j=0;
	switch (nStr) {
	case 0:
		goto ERROR_FREE;
	case 1:
		for (int i=0;;) {
			for ( ; (*src)[i] != delim; ++i) {
				EXIT_LOOPS_IF( i >= strLen);
				tmp[j++] = (*src)[i];
			}
			for ( ; (*src)[i] != '\n'; ++i)
				EXIT_LOOPS_IF( i >= strLen);
			tmp[j++] = '\n';
		}
		break;
	default:
		for (int i=0, n=1 ;;) {
			do {
				for ( ; (*src)[i] != delim; ++i)
					EXIT_LOOPS_IF( i >= strLen);
				while ((*src)[i] == delim)
					++i;
				++n;
			} while (n<nStr);
			for ( ; (*src)[i] != delim; ++i) {
				EXIT_LOOPS_IF( i >= strLen);
				tmp[j++] = (*src)[i];
			}
			for ( ; (*src)[i] != '\n'; ++i)
				EXIT_LOOPS_IF( i >= strLen);
			tmp[j++] = '\n';
		}
	}
SUCCESS:
	if (strLen > (j * 2)) {
		strLen = j * 2;
		*src = realloc(tmp, strLen);
		ERROR_FREE_IF( !*src);
		(*src)[++j] = '\0';
		return j;
	}

ERROR_FREE:
	free(tmp);
ERROR:
	fprintf(stderr, "awk(%c, %d, char **src): ", delim, nStr);
	perror("");
	return 0;
EXIT_LOOPS:
	if (j)
		goto SUCCESS;
	fprintf(stderr, "string is found\n");
	goto ERROR_FREE;
}

int awkFile(char delim, int nStr, char *filename, char **src)
{
	int fileSize = cat(filename, &(*src));
	ERROR_IF( !fileSize);
	int ret = awk(delim, nStr, &(*src), fileSize);
	if (ret)
		return ret;
	free(*src);
ERROR:
	fprintf(stderr, "awk(%c, %d, char **src): ", delim, nStr);
	perror("");
	return 0;
}
