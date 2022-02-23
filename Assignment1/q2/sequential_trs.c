#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<assert.h>

#define SIZE (10)

float **L, *y, *x;
int n;

void InitializeInput(float** L, float* y, float* x)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            L[i][j] = ((float)(random() % 100)) / 100.0;
        }
    }
    for (int i = 0; i < n; i++)
    {
        y[i] = ((float)(random() % 100)) / 100.0;
    }
    for (int i = 0; i < n; i++)
    {
        x[i] = ((float)(random() % 100)) / 100.0;
    }
}

int main()
{
    scanf("%d", &n);

    // Input taking!!
    L = (float**)malloc( (n)*sizeof(float*) );
    assert(L != NULL);
    for (int i = 0; i < n; i++)
    {
        L[i] = (float*)malloc((i + 1)*sizeof(float));
        assert(L[i] != NULL);

        for (int j = 0; j <= i; j++)
        {
            scanf("%f", &L[i][j]);
        }

    }
    y = (float*)malloc((n) * sizeof(float));
    x = (float*)malloc((n) * sizeof(float));
    assert(y != NULL);
    assert(x != NULL);
    for (int i = 0; i < n; i++)
    {
        scanf("%f", &y[i]);
        x[i] = ((float)(random() % 100)) / 100;
    }

    // Solving
    for (int j = 0; j < n; j++)
    {
        x[j] = y[j] / L[j][j];

        for (int i = j + 1; i < n; i++)
        {
            // printf("at column %d\n", j);
            // printf("y[%d] is now %f\n", j, y[j]);
            y[i] -= (x[j] * L[i][j]);
            // printf("y[%d] is now %f\n", j, y[j]);
        }
    }
    printf("Solution--------------------\n");
    for (int i = 0; i < n; i++)
    {
        printf("%f ", x[i]);
    }
    return 0;
}