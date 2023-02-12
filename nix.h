#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stdlib.h>
#include "/home/james/c/jString/jstr.h"

int sizeOfFile(char *filename);
/* flags: 'w' = overwrite; 'a' = append */
int tee(char *flag, char *inStr, char *filename);
int head(char *filename, char **dest);
int cat(char *filename, char **dest);
/* flags: 'l' =  line; 'w' =  word */
int wcc(char *src);
int wcw(char *src);
int wcl(char *src);
int awk(char delim, int nStr, char *src, int srcLen, Jstr *dest);
int awkFile(char delim, int nStr, char *filename, Jstr *dest);
#endif
