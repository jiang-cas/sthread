#include "include/semaphore.h"
#include "include/heap.h"
#include <stdio.h>
#include "include/globals.h"

int new_sem(void)
{
	return semget((*(__semkey.val))++, 1, IPC_CREAT | 0666);
}

int init_sem(int sem_id, int init_value)
{
	union semun sem_union;
	sem_union.val = init_value;
	if(semctl(sem_id, 0, SETVAL, sem_union) == -1) {
		perror("Initialize semaphore");
		return -1;
	}
	return 0;
}

int del_Sem(int sem_id)
{
	union semun sem_union;
	if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1) {
		perror("Delete semaphore");
		return -1;
	}
	return 0;
}

int wait_sem(int sem_id)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1) {
		perror("P operation");
		return -1;
	}
	return 0;
}

int post_sem(int sem_id)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;

	if(semop(sem_id, &sem_b, 1) == -1) {
		perror("V ooperation");
		return -1;
	}
	return 0;
}

int post_multiple_sem(int sem_id, int n)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = n;
	sem_b.sem_flg = SEM_UNDO;

	if(semop(sem_id, &sem_b, 1) == -1) {
		perror("V ooperation");
		return -1;
	}
	return 0;
}
