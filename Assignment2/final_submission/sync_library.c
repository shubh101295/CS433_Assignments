#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>


/* struct for Centralized sense-reversing barrier using busy-wait on flag */
struct bar_type1 {
	int counter;
	pthread_mutex_t mutex;
	int flag;
};


/* Init for Centralized sense-reversing barrier using busy-wait on flag */
void barrier_sense_reversal_init(struct bar_type1* bar_name1)
{
	bar_name1->counter = 0;
	bar_name1->flag = 0;
	pthread_mutex_init(&bar_name1->mutex,NULL);
}


/* Centralized sense-reversing barrier using busy-wait on flag */
void barrier_sense_reversal(struct bar_type1* bar_name, int P,int* local_sense)
{
	(*local_sense) = !(*local_sense);

	pthread_mutex_lock(&bar_name->mutex);
	bar_name->counter+=1;
	if (bar_name->counter == P)
	{
		bar_name->counter = 0;
		pthread_mutex_unlock(&bar_name->mutex);
		bar_name->flag = *local_sense;
	}
	else{
		pthread_mutex_unlock(&bar_name->mutex);
		while(bar_name->flag != (*local_sense)){
			asm(""::
				:"memory");
		}
	}
}

/* struct for Tree barrier using busy-wait on flags */
struct bar_type2 {
	int** flag;
	int MAX;
};

/* Init for Tree barrier using busy-wait on flags */
void tree_barrier_init(struct bar_type2* bar_name1,int num_threads)
{	
	bar_name1->flag = (int **)malloc(sizeof(int*)*num_threads);
	for (int i = 0; i<num_threads; i++) {
        bar_name1->flag[i] = (int *)malloc(sizeof(int)*num_threads);
        
        for (int j = 0; j<num_threads; j++) {
            bar_name1->flag[i][j] = 0;
        }
    }
    bar_name1->MAX = 0;
	int num_threads2 = num_threads;
	while(num_threads2)
	{
		bar_name1->MAX+=1;
		num_threads2/=2;
	}
}


/* Tree barrier using busy-wait on flags */
void tree_barrier(struct bar_type2* bar_name, int pid,int P){
	unsigned int i,mask;
	for(i=0,mask=1;(mask&pid)!=0;++i,mask<<=1)
	{
		while(!bar_name->flag[pid][i]) {
			asm(""::
				:"memory");
		};
		bar_name->flag[pid][i]= 0;
	}
	if( pid < (P-1)) {
		bar_name->flag[pid+mask][i] = 1;
		while(!bar_name->flag[pid][bar_name->MAX-1]){
			asm(""::
				:"memory");
		}
		bar_name->flag[pid][bar_name->MAX-1]=0;
	}
	for(mask>>=1;mask>0;mask>>=1)
	{
		bar_name->flag[pid-mask][bar_name->MAX-1] = 1;
	}
}

/* struct for Centralized barrier using POSIX condition variable */
struct bar_type3 {
	int counter;
	pthread_mutex_t mutex;
	pthread_cond_t cv;
};

void centralised_barrier_using_posix_condition_variable_init(struct bar_type3* bar_name1){
	bar_name1->counter=0;
	pthread_mutex_init(&bar_name1->mutex,NULL);
	pthread_cond_init(&bar_name1->cv,NULL);
}

/* Barrier for Centralized barrier using POSIX condition variable */
void centralised_barrier_using_posix_condition_variable(struct bar_type3* bar_name, int P) {

	pthread_mutex_lock(&bar_name->mutex);
	bar_name->counter+=1;
	if (bar_name->counter == P)
	{
		bar_name->counter = 0;
		pthread_cond_broadcast(&bar_name->cv);
	}
	else{
	    pthread_cond_wait(&bar_name->cv,&bar_name->mutex);
	}
	pthread_mutex_unlock(&bar_name->mutex);

}

/* struct for Tree barrier using POSIX condition variable */
struct bar_type4 {
	int** flag;
	pthread_cond_t ** cvs;
	pthread_mutex_t ** cv_locks;
	int MAX;
};

