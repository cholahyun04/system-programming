/*
 * 1_pipe_basic_modified.c — 파이프로 부모-자식이 양방향 통신(왕복)을 한다
 *
 * [컴파일·실행]
 *   gcc -Wall -Wextra -o 1_pipe 1_pipe_basic_modified.c
 *   ./1_pipe "hello world"
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>  /* toupper() 함수 사용을 위해 추가 */

int main(int argc, char *argv[])
{
    /* 커맨드라인 인자로 문자열을 받도록 확인 */
    if (argc != 2) {
        fprintf(stderr, "사용법: %s <문자열>\n", argv[0]);
        return 1;
    }

    int pipe_p2c[2]; /* 부모 -> 자식 방향 파이프 (Parent to Child) */
    int pipe_c2p[2]; /* 자식 -> 부모 방향 파이프 (Child to Parent) */

    /* 두 개의 파이프 생성 */
    if (pipe(pipe_p2c) == -1) { perror("pipe p2c"); return 1; }
    if (pipe(pipe_c2p) == -1) { perror("pipe c2p"); return 1; }

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }

    if (pid == 0) {
        /* [자식 프로세스] */
        close(pipe_p2c[1]); /* 부모->자식 파이프의 쓰기 끝 닫기 */
        close(pipe_c2p[0]); /* 자식->부모 파이프의 읽기 끝 닫기 */

        char buf[256];
        /* 부모가 보낸 문자열 읽기 */
        ssize_t n = read(pipe_p2c[0], buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            
            /* 소문자를 대문자로 변환 */
            for (int i = 0; i < n; i++) {
                buf[i] = toupper(buf[i]);
            }
            
            /* 대문자로 변환된 문자열을 자식->부모 파이프를 통해 전송 */
            write(pipe_c2p[1], buf, n); 
        }

        close(pipe_p2c[0]);
        close(pipe_c2p[1]);
        _exit(0);
    }

    /* [부모 프로세스] */
    close(pipe_p2c[0]); /* 부모->자식 파이프의 읽기 끝 닫기 */
    close(pipe_c2p[1]); /* 자식->부모 파이프의 쓰기 끝 닫기 */

    /* 1. 자식에게 커맨드라인 인자(문자열) 전송 */
    write(pipe_p2c[1], argv[1], strlen(argv[1]));
    close(pipe_p2c[1]); /* 쓰기 끝을 닫아 자식의 read가 끝나게(EOF) 만듦 */

    /* 2. 자식이 대문자로 바꿔서 보낸 문자열 수신 */
    char buf[256];
    ssize_t n = read(pipe_c2p[0], buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("[부모] 자식으로부터 돌려받은 내용: %s\n", buf);
    }

    close(pipe_c2p[0]);
    wait(NULL); /* 자식 프로세스 종료 대기 */
    
    return 0;
}