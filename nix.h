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
int nixWcw(char *src);
/* get number of lines in string */
int nixWcl(char *src);
int nixAwk(char delim, int nStr, char *src, int srcLen, char **dest);
int nixAwkFile(char delim, int nStr, const char *filename, char **dest);
int nixTokenize(const char *str, char ***arr);
void nixTokenizeFree(char **arr, int arrLen);
int nixTokenizeLine(const char *str, char ****arr);

#ifdef __cplusplus
}
#endif

#endif
