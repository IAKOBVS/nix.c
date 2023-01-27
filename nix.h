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

/* flags: */
/* 'l' =  line */
/* 'w' =  word */
static int wc(char flag, char *filename)
{
	FILE *fd = fopen(filename, "r");
	if (!fd) {
		fprintf(stderr, "wc %c: fd is 0\n", flag);
		return 0;
	}
	int cnt;
	switch (flag) {
	case 'l':
		cnt=0;
		for (int c = fgetc(fd); c != EOF; c = fgetc(fd))
			if (c == '\n')
				++cnt;
		break;
	case 'w':
		cnt=0;
		for (int c = fgetc(fd); c != EOF; c = fgetc(fd))
			switch (c) {
			case ' ':
			case '\n':
				continue;
			default:
				++cnt;
			}
		break;
	default:
		fprintf(stderr, "wc %c: invalid flag (not 'l' nor 'w')\n", flag);
		return 0;
	}
	fclose(fd);
	return cnt;
}

/* editMode: */
/* 'w' = overwrite */
/* 'a' = append */
static int tee(char *editMode, char *inStr, char *filename)
{
	FILE *fd = fopen(filename, editMode);
	if (!fd) {
		fprintf(stderr, "tee: fopen failed\nuse \"a\" to append and \"w\" for overwrite");
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
	do {
		if (!*outStr) {
			fprintf(stderr, "cat: *outStr is 0\n");
			break;
		}
		fread(*outStr, 1, fileSize, fd);
		if (ferror(fd)) {
			fprintf(stderr, "cat: ferror(fd) is 0\n");
			break;
		}
		fclose(fd);
		return fileSize;
	} while (0);
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
	switch (nStr) {
	case 1:
		{
		int i=0;
		int j=0;
		int lines = wc('l', filename);
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
		break;
		}
	default:
		{
		int i=0;
		int j=0;
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
				(*outStr)[j++] = fileStr[i];
			}
			for ( ; fileStr[i] != '\n'; ++i)
				if (i >= fileSize)
					goto RETURN_SUCCESS;
			(*outStr)[j++] = '\n';
		}
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
