#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<assert.h>
#include<omp.h>

long double **L, *y, *x;
int n;
int nthreads;

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
    L = (long double**)malloc(  (n)*sizeof(long double*) );
    assert(L != NULL);
    for (int i = 0; i < n; i++)
    {
        L[i] = (long double*)malloc((i + 1)*sizeof(long double));
        assert(L[i] != NULL);

        // for (int j = 0; j <= i; j++)
        // {
        //     scanf("%Lf", &L[i][j]);
        // }

    }
    // note size of long double 8 bytes. cache line in my computer is 64 bytes. to avoid false sharing, pad elements.
    y = (long double*)malloc((n) * sizeof(long double));
    x = (long double*)malloc((n) * sizeof(long double));
    assert(y != NULL);
    assert(x != NULL);
    InitializeInput(L, x, y);
    // for (int i = 0; i < n; i++)
    // {
    //     scanf("%Lf", &y[8 * i]);
    //     x[8 * i] = ((long double)(random() % 100)) / 100;
    // }

    // Solving
    double start = 0.0, end = 0.0;
    start = omp_get_wtime();

#pragma omp parallel num_threads(nthreads)
{

#pragma omp for
    for (int i = 0; i < n; i++)
    {
        y[i]/=L[i][i];
    }

    for (int j = 0; j < n; j++)
    {
        // printf("at col %d with threadid %d\n", j, omp_get_thread_num());
        // printf("i: %d y_i: %lf, x_i: %lf, L[i][i]: %lf\n", j, x[8 * j], y[8 * j], L[j][j]);
#pragma omp for
        for (int i = j + 1; i < n; i++)
        {
            // printf("in thread no %d\n", omp_get_thread_num());
            y[i] -= (x[j] * L[i][j] / L[i][i]);
        }
    }
}
    end = omp_get_wtime();
    // for (int i = 0; i < n; i++)
    // {
    //     printf("%Lf ", x[i]);
    // }
    printf("Solution took %lf seconds--------------------\n", end - start);
    return 0;
}
