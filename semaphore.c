#include "include/semaphore.h"
#include "include/heap.h"
#include <stdio.h>
#include "include/globals.h"

int new_sem(int total)
{
	return semget((*(__semkey.val))++, total, IPC_CREAT | 0666);
}

int init_sem(int sem_id, int num, int init_value)
{
	union semun sem_union;
	sem_union.val = init_value;
	if(semctl(sem_id, num, SETVAL, sem_union) == -1) {
		perror("Initialize semaphore");
		return -1;
	}
	return 0;
}
int read_sem(int sem_id, int num)
{
	union semun sem_union;
	return semctl(sem_id, num, GETVAL, sem_union);
}

int del_sem(int sem_id, int num)
{
	union semun sem_union;
	if(semctl(sem_id, num, IPC_RMID, sem_union) == -1) {
		perror("Delete semaphore");
		return -1;
	}
	return 0;
}

int wait_sem(int sem_id, int num)
{
	struct sembuf sem_b;
	sem_b.sem_num = num;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1) {
		perror("P operation");
		return -1;
	}
	return 0;
}

int post_sem(int sem_id, int num)
{
	struct sembuf sem_b;
	sem_b.sem_num = num;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;

	if(semop(sem_id, &sem_b, 1) == -1) {
		perror("V ooperation");
		return -1;
	}
	return 0;
}

void __acquire_lock(int lock)
{
	wait_sem(lock, 0);
}

void __release_lock(int lock)
{
	post_sem(lock, 0);
}
