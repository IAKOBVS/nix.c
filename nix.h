#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stdlib.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t nixSizeOfFile(char *filename);
int nixRev(char dest[], char *src, int srcLen);
int nixGetLastWord(char dest[], char *src, int srcLen);
int nixCut(int nStr, char *src, char dest[]);
/* flags: */
/* 'w' = overwrite */
/* 'a' = append */
int nixTee(char *flag, char *inStr, char *filename);

/* get first line of file */
int nixHead(char *filename, char dest[]);

/* read file into string */
int nixCat(char *filename, size_t fileSize, char dest[]);
int nixCatFast(char *filename, size_t fileSize, char dest[]);
int nixCatAuto(char *filename, char **dest);
int nixCatAutoFast(char *filename, char **dest);

/* find files in directory */
int nixFind(char *dir, char dest[]);
/* find that automatically reallocs to required size */
int nixFindAuto(char *dir, char **dest);

int nixWcNl(char *src);
int nixWcSpace(char *src);
int nixWcTab(char *src);
int nixWcNonWords(char *src);

/* get number of chars in string */
int nixWcChar(char *src);
int nixWcCharPipe(char *src);
int nixWcCharComma(char *src);
int nixWcCharDot(char *src);
int nixWcCharQuote(char *src);
int nixWcCharDoubleQuote(char *src);
int nixWcCharTab(char *src);
int nixWcCharNl(char *src);

int nixWcCharAlpha(char *src);
int nixWcCharAlphaPipe(char *src);
int nixWcCharAlphaComma(char *src);
int nixWcCharAlphaDot(char *src);
int nixWcCharAlphaQuote(char *src);
int nixWcCharAlphaDoubleQuote(char *src);
int nixWcCharAlphaTab(char *src);

/* get number of words in string */
int nixWcWord(char *src);
int nixWcWordComma(char *src);
int nixWcWordDot(char *src);
int nixWcWordPipe(char *src);
int nixWcWordQuote(char *src);
int nixWcWordDoubleQuote(char *src);
int nixWcWordTab(char *src);
int nixWcWordNl(char *src);

int nixWcWordAlpha(char *src);
int nixWcWordAlphaComma(char *src);
int nixWcWordAlphaDot(char *src);
int nixWcWordAlphaPipe(char *src);
int nixWcWordAlphaQuote(char *src);
int nixWcWordAlphaDoubleQuote(char *src);
int nixWcWordAlphaTab(char *src);

/* get number of words in string until newline */
int nixWcWordTilNl(char *src);
int nixWcWordTilNlComma(char *src);
int nixWcWordTilNlDot(char *src);
int nixWcWordTilNlPipe(char *src);
int nixWcWordTilNlQuote(char *src);
int nixWcWordTilNlDoubleQuote(char *src);
int nixWcWordTilNlTab(char *src);

int nixAwk(int nStr, char *src, int srcLen, char **dest);
int nixAwkComma(int nStr, char *src, int srcLen, char **dest);
int nixAwkDot(int nStr, char *src, int srcLen, char **dest);
int nixAwkDouble(int nStr, char *src, int srcLen, char **dest);
int nixAwkQuote(int nStr, char *src, int srcLen, char **dest);
int nixAwkDoubleQuote(int nStr, char *src, int srcLen, char **dest);
int nixAwkPipe(int nStr, char *src, int srcLen, char **dest);
int nixAwkTab(int nStr, char *src, int srcLen, char **dest);

/* split spaces, tabs, etc. to array of strings */
int nixSplitWords(char *str, char ***arr);
/* split lines to array of strings */
int nixSplitNl(char *str, char ***arr);
void nixSplitFree(char **arr, int arrLen);

#ifdef __cplusplus
}
#endif

#endif
