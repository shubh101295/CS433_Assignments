#include<omp.h>
#include<bits/stdc++.h>

using namespace std;

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
    cout<<"time taken is "<<total<<" with x being "<<x<<" \n";
}