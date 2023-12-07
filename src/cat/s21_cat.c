#include "s21_cat.h"

#include <string.h>

int main(int argc, char **argv) {
  if (argc > 1) {
    flags opt = {false};
    bool error = flag_parser(&opt, argc, argv);
    if (!error) {
      bool no_file = cat_handler(opt, argc, argv);
      if (no_file) {
        fprintf(stderr, "there is no file\n");
      }
    } else {
      fprintf(stderr, "wrong flag\n");
    }
  } else {
    fprintf(stderr, "there is no file\n");
  }

  return 0;
}

bool cat_handler(flags opt, int argc, char **argv) {
  bool no_file = true;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      no_file = false;
      FILE *f = fopen(argv[i], "rb");
      if (f != NULL) {
        file_print(opt, f);
        fclose(f);
      } else {
        no_file = true;
      }
    }
  }
  return no_file;
}

bool flag_parser(flags *opt, int argc, char **argv) {
  bool error = false;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] == '-') {
      if (strcmp("--number-nonblank", argv[i]) == 0) {
        opt->b = true;
      } else if (strcmp("--number", argv[i]) == 0) {
        opt->n = true;
      } else if (strcmp("--squeeze-blank", argv[i]) == 0) {
        opt->s = true;
      } else {
        error = true;
      }
    }
    if (argv[i][0] == '-' && argv[i][1] != '-') {
      for (size_t j = 1; j < strlen(argv[i]); j++) {
        switch (argv[i][j]) {
          case 'b':
            opt->b = true;
            break;
          case 'e':
            opt->e = true;
            opt->v = true;
            break;
          case 'n':
            opt->n = true;
            break;
          case 's':
            opt->s = true;
            break;
          case 't':
            opt->t = true;
            opt->v = true;
            break;
          case 'E':
            opt->e = true;
            break;
          case 'T':
            opt->t = true;
            break;
          case 'v':
            opt->v = true;
            break;
          default:
            error = true;
            break;
        }
      }
    }
  }
  if (opt->b && opt->n) {
    opt->n = false;
  }
  return error;
}

void file_print(flags opt, FILE *f) {
  int curr = 0, prev = '\n', nl_counter = 0, line_number = 1;
  while ((curr = fgetc(f)) != EOF) {
    if (opt.s) {
      if (curr == '\n') {
        nl_counter++;
      } else {
        nl_counter = 0;
      }
    }

    if (curr == '\n' && nl_counter < 3) {
      if (opt.n && prev == '\n') {
        printf("%6d\t", line_number++);
      }
      if (opt.e) {
        printf("$");
      }
      printf("%c", curr);
    }

    if (curr != '\n') {
      if ((opt.n || opt.b) && prev == '\n') {
        printf("%6d\t", line_number++);
      }
      if (opt.v && curr < 32 && curr != 9 && curr != 10 && curr >= 0) {
        printf("^%c", curr + 64);
      } else if (opt.v && curr == 127) {
        printf("^%c", curr - 64);
      } else if (opt.t && curr == '\t') {
        printf("^I");
      } else {
        printf("%c", curr);
      }
    }

    prev = curr;
  }
}
