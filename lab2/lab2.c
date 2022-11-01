#include <openssl/aes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Maximum size of string buffer
#define BUF_SIZE 256
// Maximum amount of files from ./oshw2_sample/dkuware/target
#define MAX_FILE_AMOUNT 128
// Maximum length of each filename
#define DIR_LENGTH 256

// For counting file count to be saved in directory string array
static int fileCount = 0;
// Array of files from ./oshw2_sample/dkuware/target
char directory[MAX_FILE_AMOUNT][DIR_LENGTH] = {
    {
        0,
    },
};

void readFileList() {
  char command[BUF_SIZE] = "ls ./oshw2_sample/dkuware/target";

  char directoryItem[DIR_LENGTH] = {
      0,
  };

  FILE *pipe = popen(command, "r");
  if (!pipe) {
    perror("Command execution error");
    return;
  } else {
    while (fscanf(pipe, "%s", directoryItem) > 0) {
      // when fileCount exceeds maximum index of directoryItem array, then
      // escape the loop statement.
      if (fileCount >= MAX_FILE_AMOUNT) {
        break;
      }
      strcpy(directory[fileCount++], directoryItem);
      memset(directoryItem, 0, BUF_SIZE);
    }
  }

  pclose(pipe);
}

int main(int argc, char *argv[]) {
  readFileList();  // read all files in target directory and save their
                   // name into "directory" string array

  printf("%d\n", fileCount);

  for (int i = 0; i < fileCount; i++) {
    printf("%s\n", directory[i]);
  }

  printf("%s\n", directory[0]);

  return 0;
}