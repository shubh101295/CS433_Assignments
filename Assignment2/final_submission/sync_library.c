
/* Question1 begins here */
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include<pthread.h>
#include<semaphore.h>
#include <sys/time.h>
#include<omp.h>


bool CompareAndSet(int oldVal, int newVal, int* ptr);
#define nthreads 8/* useful for q1 only */
#define CACHE_LINE_SIZE 64
#define int_lsize 16

//shared variables
int x = 0;
int y = 0;


//time measurement
double start = 0;
double finish = 0;

/*-------------------------MUTEX LOCK PTHREAD----------------------------------------------*/
// mutex checker initalizers
pthread_mutex_t mtx_lck = PTHREAD_MUTEX_INITIALIZER;

// Acquire for pthread mutex
void Acquire_pthread_mutex(pthread_mutex_t* lck)
{
	pthread_mutex_lock(lck);
    return;
}

// Release for pthread mutex
void Release_pthread_mutex(pthread_mutex_t* lck)
{
	pthread_mutex_unlock(lck);
    return;
}

/*-----------------------------BINARY SEMAPHORE PTHREAD-------------------------------------*/

sem_t sem;

// Acquire function of binary semaphore.
void Acquire_binary_semaphore(sem_t* sem)
{
    sem_wait(sem);
    return;
}

// Release function of binary semaphore.
void Release_binary_semaphore(sem_t* sem)
{
    sem_post(sem);
    return;
}
/*----------------SPIN LOCK CMPXCHG-------------------------------*/
int spin_lock = 0;

// Acquire for spin lock
void Acquire_spin_lock(int* lock_addr)
{
    while(!CompareAndSet(0, 1, lock_addr));
}

// Release for spin lock
void Release_spin_lock(int* lock_addr)
{
    asm("":::"memory");
    (*lock_addr) = 0;
}
/*--------------------TICKET LOCK-----------------------------*/
int ticket = 0;
int release_count = 0;

// Acquire for ticket lock
void Acquire_ticket_lock(int* ticket_addr, int* release_addr)
{
    int oldVal;
    while(true)
    {
        asm("":::"memory");
        oldVal = *ticket_addr;
        if(CompareAndSet(oldVal, oldVal + 1, ticket_addr)) break;
    }
    asm("mfence":::"memory");
    while(true)
    {
        asm("":::"memory");
        int rel_val = *release_addr;
        if(rel_val == oldVal) break;
    }
    return;
}

// Release for ticket lock
void Release_ticket_lock(int* ticket_addr, int* release_addr)
{
    asm("":::"memory");
    (*release_addr) = (*release_addr) + 1;
    // cout<<"*release addr is now "<<*release_addr<<"\n";
    asm("mfence":::"memory");
    return;
}
/*---------------------TEST AND TEST AND SET--------------------------*/
int tts_lock = 0;

// Acquire for test & test & set
void Acquire_test_test_set(int * lock_addr)
{
    while (true)
    {
        if(CompareAndSet(0, 1, lock_addr))
        {
            break;
        }
        while((*lock_addr))
        {
            asm("":::"memory");
        }
    }
}

// Release for test & test & set
void Release_test_test_set(int* lock_addr)
{
    asm("":::"memory");
    (*lock_addr) = 0;
    // asm("mfence":::"memory");
}
/*------------------------ARRAY LOCK-------------------------*/
bool arr[CACHE_LINE_SIZE * nthreads];
int free_id = 0;

// Acquire for array lock
int Acquire_array_lock(int* free_id_addr)
{
    int i;
    while(true)
    {
        asm("":::"memory");
        i = *free_id_addr;
        if(CompareAndSet(i, i + 1, free_id_addr)) break;
    }
    asm("mfence":::"memory");
    while(!arr[CACHE_LINE_SIZE * (i % nthreads)])
    {
        asm("":::"memory");
    }
    return i;
}


// Release for array lock
void Release_array_lock(int tid)
{
    arr[CACHE_LINE_SIZE * (tid % nthreads)] = 0;
    arr[CACHE_LINE_SIZE * ((tid + 1) % nthreads)] = 1;
    return;
}

/*--------------LAMPORT'S BAKERY LOCK---------------------------------*/
bool choosing[nthreads * CACHE_LINE_SIZE];
int tickets[nthreads * int_lsize];
int id[nthreads];

