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

// File handling block size
#define FILE_HANDLE_BLOCK_SIZE 16

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

static char *rand_string(char *str, size_t size) {
  const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  if (size) {
    --size;
    for (size_t n = 0; n < size; n++) {
      int key = rand() % (int)(sizeof(charset) - 1);
      str[n] = charset[key];
    }
    str[size] = '\0';
  }
  printf("%s\n", str);  // debug
  return str;
}

void *encryption_pdfs(void *param);

void *encryption_jpgs(void *param);

void *decryption_pdfs(void *param);

void *decryption_jpgs(void *param);

int main(int argc, char *argv[]) {
  if (!inputValidate(argc, argv)) {
    return -1;
  }

  char *fileHandleMode = argv[1];
  char *password = argv[2];

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

  // making threads and run them
  pthread_t pdfHandleThread;
  pthread_attr_t pdfHandleThreadStatus;
  pthread_attr_init(&pdfHandleThreadStatus);

  pthread_t jpgHandleThread;
  pthread_attr_t jpgHandleThreadStatus;
  pthread_attr_init(&jpgHandleThreadStatus);

  if (strcmp(fileHandleMode, "attack")) {
    pthread_create(&pdfHandleThread, &pdfHandleThreadStatus, encryption_pdfs,
                   NULL);
    pthread_join(pdfHandleThread, NULL);

    pthread_create(&jpgHandleThread, &jpgHandleThreadStatus, encryption_jpgs,
                   NULL);
    pthread_join(jpgHandleThread, NULL);

  } else if (strcmp(fileHandleMode, "restore")) {
    pthread_create(&pdfHandleThread, &pdfHandleThreadStatus, decryption_pdfs,
                   NULL);
    pthread_join(pdfHandleThread, NULL);

    pthread_create(&jpgHandleThread, &jpgHandleThreadStatus, decryption_jpgs,
                   NULL);
    pthread_join(jpgHandleThread, NULL);
  }

  printf("all processes are completed.\n");  // debug

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

void *encryption_pdfs(void *param) {
  for (int i = 0; i < pdfFileCount; i++) {
    // open a single jpg file
    char *fileDir = (char *)malloc(sizeof(char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, pdfList[i]);
    FILE *fp = fopen(fileDir, "rb");

    // read browsed jpg file by 16 bytes
    char *plainText = (char *)malloc(sizeof(char) * FILE_HANDLE_BLOCK_SIZE);

    // get size value
    int plainTextSize = fread(plainText, 1, FILE_HANDLE_BLOCK_SIZE, fp);
    int plainTextStringSize = strlen(plainText);

    // check plainText size is 16 or not.
    // if the size is less than 16, then give it zero padding.
    if (plainTextStringSize == FILE_HANDLE_BLOCK_SIZE) {
      printf("%s\n", plainText);  // debug
    } else if (plainTextStringSize >= 0) {
      // give plainText zero padding
      for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE - plainTextStringSize; i++) {
        strcat(plainText, "0");
      }
      printf("%s\n", plainText);  // debug
    } else {
      perror("File read failure");  // exception
    }

    printf("%d %ld\n", plainTextSize, strlen(plainText));  // debug

    // do XOR calculation on plainText with randomly generated mask
    char *mask = (char *)malloc(sizeof(char) * FILE_HANDLE_BLOCK_SIZE);
    mask = rand_string(mask, FILE_HANDLE_BLOCK_SIZE);

    // 11/2 (수) 할 일
    // 1. 무작위 생성한 16바이트짜리 mask 생성 후 XOR 연산하여 cipherText
    // 생성하여 덮어쓰기
    // 2. mask는 AES-128 암호화하여 파일의 맨 뒷 부분에 붙이기
    // (파일 포인터 새로 만들고, 이때 모드는 wba로 하여 fwrite 연산하기)

    // 3. 복호화 - 맨 앞부분, 뒷부분에서 각각 cipherText, 암호화된 mask 읽어오기
    // 4. mask는 password 값과 같이 AES-128 복호화하여 평문 mask 얻기
    // 5. 평문 mask를 cipherText와 XOR 연산하여 plainText 얻고, 이를 맨 앞
    // 16바이트에 덮어쓰기

    free(plainText);
    fclose(fp);
    free(fileDir);
  }
}

void *encryption_jpgs(void *param) {
  for (int i = 0; i < jpgFileCount; i++) {
    // open a single jpg file
    char *fileDir = (char *)malloc(sizeof(char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, jpgList[i]);
    FILE *fp = fopen(fileDir, "rb");

    // read browsed jpg file by 16 bytes
    char *plainText = (char *)malloc(sizeof(char) * FILE_HANDLE_BLOCK_SIZE);

    // get size value
    int plainTextSize = fread(plainText, 1, FILE_HANDLE_BLOCK_SIZE, fp);
    int plainTextStringSize = strlen(plainText);

    // check plainText size is 16 or not.
    // if the size is less than 16, then give it zero padding.
    if (plainTextStringSize == FILE_HANDLE_BLOCK_SIZE) {
      printf("%s\n", plainText);  // debug
    } else if (plainTextStringSize >= 0) {
      // give plainText zero padding
      for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE - plainTextStringSize; i++) {
        strcat(plainText, "0");
      }
      printf("%s\n", plainText);  // debug
    } else {
      perror("File read failure");  // exception
    }

    printf("%d %ld\n", plainTextSize, strlen(plainText));  // debug

    free(plainText);
    fclose(fp);
    free(fileDir);
  }
}

void *decryption_pdfs(void *param) {
  for (int i = 0; i < pdfFileCount; i++) {
    // open a single jpg file
    char *fileDir = (char *)malloc(sizeof(char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, pdfList[i]);
    FILE *fp = fopen(fileDir, "rb");

    // read browsed jpg file by 16 bytes
    char *plainText = (char *)malloc(sizeof(char) * FILE_HANDLE_BLOCK_SIZE);

    // get size value
    int plainTextSize = fread(plainText, 1, FILE_HANDLE_BLOCK_SIZE, fp);
    int plainTextStringSize = strlen(plainText);

    // check plainText size is 16 or not.
    // if the size is less than 16, then give it zero padding.
    if (plainTextStringSize == FILE_HANDLE_BLOCK_SIZE) {
      printf("%s\n", plainText);  // debug
    } else if (plainTextStringSize >= 0) {
      // give plainText zero padding
      for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE - plainTextStringSize; i++) {
        strcat(plainText, "0");
      }
      printf("%s\n", plainText);  // debug
    } else {
      perror("File read failure");  // exception
    }

    printf("%d %ld\n", plainTextSize, strlen(plainText));  // debug

    free(plainText);
    fclose(fp);
    free(fileDir);
  }
}

void *decryption_jpgs(void *param) {
  for (int i = 0; i < jpgFileCount; i++) {
    // open a single jpg file
    char *fileDir = (char *)malloc(sizeof(char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, jpgList[i]);
    FILE *fp = fopen(fileDir, "rb");

    // read browsed jpg file by 16 bytes
    char *plainText = (char *)malloc(sizeof(char) * FILE_HANDLE_BLOCK_SIZE);

    // get size value
    int plainTextSize = fread(plainText, 1, FILE_HANDLE_BLOCK_SIZE, fp);
    int plainTextStringSize = strlen(plainText);

    // check plainText size is 16 or not.
    // if the size is less than 16, then give it zero padding.
    if (plainTextStringSize == FILE_HANDLE_BLOCK_SIZE) {
      printf("%s\n", plainText);  // debug
    } else if (plainTextStringSize >= 0) {
      // give plainText zero padding
      for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE - plainTextStringSize; i++) {
        strcat(plainText, "0");
      }
      printf("%s\n", plainText);  // debug
    } else {
      perror("File read failure");  // exception
    }

    printf("%d %ld\n", plainTextSize, strlen(plainText));  // debug

    free(plainText);
    fclose(fp);
    free(fileDir);
  }
}