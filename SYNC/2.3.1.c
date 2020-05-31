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

/*
this version of the code have high probability for locklock
because every philosopher picks up the fork for his left
and want to pick the fork from his right but if he doesnt pick
both forks he putts down the left fork because he cant eat,
hence every philosopher will pick his left fork and won't have the
right fork than will put down his left fork and no one will eat!

example of running the program:
Philosopher 1 is thinking
Philosopher 2 is thinking
Philosopher 3 is thinking
Philosopher 4 is thinking
Philosopher 5 is thinking
Philosopher 1 is Hungry
Philosopher 2 is Hungry
Philosopher 3 is Hungry
Philosopher 2 takes fork 1 
Philosopher 4 is Hungry
Philosopher 3 takes fork 2 
Philosopher 5 is Hungry
Philosopher 4 takes fork 3 
Philosopher 2 putting fork 1 down
Philosopher 2 is thinking
Philosopher 3 putting fork 2 down
Philosopher 3 is thinking
Philosopher 3 is Hungry
Philosopher 2 takes fork 1 
Philosopher 2 is Hungry
Philosopher 3 takes fork 2 
Philosopher 4 putting fork 3 down
Philosopher 4 is thinking
Philosopher 3 putting fork 2 down
Philosopher 3 is thinking
Philosopher 2 putting fork 1 down
Philosopher 2 is thinking
Philosopher 4 is Hungry


*/ 

void test(int phnum) 
{ 
	if (state[LEFT] != EATING) { 
		// state that eating 
		state[LEFT] = EATING; 

		sleep(2); 

		//pick up left fork
		printf("Philosopher %d takes fork %d \n", RIGHT+1, phnum + 1);

		// sem_post(&S[phnum]) has no effect 
		// during takefork 
		// used to wake up hungry philosophers 
		// during putfork 
		sem_post(&S[LEFT+1]); 
	} 

	if (state[phnum-1] != EATING ) { 
		// state that eating 
		state[phnum-1] = EATING; 

		sleep(2); 

		//pick up right fork 
		printf("Philosopher %d takes fork %d \n", phnum, phnum);
		//printf("Philosopher %d is Eating\n", phnum + 1); 

		// sem_post(&S[phnum]) has no effect 
		// during takefork 
		// used to wake up hungry philosophers 
		// during putfork 
		sem_post(&S[RIGHT+1]);  
	}

//	if(state[LEFT] == EATING && state[phnum] == EATING){
//		printf("Philosopher %d is Eating\n", phnum + 1);
//	}
	

		
//	if(state[phnum] == HUNGRY && state[LEFT] != EATING){
//		printf("Philosopher %d takes fork %d \n", phnum + 1, LEFT + 1);
//		//sem_wait(&S[LEFT]);
//		sleep(2);
//		sem_post(&S[LEFT]);
//	}
//	if(state[phnum] == HUNGRY && state[RIGHT] != EATING){
//		printf("Philosopher %d takes fork %d \n", phnum + 1, phnum + 1);
//		printf("Philosopher %d is Eating\n", phnum + 1);
//		//sem_wait(&S[RIGHT]);
//		sleep(2);
//		sem_post(&S[phnum]);
//	}
	
} 

// take up chopsticks 
void take_fork(int phnum) 
{ 

	sem_wait(&mutex); 

	// state that hungry 
	//state[phnum] = HUNGRY; 

	printf("Philosopher %d is Hungry\n", phnum + 1);
 

	// eat if neighbours are not eating 
	test(LEFT);
	//sleep(2);
	test(RIGHT); 

	sem_post(&mutex); 

	// if unable to eat wait to be signalled 
	sem_wait(&S[phnum]); 

	sleep(1); 
} 

// put down chopsticks 
void put_fork(int phnum) 
{ 

	sem_wait(&mutex); 

	// state that thinking 
	//state[phnum] = THINKING; 

	//put left fork down
	printf("Philosopher %d putting fork %d down\n", phnum + 1, LEFT + 1);
	//put right fork down
	//printf("Philosopher %d putting fork %d down\n", phnum + 1, phnum + 1); 
	printf("Philosopher %d is thinking\n", phnum + 1);
	
	sem_post(&S[phnum]); 

	test(RIGHT);
	//sleep(2); 
	test(LEFT); 

	sem_post(&mutex); 
} 

void* philospher(void* num) 
{ 

	while (1) { 

		int* i = num; 

		sleep(1); 

		take_fork(*i);
		//take_fork((*i + 1) % N); 

		sleep(0); 

		//put_fork((*i + 1) % N);
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

	for (i = 0; i < N; i++) { 

		// create philosopher processes 
		pthread_create(&thread_id[i], NULL, 
					philospher, &phil[i]);
		state[i] = THINKING; 

		printf("Philosopher %d is thinking\n", i + 1); 
	} 

	for (i = 0; i < N; i++) 

		pthread_join(thread_id[i], NULL); 
} 
