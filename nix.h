#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static int sizeOfFile(char *filename)
{
	struct stat fileInfo;
	stat(filename, &fileInfo);
	return fileInfo.st_size;
}

/* fopenMode: */
/* 'w' = overwrite */
/* 'a' = append */
static int tee(char *flag, char *inStr, char *filename)
{
	/* fd must be fclosed */
	FILE *fd = fopen(filename, flag);
	if (!fd) {
		perror("tee: fopen failed");
		return 0;
	}
	fputs(inStr, fd);
	fclose(fd);
	return 1;
}

static int cat(char *filename, char **outStr)
{
	/* fd must be fclosed */
	FILE *fd;
	fd = fopen(filename, "r");
	if (!fd) {
		perror("cat: fopen failed");
		return 0;
	}
	int fileSize = sizeOfFile(filename);
	*outStr = malloc(fileSize);
	if (!*outStr) {
		perror("cat: *outStr malloc failed");
		goto RETURN_ERROR;
	}
	fread(*outStr, 1, fileSize, fd);
	if (ferror(fd)) {
		free(outStr);
		perror("cat: fread failed");
		goto RETURN_ERROR;
	}
	fclose(fd);
	return fileSize;
RETURN_ERROR:;
	fclose(fd);
	return 0;
}

/* flag: */
/* 'l' =  line */
/* 'w' =  word */
static int wc(char flag, char *filename)
{
	/* fileStr must be freed */
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize) {
		perror("wc: cat failed");
		return 0;
	}
	int i;
	int count;
	switch (flag) {
	case 'l':
		i=0;
		do {
			if (fileStr[i] == '\n')
				++count;
			++i;
		} while (i<fileSize);
		break;
	case 'w':
		do {
			switch (fileStr[i]) {
			case ' ':
			case '\n':
				break;
			default:
				++count;
			}
			++i;
		} while (i<fileSize);
		break;
	default:
		perror("wc: invalid flag ('l' or 'w')");
		free(fileStr);
		return 0;
	}
	free(fileStr);
	return i;
}

static int awk(char delim, int nStr, char *filename, char **outStr)
{
	/* fileStr must be freed */
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize) {
		perror("awk: cat failed");
		return 0;
	}
	char *tmpStr;
	*tmpStr = malloc(fileSize);
	if (!tmpStr) {
		perror("awk: tmpStr malloc failed");
		goto RETURN_ERROR;
	}
	int i;
	int j;
	switch (nStr) {
	case 1: {
		i=0;
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
		i=0;
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
		perror("awk: *outStr realloc failed");
		free(tmpStr);
		goto RETURN_ERROR;
	}
	free(fileStr);
	return j;
RETURN_ERROR:;
	free(fileStr);
	return 0;
}

#endif
