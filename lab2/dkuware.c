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
static int pdfFileCount = 0;
char pdfList[MAX_FILE_AMOUNT][DIR_LENGTH] = {
    {
        0,
    },
};

static int jpgFileCount = 0;
char jpgList[MAX_FILE_AMOUNT][DIR_LENGTH] = {
    {
        0,
    },
};

int inputValidate(int argc, char *argv[]);

void readFileList();

// =========== handles file encrpytion / decryption ===========
static __thread int processed_pdf_count = 0;
static __thread int processed_jpg_count = 0;

void *encryption_pdfs(void *param);

void *encryption_jpgs(void *param);

void *decryption_pdfs(void *param);

void *decryption_jpgs(void *param);

int main(int argc, char *argv[]) {
  if (!inputValidate(argc, argv)) {
    return -1;
  }

  readFileList();  // read all files in target directory and save their
                   // name into each string array "pdfList" and "jpgList"
                   // depending on its filetype

  // debug
  printf("pdfs: %d, jpgs: %d\n", pdfFileCount, jpgFileCount);

  // debug
  printf("=== pdfFile ===\n");
  for (int i = 0; i < pdfFileCount; i++) {
    printf("%s\n", pdfList[i]);
  }

  // debug
  printf("=== jpgFile ===\n");
  for (int i = 0; i < jpgFileCount; i++) {
    printf("%s\n", jpgList[i]);
  }

  // read a single jpg file by 16 bytes

  return 0;
}

int inputValidate(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Error: Invalid Input\n");
    printf("Input example: <./filename> <attack/restore> <password>\n");
    return 0;
  }

  if (strcmp(argv[1], "attack") && strcmp(argv[1], "restore")) {
    printf("Error: Invalid Input\n");
    printf("Input example: <./filename> <attack/restore> <password>\n");
    return 0;
  }

  if (strcmp(argv[2], "password")) {
    printf("Error: Inputted password is not correct.\n");
    return 0;
  }

  return 1;
}

void readFileList() {
  char command[BUF_SIZE] = "ls ./target";
  char fileItem[DIR_LENGTH] = {
      0,
  };

  char *fileTypeLower;  // .pdf, .jpg
  char *fileTypeUpper;  // .PDF, .JPG

  FILE *pipe = popen(command, "r");

  if (!pipe) {
    perror("Command execution error");
    return;
  } else {
    while (fscanf(pipe, "%s", fileItem) > 0) {
      // when fileCount exceeds maximum index of directoryItem array, then
      // escape the loop statement.
      if (pdfFileCount >= MAX_FILE_AMOUNT || jpgFileCount >= MAX_FILE_AMOUNT) {
        break;
      }

      fileTypeLower = strstr(fileItem, ".pdf\0");
      if (fileTypeLower != NULL) {
        strcpy(pdfList[pdfFileCount++], fileItem);
        memset(fileItem, 0, BUF_SIZE);
        continue;
      }

      fileTypeUpper = strstr(fileItem, ".PDF\0");
      if (fileTypeUpper != NULL) {
        strcpy(pdfList[pdfFileCount++], fileItem);
        memset(fileItem, 0, BUF_SIZE);
        continue;
      }

      fileTypeLower = strstr(fileItem, ".jpg\0");
      if (fileTypeLower != NULL) {
        strcpy(jpgList[jpgFileCount++], fileItem);
      }

      fileTypeUpper = strstr(fileItem, ".JPG\0");
      if (fileTypeUpper != NULL) {
        strcpy(jpgList[jpgFileCount++], fileItem);
      }

      memset(fileItem, 0, BUF_SIZE);
    }
  }

  pclose(pipe);
}

void *encryption_pdfs(void *param) { return NULL; }

void *encryption_jpgs(void *param) { return NULL; }

void *decryption_pdfs(void *param) { return NULL; }

void *decryption_jpgs(void *param) { return NULL; }