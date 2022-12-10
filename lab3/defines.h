// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

#pragma once

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
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

// File handling block size
#define FILE_HANDLE_BLOCK_SIZE 16

// Key size
#define KEY_SIZE 16

#define PDF 0
#define JPG 1

#define ATTACK 0
#define RESTORE 1

typedef struct _FILE_INFO {
  char filename[DIR_LENGTH];    // filename.jpg
  int action;                   // ATTACK / RESTORE flag
  int filetype;                 // JPG / PNG flag
  unsigned char key[KEY_SIZE];  // for encryption / decryption
} FILE_INFO;

static int processed_pdf_count = 0;
static int processed_jpg_count = 0;