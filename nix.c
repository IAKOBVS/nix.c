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
	memcpy(src, dest, srcLen);
	const char *end = src + srcLen - 1;
	while (end >= src)
		*dest++ = *end--;
	free(src);
	*dest = '\0';
	return 1;
}

ALWAYS_INLINE size_t nixSizeOfFile(const char *RESTRICT filename)
{
	struct stat st;
	return (!stat(filename, &st) ? st.st_size : 0);
}

int nixTee(char *RESTRICT dest, const char *RESTRICT flag, const char *RESTRICT filename)
{
	FILE *RESTRICT fp = fopen(filename, flag);
	if (unlikely(!fp))
		goto ERROR;
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
	if (unlikely(!dp))
		goto ERROR;
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
	if (unlikely(!dp))
		goto ERROR;
	if (unlikely(!(*dest = malloc(MIN_MALLOC))))
		goto ERROR;
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
			if (unlikely(!(*dest = realloc(*dest, tmpSize))))
				goto ERROR_FREE;
			mallocSize = tmpSize;
		}
		for ( ; *filename; ++i, ++filename)
			(*dest)[i] = *filename;
		(*dest)[i++] = '\n';
	}
	closedir(dp);
	if (unlikely(!(*dest = realloc(*dest, i + 1))))
		goto ERROR_FREE;
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
	if (unlikely(!fp))
		goto ERROR;
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
	if (unlikely(!fp)) \
		goto ERROR; \
	if (unlikely(!FREAD(dest, 1, fileSize, fp))) \
		goto ERROR_CLOSE; \
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
	if (unlikely(!fileSize)) \
		goto ERROR; \
	FILE *RESTRICT fp = fopen(filename, "r"); \
	if (unlikely(!fp)) \
		goto ERROR; \
	if (unlikely(!(*dest = malloc(fileSize)))) \
		goto ERROR_CLOSE; \
	if (unlikely(!FREAD(*dest, 1, fileSize, fp))) \
		goto ERROR_CLOSE_FREE; \
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

inline int nixCutFirst(char *RESTRICT dest, const char *RESTRICT src)
{
	for (;;)
		switch (*src) {
		case '\n':
		case '\t':
		case '\r':
		case ' ':
			*dest = '\0';
			return 1;
		default:
			*dest++ = *src++;
		}
}

ALWAYS_INLINE int nixCutFirstDelim(char *RESTRICT dest, const char *RESTRICT src, const int delim)
{
	while ((*dest++ = *src++) != delim);
	*--dest = '\0';
	return 1;
}

inline int nixCutLast(char *RESTRICT dest, const char *RESTRICT src, const size_t srcLen)
{
	src += srcLen - 1;
	for (;; --src)
		switch (*src) {
		case '\n':
		case '\t':
		case '\r':
		case ' ':
			++src;
			while ((*dest++ = *src++));
			return 1;
		}
}

ALWAYS_INLINE int nixCutLastDelim(char *RESTRICT dest, const char *RESTRICT src, const size_t srcLen, const int delim)
{
	src += srcLen - 1;
	while (*--src != delim);
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

inline int nixCut(char *RESTRICT dest, const char *RESTRICT src, int nStr)
{
	if (nStr > 1)
		for ( ; nStr; ++src)
			switch (*src) {
			case '\0':
			case ' ':
			case '\n':
			case '\t':
			case '\r':
				--nStr;
				for (;;)
					switch (*src) {
					case '\0':
					case ' ':
					case '\n':
					case '\t':
					case '\r':
						return 1;
					default:
						*dest++ = *src++;
					}
			}
	else
		for (;;)
			switch (*src) {
			case '\0':
			case ' ':
			case '\n':
			case '\t':
			case '\r':
				return 1;
			default:
				*dest++ = *src++;
			}
	return 0;
}

ALWAYS_INLINE int nixCount(const char *RESTRICT src, const int c)
{
	int count = 0;
	while (*src)
		if (*src++ == c) ++count;
	return count;
}

ALWAYS_INLINE int nixCountUnlikely(const char *RESTRICT src, const int c)
{
	int count = 0;
	while (*src)
		if (unlikely(*src++ == c)) ++count;
	return count;
}

ALWAYS_INLINE int nixCountDigit(const char *RESTRICT src)
{
	int count = 0;
	while (*src)
		if (isdigit(*src)) ++count;
	return count;
}

ALWAYS_INLINE int nixCountAlpha(const char *RESTRICT src)
{
	int count = 0;
	while (*src)
		if (isalpha(*src)) ++count;
	return count;
}

#define NIX_WC(FUNC_NAME, DELIM) \
inline int FUNC_NAME(const char *RESTRICT src) \
{ \
	for (int count = 0;; ++src) \
		switch (*src) { \
		case '\0': \
			return count; \
		DELIM \
			++count; \
		} \
} \

NIX_WC(nixWcNl, case '\n':)
NIX_WC(nixWcSpace, case ' ':)
NIX_WC(nixWcTab, case '\t':)
NIX_WC(nixWcNonWords, case '\n': case '\t': case '\r': case ' ':)

#define NIX_WCCHAR(FUNC_NAME, DELIM) \
inline int FUNC_NAME(const char *RESTRICT src) \
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

#define NIX_WCWORD(FUNC_NAME, DELIM) \
inline int FUNC_NAME(const char *RESTRICT src) \
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
			if (unlikely(!inWord)) \
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
inline int FUNC_NAME(const char *RESTRICT src) \
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
			if (unlikely(!inWord)) \
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
