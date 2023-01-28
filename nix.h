#ifndef NIX_H_DEF
#define NIX_H_DEF
int sizeOfFile(char *filename);
int tee(char *flag, char *inStr, char *filename);
int cat(char *filename, char **outStr);
int wc(char flag, char *filename);
int awk(char delim, int nStr, char *filename, char **outStr);
#endif
