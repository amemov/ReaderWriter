#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <semaphore.h>
// Implemented by Anton Shepelev U28654378
/*	This program is a solution to a Reader-Writer problem via semaphores and threads to the n reader 1 writer problem.
		It accepts the number of readers from the command line. In no case will more than 18 readers be used and always at least 1 reader will be used.
		Each reader must access a shared counter value 250000000 times in the critical section.
		! Note, it does not update anything, just “reads”.
	A reader reads just one time and a writer writes just one time. Each reader needs to print its name when done.
		The writer will update the value 25000 times and print done.The writer will also set a shared flag, in-cs, when it enters
		the critical section and reset it just before it leaves the critical section. The reader must, upon
		entering the critical section, check this flag and write an error message if the flag is set.
*/
/* ! Compile with gcc -ofname ReaderWriter.c -lpthread */
struct shared_dat
{
	int value;     /* shared variable to store result*/
};
struct shared_dat* counter;
int sem_init(sem_t* sem, int pshared, unsigned int value);
int sem_wait(sem_t* sem);
int sem_post(sem_t* sem);
sem_t mutant, xy9nt;
int readers_inside = 0;

/****************************************************************
*                  Threads Body                                 *
****************************************************************/
void relaxandspendtime() {
	int i;
	for (i = 0; i < 250000000; i++) 
		i = i;
}
void* reader_thread(void* arg) {
	
	int id = *((int*)arg);
	sem_wait(&mutant);
	readers_inside++;
	if (readers_inside == 1)
	{
		sem_wait(&xy9nt);
	}
	sem_post(&mutant);
	// Peform read	 
	relaxandspendtime();
	
	sem_wait(&mutant);
	readers_inside--;

	if (readers_inside == 0)
	{
		sem_post(&xy9nt);
	} 

	printf("\nReader %d - DONE", id);
	sem_post(&mutant);
	 
}
void* writer_thread(void* arg) {
	 
	sem_wait(&xy9nt);
	int i;
	for (i = 0; i < 25000; i++){
		counter->value += 1;
	}
	printf("\nWriter: +25k");
	sem_post(&xy9nt);
}
/****************************************************************
*                  Main Body                                    *
****************************************************************/
int main(int argc, char* argv[]) {
	 
	pthread_t writer[1];
	pthread_attr_t	attr[1];
	sem_init(&mutant, 0, 1);
	sem_init(&xy9nt, 0, 1);
	counter = (struct shared_dat*)malloc(sizeof(struct shared_dat));

	/* initialize shared memory to 0 */
	counter->value = 0;

	char* a = argv[1];
	int numOfReaders = atoi(a);
	printf("\nnumOfReaders = %d", numOfReaders);
	// Check if numOfReaders is defined in the range [1;18]. If < 1, set to 1. If > 18, set to 18.
	if (numOfReaders > 18)
		numOfReaders = 18;
	else if (numOfReaders < 1)
		numOfReaders = 1;

	pthread_t readers[numOfReaders];

	int i; int* id = NULL;
	id = malloc(sizeof(int) * numOfReaders);
	for (i = 0; i < numOfReaders; i++)
	{
		id[i] = i;
	}
	 
	/* Required to schedule thread independently.
 Otherwise use NULL in place of attr. */
	pthread_attr_init(&attr[0]);
	pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);  /* system-wide contention */

	int k = (int)(numOfReaders / 2);
	/* Create the reader thread(-s) */
	for (i = 0; i < k; i++) {
		pthread_create(&readers[i], &attr[0], reader_thread, (void*)&id[i]);
	}
	/* Create the writer thread(-s) */
	pthread_create(&writer[0], &attr[0], writer_thread, NULL);
	for (i = k; i < numOfReaders; i++) {
		pthread_create(&readers[i], &attr[0], reader_thread, (void*)&id[i]);
	}
	
 
	printf("\nAbout to start thread loops!\n");
 
	for (i = 0; i < k; i++) {
		pthread_join(readers[i], NULL);
	}	 
	pthread_join(writer[0], NULL);
	for (i = k; i < numOfReaders; i++) {
		pthread_join(readers[i], NULL);
	}

	printf("\nCounter value: %d", counter->value);

	exit(0);
}

