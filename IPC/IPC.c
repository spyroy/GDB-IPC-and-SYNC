
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>  


/*
when typing cat /proc/"process pid"/status | grep SigCgt
(when "process pid" is the process id of the parent or child)
on terminal while program is running, we should get:
SigCgt:	0000000000000200
which mean that we overwrited the signal handler 
*/

int val = 0;
int fd[2];

static sig_atomic_t sig_num;

void catcher(int sig)
{
	sig_num = sig;
}

void child_proceess()
{
	/* write to pipe val == 0 */
	write(fd[1], &val, sizeof(val));
	
	while(1)
	{  
		sleep(2);
		if (val < 5)
		{
			/* got val from parent,
			printing and increasing val */
			sleep(2);
			read(fd[0], &val, sizeof(val));
			printf("%d\n",val);
			val++;
			write(fd[1], &val, sizeof(val));
		   	kill(getppid(), SIGUSR1);
			/* wait to be signaled */
		   	pause();
		}
		else
		{
			/*parent sent val == 6 hence he is going 
			to be terminated */
			printf("Parent is going to be terminated\n");
			kill(getppid(), SIGUSR1);
			break;
		}
	}
}

void parent_process(pid_t pid)
{
	while(1)
	{
		/* wait to be signaled */
		pause();
		if (val < 5)
		{
			/* got val from child,
			printing and increasing val */
		      	sleep(2);
			read(fd[0], &val, sizeof(val));
			printf("%d\n",val);
			val++;
			write(fd[1], &val, sizeof(val));
			kill(pid, SIGUSR1);
		}
		else
		{
			/* child ended therefor he is terminating */
			printf("Child is going to be terminated\n");
			kill(pid, SIGUSR1);
			break;
		}
	}
}

int main()
{
	/* open pipe */
	pipe(fd);
	/* fork */
	pid_t pid = fork();
	/* overwrite signal_handler */
	signal(SIGUSR1, catcher);
	if(pid < 0)
	{
		printf("failed to fork \n");
		exit(2);
	}
	if (pid == 0)
	{
		child_proceess();
	}
	else
	{
		parent_process(pid);
	}
	
	/* close pipe */	   
	close(fd[1]);
	close(fd[0]);

		return 0;
}
