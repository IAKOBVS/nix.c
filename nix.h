#ifndef NIX_H_DEF
#define NIX_H_DEF

#include <stddef.h>
#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t nix_sizeof_file(const char *RESTRICT filename);

int nix_count_func(const char *RESTRICT src, const int c);
int nix_count_func_unlikely(const char *RESTRICT src, const int c);
int nix_count_digit(const char *RESTRICT src);
int nix_count_alpha(const char *RESTRICT src);

void nix_rev(char *RESTRICT dest, const char *RESTRICT src, const size_t src_len);
#define nix_rev_auto(dest_arr, src) nix_rev(dest_arr, src, strlen(src))
void nix_rev_self(char *RESTRICT dest, const size_t src_len);

int nix_cut_delim(char *RESTRICT dest, const char *RESTRICT src, int n_str, const int delim);
int nix_cut_first(char *RESTRICT dest, const char *RESTRICT src);
int nix_cut_first_delim(char *RESTRICT dest, const char *RESTRICT src, const int delim);
int nix_cut_last(char *RESTRICT dest, const char *RESTRICT src, const size_t src_len);
int nix_cut_last_delim(char *RESTRICT dest, const char *RESTRICT src, const size_t src_len, const int delim);

/*
flags:
'w' = overwrite
'a' = append
*/
int nix_tee(char *RESTRICT dest, const char *RESTRICT flag, const char *RESTRICT filename);

/* get first line of file */
int nix_head(char *RESTRICT dest, const char *RESTRICT filename);

/* read file into string */
int nix_cat(char *RESTRICT dest, const char *RESTRICT filename, size_t file_size);
int nix_cat_fast(char *RESTRICT dest, const char *RESTRICT filename, size_t file_size);
int nix_cat_auto(char **RESTRICT dest, const char *RESTRICT filename, size_t file_size);
int nix_cat_auto_fast(char **RESTRICT dest, const char *RESTRICT filename, size_t file_size);

/* find files in directory */
int nix_find(char *RESTRICT dest, const char *RESTRICT dir);
/* find that automatically reallocs to required size */
int nix_find_auto(char **RESTRICT dest, const char *RESTRICT dir);

int nix_wc_nl(const char *RESTRICT src);
int nix_wc_space(const char *RESTRICT src);
int nix_wc_tab(const char *RESTRICT src);
int nix_wc_non_words(const char *RESTRICT src);

/* /1* get number of chars in string *1/ */
/* int nix_wc_char(const char *RESTRICT src); */
/* int nix_wc_char_pipe(const char *RESTRICT src); */
/* int nix_wc_char_comma(const char *RESTRICT src); */
/* int nix_wc_char_dot(const char *RESTRICT src); */
/* int nix_wc_char_quote(const char *RESTRICT src); */
/* int nix_wc_char_double_quote(const char *RESTRICT src); */
/* int nix_wc_char_tab(const char *RESTRICT src); */
/* int nix_wc_char_nl(const char *RESTRICT src); */

/* int nix_wc_char_alpha(const char *RESTRICT src); */
/* int nix_wc_char_alpha_pipe(const char *RESTRICT src); */
/* int nix_wc_char_alpha_comma(const char *RESTRICT src); */
/* int nix_wc_char_alpha_dot(const char *RESTRICT src); */
/* int nix_wc_char_alpha_quote(const char *RESTRICT src); */
/* int nix_wc_char_alpha_double_quote(const char *RESTRICT src); */
/* int nix_wc_char_alpha_tab(const char *RESTRICT src); */

/* /1* get number of words in string *1/ */
int nix_wc_word(const char *RESTRICT src);
/* int nix_wc_word_comma(const char *RESTRICT src); */
/* int nix_wc_word_dot(const char *RESTRICT src); */
/* int nix_wc_word_pipe(const char *RESTRICT src); */
/* int nix_wc_word_quote(const char *RESTRICT src); */
/* int nix_wc_word_double_quote(const char *RESTRICT src); */
/* int nix_wc_word_tab(const char *RESTRICT src); */
/* int nix_wc_word_nl(const char *RESTRICT src); */

/* int nix_wc_word_alpha(const char *RESTRICT src); */
/* int nix_wc_word_alpha_comma(const char *RESTRICT src); */
/* int nix_wc_word_alpha_dot(const char *RESTRICT src); */
/* int nix_wc_word_alpha_pipe(const char *RESTRICT src); */
/* int nix_wc_word_alpha_quote(const char *RESTRICT src); */
/* int nix_wc_word_alpha_double_quote(const char *RESTRICT src); */
/* int nix_wc_word_alpha_tab(const char *RESTRICT src); */

/* /1* get number of words in string until newline *1/ */
/* int nix_wc_word_til_nl(const char *RESTRICT src); */
/* int nix_wc_word_til_nl_comma(const char *RESTRICT src); */
/* int nix_wc_word_til_nl_dot(const char *RESTRICT src); */
/* int nix_wc_word_til_nl_pipe(const char *RESTRICT src); */
/* int nix_wc_word_til_nl_quote(const char *RESTRICT src); */
/* int nix_wc_word_til_nl_double_quote(const char *RESTRICT src); */
/* int nix_wc_word_til_nl_tab(const char *RESTRICT src); */

#ifdef __cplusplus
}
#endif

#endif
