-   학번: 32184893
-   학과: 모바일시스템공학과
-   이름: 한현민

-   특이사항: 모든 기능 잘 작동하나, pdf_sample2는 공격했음에도 파일이 보임.
    그러나, Hex Editor로 덤프 떠서 확인해보면
    맨 앞 부분 16바이트 덮어쓰기 및 맨 뒷 부분 16바이트 마스크 쓰기 작업이
    다른 파일들과 동일하게 잘 되어 있음.

-   빌드 방법
    .o 파일 및 실행 파일 (./dkuware) 생성: 터미널을 압축 해제한 디렉토리에 위치해
    놓은 상태에서 make all 입력 .o 파일 및 실행 파일 (./dkuware) 제거: 터미널을
    압축 해제한 디렉토리에 위치해 놓은 상태에서 make clean 입력

-   실행 방법 (password 인자는 비밀번호를 의미하며, 비밀번호는 password 이다.)

./target 디렉토리 파일 암호화: 터미널을 압축 해제한 디렉토리에 위치해 놓은
상태에서 ./dkuware attack password 입력

./target 디렉토리 파일 복호화: 터미널을 압축 해제한 디렉토리에 위치해 놓은
상태에서 ./dkuware restore password 입력