#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "nix.h"

int sizeOfFile(char *filename)
{
	struct stat fileInfo;
	stat(filename, &fileInfo);
	return fileInfo.st_size;
}

/* fopenMode: */
/* 'w' = overwrite */
/* 'a' = append */
int tee(char *flag, char *inStr, char *filename)
{
	/* fd must be fclosed */
	FILE *fd = fopen(filename, flag);
	if (!fd)
		goto ERROR;
	fputs(inStr, fd);
	fclose(fd);
	return 1;

ERROR:;
	fprintf(stderr, "tee(%s, %s, %s):", flag, inStr, filename);
	perror("");
	return 0;
}

/* get first line of file */
int head(char *filename, char **outStr)
{
	FILE *fd;
	fd = fopen(filename, "r");
	if (!fd)
		goto ERROR;
	char *tmpStr = malloc(1024);
	if (!tmpStr)
		goto ERROR_CLOSE;
	fgets(tmpStr, 1024, stdin);
	if (ferror(fd))
		goto ERROR_CLOSE_FREE;
	int strLen = strlen(tmpStr);
	*outStr = realloc(tmpStr, strLen);
	if (!*outStr)
		goto ERROR_CLOSE_FREE;
	fclose(fd);
	return strLen;

ERROR:;
	fprintf(stderr, "head(%s, %s):", filename, *outStr);
	perror("");
	return 0;
ERROR_CLOSE:;
	fprintf(stderr, "head(%s, %s):", filename, *outStr);
	perror("");
	fclose(fd);
	return 0;
ERROR_CLOSE_FREE:;
	fprintf(stderr, "head(%s, %s):", filename, *outStr);
	perror("");
	fclose(fd);
	free(tmpStr);
	return 0;
}

int cat(char *filename, char **outStr)
{
	/* fd must be fclosed */
	FILE *fd;
	fd = fopen(filename, "r");
	if (!fd)
		goto ERROR;
	int fileSize = sizeOfFile(filename);
	*outStr = malloc(fileSize);
	if (!*outStr)
		goto ERROR_CLOSE;
	fread(*outStr, 1, fileSize, fd);
	if (ferror(fd))
		goto ERROR_CLOSE_FREE;
	fclose(fd);
	return fileSize;

ERROR:;
	fprintf(stderr, "cat(%s, *outStr):", filename);
	perror("");
	return 0;
ERROR_CLOSE:;
	fprintf(stderr, "cat(%s, *outStr):", filename);
	perror("");
	fclose(fd);
	return 0;
ERROR_CLOSE_FREE:;
	free(*outStr);
	fprintf(stderr, "cat(%s, *outStr):", filename);
	perror("");
	fclose(fd);
	return 0;
}

/* flag: */
/* 'l' =  line */
/* 'w' =  word */
int wc(char flag, char *filename)
{
	/* fileStr must be freed */
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize)
		goto ERROR;
	int count;
	switch (flag) {
	case 'l':
		{
		int i=0;
		count=0;
		do {
			if (fileStr[i] == '\n')
				++count;
			++i;
		} while (i<fileSize);
		}
		break;
	case 'w':
		{
		int i=0;
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
		}
		break;
	default:
		goto ERROR;
	}
	free(fileStr);
	return count;

ERROR:;
	fprintf(stderr, "wc(%c, %s):", flag, filename);
	perror("");
	return 0;
}

int awk(char delim, int nStr, char *filename, char **outStr)
{
	/* fileStr must be freed */
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize) {
		fprintf(stderr, "awk(%c, %d, %s, %s):", delim, nStr, filename, *outStr);
		perror("");
		return 0;
	}
	char *tmpStr = malloc(fileSize);
	if (!tmpStr) {
		goto ERROR;
	}
	int j;
	switch (nStr) {
	case 1:
		{
		int i=0;
		j=0;
		int line=0;
		int lines = wc('l', filename);
		do {
			for ( ; fileStr[i] != delim; ++i) {
				if (i >= fileSize)
					goto EXIT_LOOPS;
				tmpStr[j++] = fileStr[i];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto EXIT_LOOPS;
			tmpStr[j++] = '\n';
			++line;
		} while (line<lines);
		}
		break;
	default:
		{
		int i=0;
		j=0;
		int line=0;
		int lines = wc('l', filename);
		do {
			int n=1;
			do {
				for ( ; fileStr[i] != delim; ++i)
					if (i >= fileSize)
						goto EXIT_LOOPS;
				while (fileStr[i] == delim)
					++i;
				++n;
			} while (n<nStr);
			for ( ; fileStr[i] != delim; ++i) {
				if (i >= fileSize)
					goto EXIT_LOOPS;
				tmpStr[j++] = fileStr[i];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto EXIT_LOOPS;
			tmpStr[j++] = '\n';
			++line;
		} while (line<lines);
		}
	}
EXIT_LOOPS:;
	if (j)
		*outStr = realloc(tmpStr, j);
	if (!*outStr)
		goto ERROR_FREE;
	free(fileStr);
	return j;

ERROR:;
	fprintf(stderr, "awk(%c, %d, %s, %s):", delim, nStr, filename, *outStr);
	perror("");
	return 0;
ERROR_FREE:;
	fprintf(stderr, "awk(%c, %d, %s, %s):", delim, nStr, filename, *outStr);
	perror("");
	free(fileStr);
	return 0;
}
