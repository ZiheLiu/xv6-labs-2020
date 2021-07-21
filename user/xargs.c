#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "xargs: too few or many args\n");
    exit(0);
  }

  char *user_argv[MAXARG];
  for (int i = 1; i < argc; i++) {
    user_argv[i - 1] = argv[i];
  }

  char b;
  int arg_i;
  char buf[BUF_SIZE];
  char* buf_p;
  int n = 1;

  while (n) {
    // continuous empty lines.
    while ((n = read(0, &b, 1)) && b == '\n') {}
    if (!n) {
      break;
    }

    arg_i = argc - 1;
    buf_p = buf;
    do {
      if (arg_i == MAXARG) {
        fprintf(2, "xargs: too many args");
        exit(0);
      }
      if (buf_p >= buf + BUF_SIZE - 1) {
        fprintf(2, "xargs: too long args");
        exit(0);
      }
      *(buf_p++) = b;
    } while ((n = read(0, &b, 1)) && b != '\n');


    *buf_p = 0;
    user_argv[arg_i++] = buf;

    // truncate user_argv.
    if (arg_i < MAXARG) {
      user_argv[arg_i] = 0;
    }

    if (fork()) {
      exec(user_argv[0], user_argv);
    } else {
      wait(0);
    }
  }

  exit(0);
}