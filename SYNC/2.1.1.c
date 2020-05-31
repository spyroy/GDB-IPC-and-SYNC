#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 
#include <unistd.h>

#define N 5 
#define THINKING 2 
#define HUNGRY 1 
#define EATING 0 
#define LEFT (phnum + 4) % N 
#define RIGHT (phnum + 1) % N 

int state[N]; 
int phil[N] = { 0, 1, 2, 3, 4 }; 

sem_t mutex; 
sem_t S[N];
sem_t forks[N];

/*
this version of the code have high probability for deadlock
because every philosopher picks up the fork to his left
and want to pick the fork to his right but if he doesn't pick
both forks he cant eat,
hence every philosopher will pick his left fork and won't put
it untill he got the right fork, and thats what cause the deadlock.

example of running the program:
Philosopher 1 is thinking
Philosopher 2 is thinking
Philosopher 3 is thinking
Philosopher 4 is thinking
Philosopher 5 is thinking
Philosopher 1 is Hungry
Philosopher 1 takes fork 5
Philosopher 2 is Hungry
Philosopher 2 takes fork 1
Philosopher 4 is Hungry
Philosopher 4 takes fork 3
Philosopher 3 is Hungry
Philosopher 3 takes fork 2
Philosopher 5 is Hungry
Philosopher 5 takes fork 4
	.
	.
	.
(program does nothing from here on)

*/ 

void test(int phnum) 
{ 
	if (state[phnum] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) 
	{ 
		// if have both forks eat...
		state[phnum] = EATING; 
		sleep(2); 
		printf("Philosopher %d is eating \n", phnum + 1);
		sem_post(&S[phnum]); 
	} 	
} 

// take up chopsticks 
void take_fork(int phnum) 
{ 

	printf("Philosopher %d is Hungry\n", phnum + 1);

	sem_wait(&forks[LEFT]);
	printf("Philosopher %d takes fork %d\n", phnum + 1, LEFT + 1); 
	sleep(2);
	sem_wait(&forks[phnum]);
	printf("Philosopher %d takes fork %d\n", phnum + 1, phnum + 1);

	state[phnum] = HUNGRY; 

	// eat if neighbours are not eating 
	test(phnum);

	//free forks first right than left
	sem_post(&forks[phnum]);
	sleep(2); 
	sem_post(&forks[LEFT]); 
 
	// if unable to eat wait to be signalled 
	sem_wait(&S[phnum]); 
	sleep(1); 
} 

// put down chopsticks 
void put_fork(int phnum) 
{ 
	//lock left fork than right fork
	sem_wait(&forks[LEFT]); 
	sleep(2); 
	sem_wait(&forks[phnum]);	 

	//put left fork down
	printf("Philosopher %d putting fork %d down\n", phnum + 1, LEFT + 1); 
	//put right fork down
	printf("Philosopher %d putting fork %d down\n", phnum + 1, phnum + 1); 
	// state that thinking 
	state[phnum] = THINKING;
	printf("Philosopher %d is thinking\n", phnum + 1);  

	test(RIGHT);
	sleep(2); 
	test(LEFT); 

	sem_post(&forks[phnum]); 
	sem_post(&forks[LEFT]); 
} 

void* philospher(void* num) 
{ 

	while (1) { 

		int* i = num; 

		sleep(1); 

		take_fork(*i); 

		sleep(0); 

		put_fork(*i);  
	} 
} 

int main() 
{ 

	int i; 
	pthread_t thread_id[N]; 

	// initialize the mutexes 
	sem_init(&mutex, 0, 1); 

	for (i = 0; i < N; i++) 
		sem_init(&S[i], 0, 0); 
	for (i = 0; i < N; i++) 
		sem_init(&forks[i], 0, 1);
	for (i = 0; i < N; i++) { 
		// create philosopher processes 
		pthread_create(&thread_id[i], NULL, philospher, &phil[i]); 
		printf("Philosopher %d is thinking\n", i + 1); 
	} 

	for (i = 0; i < N; i++) 
		pthread_join(thread_id[i], NULL); 
} 
