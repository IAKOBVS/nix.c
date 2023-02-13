#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stdlib.h>
#include "/home/james/c/jString/jstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* get size of file */
int sizeOfFile(const char *filename);
/* flags: */
/* 'w' = overwrite */
/* 'a' = append */
int tee(const char *flag, char *inStr, const char *filename);
/* get first line of file */
int head(const char *filename, char **dest);
/* read file into string */
int cat(const char *filename, char **dest);
/* find files in directory */
int findDir(char *dir, char **dest);
/* get number of chars in string */
int wcc(char *src);
/* get number of words in string */
int wcw(char *src);
/* get number of lines in string */
int wcl(char *src);
int awk(char delim, int nStr, char *src, int srcLen, char **dest);
int awkFile(char delim, int nStr, const char *filename, char **dest);

#ifdef __cplusplus
}
#endif

#endif
