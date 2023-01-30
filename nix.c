#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "nix.h"

struct sizeOfPtr {
	void *p;
	int size; 
};

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
	fprintf(stderr, "tee(%s, %s, %s): ", flag, inStr, filename);
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
	if (ferror_unlocked(fd))
		goto ERR_CLOSE_FREE;
	int strLen = strlen(*outStr);
	if (mallocSize > (strLen * 2)) {
		mallocSize = strLen * 2;
		*outStr = realloc(*outStr, mallocSize);
		if (!*outStr)
			goto ERR_CLOSE_FREE;
	}
	fclose(fd);
	return mallocSize;
ERR_CLOSE_FREE:
	free(*outStr);
ERR_CLOSE:
	fclose(fd);
ERR:
	fprintf(stderr, "head(%s, char **outStr): ", filename);
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
	/* fread(*outStr, 1, fileSize, fd); */
	fread_unlocked(*outStr, 1, fileSize, fd);
	if (ferror(fd))
		goto ERR_CLOSE_FREE;
	fclose(fd);
	return fileSize;

ERR_CLOSE_FREE:
	free(*outStr);
ERR_CLOSE:
	fclose(fd);
ERR:
	fprintf(stderr, "cat(%s, *outStr): ", filename);
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
		fprintf(stderr, "wc(%c, %s): ", flag, filename);
		perror("");
		return 0;
	}
	free(fileStr);
	return count;
}

int awk(char delim, int nStr, char **src, int strLen)
{
	if (!strLen) {
		strLen = strlen(*src);
		if (!strLen)
			goto ERR;
	}
	char *tmp = malloc(strLen);
	if (!tmp)
		goto ERR;
	int j=0;
	switch (nStr) {
	case 1:
		for (int i=0;;) {
			for ( ; (*src)[i] != delim; ++i) {
				if (i >= strLen)
					goto EXIT_LOOPS;
				tmp[j++] = (*src)[i];
			}
			for ( ; (*src)[i] != '\n'; ++i)
				if (i >= strLen)
					goto EXIT_LOOPS;
			tmp[j++] = '\n';
		}
		break;
	default:
		for (int i=0, n=1 ;;) {
			do {
				for ( ; (*src)[i] != delim; ++i)
					if (i >= strLen)
						goto EXIT_LOOPS;
				while ((*src)[i] == delim)
					++i;
				++n;
			} while (n<nStr);
			for ( ; (*src)[i] != delim; ++i) {
				if (i >= strLen)
					goto EXIT_LOOPS;
				tmp[j++] = (*src)[i];
			}
			for ( ; (*src)[i] != '\n'; ++i)
				if (i >= strLen)
					goto CLEANUP;
			tmp[j++] = '\n';
		}
	}
CLEANUP:
	if (strLen > (j * 2)) {
		strLen = j * 2;
		*src = realloc(tmp, strLen);
		if (!*src)
			goto ERR_FREE;
	} else {
		goto ERR_FREE;
	}
	return j;

ERR_FREE:
	free(tmp);
ERR:
	fprintf(stderr, "awk(%c, %d, char **src): ", delim, nStr);
	perror("");
	return 0;
EXIT_LOOPS:
	if (j)
		goto CLEANUP;
	fprintf(stderr, "string is found\n");
	goto ERR_FREE;
}

int awkFile(char delim, int nStr, char *filename, char **src)
{
	/* fileStr must be freed */
	int fileSize = cat(filename, &(*src));
	if (!fileSize)
		goto ERR;
	*src = malloc(fileSize);
	if (!*src)
		goto ERR;
	int ret = awk(delim, nStr, &(*src), fileSize);
	if (ret) {
		return ret;
	}
	free(*src);
ERR:
	fprintf(stderr, "awk(%c, %d, char **src): ", delim, nStr);
	perror("");
	return 0;
}
