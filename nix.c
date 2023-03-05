#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

#include "nix.h"
#include "macros.h"

#define CASE_DIGIT case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
#define CASE_LOWER case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z'
#define CASE_UPPER	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z'
#define CASE_WHITESPACE case '\n': case '\t': case '\r':

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define MIN_MALLOC 16000

ALWAYS_INLINE void nix_upper(char *RESTRICT dest)
{
	for (;;) {
		switch (*dest) {
		CASE_LOWER:
			*dest = toupper(*dest);
		case '\0':
			break;
		}
		break;
	}
}

ALWAYS_INLINE void nix_lower(char *RESTRICT dest)
{
	for (;;) {
		switch (*dest) {
		CASE_UPPER:
			*dest = tolower(*dest);
		case '\0':
			break;
		}
		break;
	}
}

inline int nix_rev(char *RESTRICT dest, const char *RESTRICT src, const size_t src_len)
{
	for (const char *RESTRICT end = src + src_len - 1; (*dest++ = end >= src ? *end-- : '\0'); );
	return 1;
}

inline int nix_rev_self(char *RESTRICT dest, const size_t dest_len)
{
	for (size_t i = 0, j = dest_len - 1; i < j; ++i, --j) {
		const char tmp = dest[i];
		dest[i] = dest[j];
		dest[j] = tmp;
	}
	return 1;
}

ALWAYS_INLINE size_t nix_size_of_file(const char *RESTRICT filename)
{
	struct stat st;
	return (!stat(filename, &st) ? st.st_size : 0);
}

int nix_tee(char *RESTRICT dest, const char *RESTRICT flag, const char *RESTRICT filename)
{
	FILE *RESTRICT fp = fopen(filename, flag);
	return (likely(fp)) ? (fputs(dest, fp), fclose(fp), 1) : 0;
}

int nix_find(char *RESTRICT dest, const char *RESTRICT dir)
{
	struct dirent *RESTRICT ep;
	DIR *RESTRICT dp = opendir(dir);
	if (unlikely(!dp))
		goto ERROR;
	for (char *RESTRICT filename; (ep = readdir(dp)); ) {
		filename = ep->d_name;
		while ((*dest++ = *filename ? *filename++ : '\n') != '\n');
	}
	closedir(dp);
	*--dest = '\0';
	return 1;

ERROR:
	return 0;
}

int nix_find_auto(char **RESTRICT dest, const char *RESTRICT dir)
{
	struct dirent *ep;
	DIR *RESTRICT dp = opendir(dir);
	if (unlikely(!dp))
		goto ERROR;
	if (unlikely(!(*dest = malloc(MIN_MALLOC))))
		goto ERROR;
	size_t i = 0;
	for (size_t tmp_len, malloc_size = MIN_MALLOC; (ep = readdir(dp)); ) {
		char *RESTRICT filename = ep->d_name;
		tmp_len = malloc_size + strlen(filename);
		if (tmp_len > malloc_size) {
			do {
				malloc_size *= 2;
			} while (tmp_len > malloc_size);
			if (unlikely(!((*dest = realloc(*dest, malloc_size)))))
				goto ERROR_FREE;
		}
		while (((*dest)[i++] = *filename ? *filename++ : '\n') != '\n');
	}
	closedir(dp);
	if (unlikely(!(*dest = realloc(*dest, i + 1))))
		goto ERROR_FREE;
	(*dest)[--i] = '\0';
	return i;

ERROR_FREE:
	free(*dest);
ERROR:
	return 0;
}

int nix_head(char *RESTRICT dest, const char *RESTRICT filename)
{
	FILE *RESTRICT fp = fopen(filename, "r");
	if (unlikely(!fp))
		return 0;
	fgets(dest, 256, fp);
	fclose(fp);
	return 1;
}

#define NIX_CAT(FUNC_NAME, FREAD)                                                             \
int FUNC_NAME(char *RESTRICT dest, const char *RESTRICT filename, const size_t file_size)     \
{                                                                                             \
	FILE *RESTRICT fp = fopen(filename, "r");                                             \
	if (unlikely(!fp))                                                                    \
		goto ERROR;                                                                   \
	if (unlikely(!FREAD(dest, 1, file_size, fp)))                                         \
		goto ERROR_CLOSE;                                                             \
	fclose(fp);                                                                           \
	dest[file_size] = '\0';                                                               \
	return 1;                                                                             \
                                                                                              \
ERROR_CLOSE:                                                                                  \
	fclose(fp);                                                                           \
ERROR:                                                                                        \
	return 0;                                                                             \
}

