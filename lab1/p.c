#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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
        global += 5;
        pid1 = getpid();
        printf("child: pid = %d\n", pid);    /* A */
        printf("child: pid1 = %d\n", pid1);  /* B */
        printf("the global variable in child process is %d, address is %p\n", global, (void*)&global);
        printf("system: \n");
        system("./call");
        printf("exec: \n");
        execl("./call", "call", NULL);

        printf("This message is not supposed to be printed if exec succeeds.", global, (void*)&global);
    }
    else { /* parent process */
        global -= 5;
        pid1 = getpid();
        printf("parent: pid = %d\n", pid);   /* C */
        printf("parent: pid1 = %d\n", pid1); /* D */
        printf("the global variable in parent process is %d, address is %p\n", global, (void*)&global);
        wait(NULL);
    }

    return 0;
}
