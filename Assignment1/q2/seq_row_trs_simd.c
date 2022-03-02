#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<assert.h>
#include<omp.h>

#define SIZE (10)

float **L, *y, *x;
int n;

void InitializeInput(float** L, float* x, float* y)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            L[i][j] = ((float)(random() % 100)) / 100.0 + 0.01;
        }
    }
    for (int i = 0; i < n; i++)
    {
        y[i] = ((float)(random() % 100)) / 100.0 + 0.01;
    }
    for (int i = 0; i < n; i++)
    {
        x[i] = ((float)(random() % 100)) / 100.0 + 0.01;
    }
}

int main(int argc, char *argv[])
{
    n = strtol(argv[1], NULL, 10);
    // Input taking!!
    L = (float**)malloc( (n)*sizeof(float*) );
    assert(L != NULL);
    for (int i = 0; i < n; i++)
    {
        L[i] = (float*)malloc((i + 1)*sizeof(float));
        assert(L[i] != NULL);

        // for (int j = 0; j <= i; j++)
        // {
        //     scanf("%f", &L[i][j]);
        // }

    }
    y = (float*)malloc((n) * sizeof(float));
    x = (float*)malloc((n) * sizeof(float));
    assert(y != NULL);
    assert(x != NULL);
    InitializeInput(L, x, y);
    // for (int i = 0; i < n; i++)
    // {
    //     scanf("%f", &y[i]);
    //     x[i] = ((float)(random() % 100)) / 100;
    // }

    // Solving
    double start = 0.0, end = 0.0;
    start = omp_get_wtime();
    for (int i = 0; i < n; i++)
    {
        #pragma omp simd reduction(-:y[i]) aligned(L, x, y)
        for (int j = 0; j < i; j++)
        {
            y[i] -= (x[j] * L[i][j]);
        }
        x[i] = y[i] / L[i][i];
    }
    end = omp_get_wtime();
    printf("Solution took %f seconds--------------------\n", end - start);
    // for (int i = 0; i < n; i++)
    // {
    //     printf("%f ", x[i]);
    // }
    return 0;
}