#ifndef CAT_H_DEF
#define CAT_H_DEF
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
	*outFile = malloc(fileSize+1);
	FILE *file = fopen(filename, "r");
	if (!file)
		return;
	fread(*outFile, fileSize+1, 1, file);
	if (ferror(file))
		return;
	fclose(file);
	*outFile[fileSize+1] = '\0';
}

static void awk(char delim, char *inStr, int nStr, char *outStr)
{
	int i = 0;
	while (inStr[i] == delim && inStr[i])
		++i;
	if (nStr > 1)
		for (int nDelim = 1; nDelim<nStr; ++nDelim) {
			while (inStr[i] != delim && inStr[i])
				++i;
			while (inStr[i] == delim && inStr[i])
				++i;
		}
	int j;
	for (j = 0; inStr[i] != delim && inStr[i]; ++i, ++j)
		outStr[j] = inStr[i];
	outStr[++j] = '\0';
}

static void awkFile(char delim, int nStr, char *file, char *outStr)
{

}
#endif