NIX_CAT(nix_cat, fread)
NIX_CAT(nix_cat_fast, fread_unlocked)

#define NIX_CAT_AUTO(FUNC_NAME, FREAD)                               \
int FUNC_NAME(char **RESTRICT dest, const char *RESTRICT filename)   \
{                                                                    \
	const size_t file_size = nix_size_of_file(filename);         \
	if (unlikely(!(file_size)))                                  \
		goto ERROR;                                          \
	FILE *RESTRICT fp = fopen(filename, "r");                    \
	if (unlikely(!fp))                                           \
		goto ERROR;                                          \
	if (unlikely(!(*dest = malloc(file_size))))                  \
		goto ERROR_CLOSE;                                    \
	if (unlikely(FREAD(*dest, 1, file_size, fp)))                \
		goto ERROR_CLOSE_FREE;                               \
	fclose(fp);                                                  \
	(*dest)[file_size] = '\0';                                   \
	return 1;                                                    \
                                                                     \
ERROR_CLOSE_FREE:                                                    \
	free(*dest);                                                 \
ERROR_CLOSE:                                                         \
	fclose(fp);                                                  \
ERROR:                                                               \
	return 0;                                                    \
}

NIX_CAT_AUTO(nix_cat_auto, fread)
NIX_CAT_AUTO(nix_cat_auto_fast, fread_unlocked)

inline int nix_cut_first(char *RESTRICT dest, const char *RESTRICT src)
{
	for (;;) {
		switch (*src) {
		default:
			*dest++ = *src++;
			continue;
		case '\n':
		case '\t':
		case '\r':
		case ' ':
		case '\0':;
		}
		break;
	}
	*dest = '\0';
	return 1;
}

ALWAYS_INLINE int nix_cut_first_delim(char *RESTRICT dest, const char *RESTRICT src, const int delim)
{
	while ((*dest++ = *src++) != delim);
	*--dest = '\0';
	return 1;
}

inline int nix_cut_last(char *RESTRICT dest, const char *RESTRICT src, const size_t src_len)
{
	src += src_len - 1;
	for (;; --src) {
		switch (*src) {
		default:
			continue;
		case '\n':
		case '\t':
		case '\r':
		case ' ':;
		}
		break;
	}
	while ((*dest++ = *++src));
	return 1;
}

ALWAYS_INLINE int nix_cut_last_delim(char *RESTRICT dest, const char *RESTRICT src, const size_t src_len, const int delim)
{
	src += src_len - 1;
	while (*src-- != delim);
	++src;
	while ((*dest++ = *++src));
	return 1;
}

inline int nix_cut(char *RESTRICT dest, const char *RESTRICT src, int n_str)
{
	for (;;) {
		switch (*src++) {
		case '\n':
		case '\t':
		case '\r':
		case ' ':
			if (--n_str)
		default:
				continue;
		case '\0':;
		}
		break;
	}
	for (;;) {
		switch (*src) {
		default:
			*dest++ = *src++;
			continue;
		case '\0':
		case '\n':
		case '\t':
		case '\r':
		case ' ':;
		}
		break;
	}
	*dest = '\0';
	return 1;
}

#define NIX_COUNT(T, expr, ...)                    \
ALWAYS_INLINE int nix_count##T(__VA_ARGS__)        \
{                                                  \
	int count = 0;                             \
	while ((count += (expr) ? 1 : 0), *src++); \
	return count;                              \
}                                                  \

NIX_COUNT( , *src == c, const char *RESTRICT src, const int c)
NIX_COUNT(_digit, isdigit(*src), const char *RESTRICT src)
NIX_COUNT(_alpha, isalpha(*src), const char *RESTRICT src)

inline int nix_wc_word(const char *RESTRICT src)
{
	int in_word = 0, count = 0;
	for ( ;; ++src) {
		switch (*src) {
		default:
			in_word = 1;
			continue;
		case '\n':
		case '\t':
		case '\r':
		case ' ':
			count += in_word ? 1 : (in_word = 0);
			continue;
		case '\0':;
		}
		break;
	}
	return in_word ? ++count : count;
}

inline int nix_wc_word_til_nl(const char *RESTRICT src)
{
	int in_word = 0, count = 0;
	for ( ;; ++src) {
		switch (*src) {
		default:
			in_word = 1;
			continue;
		case '\t':
		case '\r':
		case ' ':
			count += in_word ? 1 : (in_word = 0);
			continue;
		case '\0':
		case '\n':;
		}
		break;
	}
	return in_word ? ++count : count;
}

