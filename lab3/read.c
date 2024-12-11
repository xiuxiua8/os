#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    int fd, i;
    char num[1024];
    fd = open("/dev/ch_device", O_RDWR, S_IRUSR | S_IWUSR);
    if (fd != -1)
    {
        while (1)
        {
            for (i = 0; i < 1024; i++)
                num[i] = '\0';
            read(fd, num, 1024);
            printf("%s\n", num);
            if (strcmp(num, "quit") == 0)
            {
                close(fd);
                break;
            }
        }
    }
    else
    {
        printf("device open failure,%d\n",fd);
    }
}

