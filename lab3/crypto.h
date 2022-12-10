// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

// 파일 암호화/복호화 관
#pragma once

#include <openssl/aes.h>

#define BLOCK_SIZE 16

unsigned char *aes_128_encryption(unsigned char *plainText,
                                  unsigned char *initKey);

unsigned char *aes_128_decryption(unsigned char *cipherText,
                                  unsigned char *initKey);
