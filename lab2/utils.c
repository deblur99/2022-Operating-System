// 학번: 32184893
// 학과: 모바일시스템공학과
// 이름: 한현민

#include "utils.h"

void printAsciiArtOnEncryption() {
  FILE *fp = fopen("./note_enc.txt", "r");
  char buf[1024] = {
      0,
  };

  fseek(fp, 0, SEEK_END);
  int fileSize = ftell(fp);
  rewind(fp);

  fread(buf, fileSize, 1, fp);
  printf("%s\n", buf);

  fclose(fp);
}

void printAsciiArtOnDecryption() {
  FILE *fp = fopen("./note_dec.txt", "r");
  char buf[10240] = {
      0,
  };

  fseek(fp, 0, SEEK_END);
  int fileSize = ftell(fp);
  rewind(fp);

  fread(buf, fileSize, 1, fp);
  printf("%s\n", buf);

  fclose(fp);
}