#include "input_data_handler.h"

INPUT_DATA *parseInputData(int argc, char *argv[]) {
  INPUT_DATA *inputData = (INPUT_DATA *)malloc(sizeof(INPUT_DATA));
  inputData->x = atoi(argv[1]);
  inputData->y = atoi(argv[2]);
  inputData->interval = atoi(argv[3]);

  return inputData;
}

bool freeInputData(INPUT_DATA *input_data) {
  if (input_data != NULL) {
    free(input_data);
    return true;
  } else {
    return false;
  }
}

int getSumFromInputData(INPUT_DATA *input_data) {
  int sum = 0;

  for (int i = input_data->x; i <= input_data->y; i += input_data->interval) {
    sum += i;
  }

  return sum;
}