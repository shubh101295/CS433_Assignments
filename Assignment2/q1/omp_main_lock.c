#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include<pthread.h>
#include<semaphore.h>
#include<omp.h>


#define nthreads 8
#define N 10000000

// global shared variables, loop counter.
int x = 0;
int y = 0;
int i;

double start, finish, total;

int main()
{

    start = omp_get_wtime();
#pragma omp parallel num_threads(nthreads) private(i)
    {
        for ( i = 0; i < N; i++)
        {
#pragma omp critical
            {
                assert(x == y);
                x = y + 1;
                y++;
            }
        }

    }

    finish = omp_get_wtime();
    assert(x == y);
    assert(x == N * nthreads);
    total = finish - start;
    printf("time taken is %lf with x being %d \n", total, x);
}