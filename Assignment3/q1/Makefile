CC = gcc
CUDACC = nvcc
CFLAGS = -g -Wall -O3 -fopenmp
CUDAFLAGS = -O3

omp:
	$(CC) omp_gauss-seidel_block_row.c $(CFLAGS) -o omp_gs
gs_basic:
	$(CUDACC) cuda-gs-basic.cu $(CUDAFLAGS) -o gs_basic
mv_basic:
	$(CUDACC) cuda-matvec-basic.cu $(CUDAFLAGS) -o mv_basic
gs_tree:
	$(CUDACC) cuda-gs-tree-red.cu $(CUDAFLAGS) -o gs_tree
gs_shmem:
	$(CUDACC) cuda-gs-shmem.cu $(CUDAFLAGS) -o gs_shmem
mv_shmem:
	$(CUDACC) cuda-matvec-shmem.cu $(CUDAFLAGS) -o mv_shmem
clean:
	rm -rf omp_gs omp_gsblockrow_outfile* gs_basic checker kernel gs_tree gs_shmem mv_shmem mv_basic