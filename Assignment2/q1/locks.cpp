#include<bits/stdc++.h>
#include<pthread.h>
#include<semaphore.h>
#include<omp.h>

using namespace std;

bool CompareAndSet(int oldVal, int newVal, int* ptr);
#define nthreads 1
#define N 10000000

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

void Acquire_ticket_lock(int* ticket_addr, int* release_addr)
{
    int oldVal = *ticket_addr;
    CompareAndSet(oldVal, oldVal + 1, ticket_addr);
    (*release_addr) = (*release_addr) + 1;
    cout<<"*ticket addr is now "<<*ticket_addr<<"\n";
    while(oldVal != *release_addr);
    return;
}

void Release_ticket_lock(int* ticket_addr, int* release_addr)
{
    asm("":::"memory");
    (*release_addr) = (*release_addr) + 1;
    cout<<"*release addr is now "<<*release_addr<<"\n";
    asm("mfence":::"memory");
    return;
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
        Acquire_ticket_lock(&ticket, &release_count);
        // cout<<"i "<<i<<" x "<<x<<" y "<<y<<"\n";
        assert(x==y);
        x = y + 1;
        y++;
        Release_ticket_lock(&ticket, &release_count);
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
    start = omp_get_wtime();

	for(int i = 0; i < nthreads; i++)
	{
		pthread_create(&tid[i], &attr, benchmark, NULL);
	}

	for (int i=0; i< nthreads; i++) {
		pthread_join(tid[i], NULL);
	}

    finish = omp_get_wtime();
    assert(x == y);
    assert(x == N * nthreads);

    cout<<"time taken is "<<finish - start<<" with x being "<<x<<" \n";
}