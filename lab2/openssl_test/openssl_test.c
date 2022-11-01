// LDFLAGS 자리에 -lpthread -lcrypto -lssl
#include <openssl/aes.h>
#include <stdio.h>

#define BLOCK_SIZE 16

void dump_hex(char *label, unsigned char buf[], int size) {
  printf("%s", label);

  for (int i = 0; i < size; i++) printf("%02X", buf[i]);

  printf("\n");
}

void aes_128_encrypt_test() {
  AES_KEY key;
  unsigned char init_key[BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                        0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                        0x0c, 0x0d, 0x0e, 0x0f};
  unsigned char plaintext[BLOCK_SIZE] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                                         0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
                                         0xcc, 0xdd, 0xee, 0xff};
  unsigned char ciphertext[BLOCK_SIZE];

  // AES 암호화 키 스케줄링
  if (AES_set_encrypt_key(init_key, sizeof(init_key) * 8, &key) < 0) {
    return;
  };
  // AES 암호화 : 평문, 암호문, 비밀키
  AES_encrypt(plaintext, ciphertext, &key);
  // 암호문 출력 후 확인
  dump_hex("ciphertext : ", ciphertext, 16);
}

void aes_128_decrypt_test() {
  AES_KEY key;
  unsigned char init_key[BLOCK_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                        0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                        0x0c, 0x0d, 0x0e, 0x0f};
  unsigned char ciphertext[BLOCK_SIZE] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b,
                                          0x04, 0x30, 0xd8, 0xcd, 0xb7, 0x80,
                                          0x70, 0xb4, 0xc5, 0x5a};
  unsigned char plaintext[BLOCK_SIZE];

  // AES 복호화 키 스케줄링
  if (AES_set_decrypt_key(init_key, sizeof(init_key) * 8, &key) < 0) {
    return;
  };
  // AES 복호화 : 암호문, 평문, 키
  AES_decrypt(ciphertext, plaintext, &key);
  // 평문 출력 후 확인
  dump_hex("plaintext : ", plaintext, 16);
}

int main() {
  aes_128_encrypt_test();
  aes_128_decrypt_test();

  return 0;
}