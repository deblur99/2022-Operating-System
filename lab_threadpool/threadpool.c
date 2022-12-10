#include "threadpool.h"

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3
#define TRUE 1

typedef struct {
  void (*function)(void *p);
  void *data;
} task;

// the work queue
task workqueue[QUEUE_SIZE];

int front;  // front of the queue
int rear;   // rear of the queue
int count;  // number of elements in the queue

// the worker bees
pthread_t tid[NUMBER_OF_THREADS];
sem_t *sem;
pthread_mutex_t mutex;

task dequeue() {
  task t;

  pthread_mutex_lock(&mutex);
  t = workqueue[front];
  front = (front + 1) % QUEUE_SIZE;
  pthread_mutex_unlock(&mutex);

  return t;
}

void *worker(void *param) {
  while (TRUE) {
    printf("%d awaiting a task ....\n", pthread_self());
    // 1. ������ sem�� ���� P���� �ڵ� ����
    sem_wait(sem);

    printf("%d got a task to do\n", pthread_self());
    // 2. work queue�κ��� �۾� �ϳ� �������� dequeue ����
    task work = dequeue();
    execute(work.function, work.data);
    usleep(10000);
    pthread_testcancel();
  }

  pthread_exit(0);
}

void execute(void (*func)(void *param), void *param) {
  // 3. func�� param�� ���ڷ��Ͽ� �����ϴ� �ڵ� ����
  func(param);
}

// workqueue�� t ���� �õ�
// ������ 1 ����, �� ���� �����ϸ� 0 ����
int enqueue(task t) {
  int ret_val = 0;
  // 3. workqueue�� ���� ��Ÿ�� ������ ���� mutex lock
  pthread_mutex_lock(&mutex);

  // 4. count < QUEUE_SIZE �� ��� rear �ε��� ��ġ�� t ����
  //      rear �ε��� ���� and mod QUEUE_SIZE
  //      count ����
  //      ret_val�� 1 ����
  if (count < QUEUE_SIZE) {
    workqueue[rear] = t;
    rear = (rear + 1) % QUEUE_SIZE;
    count++;
    ret_val = 1;
  }

  // 5. mutext lock ����
  pthread_mutex_unlock(&mutex);

  // 6. ret_val ����
  return ret_val;
}

int pool_submit(void (*somefunction)(void *p), void *p) {
  task t;
  // 1. t�� func�� param ���� �����ϱ�
  t.function = somefunction;
  t.data = p;

  // 2.enqueue(t)�� ���� (1) �ϸ� ������ sem�� ���� V����
  // and return QUEUE_SUCCESS
  // �����ϸ� return QUEUE_REJECTED
  if (enqueue(t)) {
    sem_post(sem);
    return QUEUE_SUCCESS;
  } else {
    return QUEUE_REJECTED;
  }
}

void pool_init(void) {
  // 1. mutex �ʱ�ȭ
  // 2. ������ sem �ʱ�ȭ (count = 0)
  // 3. NUMBER_OF_THREADS ��ŭ ������ ����
  // 4. start routine : worker
  pthread_mutex_init(&mutex, NULL);
  sem = sem_open("lab_sem", O_CREAT);
  for (int i = 0; i < NUMBER_OF_THREADS; i++) {
    pthread_create(&tid[i], NULL, worker, NULL);
  }
}

// worker thread�� �����
// cancellation points :
// pthread_testcancel(), pthread_join(), sem_wait(),..
// worker() �Լ� �ȿ��� execute ������ �Ŀ� pthread_testcancel()�� ����
void pool_shutdown(void) {
  for (int i = 0; i < NUMBER_OF_THREADS; i++) {
    pthread_cancel(tid[i]);
  }
}
