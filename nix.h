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

static int wcl(char *filename)
{
	FILE *file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "cat: file is 0");
		return 0;
	}
	int count = 0;
	for (int c; c != EOF; c = fgetc(file))
		if (c == '\n')
			++count;
	fclose(file);
	return count;
}

static int cat(char *filename, char **outFile)
{
	/* outFile must be freed after used */
	FILE *file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "cat: fopen failed");
		return 0;
	}
	int fileSize = sizeOfFile(filename);
	*outFile = malloc(fileSize);
	if (!*outFile) {
		fprintf(stderr, "cat: *outFile is 0");
		goto RETURN_ERROR;
	}
	fread(*outFile, 1, fileSize, file);
	if (ferror(file)) {
		fprintf(stderr, "cat: ferror(file) is 0");
		goto RETURN_ERROR;
	}
	fclose(file);
	return fileSize;
	RETURN_ERROR:;
	fclose(file);
	return 0;
}

static int awk(char delim, int nStr, char *filename, char **outStr)
{
	/* fileStr must be freed */
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (!fileSize) {
		fprintf(stderr, "awk: fileSize is 0");
		goto RETURN_ERROR;
	}
	*outStr = malloc(fileSize);
	if (!*outStr) {
		fprintf(stderr, "awk: *outStr is 0");
		goto RETURN_ERROR;
	}
	if (nStr > 1) {
		int i=0;
		int lineNum = wcl(filename);
		for (int curr=0; curr<lineNum; ++curr) {
			for (int n=1; n<nStr; ++n) {
				while (fileStr[i++] != delim)
					if (i < fileSize)
						goto RETURN_SUCCESS;
				while (fileStr[i] == delim)
					++i;
			}
			int j=0;
			while (fileStr[i++] != delim) {
				if (i >= fileSize)
					goto RETURN_SUCCESS;
				(*outStr)[j++] = fileStr[i];
			}
			while (fileStr[i++] != '\n')
				if (i >= fileSize)
					goto RETURN_SUCCESS;
			(*outStr)[j++] = '\n';
		}
	} else {
		int i=0;
		int j=0;
		int lineNum = wcl(filename);
		for (int curr=0; curr<lineNum; ++curr) {
			while (fileStr[i++] != delim) {
				if (i<fileSize)
					goto RETURN_SUCCESS;
				(*outStr)[j++] = fileStr[i];
			}
			while (fileStr[i++] != '\n')
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
