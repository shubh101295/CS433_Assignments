#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#include "sync_library.c"

#define N 1000000

struct bar_type1 bar_name1;
struct bar_type2 bar_name2;
struct bar_type3 bar_name3;
struct bar_type4 bar_name4;
struct bar_type5 bar_name5;
	
int num_threads;


/* benchmarking Centralized sense-reversing barrier using busy-wait on flag */

void* barrier_sense_reversal_caller(void *param)
{
	// int local_sense = 0;
	int  id = *(int*)(param);
	int local_sense = 0;
	for( int i=0;i<N;i++)
	{
		barrier_sense_reversal(&bar_name1,num_threads,&local_sense);
		if (id==0 && i%100==0)
		{
			printf("%d Ended \n",i);
		}
	}
}

/* benchmarking Tree barrier using busy-wait on flags */

void* tree_barrier_caller_part_b(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
	
		tree_barrier(&bar_name2,id,num_threads);
		if (id==0 && i%100==0)
		{
			printf("%d Ended from tree_barrier_caller_part_b\n",i);
		}
	}	
}

/* benchmarking Centralized barrier using POSIX condition variable */

void* centralised_barrier_using_posix_condition_variable_caller_part_c(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
	
		centralised_barrier_using_posix_condition_variable(&bar_name3,num_threads);

		if (id==0 && i%100==0)
		{
			printf("%d Ended from centralised_barrier_using_posix_condition_variable_caller_part_c\n",i);
		}
	}	
}

/* benchmarking Tree barrier using POSIX condition variable */

void* tree_barrier_using_posix_condition_variable_caller_part_d(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{

		tree_barrier_using_posix_condition_variable(&bar_name4,id,num_threads);
		if (id==0 && i%100==0)
		{
			printf("%d Ended from tree_barrier_using_posix_condition_variable_caller_part_d\n",i);
		}
		
	}	
}

/* benchmarking */

void* posix_barrier_interface_caller(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
		posix_barrier_interface(&bar_name5);
	}	
}


int main(int argc,char *argv[]){
	if(argc!=2)
	{
		printf("Need number of threads\n");
		exit(1);
	}
	pthread_attr_t attr;
	num_threads = atoi(argv[1]);
	struct timeval tv0, tv1;
	struct timezone tz0, tz1;
	pthread_t *tid;
	int *id;

	tid = (pthread_t*)malloc(num_threads*sizeof(pthread_t));
	id = (int*)malloc(num_threads*sizeof(int));
 	for (int i=0; i<num_threads; i++) id[i] = i;
	


	pthread_attr_init(&attr);
	gettimeofday(&tv0, &tz0);

	int run_for_type= 4;
	if(run_for_type==0)
	{
		barrier_sense_reversal_init(&bar_name1);
		/* part a */
		for(int i=0;i<num_threads;i++)
		{
			pthread_create(&tid[i], &attr, barrier_sense_reversal_caller, &id[i]);
		}
		for (int i=0; i<num_threads; i++) {
			pthread_join(tid[i], NULL);
		}
	}
	else if(run_for_type==1)
	{
		/* part b */
		tree_barrier_init(&bar_name2,num_threads);
		for(int i=0;i<num_threads;i++)
		{
			pthread_create(&tid[i], &attr, tree_barrier_caller_part_b, &id[i]);
		}

		for (int i=0; i<num_threads; i++) {
			pthread_join(tid[i], NULL);
		}
	}
	else if(run_for_type==2)
	{
		/* part c */
		centralised_barrier_using_posix_condition_variable_init(&bar_name3);
		for(int i=0;i<num_threads;i++)
		{
			pthread_create(&tid[i], &attr, centralised_barrier_using_posix_condition_variable_caller_part_c, &id[i]);
		}

		for (int i=0; i<num_threads; i++) {
			pthread_join(tid[i], NULL);
		}
	}
	else if(run_for_type==3)
	{
		/* part d */
		tree_barrier_posix_condition_init(&bar_name4,num_threads);
		for(int i=0;i<num_threads;i++)
		{
			pthread_create(&tid[i], &attr, tree_barrier_using_posix_condition_variable_caller_part_d, &id[i]);
		}

		for (int i=0; i<num_threads; i++) {
			pthread_join(tid[i], NULL);
		}
	}
	else if(run_for_type==4)
	{
		/* part e */
		posix_barrier_interface_init(&bar_name5,num_threads);
		for(int i=0;i<num_threads;i++)
		{
			pthread_create(&tid[i], &attr, posix_barrier_interface_caller, &id[i]);
		}

		for (int i=0; i<num_threads; i++) {
			pthread_join(tid[i], NULL);
		}

	}


	gettimeofday(&tv1, &tz1);

	printf("Time: %ld microseconds\n", (tv1.tv_sec-tv0.tv_sec)*1000000+(tv1.tv_usec-tv0.tv_usec));
	return 0;
}


	// bar_name1.counter = 0;
	// bar_name1.flag = 0;
	// pthread_mutex_init(&bar_name1.mutex,NULL);
	// bar_name2.counter = 0;
	// pthread_mutex_init(&bar_name2.mutex,NULL);
	// pthread_cond_init(&bar_name2.cv,NULL);
	// pthread_barrier_init(&barrier_from_p_thread,NULL,num_threads);

	// flag = (int **)malloc(sizeof(int*)*num_threads);
	// cvs = (pthread_cond_t **)malloc(sizeof(pthread_cond_t*)*num_threads);
	// cv_locks = (pthread_mutex_t **)malloc(sizeof(pthread_mutex_t*)*num_threads);
	
 //    for (int i = 0; i<num_threads; i++) {
 //        flag[i] = (int *)malloc(sizeof(int)*num_threads);
 //        cvs[i] = (pthread_cond_t *)malloc(sizeof(pthread_cond_t)*num_threads);
 //        cv_locks[i] = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)*num_threads);
        
 //        for (int j = 0; j<num_threads; j++) {
 //            flag[i][j] = 0;
 //        	pthread_mutex_init(&cv_locks[i][j],NULL);
	// 		pthread_cond_init(&cvs[i][j],NULL);
 //        }
 //    }
 //    MAX = 0;
	// int num_threads2 = num_threads;
	// while(num_threads2)
	// {
	// 	MAX+=1;
	// 	num_threads2/=2;
	// }