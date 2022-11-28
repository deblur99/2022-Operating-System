//1. header guard 추가하기 


#define QUEUE_SUCCESS   0
#define QUEUE_REJECTED  1


void execute(void (*func)(void *param), void *param);
int pool_submit(void (* func)(void *param), void *param);
void *worker(void *param);
void pool_init(void);
void pool_shutdown(void);



