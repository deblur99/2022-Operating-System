#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"

struct data {
  int a;
  int b;
};

void add(void *param) {
  struct data *input;
  // 1.param을 type casting하여 input에 할당
  input = (struct data *)param;
  // 2.input으로부터 a + b = 결과 출력하기
  printf("%d + %d = %d\n", input->a, input->b, input->a + input->b);
}

int main(void) {
  struct data work1;
  work1.a = 10;
  work1.b = 10;
  struct data work2;
  work2.a = 1;
  work2.b = 2;
  struct data work3 = {.a = 3, .b = 4};

  // 3.각자 data 하나씩 추가하기
  struct data work4 = {10, 20};

  // initialize the thread pool
  pool_init();

  // submit the work to the queue
  pool_submit(add, &work1);
  pool_submit(add, &work2);
  pool_submit(add, &work3);

  // 4. thread pool에 추가한 work submit하기
  pool_submit(add, &work4);

  sleep(5);
  pool_shutdown();

  return 0;
}