// Acquire for bakery lock
void Acquire_bakery_lock(int tid)
{
    choosing[tid * CACHE_LINE_SIZE] = true;
    asm("mfence":::"memory");
    int temp = 0;
    for (int i = 0; i < nthreads; i++)
    {
        if (temp < tickets[i * int_lsize])
        {
            temp = tickets[i * int_lsize];
        }
    }
    tickets[tid * int_lsize] = temp + 1;
    asm("mfence":::"memory");
    choosing[tid * CACHE_LINE_SIZE] = false;

    for (int j = 0; j < nthreads; j++)
    {
        while(choosing[j * CACHE_LINE_SIZE])
        {
            asm("":::"memory");
        }
        while(tickets[j * int_lsize] && ((tickets[j * int_lsize] < tickets[tid * int_lsize]) || ((tickets[j * int_lsize] == tickets[tid * int_lsize]) && (j < tid)) ))
        {
            asm("":::"memory");
        }
    }
    return;
}

// Release for bakery lock
void Release_bakery_lock(int tid)
{
    asm("":::"memory");
    tickets[tid * int_lsize] = 0;
}

/*----------------------------------------------------------*/
bool CompareAndSet(int oldVal, int newVal, int* ptr)
{
    int oldValOut;
    bool result;
    asm(
        "lock cmpxchgl %4, %1 \n setzb %0"
        :"=qm"(result), "+m"(*ptr), "=a"(oldValOut)
        :"a"(oldVal), "r"(newVal)
        :);
    return result;
}


/* Question2 begins here */


/* struct for Centralized sense-reversing barrier using busy-wait on flag */
struct bar_type1 {
	int counter;
	pthread_mutex_t mutex;
	int flag;
};


/* Init for Centralized sense-reversing barrier using busy-wait on flag */
void barrier_sense_reversal_init(struct bar_type1* bar_name1)
{
	bar_name1->counter = 0;
	bar_name1->flag = 0;
	pthread_mutex_init(&bar_name1->mutex,NULL);
}


/* Centralized sense-reversing barrier using busy-wait on flag */
void barrier_sense_reversal(struct bar_type1* bar_name, int P,int* local_sense)
{
	(*local_sense) = !(*local_sense);

	pthread_mutex_lock(&bar_name->mutex);
	bar_name->counter+=1;
	if (bar_name->counter == P)
	{
		bar_name->counter = 0;
		pthread_mutex_unlock(&bar_name->mutex);
		bar_name->flag = *local_sense;
	}
	else{
		pthread_mutex_unlock(&bar_name->mutex);
		while(bar_name->flag != (*local_sense)){
			asm(""::
				:"memory");
		}
	}
}

/* struct for Tree barrier using busy-wait on flags */
struct bar_type2 {
	int** flag;
	int MAX;
};

/* Init for Tree barrier using busy-wait on flags */
void tree_barrier_init(struct bar_type2* bar_name1,int num_threads)
{
	bar_name1->flag = (int **)malloc(sizeof(int*)*num_threads);
	for (int i = 0; i<num_threads; i++) {
        bar_name1->flag[i] = (int *)malloc(sizeof(int)*num_threads);

        for (int j = 0; j<num_threads; j++) {
            bar_name1->flag[i][j] = 0;
        }
    }
    bar_name1->MAX = 0;
	int num_threads2 = num_threads;
	while(num_threads2)
	{
		bar_name1->MAX+=1;
		num_threads2/=2;
	}
}


/* Tree barrier using busy-wait on flags */
void tree_barrier(struct bar_type2* bar_name, int pid,int P){
	unsigned int i,mask;
	for(i=0,mask=1;(mask&pid)!=0;++i,mask<<=1)
	{
		while(!bar_name->flag[pid][i]) {
			asm(""::
				:"memory");
		};
		bar_name->flag[pid][i]= 0;
	}
	if( pid < (P-1)) {
		bar_name->flag[pid+mask][i] = 1;
		while(!bar_name->flag[pid][bar_name->MAX-1]){
			asm(""::
				:"memory");
		}
		bar_name->flag[pid][bar_name->MAX-1]=0;
	}
	for(mask>>=1;mask>0;mask>>=1)
	{
		bar_name->flag[pid-mask][bar_name->MAX-1] = 1;
	}
}

/* struct for Centralized barrier using POSIX condition variable */
struct bar_type3 {
	int counter;
	pthread_mutex_t mutex;
	pthread_cond_t cv;
};

/* init for  Centralized barrier using POSIX condition variable */
void centralised_barrier_using_posix_condition_variable_init(struct bar_type3* bar_name1){
	bar_name1->counter=0;
	pthread_mutex_init(&bar_name1->mutex,NULL);
	pthread_cond_init(&bar_name1->cv,NULL);
}

