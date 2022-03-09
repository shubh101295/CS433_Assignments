#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<assert.h>
#include<omp.h>

#define SIZE (10)

long double **L, *y, *x;
int n, nthreads;

void InitializeInput(long double** L, long double* x, long double* y)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            L[i][j] = ((long double)(random() % 100)) / 500.0 + 0.1;
            if(i == j) L[i][i] = 1;
            // printf("%Lf ", L[i][j]);
        }
        // printf("\n");
    }
    for (int i = 0; i < n; i++)
    {
        x[i] = ((long double)(random() % 100)) / 500.0 + 0.1;
        y[i] = 0;
        // printf("%Lf ", x[i]);
        // printf("y[%d] =  %Lf\n", i, y[i]);
    }
        // printf("\n\n\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <=i; j++)
        {
            y[i]+=(L[i][j] * x[j]);
        }
    }
}

int main(int argc, char *argv[])
{
    n = strtol(argv[1], NULL, 10);
    nthreads = strtol(argv[2], NULL, 10);
    // Input taking!!
    L = (long double**)malloc( (n)*sizeof(long double*) );
    assert(L != NULL);
    for (int i = 0; i < n; i++)
    {
        L[i] = (long double*)malloc((i + 1)*sizeof(long double));
        assert(L[i] != NULL);

       // for (int j = 0; j <= i; j++)
        // {
        //     scanf("%lf", &L[i][j]);
        // }

    }
    y = (long double*)malloc((n) * sizeof(long double));
    x = (long double*)malloc((n) * sizeof(long double));
    assert(y != NULL);
    assert(x != NULL);
    InitializeInput(L, x, y);
    // for (int i = 0; i < n; i++)
    // {
    //     scanf("%lf", &y[i]);
    //     x[i] = ((long double)(random() % 100)) / 100;
    // }

    // Solving
    float start = 0.0, end = 0.0;
    start = omp_get_wtime();

#pragma omp parallel num_threads(nthreads)
{
#pragma omp for
    for (int i = 0; i < n; i++)
    {
        y[i]/=L[i][i];
    }


    for (int i = 0; i < n; i++)
    {
#pragma omp for reduction(-:y[i])
        for (int j = 0; j < i; j++)
        {
            // printf("thread id %d in col %d\n", omp_get_thread_num(), j);
            y[i] -= (x[j] * L[i][j] / L[i][i]);
        }
        // printf("at row %d with threadid %d\n", i, omp_get_thread_num());
    }
}
    end = omp_get_wtime();
    // for (int i = 0; i < n; i++)
    // {
    //     printf("%Lf ", x[i]);
    // }
    printf("\nSolution took %f seconds--------------------\n", end - start);
    return 0;
}