#include <stdio.h>

void greeting(char *name) { printf("Hello, %s!", name); }

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return -1;
  }

  greeting(argv[1]);

  return 0;
}