/* Init for Tree barrier using POSIX condition variable */
void tree_barrier_posix_condition_init(struct bar_type4* bar_name1,int num_threads)
{	
	bar_name1->flag = (int **)malloc(sizeof(int*)*num_threads);
	bar_name1->cvs = (pthread_cond_t **)malloc(sizeof(pthread_cond_t*)*num_threads);
	bar_name1->cv_locks = (pthread_mutex_t **)malloc(sizeof(pthread_mutex_t*)*num_threads);
	
	for (int i = 0; i<num_threads; i++) {
        bar_name1->flag[i] = (int *)malloc(sizeof(int)*num_threads);
        bar_name1->cvs[i] = (pthread_cond_t *)malloc(sizeof(pthread_cond_t)*num_threads);
        bar_name1->cv_locks[i] = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)*num_threads);
        
        for (int j = 0; j<num_threads; j++) {
            bar_name1->flag[i][j] = 0;
            pthread_mutex_init(&bar_name1->cv_locks[i][j],NULL);
			pthread_cond_init(&bar_name1->cvs[i][j],NULL);
        }
    }
    bar_name1->MAX = 0;
	int num_threads2 = num_threads;
	while(num_threads2)
	{
		bar_name1->MAX+=1;
		num_threads2/=2;
	}
}

/* Tree barrier using POSIX condition variable */ 
void tree_barrier_using_posix_condition_variable(struct bar_type4* bar_name,int pid,int P){
	unsigned int i,mask;
	for(i=0,mask=1;(mask&pid)!=0;++i,mask<<=1)
	{
		pthread_mutex_lock(&bar_name->cv_locks[pid][i]);
		while(!bar_name->flag[pid][i]){
			pthread_cond_wait(&bar_name->cvs[pid][i], &bar_name->cv_locks[pid][i]);
			 asm("":::"memory");
		} 
		pthread_mutex_unlock(&bar_name->cv_locks[pid][i]);

		bar_name->flag[pid][i]= 0;
	}

	if( pid < (P-1)) {
		bar_name->flag[pid+mask][i] = 1;

		pthread_mutex_lock(&bar_name->cv_locks[pid+mask][i]);
		pthread_cond_broadcast(&bar_name->cvs[pid+mask][i]);
		pthread_mutex_unlock(&bar_name->cv_locks[pid+mask][i]);
		

		pthread_mutex_lock(&bar_name->cv_locks[pid][bar_name->MAX-1]);
		while(!bar_name->flag[pid][bar_name->MAX-1]) {
			 pthread_cond_wait(&bar_name->cvs[pid][bar_name->MAX-1], &bar_name->cv_locks[pid][bar_name->MAX-1]);
			 asm("":::"memory");

		}
		pthread_mutex_unlock(&bar_name->cv_locks[pid][bar_name->MAX-1]);
		bar_name->flag[pid][bar_name->MAX-1]=0;
	}
	for(mask>>=1;mask>0;mask>>=1)
	{
		bar_name->flag[pid-mask][bar_name->MAX-1] = 1;
		pthread_mutex_lock(&bar_name->cv_locks[pid-mask][bar_name->MAX-1]);
		pthread_cond_broadcast(&bar_name->cvs[pid-mask][bar_name->MAX-1]);
		pthread_mutex_unlock(&bar_name->cv_locks[pid-mask][bar_name->MAX-1]);
		
	}

}

/* struct for POSIX barrier interface (pthread_barrier_wait) */
struct bar_type5 {
	pthread_barrier_t barrier_from_p_thread;	
};

/* init for POSIX barrier interface (pthread_barrier_wait) */

void posix_barrier_interface_init(struct bar_type5* bar_name1,int num_threads)
{
	pthread_barrier_init(&bar_name1->barrier_from_p_thread,NULL,num_threads);
}

/* POSIX barrier interface (pthread_barrier_wait) */
void posix_barrier_interface(struct bar_type5* bar_name)
{
	pthread_barrier_wait(&bar_name->barrier_from_p_thread);	
}
