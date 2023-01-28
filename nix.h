#ifndef NIX_H_DEF
#define NIX_H_DEF

static int sizeOfFile(char *filename);
static int tee(char *flag, char *inStr, char *filename);
static int cat(char *filename, char **outStr);
static int wc(char flag, char *filename);
static int awk(char delim, int nStr, char *filename, char **outStr);

#endif
