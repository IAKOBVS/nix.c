#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
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
	if (!fd) {
		fprintf(stderr, "tee(%s, %s, %s): fopen failed", flag, inStr, filename);
		perror("");
		return 0;
	}
	fputs(inStr, fd);
	fclose(fd);
	return 1;
}

int cat(char *filename, char **outStr)
{
	/* fd must be fclosed */
	FILE *fd;
	fd = fopen(filename, "r");
	if (!fd) {
		fprintf(stderr, "cat(%s, %s): fopen failed", filename, *outStr);
		perror("");
		return 0;
	}
	int fileSize = sizeOfFile(filename);
	*outStr = malloc(fileSize);
	if (!*outStr) {
		fprintf(stderr, "cat(%s, %s): *outStr malloc failed", filename, *outStr);
		goto RETURN_ERROR;
	}
	fread(*outStr, 1, fileSize, fd);
	if (ferror(fd)) {
		free(*outStr);
		fprintf(stderr, "cat(%s, %s): fread failed", filename, *outStr);
		goto RETURN_ERROR;
	}
	fclose(fd);
	return fileSize;
RETURN_ERROR:;
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
	if (!fileSize) {
		fprintf(stderr, "wc(%c, %s): cat failed", flag, filename);
		goto RETURN_ERROR;
	}
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
		fprintf(stderr, "wc(%c, %s): invalid flag (use 'l' or 'w')", flag, filename);
		perror("");
		goto RETURN_ERROR;
	}
	free(fileStr);
	return count;
RETURN_ERROR:;
	     perror("");
	     return 0;
}

int awk(char delim, int nStr, char *filename, char **outStr)
{
	/* fileStr must be freed */
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize) {
		fprintf(stderr, "awk(%c, %d, %s, %s): cat failed", delim, nStr, filename, *outStr);
		perror("");
		return 0;
	}
	char *tmpStr;
	tmpStr = malloc(fileSize);
	if (!tmpStr) {
		fprintf(stderr, "awk(%c, %d, %s, %s): malloc failed", delim, nStr, filename, *outStr);
		goto RETURN_ERROR;
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
	*outStr = realloc(tmpStr, j);
	if (!*outStr) {
		fprintf(stderr, "awk(%c, %d, %s, %s): *outStr realloc failed", delim, nStr, filename, *outStr);
		free(tmpStr);
		goto RETURN_ERROR;
	}
	free(fileStr);
	return j;
RETURN_ERROR:;
	perror("");
	free(fileStr);
	return 0;
}
