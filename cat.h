#ifndef CAT_H_DEF
#define CAT_H_DEF
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
int sizeOfFile(char *filename)
{
	struct stat fileInfo;
	stat(filename, &fileInfo);
	return fileInfo.st_size;
}

int cat(char *filename, char **outFile)
{
	int fileSize = sizeOfFile(filename);
	*outFile = malloc(fileSize+1);
	FILE *file;
	file = fopen(filename, "r");
	if (!file)
		return 0;
	fread(*outFile, fileSize+1, 1, file);
	if (ferror(file))
		return 0;
	fclose(file);
	return 1;
}
#endif
