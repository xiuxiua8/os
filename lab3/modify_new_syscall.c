#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>

int main()
{
    int ret = syscall(78, 10, 20); 
    printf("%d\n", ret);
    return 0;
}
