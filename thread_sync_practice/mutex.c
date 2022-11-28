#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int sum = 0;  // data segment as shared memory.
pthread_mutex_t lock;  // mutex lock 변수는 모든 스레드에서 접근해야 하므로
                       // 마찬가지로 전역 변수로 선언 (.bss)

void *worker(void *arg) {
  printf("%s 시작 \t %d\n", (char *)arg, sum);

  for (int i = 0; i < 1000000; i++) {
    pthread_mutex_lock(&lock);    // entry code
    sum += 10;                    // critical section
    pthread_mutex_unlock(&lock);  // exit code
  }
  printf("%s 끝 \t %d\n", (char *)arg, sum);
}

void *worker2(void *arg) {
  printf("%s 시작 \t %d\n", (char *)arg, sum);

  pthread_mutex_lock(&lock);  // entry code
  for (int i = 0; i < 1000000; i++) {
    sum += 10;  // critical section
  }
  pthread_mutex_unlock(&lock);  // exit code
  printf("%s 끝 \t %d\n", (char *)arg, sum);
}

int main(int argc, char *argv[]) {
  char *name[2] = {"Hwang Geun-chul", "Hwang Ryong"};
  pthread_t tid[2];
  pthread_attr_t attr[2];

  pthread_mutex_init(&lock, NULL);

  for (int i = 0; i < 2; i++) {
    pthread_attr_init(&attr[i]);
    pthread_create(&tid[i], &attr[i], worker, name[i]);
  }

  // join 돌리기 전까지 모든 스레드가 생성되어야 하기 때문에,
  // 반복문을 분리한다.
  for (int i = 0; i < 2; i++) pthread_join(tid[i], NULL);

  // pthread_attr_init(&attr[0]);
  // pthread_attr_init(&attr[1]);

  // pthread_create(&tid[0], &attr[0], worker, name[0]);
  // pthread_create(&tid[1], &attr[1], worker, name[1]);

  // pthread_join(tid[0], NULL);
  // pthread_join(tid[1], NULL);

  printf("최종 sum = %d\n", sum);

  pthread_mutex_destroy(&lock);
}