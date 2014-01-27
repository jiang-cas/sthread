#include "../jthread.h"

int *a __attribute__((aligned(4096)));
int *b __attribute__((aligned(4096)));

void *func(void *args)
{
	int i;
	geap_clone();
	a = (int *)mymalloc(sizeof(int)*10);
	for(i=0;i<10;i++)
		a[i] = i;
	geap_commit(&a, &a+sizeof(void *));
	jthread_commit_heap();
	jthread_exit(NULL);
}

int main()
{
	struct jthread_t newthread;
	int ret;
	int i;
printf("11111111\n");
	geap_set_flag();

	b = (int *)mymalloc(sizeof(int) * 10);
	for(i=0;i<10;i++)
		b[i] = i+10;
	jthread_commit_heap();

	ret = jthread_create(&newthread, NULL, func, NULL);
printf("222222222\n");

	jthread_join(newthread, NULL);
printf("333333333\n");
	
	geap_pull(&a, &a+sizeof(void *));
printf("444444444\n");

	jthread_pull_heap();
printf("555555555\n");

printf("a %p \n", a);

	for(i=0;i<10;i++)
		printf("%d \t", a[i]);
	printf("\n");
	for(i=0;i<10;i++)
		printf("%d \t", b[i]);
	printf("\n");

	return 0;

}
