#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
//string.h 헤더 추가
#include <string.h>

//int main(int argc, char *argv[])로 변경
int main(int argc, char *argv[])
{
    //: 자리에 NULL을 덮어씌워 두 개의 명령어 배열(cmd1, cmd2)로 쪼개는 로직을 추가
    if (argc < 4) {
        fprintf(stderr, "사용법: %s <명령1> [인자] : <명령2> [인자]\n", argv[0]);
        return 1;
    }

    int split_idx = -1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], ":") == 0) {
            split_idx = i;
            break;
        }
    }

    if (split_idx == -1 || split_idx == 1 || split_idx == argc - 1) {
        fprintf(stderr, "오류: 구분자 ':'가 없거나 양옆에 명령이 부족합니다.\n");
        return 1;
    }

    argv[split_idx] = NULL; 
    
    char **cmd1 = &argv[1];               
    char **cmd2 = &argv[split_idx + 1];   

    int fd[2];   
    if (pipe(fd) == -1) { perror("pipe"); return 1; }

    if (fork() == 0) { 
        close(fd[0]);                 
        dup2(fd[1], STDOUT_FILENO);   
        close(fd[1]);                 
        
        //execlp("ls", "ls", NULL); 를 execvp(cmd1[0], cmd1); 로 변경
        execvp(cmd1[0], cmd1);        
        perror("execvp cmd1"); 
        _exit(127); 
    }

    if (fork() == 0) {
        close(fd[1]);                 
        dup2(fd[0], STDIN_FILENO);    
        close(fd[0]);                 
        
        //execlp("wc", "wc", "-l", NULL); 를 execvp(cmd2[0], cmd2); 로 변경
        //: 뒤에 입력된 두 번째 명령어 배열(cmd2)을 동적으로 실행하기 위함
        execvp(cmd2[0], cmd2);
        perror("execvp cmd2");
        _exit(127);
    }

    close(fd[0]);
    close(fd[1]);

    wait(NULL);   
    wait(NULL);   
    
    return 0;
}