#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

enum { COUNT = 5 };

static sig_atomic_t sig_num;
static int counter;

static void catcher(int sig)
{
    sig_num = sig;
}

static void child_process(void)
{
    struct timespec sleep = { .tv_sec = 0, .tv_nsec = 100000000 };

    while (1)
    {   
        pause();
        nanosleep(&sleep, 0);
        kill(getppid(), SIGUSR1);
	counter++;
	if(counter == 5)
	   printf("Child is going to be terminated\n");
    }
}

static void parent_process(pid_t pid)
{
    struct timespec sleep = { .tv_sec = 0, .tv_nsec = 100000000 };

    for (int i = 0; i < COUNT+1; i++)
    {
        printf("%d\n", i);
	counter++;
        nanosleep(&sleep, 0);
        kill(pid, SIGUSR1);
        pause();
    }
    printf("Parent is going to be terminated\n");
    kill(pid, SIGTERM);
}

int main(void)
{
    struct sigaction SA;
    sigemptyset(&SA.sa_mask);
    SA.sa_flags = 0;
    SA.sa_handler = catcher;

    if (sigaction(SIGUSR1, &SA, NULL) == -1){
        printf("Failed to fork");
	exit(2);
    }

    pid_t pid = fork();

    if (pid < 0){
        printf("Failed to fork");
	exit(2);
    }
    else if (pid == 0)
        child_process();
    else
        parent_process(pid);

    return 0;
}
