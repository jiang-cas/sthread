#include <stdio.h>
#include <sys/types.h>
#include "geap.h"
#include <stdlib.h>
#include <sys/wait.h>

#define N 1

int a[N] __attribute__ ((aligned(4096)));
int b[N] __attribute__ ((aligned(4096)));

void *func(void *arw)
{
	int i = (int)arw;
printf("clone res %d \n",geap_clone());
	a[i] = 200;
	geap_commit(a, &a[N]);
	exit(0);
}

int main() 
{
	int i;
	int ret;
	void *pstack;
	int stacksize = 4096;


//	geap_init();

	geap_set_flag();
	pstack = malloc(stacksize);

/*	for(i=0;i<N;i++) {
		a[i] = 100;
	}*/

geap_commit(a, &a[N]);

	for(i=0;i<N;i++) {
		ret = clone(func, (void *)(unsigned char *)pstack + stacksize, SIGCHLD | 0, (void *)i);

	}
	
	wait(NULL);

	geap_pull(a, &a[N]);
	for(i=0;i<N;i++) {
		printf("%d ", a[i]);
	}
//	print_geap_num();
//	printf("\n");
	return 0;
}
