#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#define N 1000000


struct bar_type1 {
	int counter;
	pthread_mutex_t mutex;
	int flag;
};

pthread_mutex_t mutex_for_print ;

struct bar_type2 {
	int counter;
	pthread_mutex_t mutex;
	pthread_cond_t cv;
};

struct bar_type2 bar_name2;


struct bar_type1 bar_name1;
int num_threads;
int MAX;
pthread_barrier_t barrier_from_p_thread;

int **flag;
pthread_cond_t ** cvs;
pthread_mutex_t ** cv_locks;

// vector<vector<pthread_cond_t> > cvs(64,vector<pthread_cond_t>(6));
// vector<vector<pthread_mutex_t> > cv_locks(64,vector<pthread_mutex_t>(6));



/* a part */
void barrier_sense_reversal(struct bar_type1* bar_name, int P,int* local_sense,int id)
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

void* barrier_sense_reversal_caller(void *param)
{
	// int local_sense = 0;
	int  id = *(int*)(param);
	int local_sense = 0;
	for( int i=0;i<N;i++)
	{
		barrier_sense_reversal(&bar_name1,num_threads,&local_sense,id);
		if (id==0 && i%100==0)
		{
			printf("%d Ended \n",i);
		}
	}
}

/* b part */
void tree_barrier(int pid,int P){
	unsigned int i,mask;
	for(i=0,mask=1;(mask&pid)!=0;++i,mask<<=1)
	{
		while(!flag[pid][i]) {
			asm(""::
				:"memory");
		};
		flag[pid][i]= 0;
	}
	if( pid < (P-1)) {
		flag[pid+mask][i] = 1;
		while(!flag[pid][MAX-1]){
			asm(""::
				:"memory");
		}
		flag[pid][MAX-1]=0;
	}
	for(mask>>=1;mask>0;mask>>=1)
	{
		flag[pid-mask][MAX-1] = 1;
	}
}

void* tree_barrier_caller_part_b(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
		// pthread_mutex_lock(&mutex_for_print);
		// cout<<"BArrier Start for "<<i<<" in thread "<<id<<"\n";
		// pthread_mutex_unlock(&mutex_for_print);

		tree_barrier(id,num_threads);
		if (id==0 && i%100==0)
		{
			printf("%d Ended from tree_barrier_caller_part_b\n",i);
		}

		// if (id==0 && i%100==0)
		// {
		// 	cout<<"BArrier Ended for "<<i<<" in thread "<<id<<" tree_barrier_caller_part_b"<<"\n";
		// 	// cout<<
		// }
		// pthread_mutex_lock(&mutex_for_print);
		// pthread_mutex_unlock(&mutex_for_print);

	}	
}

/* part c */
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

		if (id==0 && i%100==0)
		{
			printf("%d Ended from centralised_barrier_using_posix_condition_variable_caller_part_c\n",i);
		}
		// if (id==0 && i%1==0)
		// {
		// 	cout<<"BArrier Ended for "<<i<<" in thread "<<id<<" centralised_barrier_using_posix_condition_variable_caller_part_c "<<"\n";
		// 	// cout<<
		// }
		// pthread_mutex_lock(&mutex_for_print);
		// pthread_mutex_unlock(&mutex_for_print);

	}	
}

/* part d */ 


