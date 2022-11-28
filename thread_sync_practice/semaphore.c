// 일반적으로, 세마포어는 공유 자원 N개와 스레드 M개 사이에 동기화를 수행하는
// 역할을 한다. 즉, N:M 관계!

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define NO 0
#define MAX_COUNTER 3

sem_t toiletSem;

void *guestThread(void *arg) {
  int cnt = -1;

  sem_wait(&toiletSem);  // P 연산. 현재 접근 가능한 공유 자원의 개수를 1 낮춤.
  sem_getvalue(&toiletSem, &cnt);

  printf("Customer %s enters to the toilet... Semaphore Counter = %d\n",
         (char *)arg, cnt);
  sleep(1);

  sem_post(&toiletSem);  // V 연산. 현재 접근 가능한 공유 자원의 개수를 1
                         // 올리고, 대기 중인 다른 스레드들에게 알림을 보낸다.
  sem_getvalue(&toiletSem, &cnt);

  printf("Customer %s exits from the toilet... Semaphore Counter = %d\n",
         (char *)arg, cnt);
}

int main(int argc, char *argv[]) {
  int counter = -1;
  char *name[] = {"1", "2", "3", "4", "5"};
  pthread_t tList[5];

  // 세마포어 초기화 : MAX_COUNTER 명이 동시에 사용
  sem_init(&toiletSem, NO, MAX_COUNTER);
  sem_getvalue(&toiletSem, &counter);
  printf("Semaphore Counter = %d\n", counter);

  for (int i = 0; i < 5; i++) {
    pthread_create(&tList[i], NULL, guestThread, name[i]);
  }

  for (int i = 0; i < 5; i++) {
    // Remark: pthread_join()의 첫 번째 인자는 포인터 타입이 아님에 유의하자.
    pthread_join(tList[i], NULL);
  }

  sem_getvalue(&toiletSem, &counter);
  printf("Semaphore Counter = %d\n", counter);
  sem_destroy(&toiletSem);

  return 0;
}