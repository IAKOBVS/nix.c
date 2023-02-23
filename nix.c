#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

#include "nix.h"

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
# define RESTRICT __restrict__
#elif defined(_MSC_VER)
# define RESTRICT __restrict
#else
# define RESTRICT
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
    #define ALWAYS_INLINE __forceinline inline
#else
    #define ALWAYS_INLINE inline
#endif

#if defined(__PRETTY_FUNCTION__)
	#define CURR_FUNC __PRETTY_FUNCTION__
#elif defined(__FUNCTION__)
	#define CURR_FUNC __FUNCTION__
#elif defined(__func__)
	#define CURR_FUNC __func__
#else
	#define CURR_FUNC ""
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__clang__) && __has_builtin(__builtin_expect))
  #define likely(x) __builtin_expect(!!(x), 1)
  #define unlikely(x) __builtin_expect(!!(x), 0)
#else
  #define likely(x) (x)
  #define unlikely(x) (x)
#endif

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define MIN_MALLOC 16000
#define CASE_WHITESPACE case '\n': case '\t': case '\r':

#define IF_ERROR(STATE, DO) if (STATE); else DO

inline int nixRev(char *RESTRICT dest, const char *RESTRICT src, const size_t srcLen)
{
	const char *end = src + srcLen - 1;
	while (end >= src)
		*dest++ = *end--;
	*dest = '\0';
	return 1;
}

inline int nixRevThis(char *RESTRICT dest, const size_t srcLen)
{
	char *src = malloc(srcLen);
	if (src);
	else goto ERROR;
	memcpy(src, dest, srcLen);
	const char *end = src + srcLen - 1;
	while (end >= src)
		*dest++ = *end--;
	free(src);
	*dest = '\0';
	return 1;

ERROR:
	perror("nixRevThis");
	return 0;
}

ALWAYS_INLINE size_t nixSizeOfFile(const char *RESTRICT filename)
{
	struct stat st;
	return (!stat(filename, &st) ? st.st_size : 0);
}

int nixTee(char *RESTRICT dest, const char *RESTRICT flag, const char *RESTRICT filename)
{
	FILE *RESTRICT fp = fopen(filename, flag);
	if (fp);
	else goto ERROR;
	fputs(dest, fp);
	fclose(fp);
	return 1;

ERROR:
	perror(CURR_FUNC);
	return 0;
}

int nixFind(char *RESTRICT dest, const char *RESTRICT dir)
{
	struct dirent *RESTRICT ep;
	DIR *RESTRICT dp = opendir(dir);
	if (dp);
	else goto ERROR;
	for (char *RESTRICT filename; (ep = readdir(dp)); ) {
		for (filename = ep->d_name; *filename; ++dest, ++filename)
			*dest = *filename;
		*dest++ = '\n';
	}
	closedir(dp);
	*--dest = '\0';
	return 1;

ERROR:
	perror(CURR_FUNC);
	return 0;
}

int nixFindAuto(char **RESTRICT dest, const char *RESTRICT dir)
{
	struct dirent *ep;
	DIR *RESTRICT dp = opendir(dir);
	if (dp);
	else goto ERROR;
	if ((*dest = malloc(MIN_MALLOC)));
	else goto ERROR;
	size_t mallocSize = MIN_MALLOC;
	size_t i = 0;
	char *RESTRICT filename;
	for (size_t tmpLen, tmpSize; (ep = readdir(dp)); ) {
		filename = ep->d_name;
		tmpLen = mallocSize + strlen(filename);
		tmpSize = mallocSize;
		if (tmpLen > mallocSize) {
			do {
				tmpSize *= 2;
			} while (tmpLen > mallocSize);
			if ((*dest = realloc(*dest, tmpSize)));
			else goto ERROR_FREE;
			mallocSize = tmpSize;
		}
		for ( ; *filename; ++i, ++filename)
			(*dest)[i] = *filename;
		(*dest)[i++] = '\n';
	}
	closedir(dp);
	if ((*dest = realloc(*dest, i + 1)));
	else goto ERROR_FREE;
	(*dest)[--i] = '\0';
	return i;

ERROR_FREE:
	free(*dest);
ERROR:
	perror(CURR_FUNC);
	return 0;
}

