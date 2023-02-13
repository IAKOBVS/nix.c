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

int nixWcNl(char *src)
{
	for (int count = 0;; ++src)
		switch (*src) {
		case '\0':
			return count;
		case '\n':
			++count;
		}
}

#define NIX_WCCHAR(FUNC_NAME, DELIM) \
int FUNC_NAME(char *src) \
{ \
	for (int count = 0 ;; ++src) \
		switch (*src) { \
		case '\0': \
			return count; \
		case ' ': \
		case '\n': \
		case '\t': \
		case '\r': \
		DELIM \
			continue; \
		default: \
			++count; \
		} \
}

NIX_WCCHAR(nixWcChar, )
NIX_WCCHAR(nixWcCharComma, case ',':)
NIX_WCCHAR(nixWcCharDot, case '.':)
NIX_WCCHAR(nixWcCharPipe, case '|':)
NIX_WCCHAR(nixWcCharQuote, case '\'':)
NIX_WCCHAR(nixWcCharDoubleQuote, case '"':)

#define NIX_WCWORD(FUNC_NAME, DELIM) \
int FUNC_NAME(char *src) \
{ \
	for (int inWord = 0, count = 0;; ++src) \
		switch (*src) { \
		case '\0': \
			return inWord ? ++count : count; \
		case ' ': \
		case '\n': \
		case '\t': \
		case '\r': \
		DELIM \
			if (inWord) { \
				++count; \
				inWord = 0; \
			} \
			continue; \
		default: \
			if (!inWord) \
				inWord = 1; \
		} \
}

NIX_WCWORD(nixWcWord, )
NIX_WCWORD(nixWcWordComma, case ',':)
NIX_WCWORD(nixWcWordPipe, case '|':)
NIX_WCWORD(nixWcWordDot, case '.':)
NIX_WCWORD(nixWcWordQuote, case '\'':)
NIX_WCWORD(nixWcWordDoubleQuote, case '"':)

#define NIX_WCWORDNL(FUNC_NAME, DELIM) \
int FUNC_NAME(char *src) \
{ \
	for (int inWord = 0, count = 0;; ++src) \
		switch (*src) { \
		case '\n': \
		case '\0': \
			return inWord ? ++count : count; \
		case  ' ': \
		case '\t': \
		case '\r': \
		DELIM \
			if (inWord) { \
				++count; \
				inWord = 0; \
			} \
		default: \
			if (!inWord) \
				inWord = 1; \
		} \
}

NIX_WCWORDNL(nixWcWordNl, )
NIX_WCWORDNL(nixWcWordNlPipe, case '|':)
NIX_WCWORDNL(nixWcWordNlComma, case ',':)
NIX_WCWORDNL(nixWcWordNlDot, case '.':)
NIX_WCWORDNL(nixWcWordNlQuote, case '\'':)
NIX_WCWORDNL(nixWcWordNlDoubleQuote, case '"':)

#define NIX_AWK(FUNC_NAME, DELIM_CASE, DELIM) \
int FUNC_NAME(int nStr, char *src, int srcLen, char **dest) \
{ \
	char buf[srcLen]; \
	int j = 0; \
	switch (nStr) { \
	case 0: \
		goto ERROR; \
	case 1: \
		for (;;) { \
			for (;;) { \
				switch (*src) { \
				case '\0': \
					goto SUCCESS; \
				case '\n': \
					goto SKIP_LOOPS_1; \
				DELIM_CASE \
					buf[j++] = *src++; \
					continue; \
				default: \
					++src; \
					continue; \
				} \
				break; \
			} \
			for (;;) { \
				switch (*src) { \
				case '\0': \
					goto SUCCESS; \
				case '\n': \
					goto SKIP_LOOPS_1; \
				default: \
					++src; \
					continue; \
				} \
			} \
SKIP_LOOPS_1: \
		buf[j++] = '\n'; \
		++src; \
		} \
		break; \
	default: \
		for (int n = 1;;) { \
			do { \
				for (;;) { \
					switch (*src) { \
					case '\0': \
						goto SUCCESS; \
					case '\n': \
						goto SKIP_LOOPS; \
					DELIM_CASE \
						break; \
					default: \
						++src; \
						continue; \
					} \
					break; \
				} \
				do { \
					++src; \
				} while (*src == DELIM); \
				++n; \
			} while (n < nStr); \
			buf[j++] = *src++; \
			for (;;) { \
				switch (*src) { \
				case '\0': \
					goto SUCCESS; \
				case '\n': \
					goto SKIP_LOOPS; \
				DELIM_CASE \
					break; \
				default: \
					buf[j++] = *src++; \
					continue; \
				} \
				break; \
			} \
			for (;;) { \
				switch (*src) { \
				case '\0': \
					goto SUCCESS; \
				case '\n': \
					goto SKIP_LOOPS; \
				default: \
					++src; \
					continue; \
				} \
				break; \
			} \
SKIP_LOOPS: \
		buf[j++] = '\n'; \
		++src; \
		} \
	} \
SUCCESS: \
	*dest = malloc(j + 1); \
	memcpy(*dest, buf, j); \
	(*dest)[j] = '\0'; \
	return j; \
 \
ERROR: \
	perror(""); \
	return 0; \
} \

NIX_AWK(nixAwk, case ' ':, ' ')
NIX_AWK(nixAwkComma, case ',':, ',')
NIX_AWK(nixAwkDot, case '.':, '.')
NIX_AWK(nixAwkQuote, case '"':, '"')
NIX_AWK(nixAwkDoubleQuote, case '"':, '"')
NIX_AWK(nixAwkPipe, case '|':, '|')

#define NIX_SPLIT(FUNC_NAME, DELIM) \
int FUNC_NAME(const char *str, char ***arr) \
{ \
	if (!(*arr = malloc(8 * sizeof(char *)))) return 0; \
	size_t j = 0; \
	for (size_t i;; ++j) { \
		i = 0; \
		int in = 0; \
		for (char buf[128];; ++str) { \
			switch (*str) { \
			default: \
				if (!in) \
					in = 1; \
				buf[i++] = *str; \
				continue; \
			case '\0': \
				if (!in) \
					return ++j; \
				if (!((*arr)[j] = malloc(i + 1))) goto ERROR_FREE; \
				memcpy((*arr)[j], buf, i); \
				(*arr)[j][i] = '\0'; \
				return ++j; \
			DELIM \
				if (!in) \
					continue; \
				if (!((*arr)[j] = malloc(i + 1))) goto ERROR_FREE; \
				memcpy((*arr)[j], buf, i); \
				(*arr)[j][i] = '\0'; \
			} \
			break; \
		} \
	} \
ERROR_FREE: \
	if (j) \
		for (int i = 0; i < j; ++i) \
			free((*arr)++); \
	free(*arr); \
	perror(""); \
	return 0; \
}

NIX_SPLIT(nixSplit, case ' ': case '\n': case '\t': case '\r':) 
NIX_SPLIT(nixSplitNl, case '\n':)

void nixSplitFree(char **arr, int arrLen)
{
	for (size_t i = 0; i < arrLen; ++i)
		free(arr[i]);
	free(arr);
}
