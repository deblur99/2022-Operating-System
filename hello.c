#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  unsigned char c = rand();

  printf("%c %d\n", c, (int)c);

  return 0;
}