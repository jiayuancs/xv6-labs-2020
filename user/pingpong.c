#include "kernel/types.h"
#include "user/user.h"

// 关闭父进程/子进程使用的管道文件描述符
#define CLOSE_PARENT \
    close(p2c[1]);   \
    close(c2p[0]);
#define CLOSE_CHILD \
    close(p2c[0]);  \
    close(c2p[1]);

int p2c[2];
int c2p[2];
int buf = 'X'; // 待传输的1字节数据

int main(void)
{
    pipe(p2c); // 父进程->子进程
    pipe(c2p); // 子进程->父进程

    int pid = fork();

    if (pid > 0)
    {
        CLOSE_CHILD;
        if (write(p2c[1], &buf, sizeof buf) != sizeof buf)
        {
            fprintf(2, "parent: write error\n");
            CLOSE_PARENT;
            exit(1);
        }

        if (read(c2p[0], &buf, sizeof buf) != sizeof buf)
        {
            fprintf(2, "parent: read error\n");
            CLOSE_PARENT;
            exit(1);
        }

        printf("%d: received pong\n", getpid());
        CLOSE_PARENT;
    }
    else if (pid == 0)
    {
        CLOSE_PARENT;
        if (read(p2c[0], &buf, sizeof buf) != sizeof buf)
        {
            fprintf(2, "child: read error\n");
            CLOSE_CHILD;
            exit(1);
        }

        printf("%d: received ping\n", getpid());

        if (write(c2p[1], &buf, sizeof buf) != sizeof buf)
        {
            fprintf(2, "child: write error\n");
            CLOSE_CHILD;
            exit(1);
        }

        CLOSE_CHILD;
    }
    else
    {
        fprintf(2, "fork error\n");
        CLOSE_CHILD;
        CLOSE_PARENT;
        exit(1);
    }

    exit(0);
}

/* 仅使用1个管道实现ping-pong
int main(void)
{
    int p[2];
    pipe(p);
    int pid = fork();

    if(pid > 0)
    {
        // send
        if(write(p[1], "x", 1) != 1)
        {
            printf("write error\n");
            exit(1);
        }

        // 父进程需要等待子进程退出后再接受数据
        wait((int*)0);

        // receive
        char buf;
        if(read(p[0], &buf, 1)!=1)
        {
            printf("read error\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

    }
    else if(pid == 0)
    {
        // receive
        char buf;
        if(read(p[0], &buf, 1)!=1)
        {
            printf("read error\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        // send
        if(write(p[1], "x", 1) != 1)
        {
            printf("write error\n");
            exit(1);
        }
    }
    else
    {
        printf("fork error\n");
        exit(1);
    }

    exit(0);
}
*/