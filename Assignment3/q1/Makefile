CC = gcc
CUDACC = nvcc
CFLAGS = -g -Wall -O3 -fopenmp
CUDAFLAGS = -O3

omp:
	$(CC) omp_gauss-seidel_block_row.c $(CFLAGS) -o omp_gs
basic:
	$(CUDACC) cuda-gs-basic.cu $(CUDAFLAGS) -o gs_basic
tree:
	$(CUDACC) cuda-gs-tree-red.cu $(CUDAFLAGS) -o gs_tree
clean:
	rm -rf omp_gs omp_gsblockrow_outfile* gs_basic checker kernel gs_tree