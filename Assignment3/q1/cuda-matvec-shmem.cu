#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cuda.h>
#include <sys/time.h>

#define TILE 32
#define NUM_THREADS_PER_BLOCK 1024

__global__ void init_kernel (float *a, int n, int t, int span)
{
	int id = threadIdx.x + blockIdx.x*blockDim.x;
    for(int i = id * span; i < (id + 1) * span; i++)
    {
        a[i] = (float)i / (n*n);
    }
}

__global__ void kernel(float *a, float *b, float *c, int n)
{
	int c_row = blockIdx.x * blockDim.y + threadIdx.y;
	unsigned mask = 0xffffffff;
	float x = 0;
	__shared__ float atile[TILE][TILE];
	__shared__ float btile[TILE];

	for (int i=0; i< n / TILE; i++)
    {
		atile[threadIdx.y][threadIdx.x] = a[c_row * n + i * TILE + threadIdx.x];
		btile[threadIdx.x] = b[(i * TILE) + threadIdx.x];

		__syncthreads();
        if((threadIdx.x % warpSize) == 0)x += atile[threadIdx.y][threadIdx.x] * btile[threadIdx.x];
		else x = atile[threadIdx.y][threadIdx.x] * btile[threadIdx.x];

		__syncthreads();
		for (int j=warpSize/2; j>0; j=j/2) x += __shfl_down_sync(mask, x, j);
		__syncthreads();
	}

	if ((threadIdx.x % warpSize) == 0) c[c_row] = x;
	__syncthreads();
}

int main(int argc, char *argv[]){
    float *a, *b, *c;
	int n, t;
	struct timeval tv0, tv2;
	struct timezone tz0, tz2;
	n = atoi(argv[1]);
	t = atoi(argv[2]);

    if(t >= n * n)
    {
        t = n * n;
    }

    cudaMallocManaged((void**)&a, sizeof(float)*n*n);
	cudaMallocManaged((void**)&b, sizeof(float)*n);
	cudaMallocManaged((void**)&c, sizeof(float)*n);

    int device;
    cudaGetDevice(&device);

	cudaMemAdvise(a, sizeof(float) * n * n, cudaMemAdviseSetPreferredLocation, device);
	cudaMemAdvise(b, sizeof(float) * n, cudaMemAdviseSetPreferredLocation, device);
	cudaMemAdvise(c, sizeof(float) * n, cudaMemAdviseSetPreferredLocation, device);

    int span = (n * n) / t;
    if(t >= 1024)
    {
        init_kernel<<<t / 1024, 1024>>>(a, n, t, span);
    }
    else
    {
        init_kernel<<<1, t>>>(a, n, t, span);
    }
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        printf("Failed to launch kernel (error code: %s)!\n", cudaGetErrorString(err));
    }
	cudaDeviceSynchronize();
    int tc_n = t;
    if(t >= n)
    {
        tc_n = n;
    }
    span = n / tc_n;
    if(tc_n >= 1024)
    {
        init_kernel<<<tc_n / 1024, 1024>>>(b, n, t, span);
    }
    else
    {
        init_kernel<<<1, tc_n>>>(b, n, tc_n, span);
    }
    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        printf("Failed to launch kernel (error code: %s)!\n", cudaGetErrorString(err));
    }

	cudaDeviceSynchronize();
	gettimeofday(&tv0, &tz0);

    if(t < NUM_THREADS_PER_BLOCK)
    {
        int left = (TILE * TILE) / t;
        int adjusted_block = TILE / left;
        dim3 dimBlock(TILE, adjusted_block);

		if(t > (n / adjusted_block) * (TILE * adjusted_block))
        {
			dim3 dimGrid(n/ adjusted_block);
    		kernel<<<dimGrid, dimBlock>>>(a, b, c, n);
		}
        else
        {
			dim3 dimGrid(t / (TILE * adjusted_block));
			for(int i = 0; i < n; i += (t / (TILE * adjusted_block)) * adjusted_block)
            {
    			kernel<<<dimGrid, dimBlock>>>(a + i * n, b, c + i, n);
			}

        }
    }
    else
    {
        dim3 dimBlock(TILE, TILE);
		if(t > n * TILE)
        {
			dim3 dimGrid(n / TILE);
    		kernel<<<dimGrid, dimBlock>>>(a, b, c, n);
		}
		else
        {
			dim3 dimGrid(t / (TILE * TILE));
			for(int i = 0; i < n; i += (t/(TILE * TILE)) * TILE)
            {
    			kernel<<<dimGrid, dimBlock>>>(a + i * n, b, c + i, n);
			}
        }
    }
    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        printf("Failed to launch kernel (error code: %s)!\n", cudaGetErrorString(err));
    }
    cudaDeviceSynchronize();
	gettimeofday(&tv2, &tz2);

	srand(time(0));
	int rowC = random() % n;
	float x = 0;
	for (int i=0; i<n; i++) x += a[rowC*n + i]*b[i];
	float error = fabs(c[rowC] - x);
	printf("Error: %0.12f, computed value: %0.12f, actual value: %0.12f, time: %ld microseconds\n", error, c[rowC], x, (tv2.tv_sec-tv0.tv_sec)*1000000+(tv2.tv_usec-tv0.tv_usec));
    return 0;
}
