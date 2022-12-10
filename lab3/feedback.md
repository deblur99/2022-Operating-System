-   지난 과제 피드백

*   local vairable에 대한 pointer를 리턴함
*   local pointer variable free 없음
*   인자로 들어온 변수를 free함
*   복호화 연산 후 파일 크기 복귀 안함
*   동일 파일에 대해 오픈 중복 실시

-   해결한 것

*   스레드 생성 구조 문제 -> create 다 한 다음에 join 해줘야 함 (critical problem)

-   3번 과제 요구사항

1. 2번 과제 기반으로 thread pool 적용하여 암호화, 복호화 구현하기

-   2개의 스레드를 돌리고, 각각 .jpg, .pdf를 암호화 / 복호화
-   암호화 / 복호화 시, 파일 정보를 thread pool에 저장하고 시작
-   파일 정보 submit 후 join ->처리 후 종료

2. 디렉토리 구조

-   .jpg, .pdf 파일 수는 각각 5개씩 총 10개 -> 기존 파일을 복붙하여 10개로 늘리기
-   .jpg, .pdf 확장자가 아닌 다른 확장자의 파일 10개 -> 알아서 가져오기 (.png, .exe, .mp3 등등)

3. Thread pool 구현에 대하여

-   실습 내용 바탕으로 구현할 것 (lab_threadpool)
-   Thread pool에서 생성한 thread의 수는 3개

    -   WorkQueue의 크기는 10
    -   암호화 / 복호화 : WorkQueue에 삽입된 파일 정보를 바탕으로, 알맞은 암호화 / 복호화 작업 수행 (큐에 있는 파일정보 가져와서)
    -   pool_init : mutex, semaphore 초기화 및 스레드 3개 생성
    -   pool_submit : 암호화 / 복호화를 위한 함수 정보, 인자 정보 전달, enqueue
    -   pool_submit에 넘길 인자 : key, 파일명
    -   worker : function pointer, 공격인지 복원인지에 따라 적절히 할당
        -   dequeue, execute, pthread_textcancel...
    -   execute : 암호화 / 복호화 작업
    -   pool_shutdown : pthread_cancel 수행

-   To DOs
    1. target file 수집하기 (jpg, pdf 총 10개, 그 외 확장자 10개)
    2. 구현하기
    -   jpg 처리 스레드, pdf 처리 스레드는 파일정보 20개를 각자 가져와서,
        자기가 담당한 확장자만 처리하고, 나머지는 넘어간다.
