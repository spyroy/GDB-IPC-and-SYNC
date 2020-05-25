#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
when typing cat /proc/"process pid"/status | grep SigCgt
(when "process pid" is the process id of the parent or child)
on terminal while program is running, we should get:
SigCgt:	0000000000000200
which mean that we overloaded the signal handler 
*/

int fd[2];
int val = 0;

pipe(fd);

static sig_atomic_t sig_num;
static int counter;

static void catcher(int sig)
{
    sig_num = sig;
}

static void child_process(void)
{
	
    sleep(1);
    //send val == 0 to Parent via pipe	
    write(fd[0], &val, sizeof(val));
    //send signal to Parent
    kill(getppid(), SIGUSR1); 
    while (1)
    {
	//recive val from Parent
	read(fd[0], &val, sizeof(val));
	//checks if val is not greater than 5
	if(val < 5){
		sleep(1);
		//increases val and sends the new val to Parent 
		val++;
		write(fd[0], &val, sizeof(val));
		kill(getppid(), SIGUSR1); 
	}
	else{
	   //parent sent val == 5 so parent is going to be terminated
	   printf("Parent is going to be terminated\n");
	   break;
	}
    }
    //close pipe
    close(fd[0]);
}

static void parent_process(pid_t pid)
{
	
    while(1){
	//recive val from child
    	read(fd[1], &val, sizeof(val));
	//prints val
    	printf(" 	%d\n",val);
	//checks if val is not greater than 5
    	if(val < 5){
		sleep(1);
		//increases val and sends the new val to Child 
		val++;
		write(fd[1], &val, sizeof(val));
    		kill(pid, SIGUSR1);
    	}
    	else{
		//child sent val == 5 so he is going to be terminated
    		printf("Child is going to be terminated\n");
		write(fd[1], &val, sizeof(val));
		kill(pid, SIGUSR1);
		break;
    	}
    }
    //close pipe
    close(fd[1]);
}

int main(void)
{
    //overload signal handler
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
