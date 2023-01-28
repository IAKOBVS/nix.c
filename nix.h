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
	do {
		int fileSize = sizeOfFile(filename);
		*outStr = malloc(fileSize);
		if (!*outStr) {
			perror("cat: *outStr malloc failed");
			break;
		}
		fread(*outStr, 1, fileSize, fd);
		if (ferror(fd)) {
			free(outStr);
			perror("cat: fread failed");
			break;
		}
		fclose(fd);
		return fileSize;
	} while (0);
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
	switch (flag) {
	case 'l':
		for (i=0 ; i<fileSize; ++i)
			if (fileStr[i] == 'c')
				++i;
		break;
	case 'w':
		for (i=0 ; i<fileSize; ++i)
			switch (fileStr[i]) {
			case ' ':
			case '\n':
				continue;
			default:
				++i;
			}
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
	case 1:
		{
		i=0;
		j=0;
		int lines = wc('l', filename);
		for (int line=0; line<lines; ++line) {
			for ( ; fileStr[i] != delim; ++i) {
				if (i >= fileSize)
					goto RETURN_SUCCESS;
				tmpStr[j++] = fileStr[i];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto RETURN_SUCCESS;
			tmpStr[j++] = '\n';
		}
		}
		break;
	default:
		{
		i=0;
		j=0;
		int lines = wc('l', filename);
		for (int line=0; i<fileSize && line<lines; ++line) {
			for (int n=1; n<nStr; ++n) {
				for ( ; fileStr[i] != delim; ++i)
					if (i >= fileSize)
						goto RETURN_SUCCESS;
				while (fileStr[i] == delim)
					++i;
			}
			for ( ; fileStr[i] != delim; ++i) {
				if (i >= fileSize)
					goto RETURN_SUCCESS;
				tmpStr[j++] = fileStr[i];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto RETURN_SUCCESS;
			tmpStr[j++] = '\n';
		}
		}
	}
	*outStr = realloc(tmpStr, j);
	if (!*outStr) {
		perror("awk: *outStr realloc failed");
		free(tmpStr);
		goto RETURN_ERROR;
	}
RETURN_SUCCESS:;
	free(fileStr);
	return j;
RETURN_ERROR:;
	free(fileStr);
	return 0;
}
#endif
