/*
 * 4_fifo_write_modified.c — FIFO로 메시지를 보내는 프로그램
 *
 * [컴파일] gcc -Wall -Wextra -o 4_fifo_write 4_fifo_write_modified.c
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/sysprog_fifo"

int main(int argc, char *argv[])
{
    /* [수정] 인자가 없을 때의 예외 처리 강화 */
    if (argc < 2) {
        fprintf(stderr, "사용법: %s <메시지1> [메시지2] ...\n", argv[0]);
        return 1;
    }

    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("open (받는 쪽을 먼저 실행했는지 확인)");
        return 1;
    }

    /* [수정] 입력받은 모든 인자를 띄어쓰기로 연결하여 전송 */
    for (int i = 1; i < argc; i++) {
        write(fd, argv[i], strlen(argv[i]));
        if (i < argc - 1) {
            write(fd, " ", 1); 
        }
    }

    printf("메시지 전송을 완료했습니다.\n");
    close(fd);
    
    return 0;
}