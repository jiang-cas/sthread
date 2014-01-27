#include "include/semaphore.h"
#include "include/heap.h"
#include <stdio.h>

sem_t *new_sem(void)
{
	return (sem_t *)mvshared_malloc(sizeof(sem_t));
}

int init_sem(sem_t *sema, int init_value)
{
	if(sem_init(sema, 1, init_value) < 0) {
		perror("sem init");
		return -1;
	}
	return 0;
}

int del_sem(sem_t *sema)
{
	if(sem_destroy(sema) < 0) {
		perror("delete semaphore");
		return -1;
	}
	return 0;
}

int read_sem(sem_t *sema)
{
	int value;
	sem_getvalue(sema, &value);
	return value;
}

int p_sem(sem_t *sema)
{
	if(sem_wait(sema) < 0) {
		perror("P semaphore");
		return -1;
	}
	return 0;
}

int v_sem(sem_t *sema)
{
	if(sem_post(sema) < 0) {
		perror("V semaphore");
		return -1;
	}
	return 0;
}

