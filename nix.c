#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>

#include "nix.h"
#include "/home/james/c/jString/jstr.h"
#include "/home/james/c/jArray/jarr.h"

int nixSizeOfFile(const char *filename)
{
	struct stat st;
	return (!stat(filename, &st) ? st.st_size : 0);
}

int nixTee(const char *flag, char *inStr, const char *filename)
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

int nixfindDir(char *dir, char **dest)
{
	struct dirent *ep;
	DIR *dp = opendir(dir);
	if (!dp) return 0;
	char buf[3200];
	int i = 0;
	while ((ep = readdir(dp)))
		for (int j = 0;; ) {
			switch (ep->d_name[j]) {
			default:
				buf[i++] = (ep->d_name)[j++];
				continue;
			case '\0':
				buf[i++] = '\n';
			}
			break;
		}
	*dest = malloc(i + 1);
	memcpy(*dest, buf, i);
	(*dest)[i] = '\0';
	closedir(dp);
	return i;
}

int nixHead(const char *filename, char **dest)
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

int nixCat(const char *filename, char **dest)
{
	FILE *fp = fopen(filename, "r");
	if (!fp) goto ERROR;
	int fileSize = nixSizeOfFile(filename);
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

int nixWcl(char *src)
{
	for (int count = 0;; ++src)
		switch (*src) {
		case '\0':
			return count;
		case '\n':
			++count;
		}
}

int nixWcc(char *src)
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

int nixWcw(char *src)
{
	for (int inWord = 0, count = 0;; ++src)
		switch (*src) {
		case '\0':
			return inWord ? ++count : count;
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

int nixAwk(char delim, int nStr, char *src, int srcLen, char **dest)
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

int nixAwkFile(char delim, int nStr, const char *filename, char **dest)
{
	char *fileStr;
	int fileSize = nixCat(filename, &fileStr);
	if (fileSize) {
		int ret = nixAwk(delim, nStr, fileStr, fileSize, dest);
		free(fileStr);
		if (ret)
			return ret;
		free(*dest);
	}
	perror("");
	return 0;
}

int nixTokenize(const char *str, char ***arr)
{
	if (!(*arr = malloc(8 * sizeof(char *)))) return 0;
	size_t j = 0;
	for (size_t i;; ++j) {
		i = 0;
		int in = 0;
		for (char buf[128];; ++str) {
			switch (*str) {
			default:
				if (!in)
					in = 1;
				buf[i++] = *str;
				continue;
			case '\0':
				if (!in)
					return ++j;
				if (!((*arr)[j] = malloc(i + 1)))
					goto ERROR_FREE;
				memcpy((*arr)[j], buf, i);
				(*arr)[j][i] = '\0';
				return ++j;
			case '\n':
			case '\t':
			case '\r':
			case ' ':
				if (!in)
					continue;
				if (!((*arr)[j] = malloc(i + 1))) goto ERROR_FREE;
				memcpy((*arr)[j], buf, i);
				(*arr)[j][i] = '\0';
			}
			break;
		}
	}
ERROR_FREE:
	if (j)
		for (int i = 0; i < j; ++i)
			free((*arr)++);
	free(*arr);
	perror("");
	return 0;
}

/* void nixTokenizeFree(char **arr, int arrLen) */
/* { */
/* 	for (size_t i = 0; i < arrLen; ++i) */
/* 		free(arr[i]); */
/* 	free(arr); */
/* } */

/* int nixTokenizeLine(const char *str, char ****arr) */
/* { */
/* 	size_t k = 0; */
/* 	if (!(*arr = malloc(8 * sizeof(char *)))) return 0; */
/* 	if (!(*arr[k] = malloc(8 * sizeof(char *)))) return 0; */
/* 	size_t j = 0; */
/* 	for (size_t i;; ++j) { */
/* 		i = 0; */
/* 		int in = 0; */
/* 		for (char buf[128];; ++str) { */
/* 			switch (*str) { */
/* 			default: */
/* 				if (!in) */
/* 					in = 1; */
/* 				buf[i++] = *str; */
/* 				continue; */
/* 			case '\0': */
/* 				if (!in) */
/* 					return ++j; */
/* 				if (!((*arr)[j] = malloc(i + 1))) */
/* 					goto ERROR_FREE; */
/* 				memcpy((*arr)[j], buf, i); */
/* 				arr[k][j][i] = '\0'; */
/* 				return ++j; */
/* 			case '\n': */
/* 				++k; */
/* 			case '\t': */
/* 			case '\r': */
/* 			case ' ': */
/* 				if (!in) */
/* 					continue; */
/* 				if (!((*arr)[j] = malloc(i + 1))) */
/* 					goto ERROR_FREE; */
/* 				memcpy((*arr)[j], buf, i); */
/* 				(*arr)[j][i] = '\0'; */
/* 			} */
/* 			break; */
/* 		} */
/* 	} */
/* ERROR_FREE: */
/* 	if (j) */
/* 		for (int i = 0; i < j; ++i) */
/* 			free((*arr)++); */
/* 	free(*arr); */
/* 	perror(""); */
/* 	return 0; */
/* } */
