#define _GNU_SOURCE

#include "s21_grep.h"

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define buffer 4096

#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define RED "\x1B[1;31m"
#define GREEN "\x1B[0;32m"
#define RESET "\x1B[0m"

int main(int argc, char **argv) {
  flags opts = {false};
  flag_parser(argc, argv, &opts);

  int p_count = 0;
  int file_count = 0;
  int add = 0;
  char **patterns =
      patterns_parser(&p_count, &file_count, &add, opts, argc, argv);

  for (int i = optind + add; i < argc; i++) {
    FILE *f = fopen(argv[i], "rb");
    if (f) {
      grep_handler(opts, argv[i], file_count, patterns, p_count, f);
      fclose(f);
    } else {
      if (!opts.s) {
        fprintf(stderr, "no such file or directory\n");
      }
    }
  }

  for (int q = 0; q < p_count; q++) {
    free(patterns[q]);
  }
  free(patterns);

  return 0;
}

int is_redirected() { return (!isatty(fileno(stdout))); }

void char_printer(flags opts, char ch, int color) {
  if (!opts.c && !opts.l) {
    if (color) {
      printf(RED "%c" RESET, ch);
    } else {
      printf("%c", ch);
    }
  }
}

void o_printer(int lines_count, int *count, flags opts, char *file_name,
               int file_count, int idx, char *str, substr *arr) {
  int len = 0;
  int check = 0;
  if ((idx > 0 && !opts.v) || (idx <= 0 && opts.v)) {
    *count += 1;
  }
  for (int i = 0; i < (int)strlen(str); i++) {
    if (idx > 0 && !opts.v) {
      for (int j = 0; j < idx; j++) {
        if (i >= arr[j].start && i <= arr[j].end) {
          check = 1;
          len = arr[j].end - arr[j].start;
        }
      }
      if (check) {
        if (is_redirected()) {
          if (!opts.c && !opts.l) {
            if (((idx > 0 && !opts.v) || (opts.v && idx <= 0)) &&
                file_count > 1 && !opts.h) {
              printf("%s:", file_name);
            }
            if (((idx > 0 && !opts.v) || (opts.v && idx <= 0)) && opts.n &&
                !opts.c && !opts.l) {
              printf("%d:", lines_count);
            }
            for (int x = 0; x <= len; x++) {
              printf("%c", str[i++]);
            }
            i--;
            printf("\n");
          }
        } else {
          if (!opts.c && !opts.l) {
            if (((idx > 0 && !opts.v) || (opts.v && idx <= 0)) &&
                file_count > 1 && !opts.h) {
              printf(MAGENTA "%s" RESET CYAN ":" RESET, file_name);
            }
            if (((idx > 0 && !opts.v) || (opts.v && idx <= 0)) && opts.n &&
                !opts.c && !opts.l) {
              printf(GREEN "%d:" RESET, lines_count);
            }
            for (int x = 0; x <= len; x++) {
              printf(RED "%c" RESET, str[i++]);
            }
            i--;
            printf("\n");
          }
        }
        check = 0;
      }
    }
  }
}

void grep_printer(int lines_count, int *count, flags opts, char *file_name,
                  int file_count, int idx, char *str, substr *arr) {
  if (((idx > 0 && !opts.v) || (opts.v && idx <= 0)) && file_count > 1 &&
      !opts.h) {
    if (is_redirected()) {
      if (!opts.c && !opts.l) {
        printf("%s:", file_name);
      }
    } else {
      if (!opts.c && !opts.l) {
        printf(MAGENTA "%s" RESET CYAN ":" RESET, file_name);
      }
    }
  }
  if (((idx > 0 && !opts.v) || (opts.v && idx <= 0)) && opts.n && !opts.c &&
      !opts.l) {
    if (is_redirected()) {
      printf("%d:", lines_count);
    } else {
      printf(GREEN "%d:" RESET, lines_count);
    }
  }
  int check = 0;
  if ((idx > 0 && !opts.v) || (idx <= 0 && opts.v)) {
    *count += 1;
  }
  for (int i = 0; i < (int)strlen(str); i++) {
    if (idx > 0 && !opts.v) {
      for (int j = 0; j < idx; j++) {
        if (i >= arr[j].start && i <= arr[j].end) {
          check = 1;
        }
      }
      if (check) {
        if (is_redirected()) {
          char_printer(opts, str[i], 0);
        } else {
          char_printer(opts, str[i], 1);
        }
        check = 0;
      } else {
        char_printer(opts, str[i], 0);
      }
    } else if (idx <= 0 && opts.v) {
      char_printer(opts, str[i], 0);
    }
  }
  if (((idx > 0 && !opts.v) || (opts.v && idx <= 0)) &&
      str[strlen(str) - 1] != '\n') {
    if (!opts.c && !opts.l) {
      printf("\n");
    }
  }
}

