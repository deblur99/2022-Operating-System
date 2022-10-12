// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

#include "input_validate.h"

void printInputCondition(char *filename) {
  printf("[usage] %s x y interval\n", filename);
  printf("[condition] x, y, and interval are positive integers.\n");
}

bool isValidInput(int argc, char *argv[]) {
  bool isValid = true;

  // check whether the number of input data is 4 or not
  if (argc != 4) {
    printInputCondition(argv[0]);
    return false;
  }

  // check type of input data. they should be positive integers.
  for (int i = 1; i < argc; i++) {
    if (!atoi(argv[i])) {
      printInputCondition(argv[0]);
      return false;
    }
  }

  for (int i = 1; i < argc; i++) {
    if (atoi(argv[i]) <= 0) {
      switch (i) {
        case 1:
          printf("x is not a positive integer.\n");
          break;
        case 2:
          printf("y is not a positive integer.\n");
          break;
        case 3:
          printf("interval is not a positive integer.\n");
          break;
      }
      isValid = false;
    }
  }

  return isValid;
}