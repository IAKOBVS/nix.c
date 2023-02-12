#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stdlib.h>
#include "/home/james/c/jString/jstr.h"

int sizeOfFile(char *filename);
/* flags: 'w' = overwrite; 'a' = append */
int tee(char *flag, char *inStr, char *filename);
/* first line of file */
int head(char *filename, char **dest);
/* read file into string */
int cat(char *filename, char **dest);
/* find files in directory */
int findDir(char *dir, char **dest);
/* chars in string */
int wcc(char *src);
/* words in string */
int wcw(char *src);
/* lines in string */
int wcl(char *src);
int awk(char delim, int nStr, char *src, int srcLen, char **dest);
int awkFile(char delim, int nStr, char *filename, char **dest);
#endif
