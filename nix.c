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

int nixFind(char *dir, char dest[])
{
	struct dirent *ep;
	DIR *dp = opendir(dir);
	if (!dp) goto ERROR;
	int i = 0;
	while ((ep = readdir(dp))) {
		for (char *filename = ep->d_name; *filename; ++i, ++filename)
			dest[i] = *filename;
		dest[i] = '\n';
		++i;
	}
	closedir(dp);
	dest[--i] = '\0';
	return i;

ERROR:
	perror("");
	return 0;
}

int nixHead(const char *filename, char dest[])
{
	FILE *fp = fopen(filename, "r");
	if (!fp) goto ERROR;
	fgets(dest, 256, fp);
	fclose(fp);
	return 1;

ERROR:
	perror("");
	return 0;
}

#define NIX_CAT(FUNC_NAME, FREAD) \
int FUNC_NAME(const char *filename, size_t fileSize, char dest[]) \
{ \
	FILE *fp = fopen(filename, "r"); \
	if (!fp) goto ERROR; \
	if (!FREAD(dest, 1, fileSize, fp)) goto ERROR_CLOSE; \
	fclose(fp); \
	dest[fileSize] = '\0'; \
	return fileSize; \
 \
ERROR_CLOSE: \
	fclose(fp); \
ERROR: \
	perror(""); \
	return 0; \
}

NIX_CAT(nixCat, fread)
NIX_CAT(nixCatFast, fread_unlocked)

int nixRev(char dest[], char *src, int srcLen)
{
	src += srcLen - 1;
	int i = 0;
	for ( ; srcLen; ++i, --src, --srcLen)
		dest[i] = *src;
	dest[i] = '\0';
	return i;
}

int nixGetLastWord(char dest[], char *src, int srcLen)
{
	src += srcLen - 1;
	for (;;) {
		switch (*src) {
		default:
			--src;
			continue;
		case '\n':
		case '\t':
		case '\r':
		case ' ':
			++src;
		}
		break;
	}
	int i = 0;
	for ( ; *src; ++i, ++src)
		dest[i] = *src;
	dest[i] = '\0';
	return i;
}

int nixCut(int nStr, char *src, char dest[])
{
	if (nStr == 1) {
		for (int i = 0;; )
			switch (*src) {
			case '\0':
			case ' ':
			case '\n':
			case '\t':
			case '\r':
				return i;
			default:
				dest[i] = *src;
				++i, ++src;
			}
	} else {
		while (nStr) {
			switch (*src) {
			default:
				continue;
			case '\0':
			case ' ':
			case '\n':
			case '\t':
			case '\r':
				--nStr;
			}
		}
		for (int i = 0;; )
			switch (*src) {
			case '\0':
			case ' ':
			case '\n':
			case '\t':
			case '\r':
				return i;
			default:
				dest[i] = *src;
				++i, ++src;
			}
	}
}

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
					buf[j] = *src; \
					++j, ++src; \
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
		buf[j] = '\n'; \
		++j, ++src; \
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
			buf[j] = *src; \
			++j, ++src; \
			for (;;) { \
				switch (*src) { \
				case '\0': \
					goto SUCCESS; \
				case '\n': \
					goto SKIP_LOOPS; \
				case DELIM: \
					break; \
				default: \
					buf[j] = *src; \
					++j, ++src; \
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
		buf[j] = '\n'; \
		++j, ++src; \
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
				buf[i] = *str; \
				++i; \
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
		for (int i = 0; i < j; ++i, ++(*arr)) \
			free(*arr); \
	free(*arr); \
	perror(""); \
	return 0; \
}

NIX_SPLIT(nixSplitWords, case '\n': case '\t': case '\r': case ' ':) 
NIX_SPLIT(nixSplitNl, case '\n':)

void nixSplitFree(char **arr, int arrLen)
{
	for ( ; --arrLen; ++(*arr))
		free(*arr);
	free(arr);
}

#define NIX_WC(FUNC_NAME, DELIM) \
int FUNC_NAME(char *src) \
{ \
	for (int count = 0;; ) \
		switch (*src) { \
		case '\0': \
			return count; \
		DELIM \
			++src; \
			++count; \
		} \
} \

NIX_WC(nixWcNl, case '\n':)
NIX_WC(nixWcSpace, case ' ':)
NIX_WC(nixWcTab, case '\t':)
NIX_WC(nixWcNonWords, case '\n': case '\t': case '\r': case ' ':)

#define NIX_WCCHAR(FUNC_NAME, DELIM) \
int FUNC_NAME(char *src) \
{ \
	for (int count = 0;; ++src) \
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

NIX_WCWORD_TIL_NL(nixWcWordTilNl, case ' ':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlPipe, case '|':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlComma, case ',':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlDot, case '.':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlQuote, case '\'':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlDoubleQuote, case '"':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlTab, case '\t':)

/* NIX_WCWORD_TILL_NL(nixWcWordTillNlAlpha, case '\t': case '\r': case ' ': continue; default:) */
/* NIX_WCWORD_TILL_NL(nixWcWordTillNlAlphaComma, case '\t': case '\r': case ',': continue; default:) */
/* NIX_WCWORD_TILL_NL(nixWcWordTillNlAlphaPipe, case '\t': case '\r': case '|': continue; default:) */
/* NIX_WCWORD_TILL_NL(nixWcWordTillNlAlphaDot, case '\t': case '\r': case '.': continue; default:) */
/* NIX_WCWORD_TILL_NL(nixWcWordTillNlAlphaQuote, case '\t': case '\r': case '\'': continue; default:) */
/* NIX_WCWORD_TILL_NL(nixWcWordTillNlAlphaDoubleQuote, case '\t': case '\r': case '"': continue; default:) */
