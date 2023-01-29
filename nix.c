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

/* modes
'w' = overwrite
'a' = append */
int tee(char *flag, char *inStr, char *filename)
{
	/* fd must be fclosed */
	FILE *fd = fopen(filename, flag);
	if (fd) {
		fputs(inStr, fd);
		fclose(fd);
		return 1;
	}
	fprintf(stderr, "tee(%s, %s, %s):", flag, inStr, filename);
	perror("");
	return 0;
}

/* get first line of file */
int head(char *filename, char **outStr)
{
	int mallocSize = 512;
	FILE *fd;
	fd = fopen(filename, "r");
	if (!fd)
		goto ERR;
	*outStr = malloc(mallocSize);
	if (!*outStr)
		goto ERR_CLOSE;
	fgets(*outStr, mallocSize, fd);
	if (ferror(fd))
		goto ERR_CLOSE_FREE;
	int strLen = strlen(*outStr);
	if (mallocSize > (strLen * 2)) {
		mallocSize = strLen * 2;
		char *tmp = realloc(*outStr, mallocSize);
		if (!tmp)
			goto ERR_CLOSE_FREE;
		*outStr = tmp;
		free(tmp);
	}
	fclose(fd);
	return mallocSize;
ERR_CLOSE_FREE:
	free(*outStr);
ERR_CLOSE:
	fclose(fd);
ERR:
	fprintf(stderr, "head(%s, char **outStr):", filename);
	perror("");
	return 0;
}

int cat(char *filename, char **outStr)
{
	/* fd must be fclosed */
	FILE *fd;
	fd = fopen(filename, "r");
	if (!fd)
		goto ERR;
	int fileSize = sizeOfFile(filename);
	*outStr = malloc(fileSize);
	if (!*outStr)
		goto ERR_CLOSE;
	fread(*outStr, 1, fileSize, fd);
	if (ferror(fd))
		goto ERR_CLOSE_FREE;
	fclose(fd);
	return fileSize;

ERR_CLOSE_FREE:
	free(*outStr);
ERR_CLOSE:
	fclose(fd);
ERR:
	fprintf(stderr, "cat(%s, *outStr):", filename);
	perror("");
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
		flag=0;
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
	case 0:
	default:
		fprintf(stderr, "wc(%c, %s):", flag, filename);
		perror("");
		return 0;
	}
	free(fileStr);
	return count;
}

int awk(char delim, int nStr, char *filename, char **outStr)
{
	/* fileStr must be freed */
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize)
		goto ERR;
	*outStr = malloc(fileSize);
	if (!*outStr)
		goto ERR;
	int j=0;
	int i=0;
	int line=0;
	int lines=wc('l', filename);
	switch (nStr) {
	case 1:
		do {
			for ( ; fileStr[i] != delim; ++i) {
				if (i >= fileSize)
					goto EXIT_LOOPS;
				*outStr[j++] = fileStr[i];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto EXIT_LOOPS;
			*outStr[j++] = '\n';
			++line;
		} while (line<lines);
		break;
	default:
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
				*outStr[j++] = fileStr[i];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto MALLOC;
			*outStr[j++] = '\n';
			++line;
		} while (line<lines);
	}
MALLOC:
	if (fileSize > (j * 2)) {
		fileSize = j * 2;
		char *tmp = realloc(*outStr, fileSize);
		if (!tmp)
			goto FREE;
		*outStr = tmp;
		free(tmp);
	}
	free(fileStr);
	return fileSize;
FREE:
	free(fileStr);
ERR:
	fprintf(stderr, "awk(%c, %d, %s, char **outStr):", delim, nStr, filename);
	perror("");
	return 0;
EXIT_LOOPS:
if (j)
	goto MALLOC;
goto ERR;
}
