CC = gcc
CFLAGS = -g -Wall -O3 -fopenmp -lpthread

locks:
	$(CC) locks.c $(CFLAGS) -o locks

omp_lock:
	$(CC) omp_main_lock.c $(CFLAGS) -o omp_main_lock

clean:
	rm -rf omp_main_lock locks