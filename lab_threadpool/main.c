#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"

struct data {
  int a;
  int b;
};

void add(void *param) {
  struct data *input;
  // 1.param�� type casting�Ͽ� input�� �Ҵ�
  input = (struct data *)param;
  // 2.input���κ��� a + b = ��� ����ϱ�
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

  // 3.���� data �ϳ��� �߰��ϱ�
  struct data work4 = {10, 20};

  // initialize the thread pool
  pool_init();

  // submit the work to the queue
  pool_submit(add, &work1);
  pool_submit(add, &work2);
  pool_submit(add, &work3);

  // 4. thread pool�� �߰��� work submit�ϱ�
  pool_submit(add, &work4);

  sleep(5);
  pool_shutdown();

  return 0;
}
