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
	FILE *fd;
	fd = fopen(filename, "r");
	if (!fd)
		goto ERR;
	*dest = malloc(mallocSize);
	if (!*dest)
		goto ERR_CLOSE;
	fgets(*dest, mallocSize, fd);
	if (ferror_unlocked(fd))
		goto ERR_CLOSE_FREE;
	int strLen = strlen(*dest);
	if (mallocSize > (strLen * 2)) {
		mallocSize = strLen * 2;
		*dest = realloc(*dest, mallocSize);
		if (!*dest)
			goto ERR_CLOSE_FREE;
	}
	*dest[strLen + 1] = '\0';
	fclose(fd);
	return mallocSize;

ERR_CLOSE_FREE:
	free(*dest);
ERR_CLOSE:
	fclose(fd);
ERR:
	fprintf(stderr, "head(%s, char **dest): ", filename);
	perror("");
	return 0;
}

int cat(char *filename, char **dest)
{
	FILE *fd;
	fd = fopen(filename, "r");
	if (!fd)
		goto ERR;
	int fileSize = sizeOfFile(filename);
	*dest = malloc(++fileSize);
	if (!*dest)
		goto ERR_CLOSE;
	int sizeRead = fread_unlocked(*dest, 1, fileSize, fd);
	if (sizeRead) {
		(*dest)[sizeRead + 1] = '\0';
		fclose(fd);
		return sizeRead;
	}
	free(*dest);
ERR_CLOSE:
	fclose(fd);
ERR:
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
	if (!strLen) {
		strLen = strlen(*src);
		if (!strLen)
			goto ERR;
	}
	char *tmp = malloc(++strLen);
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
					goto EXIT_LOOPS;
			tmp[j++] = '\n';
		}
	}
SUCCESS:
	if (strLen > (j * 2)) {
		strLen = j * 2;
		*src = realloc(tmp, strLen);
		if (!*src)
			goto ERR_FREE;
		(*src)[++j] = '\0';
		return j;
	} else {
		goto ERR_FREE;
	}

ERR_FREE:
	free(tmp);
ERR:
	fprintf(stderr, "awk(%c, %d, char **src): ", delim, nStr);
	perror("");
	return 0;
EXIT_LOOPS:
	if (j)
		goto SUCCESS;
	fprintf(stderr, "string is found\n");
	goto ERR_FREE;
}

int awkFile(char delim, int nStr, char *filename, char **src)
{
	int fileSize = cat(filename, &(*src));
	if (!fileSize)
		goto ERR;
	int ret = awk(delim, nStr, &(*src), fileSize);
	if (ret)
		return ret;
	free(*src);
ERR:
	fprintf(stderr, "awk(%c, %d, char **src): ", delim, nStr);
	perror("");
	return 0;
}
