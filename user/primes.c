#include "kernel/types.h"
#include "user/user.h"

// 从管道left中读数字，计算素数
void calc_primes(int left)
{
    int prime, buf;
    int fork_child = 1; // 是否已经创建了右边的进程
    int pid = -1;       // 右边进程的pid
    int p[2];           // 与右进程通信的管道

    // 从左边进程读取第一个数(一定是素数)
    if (read(left, &prime, sizeof prime) == sizeof prime)
    {
        printf("prime %d\n", prime);
    }

    // 读取剩余的数
    while (read(left, &buf, sizeof buf) == sizeof buf)
    {
        if (buf % prime == 0)
            continue;

        // 创建右边的进程和通信管道(只创建一次)
        if (fork_child)
        {
            pipe(p);
            pid = fork();
            fork_child = 0;
            if (pid > 0)
            {
                close(p[0]);
            }
            else if (pid == 0)
            {
                close(p[1]);
                calc_primes(p[0]);
                exit(0);
            }
            else
            {
                close(p[0]);
                close(p[1]);
                fprintf(2, "primes: fork error\n");
                exit(1);
            }
        }

        // 将不能被prime整除的数传给右进程
        write(p[1], &buf, sizeof buf);
    }

    close(left);
    close(p[1]);
    wait((int *)0);

    return;
}

int main()
{
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid > 0)
    {
        close(p[0]);

        // 将2~35通过管道传给右进程
        for (int i = 2; i <= 35; ++i)
        {
            write(p[1], &i, sizeof i);
        }
    }
    else if (pid == 0)
    {
        close(p[1]);
        calc_primes(p[0]);
        exit(0);
    }
    else
    {
        close(p[0]);
        close(p[1]);
        fprintf(2, "primes: fork error\n");
        exit(1);
    }
    close(p[1]);
    wait((int *)0);

    exit(0);
}