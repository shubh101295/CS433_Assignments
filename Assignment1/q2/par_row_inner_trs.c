#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<assert.h>
#include<omp.h>

#define SIZE (10)
#define PAD (1)

long double **L, *y, *x, *diag;
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
        x[PAD * i] = ((long double)(random() % 100)) / 500.0 + 0.1;
        y[PAD * i] = 0;
        // printf("%d:%Lf ", PAD * i, x[PAD * i]);
        // printf("y[%d] =  %Lf\n", i, y[i]);
    }
        // printf("\n\n\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <=i; j++)
        {
            y[PAD * i]+=(L[i][j] * x[PAD * j]);
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
    y = (long double*)malloc((PAD * n) * sizeof(long double));
    x = (long double*)malloc((PAD * n) * sizeof(long double));
    diag = (long double*)malloc((n) * sizeof(long double));
    assert(y != NULL);
    assert(x != NULL);
    InitializeInput(L, x, y);
    for (int i = 0; i < n; i++)
    {
        diag[i] = L[i][i];
    }

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
        // printf("processing %d\n", PAD * i);
        y[PAD * i]/=diag[i];
    }


    for (int i = 0; i < n; i++)
    {
        // printf("in row %d\n", i);
#pragma omp for reduction(-:y[i])
        for (int j = 0; j < i; j++)
        {
            // printf("thread id %d in col %d\n", omp_get_thread_num(), j);
            y[PAD * i] -= (x[PAD * j] * L[i][j] / diag[i]);
        }
        // printf("at row %d with threadid %d\n", i, omp_get_thread_num());
    }
}
    end = omp_get_wtime();
    // for (int i = 0; i < n; i++)
    // {
    //     printf("%Lf ", x[PAD * i]);
    // }
    printf("\nSolution took %f seconds--------------------\n", end - start);
    return 0;
}