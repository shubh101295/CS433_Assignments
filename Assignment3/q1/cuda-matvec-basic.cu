#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <cuda.h>
#include <curand_kernel.h>
#include <curand.h>

#define THREADS_PER_BLOCK 1024
#define TILE_SIZE 16

__global__ void init_kernel (float *a, int n)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;
    a[id] = (float)id / (n * n);
}

__global__ void kernel(float *a, float*b, float*c, int n)
{

}

int main (int argc, char *argv[])
{

	float *a, *b, *c;
    int n, t;
    struct timeval tv0, tv2;
    struct timezone tz0, tz2;

    n = atoi(argv[1]);

	cudaMallocManaged((void**)&a, sizeof(float) * (n) * (n));
	cudaMallocManaged((void**)&b, sizeof(float) * (n));
	cudaMallocManaged((void**)&c, sizeof(float) * (n));

    int device = -1;
    cudaGetDevice(&device);
    cudaMemAdvise(a, sizeof(float) * (n) * (n), cudaMemAdviseSetPreferredLocation, device);
    cudaMemAdvise(b, sizeof(float) * (n), cudaMemAdviseSetPreferredLocation, device);
    cudaMemAdvise(c, sizeof(float) * (n), cudaMemAdviseSetPreferredLocation, device);

    if((n) * (n) >= THREADS_PER_BLOCK)
    {
        init_kernel<<<(n * n)/ THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(a, n);
    }
    else
    {
        init_kernel<<<1, (n) * (n)>>>(a, n);
    }

    if((n) >= THREADS_PER_BLOCK)
    {
        init_kernel<<<(n)/ THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(b, n);
    }
    else
    {
        init_kernel<<<1, (n)>>>(b, n);
    }

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        printf("Failed to launch kernel (error code: %s)!\n", cudaGetErrorString(err));
    }
	cudaDeviceSynchronize();

    dim3 dimBlock(TILE_SIZE, TILE_SIZE);
    dim3 dimGrid(n / TILE_SIZE, n / TILE_SIZE);

    gettimeofday(&tv0, &tz0);
    kernel<<<dimGrid, dimBlock>>>(a, b, c, n);

    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        printf("Failed to launch kernel (error code: %s)!\n", cudaGetErrorString(err));
    }
	cudaDeviceSynchronize();
    gettimeofday(&tv2, &tz2);

    printf("time: %ld microseconds\n", (tv2.tv_sec-tv0.tv_sec)*1000000+(tv2.tv_usec-tv0.tv_usec));
	return 0;
}