int nixHead(char *RESTRICT dest, const char *RESTRICT filename)
{
	FILE *RESTRICT fp = fopen(filename, "r");
	if (!fp);
	else goto ERROR;
	fgets(dest, 256, fp);
	fclose(fp);
	return 1;

ERROR:
	perror(CURR_FUNC);
	return 0;
}

#define NIX_CAT(FUNC_NAME, FREAD) \
int FUNC_NAME(char *RESTRICT dest, const char *RESTRICT filename, const size_t fileSize) \
{ \
	FILE *RESTRICT fp = fopen(filename, "r"); \
	if (!fp); \
	else goto ERROR; \
	if (FREAD(dest, 1, fileSize, fp)); \
	else goto ERROR_CLOSE; \
	fclose(fp); \
	dest[fileSize] = '\0'; \
	return 1; \
 \
ERROR_CLOSE: \
	fclose(fp); \
ERROR: \
	perror(CURR_FUNC); \
	return 0; \
}

NIX_CAT(nixCat, fread)
NIX_CAT(nixCatFast, fread_unlocked)

#define NIX_CAT_AUTO(FUNC_NAME, FREAD) \
int FUNC_NAME(char **RESTRICT dest, const char *RESTRICT filename) \
{ \
	const size_t fileSize = nixSizeOfFile(filename); \
	if ((fileSize)); \
	else goto ERROR; \
	FILE *RESTRICT fp = fopen(filename, "r"); \
	if (fp); \
	else goto ERROR; \
	if ((*dest = malloc(fileSize))); \
	else goto ERROR_CLOSE; \
	if (FREAD(*dest, 1, fileSize, fp)); \
	else goto ERROR_CLOSE_FREE; \
	fclose(fp); \
	(*dest)[fileSize] = '\0'; \
	return 1; \
 \
ERROR_CLOSE_FREE: \
	free(*dest); \
ERROR_CLOSE: \
	fclose(fp); \
ERROR: \
	perror(CURR_FUNC); \
	return 0; \
}

NIX_CAT_AUTO(nixCatAuto, fread)
NIX_CAT_AUTO(nixCatAutoFast, fread_unlocked)

ALWAYS_INLINE int nixCutFirst(char *RESTRICT dest, const char *RESTRICT src)
{
	for (;;) {
		switch (*src) {
		default:
			*dest++ = *src++;
			continue;
		case '\n':
		case '\t':
		case '\r':
		case ' ':
		case '\0':;
		}
		break;
	}
	*dest = '\0';
	return 1;
}

ALWAYS_INLINE int nixCutFirstDelim(char *RESTRICT dest, const char *RESTRICT src, const int delim)
{
	while ((*dest++ = *src++) != delim);
	*--dest = '\0';
	return 1;
}

ALWAYS_INLINE int nixCutLast(char *RESTRICT dest, const char *RESTRICT src, const size_t srcLen)
{
	src += srcLen - 1;
	for (;; --src) {
		switch (*src) {
		default:
			continue;
		case '\n':
		case '\t':
		case '\r':
		case ' ':;
		}
		break;
	}
	while ((*dest++ = *++src));
	return 1;
}

ALWAYS_INLINE int nixCutLastDelim(char *RESTRICT dest, const char *RESTRICT src, const size_t srcLen, const int delim)
{
	src += srcLen - 1;
	while (*src != delim) --src;
	while ((*dest++ = *++src));
	return 1;
}


ALWAYS_INLINE int nixCutDelim(char *RESTRICT dest, const char *RESTRICT src, int nStr, const int delim)
{
	while (nStr--) {
		while (*src != delim) ++src;
		while (*src == delim) ++src;
	}
	while ((*dest++ = *src++) != delim);
	*--dest = '\0';
	return 1;
}

