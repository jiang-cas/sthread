#include "../sthread.h"
#include <stdio.h>
#include <sys/mman.h>
//#include "geap.h"
#define N 2

#define LOG(x) printf("res %d \n", x)

int *a;
sthread_mutex_t mutex;
//int a[N] __attribute__ ((aligned(4096)));

void *func(void *args)
{
//	geap_clone();
	int i = (int)args;
	int j;
	//	a[i] = i;
//	printf("thread %d before assignment\n", i);
	sthread_mutex_lock(&mutex);

	printf("thread %d getlock\n", i);
	for(j=0;j<N;j++)
		a[j] = i + j;
	sthread_mutex_unlock(&mutex);
	printf("thread %d unlock\n", i);
//	printf("thread %d finish assignment\n", i);
//	geap_commit(a, a+sizeof(int)*N);
//	geap_push(a, a+sizeof(int)*N);
	sthread_exit(NULL);
//exit(0);
	return NULL;
}

int main()
{
	struct sthread_t newthread[N];
	int ret;
	int i;

	a = mvshared_malloc(sizeof(int)*N);


	sthread_mutex_init(&mutex, NULL);
//	a = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
//	geap_set_flag();
//	for(i=0;i<N;i++) 
//		a[i] = 5;

	for(i=0;i<N;i++)
		ret = sthread_create(&newthread[i], NULL, func, (void *)i);

	sthread_register(N);
	printf("main 1111111\n");
//	wait(NULL);	

	for(i=0;i<N;i++)
		sthread_join(newthread[i], NULL);

//	geap_pull(a, a+sizeof(int)*N);

	for(i=0;i<N;i++) {
		printf("%d \t ", a[i]);
	}
	printf("\n");

	mvshared_free(a);
	sthread_mutex_destroy(&mutex);

	return 0;
}
