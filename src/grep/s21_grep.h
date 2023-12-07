#ifndef _S21_GREP_H_
#define _S21_GREP_H_

#include <stdbool.h>
#include <stdio.h>

#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define RED "\x1B[1;31m"
#define GREEN "\x1B[0;32m"
#define RESET "\x1B[0m"

typedef struct flags {
  bool e;
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool f;
  bool o;
} flags;

typedef struct substr {
  int start;
  int end;
} substr;

void flag_parser(int argc, char **argv, flags *opts);
char **patterns_parser(int *p_count, int *file_count, int *add, flags opts,
                       int argc, char **argv);
int is_redirected();
void char_printer(flags opts, char ch, int color);
void o_printer(int lines_count, int *count, flags opts, char *file_name,
               int file_count, int idx, char *str, substr *arr);
void grep_printer(int lines_count, int *count, flags opts, char *file_name,
                  int file_count, int idx, char *str, substr *arr);
void grep_handler(flags opts, char *file_name, int file_count, char **patterns,
                  int p_count, FILE *f);
int regex_handler(flags opts, char *file_name, int file_count, char **patterns,
                  int p_count, FILE *f);

#endif  // _S21_GREP_H_