ALWAYS_INLINE int nixCut(char *RESTRICT dest, const char *RESTRICT src, int nStr)
{
	for (;;) {
		switch (*src++) {
		default:
			continue;
		case '\0':
			break;
		case '\n':
		case '\t':
		case '\r':
		case ' ':
			if (--nStr)
				continue;
			break;
		}
		break;
	}
	for (;;) {
		switch (*src) {
		default:
			*dest++ = *src++;
			continue;
		case '\0':
		case '\n':
		case '\t':
		case '\r':
		case ' ':;
		}
		break;
	}
	*dest = '\0';
	return 1;
}

ALWAYS_INLINE int nixCountFunc(const char *RESTRICT src, const int c)
{
	int count = 0;
	while (*src)
		if (*src == c) ++count;
	return count;
}

ALWAYS_INLINE int nixCountUnlikelyFunc(const char *RESTRICT src, const int c)
{
	int count = 0;
	while (*src)
		if (*src++ != c);
		else ++count;
	return count;
}

ALWAYS_INLINE int nixCountDigit(const char *RESTRICT src)
{
	int count = 0;
	while (*src)
		if (isdigit(*src++)) ++count;
	return count;
}

ALWAYS_INLINE int nixCountAlpha(const char *RESTRICT src)
{
	int count = 0;
	while (*src)
		if (isalpha(*src++)) ++count;
	return count;
}

inline int nixWcWord(const char *RESTRICT src)
{
	int inWord = 0, count = 0;
	for ( ;; ++src) {
		switch (*src) {
		default:
			if (inWord);
			else inWord = 1;
			continue;
		case '\n':
		case '\t':
		case '\r':
		case ' ':
			if (inWord) {
				++count;
				inWord = 0;
			}
			continue;
		case '\0':;
		}
		break;
	}
	return inWord ? ++count : count;
}

inline int nixWcWordTilNl(const char *RESTRICT src)
{
	int inWord = 0, count = 0;
	for ( ;; ++src) {
		switch (*src) {
		default:
			if (inWord);
			else inWord = 1;
			continue;
		case '\t':
		case '\r':
		case ' ':
			if (inWord) {
				++count;
				inWord = 0;
			}
			continue;
		case '\0':
		case '\n':;
		}
		break;
	}
	return inWord ? ++count : count;
}

#define NIX_WCCHAR(FUNC_NAME, DELIM) \
inline int FUNC_NAME(const char *RESTRICT src) \
{ \
	int count = 0; \
	for ( ;; ++src) { \
		switch (*src) { \
		DELIM \
			continue; \
		default: \
			++count; \
		case '\0':; \
		} \
	} \
	return count; \
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

#define NIX_WCWORD(FUNC_NAME, DELIM) \
inline int FUNC_NAME(const char *RESTRICT src) \
{ \
	int inWord = 0, count = 0; \
	for ( ;; ++src) { \
		switch (*src) { \
		DELIM \
			if (inWord) { \
				++count; \
				inWord = 0; \
			} \
			continue; \
		default: \
			if (inWord); \
			else \
				inWord = 1; \
			continue; \
		case '\0':; \
		} \
		break; \
	} \
	return inWord ? ++count : count; \
}

/* NIX_WCWORD(nixWcWord, case ' ':) */
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
inline int FUNC_NAME(const char *RESTRICT src) \
{ \
	int inWord = 0, count = 0; \
	for ( ;; ++src) { \
		switch (*src) { \
		DELIM \
			if (inWord) { \
				++count; \
				inWord = 0; \
			} \
			continue; \
		default: \
			if (inWord); \
			else \
				inWord = 1; \
			continue; \
		case '\n': \
		case '\0':; \
		} \
		break; \
	} \
	return inWord ? ++count : count; \
}

/* NIX_WCWORD_TIL_NL(nixWcWordTilNl, case ' ':) */
NIX_WCWORD_TIL_NL(nixWcWordTilNlPipe, case '|':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlComma, case ',':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlDot, case '.':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlQuote, case '\'':)
NIX_WCWORD_TIL_NL(nixWcWordTilNlDoubleQuote, case '"':)
