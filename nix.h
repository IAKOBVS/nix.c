#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stdlib.h>
#include "/home/james/c/jString/jstr.h"

int sizeOfFile(char *filename);
/* flags: 'w' = overwrite; 'a' = append */
int tee(char *flag, char *inStr, char *filename);
int head(char *filename, Jstr *dest);
int cat(char *filename, Jstr *dest);
/* flags: 'l' =  line; 'w' =  word */
int wc(char flag, char *filename);
int awk(char delim, int nStr, char *src, int srcLen, Jstr *dest);
int awkFile(char delim, int nStr, char *filename, Jstr *src);
#endif
