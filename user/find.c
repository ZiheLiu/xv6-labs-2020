#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* path, char* name, char* target) {
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512], *p;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE:
      if (!strcmp(name, target)) {
        printf("%s\n", path);
      }
      break;

    case T_DIR:
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
        fprintf(2, "find: path too long\n");
        break;
      }

      strcpy(buf, path);
      p = buf + strlen(buf);
      *(p++) = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
          continue;
        }

        if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) {
          continue;
        }

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, de.name, target);
      }
      break;
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "too few or too many args\n");
    exit(0);
  }

  find(argv[1], "", argv[2]);

  exit(0);
}