#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char *path, char *filename)
{
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if ((fstat(fd, &st) < 0) || st.type != T_DIR)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    if (st.type != T_DIR)
    {
        fprintf(2, "find: %s: not a directory\n", path);
        close(fd);
        return;
    }

    char buf[512];
    int len = strlen(path);
    if (len + 1 + DIRSIZ + 1 > sizeof buf)
    {
        printf("find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    char *p = buf + len;
    *p++ = '/'; // 取值运算符*与++优先级相同，从右向左匹配

    // 遍历目录项，搜索filename
    while (read(fd, &de, sizeof de) == sizeof de)
    {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (stat(buf, &st) < 0)
        {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        // 跳过"."和"..", 避免无限递归
        if (st.type == T_DIR && strcmp(de.name, ".") && strcmp(de.name, ".."))
        {
            find(buf, filename);
        }
        else if (strcmp(de.name, filename) == 0)
        {
            printf("%s\n", buf);
        }
    }

    close(fd);
    return;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}