#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
/* #include <assert.h> */

#include "nix.h"
#include "/home/james/c/jString/jstr.h"
#include "/home/james/c/jArray/jarr.h"

int sizeOfFile(char *filename)
{
	struct stat st;
	return (!stat(filename, &st) ? st.st_size : 0);
}

int tee(char *flag, char *inStr, char *filename)
{
	FILE *fp = fopen(filename, flag);
	if (fp) {
		fputs(inStr, fp);
		fclose(fp);
		return 1;
	}
	perror("");
	return 0;
}

int findDir(char *dir, char **dest)
{
	struct dirent *ep;
	DIR *dp = opendir(dir);
	if (!dp) goto ERROR;
	if (!(*dest = malloc(3200))) goto ERROR;
	for (int i = 0; (ep = readdir(dp)); ) {
		memcpy(*dest, ep->d_name, sizeof ep->d_name);
		*dest[i += strlen(ep->d_name)] = ' ';
	}
	return 3200;

ERROR:
	perror("");
	return 0;
}

int head(char *filename, char **dest)
{
	char buf[512];
	int destLen;
	FILE *fp = fopen(filename, "r");
	if (!fp) goto ERROR;
	fgets(buf, 512, fp);
	if (ferror(fp)) goto ERROR_CLOSE;
	fclose(fp);
	if (!(*dest = malloc((destLen = strlen(buf)) + 1))) goto ERROR;
	memcpy(*dest, buf, destLen);
	(*dest)[destLen] = '\0';
	return destLen;

ERROR_CLOSE:
	fclose(fp);
ERROR:
	perror("");
	return 0;
}

int cat(char *filename, char **dest)
{
	FILE *fp = fopen(filename, "r");
	if (!fp) goto ERROR;
	int fileSize = sizeOfFile(filename);
	if (!(*dest = malloc(fileSize + 1))) goto ERROR_CLOSE;
	if (fileSize != fread(*dest, 1, fileSize, fp)) goto ERROR_CLOSE_FREE;
	fclose(fp);
	(*dest)[fileSize] = '\0';
	return fileSize;

ERROR_CLOSE_FREE:
	free(*dest);
ERROR_CLOSE:
	fclose(fp);
ERROR:
	perror("");
	return 0;
}

int wcl(char *src)
{
	for (int count = 0;; ++src)
		switch (*src) {
		case '\0':
			return count;
		case '\n':
			++count;
		}
}

int wcc(char *src)
{
	for (int count = 0 ;; ++src)
		switch (*src) {
		case '\0':
			return count;
		case ' ':
		case '\n':
		case '\t':
		case '\r':
			continue;
		default:
			++count;
		}
}

int wcw(char *src)
{
	for (int inWord = 0, count = 0;; ++src)
		switch (*src) {
		case '\0':
			if (inWord)
				return ++count;
			return count;
		case ' ':
		case '\n':
		case '\t':
		case '\r':
			if (inWord) {
				++count;
				inWord = 0;
			}
			continue;
		default:
			if (!inWord)
				inWord = 1;
		}
}

int awk(char delim, int nStr, char *src, int srcLen, char **dest)
{
	char buf[srcLen];
	int j = 0;
	switch (nStr) {
	case 0:
		goto ERROR;
	case 1:
		for (;;) {
			for (;;) {
				switch (*src) {
				case '\0':
					goto SUCCESS;
				case '\n':
					goto SKIP_LOOPS_1;
				default:
					if (*src != delim) {
						buf[j++] = *src++;
						continue;
					}
				}
				break;
			}
			for (;;) {
				switch (*src) {
				case '\0':
					goto SUCCESS;
				case '\n':
					goto SKIP_LOOPS_1;
				default:
					++src;
					continue;
				}
			}
SKIP_LOOPS_1:
		buf[j++] = '\n';
		++src;
		}
		break;
	default:
		for (int n = 1;;) {
			do {
				for (;;) {
					switch (*src) {
					case '\0':
						goto SUCCESS;
					case '\n':
						goto SKIP_LOOPS;
					default:
						if (*src != delim) {
							++src;
							continue;
						}
					}
					break;
				}
				do {
					++src;
				} while (*src == delim);
				++n;
			} while (n < nStr);
			buf[j++] = *src++;
			for (;;) {
				switch (*src) {
				case '\0':
					goto SUCCESS;
				case '\n':
					goto SKIP_LOOPS;
				default:
					if (*src != delim) {
						buf[j++] = *src++;
						continue;
					}
				}
				break;
			}
			for (;;) {
				switch (*src) {
				case '\0':
					goto SUCCESS;
				case '\n':
					goto SKIP_LOOPS;
				default:
					++src;
					continue;
				}
				break;
			}
SKIP_LOOPS:
		buf[j++] = '\n';
		++src;
		}
	}
SUCCESS:
	*dest = malloc(j + 1);
	memcpy(*dest, buf, j);
	(*dest)[j] = '\0';
	return j;

ERROR:
	perror("");
	return 0;
}

int awkFile(char delim, int nStr, char *filename, char **dest)
{
	char *fileStr;
	int fileSize = cat(filename, &fileStr);
	if (fileSize) {
		int ret = awk(delim, nStr, fileStr, fileSize, dest);
		free(fileStr);
		if (ret)
			return ret;
		free(*dest);
	}
	perror("");
	return 0;
}
