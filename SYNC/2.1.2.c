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
this version of the code have low (even none) probability for deadlock
because even philosophers picking up the fork to their left
and than the fork to their right, and odd philosophers picking up the
fork to their right and than the fork to their left. 
for example if philosophers 1 and 2 are picking up fork 1 can eat if he picks up
his right fork and 4 will be able to eat if picking up both forks, but 2 and 3 
won't be able to eat and so on.
therefor there is no deadlock (could be a state of starvation).

example of running the program:
Philosopher 1 is thinking
Philosopher 2 is thinking
Philosopher 3 is thinking
Philosopher 4 is thinking
Philosopher 5 is thinking
Philosopher 1 is Hungry
Philosopher 2 is Hungry
Philosopher 3 is Hungry
Philosopher 4 is Hungry
Philosopher 5 is Hungry
Philosopher 5 takes fork 4 and 5
Philosopher 5 is Eating
Philosopher 5 putting fork 4 down
Philosopher 5 putting fork 5 down
Philosopher 5 is thinking
Philosopher 1 takes fork 5 and 1
Philosopher 1 is Eating
Philosopher 4 takes fork 3 and 4
Philosopher 4 is Eating
Philosopher 1 putting fork 5 down
Philosopher 1 putting fork 1 down
Philosopher 1 is thinking
Philosopher 2 takes fork 1 and 2
Philosopher 2 is Eating
Philosopher 5 is Hungry
Philosopher 4 putting fork 4 down
Philosopher 4 putting fork 3 down
Philosopher 4 is thinking
Philosopher 5 takes fork 4 and 5
Philosopher 5 is Eating
Philosopher 2 putting fork 2 down
Philosopher 2 putting fork 1 down
Philosopher 2 is thinking
Philosopher 3 takes fork 2 and 3
Philosopher 3 is Eating
Philosopher 1 is Hungry
Philosopher 4 is Hungry
Philosopher 5 putting fork 4 down
Philosopher 5 putting fork 5 down
Philosopher 5 is thinking
Philosopher 1 takes fork 5 and 1
Philosopher 1 is Eating
Philosopher 2 is Hungry
Philosopher 3 putting fork 2 down
Philosopher 3 putting fork 3 down
Philosopher 3 is thinking
Philosopher 4 takes fork 3 and 4
Philosopher 4 is Eating
Philosopher 1 putting fork 5 down
Philosopher 1 putting fork 1 down
Philosopher 1 is thinking
Philosopher 2 takes fork 1 and 2
Philosopher 2 is Eating
Philosopher 5 is Hungry
Philosopher 4 putting fork 4 down
Philosopher 4 putting fork 3 down
Philosopher 4 is thinking
Philosopher 5 takes fork 4 and 5
Philosopher 5 is Eating
Philosopher 3 is Hungry
Philosopher 2 putting fork 2 down
Philosopher 2 putting fork 1 down
Philosopher 2 is thinking
Philosopher 3 takes fork 2 and 3
Philosopher 3 is Eating
Philosopher 1 is Hungry
Philosopher 4 is Hungry
Philosopher 5 putting fork 4 down
Philosopher 5 putting fork 5 down
Philosopher 5 is thinking
Philosopher 1 takes fork 5 and 1
Philosopher 1 is Eating
Philosopher 2 is Hungry
Philosopher 3 putting fork 2 down
Philosopher 3 putting fork 3 down
Philosopher 3 is thinking
Philosopher 4 takes fork 3 and 4
Philosopher 4 is Eating
Philosopher 1 putting fork 5 down
Philosopher 1 putting fork 1 down
Philosopher 1 is thinking

	.
	.
	.

*/ 

void test(int phnum) 
{ 
	if (state[phnum] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) 
	{ 
		// if have both forks eat...
		state[phnum] = EATING; 
		sleep(2); 
		printf("Philosopher %d takes fork %d and %d\n", phnum + 1, LEFT + 1, phnum + 1);
		printf("Philosopher %d is Eating\n", phnum + 1); 
		sem_post(&S[phnum]); 
	} 	
} 

// take up chopsticks 
void take_fork(int phnum) 
{ 
	 sem_wait(&mutex);

 	if(phnum % 2 == 0)
	{
		printf("Philosopher %d is Hungry\n", phnum + 1);

		//even philosopher picks left and than right
		sem_wait(&forks[LEFT]); 
		sleep(2);
		sem_wait(&forks[phnum]);

		state[phnum] = HUNGRY;
		// eat if neighbours are not eating 
		test(phnum);
		sem_post(&mutex); 

		//free forks first right than left
		sem_post(&forks[phnum]);
		sleep(2); 
		sem_post(&forks[LEFT]); 
	 
		// if unable to eat wait to be signalled 
		sem_wait(&S[phnum]); 
		sleep(1);
	}
	else
	{
		printf("Philosopher %d is Hungry\n", phnum + 1);

		//odd philosopher picks right and than left
		sem_wait(&forks[phnum]); 
		sleep(2);
		sem_wait(&forks[LEFT]);

		state[phnum] = HUNGRY;
		// eat if neighbours are not eating 
		test(phnum);
		sem_post(&mutex); 

		//free forks first right than left
		sem_post(&forks[phnum]);
		sleep(2); 
		sem_post(&forks[LEFT]); 
	 
		// if unable to eat wait to be signalled 
		sem_wait(&S[phnum]);  
		sleep(1);
	} 
} 

// put down chopsticks 
void put_fork(int phnum) 
{  
	sem_wait(&mutex);
	if(phnum % 2 == 0)
	{
		//even philosopher lock left fork than right fork
		sem_wait(&forks[LEFT]); 
		sleep(2); 
		sem_wait(&forks[phnum]);	 

		//put right fork down
		printf("Philosopher %d putting fork %d down\n", phnum + 1, phnum + 1); 
		//put left fork down
		printf("Philosopher %d putting fork %d down\n", phnum + 1, LEFT + 1); 
		// state that thinking 
		state[phnum] = THINKING;
		printf("Philosopher %d is thinking\n", phnum + 1);  

		test(RIGHT);
		sleep(2); 
		test(LEFT); 
		sem_post(&mutex); 

		sem_post(&forks[phnum]); 
		sem_post(&forks[LEFT]); 
	}
	else
	{
		//odd philosopher lock right fork than left fork
		sem_wait(&forks[phnum]); 
		sleep(2); 
		sem_wait(&forks[LEFT]);	 

		//put left fork down
		printf("Philosopher %d putting fork %d down\n", phnum + 1, LEFT + 1); 
		//put right fork down
		printf("Philosopher %d putting fork %d down\n", phnum + 1, phnum + 1); 
		// state that thinking 
		state[phnum] = THINKING;
		printf("Philosopher %d is thinking\n", phnum + 1);  

		test(LEFT);
		sleep(2); 
		test(RIGHT);
		sem_post(&mutex);  

		sem_post(&forks[LEFT]); 
		sem_post(&forks[phnum]);
	}
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
