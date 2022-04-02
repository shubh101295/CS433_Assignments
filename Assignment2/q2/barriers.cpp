#include<bits/stdc++.h>
#include<pthread.h>
#include <sys/time.h>
using namespace std;

#define N 10

int num_threads;
pthread_mutex_t mutex_for_print = PTHREAD_MUTEX_INITIALIZER;
int MAX; // log(num_threads)


struct bar_type1 {
	int counter;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	bool flag = false;
};

struct bar_type1 bar_name1;

struct bar_type2 {
	int counter;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cv;
};

struct bar_type2 bar_name2;



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

		// if (id==0 && i%100==0)
		// {
		// 	cout<<"BArrier Ended for "<<i<<" in thread "<<id<<"\n";
		// 	// cout<<
		// }
		// pthread_mutex_lock(&mutex_for_print);
		// pthread_mutex_unlock(&mutex_for_print);

	}
}

/* part b */ 

vector<vector<int> > flag;

void tree_barrier(int pid,int P){
	unsigned int i,mask;
	for(i=0,mask=1;(mask&pid)!=0;++i,mask<<=1)
	{
		while(!flag[pid][i]);
		flag[pid][i]= 0;
	}
	if( pid < (P-1)) {
		flag[pid+mask][i] = 1;
		while(!flag[pid][MAX-1]);
		flag[pid][MAX-1]=0;
	}
	for(mask>>=1;mask>0;mask>>=1)
	{
		flag[pid-mask][MAX-1] = 1;
	}
}

/* b part */ 
void* tree_barrier_caller_part_b(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
		// pthread_mutex_lock(&mutex_for_print);
		// cout<<"BArrier Start for "<<i<<" in thread "<<id<<"\n";
		// pthread_mutex_unlock(&mutex_for_print);

		tree_barrier(id,num_threads);

		// if (id==0 && i%100==0)
		// {
		// 	cout<<"BArrier Ended for "<<i<<" in thread "<<id<<"\n";
		// 	// cout<<
		// }
		// pthread_mutex_lock(&mutex_for_print);
		// pthread_mutex_unlock(&mutex_for_print);

	}	
}

/* c part */ 

// void print(u)
void centralised_barrier_using_posix_condition_variable(struct bar_type2* bar_name, int P) {

	pthread_mutex_lock(&bar_name->mutex);
	bar_name->counter+=1;
	if (bar_name->counter == P)
	{
		bar_name->counter = 0;
		pthread_cond_broadcast(&bar_name->cv);
	}
	else{
	    pthread_cond_wait(&bar_name->cv,&bar_name->mutex);
	  //   	pthread_mutex_lock(&mutex_for_print);
			// cout<<"WAIT RELEASED \n";
			// pthread_mutex_unlock(&mutex_for_print);

	}
	pthread_mutex_unlock(&bar_name->mutex);

}

void* centralised_barrier_using_posix_condition_variable_caller_part_c(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
		// pthread_mutex_lock(&mutex_for_print);
		// cout<<"BArrier Start for "<<i<<" in thread "<<id<<"\n";
		// pthread_mutex_unlock(&mutex_for_print);

		centralised_barrier_using_posix_condition_variable(&bar_name2,num_threads);

		if (id==0 && i%1==0)
		{
			cout<<"BArrier Ended for "<<i<<" in thread "<<id<<"\n";
			// cout<<
		}
		// pthread_mutex_lock(&mutex_for_print);
		// pthread_mutex_unlock(&mutex_for_print);

	}	
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
	bar_name2.counter = 0;
	
	pthread_t *tid;
	int *id;

	tid = (pthread_t*)malloc(num_threads*sizeof(pthread_t));
	id = (int*)malloc(num_threads*sizeof(int));
 	for (int i=0; i<num_threads; i++) id[i] = i;
	


 		/* part b */ 
	// MAX = 0;
	// int num_threads2 = num_threads;
	// while(num_threads2)
	// {
	// 	MAX+=1;
	// 	num_threads2/=2;
	// }
	// flag.resize(num_threads);
	// for(int i=0;i<num_threads;i++)
	// {
	// 	flag[i].resize(MAX);
	// 	for(int j=0;j<MAX;j++) flag[i][j]=0;
	// }

	pthread_attr_init(&attr);
	gettimeofday(&tv0, &tz0);

	/* part a */
	// for(int i=0;i<num_threads;i++)
	// {
	// 	pthread_create(&tid[i], &attr, barrier_sense_reversal_caller, &id[i]);
	// }

	// for (int i=0; i<num_threads; i++) {
	// 	pthread_join(tid[i], NULL);
	// }


	/* part b */

	// for(int i=0;i<num_threads;i++)
	// {
	// 	pthread_create(&tid[i], &attr, tree_barrier_caller_part_b, &id[i]);
	// }

	// for (int i=0; i<num_threads; i++) {
	// 	pthread_join(tid[i], NULL);
	// }

	/* part c */
	for(int i=0;i<num_threads;i++)
	{
		pthread_create(&tid[i], &attr, centralised_barrier_using_posix_condition_variable_caller_part_c, &id[i]);
	}

	for (int i=0; i<num_threads; i++) {
		pthread_join(tid[i], NULL);
	}


	gettimeofday(&tv1, &tz1);

	printf("Time: %ld microseconds\n", (tv1.tv_sec-tv0.tv_sec)*1000000+(tv1.tv_usec-tv0.tv_usec));
	return 0;
}