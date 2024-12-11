#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>

int main()
{
    struct timeval tv;
    syscall(78, &tv, NULL); 
    printf("tv_sec:%ld\n", (long)tv.tv_sec);
    printf("tv_usec:%ld\n", (long)tv.tv_usec);

    return 0;
} 
