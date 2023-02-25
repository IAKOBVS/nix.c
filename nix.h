#ifndef NIX_H_DEF
#define NIX_H_DEF

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
# define RESTRICT __restrict__
#elif defined(_MSC_VER)
# define RESTRICT __restrict
#else
# define RESTRICT
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t nixSizeOfFile(const char *RESTRICT filename);

#define nixCount(COUNT, SRC, CHAR)        \
	do {                              \
		for ( ;; ++src) {         \
			switch (*SRC) {   \
			case CHAR:        \
				++COUNT;  \
			default:          \
				continue; \
			case '\0':;       \
			}                 \
			break;            \
		}                         \
	} while (0)

int nixCountFunc(const char *RESTRICT src, const int c);
int nixCountFuncUnlikely(const char *RESTRICT src, const int c);
int nixCountDigit(const char *RESTRICT src);
int nixCountAlpha(const char *RESTRICT src);

int nixRev(char *RESTRICT dest, const char *RESTRICT src, const size_t srcLen);
int nixRevAuto(char **RESTRICT dest, const char *RESTRICT src, const size_t srcLen);
int nixRevSelfHeap(char *RESTRICT dest, const size_t srcLen);
int nixRevSelf(char *RESTRICT dest, const size_t srcLen);

int nixCutDelim(char *RESTRICT dest, const char *RESTRICT src, int nStr, const int delim);
int nixCutFirst(char *RESTRICT dest, const char *RESTRICT src);
int nixCutFirstDelim(char *RESTRICT dest, const char *RESTRICT src, const int delim);
int nixCutLast(char *RESTRICT dest, const char *RESTRICT src, const size_t srcLen);
int nixCutLastDelim(char *RESTRICT dest, const char *RESTRICT src, const size_t srcLen, const int delim);

/* flags: */
/* 'w' = overwrite */
/* 'a' = append */
int nixTee(char *RESTRICT dest, const char *RESTRICT flag, const char *RESTRICT filename);

/* get first line of file */
int nixHead(char *RESTRICT dest, const char *RESTRICT filename);

/* read file into string */
int nixCat(char *RESTRICT dest, const char *RESTRICT filename, const size_t fileSize);
int nixCatFast(char *RESTRICT dest, const char *RESTRICT filename, const size_t fileSize);
int nixCatAuto(char **RESTRICT dest, const char *RESTRICT filename);
int nixCatAutoFast(char **RESTRICT dest, const char *RESTRICT filename);

/* find files in directory */
int nixFind(char *RESTRICT dest, const char *RESTRICT dir);
/* find that automatically reallocs to required size */
int nixFindAuto(char **RESTRICT dest, const char *RESTRICT dir);

int nixWcNl(const char *RESTRICT src);
int nixWcSpace(const char *RESTRICT src);
int nixWcTab(const char *RESTRICT src);
int nixWcNonWords(const char *RESTRICT src);

/* /1* get number of chars in string *1/ */
/* int nixWcChar(const char *RESTRICT src); */
/* int nixWcCharPipe(const char *RESTRICT src); */
/* int nixWcCharComma(const char *RESTRICT src); */
/* int nixWcCharDot(const char *RESTRICT src); */
/* int nixWcCharQuote(const char *RESTRICT src); */
/* int nixWcCharDoubleQuote(const char *RESTRICT src); */
/* int nixWcCharTab(const char *RESTRICT src); */
/* int nixWcCharNl(const char *RESTRICT src); */

/* int nixWcCharAlpha(const char *RESTRICT src); */
/* int nixWcCharAlphaPipe(const char *RESTRICT src); */
/* int nixWcCharAlphaComma(const char *RESTRICT src); */
/* int nixWcCharAlphaDot(const char *RESTRICT src); */
/* int nixWcCharAlphaQuote(const char *RESTRICT src); */
/* int nixWcCharAlphaDoubleQuote(const char *RESTRICT src); */
/* int nixWcCharAlphaTab(const char *RESTRICT src); */

/* /1* get number of words in string *1/ */
int nixWcWord(const char *RESTRICT src);
/* int nixWcWordComma(const char *RESTRICT src); */
/* int nixWcWordDot(const char *RESTRICT src); */
/* int nixWcWordPipe(const char *RESTRICT src); */
/* int nixWcWordQuote(const char *RESTRICT src); */
/* int nixWcWordDoubleQuote(const char *RESTRICT src); */
/* int nixWcWordTab(const char *RESTRICT src); */
/* int nixWcWordNl(const char *RESTRICT src); */

/* int nixWcWordAlpha(const char *RESTRICT src); */
/* int nixWcWordAlphaComma(const char *RESTRICT src); */
/* int nixWcWordAlphaDot(const char *RESTRICT src); */
/* int nixWcWordAlphaPipe(const char *RESTRICT src); */
/* int nixWcWordAlphaQuote(const char *RESTRICT src); */
/* int nixWcWordAlphaDoubleQuote(const char *RESTRICT src); */
/* int nixWcWordAlphaTab(const char *RESTRICT src); */

/* /1* get number of words in string until newline *1/ */
/* int nixWcWordTilNl(const char *RESTRICT src); */
/* int nixWcWordTilNlComma(const char *RESTRICT src); */
/* int nixWcWordTilNlDot(const char *RESTRICT src); */
/* int nixWcWordTilNlPipe(const char *RESTRICT src); */
/* int nixWcWordTilNlQuote(const char *RESTRICT src); */
/* int nixWcWordTilNlDoubleQuote(const char *RESTRICT src); */
/* int nixWcWordTilNlTab(const char *RESTRICT src); */

#ifdef __cplusplus
}
#endif

#undef RESTRICT

#endif
