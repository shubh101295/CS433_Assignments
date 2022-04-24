CC = gcc
CFLAGS = -g -Wall -O3 -fopenmp

omp:
	$(CC) omp_gauss-seidel_block_row.c $(CFLAGS) -o omp_gs

clean:
	rm -rf omp_gs omp_gsblockrow_outfile* gs-basic checker kernel