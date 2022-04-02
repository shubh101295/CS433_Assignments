CC = g++
CFLAGS = -g -Wall -O3 -fopenmp

omp_lock:
	$(CC) omp_main_lock.cpp $(CFLAGS) -o omp_main_lock

clean:
	rm -rf omp_main_lock