#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printSum();
void *calcThread(void *param);

static __thread int tSum = 5;  // TLS : thread 고유 영역
int total = 0;                 // process의 전역 변수

int main(int argc, char *argv[]) {
  char *p[2] = {"100", "200"};
  int i;
  pthread_t tid[2];
  pthread_attr_t attr[2];

  for (int i = 0; i < 2; i++) {
    pthread_attr_init(&attr[i]);
    pthread_create(&tid[i], &attr[i], calcThread, p[i]);
    printf("calcThread #%lu thread has been created.\n", tid[i]);
  }

  for (int i = 0; i < 2; i++) {
    pthread_join(tid[i], NULL);
    printf("calcThread #%lu thread has been terminated.\n", tid[i]);
  }

  printf("sum = %d\n", total);
  return 0;
}

void *calcThread(void *param) {
  printf("tSum in early thread creation = %d\n", tSum);

  int i, sum = 0;
  int n = atoi(param);

  for (i = 1; i <= n; i++) {
    sum += i;
  }

  tSum = sum;  // tSum은 TLS 변수, sum은 지역 변수 (stack) -> thread가
               // 독립적으로 가짐
  printSum();
  total +=
      sum;  // total은 전역 변수 -> process 내 모든 thread가 동일하게 참조함
}

void printSum() { printf("tSum after calculation: %d\n", tSum); }