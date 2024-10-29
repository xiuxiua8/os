#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h> 
#include <unistd.h>

int global = 10;

int main()
{
    pid_t pid, pid1;

    /* fork a child process */
    pid = fork();

    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0) { /* child process */
        sleep(5);
        printf("slept for 5 secends\n");
        global += 5;
        pid1 = getpid();
        printf("child: pid = %d\n", pid);    /* A */
        printf("child: pid1 = %d\n", pid1);  /* B */
        printf("the global variable in child process is %d, address is %p\n", global, (void*)&global);
    }
    else { /* parent process */
        global -= 5;
        pid1 = getpid();
        printf("parent: pid = %d\n", pid);   /* C */
        printf("parent: pid1 = %d\n", pid1); /* D */
        wait(NULL);
        printf("the global variable in parent process is %d, address is %p\n", global, (void*)&global);
        
    }

    return 0;
}
