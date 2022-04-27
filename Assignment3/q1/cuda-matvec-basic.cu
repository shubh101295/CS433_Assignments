#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cuda.h>
#include <sys/time.h>
// #include <cuPrintf.h>

// #define ROWS_A  (1<<13)
// #define COLS_A  (1<<13)
#define TILE_SIZE 16
// int N = 64;
int numThr = 1;

__global__ void init_kernel (float *a, int N)
{
	int id = threadIdx.x + blockIdx.x*blockDim.x;

	a[id] = (float)id/(N*N);
	// printf("{%d}", id);
}

__global__ void matvecmult_kernel (float *a, float *b, float *c, int N)
{
	int c_row = blockIdx.y*blockDim.y + threadIdx.y;
	int i, j;
	float x = 0;

	for (i=0; i<N/TILE_SIZE; i++) {
		for (j=0; j<TILE_SIZE; j++) {
			x += (a[c_row * N + i*TILE_SIZE + j]*b[i*TILE_SIZE + j]);
			// d[0] = as[threadIdx.y][0]; d[1] = bs[0]; d[2] = as[threadIdx.y][1]; d[3] = bs[1];
		}
	}

	c[c_row] = x;
}


int main(int argc, char *argv[]){
    float *a, *b, *c;
	struct timeval tv0, tv2;
	struct timezone tz0, tz2;
	srand(time(0));
	int N;
    cudaMallocManaged((void**)&N, sizeof(int));
	N = atoi(argv[1]);

    cudaMallocManaged((void**)&a, sizeof(float)*N*N);
	cudaMallocManaged((void**)&b, sizeof(float)*N*1);
	cudaMallocManaged((void**)&c, sizeof(float)*N*1);

    int device = -1;
        cudaGetDevice(&device);
	cudaMemAdvise(c, sizeof(float)*N, cudaMemAdviseSetPreferredLocation, device);

    init_kernel<<<N*N/1024, 1024>>>(a, N);
	init_kernel<<<N/1024, 1024>>>(b, N);
	cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        printf("Failed to launch kernel (error code: %s)!\n", cudaGetErrorString(err));
    }

    cudaDeviceSynchronize();
	gettimeofday(&tv0, &tz0);

    dim3 dimBlock(TILE_SIZE, TILE_SIZE);
	dim3 dimGrid(N/TILE_SIZE, N/TILE_SIZE);

    matvecmult_kernel<<<dimGrid, dimBlock>>>(a, b, c, N);
    cudaDeviceSynchronize();
	gettimeofday(&tv2, &tz2);

	int rowC = random() % N;

	float x = 0;

	for (int i=0; i<N; i++) x += a[rowC*N + i]*b[i];
	float error = fabs(c[rowC] - x);
	printf("Error: %0.12f, computed value: %0.12f, actual value: %0.12f, time: %ld microseconds\n", error, c[rowC], x, (tv2.tv_sec-tv0.tv_sec)*1000000+(tv2.tv_usec-tv0.tv_usec));

    return 0;
}
