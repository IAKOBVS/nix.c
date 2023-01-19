#ifndef AWK_H
#define AWK_H
static void awAwk(char delim, char *inStr, int nStr, char *outStr)
{
	int i = 0;
	while (inStr[i] == delim && inStr[i])
		++i;
	if (nStr > 1)
		for (int nDelim = 1; nDelim<nStr && inStr[i]; ++nDelim) {
			while (inStr[i] != delim && inStr[i])
				++i;
			while (inStr[i] == delim && inStr[i])
				++i;
		}
	for (int j = 0; inStr[i] != delim && inStr[i]; ++i, ++j)
		outStr[j] = inStr[i];
}
#endif
