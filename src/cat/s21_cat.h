#ifndef _S21_CAT_H_
#define _S21_CAT_H_

#include <stdbool.h>
#include <stdio.h>

typedef struct flags {
  bool b;
  bool e;
  bool n;
  bool s;
  bool t;
  bool v;
} flags;

bool flag_parser(flags *opt, int argc, char **argv);
void file_print(flags opt, FILE *f);
bool cat_handler(flags opt, int argc, char **argv);

#endif  // _S21_CAT_H_
