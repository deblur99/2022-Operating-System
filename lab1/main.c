// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

#include "defines.h"
#include "input_data_handler.h"
#include "input_validate.h"

int main(int argc, char *argv[]) {
  if (!isValidInput(argc, argv)) {
    return -1;
  }

  INPUT_DATA *input_data = parseInputData(argc, argv);

  // make child process from here
  pid_t pid;
  int status;  // for tracing the status of pid.

  pid = fork();

  // concepts:
  // pid determines between parent or child process
  // status determines the status code of child process

  // > 0 means parent process, == 0 means child process, and < 0 means exception
  if (pid > 0) {
    wait(&status);  // wait until the child process is terminated
    int exitCode = WEXITSTATUS(status);  // after the child process is
                                         // terminated, pid value is updated.

    printf("Parent says that it is ");
    switch (exitCode) {
      case 0:
        printf("less than 1000.\n");
        break;

      case 1:
        printf("equal to 1000.\n");
        break;

      case 2:
        printf("greater than 1000.\n");
        break;
    }
  } else if (pid == 0) {
    int sum = getSumFromInputData(input_data);

    printf(
        "Child says that the sum of numbers from %d to %d with interval of %d "
        "is %d.\n",
        input_data->x, input_data->y, input_data->interval, sum);

    if (sum < 1000) {
      exit(0);
    } else if (sum == 1000) {
      exit(1);
    } else {
      exit(2);
    }
  } else {
    perror("failed to make child process");
  }

  freeInputData(input_data);

  return 0;
}