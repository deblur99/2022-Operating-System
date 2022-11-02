// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

// 파일 암호화/복호화 관
#pragma once

#include <openssl/aes.h>

#define BLOCK_SIZE 16

char *aes_128_encryption(char *plainText);

char *aes_128_decryption(char *cipherText);
