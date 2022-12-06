#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

char whitespace[] = " \t\r\v";

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "usage: xargs [command [initial-arguments]]\n");
        exit(1);
    }
    if (argc >= MAXARG)
    {
        fprintf(2, "xargs: too many arguments\n");
        exit(1);
    }

    int myargc = argc - 1;      // 子进程参数个数
    char *myargv[MAXARG] = {0}; // 子进程参数指针数组
    for (int i = 1; i < argc; ++i)
        myargv[i - 1] = argv[i];

    char buf[1024];
    char *p = buf;
    int last_aplha = 0; // 记录上一个读取的字符是否是字母

    while (read(0, p, sizeof(char)) == sizeof(char))
    {
        // 空格
        if (strchr(whitespace, *p))
        {
            last_aplha = 0;
            *p = '\0';
            continue;
        }

        // 行尾 开始执行命令
        if (*p == '\n')
        {
            *p = '\0';
            int pid = fork();

            if (pid == 0)
            {
                exec(myargv[0], myargv);
                fprintf(2, "xargs: exec error\n");
                exit(1);
            }
            else if (pid < 0)
            {
                fprintf(2, "xargs: fork error\n");
                exit(1);
            }

            wait((int *)0);

            // 刷新缓冲区
            p = buf;
            myargc = argc - 1;
        }

        // 单词首字母
        if (last_aplha == 0)
        {
            myargv[myargc++] = p;
        }

        last_aplha = *p++;
    }

    exit(0);
}