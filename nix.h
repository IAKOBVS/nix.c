#ifndef NIX_H_DEF
#define NIX_H_DEF
int sizeOfFile(char *filename);
/* flags: 'w' = overwrite; 'a' = append */
int tee(char *flag, char *inStr, char *filename);
int head(char *filename, char **outStr);
int cat(char *filename, char **outStr);
/* flags: l' =  line; 'w' =  word */
int wc(char flag, char *filename);
int awk(char delim, int nStr, char **src, int strLen);
int awkFile(char delim, int nStr, char *filename, char **src);
#endif
