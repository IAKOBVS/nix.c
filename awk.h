#ifndef AWK_H
#define AWK_H
static void awkc(char delim, char inStr[], int nStr, char outStr[])
{
	int i = 0;
	while (inStr[i] == delim && inStr[i] != '\0') ++i;

	if (nStr > 1)
		for (int nDelim = 1; nDelim<nStr && inStr[i] != '\0'; ++nDelim) {
			while (inStr[i] != delim && inStr[i] != '\0') ++i;
			while (inStr[i] == delim && inStr[i] != '\0') ++i;
		}

	for (int j = 0; inStr[i] != delim && inStr[i] !='\0' ; ++i, ++j)
		outStr[j] = inStr[i];
}
#endif
