#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void write_int(int fd, int val) {
  write(fd, (char*)(&val), 4);
}

int read_int(int fd, int* val) {
  return read(fd, (char *) val, 4);
}

int worker(int read_fd) {
  int first = -1;
  int num;
  int p[2] = {0, 0};
  while (read_int(read_fd, &num)) {
    if (first == -1) {
      printf("prime %d\n", num);
      first = num;
    } else if (num % first != 0) {
      if (!p[0]) {
        pipe(p);
        if (fork() == 0) {
          // child closes parent's read channel.
          close(read_fd);
          // child closes write channel.
          close(p[1]);
          return p[0];
        }
        close(p[0]);
      }
      write_int(p[1], num);
    }
  }

  // child closes parent's read channel.
  close(read_fd);
  if (p[0]) {
    // child closes write channel.
    close(p[1]);
    wait(0);
  }
  return 0;
}


int main(int argc, char *argv[]) {
  int p[2];
  pipe(p);

  if (fork() != 0) {
    // parent closes read channel.
    close(p[0]);
    for (int num = 2; num <= 35; num++) {
      write_int(p[1], num);
    }
    // parent closes write channel.
    close(p[1]);
    wait(0);
  } else {
    // child closes write channel.
    close(p[1]);
    int read_fd = p[0];
    while (read_fd) {
      read_fd = worker(read_fd);
    }
  }

  exit(0);
}