void tree_barrier_using_posix_condition_variable(int pid,int P){
	unsigned int i,mask;
	for(i=0,mask=1;(mask&pid)!=0;++i,mask<<=1)
	{
		pthread_mutex_lock(&cv_locks[pid][i]);
		while(!flag[pid][i]){
			pthread_cond_wait(&cvs[pid][i], &cv_locks[pid][i]);
			 asm("":::"memory");
		} 
		pthread_mutex_unlock(&cv_locks[pid][i]);

		flag[pid][i]= 0;
	}
	// pthread_mutex_lock(&mutex_for_print);
	// 	cout<<"BArrier PART A for "<<pid<<" for i== "<<yo<<"\n";
	// 	pthread_mutex_unlock(&mutex_for_print);

	if( pid < (P-1)) {
		flag[pid+mask][i] = 1;

		pthread_mutex_lock(&cv_locks[pid+mask][i]);
		pthread_cond_broadcast(&cvs[pid+mask][i]);
		pthread_mutex_unlock(&cv_locks[pid+mask][i]);
		

		pthread_mutex_lock(&cv_locks[pid][MAX-1]);
		while(!flag[pid][MAX-1]) {
			 pthread_cond_wait(&cvs[pid][MAX-1], &cv_locks[pid][MAX-1]);
			 asm("":::"memory");

		}
		pthread_mutex_unlock(&cv_locks[pid][MAX-1]);
		flag[pid][MAX-1]=0;
	}
	
		// pthread_mutex_lock(&mutex_for_print);
		// cout<<"BArrier PART B for "<<pid<<" for i== "<<yo<<"\n";
		// pthread_mutex_unlock(&mutex_for_print);

	for(mask>>=1;mask>0;mask>>=1)
	{
		flag[pid-mask][MAX-1] = 1;
		pthread_mutex_lock(&cv_locks[pid-mask][MAX-1]);
		pthread_cond_broadcast(&cvs[pid-mask][MAX-1]);
		pthread_mutex_unlock(&cv_locks[pid-mask][MAX-1]);
		
	}
		// pthread_mutex_lock(&mutex_for_print);
		// cout<<"BArrier PART C for "<<pid<<" for i== "<<yo<<"\n";
		// pthread_mutex_unlock(&mutex_for_print);

}

/* d part */ 
void* tree_barrier_using_posix_condition_variable_caller_part_d(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
		// pthread_mutex_lock(&mutex_for_print);
		// cout<<"BArrier Start for "<<i<<" in thread "<<id<<"\n";
		// pthread_mutex_unlock(&mutex_for_print);

		tree_barrier_using_posix_condition_variable(id,num_threads);
		// centralised_barrier_using_posix_condition_variable(&bar_name2,num_threads);
		if (id==0 && i%100==0)
		{
			printf("%d Ended from tree_barrier_using_posix_condition_variable_caller_part_d\n",i);
		}
		
	}	
}

/* e part */

void posix_barrier_interface()
{
	pthread_barrier_wait(&barrier_from_p_thread);	
}

void* posix_barrier_interface_caller(void *param)
{
	int  id = *(int*)(param);
	for(int i=0;i<N;i++)
	{
		// pthread_mutex_lock(&mutex_for_print);
		// cout<<"BArrier Start for "<<i<<" in thread "<<id<<"\n";
		// pthread_mutex_unlock(&mutex_for_print);

		posix_barrier_interface();
		
		// if (id==0 && i%100==0)
		// {
		// 	// pthread_mutex_lock(&mutex_for_print);
		// 	cout<<"BArrier Ended for "<<i<<" in thread "<<id<<" posix_barrier_interface_caller"<<"\n";
		// 	// pthread_mutex_unlock(&mutex_for_print);
		// }
	}	
}



int main(int argc,char *argv[]){
	if(argc!=2)
	{
		printf("Need number of threads\n");
		// cout<<"Need number of threads\n";
		exit(1);
	}
	pthread_attr_t attr;
	num_threads = atoi(argv[1]);
	struct timeval tv0, tv1;
	struct timezone tz0, tz1;
	bar_name1.counter = 0;
	bar_name1.flag = 0;
	pthread_mutex_init(&bar_name1.mutex,NULL);
	bar_name2.counter = 0;
	pthread_mutex_init(&bar_name2.mutex,NULL);
	pthread_cond_init(&bar_name2.cv,NULL);
	pthread_mutex_init(&mutex_for_print,NULL);
	pthread_barrier_init(&barrier_from_p_thread,NULL,num_threads);

	flag = (int **)malloc(sizeof(int*)*num_threads);
	cvs = (pthread_cond_t **)malloc(sizeof(pthread_cond_t*)*num_threads);
	cv_locks = (pthread_mutex_t **)malloc(sizeof(pthread_mutex_t*)*num_threads);
	
    for (int i = 0; i<num_threads; i++) {
        flag[i] = (int *)malloc(sizeof(int)*num_threads);
        cvs[i] = (pthread_cond_t *)malloc(sizeof(pthread_cond_t)*num_threads);
        cv_locks[i] = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)*num_threads);
        
        for (int j = 0; j<num_threads; j++) {
            flag[i][j] = 0;
        	pthread_mutex_init(&cv_locks[i][j],NULL);
			pthread_cond_init(&cvs[i][j],NULL);
        }
    }
    MAX = 0;
	int num_threads2 = num_threads;
	while(num_threads2)
	{
		MAX+=1;
		num_threads2/=2;
	}

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