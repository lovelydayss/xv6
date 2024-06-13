#include "kernel/types.h"
#include "user/user.h"

int main() {

    int p1[2], p2[2];
    char buffer[] = {'x'};
    int length = sizeof(buffer);

    pipe(p1);
    pipe(p2);

    // 拉起子进程
    if(fork() == 0) {

        close(p1[1]);
        close(p2[0]);

        // 子进程读
        if(read(p1[0], buffer, length) != length) {
            printf("a--->b read error!");
			exit(1);
        }

        // 子进程写
		printf("%d: received ping\n", getpid());
        if(write(p2[1], buffer, length) != length) {
            printf("a<---b write error!");
			exit(1);
        }

        exit(0);

    }

    close(p1[0]);
    close(p2[1]);

    // 父进程写
    if(write(p1[1], buffer, length) != length){
		printf("a--->b write error!");
		exit(1);
	}

    // 父进程读
    if(read(p2[0], buffer, length) != length){
		printf("a<---b read error!");
		exit(1);
	}

    printf("%d: received pong\n", getpid());

    // 等待子进程退出
    wait(0);
	exit(0);

}