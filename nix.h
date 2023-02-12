#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stdlib.h>
#include "/home/james/c/jString/jstr.h"

#ifdef __cplusplus
extern "C" {
namespace nix {
#endif

int sizeOfFile(const char *filename);
/* flags: 'w' = overwrite; 'a' = append */
int tee(char *flag, char *inStr, const char *filename);
/* first line of file */
int head(const char *filename, char **dest);
/* read file into string */
int cat(const char *filename, char **dest);
/* find files in directory */
int findDir(char *dir, char **dest);
/* chars in string */
int wcc(char *src);
/* words in string */
int wcw(char *src);
/* lines in string */
int wcl(char *src);
int awk(char delim, int nStr, char *src, int srcLen, char **dest);
int awkFile(char delim, int nStr, const char *filename, char **dest);

#ifdef __cplusplus
}
}
#endif

#endif