#define NIX_WCCHAR(FUNC_NAME, DELIM)           \
inline int FUNC_NAME(const char *RESTRICT src) \
{                                              \
	int count = 0;                         \
	for ( ;; ++src) {                      \
		switch (*src) {                \
		DELIM                          \
			continue;              \
		default:                       \
			++count;               \
		case '\0':;                    \
		}                              \
	}                                      \
	return count;                          \
}

NIX_WCCHAR(nix_wc_char, case ' ':)
NIX_WCCHAR(nix_wc_char_comma, case ',':)
NIX_WCCHAR(nix_wc_char_dot, case '.':)
NIX_WCCHAR(nix_wc_char_pipe, case '|':)
NIX_WCCHAR(nix_wc_char_quote, case '\'':)
NIX_WCCHAR(nix_wc_char_double_quote, case '"':)
NIX_WCCHAR(nix_wc_char_tab, case '\t':)
NIX_WCCHAR(nix_wc_char_nl, case '\n':)

NIX_WCCHAR(nix_wc_char_alpha, case '\n': case '\t': case '\r': case ' ':)
NIX_WCCHAR(nix_wc_char_alpha_comma, case '\n': case '\t': case '\r': case ',':)
NIX_WCCHAR(nix_wc_char_alpha_dot, case '\n': case '\t': case '\r': case '.':)
NIX_WCCHAR(nix_wc_char_alpha_pipe, case '\n': case '\t': case '\r': case '|':)
NIX_WCCHAR(nix_wc_char_alpha_quote, case '\n': case '\t': case '\r': case '\'':)
NIX_WCCHAR(nix_wc_char_alpha_double_quote, case '\n': case '\t': case '\r': case '"':)

#define NIX_WCWORD(FUNC_NAME, DELIM)                          \
inline int FUNC_NAME(const char *RESTRICT src)                \
{                                                             \
	int in_word = 0, count = 0;                           \
	for ( ;; ++src) {                                     \
		switch (*src) {                               \
		DELIM                                         \
			count += in_word ? 1 : (in_word = 0); \
			continue;                             \
		default:                                      \
			in_word = 1;                          \
			continue;                             \
		case '\0':;                                   \
		}                                             \
		break;                                        \
	}                                                     \
	return in_word ? ++count : count;                     \
}

/* NIX_WCWORD(nix_wc_word, case ' ':) */
NIX_WCWORD(nix_wc_word_comma, case ',':)
NIX_WCWORD(nix_wc_word_pipe, case '|':)
NIX_WCWORD(nix_wc_word_dot, case '.':)
NIX_WCWORD(nix_wc_word_quote, case '\'':)
NIX_WCWORD(nix_wc_word_double_quote, case '"':)
NIX_WCWORD(nix_wc_word_tab, case '\t':)
NIX_WCWORD(nix_wc_word_nl, case '\n':)

NIX_WCWORD(nix_wc_word_alpha, case '\n': case '\t': case '\r': case ' ':)
NIX_WCWORD(nix_wc_word_alpha_comma, case '\n': case '\t': case '\r': case ',':)
NIX_WCWORD(nix_wc_word_alpha_pipe, case '\n': case '\t': case '\r': case '|':)
NIX_WCWORD(nix_wc_word_alpha_dot, case '\n': case '\t': case '\r': case '.':)
NIX_WCWORD(nix_wc_word_alpha_quote, case '\n': case '\t': case '\r': case '\'':)
NIX_WCWORD(nix_wc_word_alpha_double_quote, case '\n': case '\t': case '\r': case '"':)

#define NIX_WCWORD_TIL_NL(FUNC_NAME, DELIM)                   \
inline int FUNC_NAME(const char *RESTRICT src)                \
{                                                             \
	int in_word = 0, count = 0;                           \
	for ( ;; ++src) {                                     \
		switch (*src) {                               \
		DELIM                                         \
			count += in_word ? 1 : (in_word = 0); \
			continue;                             \
		default:                                      \
			in_word = 1;                          \
		case 'n':                                     \
		case '\0':;                                   \
		}                                             \
		break;                                        \
	}                                                     \
	return in_word ? ++count : count;                     \
}

/* NIX_WCWORD_TIL_NL(nix_wc_word_til_nl, case ' ':) */
NIX_WCWORD_TIL_NL(nix_wc_word_til_nl_pipe, case '|':)
NIX_WCWORD_TIL_NL(nix_wc_word_til_nl_comma, case ',':)
NIX_WCWORD_TIL_NL(nix_wc_word_til_nl_dot, case '.':)
NIX_WCWORD_TIL_NL(nix_wc_word_til_nl_quote, case '\'':)
NIX_WCWORD_TIL_NL(nix_wc_word_til_nl_double_quote, case '"':)
