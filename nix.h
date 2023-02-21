#ifndef NIX_H_DEF
#define NIX_H_DEF

#ifdef __cplusplus
extern "C" {
#endif

size_t nixSizeOfFile(const char *restrict filename);
int nixRev(char *restrict dest, const char *restrict src, const size_t srcLen);
int nixGetLastWord(char *restrict dest, const char *restrict src, const size_t srcLen);
int nixCut(int nStr, const char *restrict src, char *restrict dest);
/* flags: */
/* 'w' = overwrite */
/* 'a' = append */
int nixTee(const char *restrict flag, char *restrict dest, const char *restrict filename);

/* get first line of file */
int nixHead(const char *restrict filename, char dest[]);

/* read file into string */
int nixCat(const char *restrict filename, const size_t fileSize, char *restrict dest);
int nixCatFast(const char *restrict filename, const size_t fileSize, char *restrict dest);
int nixCatAuto(const char *restrict filename, char **restrict dest);
int nixCatAutoFast(const char *restrict filename, char **restrict dest);

/* find files in directory */
int nixFind(const char *restrict dir, char *restrict dest);
/* find that automatically reallocs to required size */
int nixFindAuto(const char *restrict dir, char **restrict dest);

int nixWcNl(const char *restrict src);
int nixWcSpace(const char *restrict src);
int nixWcTab(const char *restrict src);
int nixWcNonWords(const char *restrict src);

/* get number of chars in string */
int nixWcChar(const char *restrict src);
int nixWcCharPipe(const char *restrict src);
int nixWcCharComma(const char *restrict src);
int nixWcCharDot(const char *restrict src);
int nixWcCharQuote(const char *restrict src);
int nixWcCharDoubleQuote(const char *restrict src);
int nixWcCharTab(const char *restrict src);
int nixWcCharNl(const char *restrict src);

int nixWcCharAlpha(const char *restrict src);
int nixWcCharAlphaPipe(const char *restrict src);
int nixWcCharAlphaComma(const char *restrict src);
int nixWcCharAlphaDot(const char *restrict src);
int nixWcCharAlphaQuote(const char *restrict src);
int nixWcCharAlphaDoubleQuote(const char *restrict src);
int nixWcCharAlphaTab(const char *restrict src);

/* get number of words in string */
int nixWcWord(const char *restrict src);
int nixWcWordComma(const char *restrict src);
int nixWcWordDot(const char *restrict src);
int nixWcWordPipe(const char *restrict src);
int nixWcWordQuote(const char *restrict src);
int nixWcWordDoubleQuote(const char *restrict src);
int nixWcWordTab(const char *restrict src);
int nixWcWordNl(const char *restrict src);

int nixWcWordAlpha(const char *restrict src);
int nixWcWordAlphaComma(const char *restrict src);
int nixWcWordAlphaDot(const char *restrict src);
int nixWcWordAlphaPipe(const char *restrict src);
int nixWcWordAlphaQuote(const char *restrict src);
int nixWcWordAlphaDoubleQuote(const char *restrict src);
int nixWcWordAlphaTab(const char *restrict src);

/* get number of words in string until newline */
int nixWcWordTilNl(const char *restrict src);
int nixWcWordTilNlComma(const char *restrict src);
int nixWcWordTilNlDot(const char *restrict src);
int nixWcWordTilNlPipe(const char *restrict src);
int nixWcWordTilNlQuote(const char *restrict src);
int nixWcWordTilNlDoubleQuote(const char *restrict src);
int nixWcWordTilNlTab(const char *restrict src);

int nixAwk(int nStr, const char *restrict src, const size_t srcLen, char **dest);
int nixAwkComma(int nStr, const char *restrict src, const size_t srcLen, char **dest);
int nixAwkDot(int nStr, const char *restrict src, const size_t srcLen, char **dest);
int nixAwkDouble(int nStr, const char *restrict src, const size_t srcLen, char **dest);
int nixAwkQuote(int nStr, const char *restrict src, const size_t srcLen, char **dest);
int nixAwkDoubleQuote(int nStr, const char *restrict src, const size_t srcLen, char **dest);
int nixAwkPipe(int nStr, const char *restrict src, const size_t srcLen, char **dest);
int nixAwkTab(int nStr, const char *restrict src, const size_t srcLen, char **dest);

/* split spaces, tabs, etc. to array of strings */
int nixSplitWords(const char *restrict str, char ***restrict arr);
/* split lines to array of strings */
int nixSplitNl(const char *restrict str, char ***restrict arr);
void nixSplitFree(char **restrict arr, size_t arrLen);

#ifdef __cplusplus
}
#endif

#endif
