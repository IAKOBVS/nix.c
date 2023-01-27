#ifndef NIX_H_DEF
#define NIX_H_DEF
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

static int sizeOfFile(char *filename)
{
	struct stat fileInfo;
	stat(filename, &fileInfo);
	return fileInfo.st_size;
}

static int wcl(char *filename)
{
	FILE *fd = fopen(filename, "r");
	if (!fd) {
		fprintf(stderr, "wcl: fd is 0\n");
		return 0;
	}
	int count = 0;
	for (int c; c != EOF; c = fgetc(fd))
		if (c == '\n')
			++count;
	fclose(fd);
	return count;
}

static int tee(char *inStr, char *filename)
{
	FILE *fd = fopen(filename, "w");
	if (!fd) {
		fprintf(stderr, "tee: fopen failed\n");
		return 0;
	}
	fprintf(fd, "%s", inStr);
	fclose(fd);
	return 1;
}

static int cat(char *filename, char **outStr)
{
	/* outStr must be freed after used */
	FILE *fd = fopen(filename, "r");
	if (!fd) {
		fprintf(stderr, "cat: fopen failed\n");
		return 0;
	}
	int fileSize = sizeOfFile(filename);
	*outStr = malloc(fileSize);
	if (!*outStr) {
		fprintf(stderr, "cat: *outStr is 0\n");
		goto RETURN_ERROR;
	}
	fread(*outStr, 1, fileSize, fd);
	if (ferror(fd)) {
		fprintf(stderr, "cat: ferror(fd) is 0\n");
		goto RETURN_ERROR;
	}
	fclose(fd);
	return fileSize;
	RETURN_ERROR:;
	fclose(fd);
	return 0;
}

static int awk(char delim, int nStr, char *filename, char **outStr)
{
	/* fileStr must be freed */
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize) {
		fprintf(stderr, "awk: fileSize is 0\n");
		goto RETURN_ERROR;
	}
	*outStr = malloc(fileSize);
	if (!*outStr) {
		fprintf(stderr, "awk: *outStr is 0\n");
		goto RETURN_ERROR;
	}
	if (nStr > 1) {
		int i=0;
		int j=0;
		int lines = wcl(filename);
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
				(*outStr)[j++] = fileStr[i];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto RETURN_SUCCESS;
			(*outStr)[j++] = '\n';
		}
	} else {
		int i=0;
		int j=0;
		int lines = wcl(filename);
		for (int line=0; line<lines; ++line) {
			while (fileStr[i] != delim) {
				if (i >= fileSize)
					goto RETURN_SUCCESS;
				(*outStr)[j++] = fileStr[i++];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto RETURN_SUCCESS;
			(*outStr)[j++] = '\n';
		}
	}
	RETURN_SUCCESS:;
	free(fileStr);
	return 1;
	RETURN_ERROR:;
	free(fileStr);
	return 0;
}
#endif
