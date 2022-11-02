// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

#include "crypto.h"

#include <stdio.h>
#include <string.h>

char *aes_128_encryption(char *plainText) {
  AES_KEY key;
  unsigned char initKey[BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                       0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                       0x0c, 0x0d, 0x0e, 0x0f};

  unsigned char cipherText[BLOCK_SIZE] = {
      0x00,
  };

  printf("before calling encryption: %s / length: %ld / size: %ld\n", plainText,
         strlen(plainText), sizeof(plainText));  // debug

  // AES 암호화 키 스케줄링
  if (AES_set_encrypt_key(initKey, sizeof(initKey) * 8, &key) < 0) {
    return NULL;
  };
  // AES 암호화 : 평문, 암호문, 비밀키
  AES_encrypt(plainText, cipherText, &key);

  printf("after calling encryption: %s / length: %ld / size: %ld\n", cipherText,
         strlen(cipherText), sizeof(cipherText));  // debug

  return cipherText;
}

char *aes_128_decryption(char *cipherText) {
  AES_KEY key;
  unsigned char initKey[BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                       0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                       0x0c, 0x0d, 0x0e, 0x0f};

  unsigned char plainText[BLOCK_SIZE] = {
      0x00,
  };

  // AES 복호화 키 스케줄링
  if (AES_set_decrypt_key(initKey, sizeof(initKey) * 8, &key) < 0) {
    return NULL;
  };
  // AES 복호화 : 암호문, 평문, 키
  AES_decrypt(cipherText, plainText, &key);

  return plainText;
}