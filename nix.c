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

int nixTee(const char *flag, char *src, const char *filename)
{
	FILE *fp = fopen(filename, flag);
	if (fp) {
		fputs(src, fp);
		fclose(fp);
		return 1;
	}
	perror("");
	return 0;
}

int nixfindDir(char *dir, char dest[])
{
	struct dirent *ep;
	DIR *dp = opendir(dir);
	if (!dp) return 0;
	int i = 0;
	while ((ep = readdir(dp)))
		for (int j = 0;; ) {
			switch (ep->d_name[j]) {
			default:
				dest[i++] = (ep->d_name)[j++];
				continue;
			case '\0':
				dest[i++] = '\n';
			}
			break;
		}
	dest[i] = '\0';
	closedir(dp);
	return i;
}

int nixHead(const char *filename, char dest[])
{
	int destLen;
	FILE *fp = fopen(filename, "r");
	if (!fp) goto ERROR;
	fgets(dest, 512, fp);
	fclose(fp);
	if (ferror(fp)) goto ERROR;
	dest[destLen] = '\0';
	return destLen;

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
		DELIM \
			continue; \
		default: \
			++count; \
		} \
}

NIX_WCCHAR(nixWcChar, case ' ':)
NIX_WCCHAR(nixWcCharComma, case ',':)
NIX_WCCHAR(nixWcCharDot, case '.':)
NIX_WCCHAR(nixWcCharPipe, case '|':)
NIX_WCCHAR(nixWcCharQuote, case '\'':)
NIX_WCCHAR(nixWcCharDoubleQuote, case '"':)
NIX_WCCHAR(nixWcCharTab, case '\t':)
NIX_WCCHAR(nixWcCharNl, case '\n':)

NIX_WCCHAR(nixWcCharAlpha, case '\n': case '\t': case '\r': case ' ':)
NIX_WCCHAR(nixWcCharAlphaComma, case '\n': case '\t': case '\r': case ',':)
NIX_WCCHAR(nixWcCharAlphaDot, case '\n': case '\t': case '\r': case '.':)
NIX_WCCHAR(nixWcCharAlphaPipe, case '\n': case '\t': case '\r': case '|':)
NIX_WCCHAR(nixWcCharAlphaQuote, case '\n': case '\t': case '\r': case '\'':)
NIX_WCCHAR(nixWcCharAlphaDoubleQuote, case '\n': case '\t': case '\r': case '"':)

/* case '\\n': case '\\t': case '\\r': */

#define NIX_WCWORD(FUNC_NAME, DELIM) \
int FUNC_NAME(char *src) \
{ \
	for (int inWord = 0, count = 0;; ++src) \
		switch (*src) { \
		case '\0': \
			return inWord ? ++count : count; \
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

NIX_WCWORD(nixWcWord, case ' ':)
NIX_WCWORD(nixWcWordComma, case ',':)
NIX_WCWORD(nixWcWordPipe, case '|':)
NIX_WCWORD(nixWcWordDot, case '.':)
NIX_WCWORD(nixWcWordQuote, case '\'':)
NIX_WCWORD(nixWcWordDoubleQuote, case '"':)
NIX_WCWORD(nixWcWordTab, case '\t':)
NIX_WCWORD(nixWcWordNl, case '\n':)

NIX_WCWORD(nixWcWordAlpha, case '\n': case '\t': case '\r': case ' ':)
NIX_WCWORD(nixWcWordAlphaComma, case '\n': case '\t': case '\r': case ',':)
NIX_WCWORD(nixWcWordAlphaPipe, case '\n': case '\t': case '\r': case '|':)
NIX_WCWORD(nixWcWordAlphaDot, case '\n': case '\t': case '\r': case '.':)
NIX_WCWORD(nixWcWordAlphaQuote, case '\n': case '\t': case '\r': case '\'':)
NIX_WCWORD(nixWcWordAlphaDoubleQuote, case '\n': case '\t': case '\r': case '"':)

#define NIX_WCWORD_TIL_NL(FUNC_NAME, DELIM) \
int FUNC_NAME(char *src) \
{ \
	for (int inWord = 0, count = 0;; ++src) \
		switch (*src) { \
		case '\n': \
		case '\0': \
			return inWord ? ++count : count; \
		case DELIM: \
			if (inWord) { \
				++count; \
				inWord = 0; \
			} \
		default: \
			if (!inWord) \
				inWord = 1; \
		} \
}

NIX_WCWORD_TIL_NL(nixWcWordTilNl, ' ')
NIX_WCWORD_TIL_NL(nixWcWordTilNlPipe, '|')
NIX_WCWORD_TIL_NL(nixWcWordTilNlComma, ',')
NIX_WCWORD_TIL_NL(nixWcWordTilNlDot, '.')
NIX_WCWORD_TIL_NL(nixWcWordTilNlQuote, '\'')
NIX_WCWORD_TIL_NL(nixWcWordTilNlDoubleQuote, '"')
NIX_WCWORD_TIL_NL(nixWcWordTilNlTab, '\t')

#define NIX_AWK(FUNC_NAME, DELIM) \
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
				case DELIM: \
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
					case DELIM: \
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
				case DELIM: \
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

NIX_AWK(nixAwk, ' ')
NIX_AWK(nixAwkComma, ',')
NIX_AWK(nixAwkDot, '.')
NIX_AWK(nixAwkQuote, '"')
NIX_AWK(nixAwkDoubleQuote,  '"')
NIX_AWK(nixAwkPipe,  '|')
NIX_AWK(nixAwkTab, '\t')

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

NIX_SPLIT(nixSplitWords, case '\n': case '\t': case '\r': case ' ':) 
NIX_SPLIT(nixSplitNl, case '\n':)
void nixSplitFree(char **arr, int arrLen)
{
	for (size_t i = 0; i < arrLen; ++i)
		free(arr[i]);
	free(arr);
}

int nixGetLastWord(char dest[], char *src)
{
	for (int i = 0, loop = nixWcWordAlpha(src) - 1;; )
		switch (*src) {
		case '\0':
			return 0;
		case ' ':
			--loop;
			++src;
			continue;
		default:
			if (loop) {
				++src;
				continue;
			}
			do {
				dest[i++] = *src++;
			} while (*src);
			dest[i] = '\0';
			return i;
		}
}
