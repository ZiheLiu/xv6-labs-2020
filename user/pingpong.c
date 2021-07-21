#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int to_parent_pipe[2];
  int to_child_pipe[2];
  pipe(to_parent_pipe);
  pipe(to_child_pipe);

  char buf[10];

  if (fork() == 0) {
    // close unused channel.
    close(to_child_pipe[1]);
    close(to_parent_pipe[0]);

    read(to_child_pipe[0], buf, sizeof(buf));
    close(to_child_pipe[0]);

    printf("%d: received %s\n", getpid(), buf);
    write(to_parent_pipe[1], "pong", sizeof("pong"));
    close(to_parent_pipe[1]);
  } else {
    // close unused channel.
    close(to_child_pipe[0]);
    close(to_parent_pipe[1]);

    write(to_child_pipe[1], "ping", sizeof("ping"));
    close(to_child_pipe[1]);

    read(to_parent_pipe[0], buf, sizeof(buf));
    printf("%d: received %s\n", getpid(), buf);
    close(to_parent_pipe[0]);

    wait(0);
  }

  exit(0);
}
