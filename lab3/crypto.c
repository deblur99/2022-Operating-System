// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

#include "crypto.h"

unsigned char *aes_128_encryption(unsigned char *plainText,
                                  unsigned char *userKey) {
  AES_KEY key;
  unsigned char initKey[BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                       0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                       0x0c, 0x0d, 0x0e, 0x0f};
  for (int i = 0; i < BLOCK_SIZE; i++) {
    initKey[i] = userKey[i];
  }

  unsigned char *cipherText =
      (unsigned char *)malloc(sizeof(unsigned char) * BLOCK_SIZE);
  memset(cipherText, 0, BLOCK_SIZE);

  // AES 암호화 키 스케줄링
  if (AES_set_encrypt_key(initKey, sizeof(initKey) * 8, &key) < 0) {
    return NULL;
  };

  // AES 암호화 : 평문, 암호문, 비밀키
  AES_encrypt(plainText, cipherText, &key);

  free(plainText);

  return cipherText;
}

unsigned char *aes_128_decryption(unsigned char *cipherText,
                                  unsigned char *userKey) {
  AES_KEY key;
  unsigned char initKey[BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                       0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                       0x0c, 0x0d, 0x0e, 0x0f};
  for (int i = 0; i < BLOCK_SIZE; i++) {
    initKey[i] = userKey[i];
  }

  unsigned char *plainText =
      (unsigned char *)malloc(sizeof(unsigned char) * BLOCK_SIZE);
  memset(plainText, 0, BLOCK_SIZE);

  // AES 복호화 키 스케줄링
  if (AES_set_decrypt_key(initKey, sizeof(initKey) * 8, &key) < 0) {
    return NULL;
  };
  // AES 복호화 : 암호문, 평문, 키
  AES_decrypt(cipherText, plainText, &key);

  // 평문 부분 추출
  unsigned char buf[2] = {cipherText[0], '\0'};
  plainText = (unsigned char *)strtok(plainText, buf);

  free(cipherText);

  return plainText;
}