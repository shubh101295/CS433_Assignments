
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include<pthread.h>
#include<semaphore.h>
#include<omp.h>

#include "sync_library.c"
#define N 10000000

//benchmark for all the other locks thatn array and bakery lock , just change the name of the function for acquire and release
void* benchmark(void* param)
{
    for (int i = 0; i < N; i++)
    {
        Acquire_spin_lock(&spin_lock);
        // printf("i %d, x %d, y %d\n", i, x, y);
        assert(x==y);
        x = y + 1;
        y++;
        Release_spin_lock(&spin_lock);
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
    sem_init(&sem, 0, 1);
    for (int i = 0; i < CACHE_LINE_SIZE * nthreads; i++)
    {
        arr[i] = 0;
    }
    arr[0] = true;
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
		// pthread_create(&tid[i], &attr, benchmark_array_lock, NULL);
		pthread_create(&tid[i], &attr, benchmark_bakery_lock, &id[i]);
	}

	for (int i=0; i< nthreads; i++) {
		pthread_join(tid[i], NULL);
	}

    finish = omp_get_wtime();
    assert(x == y);
    assert(x == N * nthreads);

    printf("time taken is %lf with x being %d \n", finish - start, x);
}