/* Barrier for Centralized barrier using POSIX condition variable */
void centralised_barrier_using_posix_condition_variable(struct bar_type3* bar_name, int P) {

	pthread_mutex_lock(&bar_name->mutex);
	bar_name->counter+=1;
	if (bar_name->counter == P)
	{
		bar_name->counter = 0;
		pthread_cond_broadcast(&bar_name->cv);
	}
	else{
	    pthread_cond_wait(&bar_name->cv,&bar_name->mutex);
	}
	pthread_mutex_unlock(&bar_name->mutex);

}

/* struct for Tree barrier using POSIX condition variable */
struct bar_type4 {
	int** flag;
	pthread_cond_t ** cvs;
	pthread_mutex_t ** cv_locks;
	int MAX;
};

/* Init for Tree barrier using POSIX condition variable */
void tree_barrier_posix_condition_init(struct bar_type4* bar_name1,int num_threads)
{
	bar_name1->flag = (int **)malloc(sizeof(int*)*num_threads);
	bar_name1->cvs = (pthread_cond_t **)malloc(sizeof(pthread_cond_t*)*num_threads);
	bar_name1->cv_locks = (pthread_mutex_t **)malloc(sizeof(pthread_mutex_t*)*num_threads);

	for (int i = 0; i<num_threads; i++) {
        bar_name1->flag[i] = (int *)malloc(sizeof(int)*num_threads);
        bar_name1->cvs[i] = (pthread_cond_t *)malloc(sizeof(pthread_cond_t)*num_threads);
        bar_name1->cv_locks[i] = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)*num_threads);

        for (int j = 0; j<num_threads; j++) {
            bar_name1->flag[i][j] = 0;
            pthread_mutex_init(&bar_name1->cv_locks[i][j],NULL);
			pthread_cond_init(&bar_name1->cvs[i][j],NULL);
        }
    }
    bar_name1->MAX = 0;
	int num_threads2 = num_threads;
	while(num_threads2)
	{
		bar_name1->MAX+=1;
		num_threads2/=2;
	}
}

/* Tree barrier using POSIX condition variable */
void tree_barrier_using_posix_condition_variable(struct bar_type4* bar_name,int pid,int P){
	unsigned int i,mask;
	for(i=0,mask=1;(mask&pid)!=0;++i,mask<<=1)
	{
		pthread_mutex_lock(&bar_name->cv_locks[pid][i]);
		while(!bar_name->flag[pid][i]){
			pthread_cond_wait(&bar_name->cvs[pid][i], &bar_name->cv_locks[pid][i]);
			 asm("":::"memory");
		}
		pthread_mutex_unlock(&bar_name->cv_locks[pid][i]);

		bar_name->flag[pid][i]= 0;
	}

	if( pid < (P-1)) {
		bar_name->flag[pid+mask][i] = 1;

		pthread_mutex_lock(&bar_name->cv_locks[pid+mask][i]);
		pthread_cond_broadcast(&bar_name->cvs[pid+mask][i]);
		pthread_mutex_unlock(&bar_name->cv_locks[pid+mask][i]);


		pthread_mutex_lock(&bar_name->cv_locks[pid][bar_name->MAX-1]);
		while(!bar_name->flag[pid][bar_name->MAX-1]) {
			 pthread_cond_wait(&bar_name->cvs[pid][bar_name->MAX-1], &bar_name->cv_locks[pid][bar_name->MAX-1]);
			 asm("":::"memory");

		}
		pthread_mutex_unlock(&bar_name->cv_locks[pid][bar_name->MAX-1]);
		bar_name->flag[pid][bar_name->MAX-1]=0;
	}
	for(mask>>=1;mask>0;mask>>=1)
	{
		bar_name->flag[pid-mask][bar_name->MAX-1] = 1;
		pthread_mutex_lock(&bar_name->cv_locks[pid-mask][bar_name->MAX-1]);
		pthread_cond_broadcast(&bar_name->cvs[pid-mask][bar_name->MAX-1]);
		pthread_mutex_unlock(&bar_name->cv_locks[pid-mask][bar_name->MAX-1]);

	}

}

/* struct for POSIX barrier interface (pthread_barrier_wait) */
struct bar_type5 {
	pthread_barrier_t barrier_from_p_thread;
};

/* init for POSIX barrier interface (pthread_barrier_wait) */

void posix_barrier_interface_init(struct bar_type5* bar_name1,int num_threads)
{
	pthread_barrier_init(&bar_name1->barrier_from_p_thread,NULL,num_threads);
}

/* POSIX barrier interface (pthread_barrier_wait) */
void posix_barrier_interface(struct bar_type5* bar_name)
{
	pthread_barrier_wait(&bar_name->barrier_from_p_thread);
}
