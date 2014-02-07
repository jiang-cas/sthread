#include "../sthread.h"
#include <stdio.h>

#define N 10

int *a;
sthread_barrier_t barrier;

void *func(void *args)
{
	int i;
	sthread_t thread;
	thread = sthread_self();
	i = thread.tid;
	a[i] = i + 10;
	sthread_barrier_wait(&barrier);
	sthread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	sthread_t thread[N];
	sthread_barrier_init(&barrier, NULL, N+1);
	a = (int *)mvshared_malloc(sizeof(int) * N);

	for(i=0;i<N;i++)
		sthread_create(&thread[i], NULL, func, NULL);

	sthread_main_wait(N);

	sthread_barrier_wait(&barrier);

	for(i=0;i<N;i++)
		printf("%d ", a[i]);
	printf("\n");
	for(i=0;i<N;i++)
		sthread_join(thread[i], NULL);

	mvshared_free(a);
	sthread_return();
	return 0;
}
