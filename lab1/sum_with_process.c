#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _INPUT_DATA {
  int x;
  int y;
  int interval;
} INPUT_DATA;

// 실행 인자가 3개인지 -> 값이 모두 숫자인지 검증
bool isValidInput(int argc, char *argv[]) {
  if (argc != 4) {
    return false;
  }

  for (int i = 1; i < argc; i++) {
    if (!atoi(argv[i])) {
      return false;
    }
  }

  return true;
}

INPUT_DATA *parseInputData(int argc, char *argv[]) {
  INPUT_DATA *inputData = (INPUT_DATA *)malloc(sizeof(INPUT_DATA));
  inputData->x = atoi(argv[1]);
  inputData->y = atoi(argv[2]);
  inputData->interval = atoi(argv[3]);

  return inputData;
}

// debug
void showInputData(INPUT_DATA *input_data) {
  printf("%d ", input_data->x);
  printf("%d ", input_data->y);
  printf("%d ", input_data->interval);
  printf("\n");
}

bool freeInputData(INPUT_DATA *input_data) {
  if (input_data != NULL) {
    free(input_data);
    return true;
  } else {
    return false;
  }
}

int main(int argc, char *argv[]) {
  if (!isValidInput(argc, argv)) {
    printf("invalid input. valid exec command: ./sum_with_process 1 10 2\n");
    return -1;
  }

  INPUT_DATA *input_data = parseInputData(argc, argv);

  showInputData(input_data);  // debug

  freeInputData(input_data);

  return 0;
}