int regex_handler(flags opts, char *file_name, int file_count, char **patterns,
                  int p_count, FILE *f) {
  int count = 0;
  regex_t preg;
  char line[buffer];
  substr arr[buffer];
  int lines_count = 0;

  while (fgets(line, buffer, f) != NULL) {
    lines_count += 1;
    char *str = line;
    int stop = 0;
    int idx = 0;
    for (int j = 0; j < p_count; j++) {
      char string[buffer];
      memcpy(string, line, buffer);
      stop = 0;
      char pattern[buffer];
      sprintf(pattern, ".*\\(%s\\).*", patterns[j]);
      size_t nmatch = 2;
      regmatch_t pmatch[2];
      while (stop == 0) {
        if (opts.i) {
          regcomp(&preg, pattern, REG_ICASE);
        } else {
          regcomp(&preg, pattern, 0);
        }
        stop = regexec(&preg, string, nmatch, pmatch, 0);
        if (stop == 0) {
          arr[idx].start = pmatch[1].rm_so;
          arr[idx].end = pmatch[1].rm_eo - 1;
          string[arr[idx].start] = -127;
          idx += 1;
        }
        regfree(&preg);
      }
    }
    if (opts.o && !opts.c && !opts.l) {
      o_printer(lines_count, &count, opts, file_name, file_count, idx, str,
                arr);
    } else {
      grep_printer(lines_count, &count, opts, file_name, file_count, idx, str,
                   arr);
    }
  }

  return count;
}

void grep_handler(flags opts, char *file_name, int file_count, char **patterns,
                  int p_count, FILE *f) {
  int count = regex_handler(opts, file_name, file_count, patterns, p_count, f);

  if (opts.l) {
    if (count > 0) {
      if (is_redirected()) {
        printf("%s\n", file_name);
      } else {
        printf(MAGENTA "%s\n" RESET, file_name);
      }
    }
  } else if (opts.c) {
    if (is_redirected()) {
      if (file_count > 1 && !opts.h) {
        printf("%s:", file_name);
      }
      printf("%d\n", count);
    } else {
      if (file_count > 1 && !opts.h) {
        printf(MAGENTA "%s" RESET CYAN ":" RESET, file_name);
      }
      printf("%d\n", count);
    }
  }
}

char **patterns_parser(int *p_count, int *file_count, int *add, flags opts,
                       int argc, char **argv) {
  char **patterns = malloc(sizeof(char *) * buffer);
  *p_count = 0;

  if (opts.e || opts.f) {
    for (int i = 1; i < optind; i++) {
      if (strstr(argv[i], "e") != NULL) {
        char *p = argv[i++ + 1];
        patterns[*p_count] = malloc(buffer);
        sprintf(patterns[*p_count], "%s", p);
        *p_count += 1;
      } else if (strstr(argv[i], "f") != NULL) {
        char *p = argv[i++ + 1];
        FILE *fp = fopen(p, "rb");
        if (fp) {
          char line[buffer];
          while (fgets(line, buffer, fp) != NULL) {
            if (line[0] != '\n') {
              line[strcspn(line, "\n")] = 0;
              patterns[*p_count] = malloc(buffer);
              sprintf(patterns[*p_count], "%s", line);
              *p_count += 1;
            }
          }
          fclose(fp);
        } else {
          if (!opts.s) {
            fprintf(stderr, "no such file or directory\n");
          }
        }
      }
    }
  } else {
    char *p = argv[optind];
    patterns[*p_count] = malloc(buffer);
    sprintf(patterns[*p_count], "%s", p);
    *p_count += 1;
  }

  *add = 0;
  if (!opts.e && !opts.f) {
    *add = 1;
  }

  *file_count = 0;
  for (int i = optind + *add; i < argc; i++) {
    *file_count += 1;
  }

  return patterns;
}

void flag_parser(int argc, char **argv, flags *opts) {
  char opt = 0;
  char *options = "e:ivclnhsf:o";
  while ((opt = getopt(argc, argv, options)) != -1) {
    switch (opt) {
      case 'e':
        opts->e = true;
        break;
      case 'i':
        opts->i = true;
        break;
      case 'v':
        opts->v = true;
        break;
      case 'c':
        opts->c = true;
        break;
      case 'l':
        opts->l = true;
        break;
      case 'n':
        opts->n = true;
        break;
      case 'h':
        opts->h = true;
        break;
      case 's':
        opts->s = true;
        break;
      case 'f':
        opts->f = true;
        break;
      case 'o':
        opts->o = true;
        break;
      default:
        // error = true;
        break;
    }
  }
}
