/*
 * 3_fifo_read_modified.c — FIFO로 메시지를 받는 프로그램
 *
 * [컴파일] gcc -Wall -Wextra -o 3_fifo_read 3_fifo_read_modified.c
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define FIFO_PATH "/tmp/sysprog_fifo"

int main(void)
{
    /* FIFO 생성: 이미 존재하면 그대로 사용 */
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    printf("보내는 쪽을 기다립니다... (%s)\n", FIFO_PATH);


    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) { perror("open"); return 1; }

    printf("상대방과 연결되었습니다! 데이터를 읽습니다...\n"); // 연결 확인용

    char buf[128];
    ssize_t n;
    
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("받은 조각: %s\n", buf); // 받을 때마다 즉시 한 줄씩 출력
    }

    printf("수신이 모두 끝났습니다.\n"); // 종료 확인용

    close(fd);
    unlink(FIFO_PATH);
    return 0;
}
