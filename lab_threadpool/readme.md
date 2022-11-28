1. mutex, semaphore, worker 함수 3개 정의
2. p 연산 sem = 0에서 sem = 1로 바꾸기
3. Dequeue 연산 f, p 실행한 후 pthread_testcancel 실행하고 v 연산 (sem_post)
4. pool_shutdown으로 스레드 풀 종료
