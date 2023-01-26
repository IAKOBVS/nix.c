#ifndef AWK_H_DEF
#define AWK_H_DEF
#include "/home/james/.local/bin/nix.c/cat.h"
static void awk(char delim, char *inStr, int nStr, char *outStr)
{
	int i = 0;
	while (inStr[i] == delim && inStr[i])
		++i;
	if (nStr > 1)
		for (int nDelim = 1; nDelim<nStr; ++nDelim) {
			while (inStr[i] != delim && inStr[i])
				++i;
			while (inStr[i] == delim && inStr[i])
				++i;
		}
	int j;
	for (j = 0; inStr[i] != delim && inStr[i]; ++i, ++j)
		outStr[j] = inStr[i];
	outStr[++j] = '\0';
}
#endif
