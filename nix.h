#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stdlib.h>
#include "/home/james/c/jString/jstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* get size of file */
int nixSizeOfFile(const char *filename);

/* flags: */
/* 'w' = overwrite */
/* 'a' = append */
int nixTee(const char *flag, char *inStr, const char *filename);

/* get first line of file */
int nixHead(const char *filename, char **dest);

/* read file into string */
int nixCat(const char *filename, char **dest);

/* find files in directory */
int nixFindDir(char *dir, char **dest);

/* get number of chars in string */
int nixWcc(char *src);

/* get number of words in string */
int nixWcWord(char *src);

/* get number of words in string */
int nixWcWord(char *src);
int nixWcWordSpace(char *src);
int nixWcWordComma(char *src);
int nixWcWordPipe(char *src);
int nixWcWordQuote(char *src);
int nixWcWordDoubleQuote(char *src);

/* get number of words in string until newline */
int nixWcWordNl(char *src);
int nixWcWordNlSpace(char *src);
int nixWcWordNlComma(char *src);
int nixWcWordNlPipe(char *src);
int nixWcWordNlQuote(char *src);
int nixWcWordNlDoubleQuote(char *src);

int nixAwk(int nStr, char *src, int srcLen, char **dest);
int nixAwkComma(int nStr, char *src, int srcLen, char **dest);
int nixAwkDouble(int nStr, char *src, int srcLen, char **dest);
int nixAwkQuote(int nStr, char *src, int srcLen, char **dest);
int nixAwkDoubleQuote(int nStr, char *src, int srcLen, char **dest);
int nixAwkPipe(int nStr, char *src, int srcLen, char **dest);

/* split spaces, tabs, etc. to array of strings */
int nixSplit(const char *str, char ***arr);
/* split lines to array of strings */
int nixSplitNl(const char *str, char ***arr);
void nixSplitFree(char **arr, int arrLen);

#ifdef __cplusplus
}
#endif

#endif
