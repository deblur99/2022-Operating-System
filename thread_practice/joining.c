// compiling: gcc -target x86_64-apple-darwin22.1.0

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int sum;
void *runner(void *param);

int main(int argc, char *argv[]) {
  int rc;
  pthread_t tid;

  // pthread_create() 함수 호출 시 반환값은 쓰레드 생성 성공 시 0을 반환하고,
  // 실패 시 0이 아닌 값을 반환한다.
  if ((rc = pthread_create(&tid, NULL, runner, NULL)) != 0) {
    perror("thread creation failure");
    return -1;
  }
  printf("Main thread #%lu: sleeping for 5 secs\n",
         (unsigned long)pthread_self());
  fflush(stdout);

  sleep(5);

  pthread_exit(NULL);  // 쓰레드 강제 종료

  printf("Main thread #%lu: this will not be printed.\n",
         (unsigned long)pthread_self());

  return 0;
}

void *runner(void *param) {
  printf("thread #%lu: sleeping for 5 secs\n", (unsigned long)pthread_self());
  sleep(5);

  printf("thread #%lu: awaken and sleeping again for 5 secs\n",
         (unsigned long)pthread_self());
  sleep(5);

  printf("thread #%lu: awaken from the second sleep\n",
         (unsigned long)pthread_self());

  return NULL;
}