#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *calcThread(void *param);
int sum = 0;  // main thread와 calcThread가 공유하는 전역 변수 (.data)

int main(int argc, char *argv[]) {
  pthread_t tid;        // thread의 id를 저장할 정수형 변수
  pthread_attr_t attr;  // thread 정보를 담을 구조체

  pthread_attr_init(&attr);  // default 값으로 attr 초기화
  pthread_create(&tid, &attr, calcThread, "100");  // calcThread thread 생성

  // pthread_detach(tid);
  pthread_join(tid, NULL);  // 생성한 thread가 종료할 때까지 현재 thread는 대기
  // 동기식으로 처리할 때는 join을 쓰고 비동기식으로 처리할 때는 detach를 쓴다.

  printf("calcThread has terminated\n");
  printf("sum = %d\n",
         sum);  // data segment의 값은 같은 process 내 모든 thread가 공유하므로,
                // sum 값도 업데이트됨을 확인할 수 있음
  return 0;
}

void *calcThread(void *param) {
  printf("calcThread has been started.\n");
  int to = atoi(param);
  int i;

  for (int i = 1; i <= to; i++) {
    sum += i;
  }
}