#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include<pthread.h>
#include<semaphore.h>
#include<omp.h>


bool CompareAndSet(int oldVal, int newVal, int* ptr);
#define nthreads 8
#define N 10000000
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
    for (int i = 0; i < nthreads; i++)
    {
        if (tickets[tid * int_lsize] < tickets[i * int_lsize])
        {
            tickets[tid * int_lsize] = tickets[i * int_lsize] + 1;
        }
    }
    asm("mfence":::"memory");
    choosing[tid * CACHE_LINE_SIZE] = false;
    asm("mfence":::"memory");

    for (int j = 0; j < nthreads; j++)
    {
        while(choosing[j * CACHE_LINE_SIZE])
        {
            asm("":::"memory");
        }
        int ticket_j = tickets[j * int_lsize];
        int ticket_i = tickets[tid * int_lsize];
        while(ticket_j && ((ticket_j < ticket_i) || ((ticket_j == ticket_i) && (j < tid)) ))
        {
            asm("":::"memory");
            ticket_j = tickets[j * int_lsize];
            ticket_i = tickets[tid * int_lsize];
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

//benchmark
void* benchmark(void* param)
{
    for (int i = 0; i < N; i++)
    {
        Acquire_test_test_set(&tts_lock);
        // printf("i %d, x %d, y %d\n", i, x, y);
        assert(x==y);
        x = y + 1;
        y++;
        Release_test_test_set(&tts_lock);
    }
    return NULL;
}

// benchmark array lock
void* benchmark_array_lock(void* param)
{
    for (int i = 0; i < N; i++)
    {
        int tid = Acquire_array_lock(&free_id);
        // printf("i %d, x %d, y %d\n", i, x, y);
        assert(x==y);
        x = y + 1;
        y++;
        Release_array_lock(tid);
    }
    return NULL;
}

// benchmark bakery lock
void* benchmark_bakery_lock(void* param)
{
    int tid = *(int *)(param);
    for (int i = 0; i < N; i++)
    {
        Acquire_bakery_lock(tid);
        // printf("i %d, x %d, y %d\n", i, x, y);
        assert(x==y);
        x = y + 1;
        y++;
        Release_bakery_lock(tid);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_attr_t attr;
    pthread_t *tid;

	tid = (pthread_t*)malloc(nthreads * sizeof(pthread_t));

	pthread_attr_init(&attr);
    // sem_init(&sem, 0, 1);
    // for (int i = 0; i < CACHE_LINE_SIZE * nthreads; i++)
    // {
    //     arr[i] = 0;
    // }
    // arr[0] = true;
    for (int i = 0; i < CACHE_LINE_SIZE * nthreads; i++)
    {
        choosing[i] = 0;
    }
    for (int i = 0; i < CACHE_LINE_SIZE * nthreads / sizeof(int); i++)
    {
        tickets[i] = 0;
    }
    for (int i = 0; i < nthreads; i++)
    {
        id[i] = i;
    }

    start = omp_get_wtime();

	for(int i = 0; i < nthreads; i++)
	{
		// pthread_create(&tid[i], &attr, benchmark, NULL);
		pthread_create(&tid[i], &attr, benchmark_array_lock, NULL);
		// pthread_create(&tid[i], &attr, benchmark, &id[i]);
	}

	for (int i=0; i< nthreads; i++) {
		pthread_join(tid[i], NULL);
	}

    finish = omp_get_wtime();
    assert(x == y);
    assert(x == N * nthreads);

    printf("time taken is %lf with x being %d \n", finish - start, x);
}