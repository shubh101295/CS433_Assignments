#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<assert.h>
#include<omp.h>

// #define RAND 1

long double **L, *y, *x, *diag;
char *name_in, *name_out;
int n, nthreads;

void InitializeInput(long double** L, long double* x, long double* y)
{
    // FILE* fout = fopen("random.txt", "w");
    // fprintf(fout, "%d\n", n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            L[j][i - j] = ((long double)(random() % 100)) / 500.0 + 0.1;
            if(i == j) L[j][0] = 2;
            // fprintf(fout, "%Lf ", L[j][i-j]);
            // printf("%Lf ", L[j][i- j]);
        }
        // fprintf(fout, "\n");
        // printf("\n");
    }
    for (int i = 0; i < n; i++)
    {
        x[i] = ((long double)(random() % 100)) / 500.0 + 0.1;
        y[i] = 0;
        // printf("%Lf ", x[i]);
    }
        // printf("\n\n\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <=i; j++)
        {
            y[i]+=(L[j][i - j] * x[j]);
        }
        // fprintf(fout, "%Lf ", y[i]);
    }
    for (int i = 0; i < n; i++)
    {
        x[i] = ((long double)(random() % 100)) / 500.0 + 0.1;
    }
    // fclose(fout);
}

int main(int argc, char *argv[])
{
#ifdef RAND
    n = strtol(argv[1], NULL, 10);
    nthreads = strtol(argv[2], NULL, 10);
#else
    name_in = argv[1];
    name_out = argv[2];
    nthreads = strtol(argv[3], NULL, 10);
    FILE *fin, *fout;
    fin = fopen(name_in, "r");
    fout = fopen(name_out, "w");
    fscanf(fin, "%d", &n);
#endif

    L = (long double**)malloc(  (n)*sizeof(long double*) );
    assert(L != NULL);
    for (int i = 0; i < n; i++)
    {
        L[i] = (long double*)malloc((n - i)*sizeof(long double));
        assert(L[i] != NULL);
    }
    y = (long double*)malloc((n) * sizeof(long double));
    x = (long double*)malloc((n) * sizeof(long double));
    diag = (long double*)malloc((n) * sizeof(long double));
    assert(y != NULL);
    assert(x != NULL);
    assert(diag != NULL);

#ifdef RAND
    InitializeInput(L, x, y);
    for (int i = 0; i < n; i++)
    {
        diag[i] = L[i][0];
    }
#else
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j <= i; j++)
        {
            fscanf(fin, "%Lf", &L[j][i - j]);
            if(i == j) diag[i] = L[j][i -j];
        }
    }
    for(int i = 0; i < n; i++)
    {
        fscanf(fin, "%Lf", &y[i]);
    }
#endif
    // Solving
    double start = 0.0, end = 0.0;
    start = omp_get_wtime();

#pragma omp parallel num_threads(nthreads)
{
#pragma omp for
    for(int i = 0; i < n; i++)
    {
        x[i] = y[i] / diag[i];
    }
    // printf("finished norm\n");

    for (int j = 0; j < n; j++)
    {
#pragma omp for
        for (int i = j + 1; i < n; i++)
        {
            x[i] -= (x[j] * L[j][i - j] / diag[i]);
        }
    }
}
    end = omp_get_wtime();
#ifdef RAND
    // for (int i = 0; i < n; i++)
    // {
    //     printf("%Lf ", x[i]);
    // }
#else
    for(int i = 0; i < n; i++)
    {
        fprintf(fout, "%Lf ", x[i]);
    }
    fclose(fin);
    fclose(fout);
#endif
    printf("\nSolution took %lf seconds--------------------\n", end - start);

    free(x);
    free(y);
    free(diag);
    for (int i = 0; i < n; i++)
    {
        free(L[i]);
    }
    free(L);
    return 0;
}
