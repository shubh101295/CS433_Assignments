CC = g++
CFLAGS = -g -Wall -O3 -fopenmp -lpthread

locks:
	$(CC) locks.cpp $(CFLAGS) -o locks

omp_lock:
	$(CC) omp_main_lock.cpp $(CFLAGS) -o omp_main_lock

clean:
	rm -rf omp_main_lock locks