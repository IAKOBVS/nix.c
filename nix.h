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
	if (!file)
		return 0;
	int c;
	int count=0;
	do {
		c = fgetc(file);
		if (c == '\n')
			count++;
	} while (c != EOF);
	fclose(file);
	return count;
}

static void cat(char *filename, char **outFile)
{
	int fileSize = sizeOfFile(filename);
	*outFile = malloc(fileSize);
	FILE *file = fopen(filename, "r");
	if (!file)
		return;
	fread(*outFile, 1, fileSize, file);
	if (ferror(file))
		return;
	fclose(file);
}

static void awk(char delim, int nStr, char *filename, char **outStr)
{
	int fileSize = sizeOfFile(filename);
	char *fileStr;
	*outStr = malloc(fileSize);
	cat(filename, &fileStr);
	if (!fileStr)
		return;
	for (int lineNum = wcl(filename), i=0, j=0, curr=0; curr<lineNum; ++curr) {
		if (nStr > 1)
			for (int n=1; n<nStr; ++n) {
				while (fileStr[i] != delim && i<fileSize)
					++i;
				while (fileStr[i] == delim && i<fileSize)
					++i;
			}
		/* assigns matched column to string */
		while (fileStr[i] != delim && i<fileSize)
			*outStr[j++] = fileStr[i++];
		while (fileStr[i] != '\n')
			if (i++<fileSize)
				return;
		*outStr[j] = '\n';
	}
	free(fileStr);
}

/* static void awk(char delim, char *inStr, int nStr, char *outStr) */
/* { */
/* 	int i = 0; */
/* 	while (inStr[i] == delim && inStr[i]) ++i; */
/* 	if (nStr > 1) */
/* 		for (int nDelim = 1; nDelim<nStr; ++nDelim) { */
/* 			while (inStr[i] != delim && inStr[i]) */
/* 				++i; */
/* 			while (inStr[i] == delim && inStr[i]) */
/* 				++i; */
/* 		} */
/* 	for (int j = 0; inStr[i] != delim && inStr[i]; ++i, ++j) */
/* 		outStr[j] = inStr[i]; */
/* } */

/* static void awkMult(char delim, int nStr, char *filename, char **outStr) */
/* { */
/* 	int fileSize = sizeOfFile(filename); */
/* 	char *fileBuff; */
/* 	*outStr = malloc(fileSize); */
/* 	cat(filename, &fileBuff); */
/* 	if (!fileBuff) */
/* 		return; */
/* 	for (int lineNum = wcl(filename), i=0, j=0, curr=0; curr<lineNum; ++curr) { */
/* 		while (fileBuff[i] == delim && fileBuff[i] != '\n' && i<fileSize) */
/* 			++i; */
/* 		if (nStr > 1) */
/* 			for (int nDelim = 1; nDelim<nStr; ++nDelim) { */
/* 				while (fileBuff[i] != delim && fileBuff[i] != '\n' && i<fileSize) */
/* 					++i; */
/* 				while (fileBuff[i] == delim && fileBuff[i] != '\n' && i<fileSize) */
/* 					++i; */
/* 			} */
/* 		while (fileBuff[i] != '\n') { */
/* 			if (i >= fileSize) */
/* 				return; */
/* 			else if (fileBuff[i] != delim) */
/* 				(*outStr)[j++] = fileBuff[i]; */
/* 			else */
/* 				break; */
/* 			++i; */
/* 		} */
/* 		(*outStr)[j++] = '\n'; */
/* 	} */
/* 	free(fileBuff); */
/* } */

#endif
