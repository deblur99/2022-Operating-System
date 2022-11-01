// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "crypto.h"
#include "utils.h"

// Maximum size of string buffer
#define BUF_SIZE 256
// Maximum amount of files from ./oshw2_sample/dkuware/target
#define MAX_FILE_AMOUNT 128
// Maximum length of each filename
#define DIR_LENGTH 256

// =========== handles file directory ===========
// For counting file count to be saved in directory string array
static int fileCount = 0;
// Array of files from ./oshw2_sample/dkuware/target
char directory[MAX_FILE_AMOUNT][DIR_LENGTH] = {
    {
        0,
    },
};

void readFileList();

// =========== handles file encrpytion / decryption ===========
static __thread int processed_pdf_count = 0;
static __thread int processed_jpg_count = 0;

void *encryption_pdfs(void *param);

void *encryption_jpgs(void *param);

void *decryption_pdfs(void *param);

void *decryption_jpgs(void *param);

int main(int argc, char *argv[]) {
  readFileList();  // read all files in target directory and save their
                   // name into "directory" string array

  printf("%d\n", fileCount);

  for (int i = 0; i < fileCount; i++) {
    printf("%s\n", directory[i]);
  }

  return 0;
}

void readFileList() {
  char command[BUF_SIZE] = "ls ./target";

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

void *encryption_pdfs(void *param) { return NULL; }

void *encryption_jpgs(void *param) { return NULL; }

void *decryption_pdfs(void *param) { return NULL; }

void *decryption_jpgs(void *param) { return NULL; }