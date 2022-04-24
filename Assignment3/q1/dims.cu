#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

__global__ void init_kernel ()
{
    printf("Block[%d, %d], thread[%d, %d], Dims[%d, %d]\n", blockIdx.x, blockIdx.y, threadIdx.x, threadIdx.y, blockDim.x, blockDim.y);
}

int main (int argc, char *argv[])
{

	float *a;
    int num_thread_blocks_x = 1, num_thread_blocks_y = 3;
    int num_threads_per_block_x = 2, num_threads_per_block_y = 2;

    printf("choosing gridDims: %d, %d\n", num_thread_blocks_x, num_thread_blocks_y);
    printf("choosing blockDims: %d, %d\n\n", num_threads_per_block_x, num_threads_per_block_y);


    dim3 dimBlock(num_threads_per_block_x, num_threads_per_block_y);
    dim3 dimGrid(num_thread_blocks_x, num_thread_blocks_y);

    init_kernel<<<dimGrid, dimBlock>>>();

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        printf("Failed to launch kernel (error code: %s)!\n", cudaGetErrorString(err));
    }
    cudaDeviceSynchronize();

	return 0;
}
