#ifndef SEM_H
#define SEM_H

#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int new_sem(void);
int init_sem(int sem_id, int init_value);
int del_sem(int sem_id);
int read_sem(int sem_id);
int wait_sem(int sem_id);
int post_sem(int sem_id);
int post_multiple_sem(int sem_id, int n);

#endif
