#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 같은 프로세스 내 다른 스레드들이 공유하는 메모리 영역 : code, data, heap
// 공유하지 않는 영역 : register, stack
pthread_mutex_t mutex;  // mutex 변수는 모든 스레드에서 동일한 주소로 접근해야
                        // 하기 때문에 전역 변수로 선언한다.
                        // entry code ~ exit code 사이에 모든 공유 자원을 잠금
int sum = 0;  // data segment

void *worker(void *param);

int main(int argc, char *argv[]) {
  pthread_t th[2];

  // 반복문에서 조심해야 할 것
  // pthread_create 문 따로, pthread_join 문 따로 해줘야 한다.
  for (int i = 0; i < 2; i++) {
    pthread_create(&th[i], NULL, worker, NULL);
  }

  for (int i = 0; i < 2; i++) {
    pthread_join(th[i], NULL);
  }

  printf("result : %d\n", sum);

  return 0;
}

void *worker(void *params) {
  pthread_mutex_init(&mutex, NULL);

  for (int i = 0; i < 1000000; i++) {
    pthread_mutex_lock(&mutex);    // entry code
    sum += 1;                      // critical section on shared memory
    pthread_mutex_unlock(&mutex);  // exit code
  }

  pthread_mutex_destroy(&mutex);
}