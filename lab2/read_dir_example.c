#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIR_SIZE 1024

#define BUF_SIZE 128

char directory[DIR_SIZE] = {
    0,
};

void readFileList() {
  char buffer[BUF_SIZE] = {
      0,
  };

  FILE *pipe = popen("ls", "r");
  if (!pipe) {
    perror("Command execution error");
    return;
  } else {
    while (fgets(buffer, BUF_SIZE, pipe) != NULL) {
      strcat(directory, buffer);
      memset(buffer, 0, BUF_SIZE);
    }
  }

  pclose(pipe);
}

int main(int argc, char *argv[]) {
  readFileList();

  printf("%s\n", directory);

  return 0;
}