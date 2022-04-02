#include<bits/stdc++.h>
#include<pthread.h>
#include <sys/time.h>
using namespace std;

#define N 10000

int num_threads;
pthread_mutex_t mutex_for_print = PTHREAD_MUTEX_INITIALIZER;

struct bar_type1 {
	int counter;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	bool flag = false;
};

struct bar_type1 bar_name1;

// struct local_sense1{
// 	bool local_sense = false;
// 	char c[63];
// }

void barrier_sense_reversal(struct bar_type1* bar_name, int P,bool local_sense)
{
	local_sense = !local_sense;
	// pthread_mutex_lock(&mutex_for_print);
	// cout<<"Local sence == "<<local_sense<<" and P=="<<P<<"\n";
	
	// pthread_mutex_unlock(&mutex_for_print);



	pthread_mutex_lock(&bar_name->mutex);
	bar_name->counter+=1;
	if (bar_name->counter == P)
	{
		pthread_mutex_unlock(&bar_name->mutex);
		bar_name->counter = 0;
		bar_name->flag = local_sense;
	}
	else{
		pthread_mutex_unlock(&bar_name->mutex);
		while(bar_name->flag != local_sense);
	}
}

void* barrier_sense_reversal_caller(void *param)
{
	// int local_sense = 0;
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
		// pthread_mutex_lock(&mutex_for_print);
		// cout<<"BArrier Start for "<<i<<" in thread "<<id<<"\n";
		// pthread_mutex_unlock(&mutex_for_print);

		barrier_sense_reversal(&bar_name1,num_threads,((i%2)!=0));

		if (id==0 && i%100==0)
		{
			cout<<"BArrier Ended for "<<i<<" in thread "<<id<<"\n";
			// cout<<
		}
		// pthread_mutex_lock(&mutex_for_print);
		// pthread_mutex_unlock(&mutex_for_print);

	}
}

/* b part */ 
void* tree_barrier_caller_part_b(void *param)
{
	
}

int main(int argc,char *argv[]){
	if(argc!=2)
	{
		cout<<"Need number of threads\n";
		exit(1);
	}
	pthread_attr_t attr;
	num_threads = atoi(argv[1]);
	struct timeval tv0, tv1;
	struct timezone tz0, tz1;
	bar_name1.counter = 0;
	pthread_t *tid;
	int *id;

	tid = (pthread_t*)malloc(num_threads*sizeof(pthread_t));
	id = (int*)malloc(num_threads*sizeof(int));
 	for (int i=0; i<num_threads; i++) id[i] = i;
	
	pthread_attr_init(&attr);
	gettimeofday(&tv0, &tz0);
	for(int i=0;i<num_threads;i++)
	{
		pthread_create(&tid[i], &attr, barrier_sense_reversal_caller, &id[i]);
	}

	for (int i=0; i<num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

	gettimeofday(&tv1, &tz1);
	printf("Time: %ld microseconds\n", (tv1.tv_sec-tv0.tv_sec)*1000000+(tv1.tv_usec-tv0.tv_usec));
	return 0;
}