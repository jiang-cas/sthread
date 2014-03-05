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

int new_sem(int total);
int init_sem(int sem_id, int num, int init_value);
int del_sem(int sem_id, int num);
int read_sem(int sem_id, int num);
int wait_sem(int sem_id, int num);
int post_sem(int sem_id, int num);
void __acquire_lock(int lock);
void __release_lock(int lock);

#endif
