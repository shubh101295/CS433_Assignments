#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<assert.h>
#include<omp.h>

#define SIZE (10)

double **L, *y, *x;
int n;

void InitializeInput(double** L, double* x, double* y)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            L[i][j] = ((double)(random() % 100)) / 100.0 + 0.01;
        }
    }
    for (int i = 0; i < n; i++)
    {
        y[i] = ((double)(random() % 100)) / 100.0 + 0.01;
    }
    for (int i = 0; i < n; i++)
    {
        x[i] = ((double)(random() % 100)) / 100.0 + 0.01;
    }
}

int main(int argc, char *argv[])
{
    n = strtol(argv[1], NULL, 10);
    // Input taking!!
    L = (double**)malloc( (n)*sizeof(double*) );
    assert(L != NULL);
    for (int i = 0; i < n; i++)
    {
        L[i] = (double*)malloc((i + 1)*sizeof(double));
        assert(L[i] != NULL);

        // for (int j = 0; j <= i; j++)
        // {
            // scanf("%f", &L[i][j]);
        // }

    }
    y = (double*)malloc((n) * sizeof(double));
    x = (double*)malloc((n) * sizeof(double));
    assert(y != NULL);
    assert(x != NULL);
    InitializeInput(L, x, y);
    // for (int i = 0; i < n; i++)
    // {
    //     scanf("%f", &y[i]);
    //     x[i] = ((double)(random() % 100)) / 100;
    // }

    // Solving
    double start = 0.0, end = 0.0;
    start = omp_get_wtime();
    for (int j = 0; j < n; j++)
    {
        x[j] = y[j] / L[j][j];

        for (int i = j + 1; i < n; i++)
        {
            y[i] -= (x[j] * L[i][j]);
        }
    }
    end = omp_get_wtime();
    printf("Solution took %f seconds--------------------\n", end - start);
    // for (int i = 0; i < n; i++)
    // {
    //     printf("%f ", x[i]);
    // }
    return 0;
}
